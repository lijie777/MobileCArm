#include "QWorkListAddDialog.h"
#include "ui_QWorkListAddDialog.h"
#include "singleton.h"
#include "Dialog.h"
QWorkListAddDialog::QWorkListAddDialog(QWidget *parent) :
    QDialogAbstract(parent),
    ui(new Ui::QWorkListAddDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    ui->dialogTitle->setText("新增WorkList");

    connect(ui->connectTest, &QPushButton::clicked, this, [=](){
        Singleton<Dialog>::getInstance().showDialog(Dialog::ConnectTest);
    });
}

QWorkListAddDialog::~QWorkListAddDialog()
{
    delete ui;
}


void QWorkListAddDialog::on_closeBtn_clicked()
{
    close();
}
