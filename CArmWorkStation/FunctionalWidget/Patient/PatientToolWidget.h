#pragma once

#include <QtWidgets/QFrame>
#include "ui_PatientToolWidget.h"

class PatientToolWidget : public QFrame
{
    Q_OBJECT

public:
    PatientToolWidget(QWidget *parent = 0);
    ~PatientToolWidget();

signals:
    void sigEmerPatient();
    void sigNewPatient();
    void sigEditPatient();
    void sigScheduledPatient();
    void sigCompletedPatient();
    void sigEditPatientScheduled();

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
    void showEditStatus();
    void setEditDisable();

private:
    Ui::PatientToolWidget ui;
};


