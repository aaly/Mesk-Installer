/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef CONFIGURATIONPAGE_HPP
#define CONFIGURATIONPAGE_HPP

#include <MPF/Pages/pageBase.hpp>
#include "ui_configurationPage.h"
#include <MPF/System/chroot.hpp>
#include <Pages/diskPage.hpp>

class configurationPage : public pageBase, private Ui::configurationPage

{
	Q_OBJECT
	
public:
	explicit configurationPage(QWidget *parent = 0);
	~configurationPage();
	int initAll();

public slots:
    int finishUp();
	
private slots:
	int checkRootPasswordStrength();
	int confirmRootPassword();

private :

	void changeEvent(QEvent* event);
	CHRoot croot;
	diskPage* dpage;
    //QList<Drive> drives;


	int configSystem();

	QString rootPath;


	QProcess mountRoot;
};

#endif // CONFIGURATIONPAGE_HPP
