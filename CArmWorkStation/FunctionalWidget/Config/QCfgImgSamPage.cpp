#include "QCfgImgSamPage.h"
#include "ui_SystemConfigWidget.h"
QCfgImgSamPage::QCfgImgSamPage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgImgSamPage::~QCfgImgSamPage()
{

}

void QCfgImgSamPage::exitPage()
{

}

void QCfgImgSamPage::enteryPage()
{

}

void QCfgImgSamPage::savePage()
{
    //zhoupengju@ueg.com.cn:图像采集
    //基础设置
    cfg.m_autoTransfer = ui->imgSam_autoTransfer->isChecked();
    cfg.m_autoSave = ui->imgSam_autoSave->isChecked();
    cfg.m_autoPlayback = ui->imgSam_autoPlayback->isChecked();
    cfg.m_useSmartMetal = ui->imgSam_useSmartMetal->isChecked();
    //todo: 使用是视频功能时提前5s报警提示
    //todo: 拍摄视频模式时，拍摄视频时间
    //todo：左右踏板初始设置
    //done: CAK提示限制
    cfg.m_doseWarningLimit = ui->imgSam_doseWarningLimit->text().toInt();
}

void QCfgImgSamPage::updatePage()
{
    //zhoupengju@ueg.com.cn:图像采集-----------------------------------------------------------------------------------------
    //基础设置
    ui->imgSam_autoTransfer->setChecked(cfg.m_autoTransfer);
    ui->imgSam_autoSave->setChecked(cfg.m_autoSave);
    ui->imgSam_autoPlayback->setChecked(cfg.m_autoPlayback);
    ui->imgSam_useSmartMetal->setChecked(cfg.m_useSmartMetal);
    //todo: 使用是视频功能时提前5s报警提示
    //todo: 拍摄视频模式时，拍摄视频时间
    //todo：左右踏板初始设置
    //done: CAK提示限制
    ui->imgSam_doseWarningLimit->setText(QString(cfg.m_doseWarningLimit));

}
