/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#include "statusPage.hpp"
#include "ui_statusPage.h"

statusPage::statusPage(QWidget *parent) :
	pageBase(parent)
{
	setupUi(this);
	batteryStatusLabel->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

statusPage::~statusPage()
{
}

int statusPage::initAll()
{
	pageBase::initAll();

	setStatus(tr("Finding battries"), STATUS);
	populateBatteries(batteries);

	if(batteries.size() > 0)
	{
		initBatteries();
	}
	else
	{
		initACPower();
	}

	return 0;
}

int statusPage::initACPower()
{
	batteryStatusLabel->setText("<img src=" + getApplicationFile("/Icons/acpower.png") + "> \n <br> " +
								tr("AC Power Supply 100%"));

	emit Done(true);
	return 0;
}

int statusPage::initBatteries()
{
	Battery battery = batteries.at(0);
	QString icon;
	if(battery.percentage <= 2)
	{
		icon = "battery00";
	}
	else if(battery.percentage <= 20)
	{
		icon = "battery20";
	}
	else if(battery.percentage <= 40)
	{
		icon = "battery40";
	}
	else if(battery.percentage <= 60)
	{
		icon = "battery60";
	}
	else if(battery.percentage <= 80)
	{
		icon = "battery80";
	}
	else if(battery.percentage <= 100)
	{
		icon = "battery100";
	}

	if (battery.charging)
	{
		icon += "charging";
	}
	else
	{
		setStatus(tr("Please make sure the laptop is charging to avoid installation cut offs"), WARNING);
	}

	icon = getApplicationFile("/Icons/"+icon+".png");

	/*
	QString charging;

	if(battery.charging)
	{
		charging = "Charging : √";
	}
	else
	{
		charging = "Charging : X";
		setStatus(tr("Please make sure the laptop is charging to avoid installation cut offs"), WARNING);
	}*/
	batteryStatusLabel->setText("<img src=" + icon + "> \n <br> " +
								QString::number(battery.percentage)+"% <br>");
	emit Done(true);
	return 0;
}

bool statusPage::hasBatteries()
{
	return batteries.size();
}
