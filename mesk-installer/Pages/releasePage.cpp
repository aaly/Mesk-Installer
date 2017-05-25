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
}

releasePage::~releasePage()
{
}

int releasePage::initAll()
{
    pageBase::initAll();
    
    QString content;

	setStatus(tr("Loading release file"), BUSY);
    releaseFilePath = getApplicationLanguageFile("/Release/Release.txt");

    QFile file;
    file.setFileName(releaseFilePath);

    if (!file.open(QIODevice::ReadOnly))
    {
		setStatus(tr("Couldn't open release file : ") + releaseFilePath, ERROR);
        return 1;
    }

    //QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    //releaseTextEdit->setText(QString::fromUtf8(file.readAll().data()));
    ////releaseTextEdit->setText(file.readAll());
    content = file.readAll();
    file.close();
    
    
    
    releaseFilePath = getApplicationLanguageFile("/License/License.txt");
    file.setFileName(releaseFilePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        setStatus(tr("Couldn't open license file : ") + releaseFilePath, ERROR);
        return 1;
    }
    content += file.readAll();
    //releaseTextEdit->setText(file.readAll());
    file.close();
    
    releaseTextEdit->setText(content);
    
    

    emit Ready();
	setDone(true);

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
