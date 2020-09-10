#ifndef DEV_C_ARM_H
#define DEV_C_ARM_H

#include <SystemConfigHandler.h>
#include <SlaveComputerCArmBase.h>

/**
* @brief 探测器插件库的API接口以及探测器访问基类对象
*/
class DetectorPlugin
{
public:
    /**
    * @brief 构造函数
    */
    DetectorPlugin(const IPCDeviceComponent *p)
    {
        pDetConfig = p;

        hDLL = NULL;

        apiInit = nullptr;
        apiReinit = nullptr;
        apiUninit = nullptr;
        apiDevInfo = nullptr;
        apiDevInst = nullptr;

        pDetector = nullptr;
    };

    /**
    * @brief 析构函数
    */
    ~DetectorPlugin()
    {
        Release();
    };

    /**
    * @brief 释放探测器插件库的资源，关闭探测器连接
    * @return 无
    */
    void Release()
    {
        if (pDetector != nullptr)
        {
            pDetector->OnDetectorError = nullptr;
            //pDetector->closeLink();
            pDetector = nullptr;
        }

        if (apiUninit != nullptr) apiUninit();

        if (hDLL != NULL)
        {
            FreeLibrary(hDLL);
            hDLL = NULL;
        }

        apiInit = nullptr;
        apiReinit = nullptr;
        apiUninit = nullptr;
        apiDevInfo = nullptr;
        apiDevInst = nullptr;
    }

    /**
    * @brief 探测器是否被启用
    * @return 探测器是否被启用
    *   @retval true 启用
    *   @retval false 未启用
    */
    inline bool Selected() { return pDetConfig != nullptr && pDetConfig->m_bSelected; }

    /**
    * @brief 获取探测器的当前状态
    * @return 探测器状态码
    *   @retval ErrorCode::ERR_SUCCESS 探测器连接正常
    *   @retval 其他 探测器未连接或工作状态错误
    */
    inline ErrorCode GetDetStatus() { return pDetector == nullptr ? ErrorCode::ERR_DT_NO_CONN : pDetector->GetStatus(); };

    /**
    * @brief 获取探测器信息
    * @return 状态码
    *   @retval ErrorCode::ERR_SUCCESS 成功获取
    *   @retval 其他 获取失败的错误码
    */
    ErrorCode GetDetInfo(DetectorInfoIPC &detInfo)
    {
        ErrorCode err = GetDetStatus();
        if (err != ErrorCode::ERR_SUCCESS) return err;

        return pDetector->getDetectorInfo(detInfo);
    };

    /// 探测器配置信息
    const IPCDeviceComponent *pDetConfig;

    /// 探测器插件库句柄
    HMODULE hDLL;

    PluginInitAPI apiInit;      //!< 初始化探测器SDK
    PluginReinitAPI apiReinit;  //!< 重新初始化探测器SDK
    PluginUninitAPI apiUninit;  //!< 释放探测器SDK资源
    PluginGetInfo apiDevInfo;   //!< 获取探测器信息的API
    PluginGetDetectorInstance apiDevInst;//!< 获取探测器通信接口对象的API

    UDetectorBase *pDetector;   //!< 探测器通信接口对象
};

class UDevCArm
{
public:
    UDevCArm(const IPCDeviceConfig &e, const std::string &strRawImgDir);
    ~UDevCArm();

    /**
    * @brief 加载设备用到的插件库并初始化设备上各个组件的连接
    * @return 以错误码的形式返回初始化状态
    *  @retval ErrorCode::ERR_SUCCESS 初始化成功
    *  @retval 其他 初始化错误
    */
    ErrorCode Init();

    /**
    * @brief 启动运行
    * @return 以错误码的形式返回启动运行结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode Run();

#pragma region 外部设置接口

    /**
     * @brief 开始2D扫描
     * @param bXRay 扫描时是否发射射线
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode StartScan2D(bool bXRay);

    /**
     * @brief 开始3D扫描
     * @param scanDirect C型臂旋转方向
     * @param bXRay 扫描时是否发射射线
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode StartScan3D(OptionCArmMove scanDirect, bool bXRay);

    /**
     * @brief 查询下位机当前状态
     * @return 下位机当前状态码
     */
    ErrorCode GetScStatus();

