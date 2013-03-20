/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef STATUSPAGE_HPP
#define STATUSPAGE_HPP

#include <MPF/Pages/pageBase.hpp>
#include "ui_statusPage.h"
#include <sysfs/libsysfs.h>
#include <MPF/System/drive.hpp>

class Battery
{
public:
	QString name;
	QString model;
	QString full;
	QString now;
	int percentage;
	bool charging;
};

class statusPage : public pageBase, private Ui::statusPage
{
	Q_OBJECT
	
public:
	explicit statusPage(QWidget *parent = 0);
	~statusPage();
	int initAll();
	bool hasBatteries();

private:
	QVector<Battery> batteries;

	int initACPower();
	int initBatteries();
};





/*


static inline QString getSysfsAtrribute(sysfs_class_device *clsdev, QString attribute)
{
	dlist* dList = sysfs_get_classdev_attributes(clsdev);
	int counter = 0;

	if (dList)
	{
		counter = dList->count;
	}
	else
	{
		return "";
	}

	dList->marker = &dList->headnode;

	while (counter != 0)
	{
		counter--;
		DL_node* node = 0;
		node = dList->marker;
		dlist_next(dList);
		if(node->data == 0 ||
		   node->data == NULL)
		{
			dlist_next(dList);
			continue;
		}
		sysfs_attribute* attr = (sysfs_attribute*)node->data;

		if (!strcmp(attr->name,attribute.toAscii()))
		{
			return QString(attr->value);
		}
		delete attr;
	}

	return "";
}*/


static inline int populateBatteries(QVector<Battery>& batteries)
{
	sysfs_class* sysfsClass;
	sysfsClass = sysfs_open_class("power_supply");

	dlist* dList = sysfs_get_class_devices(sysfsClass);

	int counter = 0;

	if (dList)
	{
		counter = dList->count;
	}
	else
	{
		return 1;
	}

	if(counter == 0)
	{
		return -1;
	}

	//dList->marker = &dList->headnode;
	//dlist_start(dList);
	DL_node* node = 0;
	node = &dList->headnode;
	cout << "COUNTER : " << counter << endl;
	while (counter != 0)
	{
		node = node->next;
		if(node->data == 0 ||
		   node->data == NULL)
		{
			cout << " NULL LLL " << endl;
			//dlist_next(dList);
			continue;
		}

		--counter;

		cout << "COUNTER: " << counter << endl << flush;

		if ( getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed() != "Battery" )
		{
			cout << "nota battery " << getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed().toStdString() << endl;
			continue;
		}

		cout << "battery: " << getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed().toStdString() << endl;

		sysfsClass = (sysfs_class*)node->data;


		Battery bat;

		cout << "battery2: " << getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed().toStdString() << endl << flush;

		bat.name = sysfsClass->name;

		cout << "battery3: " << getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed().toStdString() << endl << flush;
		bat.model = getSysfsAtrribute((sysfs_class_device*)node->data, "model_name").trimmed();
		bat.now = getSysfsAtrribute((sysfs_class_device*)node->data, "charge_now").trimmed();
		bat.full = getSysfsAtrribute((sysfs_class_device*)node->data, "charge_full").trimmed();
		cout << "battery4: " << getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed().toStdString() << endl << flush;
		//bat.percentage = bat.now.toInt() *100 / bat.full.toInt();
		bat.percentage = getSysfsAtrribute((sysfs_class_device*)node->data, "capacity").trimmed().toInt();
		QString charging = getSysfsAtrribute((sysfs_class_device*)node->data, "status").trimmed();
		if(charging == "Full" || charging == "Charging")
		{
			bat.charging = true;
		}
		else
		{
			bat.charging = false;
		}

		cout << "battery5: " << getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed().toStdString() << endl << flush;
		batteries.push_back(bat);
		cout << "battery6: " << getSysfsAtrribute((sysfs_class_device*)node->data, "type").trimmed().toStdString() << endl << flush;

	}
	return 0;
}


#endif // STATUSPAGE_HPP
