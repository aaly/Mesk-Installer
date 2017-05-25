#include "Pages/userspage.hpp"
#include <iostream>
#include <QMessageBox>
#include <QFile>

using namespace std;

QString meskUser::getFormattedGroups()
{
    QString grps;

    for (int i =0; i < Groups.count(); i++)
    {
        grps.append(Groups.at(i));
        if( i != Groups.count() -1)
        {
            grps.append(",");
        }
    }

    return grps;
}

usersPage::usersPage(QWidget* parent) : pageBase(parent)
{
    Ui_usersPage::setupUi(this);

    pageName = tr("Users Page");
    pageHelpMessage = tr("");

	groupsFilePath = "/etc/group";

    groupsList = new CheckBoxList(this);
    groupsHorizontalLayout->insertWidget(1, groupsList, 3);
    groupsLabel->setBuddy(groupsList);

    groupsList->setAutoUpdateDisplayText(true);

    passwordLineEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordLineEdit->setEchoMode(QLineEdit::Password);

    usersTableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    usersTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);


    connect(addUserPushButton, SIGNAL(clicked()), this, SLOT(addUser()));
    connect(removeUserPushButton, SIGNAL(clicked()), this, SLOT(removeUser()));

    connect(passwordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(enableConfirmPassword()));
    connect(confirmPasswordLineEdit, SIGNAL(textChanged(QString)), this, SLOT(checkConfirmPassword()));

    connect(selectPushButton, SIGNAL(clicked()), this, SLOT(selectAllGroups()));

    //connect(groupsList, SIGNAL(currentIndexChanged(int)), this, SLOT(modifyCurrentGroups()));

    connect(usersTableWidget, SIGNAL(cellChanged(int,int)), this, SLOT(updateUserFields()));
    connect(userNameLineEdit, SIGNAL(textChanged(QString)), this, SLOT(updateHomeLineEdit(QString)));

	addUserPushButton->setIcon(QIcon(getApplicationFile("/Icons/addUser.png")));
	removeUserPushButton->setIcon(QIcon(getApplicationFile("/Icons/removeUser.png")));

    rootPath = "/mnt/root/";
}

usersPage::~usersPage()
{
}

int usersPage::initGroups()
{
    setStatus(tr("Generating Groups list"), INFORMATION);

    QFile passwdFile;

    passwdFile.setFileName(groupsFilePath);

    if(!passwdFile.exists())
    {
        setStatus(groupsFilePath + tr(" file does not exist"), ERROR);
	return 1;
    }
    else if(!passwdFile.open(QIODevice::ReadOnly))
    {
        setStatus(tr("could not open file ") + groupsFilePath, ERROR);
	return 1;
    }

    QString line;

    while (true)
    {
	line = "";
	line = QString(passwdFile.readLine().data());
	line = line.split(':').at(0);

	if (line.length() == 0)
	{
	    break;
	}

	bool Critical = false;

	if (line == "users" || line == "bin" ||
        line == "dbus" || line == "hal" ||
        line == "games" || line == "wheel" ||
        line == "audio " || line == "video" ||
        line == "power" || line == "locate")
	{
	    Critical = true;
	}


	groupsList->addItem(line, Critical);

    }

    //setStatus(tr("Groups list generated"), INFORMATION);

    return 0;
}

int usersPage::initAll()
{
    pageBase::initAll();
    initGroups();

    setDone(true);

    return 0;
}

QVector<meskUser> usersPage::getUsers()
{
    return users;
}

int usersPage::enableConfirmPassword()
{
    QString icon;
    if(passwordLineEdit->text().size() < 6)
    {
        icon = getApplicationFile("/Icons/weakpassword.png");
    }
    else if(passwordLineEdit->text().size() < 8)
    {
        icon = getApplicationFile("/Icons/mediumpassword.png");
    }
    else
    {
        icon = getApplicationFile("/Icons/strongpassword.png");
    }
    PasswordLabel->setText("<img src="+icon+ " height=20 width=20 >");

    if(confirmPasswordLineEdit->text().size() >0)
    {
        checkConfirmPassword();
    }
    return 0;
}

