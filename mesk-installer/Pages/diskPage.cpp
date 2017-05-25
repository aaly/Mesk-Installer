/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "diskPage.hpp"
#include <QtWidgets/QComboBox>
#include <MPF/Delegates/progressbarDelegate.hpp>

#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QTreeWidgetItem>

#include <QtConcurrent/QtConcurrentRun>


diskPage::diskPage(QWidget *parent) :
    pageBase(parent)
{
    Ui_diskPage::setupUi(this);

    currentHard = 0;

    filesystemComboBox->setEnabled(false);
    formatCheckBox->setEnabled(false);
    labelLineEdit->setEnabled(false);
    mountComboBox->setEnabled(false);

    formatToolsFilePath = getApplicationFile("/config/formattools.conf");
    labelToolsFilePath =  getApplicationFile("/config/labeltools.conf");

    pageName = tr("Disks Page");
    pageHelpMessage = tr("");

    installationRoot = "/mnt/root/";
    
    connect(this, SIGNAL(drivesReady()), this, SLOT(initViews()));

}

#include <QtWidgets/QMessageBox>

int diskPage::initAll()
{
	pageBase::initAll();
	setEnabled(false);
	QFuture<void> future = QtConcurrent::run(this, &diskPage::initDrives);
	return 0;
}

int diskPage::initDrives()
{
    setStatus(tr("Preparing hard disks"), BUSY);
    populatePartedDrives(disks);
    emit drivesReady();
}

