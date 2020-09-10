#ifndef QCFGDICOMPRINTPAGE_H
#define QCFGDICOMPRINTPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgDicomPrintPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgDicomPrintPage(QWidget *parent = 0);
    virtual ~QCfgDicomPrintPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
    virtual void init();
signals:

public slots:
    void onDicomPrintAddSlot();
};

#endif // QCFGDICOMPRINTPAGE_H
