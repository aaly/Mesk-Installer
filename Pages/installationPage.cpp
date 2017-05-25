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
    Root = new CHRoot(this);
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

    installPackages();
    //installationProgressBar->setValue(80);
    QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
                               Q_ARG( int, 80 ) );

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



int installationPage::cp(const char *to, const char *from)
{
    int fd_to, fd_from;
    char buf[4096];
    ssize_t nread;
    int saved_errno;

    fd_from = open(from, O_RDONLY);
    if (fd_from < 0)
        return -1;

    fd_to = open(to, O_WRONLY | O_CREAT | O_EXCL, 0666);
    if (fd_to < 0)
        goto out_error;

    while (nread = read(fd_from, buf, sizeof buf), nread > 0)
    {
        char *out_ptr = buf;
        ssize_t nwritten;

        do {
            nwritten = write(fd_to, out_ptr, nread);

            if (nwritten >= 0)
            {
                nread -= nwritten;
                out_ptr += nwritten;
            }
            else if (errno != EINTR)
            {
                goto out_error;
            }
        } while (nread > 0);
    }

    if (nread == 0)
    {
        if (::close(fd_to) < 0)
        {
            fd_to = -1;
            goto out_error;
        }
        ::close(fd_from);

        /* Success! */
        return 0;
    }

  out_error:
    saved_errno = errno;

    ::close(fd_from);
    if (fd_to >= 0)
        ::close(fd_to);

    errno = saved_errno;
    return -1;
}

int installationPage::OSCopyFile(const char* source, const char* destination)
{    
    int input, output;    
    if ((input = open(source, O_RDONLY)) == -1)
    {
		cerr << "ERROR COPYNG FILE 1" << destination << endl << flush;
        return -1;
    }    
    if ((output = open(destination, O_WRONLY | O_CREAT | O_EXCL)) == -1)
    {
		cerr << "ERROR COPYNG FILE 2 : " << destination << endl << flush;
        ::close(input);
        return -1;
    }

    //Here we use kernel-space copying for performance reasons
    //sendfile will work with non-socket output (i.e. regular file) on Linux 2.6.33+
    off_t bytesCopied = 0;
    struct stat fileinfo = {};
    fstat(input, &fileinfo);
    int result = sendfile(output, input, &bytesCopied, fileinfo.st_size);
    
    ::close(input);
    ::close(output);
    
    chown(destination, fileinfo.st_uid, fileinfo.st_gid);

    return result;
}

