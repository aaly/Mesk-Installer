/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "bootloaderPage.hpp"
#include <MPF/Delegates/richTextDelegate.hpp>
#include <QDir>

bootloaderPage::bootloaderPage(QWidget *parent) :
    pageBase(parent){

	Ui_bootloaderPage::setupUi(this);


    connect(bootPasswordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkBootloaderPasswordStrength()));
    connect(bootConfirmPasswordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(confirmBootloaderPassword()));

    connect(resetBootMenuPushButton, SIGNAL(clicked()), this, SLOT(resetBootMenu()));
    connect(addBootEntryPushButton, SIGNAL(clicked()), this, SLOT(addBootEntry()));
    connect(removeBootEntryPushButton, SIGNAL(clicked()), this, SLOT(removeBootEntry()));


    //bootMenuTableWidget->setItemDelegate(new richTextDelegate(this));
    bootMenuTableWidget->setItemDelegateForColumn(2, new richTextDelegate(this));
    bootMenuTableWidget->setItemDelegateForColumn(3, new richTextDelegate(this));
    bootMenuTableWidget->setItemDelegateForColumn(4, new richTextDelegate(this));
    bootMenuTableWidget->setItemDelegateForColumn(5, new richTextDelegate(this));

}

bootloaderPage::~bootloaderPage()
{
    croot.unprepare();
	mountRoot.start("umount /mnt/root");
    mountRoot.waitForFinished();
}


int bootloaderPage::initAll()
{
    pageBase::initAll();
    dpage = (diskPage*)getDependency("Disk");
    drives =  dpage->getDisks();


    cout << "Disk Page 1" << endl << flush;
    for (int i =0, l =0; i < drives.size(); i++)
    {
		setupComboBox->addItem(drives[i].getdevPath().remove("/dev/"));

        for (int k =0; k < drives[i].getExtendedPartitions().size(); k++)
        {
			rootComboBox->addItem(drives[i].getExtendedPartitions()[k].getdevPath().remove("/dev/"));
            bootEntryRootComboBox->addItem(drives[i].getExtendedPartitions()[k].getdevPath().remove("/dev/"));
            if(drives[i].getExtendedPartitions()[k].getdevPath() == dpage->rootPartition)
            {
                rootComboBox->setCurrentIndex(l);
                setupComboBox->setCurrentIndex(i);
            }
            l++;
        }
    }
    cout << "Disk Page 2" << endl << flush;
    bootEntryRootComboBox->setEditable(true);

	mountRoot.start("mount " + dpage->rootPartition + " /mnt/root");
    mountRoot.waitForFinished();

    rootPath = "/mnt/root/";
    croot.setRoot(rootPath);
    croot.prepare();

cout << "Disk Page 3" << endl << flush;

	for (int i =0; i < dpage->getInstallationMountPoints().size(); i++)
	{
		if(dpage->getInstallationMountPoints().at(i).path == "/boot")
		{
			bootPartition = dpage->getInstallationMountPoints().at(i).partition;
		}
	}

    cout << "Disk Page 4" << endl << flush;
	if(bootPartition.size() > 0)
	{
		setupComboBox->setCurrentIndex(setupComboBox->findText(bootPartition.remove("/dev/").mid(0, 3)));
		rootComboBox->setCurrentIndex(rootComboBox->findText(bootPartition.remove("/dev/")));
	}

    cout << "Disk Page 5" << endl << flush;
    generateBootMenu();

    emit Done(true);
    return 0;
}



int bootloaderPage::checkBootloaderPasswordStrength()
{
	QString icon;
	if(bootPasswordLineEdit->text().size() < 6)
	{
		icon = getApplicationFile("/Icons/weakpassword.png");
	}
	else if(bootPasswordLineEdit->text().size() < 8)
	{
		icon = getApplicationFile("/Icons/mediumpassword.png");
	}
	else
	{
		icon = getApplicationFile("/Icons/strongpassword.png");
	}
	bootPasswordLabel->setText("<img src="+icon+ " height=20 width=20 >");

	if(bootConfirmPasswordLineEdit->text().size() >0)
	{
		confirmBootloaderPassword();
	}
	return 0;
}

