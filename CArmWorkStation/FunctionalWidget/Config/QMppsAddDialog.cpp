#include "QMppsAddDialog.h"
#include "ui_QWorkListAddDialog.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
QMppsAddDialog::QMppsAddDialog(QWidget *parent) : QWorkListAddDialog(parent)
{
    ui->dialogTitle->setText("新增Mpps");
}
