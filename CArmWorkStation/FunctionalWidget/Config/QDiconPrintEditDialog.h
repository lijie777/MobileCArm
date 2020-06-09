#ifndef QDICONPRINTEDITDIALOG_H
#define QDICONPRINTEDITDIALOG_H

#include <QWidget>
#include "QDicomAddDialog.h"
class QDiconPrintEditDialog : public QDicomAddDialog
{
    Q_OBJECT
public:
    explicit QDiconPrintEditDialog(QWidget *parent = 0);

signals:

public slots:
};

#endif // QDICONPRINTEDITDIALOG_H
