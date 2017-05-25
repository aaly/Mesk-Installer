/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef INSTALLATIONPAGE_HPP
#define INSTALLATIONPAGE_HPP

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/param.h>

#include <MPF/Pages/pageBase.hpp>
#include "ui_installationPage.h"
#include <MPF/System/chroot.hpp>
#include <QFuture>
#include <QFutureWatcher>
#include "ui_installationPage.h"
#include <Pages/packagesPage.hpp>
#include <Pages/diskPage.hpp>
#include <Pages/timepage.hpp>

#include <QTimer>
#include <QPixmap>

#include <QtWidgets/QMessageBox>
#include <QtConcurrent/QtConcurrentRun>







class installationPage : public pageBase, private Ui::installationPage
{
    Q_OBJECT
    
public:
    explicit installationPage(QWidget *parent = 0);
    ~installationPage();
    int     initAll();
    
private:


	int OSCopyFile(const char* source, const char* destination);
	int cp(const char *to, const char *from);
    int generateKernel();
    int generateLocales();
	int setTimeZone();
    int installPackages();
    int preparePackagesList();
	int generateMachineID();
	int copyPostPackages();

    int     initSlideShow();
    int slideStep;
    QVector<QPixmap> slides;
    QString slidesDir;

    QTimer* timer;

    QString installationRoot;
    CHRoot*      Root;
    //QProcess        filesCopier;

    void changeEvent(QEvent* event);

    QFuture< QVector<meskPackage> > future;
    QFuture< int > future2;
    QFutureWatcher< QVector<meskPackage> > futureWatcher;
    QFutureWatcher< int > futureWatcher2;


private slots:
    int     slide();

    int install();
    int installSystem();
    int finishInstallation();
    int             setAction(const QString&);


signals:
    void installationProgress(int);
    void installationStatus(QString);

};

#endif // INSTALLATIONPAGE_HPP