int bootloaderPage::confirmBootloaderPassword()
{
    QString icon;
    if(bootPasswordLineEdit->text() == bootConfirmPasswordLineEdit->text())
    {
        icon = getApplicationFile("/Icons/right.png");
    }
    else
    {
        icon = getApplicationFile("/Icons/wrong.png");
    }
    bootConfirmPasswordStatusLabel->setText("<img src=\""+icon+ "\" height=20 width=20 >");
    return 0;
}


int bootloaderPage::generateBootMenu()
{
    croot.exec("grub-mkconfig -o /boot/grub/grub.cfg");

    //FIXME: what if the boot is another partition ? need to get it from disk page
    bootMenuFile.setFileName("/mnt/root/boot/grub/grub.cfg");
    if (!bootMenuFile.open(QIODevice::ReadOnly))
    {
        setStatus(tr("Couldn't open /mnt/root/boot/grub/grub.cfg to generate boot menu view"), ERROR);
        return 1;
    }

    QTextStream bootFileStream(&bootMenuFile);
    QString line;
    //QString entry;
    bool entry = false;
	bool submenu = false;
    bootEntry bt;
    while ( !bootFileStream.atEnd() )
    {
        line = bootFileStream.readLine();
        //entry = "";

        if( line.mid(0, 9) == "menuentry")
        {
            bt.title = line.mid(line.indexOf("'")+1, line.indexOf("'", line.indexOf("'")+1)-line.indexOf("'")-1);
            //bt.os = line.mid(line.indexOf("--class")+8,line.indexOf(' ', line.indexOf("--class"))-line.indexOf("--class") );
            if(line.contains("linux"))
            {
                bt.os = "linux";
            }
            else if(line.contains("windows"))
            {
                bt.os = "windows";
            }
            cout << "OS : " << bt.os.toStdString() << endl;
            entry = true;
            continue;
        }
		else if(line == "submenu")
		{
			submenu = true;
			continue;
		}

        if(entry)
        {
			if( line.contains(QRegExp("^( |\t)+linux")))
				//if( line.contains(QRegExp("\s*linux")))
			{
				bt.kernel = line.split(QRegExp(("( |\t)"))).at(2);
				bt.kernelOptions = line.remove(bt.kernel).remove("linux");
				continue;
			}
			else if( line.contains(QRegExp("^( |\t)+initrd")))
			{
				bt.initramfs = line.split(QRegExp(("( |\t)"))).at(2);
				continue;
			}
			else if( line.contains("root=")) //set root='hd0,msdos1'
			{
				QString root = line.mid(line.indexOf("'")+1, line.lastIndexOf("'")-line.indexOf("'")-1);
				QString drive = root.split(",").at(0);
				drive.remove("hd");

				QString partition = root.split(",").at(1);
				partition.remove(QRegExp("msdos"));

				bt.rootpartition.first = drive.toInt();
				bt.rootpartition.second = partition.toInt();
				bt.root = dpage->getDisks()[drive.toInt()].getFullPartitions().at(partition.toInt()-1).getdevPath().remove("/dev/");
				bt.rootUUID=dpage->getDisks()[drive.toInt()].getFullPartitions().at(partition.toInt()-1).getUUID();
				bt.rootDevPath = dpage->getDisks()[drive.toInt()].getFullPartitions().at(partition.toInt()-1).getdevPath();
				/*for(int i =0; i < dpage->getDisks()[drive.toInt()].getExtendedPartitions().size(); i++)
					{
						if ( (dpage->getDisks().at(drive.toInt()).getPartitions().at(i)).getPartitions().size() + (i+1) >= partition.toInt() )
						{
							cout << "DONE5" << endl;
							bt.root = ((dpage->getDisks().at(drive.toInt()).getPartitions().at(i)).getPartitions().at(partition.toInt()-(i+1+1))).getdevPath().remove("/dev/");
							cout << "DONE6" << endl;
							bt.rootpartition.first = drive.toInt();
							bt.rootpartition.second = partition.toInt()-(i+1+1);
							break;
						}
					}*/


				//bt.root = dpage->getDisks().at(drive.toInt()).getPartitions().at(partition.toInt()-1).getdevPath().remove("/dev/");
			}
			else if( line.contains("echo") )
			{
				continue;
			}
			//else if( line.contains(QRegExp("( |\t)*}")) )
			else if( line == "}" )
			{
				bt.sub = submenu;
				bootMenu.push_back(bt);
				addToBootMenu();
				entry = false;
				bt = bootEntry();
			}
			else
			{
				bt.options += line + "\n";
			}
		}
		else
		{
			if(submenu && line == "}")
			{
				submenu = false;
			}
			else
			{
				cfgOptions += line + "\n";
			}
		}

	}

    bootMenuFile.close();
	connect(bootMenuTableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(modifyBootEntry(int, int)));

	return 0;

}

