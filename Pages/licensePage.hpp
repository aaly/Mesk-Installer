/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef LICENSEPAGE_H
#define LICENSEPAGE_H

#include <MPF/Pages/pageBase.hpp>
#include "ui_licensePage.h"

class licensePage : public pageBase, private Ui::licensePage
{
    Q_OBJECT

public:
    explicit licensePage(QWidget *parent = 0);
    ~licensePage();
    int     initAll();

private:
    QString licenseFilePath;

    void changeEvent(QEvent* event);
private slots:
    int setReady();
};

#endif // LICENSEPAGE_H
