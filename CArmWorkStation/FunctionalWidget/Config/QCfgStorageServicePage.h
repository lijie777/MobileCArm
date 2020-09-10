#ifndef QCFGSTORAGESERVICEPAGE_H
#define QCFGSTORAGESERVICEPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgStorageServicePage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgStorageServicePage(QWidget *parent = 0);
    virtual ~QCfgStorageServicePage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
    virtual void init();


signals:

public slots:
    void onStorageAddDialogShowSlot();
};

#endif // QCFGSTORAGESERVICEPAGE_H
