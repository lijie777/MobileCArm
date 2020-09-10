#pragma once

#include "Common\CArmStatus.h"
#include "Common\CArmCMD.h"
#include "Common\CArmWorkStationProc.h"
#include <QObject>

class StationCommManager;

/**
* @brief 工作站管理代理类
*/
class StationManagerStatus : public CArmStatus
{
public:

    /**
    * @brief 工作站管理代理类构造函数
    */
    StationManagerStatus(StationCommManager* manager, int type) :m_pStManager{ manager }, CArmStatus{ type } { }

    /**
    * @brief 工作站管理代理类析构函数
    */
    ~StationManagerStatus() { }

    /**
    * @brief 调用该函数，执行进程通讯指令
    * @param cmdObject 指令对象
    */
    virtual void doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject) { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmd 指令对象
    */
    virtual void doCmd(const CArmCMD& cmdObject) { };

protected:


    /**
    * @brief 调用该函数，更新设备状态
    * @param object 设备状态
    */
    void updateDevStatus(std::shared_ptr<CArmCmdBase> object);


protected:

    
    StationCommManager* m_pStManager;       //!< 工作站管理对象
};

/**
* @brief 设备通讯管理闲置状态类
*/
class CommManagerIdleStatus : public StationManagerStatus
{
public:


    /**
    * @brief 设备通讯管理闲置状态构造函数
    * @param widget 设备通讯管理对象
    */
    CommManagerIdleStatus(StationCommManager* widget)
        : StationManagerStatus{ widget, CArmStatus::STATUS_CODE::SC_IDLE }
    {}


    /**
    * @brief 设备通讯管理闲置状态析构函数
    */
    ~CommManagerIdleStatus() { }



    /**
    * @brief 调用该函数，执行指令
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;


    /**
    * @brief 调用该函数，执行进程通讯指令
    * @param cmdObject 指令对象
    */
    void doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject) override;


protected:

    /**
    * @brief 调用该函数，执行准备拍片指令
    * @param cmdObject 指令对象
    */
    void handlePrepareScanCMD(const CArmPrepareScanCMD& object);

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& object);

    /**
    * @brief 调用该函数，开始曝光
    * @param object 一级手闸状态
    */
    void startExpose(std::shared_ptr<CArmCmdBase> object);

};


/**
* @brief 设备通讯管理拍片准备状态类
*/
class CommManagerReadyStatus : public StationManagerStatus
{
public:

    /**
    * @brief 设备通讯管理拍片准备状态构造函数
    * @param widget 设备通讯管理对象
    */
    CommManagerReadyStatus(StationCommManager* widget)
        : StationManagerStatus(widget, CArmStatus::STATUS_CODE::SC_READY)
    {}

    /**
    * @brief 设备通讯管理拍片准备状态析构函数
    */
    ~CommManagerReadyStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;


    /**
    * @brief 调用该函数，执行进程通讯指令
    * @param cmdObject 指令对象
    */
    void doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject) override;


protected:


    /**
    * @brief 调用该函数，执行准备拍片指令
    * @param cmdObject 指令对象
    */
    void handlePrepareScanCMD(const CArmPrepareScanCMD& cmdObject);

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& object);


    /**
    * @brief 调用该函数，开始曝光
    * @param object 一级手闸状态
    */
    void startExpose(std::shared_ptr<CArmCmdBase> object);
};


/**
* @brief 设备通讯管理曝光状态类
*/
class CommManagerExposureStatus : public StationManagerStatus
{
public:

    /**
    * @brief 设备通讯管理曝光状态类构造函数
    * @param widget 设备通讯管理对象
    */
    CommManagerExposureStatus(StationCommManager* widget)
        : StationManagerStatus(widget, CArmStatus::STATUS_CODE::SC_EXPOSURE)
    {}

    /**
    * @brief 设备通讯管理曝光状态类析构函数
    */
    ~CommManagerExposureStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;


    /**
    * @brief 调用该函数，执行进程通讯指令
    * @param cmdObject 指令对象
    */
    void doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject) override;

