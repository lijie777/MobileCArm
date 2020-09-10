#include "QMppsEditDialog.h"
#include "ui_QWorkListAddDialog.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
QMppsEditDialog::QMppsEditDialog(QWidget *parent) : QMppsAddDialog(parent)
{
    ui->dialogTitle->setText("编辑Mpps");
}
