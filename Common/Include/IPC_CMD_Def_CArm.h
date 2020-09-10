#ifndef IPC_CMD_DEF_CARM_H
#define IPC_CMD_DEF_CARM_H

#include <string>
#include <map>

enum class DevWorkMode
{
    /// 正常工作模式
    /// 扫描参数由界面进程填入共享内存
    /// 设备进程收到开始扫描消息后，从共享内存中获取扫描参数，然后逐个发给下位机。
    Normal = 0,

    /// 调试模式
    /// 扫描参数由界面进程/调试软件逐个发消息到设备进程
    /// 设备进程收到扫描消息后，直接通知下位机用之前逐个设置的扫描参数进行扫描
    Debug = 1
};

#pragma pack(push)
#pragma pack(1)
struct DAPValue
{
    float Gy_per_min;   //!< 每分钟辐射剂量
    float Gy;           //!< 每点的剂量值
    float DAP;          //!< 总剂量值
};
#pragma pack(pop)

// 下位机扫描控制类型
enum class SlaveScanCtrlType : std::uint8_t
{
    SendScanParam,      //!< 向下位机发送拍片参数
    Send2DScanCmd_XRay,      //!< 发送静止中扫描命令（放射线）
    XRayAllow,          //!< 允许下位机发射线
    XRayNotAllow,       //!< 不允许下位机发射线
    CancelScan          //!< 取消扫描
};

static std::string ToString(const SlaveScanCtrlType t)
{
    std::map<SlaveScanCtrlType, std::string> mymap
    {
        { SlaveScanCtrlType::Send2DScanCmd_XRay, "Send2DScanCmd_XRay" },
        { SlaveScanCtrlType::SendScanParam, "SendScanParam"},
        { SlaveScanCtrlType::XRayAllow, "XRayAllow"},
        { SlaveScanCtrlType::XRayNotAllow, "XRayNotAllow"},
        { SlaveScanCtrlType::CancelScan, "CancelScan"}
    };
    return mymap[t];
}

class CArmIPCCMD
{
public:

    /**
    * @brief 设备进程向界面进程报告当前设备状态
    *
    * 参数1:设备的新状态，ErrorCode
    */
    const static std::string UPDATE_DEV_STATUS;

    /**
    * @brief 获取下位机当前工作状态
    *
    * 参数1:无
    */
    const static std::string GET_SC_STATUS;

    /**
    * @brief 上述消息的响应
    *
    * 参数1: 下位机工作状态，ErrorCode
    */
    const static std::string GET_SC_STATUS_RESP;

    /**
    * @brief 设备复位
    *
    * 参数: 无
    */
    const static std::string DEV_RESET;

    /**
    * @brief 设备复位消息的响应
    *
    * 参数: 无
    */
    const static std::string DEV_RESET_RESP;

    /**
    * @brief 取消扫描
    *
    * 参数: 无
    */
    const static std::string CANCEL_SCAN;

    /**
    * @brief 取消扫描消息的响应
    *
    * 参数: 无
    */
    const static std::string CANCEL_SCAN_RESP;

    /**
    * @brief 报警复位
    *
    * 参数: 无
    */
    const static std::string WARN_RESET;

    /**
    * @brief 报警复位的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string WARN_RESET_RESP;

    /**
    * @brief 激光灯开关
    *
    * 参数1: 选项 OptionLaserSwitch
    *
    * 参数2: 1开/0关
    */
    const static std::string LASER_SWITCH;

    /**
    * @brief 激光灯开关的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string LASER_SWITCH_RESP;

    /**
    * @brief 设置管电压
    *
    * 参数1: 管电压
    */
    const static std::string SET_KV;

    /**
    * @brief 设置管电压的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_KV_RESP;

    /**
    * @brief 设置管电流
    *
    * 参数1: 管电流
    */
    const static std::string SET_MA;

    /**
    * @brief 设置管电流的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_MA_RESP;

    /**
    * @brief 设置采集帧率
    *
    * 参数1: 采集帧率
    */
    const static std::string SET_FPS;

    /**
    * @brief 设置采集帧率的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_FPS_RESP;

    /**
    * @brief 设置单帧曝光时间
    *
    * 参数1: 曝光时间(ms)
    */
    const static std::string SET_EXPOSURE;

    /**
    * @brief 设置单帧曝光时间
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_EXPOSURE_RESP;

    /**
    * @brief 设置采集帧数
    *
    * 参数1: 采集帧数
    */
    const static std::string SET_FRAME_NUM;