protected:

    /**
    * @brief 调用该函数，执行设备通讯指令
    * @param cmdObject 指令对象
    */
    void handleDeviceCMD(const CArmDeviceCMD& object);

    /**
    * @brief 调用该函数，结束曝光
    * @param object 结束曝光指令
    */
    void finishExpose(std::shared_ptr<CArmCmdBase> object);
};


/**
* @brief 设备通讯管理拍片终止状态类
*/
class CommManagerAbortStatus : public StationManagerStatus
{
public:

    /**
    * @brief 设备通讯管理拍片终止状态类构造函数
    * @param widget 设备通讯管理对象
    */
    CommManagerAbortStatus(StationCommManager* widget, std::shared_ptr<StationManagerStatus> nxtStatus)
        : StationManagerStatus(widget, CArmStatus::STATUS_CODE::SC_ABORT)
        , m_pNxtStatus{ nxtStatus }
    {}


    /**
    * @brief 设备通讯管理拍片终止状态类析构函数
    */
    ~CommManagerAbortStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;



    /**
    * @brief 调用该函数，执行进程通讯指令
    * @param cmdObject 指令对象
    */
    void doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject) override;

private:

    std::shared_ptr<StationManagerStatus> m_pNxtStatus;     //!< 后一个状态
};


/**
* @brief 设备通讯管理注销状态类
*/
class CommManagerLogoutStatus : public StationManagerStatus
{
public:

    /**
    * @brief 设备通讯管理注销状态类构造函数
    * @param widget 设备通讯管理对象
    */
    CommManagerLogoutStatus(StationCommManager* widget, std::shared_ptr<StationManagerStatus> nxtStatus) 
        : StationManagerStatus{ widget, CArmStatus::STATUS_CODE::SC_LOGOUT }
        , m_pNxtStatus{ nxtStatus }
    {}

    /**
    * @brief 设备通讯管理注销状态类析构函数
    */
    ~CommManagerLogoutStatus() { }


    /**
    * @brief 调用该函数，执行指令
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmdObject) override;


    /**
    * @brief 调用该函数，执行进程通讯指令
    * @param cmdObject 指令对象
    */
    void doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject) override;

private:

    std::shared_ptr<StationManagerStatus> m_pNxtStatus;  //!<后一个状态
};





/**
* @brief 工作站通讯管理类
* @param proc 外部通讯对象实例
*/
class StationCommManager : public QObject, CArmObject
{

    friend class CommManagerIdleStatus;
    friend class CommManagerReadyStatus;
    friend class CommManagerExposureStatus;
    friend class CommManagerAbortStatus;
    friend class CommManagerLogoutStatus;


    Q_OBJECT

signals:

    void receiveDevCommSig(std::shared_ptr<CArmCmdBase> cmdObject);

public:

    /**
    * @brief 工作站通讯管理类构造函数
    * @param proc 外部通讯对象实例
    */
    StationCommManager(CArmWorkStationProc* proc);

    /**
    * @brief 工作站通讯管理类析构函数
    */
    ~StationCommManager();


    /**
    * @brief 设备通讯事件函数
    * @param deviceCmd 设备通讯指令
    */
    void deviceCommEvent(const CArmCmdBase* cmdObject);


private:

    /**
    * @brief 工作站指令处理函数
    * @param cmd 指令对象
    */
    void doCmd(const CArmCMD& cmd) override;


    /**
    * @brief 调用该函数，更新状态
    * @param status 状态
    */
    void setStatus(std::shared_ptr<StationManagerStatus> status);


    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

protected slots:
   
    /**
    * @brief 接收设备指令槽函数
    * @param cmdObject 设备通讯指令
    */
    void receiveDevComm(std::shared_ptr<CArmCmdBase> cmdObject);

private:

    CArmWorkStationProc* m_pProcInst;    //!< 外部通讯对象实例

    std::shared_ptr<StationManagerStatus> m_pStatus;         //!< 当前状态

};

