#pragma once

#include <QTimer>
#include <QtWidgets/QFrame>
#include "ui_CArmTrackingWidget.h"
#include "Common\CArmStatus.h"
#include <WinSock2.h>
#include <Windows.h>
#include "SlaveComputerCArmBase.h"
#include "CArmScCmdDef.h"
#include "Config.h"
#include "TrackView.h"
#include "CArmCommon.h"


/// 电量状态
enum POWER_STATUS
{
    PS_HIGH,    //!< 电量充足
    PS_LOW,     //!< 电量不足
};

/// 温度状态
enum TEMPRATURE_STATUS
{
    TS_NORMAL,      //!< 温度正常
    TS_WARNING,     //!< 温度告警
    TS_OVERLIMIT    //!< 温度超标
};




class CArmTrackingWidget;
class TrackingWidgetStatus : public CArmStatus
{

public:

    /**
    * @brief 实时窗口状态构造函数
    * @param widget 实时窗口对象
    */
    TrackingWidgetStatus(CArmTrackingWidget* widget, int type) : CArmStatus{ type } , m_pWidget{ widget } { initLogic(); }

    /**
    * @brief 实时窗口状态析构函数
    */
    ~TrackingWidgetStatus() { }

    /**
    * @brief 调用该函数，处理状态
    */
    virtual void dealStatus() { }

protected:

    /**
    * @brief 调用该函数，开始曝光
    * @param cmdObject 指令对象
    */
    void exposeXRay();

    /**
    * @brief 调用该函数，曝光异常终止
    * @param cmdObject 指令对象
    */
    void exposeAbort(/*参数待定*/);

    /**
    * @brief 调用该函数，更新激光灯状态
    * @param object 激光灯参数
    */
    void updateLaster(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，更新触摸屏状态
    * @param object 触摸屏参数
    */
    void updateTPStatus(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，更新触摸屏状态
    * @param object 触摸屏参数
    */
    void updateTemper(/*参数待定*/);

    /**
    * @brief 调用该函数，更新电量状态
    * @param object 触摸屏参数
    */
    void updatePower(/*参数待定*/);


    /**
    * @brief 调用该函数，更新图像处理参数
    * @param object 图像处理参数
    */
    void updateImageParms(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，更新拍片参数
    * @param object 拍片参数
    */
    void updateScanParms(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，接收图像
    * @param object 图像数据
    */
    void receiveImage(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，接收请求的数据
    * @param object 请求的数据
    */
    void receiveRequest(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，结束曝光
    */
    void finishExpore();

protected:

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();


protected:

    /// 实时窗口对象
    CArmTrackingWidget* m_pWidget;
};



/**
* @brief 实时窗口闲置状态类
*/
class TrackingWidgetIdleStatus : public TrackingWidgetStatus
{
public:


    /**
    * @brief 实时窗口闲置状态构造函数
    * @param widget 实时窗口对象
    */
    TrackingWidgetIdleStatus(CArmTrackingWidget* widget) :TrackingWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_IDLE } { }


    /**
    * @brief 实时窗口闲置状态析构函数
    */
    ~TrackingWidgetIdleStatus() { }
    

    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;

    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;

protected:

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& cmdObject);

    /**
    * @brief 调用该函数，更新设备状态
    * @param object 设备状态
    */
    void updateDevStatus(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，开始曝光
    * @param object 一级手闸状态
    */
    void startExpose(std::shared_ptr<CArmCmdBase> object);
};


/**
* @brief 实时窗口拍片准备状态类
*/
class TrackingWidgetReadyStatus : public TrackingWidgetStatus
{
public:

    /**
    * @brief 实时窗口拍片准备状态构造函数
    * @param widget 实时窗口对象
    */
    TrackingWidgetReadyStatus(CArmTrackingWidget* widget) :TrackingWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_READY } { }

    /**
    * @brief 实时窗口拍片准备状态析构函数
    */
    ~TrackingWidgetReadyStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;

    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;

protected:

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& cmdObject);

    /**
    * @brief 调用该函数，更新设备状态
    * @param object 设备状态
    */
    void updateDevStatus(std::shared_ptr<CArmCmdBase> object);

    /**
    * @brief 调用该函数，开始曝光
    * @param object 一级手闸状态
    */
    void startExpose(std::shared_ptr<CArmCmdBase> object);
};


/**
* @brief 实时窗口曝光状态类
*/
class TrackingWidgetExposureStatus : public TrackingWidgetStatus
{
public:

    /**
    * @brief 实时窗口曝光状态类构造函数
    * @param widget 实时窗口对象
    */
    TrackingWidgetExposureStatus(CArmTrackingWidget* widget) : TrackingWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_EXPOSURE } { }

    /**
    * @brief 实时窗口曝光状态类析构函数
    */
    ~TrackingWidgetExposureStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;

    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;

protected:

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& cmdObject);


    /**
    * @brief 调用该函数，更新设备状态
    * @param object 设备状态
    */
    void updateDevStatus(std::shared_ptr<CArmCmdBase> object);

};


/**
* @brief 实时窗口拍片终止状态类
*/
class TrackingWidgetAbortStatus : public TrackingWidgetStatus
{
public:

