#ifndef USERSPAGE_HPP
#define USERSPAGE_HPP

#include <MPF/Pages/pageBase.hpp>
#include "ui_usersPage.h"
#include <MPF/Delegates/checkBoxList.hpp>
#include <MPF/System/chroot.hpp>
#include <QProcess>
#include <Pages/diskPage.hpp>


class meskUser
{
public:
    QString		    userName;
    QString		    Password;
    QString		    homeDir;
    QVector<QString>	    Groups;
    QString		    getFormattedGroups();

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
	int finishUp();
    int Clean();
private:
    QVector<meskUser>	    users;
    CheckBoxList*	    groupsList;
    bool		    userExists(const QString&);
    bool		    homeExists(const QString&);
    int			    initGroups();
    QString		    groupsFilePath;

    void changeEvent(QEvent* event);

    CHRoot croot;
    QString rootPath;

    diskPage* dpage;


private slots:
    int			    addUser();
    int			    removeUser();
    int			    enableConfirmPassword();
    int			    checkConfirmPassword();
    int			    validateInfo();
    //int			    modifyCurrentGroups();
    int			    updateUserFields();
    int			    updateHomeLineEdit(const QString&);
    int             selectAllGroups();
};

#endif // USERSPAGE_HPP
