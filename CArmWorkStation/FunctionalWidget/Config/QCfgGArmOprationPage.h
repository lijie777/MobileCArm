#ifndef QCFGGARMOPRATIONPAGE_H
#define QCFGGARMOPRATIONPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgGArmOprationPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgGArmOprationPage(QWidget *parent = 0);
    virtual ~QCfgGArmOprationPage();
    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGGARMOPRATIONPAGE_H
