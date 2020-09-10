#ifndef QCFGSYSINFOPAGE_H
#define QCFGSYSINFOPAGE_H

#include "QConfigPageAbstract.h"



class QCfgSysInfoPage : public QConfigPageAbstract
{
	Q_OBJECT
public:
	explicit QCfgSysInfoPage(QWidget *parent = 0);
    virtual ~QCfgSysInfoPage();
	virtual void updatePage();
	virtual void savePage();

	virtual void enteryPage();
	virtual void exitPage();


signals:


	public slots :

private:
};

#endif // QCFGSYSINFOPAGE_H