    /**
    * @brief 实时窗口拍片终止状态类构造函数
    * @param widget 实时窗口对象
    */
    TrackingWidgetAbortStatus(CArmTrackingWidget* widget, std::shared_ptr<TrackingWidgetStatus> nxtStatus, const ErrorCode& code) : TrackingWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_ABORT }, m_pNxtStatus{ nxtStatus }, m_errCode{ code } { }


    /**
    * @brief 实时窗口拍片终止状态类析构函数
    */
    ~TrackingWidgetAbortStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;

    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;

protected:

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& cmdObject);

    /**
    * @brief 调用该函数，更新设备状态
    * @param object 设备状态
    */
    void updateDevStatus(std::shared_ptr<CArmCmdBase> object);

private:

    const ErrorCode m_errCode;           //!< 设备异常状态
    std::shared_ptr<TrackingWidgetStatus> m_pNxtStatus;  //!<后一个状态
};


/**
* @brief 实时窗口注销状态类
*/
class TrackingWidgetLogoutStatus : public TrackingWidgetStatus
{
public:

    /**
    * @brief 实时窗口注销状态类构造函数
    * @param widget 实时窗口对象
    */
    TrackingWidgetLogoutStatus(CArmTrackingWidget* widget, std::shared_ptr<TrackingWidgetStatus> nxtStatus) : TrackingWidgetStatus{ widget , CArmStatus::STATUS_CODE::SC_LOGOUT }, m_pNxtStatus{ nxtStatus } { }

    /**
    * @brief 实时窗口注销状态类析构函数
    */
    ~TrackingWidgetLogoutStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmdObject 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;

    /**
    * @brief 调用该函数，处理状态
    */
    void dealStatus() override;

protected:

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& cmdObject);

    /**
    * @brief 调用该函数，更新设备状态
    * @param object 设备状态
    */
    void updateDevStatus(std::shared_ptr<CArmCmdBase> object);

private:

    std::shared_ptr<TrackingWidgetStatus> m_pNxtStatus;  //!<后一个状态
};


/**
* @brief 实时左屏窗口类
*/
class CArmTrackingWidget : public QFrame, CArmObject
{
    friend class TrackingWidgetStatus;
    friend class TrackingWidgetIdleStatus;
    friend class TrackingWidgetReadyStatus;
    friend class TrackingWidgetExposureStatus;
    friend class TrackingWidgetAbortStatus;
    friend class TrackingWidgetLogoutStatus;

    Q_OBJECT

public:

    /**
    * @brief 实时左屏窗口类构造函数
    * @param parent 父窗口对象
    */
    CArmTrackingWidget(QWidget *parent = 0);

    /**
    * @brief 实时左屏窗口类析构函数
    */
    ~CArmTrackingWidget();

