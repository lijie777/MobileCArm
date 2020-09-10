#include "QQREditDialog.h"
#include "ui_QQueryRetrieveAddDialog.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
QQREditDialog::QQREditDialog(QWidget *parent) : QQueryRetrieveAddDialog(parent)
{
    ui->dialogTitle->setText("编辑Query&Retrieve");
}
