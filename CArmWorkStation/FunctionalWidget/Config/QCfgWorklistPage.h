#ifndef QCFGWORKLISTPAGE_H
#define QCFGWORKLISTPAGE_H

#include <QWidget>
#include "QConfigPageAbstract.h"
class QCfgWorklistPage : public QConfigPageAbstract
{
    Q_OBJECT
public:
    explicit QCfgWorklistPage(QWidget *parent = 0);
    virtual ~QCfgWorklistPage();

    virtual void updatePage();
    virtual void savePage();

    virtual void enteryPage();
    virtual void exitPage();
	virtual void init();

signals:

public slots :
	void onWorkListAddBtnClicked();
    void onConnectTestBtnClicked();
};

#endif // QCFGWORKLISTPAGE_H