int bootloaderPage::installBootLoader()
{
    //make the menu
    setStatus(tr("generating the boot menu"), STATUS);
    writeBootMenu();
    //install grub
    setStatus(tr("installing the boot software"), STATUS);
    //copy to boot partition if exists !

	if(bootPartition.size() > 0)
	{
		mountRoot.start("mkdir /mnt/boot");
		mountRoot.waitForFinished();
		mountRoot.start("umount -l " + bootPartition + " /mnt/boot");
		mountRoot.waitForFinished();
		mountRoot.start("mount " + bootPartition + " /mnt/boot");
		mountRoot.waitForFinished();
		mountRoot.start("cp -r /mnt/root/boot /mnt/boot/");
		mountRoot.waitForFinished();
		mountRoot.start("umount -l " + bootPartition + " /mnt/boot");
		mountRoot.waitForFinished();
		mountRoot.start("mount " + bootPartition + " /mnt/root/boot");
		mountRoot.waitForFinished();
	}

	mountRoot.start("grub-install --boot-directory=/mnt/root/boot/ /dev/" + setupComboBox->currentText());
	mountRoot.waitForFinished();

	if(bootPartition.size() > 0)
	{
		mountRoot.start("umount " + bootPartition + " /mnt/root/boot");
		mountRoot.waitForFinished();
	}
    return 0;
}



void bootloaderPage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
		Ui_bootloaderPage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}



int bootloaderPage::removeBootEntry()
{
    int currentRow = bootMenuTableWidget->currentRow();

    if (currentRow < 0)
    {
        return 1;
    }
    bootMenu.remove(currentRow);
    bootMenuTableWidget->takeItem(currentRow, 0);
    bootMenuTableWidget->takeItem(currentRow, 1);
    bootMenuTableWidget->takeItem(currentRow, 2);
    bootMenuTableWidget->takeItem(currentRow, 3);
    bootMenuTableWidget->takeItem(currentRow, 4);
	bootMenuTableWidget->takeItem(currentRow, 5);
    bootMenuTableWidget->removeRow(currentRow);
    return 0;
}

int bootloaderPage::addBootEntry()
{
    //TODO: check if it is already there ?
    bootEntry bt;
    bt.title = bootEntryTitlelineEdit->text();
    bt.root = bootEntryRootComboBox->currentText();
    bt.kernel = bootEntryKernellineEdit->text();
	bt.kernelOptions = bootEntryKernelOptionslineEdit->text();
    bt.initramfs = bootEntryInitramfsLineEdit->text();
    bt.options = bootEntryOptionsLineEdit->text();

	bt.rootDevPath = "/dev/"+bt.root ;

	for (int i =0; i < dpage->getDisks().size(); i++)
	{
		for (int k=0; k < dpage->getDisks().at(i).getPartitions().size(); k++)
		{
			if(bt.rootDevPath == dpage->getDisks().at(i).getPartitions().at(k).getdevPath())
			{
				bt.rootUUID = dpage->getDisks().at(i).getPartitions().at(k).getUUID();
				//FIXME: we used to get it from hd?,msdos? , but is this the right order?
				bt.rootpartition.first = i ;
				bt.rootpartition.second = k+1 ;
			}
		}
	}

    if( !bt.title.size() > 0)
    {
        bootEntryTitleLabel->setText("<div style=\" color:red;\" >" + tr("Title") + " </div>");
        return 0;
    }
    else
    {
        bootEntryTitleLabel->setText( tr("Title"));
    }

    if( !bt.root.size() > 0)
    {
        bootEntryRootLabel->setText("<div style=\" color:red;\" >" + tr("Root") + " </div>");
        return 0;
    }
    else
    {
        bootEntryRootLabel->setText( tr("Root"));
    }

    bootMenu.push_back(bt);

    bootEntryInitramfsLineEdit->setText("");
    bootEntryKernellineEdit->setText("");
	bootEntryKernelOptionslineEdit->setText("");
    bootEntryOptionsLineEdit->setText("");
    bootEntryRootComboBox->setCurrentIndex(0);
    bootEntryTitlelineEdit->setText("");

    addToBootMenu();

    return 0;
}