    /**
    * @brief 设置采集帧数的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_FRAME_NUM_RESP;

    /**
    * @brief 设置水平方向限束器位置
    *
    * 参数1: 限速器的运动速度
    * 参数2: 水平方向限束器位置
    */
    const static std::string SET_HOR_COLL;

    /**
    * @brief 设置水平方向限束器位置的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_HOR_COLL_RESP;

    /**
    * @brief 设置限束器连续运动
    *
    * 参数1: 限速器的水平或垂直
    * 参数2: 限速器的运动方向
    * 参数2: 限速器的运动速度
    */
    const static std::string SET_COLL_CONTINUE_MOVE;

    /**
    * @brief 设置限束器连续运动的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_COLL_CONTINUE_MOVE_RESP;

    /**
    * @brief 设置垂直方向限束器位置
    *
    * 参数1: 限速的运动速度
    * 参数2: 垂直方向限束器位置
    */
    const static std::string SET_VER_COLL;

    /**
    * @brief 设置垂直方向限束器位置的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_VER_COLL_RESP;

    /**
    * @brief 升降柱连续运动的启停控制
    *
    * 参数1: OptionLiftColMove
    *
    * 参数2: 运动速度
    */
    const static std::string MOVE_LIFT_COL;

    /**
    * @brief 升降柱连续运动的启停控制的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string MOVE_LIFT_COL_RESP;

    /**
    * @brief x轴连续运动的启停控制
    *
    * 参数1: OptionXAxisMove
    *
    * 参数2: 运动速度
    */
    const static std::string MOVE_X_AXIS;

    /**
    * @brief x轴连续运动的启停控制的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string MOVE_X_AXIS_RESP;

    /**
    * @brief C型臂连续运动的启停控制
    *
    * 参数1: OptionCArmMove
    *
    * 参数2: 运动速度
    */
    const static std::string MOVE_C_ARM;

    /**
    * @brief C型臂连续运动的启停控制的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string MOVE_C_ARM_RESP;

    /**
    * @brief 获取升降柱位置
    *
    * 参数: 无
    */
    const static std::string GET_POS_LIFT_COL;

    /**
    * @brief 获取升降柱位置的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 升降柱位置
    */
    const static std::string GET_POS_LIFT_COL_RESP;

    /**
    * @brief 获取前后轴（x轴）位置
    *
    * 参数: 无
    */
    const static std::string GET_POS_X_AXIS;

    /**
    * @brief 获取前后轴（x轴）位置的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 前后轴（x轴）位置
    */
    const static std::string GET_POS_X_AXIS_RESP;

    /**
    * @brief 获取C型臂位置
    *
    * 参数: 无
    */
    const static std::string GET_POS_C_ARM;

    /**
    * @brief 获取C型臂位置的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: C型臂位置
    */
    const static std::string GET_POS_C_ARM_RESP;

    /**
    * @brief 获取限束器当前位置
    *
    * 参数1: 水平限束器或垂直限速器
    */
    const static std::string GET_POS_COLLIMATOR;

    /**
    * @brief 获取限束器当前位置的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 限束器当前的位置
    */
    const static std::string GET_POS_COLLIMATOR_RESP;

    /**
    * @brief 控制C型臂转动到指定角度
    *
    * 参数1: 转动速度
    *
    * 参数2: 目的角度
    */
    const static std::string SET_POS_C_ARM;

    /**
    * @brief 控制C型臂转动到指定角度的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_POS_C_ARM_RESP;

    /**
    * @brief 控制前后轴（x轴）运动到指定位置
    *
    * 参数1: 移动速度
    *
    * 参数2: 目的位置
    */
    const static std::string SET_POS_X_AXIS;

    /**
    * @brief 控制前后轴（x轴）运动到指定位置的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_POS_X_AXIS_RESP;

    /**
    * @brief 控制升降柱运动到指定位置
    *
    * 参数1: 升降速度
    *
    * 参数2: 位置
    */
    const static std::string SET_POS_LIFT_COL;

    /**
    * @brief 控制升降柱运动到指定位置的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_POS_LIFT_COL_RESP;

    /**
    * @brief 使能C型臂人工转动
    *
    * 参数1: CArmAxisCtrlDef
    */
    const static std::string SET_MAN_C_ARM;

    /**
    * @brief 使能C型臂人工转动的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_MAN_C_ARM_RESP;

    /**
    * @brief 使能升降柱人工升降
    *
    * 参数1: CArmAxisCtrlDef
    */
    const static std::string SET_MAN_LIFT_COL;

