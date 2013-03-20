/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "releasePage.hpp"
#include <QFile>

releasePage::releasePage(QWidget *parent) :
    pageBase(parent){

    Ui_releasePage::setupUi(this);

    releaseTextEdit->setReadOnly(true);
    releaseFilePath = getApplicationLanguageFile("/Release/Release.txt");
}

releasePage::~releasePage()
{
}

int releasePage::initAll()
{
    pageBase::initAll();
	emit Done(true);

    emit Status(tr("Loading release file"), BUSY);
    QFile file;
    file.setFileName(releaseFilePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        emit Status(tr("Couldn't open release file : ") + releaseFilePath, ERROR);
        return 1;
    }


    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //releaseTextEdit->setText(QString::fromUtf8(file.readAll().data()));
    releaseTextEdit->setText(file.readAll());

    file.close();



    emit Ready();
    emit Done(true);

    return 0;
}


void releasePage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
        Ui_releasePage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}
