/******************************************************
* copyright 2011-2017 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "packagesPage.hpp"
#include <QtWidgets/QMessageBox>
#include <QFuture>
#include <QtConcurrent/QtConcurrentRun>

map<QString, QVector<QString> > pkgDeps;
QVector<QString> criticSubDeps;
QVector<meskPackage> DepsCheckedList;
QVector<meskPackage> selectedPackages;


meskPackage::meskPackage()
{
    //Name = "";
    //Repo = "ERROR";
    Name = "ERROR";
    Size = 0;
    Description = "";
    Critical = false;
    Remove = false;
    //dependenciesCounter = 0;
    dependsCounter = 0;

}

int meskPackage::initFilesList()
{
    // get package files
    QProcess fetchFiles;
    fetchFiles.start(QString("pacman -Ql %1 | awk '{print $2}'").arg(Name));

    return fetchFiles.exitCode();

}

packagesPage::packagesPage(QWidget *parent) :
    pageBase(parent)
{
    Ui_packagesPage::setupUi(this);

    pageName = tr("Packages Page");
    pageHelpMessage = tr("");

    packagesListDirPath = "/install/packages/";

    solvingCheckedDeps = false;


    connect(resetPushButton, SIGNAL(clicked()), this, SLOT(resetLists()));
    connect(packagesListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(updatePackageDescription(int)));
    connect(packagesListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(updatePackageDeps(QListWidgetItem*)), Qt::DirectConnection);
    connect(reposListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(updatePackageList(int)));
    connect(reposListWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(updatePackageList(QListWidgetItem*)));

    connect(selectNonePushButton, SIGNAL(clicked()), this, SLOT(unsetAllSelection()));
    connect(selectAllPushButton, SIGNAL(clicked()), this, SLOT(setAllSelection()));

}

int packagesPage::unsetAllSelection()
{
    for ( int i =0; i < reposListWidget->count(); i++)
    {
        reposListWidget->item(i)->setCheckState(Qt::Unchecked);
    }
    return 0;
}

int packagesPage::setAllSelection()
{
    for ( int i =0; i < reposListWidget->count(); i++)
    {
        reposListWidget->item(i)->setCheckState(Qt::Checked);
    }
    return 0;
}

const meskPackage packagesPage::getMeskPackage(QString name)
{
    for ( int i =0; i < packages.count(); i++)
    {
        for ( int k =0; k < packages.at(i).second.count(); k++)
        {
            if ( (packages.at(i)).second.at(k).Name == name)
            {
                return (packages.at(i)).second.at(k);
            }
        }
    }
    return meskPackage();
}

/*int packagesPage::updatePackageDeps(QListWidgetItem* package)
{
    // retrieve the meskPakcage first and pass it to the appropriate handler
    meskPackage mpackage = getMeskPackage(package->text());

    if (package->checkState()==Qt::Checked)
    {
        DepsCheckedList.push_back(mpackage);
        QFuture<void> future = QtConcurrent::run(this, &packagesPage::solveDepsCheckedList);
        //solveDepsChecked(mpackage);
    }
    else
    {
        QFuture<void> future = QtConcurrent::run(this, &packagesPage::solveDepsUnChecked, mpackage);
        //solveDepsUnChecked(mpackage);
    }
    return 0;
}
*/

int packagesPage::updatePackageDeps(QListWidgetItem* package)
{
    // retrieve the meskPakcage first and pass it to the appropriate handler
    meskPackage mpackage = getMeskPackage(package->text());

    if (package->checkState()==Qt::Checked)
    {
        ////DepsCheckedList.push_back(mpackage);
        //QFuture<void> future = QtConcurrent::run(this, &packagesPage::solveDepsCheckedList);
        //solveDepsChecked(mpackage);
        /////solveDepsCheckedList();
        solveDepsChecked(mpackage);
    }
    else
    {
        //QFuture<void> future = QtConcurrent::run(this, &packagesPage::solveDepsUnChecked, mpackage);
        solveDepsUnChecked(mpackage);
    }
    return 0;
}

