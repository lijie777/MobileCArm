#include "QConnectTestDialog.h"
#include "ui_QConnectTestDialog.h"
#include "QConnecting.h"
QConnectTestDialog::QConnectTestDialog(QWidget *parent) :
	QDialogAbstract(parent),
    ui(new Ui::QConnectTestDialog)
{
    ui->setupUi(this);
    setModal(true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    ui->connectingStatus1->start();
    ui->connectingStatus2->start();
}

QConnectTestDialog::~QConnectTestDialog()
{
    delete ui;
}

void QConnectTestDialog::setConnectStatus(QConnectTestDialog::ConnectStatus status)
{
    switch (status) {
    case WORKLIST | Connecting:

        break;
    case WORKLIST | ConnectOK:

        break;
    case WORKLIST | ConnectFailed:

        break;
    case DicomPrint | Connecting:

        break;
    case DicomPrint | ConnectOK:

        break;
    case DicomPrint | ConnectFailed:

        break;
    case StorageServer | Connecting:

        break;
    case StorageServer | ConnectOK:

        break;
    case StorageServer | ConnectFailed:

        break;
    case QueryRetrieve | Connecting:

        break;
    case QueryRetrieve | ConnectOK:

        break;
    case QueryRetrieve | ConnectFailed:

        break;
    case MPPS | Connecting:

        break;
    case MPPS | ConnectOK:

        break;
    case MPPS | ConnectFailed:

        break;
    default:
        break;
    }
}

void QConnectTestDialog::on_closeBtn_clicked()
{
    close();
}
