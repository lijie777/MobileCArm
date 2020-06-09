#pragma once

#include <QWidget>
namespace Ui { class QScanModeView; };
class QScanOrganView;
class QScanModeView : public QWidget
{
    Q_OBJECT

public:
    QScanModeView(QWidget *parent = Q_NULLPTR);
    ~QScanModeView();

    void updateView();
    void saveCfg();

    void init(QScanOrganView *);

private:
    Ui::QScanModeView *ui;
    int organIndex;
    int modeIndex;
    QScanOrganView *organObj;
};