int usersPage::checkConfirmPassword()
{
    QString icon;
    if(passwordLineEdit->text() == confirmPasswordLineEdit->text())
    {
        icon = getApplicationFile("/Icons/right.png");
        setDone(true);
    }
    else
    {
        icon = getApplicationFile("/Icons/wrong.png");
        setDone(false);
    }
    PasswordConfLabel->setText("<img src=\""+icon+ "\" height=20 width=20 >");
    return 0;


}

int usersPage::addUser()
{
    if (!validateInfo())
    {
	meskUser user;
	user.homeDir = homeLineEdit->text();
	user.homeDirItem = new QTableWidgetItem(user.homeDir);
	user.homeDirItem->setFlags((Qt::ItemFlag)~Qt::ItemIsEditable);
	user.userName = userNameLineEdit->text();
	user.userNameItem = new QTableWidgetItem(user.userName);
	user.userNameItem->setFlags((Qt::ItemFlag)~Qt::ItemIsEditable);
	user.Password = passwordLineEdit->text();
	user.passwordItem = new QTableWidgetItem(user.Password);
	user.passwordItem->setFlags((Qt::ItemFlag)~Qt::ItemIsEditable);

	int grpsCount = groupsList->getSelectedItems().count();

	for (int  i =0; i < grpsCount; i++)
	{
	   user.Groups.push_back(groupsList->getSelectedItems().at(i));
	}

	user.groupsItem = new QTableWidgetItem(user.getFormattedGroups());

	users.push_back(user);

	int row = usersTableWidget->rowCount();
	usersTableWidget->setRowCount(usersTableWidget->rowCount()+1);


	usersTableWidget->setSortingEnabled(false);
	usersTableWidget->setItem(row, 0, users.at(row).userNameItem);
    //usersTableWidget->setItem(row, 1, users.at(row).passwordItem);
    usersTableWidget->setItem(row, 1, users.at(row).homeDirItem);
    usersTableWidget->setItem(row, 2, users.at(row).groupsItem);

	if(usersTableWidget->columnWidth(0) < user.userName.length()*3)
	{
	    usersTableWidget->setColumnWidth(0, user.userName.length()*3);
	}

	if(usersTableWidget->columnWidth(2) < user.homeDir.length())
	{
	    usersTableWidget->setColumnWidth(2,user.homeDir.length());
	}

    setStatus(tr("User added"), INFORMATION);

	return 0;

    }

    return 1;
}

int usersPage::removeUser()
{
    int row = usersTableWidget->currentRow();

    if (row < 0)
    {
        return 1;
    }

    //usersTableWidget->takeItem(row, 0);
    //usersTableWidget->takeItem(row, 1);
    //usersTableWidget->takeItem(row, 2);
    //usersTableWidget->takeItem(row, 3);
    usersTableWidget->removeRow(row);
    users.remove(row);
    //usersTableWidget->setRowCount(usersTableWidget->rowCount()-1);
    setStatus(tr("User removed"), INFORMATION);
    return 0;
}

int usersPage::validateInfo()
{
    // check for illegal tokens in the text like ; :|
    if(userExists(userNameLineEdit->text()))
    {
        setStatus(tr("Username exists"), WARNING);
	return 1;
    }
    else if(homeExists(homeLineEdit->text()))
    {
        setStatus(tr("Home path exists"), WARNING);
	return 1;
    }
    else if(passwordLineEdit->text() != confirmPasswordLineEdit->text())
    {
        setStatus(tr("Shouldn't Happen ! :s"), WARNING);
	return 1;
    }

    return 0;
}

/*int usersPage::modifyCurrentGroups()
{
    return 0;
}*/

