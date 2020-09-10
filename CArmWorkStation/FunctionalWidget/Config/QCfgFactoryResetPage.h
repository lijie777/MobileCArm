#ifndef QCFGFACTORYRESETPAGE_H
#define QCFGFACTORYRESETPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgFactoryResetPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgFactoryResetPage(QWidget *parent = 0);
    virtual ~QCfgFactoryResetPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGFACTORYRESETPAGE_H
