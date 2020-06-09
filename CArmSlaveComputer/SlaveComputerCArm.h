#ifndef SLAVE_COMPUTER_C_ARM_H
#define SLAVE_COMPUTER_C_ARM_H

#include <SlaveComputerCArmBase.h>
#include <TCPClient.h>
#include <list>
#include <map>
#include "CArmScCmdDef.h"

class USlaveComputerCArm : public USlaveComputerCArmBase
{
public:
    USlaveComputerCArm();
    ~USlaveComputerCArm();

#pragma region 外部接口
    /**
    * @brief 连接主控板
    * @param pLogger 日志对象指针
    * @param strIP 主控板IP地址
    * @param strPort 主控板服务端口
    * @return 状态码
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他值 失败
    */
    virtual ErrorCode Connect(Logger *pLogger, const std::string &strIP, const std::string& strPort);

    /**
    * @brief 断开连接，停止连接状态检查线程
    * @return 状态码
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他值 失败
    */
    virtual ErrorCode Disconnect();

    /**
    * @brief 查询主控板状态（是否连接/主控板工作状态）
    * @return
    */
    virtual ErrorCode GetStatus();

    /**
    * @brief 设备复位，重置设备（机械复位、将扫描参数恢复为默认值）
    * @return
    */
    virtual ErrorCode DeviceReset();

    /**
    * @brief 设备报警复位
    * @return
    */
    virtual ErrorCode WarnReset();

    /**
    * @brief 控制激光灯开关
    * @param opt 激光灯控制选项
    * @param bOn 开关控制
    * @return
    */
    virtual ErrorCode LaserSwitch(OptionLaserSwitch opt, bool bOn);

    /**
    * @brief 设置管电压
    * @param nKv 射线源管电压
    * @return
    */
    virtual ErrorCode SetKV(float fKv);

    /**
    * @brief 设置管电流
    * @param nMa 管电流
    * @return
    */
    virtual ErrorCode SetMA(float fMa);

    /**
    * @brief 设置影像采集帧率
    * @param nFPS 帧率
    * @return
    */
    virtual ErrorCode SetFrameRate(unsigned char nFPS);

    /**
    * @brief 设置曝光时间
    * @param nFPS 帧率
    * @return
    */
    virtual ErrorCode SetExposure(unsigned char nExp);

    /**
    * @brief 设置采集帧数
    * @param nFrameNum 帧数
    * @return
    */
    virtual ErrorCode SetFrameNum(unsigned short nFrameNum);

    /**
    * @brief 设置限束器位置
    * @param opt 选项： 0-垂直方向限束器，1-水平方向限束器
    * @param nPulse 限束器位置（相对于原点），单位mm
    * @return
    */
    virtual ErrorCode SetCollimator(OptionCollimator opt, float speed, float fPos);
    ErrorCode SetCollimatorContinueMove(OptionCollimator coll, OptionCollimatorMoveDirection d, float speed) override;
    /**
    * @brief 升降柱(z轴)运动的启停
    * @param opt 选项：0-停止，1-向上，2-向下
    * @param fSpeed 运动速度，单位mm/s
    * @return
    */
    virtual ErrorCode MoveLiftColomn(OptionLiftColMove opt, float fSpeed);

    /**
    * @brief 前后轴(x轴)运动的启停
    * @param opt 选项：0-停止，1-向前，2-向后
    * @param fSpeed 运动速度，单位mm/s
    * @return
    */
    virtual ErrorCode MoveXAxis(OptionXAxisMove opt, float fSpeed);

    /**
    * @brief C型臂转动的启停
    * @param opt 选项：0-停止，1-顺时针，2-逆时针
    * @param fSpeed 角速度，单位rad/s
    * @return
    */
    virtual ErrorCode MoveCArm(OptionCArmMove opt, float fSpeed);

    /**
    * @brief 查询升降柱（z轴）的当前位置
    * @param fPos 相对于z轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode GetLiftColumnPos(float &fPos);

    /**
    * @brief 查询前后轴（x轴）的当前位置
    * @param fPos 相对于x轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode GetXAxisPos(float &fPos);

    /**
    * @brief 查询C型臂的当前角度
    * @param fAng C型臂的当前角度
    * @return
    */
    virtual ErrorCode GetCArmAngle(float &fAng);
    ErrorCode GetCollPos(OptionCollimator c, float& fVal) override;

