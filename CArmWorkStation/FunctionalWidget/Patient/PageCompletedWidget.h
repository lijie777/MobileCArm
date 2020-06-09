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
    void sigStudyImage(DB_PatientComplete);  //�鿴���

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
    void con_NewStudyButton_clicked();
    void con_RecoverStudyButton_clicked();
    void con_StudyButton_clicked();

private:
    Ui::PageCompletedWdiget ui;
    CArmWorkStationProc *WorkStationProc;
    SearchCondition mSearchCondition;
    QFrame *mQueryResultRemind;
    Ui::RemindFrame *QueryResultRemind_ui;

    int m_nPatientCount;               //!< �����Ļ�����Ϣ����
    DB_PatientComplete *m_pPatientsComplete;           //!< �����Ļ�����Ϣ

    DB_PatientComplete *currentPatientComplete;   //ѡ�е�����Ϣ
    StudyStatus currentStudyStatus;
    int cRow;   //ѡ�еĵ�ǰ��

    QAction *StudyDateBeginAction;  //!< ����������ڿ�ʼ
    QAction *StudyDateEndAction;    //!< ����������ڽ���
};

#endif // PAGECOMPLETED_H