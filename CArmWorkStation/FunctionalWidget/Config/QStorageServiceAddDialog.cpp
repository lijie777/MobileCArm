#include "QStorageServiceAddDialog.h"
#include "ui_QStorageServiceAddDialog.h"
#include "Dialog.h"
#include "singleton.h"
QStorageServiceAddDialog::QStorageServiceAddDialog(QWidget *parent) :
    QDialogAbstract(parent),
    ui(new Ui::QStorageServiceAddDialog)
{
    ui->setupUi(this);
	setModal(true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
	connect(ui->connectTest, &QPushButton::clicked, this, [=]() {
		Singleton<Dialog>::getInstance().showDialog(Dialog::ConnectTest);
	});
}

QStorageServiceAddDialog::~QStorageServiceAddDialog()
{
    delete ui;
}

void QStorageServiceAddDialog::on_closeBtn_clicked()
{
    close();
}
