#ifndef QCFGLOGWATCHPAGE_H
#define QCFGLOGWATCHPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgLogWatchPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgLogWatchPage(QWidget *parent = 0);
    virtual ~QCfgLogWatchPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGLOGWATCHPAGE_H
