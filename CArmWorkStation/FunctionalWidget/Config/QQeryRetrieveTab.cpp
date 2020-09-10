#include "QQeryRetrieveTab.h"
#include "ui_QServerTab.h"
QQeryRetrieveTab::QQeryRetrieveTab(QWidget *parent) : QServerTab(parent)
{
    dialogIndex = Dialog::QueryRetrieveIndex;
    ui->ServerPageTitle->setText("Query&Retrieve");
}
