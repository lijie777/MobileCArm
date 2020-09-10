#ifndef QCFGPATIENTINFOPAGE_H
#define QCFGPATIENTINFOPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgPatientInfoPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgPatientInfoPage(QWidget *parent = 0);
    virtual ~QCfgPatientInfoPage();
    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
signals:

public slots:
};

#endif // QCFGPATIENTINFOPAGE_H
