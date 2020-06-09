#include "WorkStationMsgCtrl.h"
#include "CArmScCmdDef.h"
#include "CArmCommon.h"

WorkStationMsgCtrl* WorkStationMsgCtrl::GetInstance()
{
    static auto msgctrl = new WorkStationMsgCtrl;
    return msgctrl;
}

void WorkStationMsgCtrl::RegisterObserver(const ClientType type, FUNC f)
{
    m_client_O[type] = f;
}

void WorkStationMsgCtrl::Request(const ClientType type, const CArmCmdBase* cmd) const
{
    static const std::map<ClientType, FUNC> mympa
    {
        { ClientType::DEV_STSTUS_SERVER, [this](const auto& arg) { DevStatusRequest(arg); } },
        { ClientType::SCAN_INFO_SERVER, [this](const auto& arg) { ScanInfoRequest(arg); } },
        { ClientType::WORKSTATION_GUI,[this](const auto& arg){ WSGUIRequest(arg); } },
        { ClientType::TP, [this](const auto& arg) { TouchPanelRequest(arg); } },
        { ClientType::SCAN_CTRL, [this](const auto& arg) { ScanCtrlRequest(arg); }}
    };

    try
    {
        (mympa.at(type))(cmd);
    }
    catch (const std::out_of_range&)
    {

    }
}

void WorkStationMsgCtrl::TouchPanelRequest(const CArmCmdBase* cmd) const
{
    if (WS_CARM_SET_SCAN_PARAM == cmd->Type())
    {
        OutCmd(ClientType::SCAN_CTRL, cmd);
    }
    else if ((WS_CARM_IMG_TRANSFER == cmd->Type())
        || (WS_CARM_IMG_ADJUST_PARAM == cmd->Type())
        || (WS_CARM_SET_SCAN_PARAM == cmd->Type())
        || (WS_CARM_TP_WS_NET_STATUS == cmd->Type()))
    {
        OutCmd(ClientType::WORKSTATION_GUI, cmd);
    }
    else
    {
        // 其他的交给扫描信息服务处理
        OutCmd(ClientType::SCAN_INFO_SERVER, cmd);
    }

    if (auto cmd_ = dynamic_cast<const CArmCmdTpWsNetStatus*>(cmd))
    {
        if (CArmNetStatus::Connected == cmd_->m_arg)
        {
            CArmCmdGetAllDevInfo allDevInfo{};
            OutCmd(ClientType::SCAN_INFO_SERVER, &allDevInfo);
        }
    }
}

void WorkStationMsgCtrl::WSGUIRequest(const CArmCmdBase* cmd) const
{
    if (WS_CARM_NEW_PATIENT_INFO == cmd->Type())
    {
        OutCmd(ClientType::TP, cmd);
    }
    else if (auto cmd_ = dynamic_cast<const CArmCmdWorkStationStatus*>(cmd))
    {
        if ((CArmWorkStationStatusTypeDef::USER_LOGIN == cmd_->m_arg) ||
            (CArmWorkStationStatusTypeDef::USER_LOGOUT == cmd_->m_arg))
        {
            OutCmd(ClientType::TP, cmd);
            OutCmd(ClientType::SCAN_CTRL, cmd);
        }
    }
    else
    {
        // 其他的交给扫描信息服务处理
        OutCmd(ClientType::SCAN_INFO_SERVER, cmd);
    }
}

void WorkStationMsgCtrl::DevStatusRequest(const CArmCmdBase* cmd) const
{
    OutCmd(ClientType::WORKSTATION_GUI, cmd);
    OutCmd(ClientType::TP, cmd);

    if (auto cmd_ = dynamic_cast<const CArmCmdDevStatusChanged*>(cmd))
    {
        if (ErrorCode::ERR_SUCCESS == cmd_->m_arg)
        {
            CArmCmdGetAllDevInfo allDevInfo{};
            OutCmd(ClientType::SCAN_INFO_SERVER, &allDevInfo);
        }
    }
}

void WorkStationMsgCtrl::ScanInfoRequest(const CArmCmdBase* cmd) const
{
    if (UegContains({ SC_CARM_PEDAL_PRESSED ,SC_CARM_EXP_BTN1_PRESSED, SC_CARM_EXP_BTN2_PRESSED, WS_CARM_NEW_IMG_RECEIVED }, cmd->Type()))
    {
        OutCmd(ClientType::SCAN_CTRL, cmd);
        return;
    }
    OutCmd(ClientType::WORKSTATION_GUI, cmd);
    OutCmd(ClientType::TP, cmd);
}

void WorkStationMsgCtrl::ScanCtrlRequest(const CArmCmdBase* cmd) const
{
    if (UegContains({ SC_CARM_PEDAL_PRESSED ,SC_CARM_EXP_BTN1_PRESSED, SC_CARM_EXP_BTN2_PRESSED, WS_CARM_NEW_IMG_RECEIVED, WS_CARM_IPC_WS_STATUS }, cmd->Type()))
    {
        OutCmd(ClientType::WORKSTATION_GUI, cmd);
        OutCmd(ClientType::TP, cmd);
    }
    else if (auto cmd_ = dynamic_cast<const CArmCmdSlaveScanCtrl*>(cmd))
    {
        if (m_client_O[ClientType::SCAN_INFO_SERVER])
        {
            (m_client_O[ClientType::SCAN_INFO_SERVER])(cmd_);
        }
    }
}

void WorkStationMsgCtrl::OutCmd(const ClientType& t, const CArmCmdBase* cmd) const
{
    try
    {
        (m_client_O.at(t))(cmd);
    }
    catch (const std::out_of_range&)
    {

    }
}
