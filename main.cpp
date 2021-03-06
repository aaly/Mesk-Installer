/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include <QtWidgets/QApplication>
#include <MPF/MPF.h>
#include <QTranslator>
#include <QDesktopWidget>

#include <MPF/Pages/pageBase.hpp>
#include <MPF/Pages/groupSelector.hpp>
#include <Pages/packagesPage.hpp>
#include <Pages/diskPage.hpp>
#include <Pages/licensePage.hpp>
#include <Pages/releasePage.hpp>
#include <Pages/userspage.hpp>
//#include <Pages/loadingpage.hpp>
#include <Pages/timepage.hpp>
#include <Pages/configurationPage.hpp>
#include <Pages/bootloaderPage.hpp>
#include <Pages/installationPage.hpp>
#include <MPF/Pages/languagesPage.hpp>
#include <Pages/statusPage.hpp>

int main(int argc, char *argv[])
{
    QApplication::setDesktopSettingsAware(true);
    QApplication a(argc, argv);

    qtTranslator = new QTranslator(&a);
    qtTranslator->load(getApplicationFile("/Translations/")+lang);
    a.installTranslator(qtTranslator);

	pagesGroup* installationGroup = new pagesGroup();
    installationGroup->setTitle(QApplication::translate("Groups", "Installation", 0));
	installationGroup->setIcon(getApplicationFile("/Icons/Installation.png"));

	pagesGroup* recoveryGroup = new pagesGroup();
    recoveryGroup->setTitle(QApplication::translate("Groups", "Recovery", 0));
	recoveryGroup->setIcon(getApplicationFile("/Icons/Recovery.png"));

    pagesGroup* bootloaderRecoveryGroup = new pagesGroup();
    bootloaderRecoveryGroup->setTitle(QApplication::translate("Groups", "Bootloader",  0));
    bootloaderRecoveryGroup->setIcon(getApplicationFile("/Icons/Bootloader.png"));

    pagesGroup* configurationRecoveryGroup = new pagesGroup();
    configurationRecoveryGroup->setTitle(QApplication::translate("Groups", "Configuration", 0));
    configurationRecoveryGroup->setIcon(getApplicationFile("/Icons/Confguration.png"));

    pagesGroup* usersRecoveryGroup = new pagesGroup();
    usersRecoveryGroup->setTitle(QApplication::translate("Groups", "Users", 0));
    usersRecoveryGroup->setIcon(getApplicationFile("/Icons/Users.png"));

    //pagesGroup* timeRecoveryGroup = new pagesGroup();
    //timeRecoveryGroup->setTitle(QApplication::tr("Time"));
    //timeRecoveryGroup->setIcon(getApplicationFile("/Icons/Time.png"));



	/*languagesPage* langspage = new languagesPage();
	langspage->pageName = QApplication::tr("Installation Language");
	langspage->pageHelpMessage = QApplication::tr("Please choose your desired language");
	langspage->pageGroup = "Startup";
	langspage->pageIcon = getApplicationFile("/Icons/Languages.png");
	QObject::connect(langspage, SIGNAL(selectedLanguage(QString)), &w, SLOT(updateLayout(QString)));
	installationGroup->addPage(langspage);
	recoveryGroup->addPage(langspage);*/

	releasePage* rpage = new releasePage();
    rpage->pageName = QApplication::translate("Pages", "Release", 0);
	rpage->pageHelpMessage = QApplication::tr("Please review Release");
	rpage->pageGroup = "Release";
	rpage->pageIcon = getApplicationFile("/Icons/Release.png");
	installationGroup->addPage(rpage);
	recoveryGroup->addPage(rpage);


	/*licensePage* lpage = new licensePage();
    lpage->pageName = QApplication::translate("Pages", "License", 0);
	lpage->pageHelpMessage = QApplication::tr("Please review License");
	lpage->pageGroup = "License";
    lpage->pageIcon = getApplicationFile("/Icons/License.png");
	installationGroup->addPage(lpage);*/

	statusPage* spage = new statusPage();
    spage->pageName = QApplication::translate("Pages", "Power Status", 0);
	spage->pageHelpMessage = QApplication::tr("Please Make sure your have enough battery or DC power is plugged in");
	spage->pageGroup = "Status";
	spage->pageIcon = getApplicationFile("/Icons/Power.png");
	installationGroup->addPage(spage);


	timePage* tpage = new timePage();
    tpage->pageName = QApplication::translate("Pages", "Time", 0);
	tpage->pageHelpMessage = QApplication::tr("Time review License");
	tpage->pageGroup = "Time";
	tpage->pageIcon = getApplicationFile("/Icons/Locale.png");
	installationGroup->addPage(tpage);

	packagesPage* ppage = new packagesPage();
    ppage->pageName = QApplication::translate("Pages", "Packages", 0);
	ppage->pageHelpMessage = QApplication::tr("Packages");
	ppage->pageGroup = "Packages";
	ppage->pageIcon = getApplicationFile("/Icons/Packages.png");
	installationGroup->addPage(ppage);


	diskPage* dpage = new diskPage();
    dpage->pageName = QApplication::translate("Pages", "Disk management", 0);
	dpage->pageHelpMessage = QApplication::tr("Disk management review License");
	dpage->pageGroup = "Disk management";
	dpage->pageIcon = getApplicationFile("/Icons/Disks.png");
	installationGroup->addPage(dpage);
	recoveryGroup->addPage(dpage);


    installationPage* ipage = new installationPage();
    ipage->pageName = QApplication::translate("Pages", "Installation", 0);
	ipage->pageHelpMessage = QApplication::tr("Installation");
	ipage->pageGroup = "Installation";
	ipage->pageIcon = getApplicationFile("/Icons/Installation.png");
	ipage->Depend("Time",tpage);
	ipage->Depend("Packages",ppage);
	ipage->Depend("Disk",dpage);
	installationGroup->addPage(ipage);
    //timeRecoveryGroup->addPage(ipage);

    usersPage* upage = new usersPage();
    upage->pageName = QApplication::translate("Pages", "Users management", 0);
    upage->pageHelpMessage = QApplication::tr("Users management");
    upage->pageGroup = "Users management";
    upage->pageIcon = getApplicationFile("/Icons/Users.png");
    upage->Depend("Disk",dpage);
    installationGroup->addPage(upage);
    usersRecoveryGroup->addPage(upage);


	configurationPage* cpage = new configurationPage();
    cpage->pageName = QApplication::translate("Pages", "Configuration", 0);
	cpage->pageHelpMessage = QApplication::tr("Configuration");
	cpage->pageGroup = "Configuration";
	cpage->pageIcon = getApplicationFile("/Icons/Confguration.png");
    cpage->Depend("Disk",dpage);
	installationGroup->addPage(cpage);
    configurationRecoveryGroup->addPage(cpage);

    bootloaderPage* bpage = new bootloaderPage();
    bpage->pageName = QApplication::translate("Pages", "Bootloader", 0);
    bpage->pageHelpMessage = QApplication::tr("Bootloader");
    bpage->pageGroup = "Configuration";
    bpage->pageIcon = getApplicationFile("/Icons/Bootloader.png");
    bpage->Depend("Disk",dpage);
    installationGroup->addPage(bpage);
    bootloaderRecoveryGroup->addPage(bpage);

	//bootloaderPage* blpage = new bootloaderPage(&w);
	//blpage->pageName = "BootLoader.";
	//blpage->pageHelpMessage = "Confguration";
	//blpage->pageGroup = "Confguration";
	//blpage->pageIcon = ":/Icons/Confguration.png";
	//addPage(blpage);

    loadingPage* finishedPage = new loadingPage();
    finishedPage->setIcon(getApplicationFile("/Icons/done.png"));
    finishedPage->setIconEffect(FADEIN);
    finishedPage->setMessage(QApplication::tr("Finished installation"));
    finishedPage->setMessageEffect(FADEIN);
    finishedPage->setIconSize(QSize(150,150));
    finishedPage->setMessageSize(60);
    finishedPage->pageName = QApplication::translate("Pages", "Finish", 0);
    finishedPage->pageHelpMessage = QApplication::tr("Finish");
    finishedPage->pageGroup = "Finish";
    finishedPage->pageIcon = getApplicationFile("/Icons/Finish.png");
    installationGroup->addPage(finishedPage);


    loadingPage* recoveryFinishedPage = new loadingPage();
    recoveryFinishedPage->setIcon(getApplicationFile("/Icons/done.png"));
    recoveryFinishedPage->setIconEffect(FADEIN);
    recoveryFinishedPage->setMessage(QApplication::tr("Finished Recovery"));
    recoveryFinishedPage->setMessageEffect(FADEIN);
    recoveryFinishedPage->setIconSize(QSize(150,150));
    recoveryFinishedPage->setMessageSize(60);
    recoveryFinishedPage->pageName = QApplication::translate("Pages", "Finish", 0);
    recoveryFinishedPage->pageHelpMessage = QApplication::tr("Finish");
    recoveryFinishedPage->pageGroup = "Finish";
    recoveryFinishedPage->pageIcon = getApplicationFile("/Icons/done.png");
    bootloaderRecoveryGroup->addPage(recoveryFinishedPage);
    configurationRecoveryGroup->addPage(recoveryFinishedPage);
    usersRecoveryGroup->addPage(recoveryFinishedPage);
    //timeRecoveryGroup->addPage(recoveryFinishedPage);


    groupSelector* recoverySelector = new groupSelector();
    recoverySelector->pageName = QApplication::translate("Pages", "Recovery Mode", 0);
    recoverySelector->pageHelpMessage = QApplication::tr("Recovery");
    recoverySelector->pageGroup = "Selector";
    recoverySelector->pageIcon = getApplicationFile("/Icons/Recovery.png");
    recoverySelector->addGroup(bootloaderRecoveryGroup);
    recoverySelector->addGroup(configurationRecoveryGroup);
    recoverySelector->addGroup(usersRecoveryGroup);
    //recoverySelector->addGroup(timeRecoveryGroup);
    recoveryGroup->addPage(recoverySelector);


	groupSelector* installationSelector = new groupSelector();
    installationSelector->pageName = QApplication::translate("Pages", "Installation mode", 0);
	installationSelector->pageHelpMessage = QApplication::tr("installation");
	installationSelector->pageGroup = "Selector";
	installationSelector->pageIcon = getApplicationFile("/Icons/installermode.png");
	installationSelector->pageType = "GROUPSELECTOR";
	installationSelector->addGroup(installationGroup);
	installationSelector->addGroup(recoveryGroup);


	pagesGroup* mainGroup = new pagesGroup();
	mainGroup->addPage(installationSelector);


	MPF w;
	installationSelector->setMPF(&w);
    recoverySelector->setMPF(&w);
	w.addGroup(mainGroup);
//	w.showFullScreen();
    //w.show();

    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    w.resize(screenGeometry.width(), screenGeometry.height());
    w.showNormal();

//	QTimer::singleShot(1000, &w, SLOT(showFullScreen()));

    return a.exec();
}
