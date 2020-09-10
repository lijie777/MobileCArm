#ifndef QCFGUSERPAGE_H
#define QCFGUSERPAGE_H

#include "QConfigPageAbstract.h"

class QCfgUserPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgUserPage(QWidget *parent = 0);
    UNOVERWRITEVIRTUAL()

signals:

public slots:
};

#endif // QCFGUSERPAGE_H
