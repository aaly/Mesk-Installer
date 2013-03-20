/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include <QtGui/QApplication>
#include <MPF/MPF.h>
#include <QTranslator>

#include <MPF/Pages/pageBase.hpp>
#include <Pages/packagesPage.hpp>
#include <Pages/diskPage.hpp>
#include <Pages/licensePage.hpp>
#include <Pages/releasePage.hpp>
//#include <Pages/userspage.hpp>
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

	qtTranslator = new QTranslator();
	qtTranslator->load(getApplicationFile("/Translations/")+lang);
	a.installTranslator(qtTranslator);


	MPF w;


	/*languagesPage* langspage = new languagesPage(&w);
	langspage->pageName = QApplication::tr("Installation Language");
	langspage->pageHelpMessage = QApplication::tr("Please choose your desired language");
	langspage->pageGroup = "Startup";
	langspage->pageIcon = getApplicationFile("/Icons/Languages.png");
	w.addPage(langspage);*/

	releasePage* rpage = new releasePage(&w);
	rpage->pageName = QApplication::tr("Release");
	rpage->pageHelpMessage = QApplication::tr("Please review Release");
	rpage->pageGroup = "Release";
	rpage->pageIcon = getApplicationFile("/Icons/Release.png");
    w.addPage(rpage);


	licensePage* lpage = new licensePage(&w);
	lpage->pageName = QApplication::tr("License");
	lpage->pageHelpMessage = QApplication::tr("Please review License");
	lpage->pageGroup = "License";
    lpage->pageIcon = getApplicationFile("/Icons/License.png");
    w.addPage(lpage);

	statusPage* spage = new statusPage(&w);
	spage->pageName = QApplication::tr("Status");
	spage->pageHelpMessage = QApplication::tr("Please choose your desired language");
	spage->pageGroup = "Status";
	spage->pageIcon = getApplicationFile("/Icons/Languages.png");
    w.addPage(spage);


	timePage* tpage = new timePage(&w);
	tpage->pageName = QApplication::tr("Time");
	tpage->pageHelpMessage = QApplication::tr("Time review License");
	tpage->pageGroup = "Time";
	tpage->pageIcon = getApplicationFile("/Icons/Time.png");
    w.addPage(tpage);

	packagesPage* ppage = new packagesPage(&w);
	ppage->pageName = QApplication::tr("Packages");
	ppage->pageHelpMessage = QApplication::tr("Packages review License");
	ppage->pageGroup = "Packages";
	ppage->pageIcon = getApplicationFile("/Icons/Packages.png");
    w.addPage(ppage);


	diskPage* dpage = new diskPage(&w);
	dpage->pageName = QApplication::tr("Disk management");
	dpage->pageHelpMessage = QApplication::tr("Disk management review License");
	dpage->pageGroup = "Disk management";
	dpage->pageIcon = getApplicationFile("/Icons/Disks.png");
    w.addPage(dpage);

	installationPage* ipage = new installationPage(&w);
	ipage->pageName = QApplication::tr("Installation");
	ipage->pageHelpMessage = QApplication::tr("Installation");
	ipage->pageGroup = "Installation";
	ipage->pageIcon = getApplicationFile("/Icons/Installation.png");
	ipage->Depend("Time",tpage);
	ipage->Depend("Packages",ppage);
	ipage->Depend("Disk",dpage);
    w.addPage(ipage);

	//usersPage* upage = new usersPage(&w);
	//upage->pageName = QApplication::tr("Users management");
	//upage->pageHelpMessage = QApplication::tr("Users management");
	//upage->pageGroup = "Users management";
	//upage->pageIcon = getApplicationFile("/Icons/Users.png");
	//addPage(upage);

	configurationPage* cpage = new configurationPage(&w);
	cpage->pageName = QApplication::tr("Configuration");
	cpage->pageHelpMessage = QApplication::tr("Configuration");
	cpage->pageGroup = "Configuration";
	cpage->pageIcon = getApplicationFile("/Icons/Confguration.png");
    cpage->Depend("Disk",dpage);
	w.addPage(cpage);


	bootloaderPage* bpage = new bootloaderPage(&w);
	bpage->pageName = QApplication::tr("Bootloader");
	bpage->pageHelpMessage = QApplication::tr("Bootloader");
	bpage->pageGroup = "Configuration";
	bpage->pageIcon = getApplicationFile("/Icons/Bootloader.png");
	bpage->Depend("Disk",dpage);
	w.addPage(bpage);

	//bootloaderPage* blpage = new bootloaderPage(&w);
	//blpage->pageName = "BootLoader.";
	//blpage->pageHelpMessage = "Confguration";
	//blpage->pageGroup = "Confguration";
	//blpage->pageIcon = ":/Icons/Confguration.png";
	//addPage(blpage);


	loadingPage* finishedPage = new loadingPage(&w);
	finishedPage->setIcon(getApplicationFile("/Icons/done.png"));
	finishedPage->setIconEffect(FADEIN);
	finishedPage->setMessage("finished installation");
	finishedPage->setMessageEffect(FADEIN);
	finishedPage->pageName = QApplication::tr("Finish");
	finishedPage->pageHelpMessage = QApplication::tr("Finish");
	finishedPage->pageGroup = "Finish";
	finishedPage->pageIcon = getApplicationFile("/Icons/done.png");
	w.addPage(finishedPage);


    w.showFullScreen();
    //w.show();

    return a.exec();
}
