#ifndef QCONNECTTESTDIALOG_H
#define QCONNECTTESTDIALOG_H

#include <QDialog>
#include "QDialogAbstract.h"
namespace Ui {
class QConnectTestDialog;
}

class QConnectTestDialog : public QDialogAbstract
{
    Q_OBJECT

public:
    enum ConnectStatus{
        Connecting=     1 << 0,
        ConnectOK=      1 << 1,
        ConnectFailed=  1 << 2,
        WORKLIST =      (1 << 3)*2,
        DicomPrint =    (1 << 3)*3,
        StorageServer=  (1 << 3)*4,
        QueryRetrieve=  (1 << 3)*5,
        MPPS=           (1 << 3)*6
    };

    explicit QConnectTestDialog(QWidget *parent = 0);
    ~QConnectTestDialog();

    void setConnectStatus(ConnectStatus status);

private slots:
    void on_closeBtn_clicked();

private:
    Ui::QConnectTestDialog *ui;
};

#endif // QCONNECTTESTDIALOG_H
