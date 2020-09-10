#include "QCfgWorklistPage.h"
#include "ui_SystemConfigWidget.h"
#include "singleton.h"
#include "Dialog.h"
QCfgWorklistPage::QCfgWorklistPage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgWorklistPage::~QCfgWorklistPage()
{

}

void QCfgWorklistPage::updatePage()
{

}

void QCfgWorklistPage::savePage()
{

}

void QCfgWorklistPage::enteryPage()
{

}

void QCfgWorklistPage::exitPage()
{

}

void QCfgWorklistPage::init() {
//	connect(ui->workListAddBtn, SIGNAL(clicked()), this, SLOT(onWorkListAddBtnClicked()));
//  connect(ui->ConnectTestBtn, SIGNAL(clicked()), this, SLOT(onConnectTestBtnClicked()));

//    connect(ui->workListAddBtn, &QPushButton::clicked, this, [=](){
//        emit Singleton<Dialog>::getInstance().showDialog(Dialog::WorkListAdd);
//    });

//    connect(ui->workListDelete, &QPushButton::clicked, this, [=](){
//    });

//    connect(ui->ConnectTestBtn, &QPushButton::clicked, this, [=](){
//        emit Singleton<Dialog>::getInstance().showDialog(Dialog::ConnectTest);
//    });

//    connect(ui->workListEdit, &QPushButton::clicked, this, [=](){
//        emit Singleton<Dialog>::getInstance().showDialog(Dialog::WorkListEdit);
//    });

//    connect(ui->setDefault, &QPushButton::clicked, this, [=](){
//    });
}

void QCfgWorklistPage::onWorkListAddBtnClicked() {
    emit Singleton<Dialog>::getInstance().showDialog(Dialog::WorkListAdd);
}

void QCfgWorklistPage::onConnectTestBtnClicked()
{
    emit Singleton<Dialog>::getInstance().showDialog(Dialog::ConnectTest);
}

