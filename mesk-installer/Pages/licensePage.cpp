/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "Pages/licensePage.hpp"
#include <QFile>
#include <QTextStream>
#include <QTextCodec>
#include <QtWidgets/QMessageBox>

extern QString lang;

licensePage::licensePage(QWidget *parent) :
    pageBase(parent)
{
    setupUi(this);

    agreeCheckBox->setDisabled(true);
    licenseTextEdit->setReadOnly(true);

    connect(agreeCheckBox, SIGNAL(clicked()), this, SLOT(setReady()));

    //initAll();
}

licensePage::~licensePage()
{
}

int licensePage::initAll()
{
    pageBase::initAll();
    setStatus(tr("Loading license file"), BUSY);
    licenseFilePath = getApplicationLanguageFile("/License/License.txt");
    QFile file;
    file.setFileName(licenseFilePath);

    if (!file.open(QIODevice::ReadOnly))
    {
        setStatus(tr("Couldn't open license file : ") + licenseFilePath, ERROR);
        return 1;
    }


    licenseTextEdit->setText(file.readAll());

    agreeCheckBox->setEnabled(true);
    file.close();
    emit Ready();

    return 0;
}

int licensePage::setReady()
{
    if (agreeCheckBox->isChecked())
    {
		setDone(true);
    }
    else
    {
		setDone(false);
    }
    return 0;
}


void licensePage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
        Ui_licensePage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}
