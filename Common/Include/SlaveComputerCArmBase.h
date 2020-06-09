#ifndef SLAVE_COMPUTER_C_ARM_BASE_H
#define SLAVE_COMPUTER_C_ARM_BASE_H

#include <log4cplus/loggingmacros.h>
#include <ErrorCode.h>
#include <functional>
#include "CArmEnum.h"
#include "IPC_CMD_Def_CArm.h"

using namespace log4cplus;
using namespace log4cplus::helpers;

class USlaveComputerCArmBase
{
public:

    USlaveComputerCArmBase() {};
    virtual ~USlaveComputerCArmBase() {};

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
    virtual ErrorCode Connect(Logger *pLogger, const std::string &strIP, const std::string& strPort) = 0;

    /**
    * @brief 断开连接，停止连接状态检查线程
    * @return 状态码
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他值 失败
    */
    virtual ErrorCode Disconnect() = 0;
    virtual ErrorCode GetCollPos(OptionCollimator c, float& fVal) = 0;

    /**
     * @brief 查询主控板状态（是否连接/主控板工作状态）
     * @return 
     */
    virtual ErrorCode GetStatus() = 0;

    /**
    * @brief 设备复位，重置设备（机械复位、将扫描参数恢复为默认值）
    * @return
    */
    virtual ErrorCode DeviceReset() = 0;

    virtual ErrorCode SlaveScanCtrl(const SlaveScanCtrlType t) = 0;

    /**
    * @brief 设备报警复位
    * @return
    */
    virtual ErrorCode WarnReset() = 0;

    /**
     * @brief 控制激光灯开关
     * @param opt 激光灯控制选项 
     * @param bOn 开关控制
     * @return 
     */
    virtual ErrorCode LaserSwitch(OptionLaserSwitch opt, bool bOn) = 0;

    /**
    * @brief 设置管电压
    * @param nKv 射线源管电压
    * @return
    */
    virtual ErrorCode SetKV(float fKv) = 0;

    /**
    * @brief 设置管电流
    * @param nMa 管电流
    * @return
    */
    virtual ErrorCode SetMA(float fMa) = 0;

    /**
    * @brief 设置影像采集帧率
    * @param nFPS 帧率
    * @return
    */
    virtual ErrorCode SetFrameRate(unsigned char nFPS) = 0;
    virtual ErrorCode Get(const std::string& type, float& val) = 0;
    virtual ErrorCode Get(const std::string& type, std::uint8_t& val) = 0;
    virtual ErrorCode Get(const std::string& type, DAPValue& val) = 0;

    /**
    * @brief 设置曝光时间
    * @param nFPS 帧率
    * @return
    */
    virtual ErrorCode SetExposure(unsigned char nExp) = 0;


    /**
    * @brief 允许/不允许下位机放射线
    * @param bEnable 是否允许
    * @return 以错误码的形式返回结果
    *  @retval ErrorCode::ERR_SUCCESS 成功
    *  @retval 其他 错误
    */
    virtual ErrorCode RadiationPermission(bool bEnable) = 0;

    /**
    * @brief 设置采集帧数
    * @param nFrameNum 帧数
    * @return
    */
    virtual ErrorCode SetFrameNum(unsigned short nFrameNum) = 0;

    /**
    * @brief 设置限束器位置
    * @param opt 选项： 0-垂直方向限束器，1-水平方向限束器
    * @param nPulse 限束器位置（相对于原点），单位mm
    * @return
    */
    virtual ErrorCode SetCollimator(OptionCollimator opt, float speed, float fPos) = 0;
    virtual ErrorCode SetCollimatorContinueMove(OptionCollimator coll, OptionCollimatorMoveDirection d, float speed) = 0;
    /**
    * @brief 升降柱(z轴)运动的启停
    * @param opt 选项：0-停止，1-向上，2-向下
    * @param fSpeed 运动速度，单位mm/s
    * @return
    */
    virtual ErrorCode MoveLiftColomn(OptionLiftColMove opt, float fSpeed) = 0;

