#include <chrono>
#include <thread>
#include <memory>

#include "WorkStationComm.h"

#include <log4cplus/loggingmacros.h>
#include "Common/Include/SlaveComputerCArmBase.h"

#include "Common/Include/CArmScCmdDef.h"

extern Logger g_logger;

using namespace std::literals::chrono_literals;

WorkStationComm::WorkStationComm(std::string strModuleName, std::string strIp, std::string strPort)
    : m_client(new IPCClientBin(strModuleName, strIp, strPort))
{
    using namespace std::placeholders;
    m_client->OnCmdRecv = std::bind(&WorkStationComm::OnCmdRecvPrivate, this, _1);
    m_client->OnConnected = std::bind(&WorkStationComm::OnConnected, this);
    m_client->OnDisconnected = std::bind(&WorkStationComm::OnDisconnected, this);
}

bool WorkStationComm::Request(const CArmCmdBase* cmd)
{
    const auto cmd_ = ::ToVec(cmd);
    return m_client->SendCommand(cmd_);
}

void WorkStationComm::Connect()
{
    m_client->ConnectThd();
}

void WorkStationComm::OnCmdRecvPrivate(const std::list<std::unique_ptr<CArmCmdBase>> cmds)
{
    for (auto& i : cmds)
    {
        LOG4CPLUS_INFO_FMT(g_logger, L"[WS => TP][%04X][%04X]%s", i->m_u16MsgID, i->m_u16Cmd, TOWS(i->ToString()));
        if (OnCmdRecv)
        {
            OnCmdRecv(i.get());
        }
    }
}

void WorkStationComm::OnConnected()
{
    LOG4CPLUS_INFO(g_logger, L"The touch screen and workstation are connected");

    auto msg = std::make_unique<CArmCmdTpWsNetStatus>(CArmNetStatus::Connected);
    if (OnCmdRecv)
    {
        OnCmdRecv(msg.get());
    }
}

void WorkStationComm::OnDisconnected()
{
    LOG4CPLUS_INFO(g_logger, L"The touch screen and workstation are Disconnected");

    auto msg = std::make_unique<CArmCmdTpWsNetStatus>(CArmNetStatus::Disconnected);
    if (OnCmdRecv)
    {
        OnCmdRecv(msg.get());
    }
}