    /**
     * @brief 重置设备
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode ResetDevice();

    ErrorCode Request(const std::string& type)
    {
        return m_pSC->Request(type);
    }

    /**
    * @brief 重置设备报警状态
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode ResetWarn();

    /**
     * @brief 激光灯开关控制
     * @param opt 激光灯选项
     * @param bOn 开关
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode LaserSwitch(OptionLaserSwitch opt, bool bOn);

    /**
     * @brief 设置管电压
     * @param fKV 管电压
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetKV(float fKV);

    /**
     * @brief 设置管电流
     * @param fMA 管电流
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetMA(float fMA);

    /**
     * @brief 设置采集帧率
     * @param nFPS 帧率
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetFPS(int nFPS);

    /**
     * @brief 设置曝光时间
     * @param nExp 曝光时间（ms）
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetExposure(int nExp);

    /**
     * @brief 设置采集帧数
     * @param nFrameNum 帧数
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetFrameNum(int nFrameNum);

    /**
     * @brief 设置限束器位置
     * @param coll 限束器
     * @param fPos 目标位置
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetCollimator(OptionCollimator coll, float speed, float fPos);
    ErrorCode SetCollimatorContinueMove(OptionCollimator coll, OptionCollimatorMoveDirection d, float speed);
    /**
     * @brief 控制升降柱开始连续运动
     * @param opt 运动方式
     * @param fSpeed 运动速度
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode MoveAxis(OptionLiftColMove opt, float fSpeed);

    /**
     * @brief 控制前后轴开始连续运动
     * @param opt 运动方式
     * @param fSpeed 速度
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode MoveAxis(OptionXAxisMove opt, float fSpeed);

    /**
     * @brief 控制C型臂开始连续运动
     * @param opt 运动方式
     * @param fSpeed 运动速度
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode MoveAxis(OptionCArmMove opt, float fSpeed);

    /**
     * @brief 获取某个轴的当前位置（角度）
     * @param axis 轴
     * @param fVal 当前位置（角度）
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode GetPosAxis(CArmAxisDef axis, float &fVal);

    /**
     * @brief 控制某个轴以指定速度运动到指定位置
     * @param axis 轴
     * @param fSpeed 运动速度
     * @param fPos 目标位置
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetPosAxis(CArmAxisDef axis, float fSpeed, float fPos);

    /**
     * @brief 设置某个轴自动/手动控制
     * @param axis 轴
     * @param ctrl 控制选项
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode SetManuAxis(CArmAxisDef axis, CArmAxisCtrlDef ctrl);

    /**
     * @brief 获取DAP的当前读数
     * @param fVal 读数
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode GetDAPValue(float &fVal);

    ErrorCode SlaveScanCtrl(const SlaveScanCtrlType t);

    /**
     * @brief 启用/禁用DAP
     * @param bEnable 是否启用
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode EnableDAP(bool bEnable);

    /**
    * @brief 允许/不允许下位机放射线
    * @param bEnable 是否允许
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode RadiationPermission(bool bEnable);

    /**
     * @brief 获取当前使用的管电压
     * @param fKV 管电压
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode GetKV(float &fKV);

    /**
     * @brief 获取当前使用的管电流
     * @param fMA 管电流
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode GetMA(float &fMA);

    ErrorCode GetCollPos(OptionCollimator c, float& fVal);

    /**
    * @brief 获取当前的球管温度
    * @param fTemp 球管温度
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    //ErrorCode GetBulbTubeTemp(float &fTemp);

    //ErrorCode Get(const std::string& type, float& val);
    template <typename T>
    ErrorCode Get(const std::string& type, T& val)
    {
        return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->Get(type, val);
    }
   // ErrorCode Get(const std::string& type, std::uint8_t& val);

    /**
     * @brief 获取激光灯的开关状态
     * @param opt 激光灯
     * @param bOn 是否开关
     * @param fTime 距离自动关闭的剩余延时
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode GetLaserStatus(OptionLaserSwitch opt, bool &bOn, float &fTime);

    /**
     * @brief 获取下位机主板温度
     * @param fTemp 温度
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode GetMainboardTemp(float &fTemp);

    /**
     * @brief 发送3D扫描运动轨迹
     * @param nNum 轨迹点数
     * @param pPoints 轨迹数据
     * @return 以错误码的形式返回结果
     *  @retval ErrorCode::ERR_SUCCESS 成功
     *  @retval 其他 错误
     */
    ErrorCode Set3DTrack(int nNum, const float *pPoints);