int packagesPage::checkDepPackage(QString name)
{
    for ( int i =0; i < packagesList.count(); i++)
    {
        for ( int k =0; k < packagesList.at(i).second.count(); k++)
        {
            if ( (packagesList.at(i)).second.at(k)->text() == name)
            {
				if ((packagesList.at(i)).second.at(k)->checkState() == Qt::Checked)
				{
					return 0;
				}
				
                (packagesList.at(i)).second.at(k)->setCheckState(Qt::Checked);
                (packagesList.at(i)).second.at(k)->setBackgroundColor(QColor(Qt::darkGray));
                (packagesList.at(i)).second.at(k)->setFlags((Qt::ItemFlag)((int)(~1)*Qt::ItemIsUserCheckable)|Qt::ItemIsEnabled);
                                  
                //cerr << "checking package : " << name.toStdString() << endl << flush;
                return 0;
            }
        }
    }
    return 1;
}

int packagesPage::uncheckDepPackage(QString name)
{
    for ( int i =0; i < packagesList.count(); i++)
    {
        for ( int k =0; k < packagesList.at(i).second.count(); k++)
        {
            if ( (packagesList.at(i)).second.at(k)->text() == name)
            {
				if ((packagesList.at(i)).second.at(k)->checkState() == Qt::Unchecked)
				{
					return 0;
				}
				
                (packagesList.at(i)).second.at(k)->setCheckState(Qt::Unchecked);
                (packagesList.at(i)).second.at(k)->setBackgroundColor(QColor(Qt::white));
                (packagesList.at(i)).second.at(k)->setForeground(Qt::black);
                (packagesList.at(i)).second.at(k)->setFlags((Qt::ItemFlag)((int)Qt::ItemIsUserCheckable)|Qt::ItemIsEnabled);
                return 0;
            }
        }
    }
    return 1;
}


/*int packagesPage::solveDepsCheckedList()
{
    if (solvingCheckedDeps)
    {
        return 1;
    }
    
    solvingCheckedDeps=true;
    reposListWidget->setEnabled(false);
    packagesListWidget->setEnabled(false);
    while( DepsCheckedList.count() != 0)
    {
        solveDepsChecked(DepsCheckedList.at(0));
        DepsCheckedList.erase(DepsCheckedList.begin());
    }
    reposListWidget->setEnabled(true);
    packagesListWidget->setEnabled(true);
    solvingCheckedDeps=false;
    return 0;
}*/


int packagesPage::solveDepsChecked(const meskPackage package)
{
	checkPackages.clear();
	solveDepsCheckedGenerateList(package);
	return 0;
}

meskPackage* packagesPage::findPackage(QString package)
{
    for ( int i =0; i < packages.count(); i++) // loop for each repo
    {
        for ( int k =0; k < packages.at(i).second.count(); k++) // loop for each package in the repo
        {
            if ( (packages.at(i)).second.at(k).Name == package) // is this package the critialSubDep ??
            {
				return &packages[i].second[k];
			}
		}
	}
	
	return NULL;
}

int packagesPage::solveDepsCheckedGenerateList(const meskPackage package)
{
	// populate full list and loop and see if a package is required by other checked packages that are not in the list ?
	//cerr << "checking package : " << package.Name.toStdString() << endl << flush;
	checkDepPackage(package.Name);
	checkPackages.push_back(package.Name);
		
	// check all depends ons and all their depend ons ..
    for ( int i =0; i < package.Depends.size(); i++) 
    {
        meskPackage* dep = findPackage(package.Depends.at(i));

        if(dep)
        {
			if(dep->currentDependencies.indexOf(package.Name) == -1)
			{
				dep->currentDependencies.push_back(package.Name);
			}

			if (checkPackages.indexOf(dep->Name) == -1)
			{
				solveDepsCheckedGenerateList(*dep);
			}
		}
    }

    return 0;
}

int packagesPage::solveDepsUnChecked(const meskPackage package)
{
	unCheckPackages.clear();
	solveDepsUnCheckedGenerateList(package);
	return 0;
}

