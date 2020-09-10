#include "QCfgTimeZonePage.h"
#include "ui_SystemConfigWidget.h"
QCfgTimeZonePage::QCfgTimeZonePage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgTimeZonePage::~QCfgTimeZonePage()
{

}

void QCfgTimeZonePage::updatePage()
{
    //zhoupengju@ueg.com.cn:时间区域设置------------------------------------------------------------------------------------
    ui->TZ_organizationName->setText(ToQSTR(cfg.m_timeZoneCfg.organizationName));
    ui->TZ_organizationAddr->setText(ToQSTR(cfg.m_timeZoneCfg.organizationAddr));
    ui->TZ_date->setText(ToQSTR(cfg.m_timeZoneCfg.date));
    ui->TZ_SN->setText(ToQSTR(cfg.m_timeZoneCfg.sn));
    //todo:时区设置
    //todo:语言设置
    //设置日期格式
    switch (cfg.m_timeZoneCfg.dateFormat) {
    case TimeZoneCfg::MMDDYYYY:
        ui->TZ_MMDDYYYY->setChecked(true);
        break;
    case TimeZoneCfg::DDMMYYYY:
        ui->TZ_DDMMYYYY->setChecked(true);
        break;
    case TimeZoneCfg::YYYYMMDD:
        ui->TZ_YYYYMMDD->setChecked(true);
        break;
    default:
        ui->TZ_MMDDYYYY->setChecked(true);
        break;
    }
    //时间格式
    switch (cfg.m_timeZoneCfg.timeformat) {
    case TimeZoneCfg::_12:
        ui->TZ_12H->setChecked(true);
        break;
    case TimeZoneCfg::_24:
        ui->TZ_24H->setChecked(true);
        break;
    default:
        ui->TZ_12H->setChecked(true);
        break;
    }
    //todo:设置数字格式
    //设置高度单位
    switch (cfg.m_timeZoneCfg.heightunit) {
    case TimeZoneCfg::cm:
        ui->TZ_cm->setChecked(true);
        break;
    case TimeZoneCfg::inch:
        ui->TZ_inch->setChecked(true);
        break;
    default:
        ui->TZ_cm->setChecked(true);
        break;
    }
    //设置重量单位
    switch (cfg.m_timeZoneCfg.weightuint) {
    case TimeZoneCfg::kg:
        ui->TZ_kg->setChecked(true);
        break;
    case TimeZoneCfg::lb:
        ui->TZ_lb->setChecked(true);
        break;
    default:
        ui->TZ_kg->setChecked(true);
        break;
    }
    //设置辐射单位
    switch (cfg.m_timeZoneCfg.radiationunit) {
    case TimeZoneCfg::gray:
        ui->TZ_gray->setChecked(true);
        break;
    case TimeZoneCfg::roentgen:
        ui->TZ_roentgen->setChecked(true);
        break;
    default:
        ui->TZ_gray->setChecked(true);
        break;
    }
    //设置dap单位
    switch (cfg.m_timeZoneCfg.dapunit) {
    case TimeZoneCfg::Gycm2:
        ui->TZ_Gycm2->setChecked(true);
        break;
    case TimeZoneCfg::mGym2:
        ui->TZ_mGym2->setChecked(true);
        break;
    case TimeZoneCfg::mGycm2:
        ui->TZ_mGycm2->setChecked(true);
        break;
    case TimeZoneCfg::Rcm2:
        ui->TZ_Rcm2->setChecked(true);
        break;
    default:
        ui->TZ_Gycm2->setChecked(true);
        break;
    }

}

void QCfgTimeZonePage::savePage()
{
    //zhoupengju@ueg.com.cn:时间区域设置------------------------------------------------------------------------------------
    cfg.m_timeZoneCfg.organizationName = QStr2Std(ui->TZ_organizationName->text());
    cfg.m_timeZoneCfg.organizationAddr = QStr2Std(ui->TZ_organizationAddr->text());
    //todo:时区设置
    //todo:语言设置
    //设置日期格式
    if(ui->TZ_MMDDYYYY->isChecked()){
        cfg.m_timeZoneCfg.dateFormat = TimeZoneCfg::MMDDYYYY;
    }else if(ui->TZ_DDMMYYYY->isChecked()){
        cfg.m_timeZoneCfg.dateFormat = TimeZoneCfg::DDMMYYYY;
    }else if(ui->TZ_YYYYMMDD->isChecked()){
        cfg.m_timeZoneCfg.dateFormat = TimeZoneCfg::YYYYMMDD;
    }
    //时间格式
    if(ui->TZ_12H->isChecked()){
        cfg.m_timeZoneCfg.timeformat = TimeZoneCfg::_12;
    }else if(ui->TZ_24H->isChecked()){
        cfg.m_timeZoneCfg.timeformat = TimeZoneCfg::_24;
    }
    //todo:设置数字格式
    //设置高度单位
    if(ui->TZ_cm->isChecked()){
        cfg.m_timeZoneCfg.heightunit = TimeZoneCfg::cm;
    }else if(ui->TZ_inch->isChecked()){
        cfg.m_timeZoneCfg.heightunit = TimeZoneCfg::inch;
    }
    //设置重量单位
    if(ui->TZ_kg->isChecked()){
        cfg.m_timeZoneCfg.weightuint = TimeZoneCfg::kg;
    }else if(ui->TZ_lb->isChecked()){
        cfg.m_timeZoneCfg.weightuint = TimeZoneCfg::lb;
    }
    //设置辐射单位
    if(ui->TZ_gray->isChecked()){
        cfg.m_timeZoneCfg.radiationunit = TimeZoneCfg::gray;
    }else if(ui->TZ_roentgen->isChecked()){
        cfg.m_timeZoneCfg.radiationunit = TimeZoneCfg::roentgen;
    }
    //设置dap单位
    if(ui->TZ_Gycm2->isChecked()){
        cfg.m_timeZoneCfg.dapunit = TimeZoneCfg::Gycm2;
    }else if(ui->TZ_mGym2->isChecked()){
        cfg.m_timeZoneCfg.dapunit = TimeZoneCfg::mGym2;
    }else if(ui->TZ_mGycm2->isChecked()){
        cfg.m_timeZoneCfg.dapunit = TimeZoneCfg::mGycm2;
    }else if(ui->TZ_Rcm2->isChecked()){
        cfg.m_timeZoneCfg.dapunit = TimeZoneCfg::Rcm2;
    }

}

void QCfgTimeZonePage::enteryPage()
{

}

void QCfgTimeZonePage::exitPage()
{

}
