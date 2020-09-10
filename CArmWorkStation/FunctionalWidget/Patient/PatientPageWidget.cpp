#include "PatientPageWidget.h"
#include "CommonUtility.h"
#include <QListView>
#include <QtCore/QSettings.h>
#include "..\Message\MessageDialog.h"
#include "..\Message\MessageTipDialog.h"

PatientPageWidget::PatientPageWidget(CArmWorkStationProc* proc, QWidget *parent)
    : QFrame(parent)
    , WorkStationProc(proc)
    , PageNewPatient(new PageNewPatientWidget(proc))
    , PageScheduled(new PageScheduledWidget(proc))
    , PageScheduledHistory(new PageScheduledWidget(proc, true))
    , PageEditPatientSchedule(new PageNewPatientWidget(proc))
    , PageCompleted(new PageCompletedWidget(proc))
{
    initUI();
    initLogic();
}

PatientPageWidget::~PatientPageWidget()
{

}

void PatientPageWidget::initUI()
{
    ui.setupUi(this);
    PageNewPatient->setPageMode(PageMode::NewScan);
    ui.pageList->addWidget(PageNewPatient);
    ui.pageList->addWidget(PageScheduled);
    ui.pageList->addWidget(PageScheduledHistory);
    PageEditPatientSchedule->setPageMode(PageMode::EditSchedule);
    ui.pageList->addWidget(PageEditPatientSchedule);
    ui.pageList->addWidget(PageCompleted);
    ui.pageList->setCurrentWidget(PageNewPatient);
}

void PatientPageWidget::initLogic()
{
    connect(this, SIGNAL(sigEmerPatient()), PageNewPatient, SIGNAL(sigEmerPatient()));  //新建患者
    connect(this, SIGNAL(sigNewPatient()), this, SLOT(toNewPatient()));  //新建患者
    //connect(this, SIGNAL(sigEditPatient()), this, SLOT(toEditPatient()));  //编辑患者
    connect(this, SIGNAL(sigScheduledPatient()), this, SLOT(toScheduledPatient()));  //已预约患者
    connect(this, SIGNAL(sigCompletedPatient()), this, SLOT(toCompletedPatient()));  //已执行患者
    connect(PageScheduled, SIGNAL(sigEditPatientScheduled(DB_Patient)), this, SIGNAL(sigEditPatientScheduled()));  //tool切换为编辑患者
    connect(PageScheduled, SIGNAL(sigEditPatientScheduled(DB_Patient)), this, SLOT(toEditPatientScheduled()));  //显示编辑预约患者窗口
    connect(PageScheduled, SIGNAL(sigEditPatientScheduled(DB_Patient)), PageEditPatientSchedule, SIGNAL(sigEditPatientScheduled(DB_Patient)));  //编辑预约患者
    connect(PageCompleted, SIGNAL(sigEditPatientCompleted(DB_Patient)), this, SIGNAL(sigEditPatientScheduled()));  //tool切换为编辑患者
    connect(PageCompleted, SIGNAL(sigEditPatientCompleted(DB_Patient)), this, SLOT(toEditPatientScheduled()));  //显示编辑预约患者窗口
    connect(PageCompleted, SIGNAL(sigEditPatientCompleted(DB_Patient)), PageEditPatientSchedule, SIGNAL(sigEditPatientScheduled(DB_Patient)));  //编辑预约患者
    connect(PageCompleted, SIGNAL(sigStudyImage(DB_PatientComplete)), this, SIGNAL(sigStudyImage(DB_PatientComplete)));  //查看检查
    connect(PageScheduled, SIGNAL(sigPatientScheduledHistory()), this, SLOT(toScheduledHistoryPatient()));  //显示预约历史界面
    connect(PageScheduledHistory, SIGNAL(sigPatientScheduled()), this, SLOT(toScheduledPatient()));  //显示预约界面
}

void PatientPageWidget::toNewPatient()
{
    ui.pageList->setCurrentWidget(PageNewPatient);
}

void PatientPageWidget::toEditPatientScheduled()
{
    ui.pageList->setCurrentWidget(PageEditPatientSchedule);
}

void PatientPageWidget::toScheduledPatient()
{
    ui.pageList->setCurrentWidget(PageScheduled);
}

void PatientPageWidget::toScheduledHistoryPatient()
{
    ui.pageList->setCurrentWidget(PageScheduledHistory);
}


void PatientPageWidget::toCompletedPatient()
{
    ui.pageList->setCurrentWidget(PageCompleted);
}