int packagesPage::solveDepsUnCheckedGenerateList(const meskPackage package)
{
 	unCheckPackages.push_back(package.Name);
 	uncheckDepPackage(package.Name);
	//cerr << "2" << endl << flush;
	// try to uncheck all the required by packages
	for ( int i =0; i < package.Depends.size(); i++) 
    {
        meskPackage* dep = findPackage(package.Depends.at(i));
        if(dep)
        {
			dep->currentDependencies.removeAll(package.Name);
						
			if (unCheckPackages.indexOf(dep->Name) == -1 &&
				dep->currentDependencies.size() == 0)
			{
				solveDepsUnCheckedGenerateList(*dep);
			}
		}
    }
    
    // remove all parent packages , useful if we enable to edit dependy package in the future
   	for ( int i =0; i < package.Dependencies.size(); i++) 
    {
        meskPackage* dep = findPackage(package.Dependencies.at(i));
        if(dep)
        {
			dep->currentDependencies.removeAll(package.Name);
						
			if (unCheckPackages.indexOf(dep->Name) == -1 &&
				dep->currentDependencies.size() == 0)
			{
				solveDepsUnCheckedGenerateList(*dep);
			}
		}
    }
    return 0;
    
    /*
	meskPackage* pkg = findPackage(package.Depends.at(i));
	if(!pkg)
	{
		return 1;
		//dep->currentDependencies.push_back(package.Name);
	}
		

	for ( int i =0; i < dep->currentDependencies.size(); i++)
	{
		meskPackage mpackage = getMeskPackage(package.Dependencies.at(i));
		if(mpackage.Critical || solveDepsUnChecked(mpackage))
		{
			setStatus(tr("Couldn't un-select ") + package.Name + tr(" which is required by ") + mpackage.Name, ERROR);
			continue;
		}
		uncheckDepPackage(package.Name);
	}
	
	return 0;
	
	for ( int i =0; i < package.Dependencies.size(); i++)
	{
		// if package is selected, then give an error and say that
		// this package is required by this dependency
		// otherwise continue and try to uncheck this pakcage and all it's packages...
		meskPackage mpackage = getMeskPackage(package.Dependencies.at(i));
		if(mpackage.Critical || solveDepsUnChecked(mpackage))
		{
			setStatus(tr("Couldn't un-select ") + package.Name + tr(" which is required by ") + mpackage.Name, ERROR);
			continue;
		}
		
	}
	
	uncheckDepPackage(package.Name);
	return 0;*/
	
	/*
    for ( int i =0; i < package.Depends.size(); i++)
    {
        solveDepsUnChecked(getMeskPackage(package.Depends.at(i)));
        for ( int k =0; k < pkgDeps[package.Depends.at(i)].size(); k++)
        {
            if (pkgDeps[package.Depends.at(i)].at(k) == package.Name)
            {
                pkgDeps[package.Depends.at(i)].erase(pkgDeps[package.Depends.at(i)].begin()+k);
                if (pkgDeps[package.Depends.at(i)].size() == 0)
                {
                    uncheckDepPackage(package.Depends.at(i));
                }
                break;
            }
        }
    }
    return 0;
    */
}


int packagesPage::fixCriticals()
{
    for (int l=0; l < criticSubDeps.count() ; l++) // add deps of all criticalSubDeps to criticSubDeps too !
    {
        fixCriticalsDeps(criticSubDeps.at(l));
    }

    for (int l=0; l<criticSubDeps.size(); l++) // mark all criticSubDeps as critical
    {
        for ( int i =0; i < packages.count(); i++)
        {
            for ( int k =0; k < packages.at(i).second.count(); k++)
            {
                if ( packages.at(i).second.at(k).Name == criticSubDeps.at(l))
                {
                    packages[i].second[k].Critical = true;
                }
            }
        }
    }

    return 0;
}

