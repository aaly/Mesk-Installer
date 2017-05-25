/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "installationPage.hpp"

installationPage::installationPage(QWidget *parent) :
pageBase(parent)
{
    Ui_installationPage::setupUi(this);

	slidesDir = getApplicationFile("/slides/"+lang);

	if(QFile::exists(getApplicationFile("/slides/"+lang)))
	{
		slidesDir = getApplicationFile("/slides/"+lang);
	}
	else
	{
		slidesDir =  getApplicationFile("/slides/en");
	}

    installationRoot = "/mnt/root/";
    Root = new chroot(this);
    Root->setRoot(installationRoot);


    installationProgressBar->setValue(0);
    installationProgressBar->setMaximum(100);

    connect(this, SIGNAL(installationStatus(QString)), this, SLOT(setAction(QString)), Qt::QueuedConnection);
    slideStep=0;
}

installationPage::~installationPage()
{
}


int installationPage::initAll()
{

    pageBase::initAll();
	setStatus(tr("Loading slides"), BUSY);
    initSlideShow();
    emit Ready();
    preparePackagesList();
    /*if (install() == 0)
    {
        emit Done(true);
        timer->stop();
        return 0;
    }*/

	cerr << "done init all " << endl << flush;
    return 0;
}

int installationPage::initSlideShow()
{

    QDir dir(slidesDir);

    foreach (const QString &fileName, dir.entryList(QDir::Files))
    {
        QString absolutePath = dir.absoluteFilePath(fileName);
        slides.push_back(QPixmap(absolutePath));
    }

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slide()));
    timer->start(5000);
    displayLabel->setPixmap(slides.at(slideStep));

    return 0;
}

int installationPage::slide()
{
    slideStep++;
    if (slideStep > slides.count()-1)
    {
        slideStep=0;
    }
    displayLabel->setPixmap(slides.at(slideStep));
    return slideStep;
}

int installationPage::install()
{
	cerr << "begin install " << endl << flush;
	setStatus(tr("Installing packages ..."), BUSY);
    // mount the root partition
    diskPage* dpage = (diskPage*)getDependency("Disk");

    /*QProcess mountRoot;

    mountRoot.execute("mkdir -p " + installationRoot);
    mountRoot.waitForFinished();
    if(mountRoot.readAllStandardError().size() > 0)
    {
        setStatus("Couldn't create installation root : "+installationRoot, ERROR);
        return 1;
    }

    // TODO: only if it is already mounted ???
    mountRoot.start("umount " + dpage->rootPartition);
    mountRoot.waitForFinished(20000);

    //if (mountRoot.readAllStandardError().size() > 0)
    //{
    //    setStatus("Couldn't un mount the installation partition : " + dpage->rootPartition , ERROR);
    //}

    mountRoot.start("mount " + dpage->rootPartition + " " + installationRoot);
    mountRoot.waitForFinished(10000);

    if (mountRoot.readAllStandardError().size() > 0)
    {
        setStatus("Couldn't mount the installation partition : " + dpage->rootPartition , ERROR);
        return 1;
    }*/


    connect(&futureWatcher2, SIGNAL(finished()), this, SLOT(finishInstallation()));
    future2 = QtConcurrent::run(this, &installationPage::installSystem);
    futureWatcher2.setFuture(future2);

    return 0;
}

int installationPage::installSystem()
{

    //////////installPackages();
    //installationProgressBar->setValue(80);
    QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
                               Q_ARG( int, 80 ) );

    Root->prepare();
    generateKernel();
    //installationProgressBar->setValue(90);
    QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
                               Q_ARG( int, 90 ) );
    generateLocales();
    //installationProgressBar->setValue(100);
    QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
							   Q_ARG( int, 95 ) );

	generateMachineID();
	//installationProgressBar->setValue(100);
	QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
							   Q_ARG( int, 96 ) );

	setTimeZone();
	//installationProgressBar->setValue(100);
	QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
							   Q_ARG( int, 97 ) );


	copyPostPackages();
	//installationProgressBar->setValue(100);
	QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
							   Q_ARG( int, 100 ) );


    Root->unprepare();

    /*QProcess mountRoot;
    mountRoot.start("umount " + installationRoot);
    mountRoot.waitForFinished(10000);*/

    return 0;
}

