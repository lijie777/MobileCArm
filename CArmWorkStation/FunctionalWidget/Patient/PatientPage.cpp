#include "PatientPage.h"

PatientPage::PatientPage(CArmWorkStationProc* proc):
    WorkStationProc(proc)
    , page(new PatientPageWidget(proc))
    , tool(new PatientToolWidget())
{
    // 注册到列表，接收指令
    CArmPage::registerPage(this);

    initUI();
    initLogic();
}

PatientPage::~PatientPage()
{

}

void PatientPage::initUI()
{
}

void PatientPage::initLogic()
{
    connect(tool, SIGNAL(sigEmerPatient()), page, SIGNAL(sigEmerPatient()));  //新建急诊患者
    connect(tool, SIGNAL(sigNewPatient()), page, SIGNAL(sigNewPatient()));  //新建患者
    connect(tool, SIGNAL(sigEditPatient()), page, SIGNAL(sigEditPatient()));  //编辑患者
    connect(tool, SIGNAL(sigScheduledPatient()), page, SIGNAL(sigScheduledPatient()));  //已预约患者
    connect(tool, SIGNAL(sigCompletedPatient()), page, SIGNAL(sigCompletedPatient()));  //已执行患者
    connect(page, SIGNAL(sigEditPatientScheduled()), tool, SIGNAL(sigEditPatientScheduled()));  //编辑患者预约信息
    connect(page, SIGNAL(sigStudyImage(DB_PatientComplete)), this, SIGNAL(sigStudyImage(DB_PatientComplete)));  //编辑患者预约信息
}


QWidget * PatientPage::getPageWidget()
{
    return page;
}

QWidget * PatientPage::getToolWidget()
{
    return tool;
}

