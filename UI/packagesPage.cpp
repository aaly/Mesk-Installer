#include "packagesPage.h"
#include "ui_packagesPage.h"

packagesPage::packagesPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::packagesPage)
{
    ui->setupUi(this);
}

packagesPage::~packagesPage()
{
    delete ui;
}
