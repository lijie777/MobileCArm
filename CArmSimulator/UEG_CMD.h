#pragma once

#include <vector>

#include <QtNetwork/QTcpSocket>

using uint8Vec = std::vector<uint8_t>;

class Msg_t
{
public:
    enum Type : std::uint16_t
    {
        NOT_A_CMD = 0xFFFF,
        QUERY_DEV_STATUS = 0x0000,              // 查询设备的当前状态
        DEV_RESET = 0x0001,                     // 设备复位
        ALARM_RESET = 0x0002,                   // 报警复位
        LASER_SWITCH = 0x0003,                  // 激光灯开关
        SET_VOLTAGE = 0x0004,                   // 设置电压
        SET_CURRENT = 0x0005,                   // 设置电流
        SET_FPS = 0x0006,                       // 设置采集帧率
        SET_EXPOSURE_TIME = 0x0007,             // 设置曝光时间（每帧影像 X 射线发射时间）
        SET_FRAME_COUNT = 0x0008,               // 设置采集帧数
        COLLIMATOR_H_CTRL = 0x0009,             // 限束器窗口控制 – 水平方向
        COLLIMATOR_V_CTRL = 0x000A,             // 限束器窗口控制 – 垂直方向
        Z_MOTION_CTRL = 0x000B,                 // 升降柱（z 轴） 连续运动/停止
        X_MOTION_CTRL = 0x000C,                 // 前后轴（x 轴） 连续运动/停止
        Y_MOTION_CTRL = 0x000D,                 // C 型臂(Y 轴)连续运动/停止
        QUERY_Z_POS = 0x000E,                   // 查询升降柱（z 轴） 当前位置
        QUERY_X_POS = 0x000F,                   // 查询前后轴（x 轴） 当前位置
        QUERY_Y_POS = 0x0010,                   // 查询 C 型臂（Y 轴） 当前位置
        Y_POS_CTRL = 0x0011,                    // 控制 C 型臂（Y 轴） 运动到指定位置
        X_POS_CTRL = 0x0012,                    // 控制前后轴（x 轴） 运动到指定位置
        Z_POS_CTRL = 0x0013,                    // 控制升降柱（z 轴）运动到指定位置
        Y_HAND_CTRL_ENABLE = 0x0014,            // 使能 C 型臂（Y 轴） 电机手工控制
        Z_HAND_CTRL_ENABLE = 0x0015,            // 使能升降柱(z 轴)电机手工控制
        X_HAND_CTRL_ENABLE = 0x0016,            // 使能前后轴(x 轴)电机手工控制
        QUERY_DAP = 0x0017,                     // DAP 数值查询
        SEND_3D_TRACK_DATA = 0x0018,            // 发送 3D 扫描轨迹数据
        DEV_STATIC_ACQ = 0x0019,                // 设备静止中采集影像
        DEV_DYNAMIC_ACQ = 0x001A,               // 设备运动中采集影像
        FIRST_EXPOSURE_BTN_STATUS_ = 0x001B,    // 通知上位机曝光手闸状态变化（一级）
        SECOND_EXPOSURE_BTN_STATUS_ = 0x001C,   // 通知上位机曝光手闸状态变化（二级）
        PEDAL_STATUS_ = 0x001D,                 // 通知上位机脚踏板状态变化
        Z_MOTION_STATUS_ = 0x001E,              // 通知上位机升降柱（z 轴）运动状态变化
        X_MOTION_STATUS_ = 0x001F,              // 通知上位机前后轴（x 轴）运动状态变化
        Y_MOTION_STATUS_ = 0x0020,              // 通知上位机 C 型臂（Y 轴） 运动状态变化
        X_RAY_EMIT_ = 0x0021,                   // 通知上位机 X 射线开始发射
        QUERY_MOTION_TRACK = 0x0022,            // A 索要扫描过程中 B 记录的轨迹
        DAP_ENABLE = 0x0023,                    // 启用/禁用 DAP
        ALARM_MSG_ = 0x0024,                    // 通知上位机设备报警信息
        COLLIMATOR_POS_H_ = 0x0025,             // 通知上位机限束器位置-水平方向
        COLLIMATOR_POS_V_ = 0x0026,             // 通知上位机限束器位置-垂直方向
        Y_HAND_CTRL_STATUS_ = 0x0027,           // 通知上位机手动/自动控制变化-C 型臂（Y 轴）
        Z_HAND_CTRL_STATUS_ = 0x0028,           // 通知上位机手动/自动控制变化-升降柱（z 轴）
        X_HAND_CTRL_STATUS_ = 0x0029,           // 通知上位机手动/自动控制变化-前后轴（x 轴）
        CROSS_LASER_SWITCH_STATUS_ = 0x002A,    // 通知上位机激光灯开关变化-十字线激光灯
        FOV_LASER_SWITCH_STATUS_ = 0x002B,      // 通知上位机激光灯开关变化-视野范围激光灯
        Y_ANGLE_STATUS_ = 0x002C,               // 通知上位机 C 型臂（Y 轴）当前角度
        Z_HEIGHT_STATUS_ = 0x002D,              // 通知上位机升降柱（z 轴）当前高度
        X_POS_STATUS_ = 0x002E,                 // 通知上位机前后轴（x 轴）当前位置
        QUERY_VOLTAGE = 0x002F,                 // 查询当前管电压
        QUERY_CURRENT = 0x0030,                 // 查询当前管电流
        QUERY_CROSS_LASER_SWITCH = 0x0031,      // 查询激光灯的当前开关状态-十字线激光灯
        QUERY_FOV_LASER_SWITCH = 0x0032,        // 查询激光灯的当前开关状态-视野范围激光灯
        QUERY_DEV_TEMP = 0x0033,                // 查询设备当前温度
        RESET_FINISHED_ = 0x0034,               // 通知上位机设备复位完成
        TUBE_CTRL = 0x0035,                     // 通知下位机暂停/开始对球管的控制
        TUBE_TEMP = 0x0036,                     // 查询球管温度
        RADIATION_PERMISSION = 0x0037,          // 是否允许下位机放射线
        GET_BATTERY_PERCENT = 0x0038,           // 查询下位机电池电量
        GET_RASTER_STATUS = 0x0039,             // 查询光栅的当前开关状态
        RASTER_STATUS_CHANGED = 0x003A,  // 通知上位机光栅开关变化
        CANCEL_SCAN = 0x003B,            // 上位机通知下位机取消扫描
        SET_COLL_H_CONTINUE_MOVE = 0x003C, // 设置水平限束器连续运动
        SET_COLL_V_CONTINUE_MOVE = 0x003D, // 设置垂直限束器连续运动
        QUERY_COLLIMATOR_POS_H = 0x003E,  // 查询当前水平限速器位置
        QUERY_COLLIMATOR_POS_V = 0x003F  // 查询当前垂直限速器位置
    };
    const std::uint8_t MSG_HEAD{ 0xaa };

    Msg_t() = default;
    Msg_t(const Type type_, const std::uint16_t uid_, const std::vector<std::uint8_t>& arg_);
    Msg_t(const Type type_, const float arg_);
    bool ReadData(QTcpSocket* socketPtr);
    bool Valid() const;
    std::size_t Size() const;
    std::vector<std::uint8_t> ToVec() const;
    static std::uint16_t GenUID();

public:
    std::uint8_t head;
    std::uint32_t length;
    std::uint16_t uid; // 标记某种类型消息的唯一身份
    Msg_t::Type type;
    std::vector<std::uint8_t> arg;
};
