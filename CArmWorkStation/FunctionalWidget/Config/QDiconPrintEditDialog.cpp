#include "QDiconPrintEditDialog.h"
#include "ui_QWorkListAddDialog.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
QDiconPrintEditDialog::QDiconPrintEditDialog(QWidget *parent) : QDicomAddDialog(parent)
{
    ui->dialogTitle->setText("编辑DICOM打印机");
}
