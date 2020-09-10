#ifndef SCAN_INFO_SERVER_H
#define SCAN_INFO_SERVER_H

#include <IPCServerBase.h>
#include <SlaveComputerCArmBase.h>
#include <IPC_CMD_Def_CArm.h>

class UScanInfoServer : public IPCServerBase
{
public:

    /**
    * @brief 构造函数
    * @param strPort 端口号
    * @param bSingleClient 是否单个客户端连接
    * @return  
    */
    UScanInfoServer(const std::string &strPort, bool bSingleClient = false);

    /**
    * @brief 析构函数
    * @return  
    */
    ~UScanInfoServer();

#pragma region 父类虚函数实现

    /**
    * @brief 解析收到的子进程发来的消息
    * @param soc 子进程连接的socket
    * @param strClientName 子进程名称
    * @param lstCmd 收到的消息
    * @return void 
    */
    virtual void ParseCmdsRecv(SOCKET soc, const std::string &strClientName, const std::vector<std::string> &lstCmd);

    /**
    * @brief 客户端与服务端建立连接后的处理
    * @param soc DICOM服务申请连接socket
    * @param strModuleName DICOM服务申请进程名称
    * @return void 
    */
    virtual void OnConnected(SOCKET soc, const std::string &strModuleName) { m_soc = soc; };

    /**
    * @brief 客户端与服务端连接中断时的处理
    * @param soc DICOM服务申请连接socket
    * @return void
    */
    virtual void OnDisconnected(SOCKET soc) { if(soc == m_soc) m_soc = INVALID_SOCKET; };

#pragma endregion

#pragma region 设备控制函数

    /**
    * @brief 开始2D扫描
    * @param bXRay 扫描时是否发射射线
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode StartScan2D(DevWorkMode m, bool bXRay);

    /**
    * @brief 开始3D扫描
    * @param direct C型臂旋转方向
    * @param bXRay 扫描时是否发射射线
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode StartScan3D(DevWorkMode m, OptionCArmMove direct, bool bXRay);

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

    /**
    * @brief 取消扫描
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode CancelScan();

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
    ErrorCode MoveAxisZ(OptionLiftColMove opt, float fSpeed);

    /**
    * @brief 控制前后轴开始连续运动
    * @param opt 运动方式
    * @param fSpeed 速度
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode MoveAxisX(OptionXAxisMove opt, float fSpeed);

    /**
    * @brief 控制C型臂开始连续运动
    * @param opt 运动方式
    * @param fSpeed 运动速度
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode MoveAxisY(OptionCArmMove opt, float fSpeed);

    /**
    * @brief 获取某个轴的当前位置（角度）
    * @param axis 轴
    * @param fVal 当前位置（角度）
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode GetPosAxis(CArmAxisDef axis, float &fVal);

    ErrorCode GetPosColl(OptionCollimator coll, float& fVal);

    ErrorCode SlaveScanCtrl(const SlaveScanCtrlType t);

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
    ErrorCode GetDAPValue(DAPValue &fVal);

    /**
    * @brief 查询设备电池电量
    * @param fVal 读数
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode GetBatteryPercent(float &fVal);

    /**
    * @brief 启用/禁用DAP
    * @param bEnable 是否启用
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode EnableDAP(bool bEnable);

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

    /**
    * @brief 获取当前球管的温度
    * @param fTemp 球管温度
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode GetBulbTubeTemp(float &fTemp);

    template <typename T>
    T ConvertToVal(const std::string& str);

    template <>
    float ConvertToVal(const std::string& str)
    {
        return std::stof(str);
    }
    template <>
    int ConvertToVal(const std::string& str)
    {
        return std::stoi(str);
    }

    template <>
    std::uint8_t ConvertToVal(const std::string& str)
    {
        return std::stoi(str);
    }

    ErrorCode GetRasterStatus(int& val)
    {
        return Get(CArmIPCCMD::GET_RASTER_STATUS, val);
    }

    template <typename T>
    ErrorCode Get(const std::string& strCMD, T& val)
    {
        if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_3IN1_DEV_NOT_READY;

        std::vector<std::string> vecResp;

        ErrorCode res = SendCmd2DevProc(strCMD, strCMD + "_RESP", {}, vecResp);
        if (IS_ERR(res)) return res;

        val = ConvertToVal<T>(vecResp[2]);

        return ErrorCode(std::stoi(vecResp[1]));
    }

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
    * @param bEnable true-恢复控制，false-暂停控制
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode EnableTubeCtrl(bool bEnable);

    /**
    * @brief 是否允许下位机放射线
    * @param bPermission true-允许放射线，false-不允许放射线
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode RadiationPermissionCtrl(bool bPermission);

#pragma endregion

#pragma region 设备进程发来的消息处理函数

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
    std::function<void(bool)> OnCArmRasterStatusChanged;

    /// 各轴位置变化时发出的实时位置信息处理函数
    std::function<void(CArmAxisDef eAxis, float fPos)> OnCArmAxisPosChanged;

    /// 设备复位完成后的回调函数
    std::function<void(ErrorCode)> OnCArmDevResetCompleted;

    /// 收到新影像
    std::function<void(int nIdx, int nWidth, int nHeight, unsigned short*pImageData)> OnNewImageReceived;

#pragma endregion

    bool IsClientConnected() { return m_soc != INVALID_SOCKET; }

private:

    /// 与设备进程的连接socket
    SOCKET m_soc;

    std::mutex m_mtxCmdMap;

    /// 向设备进程发送命令的句柄和反馈
    std::map<std::string, std::pair<HANDLE, std::vector<std::string>>> m_mapCmdResp;

    /**
     * @brief 向设备进程发送命令
     * @param strCmd 命令
     * @param strRespCmd 响应命令
     * @param vecParam 命令的参数
     * @param vecResp 返回消息
     * @return 
     */
    ErrorCode SendCmd2DevProc(const std::string &strCmd, const std::string &strRespCmd, const std::vector<std::string> &vecParam, std::vector<std::string> &vecResp);

    /**
     * @brief 输入的命令是否是界面进程发送给设备进程的返回
     * @param strCmd 命令
     * @return 是否
     */
    bool IsUIOriginatedCommand(const std::string &strCmd);
};

#endif//SCAN_INFO_SERVER_H