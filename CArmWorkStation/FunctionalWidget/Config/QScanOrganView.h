#pragma once

#include <QWidget>
#include "Config.h"
namespace Ui { class QScanOrganView; };

class QScanOrganView : public QWidget
{
	Q_OBJECT

public:
	QScanOrganView(QWidget *parent = Q_NULLPTR);
	~QScanOrganView();

    void updateView();
    void saveCfg();

    void init();


public:
	Ui::QScanOrganView *ui;
private:
    int index;
};