    /**
    * @brief 控制C型臂转到指定角度
    * @param fSpeed 转动的角速度，单位red/s
    * @param fAng 目的角度
    * @return
    */
    virtual ErrorCode SetCArmAngle(float fSpeed, float fAng);

    /**
    * @brief 控制前后轴（x轴）运动到指定位置
    * @param fSpeed 运动速度，单位mm/s
    * @param fPos 相对于x轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode SetXAxisPos(float fSpeed, float fPos);

    /**
    * @brief 控制升降柱（z轴）运动到指定位置
    * @param fSpeed 运动速度，单位mm/s
    * @param fPos 相对于z轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode SetLiftColumnPos(float fSpeed, float fPos);

    /**
    * @brief 设置C型臂控制模式
    * @param m 自动/手动
    * @return
    */
    virtual ErrorCode SetCArmCtrlMode(CArmAxisCtrlDef m);

    /**
    * @brief 设置升降柱（z轴）控制模式
    * @param m 自动/手动
    * @return
    */
    virtual ErrorCode SetLiftColumnCtrlMode(CArmAxisCtrlDef m);

    /**
    * @brief 设置前后轴（x轴）控制模式
    * @param m 自动/手动
    * @return
    */
    virtual ErrorCode SetXAxisCtrlMode(CArmAxisCtrlDef m);

    /**
    * @brief 获取当前DAP读数
    * @param fDapVal DAP读数
    * @return
    */
    virtual ErrorCode GetCurrentDapValue(float &fDapVal);

    ErrorCode SlaveScanCtrl(const SlaveScanCtrlType t) override;

    /**
    * @brief 设置3D扫描轨迹数据
    * @param u16Num 轨迹点数
    * @param pPoints 轨迹点，每个轨迹点包含三个轨迹数据（C型臂角度、x轴位置、z轴位置），因此pPoints包含u16Num*3个float
    * @return
    */
    virtual ErrorCode Send3DTrackData(unsigned short u16Num, const float *pPoints);

    /**
    * @brief 获取上次扫描中记录的实际3轴运动轨迹数据
    * @param u16Num 轨迹点数
    * @param ppPoints 轨迹点，每个轨迹点包含三个轨迹数据（C型臂角度、x轴位置、z轴位置），因此pPoints包含u16Num*3个float
    * @return
    */
    virtual ErrorCode Retrieve3DTrackData(unsigned short &u16Num, float **ppPoints);

    /**
    * @brief 设备不动，开始2D扫描，设备按照当前的采集设置开始扫描
    * @param bXRay 是否发射X射线
    * @return
    */
    virtual ErrorCode Start2DScan(bool bXRay);

    ErrorCode Get(const std::string& type, float& val) override;
    ErrorCode Get(const std::string& type, std::uint8_t& val) override;
    ErrorCode Get(const std::string& type, DAPValue& val) override;

    template <typename T>
    ErrorCode GetVal(const std::string& type, T& val)
    {
        if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

        const std::map<std::string, int> mymap
        {
            { CArmIPCCMD::GET_BATTERY_PERCENT , SC_CARM_GET_BATTERY_PERCENT },
            { CArmIPCCMD::GET_Bulb_Tube_Temp, SC_CARM_GET_BULB_TUBE_TEMP },
            { CArmIPCCMD::GET_RASTER_STATUS, SC_CARM_GET_RASTER_STATUS },
            { CArmIPCCMD::GET_DAP_VALUE, SC_CARM_GET_DAP_VALUE }
        };

        if (std::end(mymap) == mymap.find(type))
        {
            return ErrorCode::ERR_SC_FUNC_UNSUPPORTED;
        }

        CArmCmdNoParam cmd(mymap.at(type));
        auto res = SendCmd2Sc(&cmd, L"to get val");
        if (IS_ERR(res)) return res;

        CArmRawCmd recvCmd;
        {
            std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
            recvCmd = m_CmdRecv[cmd.m_u16MsgID];
            m_CmdRecv.erase(cmd.m_u16MsgID);
        }

        val = *(T*)recvCmd.m_pu8Data;

        delete[] recvCmd.m_pu8Data;

        return ErrorCode::ERR_SUCCESS;
    }

    ErrorCode Request(const std::string& type);

    ErrorCode CancelScan();

