#pragma once

#include "Common\CArmWorkStationProc.h"
#include "Common\CArmPage.h"
#include "DataPageWidget.h"
#include "DataToolWidget.h"

class DataPage : public CArmPage
{
    Q_OBJECT

public:
    DataPage(CArmWorkStationProc* proc = NULL);
    ~DataPage();

    QWidget* getPageWidget();

    QWidget* getToolWidget();

signals:
    void sigStudyImage(DB_PatientComplete);  //查看检查
    void sigClickDataBtn();

private:
    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

private:
    CArmWorkStationProc *WorkStationProc;
    DataPageWidget *page;
    DataToolWidget *tool;
};


