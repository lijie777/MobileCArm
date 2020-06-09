#ifndef QSTORAGESERVICEADDDIALOG_H
#define QSTORAGESERVICEADDDIALOG_H

#include <QDialog>
#include "QDialogAbstract.h"
namespace Ui {
class QStorageServiceAddDialog;
}

class QStorageServiceAddDialog : public QDialogAbstract
{
    Q_OBJECT

public:
    explicit QStorageServiceAddDialog(QWidget *parent = 0);
    ~QStorageServiceAddDialog();

private slots:
    void on_closeBtn_clicked();

protected:
    Ui::QStorageServiceAddDialog *ui;
};

#endif // QSTORAGESERVICEADDDIALOG_H