int diskPage::initViews()
{
    for( int r=0; r<disks.size(); ++r )
    {
        QIcon dicon;

    // deprecated LOL ? kiss my axe
        if(disks[r].getTransport() == "ide" ||
            disks[r].getTransport() == "scsi" ||
            disks[r].getTransport() == "ataraid")
        {
            if (disks[r].isRemovable())
            {
                dicon = QIcon(getApplicationFile("/Icons/drive-removable-media.png"));
            }
            else
            {
                dicon = QIcon(getApplicationFile("/Icons/drive-harddisk.png"));
            }
        }
        else if(disks[r].getTransport() == "sdmmc")
        {
                dicon = QIcon(getApplicationFile("/Icons/media-flash-smart-media.png"));
        }

        disksList->addItem(dicon,
               disks[r].getModel() + " (" + disks[r].getSize() + ")");

        QStandardItemModel* partitionsTreeModel =
        new QStandardItemModel(disks[r].getPartitions().size(), 6, this);

        partitionsTreeModel->setHeaderData(0, Qt::Horizontal, tr("Label"));
        partitionsTreeModel->setHeaderData(1, Qt::Horizontal, tr("Partition"));
        partitionsTreeModel->setHeaderData(2, Qt::Horizontal, tr("Mount point"));
        partitionsTreeModel->setHeaderData(3, Qt::Horizontal, tr("File system"));
        partitionsTreeModel->setHeaderData(4, Qt::Horizontal, tr("Size"));
        partitionsTreeModel->setHeaderData(5, Qt::Horizontal, tr("Format ?"));

        for(int i =0; i < disks[r].getPartitions().size(); i++)
        {
            partitionsTreeModel->setItem(i, 0,
                                              new QStandardItem(disks[r].getPartitions()[i].getLabel()));
            partitionsTreeModel->setItem(i, 1,
                          new QStandardItem(disks[r].getPartitions()[i].getdevPath()));
            partitionsTreeModel->setItem(i, 2,
                                              new QStandardItem(""));
            partitionsTreeModel->setItem(i, 3,
                          new QStandardItem(disks[r].getPartitions()[i].getFSType()));
            partitionsTreeModel->setItem(i, 4,
                                              new QStandardItem(disks[r].getPartitions()[i].getSize()));
            partitionsTreeModel->setItem(i, 5,
                                         new QStandardItem(" "));


            QVector<Partition> partitions = disks[r].getPartitions()[i].getPartitions();

            if (partitions.count() > 0)
            {
                partitionsTreeModel->item(i, 3)->setText(tr("Extended"));
                for (int j = 0; j < partitions.count(); j++)
                {
                        partitionsTreeModel->item(i, 0)->setChild(j, 0,
                                                                  new QStandardItem(partitions[j].getLabel()));
                        partitionsTreeModel->item(i, 0)->setChild(j, 1,
                                                                  new QStandardItem(partitions[j].getdevPath()));
                        partitionsTreeModel->item(i, 0)->setChild(j, 2,
                                      new QStandardItem(""));
                        partitionsTreeModel->item(i, 0)->setChild(j, 3,
                                      new QStandardItem(partitions[j].getFSType()));
                        partitionsTreeModel->item(i, 0)->setChild(j, 4,
                                                                  new QStandardItem(partitions[j].getSize()));
                        partitionsTreeModel->item(i, 0)->setChild(j, 5,
                                                                  new QStandardItem(" "));
                }
                partitionsTreeView->setExpanded(partitionsTreeModel->item(i, 0)->index(), true);
            }
        }
        partitionsTreeModels.push_back(partitionsTreeModel);
    }



    partitionsTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    partitionsTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    partitionsTreeView->setSelectionMode(QAbstractItemView::SingleSelection);
    partitionsTreeView->setModel(partitionsTreeModels[0]);


    connect(disksList, SIGNAL(currentIndexChanged(int)),
        this, SLOT(refreshPartitionViews(int)));
    connect(formatCheckBox, SIGNAL(clicked(bool)), this, SLOT(formatSelected(bool)));
	connect(partitionsTreeView, SIGNAL(activated(QModelIndex)), this, SLOT(enableOperations(QModelIndex)));
	connect(partitionsTreeView, SIGNAL(clicked(QModelIndex)), this, SLOT(enableOperations(QModelIndex)));
	connect(partitionsTreeView, SIGNAL(pressed(QModelIndex)), this, SLOT(enableOperations(QModelIndex)));
    connect(applyPushButton, SIGNAL(clicked()), this, SLOT(applyActions()));
    connect(labelLineEdit, SIGNAL(textChanged(QString)), this, SLOT(changeLabel(QString)));
    connect(mountComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeMountPoint(int)));
    connect(filesystemComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeFilesystem()));
    connect(resetPushButton, SIGNAL(clicked()), this, SLOT(reset()));

    connect(addPushButton, SIGNAL(clicked()), this, SLOT(addAction()));
    connect(removePushButton, SIGNAL(clicked()), this, SLOT(removeAction()));


    initFormatTools();
    initLabelTools();


    mountPoints.push_back(tr("None"));
    mountPoints.push_back("/");
    mountPoints.push_back("/boot");
    mountPoints.push_back("/usr");
    mountPoints.push_back("/home");
    mountPoints.push_back("/opt");

    for ( int i =0; i < mountPoints.count(); i++)
    {
        mountComboBox->addItem(mountPoints.at(i));
    }

   /* QVector<QString> filesystems =  initFileSystems();

    for ( unsigned int i =0; i < filesystems.count(); i++)
    {
        filesystemComboBox->addItem(filesystems.at(i));
    }
    */

    availableFileSystems = initFileSystems2();

    for ( int i =0; i < availableFileSystems.count(); i++)
    {
        filesystemComboBox->addItem(availableFileSystems.at(i).second);
    }

    rootPartition = "";
    if (disks.size() > 0)
    {
        refreshPartitionViews(0);
    }

	setEnabled(true);
	update();
	disksList->update();
    emit Ready();

    return 0;
}

diskPage::~diskPage()
{
    QProcess mountRoot;


    mountRoot.start("umount " + rootPartition);
    mountRoot.waitForFinished();

    if (mountRoot.readAllStandardError().size() > 0)
    {
        setStatus("Couldn't un mount the installation partition : " + rootPartition , ERROR);
    }

}

