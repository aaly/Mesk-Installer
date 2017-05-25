/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef RELEASEPAGE_H
#define RELEASEPAGE_H

#include <MPF/Pages/pageBase.hpp>
#include "ui_releasePage.h"

#include <QFile>

class releasePage : public pageBase, private Ui::releasePage
{
    Q_OBJECT

public:
    explicit releasePage(QWidget *parent = 0);
    ~releasePage();
    int     initAll();

private:
    QString releaseFilePath;

    void changeEvent(QEvent* event);

private slots:
};

#endif // RELEASEPAGE_H
