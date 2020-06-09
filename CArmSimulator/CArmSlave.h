#pragma once

#include <utility>
#include <set>
#include <mutex>
#include <set>

#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QTimer>
#include <functional>

#include "ControlSource.h"
#include "common.h"
#include "SeqFile.h"
#include "UEG_CMD.h"
//#include "SlaveComputerBase.h"

class CArmSlave : public ControlSource
{
    Q_OBJECT

    enum class AxisType { X, Y, Z , COLL_H, COLL_V};

    struct MoveInfo
    {
        const std::function<void(const float)> posSetter;
        const std::function<float()> posGetter;
        const Msg_t::Type feedbackType;   // 发送位置的消息类型
        const Msg_t::Type statusChaged;   // 运动状态变化
        const AxisType axis;
        const float maxPos;         // 能运动的最大位置
        const float minPos;         // 能运动的最小位置
        float velocity;             // 指定的速度
        float destPos;                 // 指定要运动到的位置
    };

private:
    CArmSlave();

public:
    ~CArmSlave() = default;
    // 获取此类型的一个实例
    static CArmSlave* instance();
    void Reset();

private:
    // 命令处理器
    void MsgHandler(const Msg_t& msg);
    // 上位机是否已连接
    bool UpperConnected() const;
    void SetXPos(const float pos);
    float GetXPos() const;
    void SetYPos(const float pos);
    float GetYPos() const;
    void SetZPos(const float pos);
    float GetZPos() const;
    void SetVCollPos(const float pos);
    float GetVCollPos() const;
    void SetHCollPos(const float pos);
    float GetHCollPos() const;
    void AddStopMoveType(const AxisType type);
    void RemoveStopMoveType(const AxisType type);
    bool IsStoppedMoveType(const AxisType type);
    void ClearStopMoveType();

    uint8Vec ParseLaserCtrl(const Msg_t& msg);
    uint8Vec ParseLaserStatus(const Msg_t& msg);
    uint8Vec ParseExposureMsg(const Msg_t& msg);
    uint8Vec ParsePosQuery(const float pos);
    uint8Vec ParseMovePos(const Msg_t& msg);
    uint8Vec ParseContinueMove(const Msg_t& msg, const AxisType& stopType);
    uint8Vec ParseDevReset(const Msg_t& msg);
    uint8Vec ParseRadiationPermission(const Msg_t& msg);
    void XYZMoveThread(const MoveInfo* pInfo);

    void Get_pos_velocity(const Msg_t& msg, MoveInfo* pInfo);

public slots:
    // 当上位机有新的信息传递过来的时候
    void OnReadyRead();

    void OnAFrameAcq(const int index);

    void OnFrameAcqFinished();

    void OnNewConnection();
    void SendMsg(Msg_t::Type type, const uint8Vec& arg);

signals:
    // 将info展现在UI上，供用户查看
    void displayMessage(const QString& info) const;
    void ResetDevSig();

private:
    QTcpServer                  m_tcpServer;            // 用于监听上位机连接的tcp服务器
    QTcpSocket*      m_masterComputer;           // 连接到上位机的socket
    struct MSG_RESP { const QString message; std::function<uint8Vec(const Msg_t& msg)> respFunc; };
    const std::map<Msg_t::Type, MSG_RESP> m_msgMap;
    float m_currentXPos;
    float m_currentZPos;
    float m_currentYPos;
    float m_currentCollV;
    float m_currentCollH;
    mutable std::mutex m_CurrentPosMutex;
    std::set<AxisType> m_stopMoveSet;
    std::map<AxisType, bool> m_manualoModeMap;
};