int packagesPage::fixCriticalsDeps(QString package, QString caller) // added caller to detect cycle depecency 
{
	
    for ( int i =0; i < packages.count(); i++) // loop for each repo
    {
        for ( int k =0; k < packages.at(i).second.count(); k++) // loop for each package in the repo
        {
            if ( (packages.at(i)).second.at(k).Name == package) // is this package the critialSubDep ??
            {
                //for ( int l=0; l < (packages[i]).second[k].Depends.count(); l++)
                for ( int l=0; l < (packages.at(i)).second.at(k).Depends.count(); l++) // loop for all depdencies of this critialSubDep
                {
                    //if ( criticSubDeps.indexOf((packages[i]).second[k].Depends.at(l)) == -1)
                    if ( criticSubDeps.indexOf((packages.at(i)).second.at(k).Depends.at(l)) == -1)
                    {
                        //fixCriticalsDeps((packages[i]).second[k].Depends.at(l));
                        if ((packages.at(i)).second.at(k).Depends.at(l) == caller)
						{
							//setStatus(tr("Detected depency cycle between ") + caller + tr(" and ") + (packages.at(i)).second.at(k).Depends.at(l) , WARNING);
							setStatus(tr("Detected depency cycle with package ") + caller , WARNING);
							continue;
						}
                        fixCriticalsDeps((packages.at(i)).second.at(k).Depends.at(l), package);
                    }
                }
                
                if ( criticSubDeps.indexOf(package) == -1)
                {
                    criticSubDeps.push_back(package);

                }
                return 0;
            }
        }
    }
    return 1;
}

int packagesPage::initPackages()
{
    QDir packagesDir;

    if (!packagesDir.cd(packagesListDirPath))
    {
		setStatus(tr("Ooops , Couldn't find the packages info directory : ")+packagesListDirPath, ERROR);
        return 1;
    }
    
    cerr << packagesListDirPath.toStdString() << endl  << flush;

    QString repoName("");
    QVector<meskPackage> repoPackages;
    //packages.clear();

    for ( unsigned int repo =0; repo < packagesDir.count(); repo++)
    {
        repoName = packagesDir.entryList().at(repo);

        if (repoName == "." || repoName == "..")
        {
            continue;
        }

        packagesDir.cd(repoName);

        for ( unsigned int package = 0; package < packagesDir.count(); package++)
        {
            if (packagesDir.entryList().at(package) == "." ||
                packagesDir.entryList().at(package) == "..")
            {
                continue;
            }

            meskPackage mpackage = parsePackageFile(packagesDir.absoluteFilePath(packagesDir.entryList().at(package)));
            repoPackages.push_back(mpackage);
        }

        packages.push_back(qMakePair(repoName, repoPackages));

        repoName = "";
        repoPackages.clear();

        packagesDir.cdUp();
    }

    fixCriticals();

    return 0;
}


int packagesPage::initPackagesLists()
{
    for (int i =0; i < packages.size(); i++)
    {
        //packagesList.clear();
        QListWidgetItem* repo = new QListWidgetItem(packages.at(i).first);

        QVector<QListWidgetItem*> listPackages;

        repo->setData(Qt::CheckStateRole, Qt::Unchecked);

		Qt::ItemFlags itemFlags;
        for (int p=0; p < packages.at(i).second.size(); p++)
        {
			//cout << packages.at(i).second.at(p).Name.toUtf8().constData() << endl;
            QListWidgetItem* packagesListItem = new QListWidgetItem(packages.at(i).second.at(p).Name);
            //package->setFlags((Qt::ItemFlag)((int)(~(packages.at(i).second.at(p).Critical))*Qt::ItemIsUserCheckable)|
            //                  Qt::ItemIsEnabled|);
            if(packages.at(i).second.at(p).Critical)
            {
				//itemFlags = 0;
				itemFlags = Qt::ItemIsEnabled;
				//DpackagesListItem->setFlags((Qt::ItemFlag)(~Qt::ItemIsUserCheckable|~Qt::ItemIsEditable));
                if(packages.at(i).second.at(p).Remove)
                {
                    //packagesListItem->setData(Qt::CheckStateRole, Qt::Unchecked);
                    packagesListItem->setCheckState(Qt::Unchecked);
                    packagesListItem->setBackgroundColor(QColor(Qt::darkRed));
                }
                else
                {
                    //packagesListItem->setData(Qt::CheckStateRole, Qt::Checked);
                    packagesListItem->setCheckState(Qt::Checked);
                    packagesListItem->setBackgroundColor(QColor(Qt::darkBlue));
                }
                packagesListItem->setForeground(QBrush(QColor(Qt::white)));

            }
            else
            {
				itemFlags = Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEnabled;
                //packagesListItem->setData(Qt::CheckStateRole, Qt::Unchecked);
                packagesListItem->setCheckState(Qt::Unchecked);
            }
            
            packagesListItem->setFlags(itemFlags);
            listPackages.push_back(packagesListItem);
        }
        packagesList.push_back(qMakePair(repo, listPackages));
    }
    return 0;
}

