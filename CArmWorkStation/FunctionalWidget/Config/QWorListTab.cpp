#include "QWorListTab.h"
#include "ui_QServerTab.h"
QWorListTab::QWorListTab(QWidget *parent) : QServerTab(parent)
{
    dialogIndex = Dialog::WorkListIndex;
    ui->ServerPageTitle->setText("Worklist");
}
