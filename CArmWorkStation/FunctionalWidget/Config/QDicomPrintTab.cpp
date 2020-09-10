#include "QDicomPrintTab.h"
#include "ui_QServerTab.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
QDicomPrintTab::QDicomPrintTab(QWidget *parent) : QServerTab(parent)
{
    dialogIndex = Dialog::DicomPrintIndex;
    ui->ServerPageTitle->setText("DICOM打印");
}
