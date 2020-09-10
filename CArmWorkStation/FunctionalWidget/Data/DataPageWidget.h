#pragma once

#include "Common\CArmWorkStationProc.h"
#include <QtWidgets/QFrame>
#include "ui_DataPageWidget.h"
#include "ui_DataPageRemindWidget.h"
#include "PreviewWidget.h"

#pragma execution_character_set("utf-8")


class DataPageWidget : public QFrame
{
    Q_OBJECT

public:
    DataPageWidget(CArmWorkStationProc* proc = NULL, QWidget *parent = 0);
    ~DataPageWidget();

signals:
    void sigStudyImage(DB_PatientComplete);  //查看检查
    void sigClickDataBtn();

private:
    /**
    * @brief 调用该函数，初始化功能右屏窗口布局
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

    void clearPreviewDisplay();

    void clearAllDisplay();

    int getCurrentAge(std::string str_birthDate);

    void setStudyDateBox();

    void remindMessage(const QString & text);

    void setStudyTimeBox(const std::string &selStudyDate);

    void showPreviewDisplay();

    private slots:
    void startStudyImage(DB_PatientComplete sPatientComplete);
    void showCurrentData();
    void PreviewChecked(bool);
    void FoldButtonChecked(PreviewWidget *view,bool ischecked);
    void PreviewDoubleClicked(std::string dicomPath);
    void con_StudyDateBox_change(const QString &text);
    void con_StudyTimeBox_change(const QString &text);

private:
    Ui::DataPageWidget ui;
    CArmWorkStationProc *WorkStationProc;

    QFrame *mPatientStatusRemind;
    Ui::DataPageRemindWidget *PatientStatusRemind_ui;

    DB_Patient currentPatient;
    std::string currentPatientCompleteID;
    std::string currentStudyDate;
    std::string currentStudyTime;
    bool isLoadPatient = false;

    DB_TmpSeries *currentTmpSeries;
    int countTmpSeries;

    std::vector<DB_StudyDate> vecDBStudyDate;

    std::map<std::string, std::string> mapStudyTime;

    int SelectedCount = 0;

    std::vector<PreviewWidget *> vePreviewWidget;

    bool isAllowStudyDate = false;
    bool isAllowStudyTime = false;

    bool isDefaultStudyDate = false;
};


