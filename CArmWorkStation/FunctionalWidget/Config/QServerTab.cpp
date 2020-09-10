#include "QServerTab.h"
#include "ui_QServerTab.h"

QServerTab::QServerTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QServerTab)
{
    ui->setupUi(this);
    init();
}

QServerTab::~QServerTab()
{
    delete ui;
}

void QServerTab::init()
{
    ui->ServerPageTitle->setText("");
    connect(ui->ServerAdd, &QPushButton::clicked, this, [=](){
        emit Singleton<Dialog>::getInstance().showDialog(Dialog::DialogType(dialogIndex + 1));
    });

    connect(ui->ServerConnectTest, &QPushButton::clicked, this, [=](){
        emit Singleton<Dialog>::getInstance().showDialog(Dialog::ConnectTest);
    });

    connect(ui->ServerEdit, &QPushButton::clicked, this, [=](){
        emit Singleton<Dialog>::getInstance().showDialog(Dialog::DialogType(dialogIndex + 2));
    });
}
