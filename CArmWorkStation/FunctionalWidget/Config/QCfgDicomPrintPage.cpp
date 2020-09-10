#include "QCfgDicomPrintPage.h"
#include "ui_SystemConfigWidget.h"
#include "singleton.h"
#include "Dialog.h"
QCfgDicomPrintPage::QCfgDicomPrintPage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgDicomPrintPage::~QCfgDicomPrintPage()
{

}

void QCfgDicomPrintPage::updatePage()
{

}

void QCfgDicomPrintPage::savePage()
{

}

void QCfgDicomPrintPage::enteryPage()
{

}

void QCfgDicomPrintPage::exitPage()
{

}

void QCfgDicomPrintPage::init()
{
//    connect(ui->DicomPrintAdd, SIGNAL(clicked()), this, SLOT(onDicomPrintAddSlot()));

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

void QCfgDicomPrintPage::onDicomPrintAddSlot()
{
    emit Singleton<Dialog>::getInstance().showDialog(Dialog::DicomPrintAdd);
}