    /**
    * @brief 获取上次3D扫描的运动轨迹
    * @param nNum 轨迹点数
    * @param pPoints 轨迹数据
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode Get3DTrack(WORD &nNum, float **ppPoints);

    /**
    * @brief 暂停/恢复球管控制
    * @param bEnable 暂停(false)/恢复(true)
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode EnableTubeCtrl(bool bEnable);

#pragma endregion

    /// 请求进程间通信服务的函数
    std::function<bool(const std::string &strServicePortType, const std::string &strCmd, std::string &strResp)> RequestService;

    /// 从探测器接收影像的函数
    std::function<void(int nIdx, const unsigned short *pImgData, int nImgWid, int nImgHgt, bool bLossed)> OnImageAcquired;

    /// 设备状态报告函数
    std::function<void(ErrorCode err)> OnDevStatusChanged;

    /// 曝光手闸按下状态变化处理函数
    std::function<void(CArmExpBtnDef eBtnStep, bool bPressed)> OnExpBtnChanged;

    /// 脚踏板踏下状态变化处理函数
    std::function<void(CArmPedalDef eBtnStep, CArmExpBtnDef,bool bPressed)> OnPedalChanged;

    /// 各轴启停变化处理函数
    std::function<void(CArmAxisDef eAxis, CArmAxisMotionDef eStatus)> OnAxisStatusChanged;

    /// 限束器运动到位消息处理函数
    std::function<void(OptionCollimator coll, float fPos)> OnCArmCollimatorInPlace;

    /// 各轴自动手动状态变化处理函数
    std::function<void(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus)> OnCArmAxisManAutoChanged;

    /// 激光灯开关状态变化处理函数
    std::function<void(OptionLaserSwitch eLaser, bool bOn)> OnCArmLaserStatusChanged;

    /// 通知上位机光栅开关变化的消息的回调函数
    std::function<void(bool)> OnCArmRasterStatusChanged;

    /// 各轴位置变化时发出的实时位置信息处理函数
    std::function<void(CArmAxisDef eAxis, float fPos)> OnCArmAxisPosChanged;

    /// 设备复位完成后的处理函数
    std::function<void(ErrorCode)> OnCArmDevResetCompleted;

private:

    /// 设备配置
    IPCDeviceConfig m_DevCfg;

    /// 下位机插件库句柄
    HMODULE m_hScDLL;

    /// 探测器插件参数
    DetectorPlugin m_Det;

    /// 下位机通信配置
    const IPCDeviceComponent *m_pScCfg;

    /// 下位机对象指针
    USlaveComputerCArmBase *m_pSC;

    /// 探测器状态
    ErrorCode m_eDetStatus;

    /// 下位机状态
    ErrorCode m_eScStatus;

    /**
    * @brief 获得某个组件测配置
    * @param e 组件类型
    * @return 组件配置
    */
    const IPCDeviceComponent *GetComponent(DeviceComponentType e);

    /**
    * @brief 从指定设备中获取指定组件测配置
    * @param e 组件类型
    * @param devCfg 设备配置
    * @return 组件配置
    */
    static const IPCDeviceComponent *GetComponent(DeviceComponentType e, const IPCDeviceConfig &devCfg);

    /**
    * @brief 初始化下位机连接
    */
    ErrorCode InitSlaveComputer();

