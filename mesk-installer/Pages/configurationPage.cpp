/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/



/*
DIALOG $DEFAULT --menu "Configuration" 17 80 10 \
           "/etc/hostname"              "${_hostnametext}" \
       "/etc/vconsole.conf"              "${_vconsoletext}" \
       "/etc/locale.conf"              "${_localetext}" \
       "/etc/adjtime"              "${_adjtimetext}" \
           "/etc/fstab"                "${_fstabtext}" \
           "/etc/mkinitcpio.conf"      "${_mkinitcpioconftext}" \
           "/etc/modprobe.d/modprobe.conf" "${_modprobeconftext}" \
           "/etc/resolv.conf"          "${_resolvconftext}" \
           "/etc/hosts"                "${_hoststext}" \
           "/etc/X11/xorg.conf.d/01-keyboard-layout.conf"		"${_xorgkblayouttext}" \
           "/etc/locale.gen"           "${_localegentext}" \
           "/etc/pacman.d/mirrorlist"  "${_mirrorlisttext}" \
           "/etc/X11/xorg.conf.d/10-evdev.conf"  "${_xorgevdevconftext}" \
           "${_return_label}"        "${_return_label}" 2>$ANSWER || CURRENT_SELECTION="${_return_label}"

*/




#include "configurationPage.hpp"
#include "ui_configurationPage.h"

configurationPage::configurationPage(QWidget *parent) :
	pageBase(parent)
{
	setupUi(this);

	connect(rootPasswordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkRootPasswordStrength()));
	connect(rootPasswordConfLineEdit, SIGNAL(textChanged(QString)), this, SLOT(confirmRootPassword()));

}

configurationPage::~configurationPage()
{
}


int configurationPage::initAll()
{
	pageBase::initAll();

    ////dpage = (diskPage*)getDependency("Disk");
    //drives =  dpage->getDisks();

    ////mountRoot.start("mount " + dpage->rootPartition + " /mnt/root");
    ////mountRoot.waitForFinished();

	rootPath = "/mnt/root/";
	croot.setRoot(rootPath);
	croot.prepare();

	return 0;
}


int configurationPage::configSystem()
{
	// set root password ...
	if(rootPasswordLineEdit->text() > 0)
	{
		setStatus(tr("Setting root password"), INFORMATION);
        mountRoot.start("openssl passwd -1 " + rootPasswordLineEdit->text());
        mountRoot.waitForFinished();
        croot.exec("usermod -p "+ mountRoot.readAllStandardOutput() + " root");
	}

    if(hostnameLineEdit->text() > 0)
    {
        setStatus(tr("Setting Hostnam"), INFORMATION);
        //croot.exec("hostnamectl set-hostname " + hostnameLineEdit->text());
        ///mountRoot.start("echo " + hostnameLineEdit->text() + " > " + rootPath+"/etc/hostname");
        ///mountRoot.waitForFinished();
        QFile hostnamefile(rootPath+"/etc/hostname");
        hostnamefile.open(QIODevice::WriteOnly);
        QTextStream hostname(&hostnamefile);
        hostname << hostnameLineEdit->text();
        hostnamefile.close();
    }

    croot.unprepare();


	return 0;
}




int configurationPage::checkRootPasswordStrength()
{
	QString icon;
	if(rootPasswordLineEdit->text().size() < 6)
	{
		icon = getApplicationFile("/Icons/weakpassword.png");
	}
	else if(rootPasswordLineEdit->text().size() < 8)
	{
		icon = getApplicationFile("/Icons/mediumpassword.png");
	}
	else
	{
		icon = getApplicationFile("/Icons/strongpassword.png");
	}
	rootPasswordLabel->setText("<img src="+icon+ " height=20 width=20 >");

	if(rootPasswordConfLineEdit->text().size() >0)
	{
		confirmRootPassword();
	}
	return 0;
}

int configurationPage::confirmRootPassword()
{
	QString icon;
	if(rootPasswordLineEdit->text() == rootPasswordConfLineEdit->text())
	{
		icon = getApplicationFile("/Icons/right.png");
		setDone(true);
	}
	else
	{
		icon = getApplicationFile("/Icons/wrong.png");
		setDone(false);
	}
	rootPasswordConfLabel->setText("<img src=\""+icon+ "\" height=20 width=20 >");
	return 0;
}


void configurationPage::changeEvent(QEvent* event)
{


	if (event->type() == QEvent::LanguageChange)
	{
		retranslateUi(this);
	}

	pageBase::changeEvent(event);

}

int configurationPage::finishUp()
{
    pageBase::finishUp();
    configSystem();
    return 0;
}