    /**
    * @brief 前后轴(x轴)运动的启停
    * @param opt 选项：0-停止，1-向前，2-向后
    * @param fSpeed 运动速度，单位mm/s
    * @return
    */
    virtual ErrorCode MoveXAxis(OptionXAxisMove opt, float fSpeed) = 0;

    /**
    * @brief C型臂转动的启停
    * @param opt 选项：0-停止，1-顺时针，2-逆时针
    * @param fSpeed 角速度，单位rad/s
    * @return
    */
    virtual ErrorCode MoveCArm(OptionCArmMove opt, float fSpeed) = 0;

    /**
    * @brief 查询升降柱（z轴）的当前位置
    * @param fPos 相对于z轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode GetLiftColumnPos(float &fPos) = 0;

    /**
    * @brief 查询前后轴（x轴）的当前位置
    * @param fPos 相对于x轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode GetXAxisPos(float &fPos) = 0;

    /**
    * @brief 查询C型臂的当前角度
    * @param fAng C型臂的当前角度
    * @return
    */
    virtual ErrorCode GetCArmAngle(float &fAng) = 0;

    /**
    * @brief 控制C型臂转到指定角度
    * @param fSpeed 转动的角速度，单位red/s
    * @param fAng 目的角度
    * @return
    */
    virtual ErrorCode SetCArmAngle(float fSpeed, float fAng) = 0;

    /**
    * @brief 控制前后轴（x轴）运动到指定位置
    * @param fSpeed 运动速度，单位mm/s
    * @param fPos 相对于x轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode SetXAxisPos(float fSpeed, float fPos) = 0;

    /**
    * @brief 控制升降柱（z轴）运动到指定位置
    * @param fSpeed 运动速度，单位mm/s
    * @param fPos 相对于z轴原点的距离，单位mm
    * @return
    */
    virtual ErrorCode SetLiftColumnPos(float fSpeed, float fPos) = 0;

    /**
    * @brief 设置C型臂控制模式
    * @param m 自动/手动
    * @return
    */
    virtual ErrorCode SetCArmCtrlMode(CArmAxisCtrlDef m) = 0;

    /**
    * @brief 设置升降柱（z轴）控制模式
    * @param m 自动/手动
    * @return
    */
    virtual ErrorCode SetLiftColumnCtrlMode(CArmAxisCtrlDef m) = 0;

    /**
    * @brief 设置前后轴（x轴）控制模式
    * @param m 自动/手动
    * @return
    */
    virtual ErrorCode SetXAxisCtrlMode(CArmAxisCtrlDef m) = 0;

    /**
    * @brief 获取当前DAP读数
    * @param fDapVal DAP读数
    * @return
    */
    virtual ErrorCode GetCurrentDapValue(float &fDapVal) = 0;

    /**
    * @brief 设置3D扫描轨迹数据
    * @param u16Num 轨迹点数
    * @param pPoints 轨迹点，每个轨迹点包含三个轨迹数据（C型臂角度、x轴位置、z轴位置），因此pPoints包含u16Num*3个float
    * @return
    */
    virtual ErrorCode Send3DTrackData(unsigned short u16Num, const float *pPoints) = 0;

    /**
    * @brief 获取上次扫描中记录的实际3轴运动轨迹数据
    * @param u16Num 轨迹点数
    * @param ppPoints 轨迹点，每个轨迹点包含三个轨迹数据（C型臂角度、x轴位置、z轴位置），因此pPoints包含u16Num*3个float
    * @return
    */
    virtual ErrorCode Retrieve3DTrackData(unsigned short &u16Num, float **ppPoints) = 0;

    /**
    * @brief 设备不动，开始2D扫描，设备按照当前的采集设置开始扫描
    * @param bXRay 是否发射X射线
    * @return
    */
    virtual ErrorCode Start2DScan(bool bXRay) = 0;

