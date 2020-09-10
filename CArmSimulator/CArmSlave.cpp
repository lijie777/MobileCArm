#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <cassert>

#include <QMessageBox>
#include <QDebug>

#include "CArmSlave.h"
#include "DataSource.h"
#include "CArmWidget.h"

using namespace std::literals::chrono_literals;

static const auto move_time_interval{ 1s };

constexpr static float limitation_x_max{ 200 }; // mm
constexpr static float limitation_x_min{ 0 }; // mm
constexpr static float limitation_x_origin{ 100 }; // mm
constexpr static float limitation_y_max{ 110 }; // 度
constexpr static float limitation_y_min{ -110 }; // 度
constexpr static float limitation_y_origin{ 0 }; // 度
constexpr static float limitation_z_max{ 200 }; // mm
constexpr static float limitation_z_min{ 0 }; // mm
constexpr static float limitation_z_origin{ 100 }; // mm

constexpr static float limitation_coll_v_max{ 100.f }; // mm
constexpr static float limitation_coll_v_min{ 0.f }; // mm
constexpr static float limitation_coll_v_origin{ 10.f }; // mm

constexpr static float limitation_coll_h_max{ 100.f }; // mm
constexpr static float limitation_coll_h_min{ 0.f }; // mm
constexpr static float limitation_coll_h_origin{ 10.f }; // mm

static constexpr std::uint8_t StopMove{ 0 };
static constexpr std::uint8_t PositiveMove{ 1 };
static constexpr std::uint8_t NegativeMove{ 2 };

static const uint8Vec DefaultStatus{ 0, 0 };
static const uint8Vec DefaultPos{ 0, 0, 0, 0 };
static const uint8Vec DefaultDAP{ 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};
static auto DefaultTrack = []
{
    const std::uint16_t dotCount{ 6 };
    uint8Vec vec(sizeof(dotCount)+3*sizeof(float)*dotCount);
    auto ptr = (std::uint16_t*)(vec.data());
    *ptr = dotCount;
    return vec;
};