int usersPage::updateUserFields()
{
    /*int row = usersTableWidget->currentRow();

    if (row < 0)
    {
	return 1;
    }
    QMessageBox::information(this, "test", QString::number(row));
    userNameLineEdit->setText(usersTableWidget->itemAt(row, 0)->text());
    nameLineEdit->setText(usersTableWidget->itemAt(row, 1)->text());
    homeLineEdit->setText(usersTableWidget->itemAt(row, 2)->text());

    for ( int i =0; i < groupsList->count(); i++)
    {
	groupsList->setItemData(i, false);

	for ( int j =0; j < users.at(row).Groups.count(); j++)
	{
	    if(groupsList->itemText(i) == users.at(row-1).Groups.at(j))
	    {
		groupsList->setItemData(i, true);
	    }
	}
    }

    return 0;*/

    return 0;
}

bool usersPage::userExists(const QString& username)
{
    /*for ( int i =0; i < usersTableWidget->rowCount(); i++)
    {
	if(username == usersTableWidget->itemAt(i, 0)->text())
	{
	    return true;
	}
    }*/

    for ( int i =0; i < users.count(); i++)
    {
	if( username == users.at(i).userName)
	{
	    return true;
	}
    }
    return false;
}

bool usersPage::homeExists(const QString& homedir)
{
    /*for ( int i =0; i < usersTableWidget->rowCount(); i++)
    {
	if(homedir == usersTableWidget->itemAt(i, 3)->text())
	{
	    return true;
	}
    }*/

    for ( int i =0; i < users.count(); i++)
    {
	if( homedir == users.at(i).homeDir)
	{
	    return true;
	}
    }

    return false;
}

int usersPage::updateHomeLineEdit(const QString& user)
{
    homeLineEdit->setText("/home/"+user);
    return 0;
}



void usersPage::changeEvent(QEvent* event)
{


    if (event->type() == QEvent::LanguageChange)
    {
        Ui_usersPage::retranslateUi(this);
    }

    pageBase::changeEvent(event);

}

int usersPage::Clean()
{
    usersTableWidget->setCurrentCell(0, 0);
    while(!removeUser())
    {
        if(users.size() > 0)
        {
            usersTableWidget->setCurrentCell(0, 0);
        }
    }
    return 0;
}

int usersPage::finishUp()
{
    dpage = (diskPage*)getDependency("Disk");

    QProcess passwordGenerator;
    ////passwordGenerator.start("mount " + dpage->rootPartition + " " + rootPath);
   //// passwordGenerator.waitForFinished();

    croot.setRoot(rootPath);
    croot.prepare();



    for (int i =0; i < users.size(); i++)
    {
        QString password;
        passwordGenerator.start("openssl passwd -1 " + users.at(i).Password);
        passwordGenerator.waitForFinished();
        password = passwordGenerator.readAllStandardOutput();

        QString grps;
        for (int k = 0; k < users.at(i).Groups.size(); k++)
        {
            grps += users.at(i).Groups[k];
            if(k < users.at(i).Groups.size()-1)
            {
                grps += ",";
            }
        }
        cout << QString("useradd -d "+users.at(i).homeDir + " -p " + password + " -G " + grps).toStdString() << endl;
        croot.exec("/usr/bin/useradd -m -d "+users.at(i).homeDir + " -p " + password + " -G " + grps + " " + users.at(i).userName);
    }

    ////passwordGenerator.start("umount " + rootPath);
    ////passwordGenerator.waitForFinished();

    croot.unprepare();


	return 0;
}


/*int usersPage::createUsers()
{


}*/

int usersPage::selectAllGroups()
{
    if(selectPushButton->text() == tr("Select All") )
    {
        groupsList->selectAll(true);
        selectPushButton->setText(tr("unSelect All"));
    }
    else
    {
        groupsList->selectAll(false);
        selectPushButton->setText(tr("Select All"));
    }
    return 0;
}
