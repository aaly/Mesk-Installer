/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef PACKAGESPAGE_HPP
#define PACKAGESPAGE_HPP

#include <QtWidgets/QWidget>
#include "ui_packagesPage.h"
#include <MPF/Pages/pageBase.hpp>
#include <QDir>
#include <QFile>
#include <QProcess>

// select packages , then later calculate the total size , and get the big files list , partition , mount , copy :D

class meskPackage
{
public:
                        meskPackage();
    int                 initFilesList();

    QString             Name;
    //QString           Repo;
    QString             Description;
    unsigned long long  Size;
    QString             strSize;
    bool                Critical;
    bool                Remove;
    QString             packageFilePath;
    QVector<QString>    packageFiles;
    QVector<QString>    Depends;
    QVector<QString>    Dependencies;
    QVector<QString>	currentDependencies;
    int					dependsCounter;
};

class packagesPage : public pageBase, private Ui::packagesPage
{
    Q_OBJECT
public:
    explicit packagesPage(QWidget *parent = 0);
    long long               calculateTotalSize();
    int                     initAll();
    QVector<meskPackage>    getSelectedPackages();
    long long               getTotalSize();
private:
    int             initPackages();
    int             initPackagesLists();
    int             initListViews();

    meskPackage     parsePackageFile(const QString&);
    QString         packagesListDirPath;
    QVector< QPair<QString, QVector<meskPackage> > > packages;
    QVector< QPair<QListWidgetItem*, QVector<QListWidgetItem*> > > packagesList;
    long long       totalSize;

    
    
    
    
    
    const meskPackage   getMeskPackage(QString);
    meskPackage*		findPackage(QString package);
    
    int     solveDepsChecked(const meskPackage);
    int		solveDepsCheckedGenerateList(const meskPackage package);
    
    int     solveDepsUnChecked(const meskPackage);
    int		solveDepsUnCheckedGenerateList(const meskPackage package);
    
    int     checkDepPackage(QString);
    int     uncheckDepPackage(QString);
    
    QVector<QString> checkPackages;
    QVector<QString> unCheckPackages;
    
    
    

    //int     fixCriticalsDeps(QString);
    int     fixCriticalsDeps(QString, QString = "");
    int     fixCriticals();

    int     initPackageLists();

    int     solveDepsCheckedList();

    atomic<bool>    solvingCheckedDeps;

    int     addSelectedPackage(meskPackage);

    void changeEvent(QEvent* event);
    
    
    






signals:

private slots:
    int     resetLists();
    //int     updatePackageDescription(QListWidgetItem*,QListWidgetItem*);
    int     updatePackageDescription(int);
    //int     updatePackageList(QListWidgetItem*,QListWidgetItem*);
    int     updatePackageList(int);
    int     updatePackageList(QListWidgetItem*);
    int     updatePackageDeps(QListWidgetItem*);
    int     setAllSelection();
    int     unsetAllSelection();

};

#endif // PACKAGESPAGE_HPP

