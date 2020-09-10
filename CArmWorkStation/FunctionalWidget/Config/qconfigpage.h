#ifndef QCONFIGPAGE_H
#define QCONFIGPAGE_H

#include <QWidget>
#include <QDebug>
#include "Config.h"
#include "singleton.h"
#include "QConfigPageAbstract.h"

class QConfigPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QConfigPage(QWidget *parent = 0);
    virtual ~QConfigPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();


signals:


public slots:

private:

};

#endif // QCONFIGPAGE_H
