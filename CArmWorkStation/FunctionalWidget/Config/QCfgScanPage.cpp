#include "QCfgScanPage.h"
#include "ui_SystemConfigWidget.h"
#include "configdataprovider.h"
#include "singleton.h"
#include "QNavigationView.h"
#include "QScanOrganView.h"
QCfgScanPage::QCfgScanPage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgScanPage::~QCfgScanPage()
{

}

void QCfgScanPage::updatePage()
{
    for (int index =0; index < ui->ScanOrganStack->count(); index++) {
        dynamic_cast<QScanOrganView*>(ui->ScanOrganStack->widget(index))->updateView();
    }
}

void QCfgScanPage::savePage()
{
    for (int index =0; index < ui->ScanOrganStack->count(); index++) {
        dynamic_cast<QScanOrganView*>(ui->ScanOrganStack->widget(index))->saveCfg();
    }
}

void QCfgScanPage::enteryPage()
{

}

void QCfgScanPage::exitPage()
{

}

void QCfgScanPage::init()
{
    //zhoupengju@ueg.com.cn: 初始化系统设置主菜单
    foreach (QString item, Singleton<SysConfig::ConfigDataProvider>::getInstance().getScanOrganList()) {
        ui->Scan_organMenu->addItem(item);
    }
    //设置拍摄设置菜单和页面关联
    connect(ui->Scan_organMenu, &QNavVComplexView::currentItemChanged, this, [=](const int &current){
        ui->ScanOrganStack->setCurrentIndex(current);
        //dynamic_cast<QConfigPageAbstract*>(ui->stackedWidget->currentWidget())->updatePage();
    });
    for (int index =0; index < ui->ScanOrganStack->count(); index++) {
        dynamic_cast<QScanOrganView*>(ui->ScanOrganStack->widget(index))->init();
    }

    //updatePage();
}
