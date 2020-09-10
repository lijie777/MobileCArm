#include "QWorkListEditDialog.h"
#include "ui_QWorkListAddDialog.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
QWorkListEditDialog::QWorkListEditDialog(QWidget *parent) : QWorkListAddDialog(parent)
{
    ui->dialogTitle->setText("编辑WorkList");
}
