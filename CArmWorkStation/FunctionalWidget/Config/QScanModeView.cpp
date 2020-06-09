#include "QScanModeView.h"
#include "ui_QScanModeView.h"
#include "QConfigPageAbstract.h"
#include "ui_SystemConfigWidget.h"
#include "ui_QScanOrganView.h"
#include "Config.h"
#include "QConfigPageAbstract.h"
#include "QScanOrganView.h"

QScanModeView::QScanModeView(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::QScanModeView();
	ui->setupUi(this);

}

QScanModeView::~QScanModeView()
{
    delete ui;
}

void QScanModeView::updateView()
{
    CArmScanConfig organCfg = QConfigPageAbstract::cfg.m_scanPartArray[organIndex];
    int defaultMode = (int)organCfg.m_defaultScanMode;
    CArmScanMode scanMode = organCfg.m_scanModeArray[modeIndex];
    ui->Scan_defaultMode->setChecked(!(defaultMode ^ modeIndex));

    ui->Scan_pulseSelect->setCurrentIndex(scanMode.m_pps);

    ui->Scan_defaultDosimeter->setCurrentIndex((int)scanMode.m_defaultDoseType);

    ui->Scan_kVp->setText(QString("%1").arg(scanMode.m_scanParamArray[(int)scanMode.m_defaultDoseType].m_kVp));
    ui->Scan_mA->setText(QString("%1").arg(scanMode.m_scanParamArray[(int)scanMode.m_defaultDoseType].m_mA));
}

void QScanModeView::saveCfg()
{

}

void QScanModeView::init(QScanOrganView *organView)
{
    organObj = organView;
    organIndex = QConfigPageAbstract::ui->ScanOrganStack->indexOf(organObj);
    modeIndex = organObj->ui->ScanModeStacked->indexOf(this);
}