CArmSlave::CArmSlave()
    : m_masterComputer{}
    , m_currentXPos{::limitation_x_origin}
    , m_currentYPos{::limitation_y_origin}
    , m_currentZPos{::limitation_z_origin}
    , m_currentCollH{::limitation_coll_h_origin}
    , m_currentCollV{::limitation_coll_v_origin}
    , m_msgMap{
        {Msg_t::QUERY_DEV_STATUS, {"查询设备的当前状态", [](const Msg_t& msg) { return ::DefaultStatus; }}},
        {Msg_t::DEV_RESET, {"设备复位", [this](const Msg_t& msg) { return ParseDevReset(msg); } }},
        {Msg_t::ALARM_RESET, {"报警复位",[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::LASER_SWITCH,{ "激光灯开关" ,[this](const Msg_t& msg) { return ParseLaserCtrl(msg); }} },
        { Msg_t::SET_VOLTAGE,{ "设置电压" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::SET_CURRENT,{ "设置电流" ,[](const Msg_t& msg) { return ::DefaultStatus; } } },
        { Msg_t::CANCEL_SCAN,{ "取消扫描" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::SET_FPS,{ "设置采集帧率" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::SET_EXPOSURE_TIME,{ "设置曝光时间" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::SET_FRAME_COUNT,{ "设置采集帧数" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::COLLIMATOR_H_CTRL,{ "限束器窗口控制 – 水平方向" ,[this](const Msg_t& msg) { return ParseMovePos(msg); }} },
        { Msg_t::COLLIMATOR_V_CTRL,{ "限束器窗口控制 – 垂直方向" ,[this](const Msg_t& msg) { return ParseMovePos(msg); } } },
        { Msg_t::RADIATION_PERMISSION,{ "允许/不允许下位机放射线" ,[this](const Msg_t& msg) { return ParseRadiationPermission(msg); }} },
        { Msg_t::Z_MOTION_CTRL,{ "升降柱（z 轴） 连续运动/停止" ,[this](const Msg_t& msg) { return ParseContinueMove(msg, AxisType::Z); }} },
        { Msg_t::X_MOTION_CTRL,{ "前后轴（x 轴） 连续运动/停止" ,[this](const Msg_t& msg) { return ParseContinueMove(msg, AxisType::X); }} },
        { Msg_t::Y_MOTION_CTRL,{ "C 型臂(Y 轴)连续运动/停止" ,[this](const Msg_t& msg) { return ParseContinueMove(msg, AxisType::Y); }} },
        { Msg_t::SET_COLL_H_CONTINUE_MOVE,{ "设置水平限束器连续运动", [this](const Msg_t& msg) { return ParseContinueMove(msg, AxisType::COLL_H); } } },
        { Msg_t::SET_COLL_V_CONTINUE_MOVE, { "设置垂直限束器连续运动", [this](const Msg_t& msg) { return ParseContinueMove(msg, AxisType::COLL_V); } }},
        { Msg_t::QUERY_Z_POS,{ "查询升降柱（z 轴） 当前位置" ,[this](const Msg_t& msg) { return ParsePosQuery(GetZPos()); }} },
        { Msg_t::QUERY_X_POS,{ "查询前后轴（x 轴） 当前位置" ,[this](const Msg_t& msg) { return ParsePosQuery(GetXPos());  }} },
        { Msg_t::QUERY_Y_POS,{ "查询 C 型臂（Y 轴） 当前位置" ,[this](const Msg_t& msg) { return ParsePosQuery(GetYPos()); }} },
        { Msg_t::QUERY_COLLIMATOR_POS_H,{ "查询水平限束器位置", [this](const Msg_t& msg) { return ParsePosQuery(GetHCollPos()); } } },
        { Msg_t::QUERY_COLLIMATOR_POS_V, { "查询垂直限束器位置", [this](const Msg_t& msg) { return ParsePosQuery(GetVCollPos()); } } },
        { Msg_t::Y_POS_CTRL,{ "控制 C 型臂（Y 轴） 运动到指定位置" ,[this](const Msg_t& msg) { return ParseMovePos(msg); }} },
        { Msg_t::X_POS_CTRL,{ "控制前后轴（x 轴） 运动到指定位置" ,[this](const Msg_t& msg) { return ParseMovePos(msg); }} },
        { Msg_t::Z_POS_CTRL,{ "控制升降柱（z 轴）运动到指定位置" ,[this](const Msg_t& msg) { return ParseMovePos(msg); }} },
        { Msg_t::Y_HAND_CTRL_ENABLE,{ "使能 C 型臂（Y 轴） 电机手工控制" ,[this](const Msg_t& msg) { m_manualoModeMap[AxisType::Y] = bool(*(msg.arg.data())); return ::DefaultStatus; }} },
        { Msg_t::Z_HAND_CTRL_ENABLE,{ "使能升降柱(z 轴)电机手工控制" ,[this](const Msg_t& msg) { m_manualoModeMap[AxisType::Z] = bool(*(msg.arg.data())); return ::DefaultStatus; }} },
        { Msg_t::X_HAND_CTRL_ENABLE,{ "使能前后轴(x 轴)电机手工控制" ,[this](const Msg_t& msg) { m_manualoModeMap[AxisType::X] = bool(*(msg.arg.data())); return ::DefaultStatus; }} },
        { Msg_t::QUERY_DAP,{ "DAP 数值查询" ,[](const Msg_t& msg) { return ::DefaultDAP; }} },
        { Msg_t::SEND_3D_TRACK_DATA,{ "发送 3D 扫描轨迹数据" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::DEV_STATIC_ACQ,{ "设备静止中采集影像" ,[this](const Msg_t& msg) { return  ParseExposureMsg(msg); }} },
        { Msg_t::DEV_DYNAMIC_ACQ,{ "设备运动中采集影像" ,[this](const Msg_t& msg) { return ParseExposureMsg(msg); }} },
        { Msg_t::QUERY_MOTION_TRACK,{ "A 索要扫描过程中 B 记录的轨迹" ,[](const Msg_t& msg) { return ::DefaultTrack(); }} },
        { Msg_t::DAP_ENABLE,{ "启用/禁用 DAP" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::QUERY_VOLTAGE,{ "查询当前管电压" ,[](const Msg_t& msg) { return uint8Vec{ 0, 0, 0, 0 }; }} },
        { Msg_t::QUERY_CURRENT,{ "查询当前管电流" ,[](const Msg_t& msg) { return uint8Vec{ 0, 0, 0, 0 }; } } },
        { Msg_t::GET_RASTER_STATUS,{ "查询光栅的当前开关状态" ,[](const Msg_t& msg) { return uint8Vec{ 0 }; } } },
        { Msg_t::GET_BATTERY_PERCENT,{ "查询下位机电池电量" ,[](const Msg_t& msg) { return uint8Vec{ 0, 0, 0, 0 }; }} },
        { Msg_t::TUBE_TEMP,{ "查询当前球管温度" ,[](const Msg_t& msg) { return uint8Vec{ 0, 0, 0, 0 }; } } },
        { Msg_t::QUERY_CROSS_LASER_SWITCH,{ "查询激光灯的当前开关状态-十字线激光灯" ,[this](const Msg_t& msg) { return ParseLaserStatus(msg); }} },
        { Msg_t::QUERY_FOV_LASER_SWITCH,{ "查询激光灯的当前开关状态-视野范围激光灯" ,[this](const Msg_t& msg) { return ParseLaserStatus(msg); }} },
        { Msg_t::TUBE_CTRL,{ "暂停/开始对球管的控制" ,[](const Msg_t& msg) { return ::DefaultStatus; }} },
        { Msg_t::QUERY_DEV_TEMP,{ "查询设备当前温度" ,[](const Msg_t& msg) { return uint8Vec{ 0, 0, 0, 0 }; }} }
    }
{
    // tcp server
    connect(&m_tcpServer, &QTcpServer::newConnection, this, &CArmSlave::OnNewConnection);
    m_tcpServer.listen(QHostAddress::Any, 8888);
}

// 获取此类型的一个实例
CArmSlave* CArmSlave::instance()
{
    static auto ins = new CArmSlave;
    return ins;
}

void CArmSlave::Reset()
{
    SetXPos(::limitation_x_origin);
    SetYPos(::limitation_y_origin);
    SetZPos(::limitation_z_origin);
    ClearStopMoveType();
    m_manualoModeMap.clear();

    QTimer::singleShot(5000, [this]
    {
        UegNetwork::sendData(m_masterComputer, Msg_t{ Msg_t::RESET_FINISHED_, Msg_t::GenUID(), ::DefaultStatus });
    });
}

bool CArmSlave::UpperConnected() const
{
    return QAbstractSocket::ConnectedState == m_masterComputer->state();
}

void CArmSlave::MsgHandler(const Msg_t& msg)
{
    if (!msg.Valid())
    {
        emit displayMessage("收到的上位机消息有误");
        return;
    }

    if (std::end(m_msgMap) == m_msgMap.find(msg.type))
    {
        emit displayMessage("收到的消息类型未知：" + QString::number(int(msg.type)));
        return;
    }

    const auto& msgResp = m_msgMap.at(msg.type);
    emit displayMessage("收到上位机消息: " + msgResp.message);

    if (msgResp.respFunc)
    {
        const auto retval = msgResp.respFunc(msg);
        if (!retval.empty())
        {
            UegNetwork::sendData(m_masterComputer, { msg.type, msg.uid, retval });
        }
    }
}

// 当上位机有新的信息传递过来的时候
void CArmSlave::OnReadyRead()
{
    Msg_t msg;
    while (msg.ReadData(m_masterComputer))
    {
        MsgHandler(msg);
    }
}

uint8Vec CArmSlave::ParseExposureMsg(const Msg_t& msg)
{
    return ::DefaultStatus;
}

uint8Vec CArmSlave::ParseContinueMove(const Msg_t& msg, const AxisType& stopType)
{
    if (::StopMove == *(msg.arg.data()))
    {
        AddStopMoveType(stopType);
        return ::DefaultStatus;
    }

    return ParseMovePos(msg);
}

uint8Vec CArmSlave::ParseDevReset(const Msg_t& msg)
{
    emit ResetDevSig();

    return ::DefaultStatus;
}

uint8Vec CArmSlave::ParseRadiationPermission(const Msg_t& msg)
{
    const auto isPermission = (0 != msg.arg[0]);

    if (isPermission)
    {
        if (const auto seq = CArmWidget::instance()->GetSelectedSeq())
        {
            emit dataSourceChanged(new DataSource(seq->clone()));
        }

        // 100ms后告知上位机已开始发射线
        QTimer::singleShot(1000, [this] {UegNetwork::sendData(m_masterComputer, { Msg_t::X_RAY_EMIT_, Msg_t::GenUID(),uint8Vec{} }); });
    }

    emit displayMessage((isPermission ? "允许" : "不允许") + QString("放射线"));
    return ::DefaultStatus;
}

void CArmSlave::Get_pos_velocity(const Msg_t& msg, MoveInfo* pInfo)
{
    const auto msgtype = msg.type;

    if ((Msg_t::Z_POS_CTRL == msgtype)
        || (Msg_t::X_POS_CTRL == msgtype)
        || (Msg_t::Y_POS_CTRL == msgtype))
    {
        auto ptr = (float*)(msg.arg.data());
        pInfo->velocity = *ptr;
        pInfo->destPos = *(ptr + 1);
    }
    else if ((Msg_t::Z_MOTION_CTRL == msgtype)
        || (Msg_t::X_MOTION_CTRL == msgtype)
        || (Msg_t::Y_MOTION_CTRL == msgtype))
    {
        auto ptr = msg.arg.data();
        pInfo->destPos = ((::PositiveMove == *ptr) ? pInfo->maxPos : pInfo->minPos);
        pInfo->velocity = *((float*)(ptr + 1));
    }
    else
    {
        emit displayMessage("获取位置和速度出错");
    }
}

uint8Vec CArmSlave::ParseMovePos(const Msg_t& msg)
{
    static std::map<Msg_t::Type, MoveInfo> mymap
    {
        { Msg_t::Z_POS_CTRL,{ [this](const float pos) { SetZPos(pos); }, [this] {return GetZPos(); }, Msg_t::Z_HEIGHT_STATUS_, Msg_t::Z_MOTION_STATUS_, AxisType::Z, ::limitation_z_max, ::limitation_z_min, 0, 0 } },
        { Msg_t::Z_MOTION_CTRL, {[this](const float pos) { SetZPos(pos); }, [this] {return GetZPos(); }, Msg_t::Z_HEIGHT_STATUS_, Msg_t::Z_MOTION_STATUS_, AxisType::Z, ::limitation_z_max, ::limitation_z_min, 0, 0 } },
        { Msg_t::X_POS_CTRL,{ [this](const float pos) { SetXPos(pos); }, [this] {return GetXPos(); }, Msg_t::X_POS_STATUS_, Msg_t::X_MOTION_STATUS_, AxisType::X, ::limitation_x_max, ::limitation_x_min, 0, 0 } },
        { Msg_t::X_MOTION_CTRL,{ [this](const float pos) { SetXPos(pos); }, [this] {return GetXPos(); }, Msg_t::X_POS_STATUS_, Msg_t::X_MOTION_STATUS_, AxisType::X, ::limitation_x_max, ::limitation_x_min, 0, 0 } },
        { Msg_t::Y_POS_CTRL,{ [this](const float pos) { SetYPos(pos); }, [this] {return GetYPos(); }, Msg_t::Y_ANGLE_STATUS_, Msg_t::Y_MOTION_STATUS_, AxisType::Y, ::limitation_y_max, ::limitation_y_min, 0, 0 } },
        { Msg_t::Y_MOTION_CTRL,{ [this](const float pos) { SetYPos(pos); }, [this] {return GetYPos(); }, Msg_t::Y_ANGLE_STATUS_, Msg_t::Y_MOTION_STATUS_, AxisType::Y, ::limitation_y_max, ::limitation_y_min, 0, 0 } },
        { Msg_t::COLLIMATOR_H_CTRL,{ [this](const float pos) { SetHCollPos(pos); }, [this] { return GetHCollPos(); }, Msg_t::COLLIMATOR_POS_H_, Msg_t::NOT_A_CMD, AxisType::COLL_H, ::limitation_coll_h_max, ::limitation_coll_h_min, 0, 0 } },
        { Msg_t::SET_COLL_H_CONTINUE_MOVE, { [this](const float pos) { SetHCollPos(pos); }, [this] { return GetHCollPos(); }, Msg_t::COLLIMATOR_POS_H_, Msg_t::NOT_A_CMD, AxisType::COLL_H, ::limitation_coll_h_max, ::limitation_coll_h_min, 0, 0 } },
        { Msg_t::COLLIMATOR_V_CTRL,{ [this](const float pos) { SetVCollPos(pos); }, [this] { return GetVCollPos(); }, Msg_t::COLLIMATOR_POS_V_, Msg_t::NOT_A_CMD, AxisType::COLL_V, ::limitation_coll_v_max, ::limitation_coll_v_min, 0, 0 } },
        { Msg_t::SET_COLL_V_CONTINUE_MOVE,{ [this](const float pos) { SetVCollPos(pos); }, [this] { return GetVCollPos(); }, Msg_t::COLLIMATOR_POS_V_, Msg_t::NOT_A_CMD, AxisType::COLL_V, ::limitation_coll_v_max, ::limitation_coll_v_min, 0, 0 } }
    };

    MoveInfo* pInfo = nullptr;
    try
    {
        pInfo = &(mymap.at(msg.type));
    }
    catch (const std::out_of_range&)
    {
        emit displayMessage("未能找到处理此消息的信息");
        return ::DefaultStatus;
    }

    if (m_manualoModeMap[pInfo->axis])
    {
        emit displayMessage("手动模式下不允许通过命令运动XYZ轴");
        return ::DefaultStatus;
    }

    Get_pos_velocity(msg, pInfo);
    if ((pInfo->destPos > pInfo->maxPos) || (pInfo->destPos < pInfo->minPos))
    {
        emit displayMessage("指定的位置超过了极限。 pos:" + QString::number(pInfo->destPos));
        return ::DefaultStatus;
    }
    if ((pInfo->posGetter() > pInfo->maxPos) || (pInfo->posGetter() < pInfo->minPos))
    {
        emit displayMessage("当前的位置超过了极限。 ");
        return ::DefaultStatus;
    }

    std::thread(&CArmSlave::XYZMoveThread, this, pInfo).detach();

    return ::DefaultStatus;
}

static auto is_exceeded = [](const auto direction, const auto target, const auto pos)
{
    assert(direction != 0);
    return (std::signbit(double(direction)) == std::signbit(double(pos) - double(target)));
};

void CArmSlave::XYZMoveThread(const MoveInfo* pInfo)
{
    static constexpr std::uint8_t MoveStart{ 0 };
    static constexpr std::uint8_t MoveStop{ 1 };

    //float rangeMax{};
    //float rangeMin{};
    float offset{};
    if (pInfo->posGetter() < pInfo->destPos)
    {
        offset = std::abs(pInfo->velocity);
        //rangeMax = pInfo->destPos;
        //rangeMin = pInfo->posGetter();
    }
    else
    {
        offset = -std::abs(pInfo->velocity);
        //rangeMax = pInfo->posGetter();
        //rangeMin = pInfo->destPos;
    }

    // 运动开始
    UegNetwork::sendData(m_masterComputer, Msg_t{ pInfo->statusChaged, Msg_t::GenUID(), uint8Vec{ MoveStart } });
    while (!IsStoppedMoveType(pInfo->axis))
    {
        std::this_thread::sleep_for(::move_time_interval);

        auto pos_ = pInfo->posGetter();
        pos_ += offset;

        bool done = false;
        //if ((pos_ > rangeMax) || (pos_ < rangeMin))
        if ((pos_ == pInfo->destPos) || is_exceeded(offset, pInfo->destPos, pos_))
        {
            done = true;
            pos_ = pInfo->destPos;
        }

        emit displayMessage("当前运动到的位置：" + QString::number(pos_));

        pInfo->posSetter(pos_);
        UegNetwork::sendData(m_masterComputer, Msg_t{ pInfo->feedbackType, pos_ });

        if (done)
        {
            break;
        }
    }

    // 将此轴从要停止的轴中去除
    RemoveStopMoveType(pInfo->axis);

    // 正常结束和上位机通知结束都发此消息
    UegNetwork::sendData(m_masterComputer, Msg_t{ pInfo->statusChaged, Msg_t::GenUID(),uint8Vec{ MoveStop } });
}



void CArmSlave::SetXPos(const float pos)
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    m_currentXPos = pos;
}

float CArmSlave::GetXPos() const
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    return m_currentXPos;
}

void CArmSlave::SetYPos(const float pos)
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    m_currentYPos = pos;
}

float CArmSlave::GetYPos() const
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    return m_currentYPos;
}

void CArmSlave::SetZPos(const float pos)
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    m_currentZPos = pos;
}

float CArmSlave::GetZPos() const
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    return m_currentZPos;
}

void CArmSlave::SetVCollPos(const float pos)
{
    std::lock_guard<std::mutex>{ m_CurrentPosMutex }, m_currentCollV = pos;
}

float CArmSlave::GetVCollPos() const
{
    return std::lock_guard<std::mutex>{ m_CurrentPosMutex }, m_currentCollV;
}

void CArmSlave::SetHCollPos(const float pos)
{
    std::lock_guard<std::mutex>{ m_CurrentPosMutex }, m_currentCollH = pos;
}

float CArmSlave::GetHCollPos() const
{
    return std::lock_guard<std::mutex>{ m_CurrentPosMutex }, m_currentCollH;
}

void CArmSlave::AddStopMoveType(const AxisType type)
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    m_stopMoveSet.insert(type);
}

void CArmSlave::RemoveStopMoveType(const AxisType type)
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    m_stopMoveSet.erase(type);
}

bool CArmSlave::IsStoppedMoveType(const AxisType type)
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    return (std::end(m_stopMoveSet) != m_stopMoveSet.find(type));
}

