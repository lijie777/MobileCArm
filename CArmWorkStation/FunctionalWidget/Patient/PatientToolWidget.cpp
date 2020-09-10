#include "PatientToolWidget.h"

PatientToolWidget::PatientToolWidget(QWidget *parent)
    : QFrame(parent)
{
    initUI();
    initLogic();
}

PatientToolWidget::~PatientToolWidget()
{

}

void PatientToolWidget::initUI()
{
    ui.setupUi(this);
}

void PatientToolWidget::initLogic()
{
    connect(ui.EmerPatientBtn, SIGNAL(clicked()), this, SIGNAL(sigEmerPatient()));  //急诊患者
    connect(ui.NewPatientBtn, SIGNAL(clicked()), this, SIGNAL(sigNewPatient()));  //新建患者
    connect(ui.NewPatientBtn, SIGNAL(clicked()), this, SLOT(setEditDisable()));  //新建患者

    //connect(ui.EditPatientBtn, SIGNAL(clicked()), this, SIGNAL(sigEditPatient()));  //编辑患者

    connect(ui.ScheduledPatientBtn, SIGNAL(clicked()), this, SIGNAL(sigScheduledPatient()));  //已预约患者
    connect(ui.ScheduledPatientBtn, SIGNAL(clicked()), this, SLOT(setEditDisable()));  //已预约患者

    connect(ui.CompletedPatientBtn, SIGNAL(clicked()), this, SIGNAL(sigCompletedPatient()));  //已执行患者
    connect(ui.CompletedPatientBtn, SIGNAL(clicked()), this, SLOT(setEditDisable()));  //已执行患者

    connect(this, SIGNAL(sigEditPatientScheduled()), this, SLOT(showEditStatus()));  //显示编辑患者
}

void PatientToolWidget::setEditDisable()
{
    ui.EditPatientBtn->setEnabled(false);
}

void PatientToolWidget::showEditStatus()
{
    ui.EditPatientBtn->setEnabled(true);
    ui.EditPatientBtn->setChecked(true);
}