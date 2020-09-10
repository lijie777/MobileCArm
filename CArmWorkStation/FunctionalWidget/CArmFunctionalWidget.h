#pragma once

#include <QtWidgets/QFrame>
#include "Common\CArmStatus.h"

#include "ui_CArmFunctionalWidget.h"
#include "Common\CArmLoginWidget.h"
#include "Common\CArmLogOutWidget.h"
#include "Config\SysConfigWidget.h"
#include "Review\ReviewPage.h"
#include "Data\DataPage.h"
#include "Patient\PatientPage.h"

#pragma execution_character_set("utf-8")

class CArmFunctionalWidget;
class FunctionalWidgetStatus : public CArmStatus
{

public:

    /**
    * @brief 实时窗口状态构造函数
    * @param widget 实时窗口对象
    */
    FunctionalWidgetStatus(CArmFunctionalWidget* widget, int type) : CArmStatus{ type }, m_pWidget{ widget } { initLogic(); }

    /**
    * @brief 实时窗口状态析构函数
    */
    ~FunctionalWidgetStatus() { }

    /**
    * @brief 调用该函数，处理状态
    */
    virtual void dealStatus() { }

protected:

    void initLogic();


protected:

    /// 功能窗口对象
    CArmFunctionalWidget* m_pWidget;
};



/**
* @brief 功能窗口闲置状态类
*/
class FunctionalWidgetIdleStatus : public FunctionalWidgetStatus
{
public:

    /**
    * @brief 实时窗口闲置状态构造函数
    * @param widget 实时窗口对象
    */
    FunctionalWidgetIdleStatus(CArmFunctionalWidget* widget) : FunctionalWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_IDLE } { }

    /**
    * @brief 实时窗口闲置状态析构函数
    */
    ~FunctionalWidgetIdleStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;


    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;
};


/**
* @brief 实时窗口拍片准备状态类
*/
class FunctionalWidgetReadyStatus : public FunctionalWidgetStatus
{
public:

    /**
    * @brief 实时窗口拍片准备状态构造函数
    * @param widget 实时窗口对象
    */
    FunctionalWidgetReadyStatus(CArmFunctionalWidget* widget) : FunctionalWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_READY } { }

    /**
    * @brief 实时窗口拍片准备状态析构函数
    */
    ~FunctionalWidgetReadyStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;


    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;
};


/**
* @brief 实时窗口曝光状态类
*/
class FunctionalWidgetExposureStatus : public FunctionalWidgetStatus
{
public:

    /**
    * @brief 实时窗口曝光状态类构造函数
    * @param widget 实时窗口对象
    */
    FunctionalWidgetExposureStatus(CArmFunctionalWidget* widget) : FunctionalWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_EXPOSURE } { }

    /**
    * @brief 实时窗口曝光状态类析构函数
    */
    ~FunctionalWidgetExposureStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;

    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;
};


/**
* @brief 实时窗口拍片终止状态类
*/
class FunctionalWidgetAbortStatus : public FunctionalWidgetStatus
{
public:

    /**
    * @brief 实时窗口拍片终止状态类构造函数
    * @param widget 实时窗口对象
    */
    FunctionalWidgetAbortStatus(CArmFunctionalWidget* widget, FunctionalWidgetStatus* nxtStatus) : FunctionalWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_ABORT }, m_pNxtStatus{ nxtStatus } { }

    /**
    * @brief 实时窗口拍片终止状态类析构函数
    */
    ~FunctionalWidgetAbortStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;


    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;

private:

    std::shared_ptr<FunctionalWidgetStatus> m_pNxtStatus;       //!<后一个状态
};


/**
* @brief 实时窗口注销状态类
*/
class FunctionalWidgetLogoutStatus : public FunctionalWidgetStatus
{
public:

    /**
    * @brief 实时窗口注销状态类构造函数
    * @param widget 实时窗口对象
    */
    FunctionalWidgetLogoutStatus(CArmFunctionalWidget* widget, std::shared_ptr<FunctionalWidgetStatus> nxtStatus) : FunctionalWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_LOGOUT }, m_pNxtStatus{ nxtStatus } { }

    /**
    * @brief 实时窗口注销状态类析构函数
    */
    ~FunctionalWidgetLogoutStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;

    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;

private:

    std::shared_ptr<FunctionalWidgetStatus> m_pNxtStatus;   //!<后一个状态
};



/**
* @brief 功能右屏窗口类
*/
class CArmFunctionalWidget : public QFrame, CArmObject
{

    friend class FunctionalWidgetIdleStatus;
    friend class FunctionalWidgetReadyStatus;
    friend class FunctionalWidgetExposureStatus;
    friend class FunctionalWidgetAbortStatus;
    friend class FunctionalWidgetLogoutStatus;

    Q_OBJECT

public:



public:

    /**
    * @brief 功能右屏窗口类构造函数
    * @param proc 外部通讯对象
    * @param parent 父窗口对象
    */
    CArmFunctionalWidget(CArmWorkStationProc* proc, QWidget *parent = 0);

    /**
    * @brief 功能右屏窗口类析构函数
    */
    ~CArmFunctionalWidget();

    /**
    * @brief 准备拍片
    */
    void prepareScan(const CP_StudyData& ptInfo);

    /**
    * @brief 清空患者信息
    */
    void clearPtInfo();

    /**
    * @brief 更新配置
    */
    void updateConfig(const CArmConfig& config);

    /**
    * @brief 已执行信息完成拍摄,本次检查下第一次完成拍摄
    * @param patient_complete_id 执行ID
    * @param studyUID 检查UID
    * @return bool 是否更新成功
    */
    bool UpdatePatientCompleteOver(const std::string &patient_complete_id, const std::string &studyUID);

    /**
    * @brief 更新执行信息状态
    * @param patient_complete_id 执行ID
    * @param sStudyStatus 检查状态
    * @return bool 是否更新成功
    */
    bool UpdatePatientCompleteStatus(const std::string &patient_complete_id, StudyStatus sStudyStatus);

    protected slots:

    /**
    * @brief 登录工作站槽函数
    */
    void loginStation();

    /**
    * @brief 弹窗退出槽函数
    */
    void logoutStation();

    void enterSystemConfig();

    void exitSystemConfig();

    void showReviewPage();

    void showDataPage();

    void showPatientPage();

    void showLogoutWidget();

    /**
    * @brief 调用该函数，设置窗口状态，通过信号槽机制，不可直接调用
    * @param status 状态
    */
    void setWidgetStatus(std::shared_ptr<FunctionalWidgetStatus> status);

private:

    /**
    * @brief 调用该函数，初始化功能右屏窗口布局
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

    /**
    * @brief 调用该函数，创建功能右屏窗口布局
    */
    void createUI();

    /**
    * @brief 工作站指令处理函数
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmd) override;


    /**
    * @brief 重写父类paintEvent
    * @param event 事件指针
    * @return void
    */
    void paintEvent(QPaintEvent *event);

private:


    std::shared_ptr<FunctionalWidgetStatus> m_pStatus;  //!< 当前状态
    CArmWorkStationProc *WorkStationProc;               //!< 外部通讯接口


    Ui::CArmFunctionWidget ui;                          //!< 界面布局
    CArmLoginWidget *m_loginWidget;                     //!< 登录窗口
    CArmLogOutWidget *m_logoutWidget;                   //!< 注销窗口
    SysConfig::SystemConfigWidget m_sysConfigWidget;    //!< 配置窗口
    ReviewPage* m_reviewPage;                           //!< 阅片窗口
    DataPage *m_dataPage;                                //!< 目录窗口
    PatientPage *m_patientPage;                         //!< 患者窗口

};