void CArmSlave::ClearStopMoveType()
{
    std::lock_guard<std::mutex> lg{ m_CurrentPosMutex };
    m_stopMoveSet.clear();
}

uint8Vec CArmSlave::ParseLaserCtrl(const Msg_t& msg)
{
    auto w = CArmWidget::instance();
    const auto light_ = msg.arg.at(0);
    const bool open_ = msg.arg.at(1);
    if (0 == light_)
    {
        open_ ? w->AllLaserOn() : w->AllLaserOff();
    }
    else if (1 == light_)
    {
        open_ ? w->CrossLaserOn() : w->CrossLaserOff();
    }
    else
    {
        open_ ? w->FovLaserOn() : w->FovLaserOff();
    }

    return ::DefaultStatus;
}

uint8Vec CArmSlave::ParseLaserStatus(const Msg_t& msg)
{
    float remainingTime{};
    if (Msg_t::QUERY_CROSS_LASER_SWITCH == msg.type)
    {
        remainingTime = CArmWidget::instance()->CrossLaserRemainingTime();
    }
    else if (Msg_t::QUERY_FOV_LASER_SWITCH == msg.type)
    {
        remainingTime = CArmWidget::instance()->FOVLaserRemainingTime();
    }
    else
    {
        remainingTime = 0.f;
        emit displayMessage("激光灯状态查询消息存在问题");
    }

    uint8Vec vec;
    vec.push_back(0 == remainingTime ? 0 : 1);
    std::uint8_t* ptr = (std::uint8_t*)(&remainingTime);
    vec.insert(std::end(vec), ptr, ptr + sizeof(remainingTime));

    return vec;
}