int installationPage::finishInstallation()
{
    if (future2.result() == 1)
    {
        setStatus(tr("Error installing the system"), ERROR);
        return 1;
    }

    /*QProcess mountRoot;
    diskPage* dpage = (diskPage*)getDependency("Disk");

    mountRoot.start("umount " + dpage->rootPartition);
    mountRoot.waitForFinished(10000);

    if (mountRoot.readAllStandardError().size() > 0)
    {
        setStatus("Couldn't un mount the installation partition : " + dpage->rootPartition, ERROR);
        //return 1;
    }*/

    emit installationStatus(tr("Done installing"));

	setDone(true);
    timer->stop();

    return 0;
}

int installationPage::preparePackagesList()
{
    setStatus(tr("Preparing packages and dependencies"), BUSY);
    packagesPage* ppage = (packagesPage*)getDependency("Packages");

    //futureWatcher = QFutureWatcher< QVector<meskPackage> >();


    connect(&futureWatcher, SIGNAL(finished()), this, SLOT(install()));

    future = QtConcurrent::run(ppage, &packagesPage::getSelectedPackages);
    futureWatcher.setFuture(future);

    return 0;
}

int installationPage::installPackages()
{
    QVector<meskPackage>    packages = future.result();
    QVector<QString>    packagesScripts;

    cout << "pakcages size : " << packages.size() << endl;
    //QEventLoop* eventLoop = new QEventLoop(this->parent());

    for (int i=0; i < packages.count(); i++)
    {
        ////cout << "Installing: " << packages.at(i).Name.toStdString() << endl;

        emit installationStatus(tr("installing package: ")+packages.at(i).Name+"\n"+tr("size: ")+packages.at(i).strSize +
                  "\n (" + QString::number(i+1) + "/" + QString::number(packages.size()) + ")");
        //cout << packages.at(i).Name.toStdString() << " " << i << " " << packages.count() << flush;
        for ( int k=0, count=packages.at(i).packageFiles.count(); k < count; k++)
        {
            QString file = packages.at(i).packageFiles.at(k).trimmed();
            //QFileInfo finfo(file);
            if (file.at(file.size()-1) == '/' )
            //if(finfo.isDir())
            {
                QString filepath = installationRoot+file;
                //cout << "DIR : " << file.toStdString() << flush << endl;
                if (QFile::exists(filepath) == false)
                {
                    filesCopier.execute("mkdir " + filepath );
                }
                //TODO: place them outside so that we make sure it is a recover if this is over installation
                filesCopier.execute("chmod --reference=" + file + " " + filepath );
                filesCopier.execute("chown --reference=" + file + " " + filepath );
                filesCopier.waitForFinished();
            }
            else
            {
                //cout << "FILE : " <<file.toStdString() << "::" <<  (installationRoot + file.mid(0, file.lastIndexOf('/'))).toStdString() << flush << endl;
                filesCopier.execute("cp -af " + file + " " + installationRoot + file.mid(0, file.lastIndexOf('/')) );
                filesCopier.waitForFinished();

                if(filesCopier.readAllStandardError().size() > 0)
                {
                    setStatus(tr("Couldn't copy file : ") + file + tr(" to : ") + installationRoot, ERROR);
                    return 1;
                }
            }
        }

        //installationProgressBar->setValue((i*80)/packages.at(i).packageFiles.count());
        QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
                                   Q_ARG( int, (i+1)*80/packages.count() ) );
        //connect(this, SIGNAL(signal()), _receiver, slot);
        //emit signal();
        //disconnect(this, SIGNAL(signal()), _receiver, slot);

        //eventLoop->processEvents();
    }



    emit installationStatus(tr("updating original packages files"));
    QFile overlayFiles;
    overlayFiles.setFileName("/install/overlayfiles");
    if (!overlayFiles.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        setStatus(tr("Couldn't open overlay file : ") + "/install/overlayfiles", ERROR);
    }
    else
    {
        QString ovfile;
        while (!overlayFiles.atEnd())
        {
            // if not a dir and then exists, then overwrite
            //eventLoop->processEvents();
            ovfile=overlayFiles.readLine();
            //QString file = packages.at(i).packageFiles.at(k);
            ovfile = ovfile.mid(0, ovfile.size()-1);
            QFileInfo finfo(ovfile);
            if (finfo.isDir() != true)
            {
                finfo = QFileInfo(installationRoot+ovfile);
                if (finfo.exists())
                {
                    cout << "copying overlay file" << ovfile.toStdString() << endl << flush;
                    filesCopier.execute("cp -af " + ovfile+".mesksave" + " " + installationRoot+ovfile);
                    filesCopier.waitForFinished();
                    if(filesCopier.readAllStandardError().size() > 0)
                    {
                        setStatus(tr("Couldn't copy overlay file : ") + ovfile + tr(" to : ") + installationRoot, ERROR);
                        return 1;
                    }
                }
            }
        }
    }

    emit installationStatus(tr("updating local packages database"));

    //TODO : add to the previous package files copier loop
    QDir localPackages("/var/lib/pacman/local/");
    for ( int i =0; i < localPackages.entryList().size(); i++)
    {
        QString pkg = localPackages.entryList().at(i).split(QRegExp("-[0-9].*")).at(0);
        for (int k =0; k < packages.size(); k++)
        {
            if(packages.at(k).Name == pkg)
            {
                filesCopier.execute("cp -af /var/lib/pacman/local/" +localPackages.entryList().at(i) + " " + installationRoot + "/var/lib/pacman/local/" );
                filesCopier.waitForFinished();
                if(filesCopier.readAllStandardError().size() > 0)
                {
                    setStatus(tr("Couldn't copy package local : ") + localPackages.entryList().at(i) + tr(" to : ") + installationRoot+ "/var/lib/pacman/local/" , ERROR);
                    return 1;
                }
                break;
            }
        }
    }

    emit installationStatus(tr("configuring local packages"));

    filesCopier.execute("mkdir " + installationRoot + "/install" );
    filesCopier.waitForFinished();

    filesCopier.execute("cp /install/localpkgs.setup " + installationRoot + "/install" );
    filesCopier.waitForFinished();

    Root->exec("/usr/bin/bash /install/localpkgs.setup");

    filesCopier.execute("cp -af /var/lib/pacman/sync/ " + installationRoot + "/var/lib/pacman/sync/" );
    filesCopier.waitForFinished();
    if(filesCopier.readAllStandardError().size() > 0)
    {
        setStatus(tr("Couldn't copy package sync : /var/lib/pacman/sync/") + tr(" to : ") + installationRoot+ "/var/lib/pacman/sync/" , ERROR);
        return 1;
    }

    //mountRoot.start("cp -rf " + packages.at(i).packageFiles.at(k) + " /mnt/root/"); ;
    // remove uninstalled packages so that they are removed from local db :D
    // use pacman's option --dbonly

    //delete eventLoop;
    return 0;
}