    /**
    * @brief 设备开始走3D轨迹扫描
    * @param dr 旋转方向
    * @param bXRay 扫描中是否发射X射线
    * @return
    */
    virtual ErrorCode Start3DScan(OptionCArmMove dr, bool bXRay) = 0;

    /**
    * @brief 启用/禁用DAP
    * @param bEnable 是否使用DAP，true-使用，false-禁用
    * @return
    */
    virtual ErrorCode EnableDAP(bool bEnable) = 0;

    /**
    * @brief 从下位机获取当前使用的管电压
    * @param fKV 读取到的管电压
    * @return
    */
    virtual ErrorCode GetCurrentKV(float &fKV) = 0;

    /**
    * @brief 从下位机获取当前使用的管电流
    * @param fMA 读取到的管电流
    * @return
    */
    virtual ErrorCode GetCurrentMA(float &fMA) = 0;

    /**
    * @brief 从下位机获取球管的温度
    * @param fTemp 读取到的管电流
    * @return
    */
    //virtual ErrorCode GetBulbTubeTemp(float &fTemp) = 0;

    /**
    * @brief 从下位机获取激光灯开关状态
    * @param lsr 激光灯，OptionLaserSwitch.CrossCurve/OptionLaserSwitch.ViewRange
    * @param bOn 打开/关闭状态
    * @param fRemainingDelay 剩余关闭延时，单位s
    * @return
    */
    virtual ErrorCode GetLaserStatus(OptionLaserSwitch lsr, bool &bOn, float &fRemainingDelay) = 0;

    /**
    * @brief 从下位机获取设备主板的当前温度
    * @param fCentigrade 摄氏度
    * @return
    */
    virtual ErrorCode GetMainboardTemp(float &fCentigrade) = 0;

    /**
    * @brief 通知下位机暂停/恢复对球管的控制
    * @param bEnable true-恢复控制，false-暂停控制
    * @return
    */
    virtual ErrorCode EnableTubeCtrl(bool bEnable) = 0;

    virtual ErrorCode Request(const std::string& type) = 0;

#pragma endregion

#pragma region 回调函数

    /// 主控板工作状态变化的回调函数
    std::function<void(ErrorCode)> OnDevStatusChanged;

    /// 曝光手闸按下状态变化的回调函数
    std::function<void(CArmExpBtnDef, bool)> OnExpBtnChanged;

    /// 脚踏板踏下状态变化的回调函数
    std::function<void(CArmPedalDef, CArmExpBtnDef, bool)> OnPedalChanged;

    /// 各轴运动状态变化（启停）的回调函数
    std::function<void(CArmAxisDef, CArmAxisMotionDef)> OnAxisStatusChanged;

    /// X射线开始发射
    std::function<void(void)> OnXRayReady;

    /// 设备报警的回调函数
    std::function<void(ErrorCode)> OnCArmWarn;

    /// 下位机在限束器运动到位后发送位置信息的回调函数
    std::function<void(OptionCollimator, float)> OnCArmCollimatorInPlace;

    /// 下位机在各轴自动/手动控制状态变化时发送消息的回调函数
    std::function<void(CArmAxisDef, CArmAxisCtrlDef)> OnCArmAxisManAutoChanged;

    /// 下位机在激光灯开关状态变化时发送消息的回调函数,bool=true:打开，bool=false:关闭
    std::function<void(OptionLaserSwitch, bool)> OnCArmLaserStatusChanged;

    /// 下位机在各轴发生位移时发送新位置的消息的回调函数
    std::function<void(CArmAxisDef, float)> OnCArmAxisPosChanged;

    /// 通知上位机光栅开关变化的消息的回调函数
    std::function<void(bool)> OnCArmRasterStatusChanged;

    /// 设备复位完成后的回调函数
    std::function<void(ErrorCode)> OnCArmDevResetCompleted;

#pragma endregion

private:

};

#endif//SLAVE_COMPUTER_C_ARM_BASE_H