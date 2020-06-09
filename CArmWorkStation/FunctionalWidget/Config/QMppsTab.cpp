#include "QMppsTab.h"
#include "ui_QServerTab.h"
QMppsTab::QMppsTab(QWidget *parent) : QServerTab(parent)
{
    dialogIndex = Dialog::MppsServerIndex;
    ui->ServerPageTitle->setText("MPPS Server");

}
