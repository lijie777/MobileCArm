#ifndef QCFGUSERMANAGERSPAGE_H
#define QCFGUSERMANAGERSPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"

class QCfgUserManagerPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgUserManagerPage(QWidget *parent = 0);
    ~QCfgUserManagerPage();

    void updatePage()override;
    void savePage()override;

    void enteryPage()override;
    void exitPage()override;

    void init()override;
signals:

public slots :

private slots :
    void userDeleteSlot();
    void userAddSlot();
    void userEditSlot();
    void userDeleteAllCheckSlot();

};

#endif // QCFGTRANSFERSTATUSPAGE_H
