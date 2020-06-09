#pragma once

#include <map>

/// 激光灯类型定义
enum class OptionLaserSwitch
{
    All = 0,        //!< 全部激光灯
    CrossCurve = 1, //!< 十字线激光灯
    ViewRange = 2,  //!< 视野范围激光灯
};

/// 限束器类型定义
enum class OptionCollimator
{
    Hor = 0, //!< 水平方向限束器
    Ver = 1, //!< 垂直方向限束器
};

enum class OptionCollimatorMoveDirection : std::uint8_t
{
    STOP = 0,       //!< 停止
    INWARD = 1,     //!< 往里
    OUTWARD = 2,    //!< 往外
};

/// 前后轴运动状态定义
enum class OptionXAxisMove
{
    Stop = 0,       //!< 停止运动
    Forward = 1,    //!< 向前持续运动
    Backward = 2    //!< 向后持续运动
};

/// C型臂运动状态定义
enum class OptionCArmMove
{
    Stop = 0,           //!< 停止运动
    Clockwise = 1,      //!< C型臂持续顺时针旋转
    Anticlockwize = 2   //!< C型臂持续逆时针旋转
};

/// 升降柱运动状态定义
enum class OptionLiftColMove
{
    Stop = 0,      //!< 停止运动
    Upward = 1,    //!< 升降柱持续上升
    Downward = 2   //!< 升降柱持续下降
};

/// 各轴定义
enum class CArmAxisDef
{
    ForBackward = 0, //!< 前后轴
    LiftingCol = 1,  //!< 升降柱
    CArm = 2         //!< C型臂
};

/// 脚踏板定义
enum class CArmPedalDef : std::uint8_t
{
    Left = 0,  //!< 左脚
    Right = 1  //!< 右脚
};

static std::string ToString(const CArmPedalDef val)
{
    static std::map<CArmPedalDef, std::string> mymap
    {
        { CArmPedalDef::Left, "Left" },
        { CArmPedalDef::Right, "Right"}
    };

    return mymap[val];
}

/// 曝光手闸定义
enum class CArmExpBtnDef : std::uint8_t
{
    Level1 = 0,  //!< 左脚
    Level2 = 1  //!< 右脚
};

static std::string ToString(const CArmExpBtnDef val)
{
    static std::map<CArmExpBtnDef, std::string> mymap
    {
        { CArmExpBtnDef::Level1, "Level1" },
        { CArmExpBtnDef::Level2, "Level2"}
    };

    return mymap[val];
}

/// 按钮状态
enum class CArmBtnStatus : std::uint8_t
{
    Released = 0,   //!< 松开
    Pressed = 1     //!< 按下
};

static std::string ToString(const CArmBtnStatus val)
{
    static std::map<CArmBtnStatus, std::string> mymap
    {
        { CArmBtnStatus::Pressed, "Pressed" },
        { CArmBtnStatus::Released, "Released" }
    };

    return mymap[val];
}

/// 各轴运动状态定义
enum class CArmAxisMotionDef : std::uint8_t
{
    Moving = 0, //!< 运动中
    Stopped = 1 //!< 停止中
};

/// 各轴控制状态定义
enum class CArmAxisCtrlDef : std::uint8_t
{
    Auto = 0, //!< 自动
    Manu = 1  //!< 手动
};

/// 开关状态定义
enum class CArmOnOffDef : std::uint8_t
{
    Off = 0, //!< 关闭
    On = 1  //!< 打开
};

/// 网络状态定义
enum class CArmNetStatus : std::uint8_t
{
    Disconnected = 0,   //!< 连接断开
    Connected = 1       //!< 已建立连接
};

/// 性别定义
enum class CArmGenderDef : std::uint8_t
{
    F,              //!< 女性
    M,              //!< 男性
    Other           //!< 未知
};

/// 是否允许定义
enum class CArmPermissionDef : std::uint8_t
{
    NO,             //!< 不允许
    YES             //!< 允许
};

enum class CArmDeviceValueTypeDef : std::uint8_t
{
    CURRENT_X_AXIS_POS,         //!< x轴的位置
    CURRENT_Y_AXIS_POS,         //!< y轴的位置
    CURRENT_Z_AXIS_POS,         //!< z轴的位置
    CURRENT_COLL_H_POS,         //!< 水平限束器位置
    CURRENT_COLL_V_POS,         //!< 垂直限束器位置
    CURRENT_BULB_TUBE_TEMP,     //!< 获取球管温度
    CURRENT_BATTERY_PERCENT,    //!< 获取电池电量
    CURRENT_RASTER_STATUS,      //!< 获取当前光栅的状态
    CURRENT_DAP_VALUE,          //!< 当前DAP的值
};

enum class CArmDeviceMoveTypeDef : std::uint8_t
{
    SET_X_AXIS_POS,                         //!< 设置x轴到指定的位置
    SET_Y_AXIS_POS,                         //!< 设置y轴到指定的位置
    SET_Z_AXIS_POS,                         //!< 设置z轴到指定的位置
    SET_COLLIMATOR_POS_V,                   //!< 设置垂直方向位置
    SET_COLLIMATOR_POS_H,                   //!< 设置水平方向位置
    SET_X_AXIS_CONTINUE_MOVE,               //!< 设置x轴连续运动
    SET_Y_AXIS_CONTINUE_MOVE,               //!< 设置y轴连续运动
    SET_Z_AXIS_CONTINUE_MOVE,               //!< 设置z轴连续运动
    SET_COLLIMATOR_V_CONTINUE_MOVE,         //!< 设置限束器垂直方向连续运动
    SET_COLLIMATOR_H_CONTINUE_MOVE,         //!< 设置限束器水平方向连续运动
};

enum class CArmWorkStationStatusTypeDef : std::uint8_t
{
    SCAN_FINISH,                        //!< 扫描结束
    USER_LOGIN,                         //!< 用户登录
    USER_LOGOUT,                        //!< 用户登出
};