int installationPage::generateKernel()
{
    //generate kernel images and stuff
    emit installationStatus(tr("generating kernel initramfs"));
    Root->exec("/sbin/depmod");

    //chroot  /usr/bin/mkinitcpio -p linux
    //QDir bootDir(installationRoot+"/boot/");    
    QDir bootDir(installationRoot+"/usr/lib/modules/");
    QStringList filters;
    filters << "vmlinuz*";
    bootDir.setNameFilters(filters);

    for ( int i =0; i < bootDir.entryList().size(); i++)
    {
        //if(bootDir.entryList().at(i).right(3) == "img")
        if(bootDir.entryList().at(i).contains("extramodules"))
        {
            continue;
        }
        
        if(bootDir.entryList().at(i).contains("ARCH"))
        {
			Root->exec("mkinitcpio -k " + bootDir.entryList().at(i) + " -g /boot/initramfs-linux.img ", 600000);
		}
		else
		{
			Root->exec("mkinitcpio -k " + bootDir.entryList().at(i) + " -g /boot/initramfs-" + bootDir.entryList()[i].remove(QRegExp("*-")  + ".img ", 600000);
		}
    }
    //FIX loop through all kernels in /usr/modules/ , and do depmod -b /lib/modules/version
    return 0;
}

int installationPage::generateLocales()
{
    emit installationStatus(tr("generating system locales"));

    // generate locales with locale-gen
    timePage* tpage = (timePage*)getDependency("Time");
    QString locales = tpage->getLocales();
    QFile localesfile;
    filesCopier.execute(installationRoot+"mv /etc/locale.gen " + installationRoot + "/etc/locale.gen.install");
    filesCopier.waitForFinished();

    localesfile.setFileName(installationRoot+"/etc/locale.gen");
    localesfile.open(QIODevice::WriteOnly | QIODevice::Append);
    localesfile.write(locales.toUtf8());
    localesfile.flush();
    localesfile.close();

    return Root->exec("locale-gen");
}

int installationPage::setTimeZone()
{
	emit installationStatus(tr("setting system timezone"));

	timePage* tpage = (timePage*)getDependency("Time");
	//ln -sf /usr/share/zoneinfo/your/zone /etc/localtime
	filesCopier.execute("ln -sf /usr/share/zoneinfo/" + tpage->getTimeZone() + " " + installationRoot + "/etc/localtime");
	filesCopier.waitForFinished();

    Root->exec("timedatectl set-timezone "+ tpage->getTimeZone());

    if(tpage->isLocalTime())
    {
        Root->exec("timedatectl set-local-rtc true");
    }

	return 0;
}

int installationPage::setAction(const QString& action)
{
    actionLabel->setText(action);
    //update();
    return 0;
}

void installationPage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
        Ui_installationPage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}