int packagesPage::initListViews()
{
    reposListWidget->blockSignals(true);
    for ( int i =0; i < packagesList.size(); i++)
    {
        reposListWidget->addItem(packagesList[i].first);
    }
    reposListWidget->blockSignals(false);

    reposListWidget->setCurrentRow(0);
    return 0;
}

meskPackage packagesPage::parsePackageFile(const QString& packageFilePath)
{
    meskPackage package;

    QFile packageFile;
    QString buffer;
    packageFile.setFileName(packageFilePath);

    package.packageFilePath = packageFilePath;

    if (!packageFile.open(QIODevice::ReadOnly))
    {
		setStatus(tr("Ooops , Couldn't open package : ")+package.packageFilePath, ERROR);
        return package;
    }

    package.Name = packageFilePath.mid( packageFilePath.lastIndexOf('/')+1);

    for ( int i =0; i < 6; i++)
    {
        buffer = (packageFile.readLine()).data();

        if (buffer.size() == 0)
        {
            break;
        }

        if ( i == 0)
        {
            package.Size = buffer.mid(0,buffer.indexOf(' ')).toLongLong();

            if ( package.Size > 1024.00 )
            {
                package.strSize = QString::number(package.Size)+tr(" mb");
            }
            else
            {
                package.strSize = buffer;
            }
        }
        else if (i == 1)
        {
            if ( buffer.toInt() == 2)
            {
                package.Remove=true;
            }
            package.Critical = (bool)buffer.toInt();
        }
        else if (i == 2)
        {
            while(buffer.mid(0,3) != "EOD" && !packageFile.atEnd())
            {
               package.Description += buffer;
               buffer.clear();
               buffer = (packageFile.readLine()).data();
            }
        }
        else if (i == 3)
        {
            while(buffer.mid(0,3) != "EOR" && !packageFile.atEnd())
            {
				buffer = buffer.simplified().trimmed();
                for (int k=0; k < buffer.split(' ',QString::SkipEmptyParts).size(); k++)
                {
					QString dep = buffer.split(' ',QString::SkipEmptyParts).at(k).simplified();
					//cerr << "OOOOOOOOOOPS : "<< dep.remove(QRegExp("[\\n\\t\\r]")).toStdString() << " : " << package.Name.toStdString() << endl << flush;
					
					if (dep.toStdString() == " ")
					{
						continue;
					}
					//cerr << "OOOOOOOOOOPS : "<< dep.toStdString() << " : " << package.Name.toStdString() << endl << flush;
                    package.Depends.push_back(dep);
                    if (package.Critical != 0)
                    {
                        pkgDeps[dep].push_back(package.Name);

                        if (package.Critical == 1)
                        {
                            if ( criticSubDeps.indexOf(dep) == -1)
                            {
                                criticSubDeps.push_back(dep);
                            }
                        }
                    }
                }
                buffer.clear();
                buffer = (packageFile.readLine()).data();
            }
        }
        else if (i == 4)
        {
            while(buffer.mid(0,3) != "EOK" && !packageFile.atEnd())
            {
				buffer = buffer.simplified().trimmed();
                for (int k=0; k < buffer.split(' ',QString::SkipEmptyParts).size(); k++)
                {
					QString dep = buffer.split(' ',QString::SkipEmptyParts).at(k).simplified();
					if (dep.toStdString() == " ")
					{
						continue;
					}
                    package.Dependencies.push_back(dep);
                    if (package.Critical != 0)
                    {
                        pkgDeps[dep].push_back(package.Name);

                        if (package.Critical == 1)
                        {
                            if ( criticSubDeps.indexOf(dep) == -1)
                            {
                                criticSubDeps.push_back(dep);
                            }
                        }
                    }
                }
                buffer.clear();
                buffer = (packageFile.readLine()).data();
            }
        }
        else
        {
            while(buffer.size() != 0)
            {
                package.packageFiles.push_back(buffer.trimmed());
                buffer.clear();
                buffer = (packageFile.readLine()).data();
            }
        }
        buffer.clear();
    }

    return package;
}

