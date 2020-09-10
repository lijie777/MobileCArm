#ifndef PAGECOMPLETED_H
#define PAGECOMPLETED_H

#pragma once

#include "Common\CArmWorkStationProc.h"
#include <QtWidgets/QFrame>
#include "ui_PageCompletedWidget.h"
#include "ui_RemindWidget.h"
#include "MyCalendarWidget.h"
#include "DbDataStructures.h"

#pragma execution_character_set("utf-8")

class PageCompletedWidget : public QFrame
{
    Q_OBJECT

public:
    PageCompletedWidget(CArmWorkStationProc* proc = NULL, QWidget *parent = 0);
    ~PageCompletedWidget();

signals:
    void sigEditPatientCompleted(DB_Patient);
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
    void con_NewStudyButton_clicked();
    void con_RecoverStudyButton_clicked();
    void con_StudyButton_clicked();

private:
    Ui::PageCompletedWdiget ui;
    CArmWorkStationProc *WorkStationProc;
    SearchCondition mSearchCondition;
    QFrame *mQueryResultRemind;
    Ui::RemindFrame *QueryResultRemind_ui;

    int m_nPatientCount;               //!< 搜索的患者信息数量
    DB_PatientComplete *m_pPatientsComplete;           //!< 搜索的患者信息

    DB_PatientComplete *currentPatientComplete;   //选中的行信息
    StudyStatus currentStudyStatus;
    int cRow;   //选中的当前行

    QAction *StudyDateBeginAction;  //!< 点击拍摄日期开始
    QAction *StudyDateEndAction;    //!< 点击拍摄日期结束
};

#endif // PAGECOMPLETED_H