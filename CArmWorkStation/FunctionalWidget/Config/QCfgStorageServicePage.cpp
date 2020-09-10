#include "QCfgStorageServicePage.h"
#include "ui_SystemConfigWidget.h"
#include "singleton.h"
#include "Dialog.h"
QCfgStorageServicePage::QCfgStorageServicePage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgStorageServicePage::~QCfgStorageServicePage()
{

}

void QCfgStorageServicePage::updatePage()
{

}

void QCfgStorageServicePage::savePage()
{

}

void QCfgStorageServicePage::enteryPage()
{

}

void QCfgStorageServicePage::exitPage()
{

}

void QCfgStorageServicePage::init()
{
//    connect(ui->StorageServiceAddBtn, SIGNAL(clicked()), this, SLOT(onStorageAddDialogShowSlot()));
}

void QCfgStorageServicePage::onStorageAddDialogShowSlot()
{
    Singleton<Dialog>::getInstance().showDialog(Dialog::StorageServiceAdd);
}