    /**
    * @brief 更新配置
    */
    void updateConfig(const CArmConfig& config);


private:

    /**
    * @brief 调用该函数，初始化实时左屏窗口布局
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

    /**
    * @brief 工作站指令处理函数
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmd) override;





protected slots:

    /**
    * @brief 计时槽函数
    */
    void clockTimeOut();

    /**
    * @brief 屏保显示槽函数
    */
    void showScreen();

    /**
    * @brief 图像传输槽函数
    */
    void transferImage();

    /**
    * @brief 调用该函数，更新时间显示
    */
    void updateTime();

    /**
    * @brief 调用该函数，更新磁盘显示
    */
    void updateDiskCapacity();

    /**
    * @brief 调用该函数，更新图像信息
    */
    void updatePictureInfo();

    /**
    * @brief 调用该函数，更新视频信息
    */
    void updateMovieInfo();



    /**
    * @brief 调用该函数，更新检查信息
    * @param info 检查信息
    */
    void updateStudyInfo(const CP_StudyData& info);


    /**
    * @brief 调用该函数，更新拍摄信息
    * @param info 拍摄信息
    */
    void updateSeriesInfo(const CP_SeriesData& info);

    /**
    * @brief 调用该函数，更新图像显示
    * @param image 图像
    */
    void updateImage(const CP_ImageData& image);


    /**
    * @brief 调用该函数，更新扫描信息
    * @param config 扫描配置
    * @param isAuto 是否自动
    * @param count 拍片帧数
    */
    void updateScanInfo(CArmLastScanConfig data);

    /**
    * @brief 调用该函数，更新曝光信息
    * @param gycm gycm参数
    * @param mgymin mgymin参数
    * @param mgy mgy参数
    */
    void updateExposeInfo(float gycm, float mgymin, float mgy);

    /**
    * @brief 调用该函数，更新图像调节参数
    * @param parms 图像调节参数
    */
    void updateImgAdjustParms(ImageParam parms);

    /**
    * @brief 调用该函数，更新激光灯状态
    * @param on 激光灯状态
    */
    void updatLasterStatus(bool on);

    /**
    * @brief 调用该函数，更新网络状态
    * @param on 网络状态
    */
    void updateNetworkStatus(bool on);

    /**
    * @brief 调用该函数，更新设备状态
    * @param on 设备状态
    */
    void updateDeviceStatus(ErrorCode code);

    /**
    * @brief 调用该函数，更新光栅状态
    * @param on 光栅状态
    */
    void updateRasterStatus(bool on);

    /**
    * @brief 调用该函数，更新触摸屏状态
    * @param on 触摸屏状态
    */
    void updateTPStatus(bool on);

    /**
    * @brief 调用该函数，更新电量状态
    * @param on 电量状态
    */
    void updatePowerStatus(POWER_STATUS status);

    /**
    * @brief 调用该函数，更新电量状态
    * @param on 电量状态
    */
    void updateTemperatureStatus(TEMPRATURE_STATUS status);

    /**
    * @brief 调用该函数，设置窗口状态
    * @param status 状态
    */
    void setWidgetStatus(std::shared_ptr<TrackingWidgetStatus> status);

private:

    CP_ReviewData* m_CurrImageInfo;             //!< 当前图像
    CP_SeriesData m_CurrSeriesInfo;             //!< 当前拍摄
    CP_StudyData m_CurrStudyInfo;               //!< 当前检查
    std::map<std::string, std::list<CP_ReviewData*>> m_ImageInfo; //!< 图像列表


    Ui::CArmTrackingWidget ui;      //!< 左屏布局
    TrackView* m_pTrackViewer;      //!< 图像窗口
    QTimer m_ClockTimer;            //!< 时钟定时器

    IImage2DTextureImportorComponentPtr m_pTextureImportor; //!< 图像纹理加载工具
    CArmLastScanConfig m_ScanData;            //!< 拍片参数
    std::shared_ptr<TrackingWidgetStatus> m_pStatus;//!< 当前状态

};


