#include "QQueryRetrieveAddDialog.h"
#include "ui_QQueryRetrieveAddDialog.h"
#include "Dialog.h"
#include "singleton.h"
QQueryRetrieveAddDialog::QQueryRetrieveAddDialog(QWidget *parent) :
	QDialogAbstract(parent),
    ui(new Ui::QQueryRetrieveAddDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    connect(ui->connectTest, &QPushButton::clicked, this, [=](){
        Singleton<Dialog>::getInstance().showDialog(Dialog::ConnectTest);
    });
}

QQueryRetrieveAddDialog::~QQueryRetrieveAddDialog()
{
    delete ui;
}

void QQueryRetrieveAddDialog::on_closeBtn_clicked()
{
    close();
}
