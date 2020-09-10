#pragma once

#include <map>
#include <list>
#include <WinSock2.h>
#include <Windows.h>
#include "CArmScCmdDef.h"
#include "CArmCommon.h"





/// 小C工作站指令类型
enum class CARMST_CMD
{
    CMD_EXIT,           //!< 退出指令
    CMD_LOGOUT,         //!< 注销指令
    CMD_INITIALIZE,     //!< 工作站恢复初始状态指令
    CMD_PREPARESCAN,    //!< 准备拍片指令
    CMD_EXPOSE,         //!< 曝光指令
    CMD_EXPOSEDONE,     //!< 曝光结束指令
    CMD_DEVICE,         //!< 设备消息处理指令
    CMD_CONFIG          //!< 配置指令
};



struct CArmCMD;


/**
* @brief 移动C型臂工作站对象基类
*/
class CArmObject
{
public:

    /**
    * @brief 移动C型臂工作站对象基类构造函数
    */
    CArmObject() { }

    /**
    * @brief 移动C型臂工作站对象基类析构函数
    */
    virtual ~CArmObject() { }

    /**
    * @brief 调用该函数，注册工作站对象
    * @param object 工作站对象
    */
    static void registerObject(CArmObject* object);

    /**
    * @brief 调用该函数，广播工作站消息给已注册的工作站对象
    * @param cmd 指令对象
    */
    static void executeCmd(const CArmCMD& cmd);


protected:


    /**
    * @brief 调用该函数，工作站对象处理指令
    * @param cmd 指令对象
    */
    virtual void doCmd(const CArmCMD& cmd) { }


protected:

    /// 工作站对象注册列表
    static std::list<CArmObject*> m_Lst;
};




/**
* @brief 移动C型臂工作站消息指令类
*/
struct CArmCMD
{
    CArmCMD(CARMST_CMD type) : _type{ type } { }
    virtual ~CArmCMD() { }

    /// 指令类型
    const CARMST_CMD _type;

};


/**
* @brief 移动C型臂工作站退出指令类
*/
struct CArmExitCMD : CArmCMD
{
    CArmExitCMD() : CArmCMD{ CARMST_CMD::CMD_EXIT } { }
    ~CArmExitCMD() { }

};

/**
* @brief 移动C型臂工作站注销指令类
*/
struct CArmLogoutCMD : CArmCMD
{
    CArmLogoutCMD() : CArmCMD{ CARMST_CMD::CMD_LOGOUT } { }
    ~CArmLogoutCMD() { }

};

/**
* @brief 移动C型臂工作站初始化指令类
*/
struct CArmInitCMD : CArmCMD
{
    CArmInitCMD() : CArmCMD{ CARMST_CMD::CMD_INITIALIZE } { }
    ~CArmInitCMD() { }

};


/**
* @brief 移动C型臂工作站拍片准备指令类
*/
struct CArmPrepareScanCMD : CArmCMD
{
    CArmPrepareScanCMD(const CP_ScanRespInfo& scan, const CP_PatientData& pt) : CArmCMD{ CARMST_CMD::CMD_PREPARESCAN }, _scanInfo{ scan }, _patientInfo{ pt } { }
    ~CArmPrepareScanCMD() { }

    const CP_ScanRespInfo&      _scanInfo;       //!< 拍摄信息
    const CP_PatientData&       _patientInfo;    //!< 患者信息
};


/**
* @brief 移动C型臂工作站曝光指令类
*/
struct CArmExposeCMD : CArmCMD
{
    CArmExposeCMD() : CArmCMD{ CARMST_CMD::CMD_EXPOSE } { }
    ~CArmExposeCMD() { }

};

/**
* @brief 移动C型臂工作站曝光结束指令类
*/
struct CArmExposeDoneCMD : CArmCMD
{
    CArmExposeDoneCMD() : CArmCMD{ CARMST_CMD::CMD_EXPOSEDONE } { }
    ~CArmExposeDoneCMD() { }

};


/**
* @brief 移动C型臂工作站设备通讯指令类
*/
struct CArmDeviceCMD : CArmCMD
{
    CArmDeviceCMD(std::shared_ptr<CArmCmdBase> cmd)
        : CArmCMD{ CARMST_CMD::CMD_DEVICE }
        , _cmdObject(cmd)
    { 
        
    }
    ~CArmDeviceCMD() { }

    std::shared_ptr<CArmCmdBase> _cmdObject;
};


/**
* @brief 移动C型臂工作站配置指令类
*/
struct CArmConfigCMD : CArmCMD
{
    CArmConfigCMD(const CArmConfig& cfg)
        : CArmCMD{ CARMST_CMD::CMD_CONFIG }
        , _config(cfg)
    {

    }
    ~CArmConfigCMD() { }

    const CArmConfig& _config;
};