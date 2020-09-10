#ifndef QCFGSYSMAINTENANCEPAGE_H
#define QCFGSYSMAINTENANCEPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgSysMaintenancePage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgSysMaintenancePage(QWidget *parent = 0);
    virtual ~QCfgSysMaintenancePage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGSYSMAINTENANCEPAGE_H
