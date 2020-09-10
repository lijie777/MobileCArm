#ifndef QCFGCARMSYSPAGE_H
#define QCFGCARMSYSPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgCArmSysPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgCArmSysPage(QWidget *parent = 0);
    virtual ~QCfgCArmSysPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();

signals:

public slots:
};

#endif // QCFGCARMSYSPAGE_H
