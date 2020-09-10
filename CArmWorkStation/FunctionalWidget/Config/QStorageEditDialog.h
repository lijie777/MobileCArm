#ifndef QSTORAGEEDITDIALOG_H
#define QSTORAGEEDITDIALOG_H

#include <QWidget>
#include "QStorageServiceAddDialog.h"
class QStorageEditDialog : public QStorageServiceAddDialog
{
    Q_OBJECT
public:
    explicit QStorageEditDialog(QWidget *parent = 0);

signals:

public slots:
};

#endif // QSTORAGEEDITDIALOG_H
