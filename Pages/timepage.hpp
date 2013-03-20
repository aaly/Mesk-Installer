/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef TIMEPAGE_HPP
#define TIMEPAGE_HPP


#include <MPF/Pages/pageBase.hpp>
#include <ui_timepage.h>

class timePage : public pageBase, private Ui::timePage
{
    Q_OBJECT

public:
    explicit timePage(QWidget *parent = 0);
    int      initAll();
    QString  getDate();
    int      loadTimeZones();
    QString  getTimeZone();
    QString  getLocales();
    QString  getCountry();
    ~timePage();

private:
    int     loadLocales();
    int     saveLocales();

    QString localeFilePath;

    void changeEvent(QEvent* event);

};

#endif // TIMEPAGE_HPP