uint8Vec CArmSlave::ParsePosQuery(const float pos)
{
    uint8Vec vec;
    const std::uint8_t* ptr = (std::uint8_t*)(&pos);
    vec.insert(std::end(vec), ptr, ptr + sizeof(pos));
    return vec;
}

void CArmSlave::OnAFrameAcq(const int index)
{
    emit displayMessage("正在进行拍片，第" + QString::number(index)+"帧已发送成功");
}

// 曝光正常结束
void CArmSlave::OnFrameAcqFinished()
{
    emit displayMessage("本次拍片已结束");
}

void CArmSlave::OnNewConnection()
{
    if (m_masterComputer)
    {
        m_tcpServer.nextPendingConnection()->close();
        return;
    }

    m_masterComputer = m_tcpServer.nextPendingConnection();
    emit displayMessage("[下位机] 已和上位机建立连接: Ip: " + m_masterComputer->peerAddress().toString() + ", port: " + QString::number(m_masterComputer->peerPort()));
    connect(m_masterComputer, &QTcpSocket::stateChanged, [&]()
    {
        if (m_masterComputer && (QAbstractSocket::UnconnectedState == m_masterComputer->state()))
        {
            m_masterComputer = nullptr;
            emit displayMessage("上位机已和下位机断开连接.");
        }
    });
    connect(m_masterComputer, &QTcpSocket::readyRead, this, &CArmSlave::OnReadyRead);
}

void CArmSlave::SendMsg(Msg_t::Type type, const uint8Vec& arg)
{
    //ParseMovePos({ Msg_t::Z_POS_CTRL, Msg_t::GenUID(), {1,1,1,1,1,1,1,1} });
    UegNetwork::sendData(m_masterComputer, { type, Msg_t::GenUID(), arg });
}