int diskPage::initFormatTools()
{
    QFile file(formatToolsFilePath);

    if (!file.open(QFile::ReadOnly|QFile::Text))
    {
         return 1;
    }

    QTextStream inStream(&file);

    QString line = inStream.readLine();

    while (!line.isNull())
    {
        formatTools.insert(line.mid(0, line.indexOf(' ')),
                           line.mid(line.indexOf(' '), line.size()-line.indexOf(' ')+1));
        line = inStream.readLine();
    }
    return 0;
}

int diskPage::initLabelTools()
{
    QFile file(labelToolsFilePath);

    if (!file.open(QFile::ReadOnly|QFile::Text))
    {
         return 1;
    }

    QTextStream inStream(&file);

    QString line = inStream.readLine();

    while (!line.isNull())
    {
        labelTools.insert(line.mid(0, line.indexOf(' ')),
                           line.mid(line.indexOf(' '), line.size()-line.indexOf(' ')+1));
        line = inStream.readLine();
    }
    return 0;
}

QList<Drive> diskPage::getDisks()
{
    return disks;
}

int diskPage::refreshPartitionViews(int x)
{
    partitionsTreeView->setModel(partitionsTreeModels[x]);
    currentHard = x;
    partitionsTreeView->setItemsExpandable(true);
    partitionsTreeView->expandAll();

    // and set format ? field
    return 0;
}


int diskPage::formatSelected(bool cond)
{
    filesystemComboBox->setEnabled(cond);
    return 0;
}

QString diskPage::getText(int row)
{
    QString text;
    QModelIndex index = partitionsTreeView->currentIndex();
    QModelIndex parent = index.parent();

    if (parent.isValid())
    {
        text = partitionsTreeModels[currentHard]->item(parent.row())->child(index.row(), row)->text();
    }
    else
    {
        text = partitionsTreeModels[currentHard]->item(index.row(), row)->text();
    }

    return text;
}


Partition diskPage::getCurrentPartition()
{
    QModelIndex index = partitionsTreeView->currentIndex();
    QModelIndex parent = index.parent();

    if (parent.isValid())
    {
        return disks[currentHard].getPartitions()[parent.row()].getPartitions()[index.row()];
    }

    return disks[currentHard].getPartitions()[index.row()];
}

int diskPage::enableOperations(QModelIndex index)
{
    labelLineEdit->setText("");
    formatCheckBox->setChecked(false);

    QString fs = getText(FILESYSTEM);

    if(fs != "Extended")
    {
        mountComboBox->setEnabled(true);
        formatCheckBox->setEnabled(true);
        labelLineEdit->setEnabled(true);
    }
    else
    {
        formatCheckBox->setEnabled(false);
        labelLineEdit->setEnabled(false);
        mountComboBox->setEnabled(false);

    }

    /*if(formatTools.find(fs) == formatTools.end())
    {
        formatCheckBox->setEnabled(false);
    }
    else
    {
        formatCheckBox->setEnabled(true);
    }

    if(labelTools.find(fs) == labelTools.end())
    {
        labelLineEdit->setEnabled(false);
    }
    else
    {
        labelLineEdit->setEnabled(true);
    }*/

    return 0;
}

int diskPage::changeLabel(QString label)
{
    if (label == "")
    {
        return 1;
    }
    //partitionsTreeModels[currentHard]->item()
    return 0;
}

int diskPage::changeMountPoint(int index)
{
    return 0;
}


int diskPage::changeFilesystem()
{
    return 0;
}

int diskPage::hasAction(Partition& part, int Action)
{
    for (int i =0; i < diskActions.count(); i++)
    {
        if(diskActions[i].Action == Action &&
           diskActions[i].partition.getdevPath() == part.getdevPath())
        {
            return i;
        }
    }
    return -1;
}