int bootloaderPage::addToBootMenu()
{
	bootEntry bt = (const bootEntry)bootMenu.at(bootMenu.size()-1);

int nextRow = bootMenuTableWidget->rowCount();

bootMenuTableWidget->insertRow(nextRow);
bootMenuTableWidget->setItem(nextRow, 0, new QTableWidgetItem(bt.title));

if(bt.sub)
{
	(bootMenuTableWidget->itemAt(nextRow, 0))->setIcon(QIcon(getApplicationFile("/Icons/submenu.png")));
}
bootMenuTableWidget->setItem(nextRow, 1, new QTableWidgetItem(bt.root));

bool kernelEditable = true;
bool kernelOptionsEditable = true;
bool initramfsEditable = true;
bool optionsEditable = true;

if(bt.kernel.size() == 0)
{
	bt.kernel = "<img align=absmiddle height=20 width=20 src=" + getApplicationFile("/Icons/windows.png") + ">";
    kernelEditable= false;
}
/*
//bootMenuTableWidget->setItem(nextRow, 2, new QTableWidgetItem(kernel).setIcon(););
QIcon icon(getApplicationFile("/Icons/windows.png"));
//bootMenuTableWidget->setItem(nextRow, 2, (new QTableWidgetItem())->setIcon(icon));
QTableWidgetItem* kernelitem = new QTableWidgetItem();
kernelitem->setIcon(icon);
kernelitem->setTextAlignment(Qt::AlignHCenter);
bootMenuTableWidget->setItem(nextRow, 2, kernelitem);
*/

QTableWidgetItem* kernelItem = new QTableWidgetItem(bt.kernel);
if(!kernelEditable)
{
    kernelItem->setFlags(kernelItem->flags() & (Qt::ItemFlag)~Qt::ItemIsEditable);
}
bootMenuTableWidget->setItem(nextRow, 2, kernelItem);


if(bt.kernelOptions.size() == 0)
{
	bt.kernelOptions = "<img align=absmiddle height=20 width=20 src=" + getApplicationFile("/Icons/windows.png") + ">";
    kernelOptionsEditable = false;
}

QTableWidgetItem* kernelOptionsItem = new QTableWidgetItem(bt.kernelOptions);
if(!kernelOptionsEditable)
{
    kernelOptionsItem->setFlags(kernelOptionsItem->flags() & (Qt::ItemFlag)~Qt::ItemIsEditable);
}
bootMenuTableWidget->setItem(nextRow, 3, kernelOptionsItem);


if(bt.initramfs.size() == 0)
{
	bt.initramfs = "<img align=absmiddle height=20 width=20 src=" + getApplicationFile("/Icons/windows.png") + ">";
    initramfsEditable= false;
}

QTableWidgetItem* initramfslItem = new QTableWidgetItem(bt.initramfs);
if(!initramfsEditable)
{
    initramfslItem->setFlags(initramfslItem->flags() & (Qt::ItemFlag)~Qt::ItemIsEditable);
}
bootMenuTableWidget->setItem(nextRow, 4, initramfslItem);

if(bt.options.size() == 0)
{
	bt.options = "<img align=absmiddle height=20 width=20 src=" + getApplicationFile("/Icons/windows.png") + ">";
    optionsEditable = false;
}

QTableWidgetItem* optionsItem = new QTableWidgetItem(bt.options);
if(!optionsEditable)
{
    optionsItem->setFlags(optionsItem->flags() & (Qt::ItemFlag)~Qt::ItemIsEditable);
}
bootMenuTableWidget->setItem(nextRow, 5, optionsItem);

/*
QColor bgcolor(Qt::blue);

if(nextRow > 0)
{
    if(bootMenuTableWidget->itemAt(nextRow, 1)->text() != bootMenuTableWidget->itemAt(nextRow-1, 1)->text() )
    {
        bgcolor = QColor(Qt::red);
    }
}

bootMenuTableWidget->itemAt(nextRow, 0)->setBackgroundColor(bgcolor);
bootMenuTableWidget->itemAt(nextRow, 1)->setBackgroundColor(bgcolor);
bootMenuTableWidget->itemAt(nextRow, 2)->setBackgroundColor(bgcolor);
bootMenuTableWidget->itemAt(nextRow, 3)->setBackgroundColor(bgcolor);
bootMenuTableWidget->itemAt(nextRow, 4)->setBackgroundColor(bgcolor);
bootMenuTableWidget->itemAt(nextRow, 5)->setBackgroundColor(bgcolor);
*/

/*
QIcon icon;

if(bt.os == "windows")
{
    icon = QIcon(getApplicationFile("/Icons/windows.png"));
}
else if (bt.os == "linux")
{
    icon = QIcon(getApplicationFile("/Icons/linux.png"));
}
else if (bt.os == "osx")
{
    icon = QIcon(getApplicationFile("/Icons/mac.png"));
}

(bootMenuTableWidget->itemAt(0, 2))->setIcon(icon);
(bootMenuTableWidget->itemAt(0, 3))->setIcon(icon);
(bootMenuTableWidget->itemAt(0, 4))->setIcon(icon);
(bootMenuTableWidget->itemAt(0, 5))->setIcon(icon);
*/

bootMenuTableWidget->resizeColumnsToContents();


return 0;
}

