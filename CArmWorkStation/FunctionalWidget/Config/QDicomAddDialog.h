#ifndef QDICOMADDDIALOG_H
#define QDICOMADDDIALOG_H

#include <QWidget>
#include "QWorkListAddDialog.h"
class QDicomAddDialog : public QWorkListAddDialog
{
    Q_OBJECT
public:
    explicit QDicomAddDialog(QWidget *parent = 0);

signals:

public slots:
};

#endif // QDICOMADDDIALOG_H
