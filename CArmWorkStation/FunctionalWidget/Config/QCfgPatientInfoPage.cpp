#include "QCfgPatientInfoPage.h"
#include "ui_SystemConfigWidget.h"
QCfgPatientInfoPage::QCfgPatientInfoPage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgPatientInfoPage::~QCfgPatientInfoPage()
{

}

void QCfgPatientInfoPage::updatePage()
{
    //zhoupengju@ueg.com.cn:患者信息设置------------------------------------------------------------------------------------
    ui->patientInfo_name->setChecked(cfg.m_imageInfoDisplay.m_patientName);
    ui->patientInfo_BirthDate->setChecked(cfg.m_imageInfoDisplay.m_patientBirthDate);
    ui->patientInfo_Gender->setChecked(cfg.m_imageInfoDisplay.m_patientGender);
    ui->patientInfo_id->setChecked(cfg.m_imageInfoDisplay.m_patientID);
    //todo:检查步骤
    //todo:检查部位
    ui->patientInfo_scanDate->setChecked(cfg.m_imageInfoDisplay.m_scanDate);
    ui->patientInfo_hospitalName->setChecked(cfg.m_imageInfoDisplay.m_hospitalName);
    ui->patientInfo_hospitalNum->setChecked(cfg.m_imageInfoDisplay.m_hospitalNumber);
    ui->patientInfo_marker->setChecked(cfg.m_imageInfoDisplay.m_marker);
}

void QCfgPatientInfoPage::savePage()
{
    //zhoupengju@ueg.com.cn:患者信息设置------------------------------------------------------------------------------------
    cfg.m_imageInfoDisplay.m_patientName = ui->patientInfo_name->isChecked();
    cfg.m_imageInfoDisplay.m_patientBirthDate = ui->patientInfo_BirthDate->isChecked();
    cfg.m_imageInfoDisplay.m_patientGender = ui->patientInfo_Gender->isChecked();
    cfg.m_imageInfoDisplay.m_patientID = ui->patientInfo_id->isChecked();
    //todo:检查步骤
    //todo:检查部位
    cfg.m_imageInfoDisplay.m_scanDate = ui->patientInfo_scanDate->isChecked();
    cfg.m_imageInfoDisplay.m_hospitalName = ui->patientInfo_hospitalName->isChecked();
    cfg.m_imageInfoDisplay.m_hospitalNumber = ui->patientInfo_hospitalNum->isChecked();
    cfg.m_imageInfoDisplay.m_marker = ui->patientInfo_marker->isChecked();
}

void QCfgPatientInfoPage::enteryPage()
{

}

void QCfgPatientInfoPage::exitPage()
{

}