    /**
    * @brief 使能升降柱人工升降的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_MAN_LIFT_COL_RESP;

    /**
    * @brief 使能前后轴（x轴）人工移动
    *
    * 参数1: CArmAxisCtrlDef
    */
    const static std::string SET_MAN_X_AXIS;

    /**
    * @brief 使能前后轴（x轴）人工移动
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_MAN_X_AXIS_RESP;

    /**
    * @brief 获取DAP当前读数
    *
    * 参数: 无
    */
    const static std::string GET_DAP_VALUE;

    /**
    * @brief 获取DAP当前读数的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 每分钟辐射剂量
    * 参数3: 辐射总剂量
    * 参数4: 每平方辐射剂量
    */
    const static std::string GET_DAP_VALUE_RESP;

    /**
    * @brief 设置3D扫描运动轨迹
    *
    * 参数1: n(轨迹点数) 
    *
    * 参数2-(3*n + 1): 3*n个轨迹点
    */
    const static std::string SET_3D_TRACK_DATA;

    /**
    * @brief 设置3D扫描运动轨迹的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SET_3D_TRACK_DATA_RESP;

    /**
    * @brief 开始采集2D图像
    *
    * 参数1: DevWorkMode
    *
    * 参数2: bool，是否发射射线
    */
    const static std::string START_SCAN_2D;

    /**
    * @brief 开始采集2D图像的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string START_SCAN_2D_RESP;

    /**
    * @brief 开始采集3D图像
    *
    * 参数1: DevWorkMode
    *
    * 参数2: bool，是否发射射线
    *
    * 参数3: 扫描时C型臂旋转方向
    */
    const static std::string START_SCAN_3D;

    /**
    * @brief 开始采集3D图像的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string START_SCAN_3D_RESP;

    /**
    * @brief 获取上次3D扫描记录的实际轨迹
    *
    * 参数: 无
    */
    const static std::string FETCH_3D_TRACK;

    /**
    * @brief 获取上次3D扫描记录的实际轨迹的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: n(轨迹点数)
    *
    * 参数3-(3*n + 1): 3*n个轨迹点
    */
    const static std::string FETCH_3D_TRACK_RESP;

    /**
    * @brief 启用/禁用DAP
    *
    * 参数: bool，启用/禁用
    */
    const static std::string ENABLE_DAP;

    /**
    * @brief 启用/禁用DAP消息的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string ENABLE_DAP_RESP;

    /**
    * @brief 查询当前管电压
    *
    * 参数: 无
    */
    const static std::string GET_KV;

    /**
    * @brief 查询当前管电压的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 当前管电压
    */
    const static std::string GET_KV_RESP;

    /**
    * @brief 查询当前管电流
    *
    * 参数: 无
    */
    const static std::string GET_MA;

    /**
    * @brief 查询当前管电流的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 当前管电流
    */
    const static std::string GET_MA_RESP;

    /**
    * @brief 查询当前球管温度
    *
    * 参数: 无
    */
    const static std::string GET_Bulb_Tube_Temp;

    /**
    * @brief 查询当前球管温度的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 当前球管温度
    */
    const static std::string GET_Bulb_Tube_Temp_RESP;

    /**
    * @brief 查询十字线激光灯开关状态
    *
    * 参数: 无
    */
    const static std::string GET_CROSS_LINE_LSR_STS;

    /**
    * @brief 查询十字线激光灯开关状态的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 0/1
    *
    * 参数3: 打开状态时，距离自动关闭的剩余延时
    */
    const static std::string GET_CROSS_LINE_LSR_STS_RESP;

    /**
    * @brief 查询视野范围激光灯开关状态
    *
    * 参数: 无
    */
    const static std::string GET_VIEW_RANGE_LSR_STS;

    /**
    * @brief 查询视野范围激光灯开关状态的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 开关，0/1
    *
    * 参数3: 打开状态时，距离自动关闭的剩余延时
    */
    const static std::string GET_VIEW_RANGE_LSR_STS_RESP;

    /**
    * @brief 查询设备主板温度
    *
    * 参数: 无
    */
    const static std::string GET_MB_TEMP;

    /**
    * @brief 查询设备主板温度的响应
    *
    * 参数1: ErrorCode
    *
    * 参数2: 主板温度
    */
    const static std::string GET_MB_TEMP_RESP;

    /**
    * @brief 曝光手闸状态变化消息
    *
    * 参数1: CArmExpBtnDef 手闸级别
    *
    * 参数2: bool 松开/按下
    */
    const static std::string EXP_BTN_PRESSED;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string EXP_BTN_PRESSED_RESP;

