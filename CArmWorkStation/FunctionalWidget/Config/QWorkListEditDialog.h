#ifndef QWORKLISTEDITDIALOG_H
#define QWORKLISTEDITDIALOG_H

#include <QWidget>
#include "QWorkListAddDialog.h"

class QWorkListEditDialog : public QWorkListAddDialog
{
    Q_OBJECT
public:
    explicit QWorkListEditDialog(QWidget *parent = 0);

signals:

public slots:
};

#endif // QWORKLISTEDITDIALOG_H
