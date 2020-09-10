#include "QStorageTab.h"
#include "ui_QServerTab.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
QStorageTab::QStorageTab(QWidget *parent) : QServerTab(parent)
{
    dialogIndex = Dialog::StorageServiceIndex;
    ui->ServerPageTitle->setText("存储服务器");
}