    /**
    * @brief 允许/不允许下位机放射线
    * @param bEnable 是否允许
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    ErrorCode RadiationPermission(bool bEnable) override;

    /**
    * @brief 设备开始走3D轨迹扫描
    * @param dr 旋转方向
    * @param bXRay 扫描中是否发射X射线
    * @return
    */
    virtual ErrorCode Start3DScan(OptionCArmMove dr, bool bXRay);

    /**
    * @brief 启用/禁用DAP
    * @param bEnable 是否使用DAP，true-使用，false-禁用
    * @return
    */
    virtual ErrorCode EnableDAP(bool bEnable);

    /**
    * @brief 从下位机获取当前使用的管电压
    * @param fKV 读取到的管电压
    * @return
    */
    virtual ErrorCode GetCurrentKV(float &fKV);

    /**
    * @brief 从下位机获取当前使用的管电流
    * @param fMA 读取到的管电流
    * @return
    */
    virtual ErrorCode GetCurrentMA(float &fMA);

    /**
    * @brief 从下位机获取球管的温度
    * @param fTemp 读取到的管电流
    * @return
    */
    //virtual ErrorCode GetBulbTubeTemp(float &fTemp);

    /**
    * @brief 从下位机获取激光灯开关状态
    * @param lsr 激光灯，OptionLaserSwitch.CrossCurve/OptionLaserSwitch.ViewRange
    * @param bOn 打开/关闭状态
    * @param fRemainingDelay 剩余关闭延时，单位s
    * @return
    */
    virtual ErrorCode GetLaserStatus(OptionLaserSwitch lsr, bool &bOn, float &fRemainingDelay);

    /**
    * @brief 从下位机获取设备主板的当前温度
    * @param fCentigrade 摄氏度
    * @return
    */
    virtual ErrorCode GetMainboardTemp(float &fCentigrade);

    /**
    * @brief 通知下位机暂停/恢复对球管的控制
    * @param bEnable true-恢复控制，false-暂停控制
    * @return
    */
    virtual ErrorCode EnableTubeCtrl(bool bEnable);

#pragma endregion

private:

#pragma region 私有成员变量

    /// 下位机IP地址
    std::string m_strIP;

    /// 下位机端口
    std::string m_strPort;

    /// 与下位机的socket连接
    CTCPClient m_socClient;

    /// 主动关闭连接，不再重新连接
    bool m_bShutdown;

    /// 发送消息与其句柄映射的互斥量
    std::mutex m_mtxCmdSend;

    std::map<WORD, std::map<WORD, HANDLE>> m_CmdMap;//!< 消息ID与消息句柄的映射,std::map<消息标识符, std::map<消息ID, 消息句柄>>

    /// 返回消息与其句柄映射的互斥量
    std::mutex m_mtxCmdRecv;

    /// 上位机发送消息的返回消息,WORD为发送消息的ID
    std::map<WORD, CArmRawCmd> m_CmdRecv;

#pragma endregion

#pragma region 私有成员函数

    /**
     * @brief 下位机连接线程，持续尝试连接，直到连接成功
     * @return 
     */
    void ConnectThread();

    /**
    * @brief 从下位机持续接收数据的线程
    * @return
    */
    void ReceiveThread();

    /**
     * @brief 从接收到数据中提取完整消息
     * @param recvBuff 接收到的数据
     * @return 提取到的消息列表
     */
    std::list<CArmRawCmd> ExtractCommands(std::vector<BYTE> &recvBuff);

    /**
     * @brief 解析从下位机收到的消息
     * @param cmds 从下位机收到的消息
     * @return 
     */
    void ParseCommands(std::list<CArmRawCmd> cmds);

    /**
     * @brief 向下位机发送消息的通用接口
     * @param pCmd 向下位机发送的消息
     * @param strLog 发送消息时的日志头
     * @return 
     */
    ErrorCode SendCmd2Sc(CArmCmdBase *pCmd, const std::wstring &strLog);

    /**
     * @brief 从下位机返回的状态码转为系统状态码
     * @param devSts 从下位机查询到的当前工作状态码
     * @return 系统状态码
     */
    ErrorCode ToSysStatusCode(WORD devSts);

    /**
     * @brief 从下位机返回的操作结果码转为系统错误码
     * @param operSts 下位机返回的操作结果码
     * @return 
     */
    ErrorCode ToSysErrorCode(WORD operSts);

#pragma endregion

};

#endif//SLAVE_COMPUTER_C_ARM_H