    /**
    * @brief 脚踏板状态变化消息
    *
    * 参数1: CArmPedalDef 脚踏板
    *
    * 参数2: bool 松开/踏下
    */
    const static std::string PEDAL_PRESSED;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string PEDAL_PRESSED_RESP;

    /**
    * @brief 各轴运动状态变化消息
    *
    * 参数1: CArmAxisDef 轴
    *
    * 参数2: CArmAxisMotionDef
    */
    const static std::string AXIS_MOTION_STATUS_CHGD;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string AXIS_MOTION_STATUS_CHGD_RESP;

    /**
    * @brief 限束器运动到位消息
    *
    * 参数1: OptionCollimator 轴
    *
    * 参数2: float 当前位置
    */
    const static std::string COLLIMATOR_IN_PLACE;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string COLLIMATOR_IN_PLACE_RESP;

    /**
    * @brief 各轴操作模式（自动/手动）变化消息
    *
    * 参数1: CArmAxisDef 轴
    *
    * 参数2: CArmAxisCtrlDef 自动/手动
    */
    const static std::string AXIS_OPER_MODE_CHGD;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string AXIS_OPER_MODE_CHGD_RESP;

    /**
    * @brief 激光灯开关状态变化消息
    *
    * 参数1: OptionLaserSwitch 激光灯
    *
    * 参数2: bool 开关
    */
    const static std::string LASER_SWITCH_CHGD;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string LASER_SWITCH_CHGD_RESP;

    /**
    * @brief 光栅开光状态变化消息
    *
    * 参数1: bool 开关
    */
    const static std::string RASTER_STATUS_CHGD;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string RASTER_STATUS_CHGD_RESP;

    /**
    * @brief 各个轴位置变化消息
    *
    * 参数1: CArmAxisDef 轴
    *
    * 参数2: float 当前位置/角度
    */
    const static std::string AXIS_POS_CHGD;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string AXIS_POS_CHGD_RESP;

    /**
    * @brief 向界面进程发送影像
    *
    * 参数1: 影像索引
    *
    * 参数2: 影像宽度
    *
    * 参数3: 影像高度
    *
    * 参数4: 存放影像数据的内存映射名称
    */
    const static std::string SEND_NEW_IMAGE;
    /**
    * @brief 上述消息的响应
    *
    * 参数1: 存放影像数据的内存映射名称
    */
    const static std::string SEND_NEW_IMAGE_RESP;

    /**
    * @brief 设备复位完成
    *
    * 参数1: 复位结果
    */
    const static std::string DEV_RESET_DONE;
    /**
    * @brief 上述消息的响应
    *
    * 参数: 无
    */
    const static std::string DEV_RESET_DONE_RESP;

    /**
    * @brief 暂停/恢复球管控制
    *
    * 参数: bool，暂停(false)/恢复(true)
    */
    const static std::string ENABLE_TUBE_CTRL;

    /**
    * @brief 暂停/恢复球管控制消息的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string ENABLE_TUBE_CTRL_RESP;

    /**
    * @brief 是否允许下位机放射线
    *
    * 参数: bool，不允许(false)/允许(true)
    */
    const static std::string RADIARION_PERMISSION_CTRL;

    /**
    * @brief 是否允许下位机放射线消息的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string RADIARION_PERMISSION_CTRL_RESP;

    /**
    * @brief 获取下位机电量百分比
    *
    * 参数: 无
    */
    const static std::string GET_BATTERY_PERCENT;

    /**
    * @brief 获取下位机电量百分比消息的响应
    *
    * 参数1: ErrorCode
    * 参数2: 电量百分比
    */
    const static std::string GET_BATTERY_PERCENT_RESP;

    /**
    * @brief 获取光栅当前开关状态
    *
    * 参数: 无
    */
    const static std::string GET_RASTER_STATUS;

    /**
    * @brief 获取光栅当前开关状态消息的响应
    *
    * 参数1: ErrorCode
    * 参数2: 0-关闭；1-开启
    */
    const static std::string GET_RASTER_STATUS_RESP;

    /**
    * @brief 扫描控制命令
    *
    * 参数1: SlaveScanCtrlType 具体的控制项
    */
    const static std::string SLAVE_SCAN_CTRL;

    /**
    * @brief 扫描控制命令消息的响应
    *
    * 参数1: ErrorCode
    */
    const static std::string SLAVE_SCAN_CTRL_RESP;
};

#endif//IPC_CMD_DEF_CARM_H