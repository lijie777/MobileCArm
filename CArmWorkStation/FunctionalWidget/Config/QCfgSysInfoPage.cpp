#include "QCfgSysInfoPage.h"
#include "ui_SystemConfigWidget.h"
QCfgSysInfoPage::QCfgSysInfoPage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgSysInfoPage::~QCfgSysInfoPage()
{

}

void QCfgSysInfoPage::updatePage()
{
    //zhoupengju@ueg.com.cn:系统信息页面的UI数据更新--------------------------------------------------------------------------
    //系统信息
    ui->sysInfo_hostInfo->setText(ToQSTR(cfg.m_systemInfo.info.hostInfo));
    ui->sysInfo_hostSN->setText(ToQSTR(cfg.m_systemInfo.info.sn));
    ui->sysInfo_maintenanceTrackingNumber->setText(ToQSTR(cfg.m_systemInfo.info.maintenanceTrackingNumber));
    ui->sysInfo_version->setText(ToQSTR(cfg.m_systemInfo.info.version));
    ui->sysInfo_versionDate->setText(ToQSTR(cfg.m_systemInfo.info.versionDate));
    //当前系统使用统计
    ui->sysInfo_executedStudyNum->setText(QString(cfg.m_systemInfo.usageStatistics.executedStudyNum));
    ui->sysInfo_reservedStudyNum->setText(QString(cfg.m_systemInfo.usageStatistics.reservedStudyNum));
    ui->sysInfo_SavedPicNumber->setText(QString(cfg.m_systemInfo.usageStatistics.SavedPicNumber));
    //当前信息硬件信息
    ui->sysInfo_cArmModel->setText(ToQSTR(cfg.m_systemInfo.hwInfo.cArmModel));
    ui->sysInfo_cArmSN->setText(ToQSTR(cfg.m_systemInfo.hwInfo.sn));
}

void QCfgSysInfoPage::savePage()
{

}

void QCfgSysInfoPage::enteryPage()
{

}

void QCfgSysInfoPage::exitPage()
{

}