int diskPage::addAction()
{
    if(formatCheckBox->isChecked())
    {
        partitionAction pAction;
        pAction.Action = PFORMAT;
        pAction.partition = getCurrentPartition();
        pAction.Value = filesystemComboBox->currentText();
        int index = hasAction(pAction.partition, pAction.Action);
        if( index == -1)
        //if( hasAction(pAction.partition, pAction.Action) == -1)
        {
            diskActions.push_back(pAction);
            QString text = tr("Format : ") + pAction.partition.getdevPath() + tr(" With ") + pAction.Value;
            QListWidgetItem* item = new QListWidgetItem(text, actionsListWidget);
            item->setTextColor(Qt::red);

            QModelIndex index = partitionsTreeView->currentIndex();
            QModelIndex parent = index.parent();

            if (parent.isValid())
            {
                partitionsTreeModels[currentHard]->item(parent.row())->child(index.row(), 5)->setText("✔ " + pAction.Value);
            }
            else
            {
                partitionsTreeModels[currentHard]->item(index.row(), 5)->setText("✔ " + pAction.Value); // ✓
            }

        }
        else
        {
            QString message = diskActions[index].partition.getdevPath() + tr(" will already be formatted with ") +
                                diskActions[index].Value;
            setStatus(message, WARNING);

        }
    }

    if(!labelLineEdit->text().isEmpty())
    {
        partitionAction pAction;
        pAction.Action = PLABEL;
        pAction.partition = getCurrentPartition();
        pAction.Value = labelLineEdit->text();
        int index = hasAction(pAction.partition, pAction.Action);
        if( index == -1)
        {
            diskActions.push_back(pAction);
            QString text = tr("Label   : ") + pAction.partition.getdevPath() + tr(" With ") + pAction.Value;
            QListWidgetItem* item = new QListWidgetItem(text, actionsListWidget);
            item->setTextColor(Qt::darkYellow);
        }
        else
        {
            QString message = diskActions[index].partition.getdevPath() + tr(" will be already labled with ") +
                                diskActions[index].Value;
            setStatus(message, WARNING);
        }
    }

    if(mountComboBox->itemText(mountComboBox->currentIndex()) != "None")
    {
        partitionAction pAction;
        pAction.Action = PMOUNT;
        pAction.partition = getCurrentPartition();
        pAction.Value = mountComboBox->itemText(mountComboBox->currentIndex());
        int index = hasAction(pAction.partition, pAction.Action);
        QString message;
        for (int i =0; i < diskActions.size(); i++)
        {

            if ( diskActions.at(i).Action == PMOUNT)
            {
                if ( diskActions.at(i).Value == mountComboBox->itemText(mountComboBox->currentIndex()))
                {
                    message =  diskActions[i].partition.getdevPath() + tr("will already be mounted to :") + pAction.Value;
                    index = -1;
                }
            }
        }

        if( index == -1)
        {
            // if rootPartition is already set, give error
            // if we clear or reset actions, rootParititon should be cleared
            // check if there is a an action related to formatting that or if that partition has a linux type anyway

            if(pAction.Value == "/")
            {
                rootPartition = getCurrentPartition().getdevPath();
            }

            diskActions.push_back(pAction);
            QString text = tr("Mount   : ") + pAction.partition.getdevPath() + tr(" To ") + pAction.Value;
            QListWidgetItem* item = new QListWidgetItem(text, actionsListWidget);
            item->setTextColor(Qt::darkCyan);

        }
        else
        {
            message = diskActions[index].partition.getdevPath() + tr(" will be already mounted to ") +
                                diskActions[index].Value;
            setStatus(message, WARNING);
        }
    }

    labelLineEdit->setText("");
    formatCheckBox->setChecked(false);
    mountComboBox->setCurrentIndex(0);

    return 0;
}

