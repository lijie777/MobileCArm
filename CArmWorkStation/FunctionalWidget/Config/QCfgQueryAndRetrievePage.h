#ifndef QCFGQUERYANDRETRIEVEPAGE_H
#define QCFGQUERYANDRETRIEVEPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgQueryAndRetrievePage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgQueryAndRetrievePage(QWidget *parent = 0);
    virtual ~QCfgQueryAndRetrievePage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
    virtual void init();
signals:

public slots:
    void onQRAddDialogShowSlot();
};

#endif // QCFGQUERYANDRETRIEVEPAGE_H
