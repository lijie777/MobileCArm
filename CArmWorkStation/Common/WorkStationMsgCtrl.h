#pragma once

#include <functional>
#include <map>

enum ClientType
{
    TP,                     //!< 触摸屏
    WORKSTATION_GUI,        //!< 工作站GUI
    DEV_STSTUS_SERVER,      //!< 设备状态服务
    SCAN_INFO_SERVER,       //!< 采集信息服务
    SCAN_CTRL               //!< 拍片控制
};

class CArmCmdBase;

#define WORK_STATION_MSG_CTRL WorkStationMsgCtrl::GetInstance()

class WorkStationMsgCtrl final
{
private:
    WorkStationMsgCtrl() = default;

    using FUNC = std::function<void(const CArmCmdBase*)>;

public:
    static WorkStationMsgCtrl* GetInstance();

    ~WorkStationMsgCtrl() = default;

    void RegisterObserver(const ClientType type, FUNC f);

    void Request(const ClientType type, const CArmCmdBase* cmd) const;

private:
    void TouchPanelRequest(const CArmCmdBase* cmd) const;
    void WSGUIRequest(const CArmCmdBase* cmd) const;
    void DevStatusRequest(const CArmCmdBase* cmd) const;
    void ScanInfoRequest(const CArmCmdBase* cmd) const;
    void ScanCtrlRequest(const CArmCmdBase* cmd) const;
    void OutCmd(const ClientType& t, const CArmCmdBase* cmd) const;

private:
    mutable std::map<ClientType, FUNC> m_client_O;  // 各个用户的输出函数
};