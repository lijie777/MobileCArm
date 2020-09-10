#ifndef DEVICE_COMM_H
#define DEVICE_COMM_H

#include <IPCClientBase.h>
#include <SubProcessBase.h>
#include "ScanInfoServer.h"
#include "DeivceStatusServer.h"
#include <IPC_CMD_Def_CArm.h>

class UCArmDeviceComm : public USubProcessBase
{
public:
    UCArmDeviceComm();
    ~UCArmDeviceComm();

#pragma region 父类虚函数实现

    /**
    * @brief 解析输入参数
    * @param argc 参数数量
    * @param argv 参数
    * @return bool 是否解析成功
    */
    virtual bool ArgsParse(int argc, char *argv[]);

    /**
    * @brief 连接父进程心跳服务
    * @return bool 是否连接成功
    */
    virtual bool StartHeartbeat();

    /**
    * @brief 连接日志服务(包括获取日志端口)
    * @return bool 是否连接成功
    */
    virtual bool StartLogging();

    /**
    * @brief 获取服务端口（包括请求分配自身提供的服务和其他进程提供的服务（不包含日志服务））
    * @return bool 是否获取成功
    */
    virtual bool GetServicePorts();

    /**
    * @brief 启动自身提供的服务
    * @return bool 是否启动成功
    */
    virtual bool StartServices();

    /**
    * @brief 开始运行
    * @return bool 是否运行成功
    */
    virtual bool Run() { return true; };

#pragma  endregion

    bool IsDevProcConnected() { return m_pScanInfoServer != nullptr && m_pScanInfoServer->IsClientConnected(); };
    void Uninitialize();

    void ExitApp();

    std::function<void(void)> OnHBDisconnected;

#pragma region 设备控制函数

    //开始2D扫描
    std::function<ErrorCode(DevWorkMode m, bool bXRay)>StartScan2D;

    //开始3D扫描
    std::function<ErrorCode(DevWorkMode m, OptionCArmMove direct, bool bXRay)>StartScan3D;

    //查询下位机当前状态
    std::function<ErrorCode(void)>GetScStatus;

    //重置设备
    std::function<ErrorCode()>ResetDevice;

    //重置设备报警状态
    std::function<ErrorCode()>ResetWarn;

    //激光灯开关控制
    std::function<ErrorCode(OptionLaserSwitch opt, bool bOn)>LaserSwitch;

    //设置管电压
    std::function<ErrorCode(float fKV)>SetKV;

    //设置管电流
    std::function<ErrorCode(float fMA)>SetMA;

    //设置采集帧率
    std::function<ErrorCode(int nFPS)>SetFPS;

    //设置曝光时间
    std::function<ErrorCode(int nExp)>SetExposure;

    //设置采集帧数
    std::function<ErrorCode(int nFrameNum)>SetFrameNum;

    //设置限束器位置
    std::function<ErrorCode(OptionCollimator coll, float speed, float fPos)>SetCollimator;

    //控制升降柱开始连续运动
    std::function<ErrorCode(OptionLiftColMove opt, float fSpeed)>MoveAxisZ;

    //控制前后轴开始连续运动
    std::function<ErrorCode(OptionXAxisMove opt, float fSpeed)>MoveAxisX;

    //控制C型臂开始连续运动
    std::function<ErrorCode(OptionCArmMove opt, float fSpeed)>MoveAxisY;

    //获取某个轴的当前位置（角度）
    std::function<ErrorCode(CArmAxisDef axis, float &fVal)>GetPosAxis;

    //控制某个轴以指定速度运动到指定位置
    std::function<ErrorCode(CArmAxisDef axis, float fSpeed, float fPos)>SetPosAxis;

    //设置某个轴自动/手动控制
    std::function<ErrorCode(CArmAxisDef axis, CArmAxisCtrlDef ctrl)>SetManuAxis;

    //获取DAP的当前读数
    std::function<ErrorCode(DAPValue &fVal)>GetDAPValue;

    //启用/禁用DAP
    std::function<ErrorCode(bool bEnable)>EnableDAP;

    //获取当前使用的管电压
    std::function<ErrorCode(float &fKV)>GetKV;

    //获取当前使用的管电流
    std::function<ErrorCode(float &fMA)>GetMA;

    //获取激光灯的开关状态
    std::function<ErrorCode(OptionLaserSwitch opt, bool &bOn, float &fTime)>GetLaserStatus;

    //获取下位机主板温度
    std::function<ErrorCode(float &fTemp)>GetMainboardTemp;

    //发送3D扫描运动轨迹
    std::function<ErrorCode(int nNum, const float *pPoints)>Set3DTrack;

    //获取上次3D扫描的运动轨迹
    std::function<ErrorCode(WORD &nNum, float **ppPoints)>Get3DTrack;

    //暂停/恢复球管控制
    std::function<ErrorCode(bool bEnable)>EnableTubeCtrl;

#pragma endregion

#pragma region 设备进程发来的消息处理函数

    /// 设备状态报告函数
    std::function<void(ErrorCode err)> OnDevStatusChanged;

    /// 曝光手闸按下状态变化处理函数
    std::function<void(CArmExpBtnDef eBtnStep, bool bPressed)> OnExpBtnChanged;

    /// 脚踏板踏下状态变化处理函数
    std::function<void(CArmPedalDef eBtnStep, CArmExpBtnDef, bool bPressed)> OnPedalChanged;

    /// 各轴启停变化处理函数
    std::function<void(CArmAxisDef eAxis, CArmAxisMotionDef eStatus)> OnAxisStatusChanged;

    /// 限束器运动到位消息处理函数
    std::function<void(OptionCollimator coll, float fPos)> OnCArmCollimatorInPlace;

    /// 各轴自动手动状态变化处理函数
    std::function<void(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus)> OnCArmAxisManAutoChanged;

    /// 激光灯开关状态变化处理函数
    std::function<void(OptionLaserSwitch eLaser, bool bOn)> OnCArmLaserStatusChanged;

    /// 各轴位置变化时发出的实时位置信息处理函数
    std::function<void(CArmAxisDef eAxis, float fPos)> OnCArmAxisPosChanged;

    /// 设备复位完成后的回调函数
    std::function<void(ErrorCode)> OnCArmDevResetCompleted;
    std::function<void(bool)> OnCArmRasterStatusChanged;;

    /// 收到新影像
    std::function<void(int nIdx, int nWidth, int nHeight, unsigned short*pImageData)> OnNewImageReceived;

#pragma endregion

protected:

    /// 设备状态服务端口，用于与设备进程交互设备状态信息
    int m_nDevStatusPort;

    /// 扫描服务端口，用于与设备进程交互扫描相关的信息
    int m_nScanInfoPort;

    /// 心跳连接
    IPCClientBase *m_pHBConn;

    /// 设备状态服务器
    UDeivceStatusServer *m_pDevStatusServer;

    /// 扫描信息服务器
    UScanInfoServer *m_pScanInfoServer;

    void HeartbeatParser(std::list<std::string>) {};

    void HBCheckThread();
};

#endif//DEVICE_COMM_H