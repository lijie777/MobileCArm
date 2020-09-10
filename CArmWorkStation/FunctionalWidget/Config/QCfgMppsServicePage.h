#ifndef QCFGMPPSSERVICEPAGE_H
#define QCFGMPPSSERVICEPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgMppsServicePage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgMppsServicePage(QWidget *parent = 0);
    virtual ~QCfgMppsServicePage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
    virtual void init();
signals:

public slots:
        void onMppsAddDialogShowSlot();
};

#endif // QCFGMPPSSERVICEPAGE_H
