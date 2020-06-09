#ifndef PAGESCHEDULED_H
#define PAGESCHEDULED_H

#pragma once

#include "Common\CArmWorkStationProc.h"
#include <QtWidgets/QFrame>
#include "ui_PageScheduledWidget.h"
#include "ui_RemindWidget.h"
#include "MyCalendarWidget.h"
#include "DbDataStructures.h"

#pragma execution_character_set("utf-8")

class PageScheduledWidget : public QFrame
{
    Q_OBJECT

public:
    PageScheduledWidget(CArmWorkStationProc* proc = NULL,bool isHistory = false, QWidget *parent = 0);
    ~PageScheduledWidget();

signals:
    void sigEditPatientScheduled(DB_Patient sPatient);
    void sigPatientScheduledHistory();
    void sigPatientScheduled();

private:
    /**
    * @brief 调用该函数，初始化实时左屏窗口布局
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

    void remindMessage(const QString& text);

    void clearCondition();

    void clearRemind();

    void getCurrentRowInfo();

    void setDataButtonStatus(bool m_status);

    void setTableWidgetStyle();

    private slots:
    void con_ClearButton_clicked();
    void con_QueryButton_clicked();
    void setDataButtonDisable();
    void setDataButtonEnable();
    void showScheduledDateBeginCalendar();
    void showScheduledDateEndCalendar();
    void con_DeleteButton_clicked();
    void con_EditButton_clicked();
    void con_SubmitButton_clicked();

private:
    Ui::PageScheduledWdiget ui;
    CArmWorkStationProc *WorkStationProc;
    SearchCondition mSearchCondition;
    QFrame *mQueryResultRemind;
    Ui::RemindFrame *QueryResultRemind_ui;

    int m_nPatientCount;               //!< 搜索的患者信息数量
    DB_PatientSchedule *m_pPatientsSchedule;           //!< 搜索的患者信息

    DB_PatientSchedule *currentPatientSchedule;  //当前选中信息
    int cRow;   //当前选中行

    QAction *SchedueldDateBeginAction;  //!< 点击拍摄日期开始
    QAction *ScheduledDateEndAction;    //!< 点击拍摄日期结束

    bool isScheduledHistoryMode;  //!< 是否预约历史模式
};

#endif // PAGESCHEDULED_H