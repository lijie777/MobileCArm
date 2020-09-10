#ifndef QCFGPADDETECTORPAGE_H
#define QCFGPADDETECTORPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgPadDetectorPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgPadDetectorPage(QWidget *parent = 0);
    virtual ~QCfgPadDetectorPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGPADDETECTORPAGE_H