int installationPage::generateMachineID()
{
	Root->exec("systemd-machine-id-setup");
	return 0;
}


int installationPage::copyPostPackages()
{
	emit installationStatus(tr("copying post install packages"));

    filesCopier.execute("cp -rf /install/overlay/. " + installationRoot);
    filesCopier.waitForFinished(9900000);

	filesCopier.execute("mkdir " + installationRoot+"/install/");
	filesCopier.waitForFinished();

    filesCopier.execute("cp -r /install/postpackagesrepo/ " + installationRoot+"/install/");
    filesCopier.waitForFinished(9900000);

    filesCopier.execute("cp -r /install/postpackages/ " + installationRoot+"/install/");
    filesCopier.waitForFinished();

	QFile configStatusFile(installationRoot+"/install/config");
	configStatusFile.open(QIODevice::WriteOnly);
    configStatusFile.write("1\n", 2);
	configStatusFile.flush();
	configStatusFile.close();

	QFile inittabFile(installationRoot+"/etc/inittab");
	inittabFile.open(QIODevice::ReadWrite);
	QString inittabLines;
	while (!inittabFile.atEnd())
	{
		QString line = inittabFile.readLine();
		if(line.contains("3:initdefault"))
		{
			line = "#" + line;
		}
		else if(line.contains("5:initdefault"))
		{
			line = line.mid(1);
		}
        else if(line.contains("x:5:"))
        {
            line = "#" + line;
        }
		inittabLines += line;
	}

	inittabLines += "x:5:respawn:/sbin/mesk-config";

	inittabFile.close();
    inittabFile.open(QIODevice::Truncate|QIODevice::WriteOnly);
    inittabFile.write(inittabLines.toLatin1());
	inittabFile.close();
    //depmod
    Root->exec("fc-cache");
	return 0;
}
