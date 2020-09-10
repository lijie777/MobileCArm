#ifndef PATIENTPAGEWIDGET_H
#define PATIENTPAGEWIDGET_H

#pragma once

#include "Common\CArmWorkStationProc.h"
#include <QtWidgets/QFrame>
#include "ui_PatientPageWidget.h"
#include "DbDataStructures.h"
#include "PageNewPatientWidget.h"
#include "PageScheduledWidget.h"
#include "PageCompletedWidget.h"

#pragma execution_character_set("utf-8")

class PatientPageWidget : public QFrame
{
    Q_OBJECT

public:
    PatientPageWidget(CArmWorkStationProc* proc = NULL, QWidget *parent = 0);
    ~PatientPageWidget();

signals:
    void sigEmerPatient();
    void sigNewPatient();
    void sigEditPatient();
    void sigScheduledPatient();
    void sigCompletedPatient();
    void sigEditPatientScheduled();
    void sigStudyImage(DB_PatientComplete);  //查看检查

private:

    /**
    * @brief 调用该函数，初始化实时左屏窗口布局
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

    private slots:
    void toNewPatient();
    void toEditPatientScheduled();
    void toScheduledPatient();
    void toCompletedPatient();
    void toScheduledHistoryPatient();

private:
    Ui::PatientPageWidget ui;
    PageNewPatientWidget *PageNewPatient;
    PageScheduledWidget *PageScheduled;
    PageScheduledWidget *PageScheduledHistory;
    PageNewPatientWidget *PageEditPatientSchedule;
    PageCompletedWidget *PageCompleted;
    CArmWorkStationProc *WorkStationProc;

};

#endif // PATIENTPAGEWIDGET_H