int installationPage::installPackages()
{
    QVector<meskPackage>    packages = future.result();
    QVector<QString>    packagesScripts;

    cout << "pakcages size : " << packages.size() << endl;
    //QEventLoop* eventLoop = new QEventLoop(this->parent());
    
    QDir dirMaker;

    for (int i=0, pcount = packages.count(); i < pcount; i++)
    {
        emit installationStatus(tr("installing package: ")+packages.at(i).Name+"\n"+tr("size: ")+packages.at(i).strSize +
                  "\n (" + QString::number(i+1) + "/" + QString::number(packages.size()) + ")");

        for ( int k=0, count=packages.at(i).packageFiles.count(); k < count; k++)
        {
            QString file = packages.at(i).packageFiles.at(k).trimmed();
            //if (file.at(file.size()-1) == '/' )
            
            struct stat st = {};
            
            if (lstat(file.toStdString().c_str(), &st) == -1)
            {
				cerr << "PATH NOT FOUND :" << file.toStdString() << endl << flush;
			}
			
			if(S_ISLNK(st.st_mode)) //link
            {
				if (file == "/bin")
				{
					//cerr << "IT IS LINK :" << file.toStdString() << endl << flush;
				}
				//read the original link
				char resolvedPath[MAXPATHLEN];
				//char buf[1024];
				//if (readlink(file.toStdString().c_str(), buf, sizeof(buf)-1) == -1)
				if (realpath(file.toStdString().c_str(), resolvedPath) == NULL)
				{
					//cerr << "ERROR READING LINK :" << file.toStdString() << endl << flush;
					continue;
				}
				//create the new link
				if(symlink (resolvedPath, (installationRoot + file).toStdString().c_str()) != 0)
				{
					//cerr << "ERROR CREATING LINK :" << (installationRoot + file).toStdString() << endl << flush;
					continue;
				}
				// get all chown stuff
			}
			else if(S_ISDIR(st.st_mode)) //dir 
			{
				if (file == "/bin")
				{
					//cerr << "IT IS DIR :" << file.toStdString() << endl << flush;
				}
				
                QString filepath = installationRoot+file;
                //cout << "DIR : " << file.toStdString() << flush << endl;
                st={};
				if (stat(filepath.toStdString().c_str(), &st) == -1)
				{
					stat(file.toStdString().c_str(), &st);
					//chmod(filepath.toStdString().c_str(), st.st_mode);
					if (mkdir(filepath.toStdString().c_str(), st.st_mode) != 0)
					{
						//cerr << "ERROR CREATING DIR :" << filepath.toStdString() << endl << flush;
					}
					chown(filepath.toStdString().c_str(), st.st_uid, st.st_gid);
				}

                /*if (dirMaker.exists(filepath) == false)
                {
                    //filesCopier.execute("mkdir " + filepath );
                    dirMaker.mkpath(filepath);
                    struct stat st;
					stat(file.toStdString().c_str(), &st);
					chmod(filepath.toStdString().c_str(), st.st_mode);
					chown(filepath.toStdString().c_str(), st.st_uid, st.st_gid);
                }*/
                
                //TODO: place them outside so that we make sure it is a recover if this is over installation
                //filesCopier.execute("chmod --reference=" + file + " " + filepath );
                //filesCopier.execute("chown --reference=" + file + " " + filepath );
                ////////filesCopier.waitForFinished();
            }
            else
            {
                //filesCopier.execute("cp -af " + file + " " + installationRoot + file.mid(0, file.lastIndexOf('/')) );
                //filesCopier.execute("cp --preserve=all " + file + " " + installationRoot + file.mid(0, file.lastIndexOf('/')) );
                
                //if (OSCopyFile(file.toStdString().c_str(), (installationRoot + file.mid(1, file.size())).toStdString().c_str()) == -1)
                if (cp((installationRoot + file).toStdString().c_str(), file.toStdString().c_str()) == -1)
                {
					//cerr << "ERROR COPYNG FILE :" << (installationRoot + file).toStdString() << endl << flush;
					continue;
				}
                //struct stat st;
                st={};
				stat(file.toStdString().c_str(), &st);
				chmod((installationRoot + file).toStdString().c_str(), st.st_mode);
				chown((installationRoot + file).toStdString().c_str(), st.st_uid, st.st_gid);
            }
        }
        
        /*if(filesCopier.readAllStandardError().size() > 0)
		{
			setStatus(tr("Couldn't install package : ") + packages.at(i).Name,ERROR);
			//setStatus(tr("Couldn't copy file : ") + file + tr(" to : ") + installationRoot, ERROR);
			return 1;
		}*/

		
		QMetaObject::invokeMethod( installationProgressBar, "setValue", Qt::QueuedConnection,
                                  Q_ARG( int, (i+1)*80/pcount ) );
        //installationProgressBar->setValue((i*80)/packages.at(i).packageFiles.count());
        
        //connect(this, SIGNAL(signal()), _receiver, slot);
        //emit signal();
        //disconnect(this, SIGNAL(signal()), _receiver, slot);

        //eventLoop->processEvents();
    }
    
    
    return 0;
    
    QProcess        filesCopier;

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

    filesCopier.execute("/usr/bin/mkdir " + installationRoot + "/install/" );
    filesCopier.waitForFinished();

    filesCopier.execute("/usr/bin/cp /install/localpkgs.setup " + installationRoot + "/install/" );
    filesCopier.execute("/usr/bin/chmod +x " + installationRoot + "/install/localpkgs.setup" );
    filesCopier.waitForFinished();    
    Root->exec("/usr/bin/bash /install/localpkgs.setup");
    

    filesCopier.execute("/usr/bin/cp -af /var/lib/pacman/sync/ " + installationRoot + "/var/lib/pacman/sync/" );
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
    
    Root->prepare();
    Root->exec("/sbin/depmod");

    //chroot  /usr/bin/mkinitcpio -p linux
    //QDir bootDir(installationRoot+"/boot/");    
    QDir bootDir(installationRoot+"/usr/lib/modules/");
    //QStringList filters;
    //filters << "vmlinuz*";
    //bootDir.setNameFilters(filters);

    for ( int i =0; i < bootDir.entryList().size(); i++)
    {
		if( (bootDir.entryList().at(i) == ".") || (bootDir.entryList().at(i) == ".." ) )
		{
			continue;
		}

        if(bootDir.entryList().at(i).contains("extramodules"))
        {
            continue;
        }
        
        if(bootDir.entryList().at(i).contains("ARCH"))
        {
			Root->exec("/usr/bin/mkinitcpio -k " + bootDir.entryList().at(i) + " -g /boot/initramfs-linux.img ", 600000);
		}
		else
		{
			Root->exec("/usr/bin/mkinitcpio -k " + bootDir.entryList().at(i) + " -g /boot/initramfs-" + bootDir.entryList()[i].remove(QRegExp("*-")) + ".img ", 600000);
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
    QProcess        filesCopier;
    filesCopier.execute("/usr/bin/mv " + installationRoot +"/etc/locale.gen " + installationRoot + "/etc/locale.gen.install");
    filesCopier.waitForFinished();

    localesfile.setFileName(installationRoot+"/etc/locale.gen");
    localesfile.open(QIODevice::WriteOnly | QIODevice::Append);
    localesfile.write(locales.toUtf8());
    localesfile.flush();
    localesfile.close();

    return Root->exec("/usr/bin/locale-gen");
}

int installationPage::setTimeZone()
{
	emit installationStatus(tr("setting system timezone"));
	
	QProcess        filesCopier;

	timePage* tpage = (timePage*)getDependency("Time");
	//ln -sf /usr/share/zoneinfo/your/zone /etc/localtime
	filesCopier.execute("/usr/bin/ln -sf /usr/share/zoneinfo/" + tpage->getTimeZone() + " " + installationRoot + "/etc/localtime");
	filesCopier.waitForFinished();

    Root->exec("/usr/bin/timedatectl set-timezone "+ tpage->getTimeZone());

    if(tpage->isLocalTime())
    {
        Root->exec("/usr/bin/timedatectl set-local-rtc true");
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

	QProcess        filesCopier;
	
    filesCopier.execute("/usr/bin/cp -rf /install/overlay/. " + installationRoot);
    filesCopier.waitForFinished(9900000);

	filesCopier.execute("/usr/bin/mkdir " + installationRoot+"/install/");
	filesCopier.waitForFinished();

    filesCopier.execute("/usr/bin/cp -r /install/postpackagesrepo/ " + installationRoot+"/install/");
    filesCopier.waitForFinished(9900000);

    filesCopier.execute("/usr/bin/cp -r /install/postpackages/ " + installationRoot+"/install/");
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
    Root->exec("/usr/bin/fc-cache");
	return 0;
}
