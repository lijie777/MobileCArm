#ifndef PAGENEWPATIENT_H
#define PAGENEWPATIENT_H

#pragma once

#include "Common\CArmWorkStationProc.h"
#include <QtWidgets/QFrame>
#include "ui_PageNewPatientWidget.h"
#include "ui_MoreInfoWidget.h"
#include "ui_OrderInfoWidget.h"
#include "MyCalendarWidget.h"
#include "DbDataStructures.h"

#pragma execution_character_set("utf-8")

enum PageMode
{
    NewScan,
    NewOrder,
    EditSchedule
};

class PageNewPatientWidget : public QFrame
{
    Q_OBJECT

public:
    PageNewPatientWidget(CArmWorkStationProc* proc = NULL, QWidget *parent = 0);
    ~PageNewPatientWidget();

    void setPageMode(PageMode mode);

signals:
    void sigEditPatientScheduled(DB_Patient sPatient);
    void sigEmerPatient();

private:

    /**
    * @brief 调用该函数，初始化实时左屏窗口布局
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

    int getCurrentAge(std::string str_birthDate);

    void SaveDataAndScan();

    void SaveData();

    void UpdateScheduleData();

    void clearWarning();

    void clearInput();

    bool checkString(QString qstr, int checkType);

    void remindMessage(const QString& text);

    std::string getTodayPatientNo();

    std::string getTodayPatientEmerNo();

    void updateTodayPatientNo();

    bool inputCheck();

    bool isDataChange(DB_Patient oldPatient, DB_Patient newPatient);

    private slots:
    void toEmerPatient();
    void showToScanDisplay();
    void showToOrderDisplay();
    void con_submit_clicked();
    void setAgeEdit(QString text);
    void showPatientBirthCalendar();
    void showScheduledDateCalendar();
    void setAutoPatinetID();
    void toEditPatientScheduled(DB_Patient sPatient);

private:
    Ui::PageNewPatientWidget ui;
    Ui::MoreInfoWidget moreInfo_ui;
    QFrame *moreInfoFrame;
    Ui::OrderInfoWidget orderInfo_ui;
    QFrame *orderInfoFrame;

    CArmWorkStationProc *WorkStationProc;

    QAction *PatientBirthAction; //!< 点击日历图标
    QAction *StudyDateAction; //!< 点击日历图标

    DB_Patient m_PatientInfo;  //!< 患者信息
    DB_PatientSchedule m_PatientSchedule; //!< 患者预约信息

    //基本信息
    QString strPatientID;
    QString strLastName;
    QString strFirstName;
    QString strGender;
    std::string strPatientBirth;

    //预约信息
    std::string strScheduledDate;
    QString strStudyHour;
    QString strStudyMinute;
    QString strAttendingDoctor;
    ScanPart mScanPart;

    //更多信息
    QString strAession;
    QString strStudyID;
    QString strHeight;
    QString strWeight;
    QString strReferralDoctor;
    QString strDesc;

    PageMode currentPageMode;

    DB_Patient noEdit_Patient;

protected:
    /**
    * @brief Qt事件过滤器，默认事件触发时，执行
    * @param obj 触发事件的对象
    * @param e 触发的事件
    * @return bool 反馈程序执行情况
    */
    bool eventFilter(QObject* obj, QEvent* e);
};

#endif // PAGENEWPATIENT_H