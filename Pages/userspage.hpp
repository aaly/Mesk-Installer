/******************************************************
* copyright 2011, 2012, 2013 AbdAllah Aly Saad , aaly90[@]gmail.com
* Part of Mesklinux Installer
* See LICENSE file for more info
******************************************************/

#ifndef USERSPAGE_HPP
#define USERSPAGE_HPP

#include <MPF/Pages/pageBase.hpp>
#include "ui_usersPage.h"
#include <Widgets/checkBoxList.hpp>

class meskUser
{
public:
    QString		    Name;
    QString		    userName;
    QString		    Password;
    QString		    homeDir;
    QVector<QString>	    Groups;
    QString		    getFormattedGroups();

    QTableWidgetItem*	    nameItem;
    QTableWidgetItem*	    userNameItem;
    QTableWidgetItem*	    passwordItem;
    QTableWidgetItem*	    homeDirItem;
    QTableWidgetItem*	    groupsItem;
};


class usersPage : public pageBase , private Ui::usersPage
{
    Q_OBJECT
public:
    explicit usersPage(QWidget* parent = 0);
    ~usersPage();

    int			    initAll();
    QVector<meskUser>	    getUsers();
private:
    QVector<meskUser>	    users;
    CheckBoxList*	    groupsList;
    bool		    userExists(const QString&);
    bool		    homeExists(const QString&);
    int			    initGroups();
    QString		    groupsFilePath;

    void changeEvent(QEvent* event);


private slots:
    int			    addUser();
    int			    removeUser();
    int			    enableConfirmPassword(QString);
    int			    checkConfirmPassword(QString);
    int			    validateInfo();
    //int			    modifyCurrentGroups();
    int			    updateUserFields();
    int			    updateHomeLineEdit(const QString&);
};

#endif // USERSPAGE_HPP
