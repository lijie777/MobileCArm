#include "QCfgUserManagerPage.h"
#include "ui_SystemConfigWidget.h"

QCfgUserManagerPage::QCfgUserManagerPage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgUserManagerPage::~QCfgUserManagerPage()
{

}

void QCfgUserManagerPage::updatePage()
{

}

void QCfgUserManagerPage::savePage()
{

}

void QCfgUserManagerPage::enteryPage()
{

}

void QCfgUserManagerPage::exitPage()
{

}

void QCfgUserManagerPage::init()
{
    ui->UserTableWidget->setColumnWidth(0, 80);
    ui->UserTableWidget->setColumnWidth(1, 147);

    connect(ui->userAddBtn, &QPushButton::clicked, this, &QCfgUserManagerPage::userAddSlot);
    connect(ui->userDeleteBtn, &QPushButton::clicked, this, &QCfgUserManagerPage::userDeleteSlot);
    connect(ui->userEditBtn, &QPushButton::clicked, this, &QCfgUserManagerPage::userEditSlot);
    connect(ui->userDeleteAllCheckBtn, &QPushButton::clicked, this, &QCfgUserManagerPage::userDeleteAllCheckSlot);
}

void QCfgUserManagerPage::userAddSlot()
{
    emit Singleton<Dialog>::getInstance().showDialog(Dialog::UserAdd);
}

void QCfgUserManagerPage::userDeleteSlot()
{

}

void QCfgUserManagerPage::userEditSlot()
{

}

void QCfgUserManagerPage::userDeleteAllCheckSlot()
{

}