    /**
    * @brief 初始化CT探测器连接
    * @param bReconn 本次初始化是否是重新初始化
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode InitDetector(bool bReconn = false);

    /**
    * @brief 加载插件库
    * @param strPlugin 插件库名称
    * @param pAPIInfo 获取插件信息的API
    * @param pAPIInst 获取插件实例的API
    * @return 插件库句柄
    *  @retval NULL 加载失败
    *  @retval !NULL 加载成功
    */
    HMODULE LoadPlugin(const std::string &strPlugin, PluginGetInfo *pAPIInfo, PluginGetInstance *pAPIInst);

    /**
    * @brief 加载探测器插件
    * @param dp 探测器插件参数
    * @return 是否加载成功
    */
    bool LoadDetectorPlugin(DetectorPlugin &dp);

    /**
    * @brief 从系统配置中查询探测器信息
    * @param detInfo 探测器信息
    * @return
    */
    ErrorCode QueryDetectorConfig(DetectorInfoIPC &detInfo);

    /**
     * @brief 探测器状态变化处理函数
     * @param e 新状态
     * @return 
     */
    void OnDetectorStatus(ErrorCode e);

    /**
     * @brief 持续连接探测器，直到连接成功
     * @return 
     */
    void DetectorThread();

    /**
     * @brief 从探测器收到一帧影像
     * @param nIdx 在本轮采集中的索引
     * @param pImgData 数据
     * @param nImgWid 影像宽度
     * @param nImgHgt 影像高度
     * @param bLossed 丢帧标识
     * @return 
     */
    void _onImageAcquired(int nIdx, unsigned short *pImgData, int nImgWid, int nImgHgt, bool bLossed);

#pragma region 下位机消息处理函数

    /**
     * @brief 主控板工作状态变化的处理函数
     * @param e 新状态
     * @return 
     */
    void OnScDevStatusChanged(ErrorCode e);

    /**
     * @brief 曝光手闸按下状态变化的处理函数
     * @param eBtnStep 曝光手闸级别
     * @param bPressed 手闸按下/松开， true-按下，false-松开
     * @return 
     */
    void OnScExpBtnChanged(CArmExpBtnDef eBtnStep, bool bPressed);

    /**
     * @brief 脚踏板踏下状态变化的处理函数
     * @param ePedal 脚踏板，1-左，2-右
     * @param bPressed 踏下/松开， true-踏下，false-松开
     * @return 
     */
    void OnScPedalChanged(CArmPedalDef ePedal, CArmExpBtnDef level, bool bPressed);

    /**
     * @brief 各轴运动状态变化（启停）的处理函数
     * @param eAxis 轴 
     * @param eStatus 新状态
     * @return 
     */
    void OnScAxisStatusChanged(CArmAxisDef eAxis, CArmAxisMotionDef eStatus);

    /**
     * @brief X射线开始发射
     * @return 
     */
    void OnScXRayReady();

    /**
     * @brief 设备报警的处理函数
     * @param e 错误码
     * @return 
     */
    void OnScCArmWarn(ErrorCode e);

    /**
     * @brief 下位机在限束器运动到位后发送位置信息的处理函数
     * @param coll 垂直/水平限束器
     * @param fPos 限束器位置
     * @return 
     */
    void OnScCArmCollimatorInPlace(OptionCollimator coll, float fPos);

    /**
     * @brief 下位机在各轴自动/手动控制状态变化时发送消息的处理函数
     * @param eAxis 三个轴
     * @param eStatus 自动/手动
     * @return 
     */
    void OnScCArmAxisManAutoChanged(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus);

    /**
     * @brief 下位机在激光灯开关状态变化时发送消息的处理函数
     * @param eLaser 激光灯
     * @param bOn 开关状态
     * @return 
     */
    void OnScCArmLaserStatusChanged(OptionLaserSwitch eLaser, bool bOn);

    /**
     * @brief 下位机在各轴发生位移时发送新位置的消息的处理函数
     * @param eAxis 三个轴
     * @param fPos 位置
     * @return 
     */
    void OnScCArmAxisPosChanged(CArmAxisDef eAxis, float fPos);

    /**
    * @brief 设备复位完成后的处理函数
    * @param res 复位结果
    * @return
    */
    void OnScCArmDevResetCompleted(ErrorCode res);

#pragma endregion

};

#endif // !DEV_C_ARM_H


