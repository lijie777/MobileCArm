#ifndef QCFGSAFESETPAGE_H
#define QCFGSAFESETPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgSafeSetPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgSafeSetPage(QWidget *parent = 0);
    virtual ~QCfgSafeSetPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();

signals:

public slots:
};

#endif // QCFGSAFESETPAGE_H
