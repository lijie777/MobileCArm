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
    * @brief ���øú�������ʼ��ʵʱ�������ڲ���
    */
    void initUI();

    /**
    * @brief ���øú�������ʼ���źŲ�����
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

    int m_nPatientCount;               //!< �����Ļ�����Ϣ����
    DB_PatientSchedule *m_pPatientsSchedule;           //!< �����Ļ�����Ϣ

    DB_PatientSchedule *currentPatientSchedule;  //��ǰѡ����Ϣ
    int cRow;   //��ǰѡ����

    QAction *SchedueldDateBeginAction;  //!< ����������ڿ�ʼ
    QAction *ScheduledDateEndAction;    //!< ����������ڽ���

    bool isScheduledHistoryMode;  //!< �Ƿ�ԤԼ��ʷģʽ
};

#endif // PAGESCHEDULED_H