int diskPage::removeAction()
{
    if (! actionsListWidget->count() > 0)
    {
        return 1;
    }
    if (diskActions.at(actionsListWidget->currentRow()).Action == PFORMAT)
    {
        QModelIndex index;
        for (int i =0; i < disks.size(); i++)
        {
            for (int k =0; k < disks[i].getPartitions().size(); k++)
            {
                if(disks[i].getPartitions()[k].getPartitions().size() > 0)
                {
                    for (int l =0; l < disks[i].getPartitions()[k].getPartitions().size(); l++)
                    {
                        if(disks[i].getPartitions()[k].getPartitions()[l].getdevPath() == diskActions[actionsListWidget->currentRow()].partition.getdevPath())
                        {
                            index = partitionsTreeModels[i]->index(k, 0).child(l, 5);
                            break;
                        }
                    }
                }
                else
                {
                    if(disks[i].getPartitions()[k].getdevPath() == diskActions[actionsListWidget->currentRow()].partition.getdevPath())
                    {
                        index = partitionsTreeModels[i]->index(k, 5);
                        break;
                    }
                }
            }
        }
        QModelIndex parent = index.parent();

        if (parent.isValid())
        {
            partitionsTreeModels[currentHard]->item(parent.row())->child(index.row(), 5)->setText(" ");
        }
        else
        {
            partitionsTreeModels[currentHard]->item(index.row(), 5)->setText(" ");
        }
    }

    if (diskActions.at(actionsListWidget->currentRow()).Action == PMOUNT &&
            diskActions.at(actionsListWidget->currentRow()).Value == "/")
    {
        rootPartition = "";
    }

    diskActions.remove(actionsListWidget->currentRow());
    actionsListWidget->takeItem(actionsListWidget->currentRow());
    return 0;
}

int diskPage::applyActions()
{
    QtConcurrent::run(this, &diskPage::applyActionsThread);
    return 0;
}

