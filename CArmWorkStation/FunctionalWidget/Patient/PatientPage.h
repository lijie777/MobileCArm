#pragma once

#include "Common\CArmWorkStationProc.h"
#include "Common\CArmPage.h"
#include "PatientPageWidget.h"
#include "PatientToolWidget.h"

class PatientPage : public CArmPage
{
    Q_OBJECT

public:
    PatientPage(CArmWorkStationProc* proc = NULL);
    ~PatientPage();

    QWidget* getPageWidget();

    QWidget* getToolWidget();

signals:
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

private:
    PatientPageWidget *page;
    PatientToolWidget *tool;

    CArmWorkStationProc *WorkStationProc;
};


