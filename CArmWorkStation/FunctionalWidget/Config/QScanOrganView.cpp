#include "QScanOrganView.h"
#include "ui_QScanOrganView.h"
#include "configdataprovider.h"
#include "QNavigationView.h"
#include "singleton.h"
#include "QScanModeView.h"
#include "QConfigPageAbstract.h"
#include "ui_SystemConfigWidget.h"

QScanOrganView::QScanOrganView(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::QScanOrganView();
	ui->setupUi(this);
}

QScanOrganView::~QScanOrganView()
{
    delete ui;
}

void QScanOrganView::updateView()
{
    CArmScanConfig cfgScanPart = QConfigPageAbstract::cfg.m_scanPartArray[index];
    ui->Scan_defaultOrgan->setChecked(!(((int)QConfigPageAbstract::cfg.m_defaultScanPart) ^ index));
    ui->Scan_upDownAdjust->setText(QString("%1").arg(cfgScanPart.m_fov.m_topBottom));
    ui->Scan_leftRightAdjust->setText(QString("%1").arg(cfgScanPart.m_fov.m_leftRight));

    for (int index =0; index < ui->ScanModeStacked->count(); index++) {
        dynamic_cast<QScanModeView*>(ui->ScanModeStacked->widget(index))->updateView();
    }
}

void QScanOrganView::saveCfg()
{


    for (int index =0; index < ui->ScanModeStacked->count(); index++) {
        dynamic_cast<QScanModeView*>(ui->ScanModeStacked->widget(index))->saveCfg();
    }
}

void QScanOrganView::init()
{
    //QScanModeView::setOrganUi(ui);
    foreach (QString item, Singleton<SysConfig::ConfigDataProvider>::getInstance().getScanModeList()) {
        ui->Scan_mode->addItem(item);
    }
    connect(ui->Scan_mode, &QNavHView::currentItemChanged, this, [=](const int &current){
        ui->ScanModeStacked->setCurrentIndex(current);
    });

    index = QConfigPageAbstract::ui->ScanOrganStack->indexOf(this);

    for (int index =0; index < ui->ScanModeStacked->count(); index++) {
        dynamic_cast<QScanModeView*>(ui->ScanModeStacked->widget(index))->init(this);
    }

//    index = QConfigPageAbstract::ui->ScanOrganStack->indexOf(this);
}
