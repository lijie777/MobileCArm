#ifndef QCFGNETANDDICOMPAGE_H
#define QCFGNETANDDICOMPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgNetAndDicomPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgNetAndDicomPage(QWidget *parent = 0);
    virtual ~QCfgNetAndDicomPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGNETANDDICOMPAGE_H
