#ifndef QQUERYRETRIEVEADDDIALOG_H
#define QQUERYRETRIEVEADDDIALOG_H

#include <QDialog>
#include "QDialogAbstract.h"
namespace Ui {
class QQueryRetrieveAddDialog;
}

class QQueryRetrieveAddDialog : public QDialogAbstract
{
    Q_OBJECT

public:
    explicit QQueryRetrieveAddDialog(QWidget *parent = 0);
    ~QQueryRetrieveAddDialog();

private slots:
    void on_closeBtn_clicked();

protected:
    Ui::QQueryRetrieveAddDialog *ui;
};

#endif // QQUERYRETRIEVEADDDIALOG_H
