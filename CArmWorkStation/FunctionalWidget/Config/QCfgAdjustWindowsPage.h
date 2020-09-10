#ifndef QCFGADJUSTWINDOWSPAGE_H
#define QCFGADJUSTWINDOWSPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgAdjustWindowsPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgAdjustWindowsPage(QWidget *parent = 0);
    virtual ~QCfgAdjustWindowsPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();

signals:

public slots:
};

#endif // QCFGADJUSTWINDOWSPAGE_H