long long packagesPage::calculateTotalSize()
{

    return totalSize;
}

int packagesPage::resetLists()
{
    packagesListWidget->blockSignals(true);
    reposListWidget->blockSignals(true);

    while(packagesListWidget->count() > 0)
    {
        packagesListWidget->takeItem(0);
    }

    while(reposListWidget->count() > 0)
    {
        reposListWidget->takeItem(0);
    }
   
   emit clearMessages();

    packagesListWidget->blockSignals(false);
    reposListWidget->blockSignals(false);

    packages.clear();
    packagesList.clear();
    statuses.clear();
    initAll();
    return 0;
}

int packagesPage::initAll()
{
    QFuture<void> future = QtConcurrent::run(this, &packagesPage::initPackageLists);
    return 0;
}


int packagesPage::initPackageLists()
{
	setStatus(tr("Generating packages info"), BUSY);
    if (!initPackages())
	{
		setStatus(tr("Generating packages lists"), BUSY);
		initPackagesLists();
		setStatus(tr("Generating packages preview"), BUSY);
		initListViews();
		init = true;
		emit Ready();
		setDone(true);
		return 0;
	}
	return 1;
}
/*int packagesPage::updatePackageDescription(QListWidgetItem* newPackage,QListWidgetItem* oldPackage)
{
    if (newPackage == NULL)
    {
        return 1;
    }

    return 0;
}*/


int packagesPage::updatePackageDescription(int row)
{
    if (row < 0)
    {
		setStatus(tr("Ooops , Wrong package row : ")+QString(row), ERROR);
        return 1;
    }

    QString desc =	tr("Description : ")+
					packages.at(reposListWidget->currentRow()).second.at(row).Description//+"\n"
					+tr("Size : ")+
					packages.at(reposListWidget->currentRow()).second.at(row).strSize//+"\n"
					+tr("Depends on : ")
					+QString::number(packages.at(reposListWidget->currentRow()).second.at(row).Depends.size())
					+" "+tr("Packages");
					
	if (packages.at(reposListWidget->currentRow()).second.at(row).currentDependencies.size())
	{
		QVector<QString> pkgs = packages.at(reposListWidget->currentRow()).second.at(row).currentDependencies;
		desc += "\n" + tr("Required By : ");
		for (int i = 0; i < pkgs.size(); i++)
		{
			desc += pkgs.at(i) + " ";
		}
	}
	packageDescriptionTextEdit->setText(desc);
    return 0;
}

/*int packagesPage::updatePackageList(QListWidgetItem* newRepo,QListWidgetItem* oldRepo)
{
    if (!newRepo || !oldRepo)
    {
        return 1;
    }

    for ( int i =0; i < packagesList.size(); i++)
    {
        if (packagesList.at(i).first->text() == oldRepo->text())
        {
            for ( int j =0; j < packagesList.at(i).second.size(); j++)
            {
                packagesListWidget->removeItemWidget(packagesList[i].second[j]);
            }
        }
    }


    for ( int i =0; i < packagesList.size(); i++)
    {
        if (packagesList.at(i).first->text() == newRepo->text())
        {
            for ( int j =0; j < packagesList.at(i).second.size(); j++)
            {
               packagesListWidget->addItem(packagesList[i].second[j]);
            }
        }
    }


    return 0;
}*/