int diskPage::applyActionsThread()
{
    applyPushButton->setEnabled(false);
    resetPushButton->setEnabled(false);
    removePushButton->setEnabled(false);

    //sort format before mount
    for (int i =0; i < diskActions.size(); i++)
    {
        if (diskActions.at(i).Action == PMOUNT)
        {
            for (int k =i; k < diskActions.size(); k++)
            {
                if (diskActions.at(k).Action == PFORMAT)
                {
                    partitionAction old = diskActions.at(i);
                    diskActions[i] =  diskActions.at(k);
                    diskActions[k] = old;
                }
            }
        }
    }

    // check if root has a good filesystem ?
    // check if any of the formatted or whatever partitions is the actually live usb?
    for (int i =0; i < diskActions.size(); i++)
    {
        //setStatus(actionsListWidget->item(i)->text(), BUSY);

        Partition p = (diskActions.at(i)).partition;
        unmount(p.getdevPath());
		QProcess format;
		format.start("/bin/umount -f " + p.getdevPath());
		format.waitForFinished(800000);

        if (diskActions.at(i).Action == PFORMAT)
        {
			cout << "Formatting : " << p.getdevPath().toStdString() <<" : " << diskActions.at(i).Value.toStdString() <<  endl << flush;
            for (int k =0; k < availableFileSystems.size(); k++)
            {
                if ( availableFileSystems.at(k).second == diskActions.at(i).Value)
                {
					cout << "Formatting : " << availableFileSystems.at(k).first.toStdString() <<  endl << flush;
                    //execute command
                    QProcess format;
                    format.start(availableFileSystems.at(k).first + " " + p.getdevPath());
                    format.waitForFinished(800000); // 800 second ? enough ?

                }
            }
        }
        else if (diskActions.at(i).Action == PLABEL)
        {
            p.setLabel(diskActions.at(i).Value);
        }
        else if (diskActions.at(i).Action == PMOUNT)
        {
            //TODO: check if the mountpoint is already used before ?
            mountPoint mp;
            mp.partition=p.getdevPath();
			mp.path = diskActions.at(i).Value;

            int index = hasAction(p, PFORMAT);
            if ( index != -1 )
            {
                mp.fs="NONE";
            }
            else
            {
                mp.fs=diskActions.at(i).Value;
            }
            bool mountedBefore = false;
            for ( int k =0; k < installationMountPoints.size() ; k++)
            {
                if(mp.path == installationMountPoints.at(k).path)
                {
                    setStatus(tr("Mount point is already used :") + mp.path + tr(" Ignoring this action !"), WARNING);
                    mountedBefore = true;
                    break;
                }
            }
            if (!mountedBefore)
            {
                installationMountPoints.push_back(mp);
            }
        }
        setStatus(tr("Done") + " " +  actionsListWidget->item(i)->text(), STATUS);

        // leave them for finding formatted filesystems for mounting and stuff... ?
        diskActions.remove(i);
		actionsListWidget->takeItem(i);
        i--;
    }

    if (rootPartition.isEmpty())
    {
        setStatus(tr("Please choose a root mount point to install Mesklinux"), ERROR);
    }

    applyPushButton->setEnabled(true);
    resetPushButton->setEnabled(true);
    removePushButton->setEnabled(true);


    QProcess mountRoot;

    mountRoot.execute("mkdir -p " + installationRoot);
    mountRoot.waitForFinished();
    if(mountRoot.readAllStandardError().size() > 0)
    {
        setStatus("Couldn't create installation root : "+installationRoot, ERROR);
        return 1;
    }

    // TODO: only if it is already mounted ???
    mountRoot.start("umount " + rootPartition);
    mountRoot.waitForFinished(20000);
    /*
    if (mountRoot.readAllStandardError().size() > 0)
    {
        setStatus("Couldn't un mount the installation partition : " + dpage->rootPartition , ERROR);
    }*/

    mountRoot.start("mount " + rootPartition + " " + installationRoot);
    mountRoot.waitForFinished(10000);

    if (mountRoot.readAllStandardError().size() > 0)
    {
        setStatus("Couldn't mount the installation partition : " + rootPartition , ERROR);
        return 1;
    }
    
    setDone(true);

    return 0;
}


QString diskPage::parseCommand(QString command, QString part, QString oldString ="", QString newString ="")
{
    QString result = command;
    if(oldString != "" &&
       newString != "")
    {
        result = result.replace(oldString, newString);
    }
    result = result.replace("partition", part);
    return result;
}

int diskPage::formatPartition(Partition part, QString fs)
{
    PedFileSystemType* pfst = ped_file_system_type_get(fs.toStdString().c_str());

    //if (ped_file_system_create(&part.getPartedPartition()->geom, pfst, NULL) == NULL)
    if(ped_partition_set_system(part.getPartedPartition(), pfst))
    {
        actionProcess.start(parseCommand(formatTools[fs], part.getdevPath()));
        if(actionProcess.waitForStarted(1000) ||
           actionProcess.waitForFinished(10000))
        {
            return 0;
        }
        return 1;
    }

    return 0;
}

int diskPage::labelPartition(Partition part, QString label)
{
    if (!ped_partition_set_name(part.getPartedPartition(), label.toStdString().c_str()))
    {
        actionProcess.start(labelTools[label],  QStringList() << part.getdevPath());
        if(actionProcess.waitForStarted(1000) ||
           actionProcess.waitForFinished(10000))
        {
            return 0;
        }
        return 1;
    }
    return 0;
}

int diskPage::reset()
{
    //actionsListWidget->clear();
    //diskActions.clear();

    while (diskActions.size() > 0)
    {
        //actionsListWidget->itemAt(0, 0)->setSelected(true);
        actionsListWidget->setCurrentRow(0);
        removeAction();
    }
    return 0;
}


QVector<mountPoint> diskPage::getInstallationMountPoints() const
{
    return installationMountPoints;
}

void diskPage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
        Ui_diskPage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}
