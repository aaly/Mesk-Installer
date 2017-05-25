/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "timepage.hpp"
#include "ui_timepage.h"
//#include <QLocale>
#include <QFile>
#include <QTextStream>

#include <QDir>

timePage::timePage(QWidget *parent) :
    pageBase(parent)
{
    Ui_timePage::setupUi(this);

    localeFilePath = "/etc/locale.gen";

    layout()->setAlignment(Qt::AlignLeft);

    localeListWidget->setSelectionMode(QAbstractItemView::MultiSelection);
    init= false;

    timeEdit->setTime(QTime::currentTime());
    timeEdit->setEnabled(false);

}


int timePage::initAll()
{
    pageBase::initAll();
	setStatus(tr("Loading locales and time zones"), BUSY);

    loadLocales();
    loadTimeZones();
    emit Ready();
	//emit Done(true);

    localeListWidget->setLayoutDirection(Qt::LeftToRight);
	//countryListWidget->setLayoutDirection(Qt::LeftToRight);
    timeZoneListWidget->setLayoutDirection(Qt::LeftToRight);

	setDone(true);
	return 0;
}

#include <iostream>

using namespace std;

QString timePage::getDate()
{
    QString datestring = QString("%1%2%3%4%5").arg(QString::number(calendarWidget->selectedDate().day()), 2, '0').arg(QString::number(calendarWidget->selectedDate().month()), 2, '0').
            arg(QString::number(timeEdit->time().hour()), 2, '0')
            .arg(QString::number(timeEdit->time().minute()), 2, '0')
            .arg(QString::number(calendarWidget->selectedDate().year()), 4, '0');
    cout << datestring.toStdString() << flush;
    return datestring;
}

QString timePage::getTimeZone()
{
    return timeZoneListWidget->currentItem()->text();
}

QString timePage::getCountry()
{
	//return countryListWidget->currentItem()->text();
	return "";
}

QString timePage::getLocales()
{
    const QList<QListWidgetItem *> locales = localeListWidget->selectedItems();
    QString retlocales;
    for ( int i=0; i < locales.size(); i++)
    {
        cout << "Adding locale : " << locales.at(i)->text().toStdString() << endl;
        retlocales += locales.at(i)->text() + "\n";
    }
    return retlocales;
}

int timePage::loadLocales()
{

	/*for ( int i=0; i <= 246; i++)
    {
        QLocale::Country c = QLocale::Country(i);
		countryListWidget->addItem(QLocale::countryToString(c));
	}*/

    //for ( int i=1; i <= 214; i++)
    //{
    //    QLocale::Language c = QLocale::Language(i);
    //    localeListWidget->addItem(QLocale::languageToString(c));
    //}


    QFile localeFile(localeFilePath);

    if (!localeFile.open(QIODevice::Text | QIODevice::ReadOnly))
    {
        return 1;
    }

    QTextStream localeFileStream(&localeFile);

    while(!localeFileStream.atEnd())
    {
        QString line = localeFileStream.readLine();

        if ( line.size() < 2 || ( line.size() > 2 && line.mid(0,2) == "# ") )
        {
            continue;
        }

        //if (line.mid(0,2) != "# ")
        if (line.at(0) == '#' && line.at(1) != '#')
        {
            //localeListWidget->addItem(line.mid(1).remove(' '));
            localeListWidget->addItem(line.mid(1));
        }
        else if (line.at(1) == ' ') // isn;t needed at all since i patched glibc ?
        {
            localeListWidget->addItem(line.mid(2));
        }
        else
        {
            //localeListWidget->addItem(line.remove(' '));
            localeListWidget->addItem(line);
            localeListWidget->item(localeListWidget->count()-1)->setSelected(true);
        }
    }

    return 0;
}



int timePage::loadTimeZones()
{
    //grep ^TIMEZONE /etc/rc.conf
    // sed 's/^TIMEZONE=.*/lol/' /etc/rc.conf

    // current time zone grep TIMEZONE= /etc/rc.conf | sed 's/.*=//'

    QDir timeZoneDir;
    timeZoneDir.cd("/usr/share/zoneinfo/posix/");

    if (!timeZoneDir.exists())
    {
		setStatus("Ooops , Couldn't find the time Zone info directory : /usr/share/zoneinfo/", ERROR);
        return 1;
    }

    QString continentName("");

    for ( unsigned int continent =0; continent < timeZoneDir.count(); continent++)
    {
        continentName = timeZoneDir.entryList().at(continent);


       /* if (continentName != "Africa" || continentName != "America" || continentName != "Asia" ||
            continentName != "Europe" || continentName != "Antarctica" || continentName != "Australia")
        {
            continue;
        }*/


        if(continentName == "." || continentName == "..")
        {
            continue;
        }

        if (!timeZoneDir.cd("/usr/share/zoneinfo/posix/"+continentName))
        {
            continue;
        }


        for ( unsigned int country = 0; country < timeZoneDir.count(); country++)
        {
            if (timeZoneDir.entryList().at(country) == "." ||
                timeZoneDir.entryList().at(country) == "..")
            {
                continue;
            }

            timeZoneListWidget->addItem(continentName+"/"+timeZoneDir.entryList().at(country));
        }
        timeZoneDir.cdUp();
    }

	timeZoneListWidget->setCurrentRow(0);

    return 0;
}

int timePage::saveLocales()
{
    return 0;
}

bool timePage::isUTCTime()
{
    return UTCCheckBox->isChecked();
}

bool timePage::isLocalTime()
{
    return !UTCCheckBox->isChecked();
}

timePage::~timePage()
{
}



void timePage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
        Ui_timePage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}