int bootloaderPage::writeBootMenu()
{
	mountRoot.start("rm " + rootPath + "/boot/grub/grub.cfg");
    mountRoot.waitForFinished();

    QFile grubCfg(rootPath + "/boot/grub/grub.cfg");

    if (!grubCfg.open(QIODevice::WriteOnly))
    {
        setStatus(tr("Error opening the grub config file for writing"), ERROR);
        return 1;
    }
    QTextStream grubCfgStream(&grubCfg);
    grubCfgStream << cfgOptions << endl;

    for(int i =0; i < bootMenu.size(); i++)
    {
    cout << "TITLE  :" << bootMenu.at(i).title.toStdString();
        // title
        grubCfgStream << "menuentry '" << bootMenu.at(i).title << "' --class " << bootMenu.at(i).os << " {" << endl;
        // entry options
		if(UUIDBootCheckBox->isChecked())
		{
			//opts.replace(QRegExp("UUID=\\S*"), "/dev/sda3")
			QString opts = bootMenu.at(i).options;
			grubCfgStream << opts.replace(QRegExp("UUID=\\S*"), bootMenu.at(i).rootDevPath) << endl;

		}
		else
		{
			grubCfgStream << bootMenu.at(i).options << endl;
		}

        // root
		grubCfgStream << "set root=hd" << bootMenu.at(i).rootpartition.first << "," << bootMenu.at(i).rootpartition.second <<  endl;

        if(bootMenu.at(i).os == "linux")
        {
			// kernel
			grubCfgStream << "echo '" + tr("Loading Kernel") + "'" << endl;
			grubCfgStream << "linux " << bootMenu.at(i).kernel << " " << bootMenu.at(i).kernelOptions << endl;
			// initramfs
			grubCfgStream << "echo '" + tr("Loading initial ramdisk") + "'" << endl;
			grubCfgStream << "initrd " << bootMenu.at(i).initramfs << endl;
		}
        grubCfgStream << "}" <<  endl;
        grubCfgStream.flush();
        grubCfg.flush();
    }

    return 0;
}

int bootloaderPage::resetBootMenu()
{
    for ( int i =0; i < bootMenuTableWidget->rowCount(); i++)
    {
        for ( int k =0; k < bootMenuTableWidget->columnCount(); k++)
        {
            bootMenuTableWidget->takeItem(i, k);
        }
    }
    bootMenuTableWidget->setRowCount(0);
    return 0;

}

int bootloaderPage::finishUp()
{
    pageBase::finishUp();
	if(bootloaderGroupBox->isChecked())
	{
		installBootLoader();
	}
    //if boot partition , mount it and copy /boot/* to it


	setStatus(tr("Generating fstab"), INFORMATION);
	generateFsTab();
    return 0;
}

