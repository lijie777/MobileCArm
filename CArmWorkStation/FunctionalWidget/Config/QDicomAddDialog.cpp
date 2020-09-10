#include "QDicomAddDialog.h"
#include "ui_QWorkListAddDialog.h"
QDicomAddDialog::QDicomAddDialog(QWidget *parent) : QWorkListAddDialog(parent)
{
    ui->dialogTitle->setText("新增Dicom打印机");

}
