#ifndef QCFGSCANPAGE_H
#define QCFGSCANPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QNavHComplexDelegate;

class QCfgScanPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgScanPage(QWidget *parent = 0);
    virtual ~QCfgScanPage();
    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
    virtual void init();

signals:

public slots:

private:

};

#endif // QCFGSCANPAGE_H