int bootloaderPage::modifyBootEntry(int row, int col)
{
    if(col == 0) // title
    {
        bootMenu[row].title = bootMenuTableWidget->itemAt(row, col)->text();
    }
    else if(col == 1) // root
    {
        bootMenu[row].root = bootMenuTableWidget->itemAt(row, col)->text();
        //bootMenu[row].rootpartition.first = bootMenuTableWidget->itemAt(row, col)->text();
        //bootMenu[row].rootpartition.second = bootMenuTableWidget->itemAt(row, col)->text();
    }
    else if(col == 2) // kernel
    {
        bootMenu[row].kernel = bootMenuTableWidget->itemAt(row, col)->text();
    }
    else if(col == 3) // kernel opts
    {
        bootMenu[row].kernelOptions = bootMenuTableWidget->itemAt(row, col)->text();
    }
    else if(col == 4) // initramfs
    {
        bootMenu[row].initramfs = bootMenuTableWidget->itemAt(row, col)->text();
    }
    else if(col == 5) // options
    {
        bootMenu[row].options = bootMenuTableWidget->itemAt(row, col)->text();
    }
    return 0;
}



int bootloaderPage::generateFsTab()
{
	QFile fstabfile(rootPath+"/etc/fstab");
	if (!fstabfile.open(QIODevice::Append))
	{
		setStatus(tr("Couldn't open the installation fstab file !"), ERROR);
		return 1;
	}
	QTextStream fstabfilestream(&fstabfile);

// loop for all devices, if removeable, skip ( add removable bool to drives
// add all partitions, if swap 0 0 else 0 1
// if ntfs then write ntfs-3g


	for ( int i =0; i < drives.size(); i++)
	{
		if(drives.at(i).isRemovable())
		{
			continue;
		}

		for (int k=0, l =0; k < drives[i].getExtendedPartitions().size(); k++)
		{

			Partition part = drives[i].getExtendedPartitions().at(k);
			/*if(part.getPartitions().size() > 0)
			{
				part = part.getPartitions().at(l);
				l++;

				if(l == part.getPartitions().size())
				{
					l = 0;
				}
				else
				{
					k--;
				}
			}*/

			fstabfilestream << "\n";

			// partition
			if(UUIDFsTabCheckBox->isChecked())
			{
				fstabfilestream << "UUID=" << part.getUUID() << "\t";
			}
			else
			{
				fstabfilestream << part.getdevPath() << "\t";
			}

			// mount point ('none' if swap)
			if(part.getFSType() == "swap")
			{
				fstabfilestream << "none" << "\t";
			}
			else
			{
				QString mountPoint = "/mnt/";

				if( part.getLabel().size() != 0)
				{
					mountPoint += part.getLabel();
				}
				else
				{
					mountPoint += part.getdevPath().mid(part.getdevPath().lastIndexOf('/'));
				}

				for ( int k =0; k < dpage->getInstallationMountPoints().size(); k++)
				{
					if(dpage->getInstallationMountPoints().at(k).partition == part.getdevPath())
					{
						mountPoint = dpage->getInstallationMountPoints().at(k).path;
					}
				}

				fstabfilestream << mountPoint << "\t";

				//QDir::mkdir(rootPath+mountPoint);
			}

			// filesystem (ntfs-3g if ntfs)
			if(part.getFSType() == "ntfs")
			{
				fstabfilestream << "ntfs-3g" << "\t";
			}
			else
			{
				fstabfilestream << part.getFSType() << "\t";
			}

			// options sw if swap , defaults if others
			 if(part.getFSType() == "swap")
			 {
				 fstabfilestream << "sw" << "\t";
			 }
			 else
			 {
				 fstabfilestream << "defaults" << "\t";
			 }

			// 0 1 ( 0 0 if swap ! )
			 if(part.getFSType() == "swap")
			 {
				 fstabfilestream << "0 0";
			 }
			 else
			 {
				 fstabfilestream << "0 1";
			 }
			 fstabfilestream.flush();
		}
	}
	cout << "DONEEEEEEEEEE" << endl << flush;
	return 0;
}

