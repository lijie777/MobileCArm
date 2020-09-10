#ifndef DEV_C_ARM_PROC_H
#define DEV_C_ARM_PROC_H

#include <SubProcessBase.h>
#include "DevCArm.h"
#include <IPC_CMD_Def_CArm.h>

class UDevCArmProc :public USubProcessBase
{
public:
    UDevCArmProc();
    ~UDevCArmProc();

    /**
     * @brief 解析输入参数
     * @param argc 参数数量
     * @param argv 参数字符串数组
     * @return 
     */
    virtual bool ArgsParse(int argc, char *argv[]);

    /**
     * @brief 连接父进程心跳服务
     * @return 
     */
    virtual bool StartHeartbeat();

    /**
     * @brief 连接日志服务(包括获取日志端口)
     * @return 
     */
    virtual bool StartLogging();

    /**
     * @brief 获取服务端口（包括请求分配自身提供的服务和其他进程提供的服务（不包含日志服务））
     * @return 
     */
    virtual bool GetServicePorts() { return true; };

    /**
     * @brief 启动自身提供的服务
     * @return 
     */
    virtual bool StartServices() { return true; };

    /**
     * @brief 开始运行，如果此函数退出则进程退出
     * @return 
     */
    virtual bool Run();

    /**
    * @brief 查询心跳连接是否还在
    * @return 是否成功
    */
    bool IsAlive();

private:

    /// 设备ID
    std::string m_strDeviceID;

    /// 心跳连接
    IPCClientBase *m_pConnHeartBeat;

    /// 设备状态连接
    IPCClientBase *m_pConnDevStatus;

    /// 扫描通信连接
    IPCClientBase *m_pConnScanComm;

    /// 设备对象
    UDevCArm *m_pDev;

    /// 影像数据内存映射的互斥量
    std::mutex m_mtxMemMap;

    /// 影像数据内存映射
    std::map<std::string, HANDLE> m_ImgMemMap;

    /// 设备的最新状态
    ErrorCode m_eLatestDevStatus;

    /**
    * @brief 心跳连接发来的消息处理函数
    * @param cmds 心跳连接发来的消息
    * @return 无
    */
    void HeartbeatChecker(std::list<std::string> cmds) {};

    /**
     * @brief 设备状态服务连接监控线程
     * @return 
     */
    void DevStatusUpdateThread();

    /**
    * @brief 扫描通信连接监控线程
    * @return
    */
    void ScanCommThread();

    /**
    * @brief 设备状态更新服务消息处理
    * @param lstCmds 设备状态更新服务消息
    * @return 无
    */
    void OnDevStatusCmdRecv(std::list<std::string> lstCmds);

    /**
    * @brief 扫描服务消息处理
    * @param lstCmds 扫描服务消息
    * @return 无
    */
    void OnScanStatusCmdRecv(std::list<std::string> lstCmds);

#pragma region 设备对象回调函数

    /**
    * @brief 设备更新状态
    * @param err 错误码
    * @return 无
    */
    void OnDevStatusChanged(ErrorCode c);

    /**
    * @brief 设备采集到一帧影像
    * @param nIdx 影像索引
    * @param pImgData 影像数据
    * @param nImgWid 影像宽度
    * @param nImgHgt 影像高度
    * @param bLossed 是否丢帧
    * @return 无
    */
    void OnImageAcquired(int nIdx, const unsigned short *pImgData, int nImgWid, int nImgHgt, bool bLossed);

    /**
     * @brief 曝光手闸按下状态变化处理函数
     * @param eBtnStep 手闸按钮级别
     * @param bPressed  是否按下
     * @return 
     */
    void OnExpBtnChanged(CArmExpBtnDef eBtnStep, bool bPressed);

    /**
     * @brief 脚踏板踏下状态变化处理函数
     * @param eBtnStep 脚踏
     * @param bPressed 是否踏下
     * @return 
     */
    void OnPedalChanged(CArmPedalDef eBtnStep, CArmExpBtnDef level, bool bPressed);

    /**
     * @brief 各轴启停变化处理函数
     * @param eAxis 轴
     * @param eStatus 运动状态
     * @return 
     */
    void OnAxisStatusChanged(CArmAxisDef eAxis, CArmAxisMotionDef eStatus);

    /**
     * @brief 限束器运动到位消息处理函数
     * @param coll 限束器
     * @param fPos 当前位置
     * @return 
     */
    void OnCArmCollimatorInPlace(OptionCollimator coll, float fPos);

    /**
     * @brief 各轴自动手动状态变化处理函数
     * @param eAxis 轴
     * @param eStatus 自动/手动
     * @return 
     */
    void OnCArmAxisManAutoChanged(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus);

    /**
     * @brief 激光灯开关状态变化处理函数
     * @param eLaser 激光灯
     * @param bOn 开/关
     * @return 
     */
    void OnCArmLaserStatusChanged(OptionLaserSwitch eLaser, bool bOn);

    /**
     * @brief 各轴位置变化时发出的实时位置信息处理函数
     * @param eAxis 轴
     * @param fPos 当前位置
     * @return 
     */
    void OnCArmAxisPosChanged(CArmAxisDef eAxis, float fPos);

    /**
    * @brief 设备复位完成后的处理函数
    * @param res 复位结果
    * @return
    */
    void OnCArmDevResetCompleted(ErrorCode res);

    /// 通知上位机光栅开关变化的消息的回调函数
    void OnCArmRasterStatusChanged(const bool open_);

#pragma endregion

    /**
    * @brief 根据设备ID从Launcher进程查询设备信息
    * @param strDevID 设备ID
    * @param devConfig 查询到的设备配置信息
    * @return 是否查询成功
    */
    bool GetDeviceConfig(const std::string &strDevID, IPCDeviceConfig &devConfig);

    /**
    * @brief 获取原始影像存储路径
    * @return 原始影像存储路径
    */
    std::string GetRawImageDir();

    /**
     * @brief 为下位机设置扫参数
     * @return 
     */
    ErrorCode SetScanParam();

    /**
     * @brief 开始2D扫描
     * @param workMode 设备工作模式
     * @param bXRay 扫描过程中是否发射射线（演示模式不放线）
     * @return 
     */
    ErrorCode StartScan2D(DevWorkMode workMode, bool bXRay);

    /**
     * @brief 开始3D扫描
     * @param workMode 设备工作模式
     * @param opt 扫描方向（逆时针/顺时针）
     * @param bXRay 扫描过程中是否发射射线（演示模式不放线）
     * @return 
     */
    ErrorCode StartScan3D(DevWorkMode workMode, OptionCArmMove opt, bool bXRay);

    /**
     * @brief 检查消息是否不需要处理
     * @param strCmd 消息
     * @return 是否需要处理
     */
    bool IsIgnoredCmd(const std::string &strCmd);
};

#endif // !DEV_C_ARM_PROC_H


