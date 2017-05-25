/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef BOOTLOADERPAGE_HPP
#define BOOTLOADERPAGE_HPP

#include <QtWidgets/QWidget>
//#include <MPF/pageBase.hpp>
#include "ui_bootloaderPage.h"
#include <MPF/System/drive.hpp>
#include <Pages/diskPage.hpp>
#include <MPF/System/chroot.hpp>


class bootEntry
{
public:
    QString title;
    QString kernel;
    QString kernelOptions;
    QString initramfs;
    QString options;
    QString root;
    QString os;
    QPair<int, int> rootpartition;
	QString rootUUID;
	QString rootDevPath;
	bool  sub;
	//QVector<bootEntry> subEntries;
private:

};


class bootloaderPage : public pageBase, private Ui::bootloaderPage
{
    Q_OBJECT
    
public:
	explicit bootloaderPage(QWidget *parent = 0);

    int     initAll();
	~bootloaderPage();

    
private:
    void changeEvent(QEvent* event);
    chroot croot;
    QFile bootMenuFile;
    diskPage* dpage;
    QList<Drive> drives;


    QVector<bootEntry> bootMenu;

    int generateBootMenu();
    int installBootLoader();

    QString rootPath;


    QProcess mountRoot;

    int writeBootMenu();
    QString cfgOptions;
	QString bootPartition;

	int generateFsTab();

    int initMenu();

    QString rootdev;


private slots:



    int checkBootloaderPasswordStrength();
    int confirmBootloaderPassword();


    int removeBootEntry();
    int addBootEntry();
    int resetBootMenu();

    int modifyBootEntry(int, int);

    int addToBootMenu();

	int changeOptions();

public slots:
    int     finishUp();


};



#endif // BOOTLOADERPAGE_HPP
