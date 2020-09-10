#ifndef QWORKLISTADDDIALOG_H
#define QWORKLISTADDDIALOG_H

#include <QDialog>
#include "QDialogAbstract.h"
#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif
namespace Ui {
class QWorkListAddDialog;
}

class QWorkListAddDialog : public QDialogAbstract
{
    Q_OBJECT

public:
    explicit QWorkListAddDialog(QWidget *parent = 0);
    ~QWorkListAddDialog();

private slots:
    void on_closeBtn_clicked();

protected:
    Ui::QWorkListAddDialog *ui;
};

#endif // QWORKLISTADDDIALOG_H