int packagesPage::updatePackageList(QListWidgetItem* item)
{
    packageDescriptionTextEdit->clear();
    if (item->checkState() == Qt::Checked)
    {
        for ( int j =0; j < packagesList.at(item->listWidget()->row(item)).second.size(); j++)
        //for ( int j =0; j < (packagesList.at(item)).second.size(); j++)
        {
            if (packagesList.at(item->listWidget()->row(item)).second.at(j)->flags()&Qt::ItemIsUserCheckable)
            {
                packagesList.at(item->listWidget()->row(item)).second.at(j)->setCheckState(Qt::Checked);
            }

        }
    }
    else if (item->checkState() == Qt::Unchecked)
    {
        for ( int j =0; j < packagesList.at(item->listWidget()->row(item)).second.size(); j++)
        //for ( int j =0; j < (packagesList.at(item)).second.size(); j++)
        {
            ////packagesList.at(item).second.at(j)->setCheckState(Qt::Checked);
            //package->setFlags((Qt::ItemFlag)((int)(~(packages.at(i).second.at(p).Critical))*Qt::ItemIsUserCheckable)|
            //                  Qt::ItemIsEnabled);
            if (packagesList.at(item->listWidget()->row(item)).second.at(j)->flags()&Qt::ItemIsUserCheckable)
            {
                packagesList.at(item->listWidget()->row(item)).second.at(j)->setCheckState(Qt::Unchecked);
            }

            //packagesListWidget->addItem(packagesList[row].second[j]);
        }
    }
    
    return 0;
}

int packagesPage::updatePackageList(int row)
{
    if (solvingCheckedDeps)
    {
        return 1;
    }
    packageDescriptionTextEdit->clear();

    packagesListWidget->blockSignals(true);

    while(packagesListWidget->count() > 0)
    {
        packagesListWidget->takeItem(0);
    }


   // for ( int i =0; i < packagesList.size(); i++)
    //{
      //  if (packagesList.at(i).first->text() == reposListWidget->currentItem()->text())
        //{
            for ( int j =0; j < packagesList.at(row).second.size(); j++)
            {
                packagesListWidget->addItem(packagesList[row].second[j]);
            }
        //}
    //}

    packagesListWidget->blockSignals(false);
    packagesListWidget->setCurrentRow(0);


    return 0;
}


int packagesPage::addSelectedPackage(meskPackage package)
{
    if (!package.Name.size())
    {
        return 1;
    }

    totalSize += package.Size;
    bool brokenDep = false;


    for (int deppkg =0; deppkg < selectedPackages.count() ; deppkg++)
    {
        if (selectedPackages.at(deppkg).Name == package.Name)
        {
            brokenDep = true;
            break;
        }
    }

    if (! brokenDep )
    {
         selectedPackages.push_back(package);
    }
    else
    {
        return 1;
    }


    for (int l=0; l< package.Depends.count(); l++)
    {
        meskPackage dep = getMeskPackage(package.Depends.at(l));
        //if (dep.Name.size() > 0 )
        //{
            /*brokenDep = false;
            for (int deppkg =0; deppkg < selectedPackages.count() ; deppkg++)
            {
                if (selectedPackages.at(deppkg).Name == dep.Name)
                {
                    brokenDep = true;
                    break;
                }
            }

            if (! brokenDep )
            {
                addSelectedPackage(dep);
            }*/

             addSelectedPackage(dep);

        //}
    }

    return 0;
}

QVector<meskPackage> packagesPage::getSelectedPackages()
{
    selectedPackages.clear();
    totalSize = 0;

    for  (int i =0; i < packagesList.size(); i++)
    {
        for (int j =0; j < packagesList.at(i).second.size(); j++)
        {
            if(packagesList.at(i).second.at(j)->checkState() == Qt::Checked)
            {
                addSelectedPackage(packages.at(i).second.at(j));
            }
        }
    }
    return selectedPackages;
}

void packagesPage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
        Ui_packagesPage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}

