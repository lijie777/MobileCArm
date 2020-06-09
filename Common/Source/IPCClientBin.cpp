#include <WinSock2.h>

#include "Common/Include/CArmScCmdDef.h"
#include "Common/Include/IPCClientBin.h"
#include "IPCDefinitions.h"
#include "Common/Include/IPCBinBase.h"

using namespace std::literals::chrono_literals;

IPCClientBin::IPCClientBin(std::string strModuleName, std::string strIp, std::string strPort)
{
    m_strModuleName = strModuleName;
    m_strIp = strIp;
    m_strPort = strPort;
    OnCmdRecv = nullptr;
    OnDisconnected = nullptr;
}

IPCClientBin::~IPCClientBin()
{
    OnCmdRecv = nullptr;
    OnDisconnected = nullptr;

    Disconnect();
}

void IPCClientBin::ConnectThd()
{
    std::thread([this]
    {
        while (true)
        {
            std::this_thread::sleep_for(1s);

            if (IsConnected())
            {
                continue;
            }

            if (!Connect())
            {
                LOG4CPLUS_INFO(g_logger, L"connect to workstation failed");
            }
        }
    }
    ).detach();
}

bool IPCClientBin::Connect()
{
    if (IsConnected())
    {
        return true;
    }

    if (!m_client.Connect(m_strIp, m_strPort))
    {
        return false;
    }

    {
        // 向服务器发送握手信息
        CArmCmdC2SHandShake msg(true);
        const auto vec = ::ToVec(&msg);
        m_client.Send(vec);
    }

    {
        auto buff = std::make_unique<CArmCmdS2CACK>();
        const auto n = m_client.Receive((char*)(buff->GetData()), buff->GetSize(), false);

        if (n != buff->GetSize())
        {
            Disconnect();
            return false;
        }

        if (WS_CARM_S_2_C_ACK != ((CArmCmdBase*)(buff.get()))->m_u16Cmd)
        {
            Disconnect();
            return false;
        }
    }

    if (OnConnected)
    {
        OnConnected();
    }

    std::thread(&IPCClientBin::RecvThread, this).detach();
    std::thread(&IPCClientBin::SendHeartbeatThd, this).detach();

    return true;
}

void IPCClientBin::SendHeartbeatThd()
{
    m_nWatchDog = 0;

    while (m_client.IsConnected())
    {
        CArmCmdNetHB hb;
        const auto msg = ::ToVec(&hb);
        if (!m_client.Send(msg))
        {
            Disconnect();
            break;
        }

        if (++m_nWatchDog >= MAX_MISSED_HEARTBEAT)
        {
            Disconnect();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL_MS));
    }
}

bool IPCClientBin::Disconnect()
{
    if (OnDisconnected)
    {
        OnDisconnected();
    }

    if (IsConnected())
    {
        m_client.Disconnect();
    }

    return true;
}

bool IPCClientBin::IsConnected()
{
    return m_client.IsConnected();
}

bool IPCClientBin::SendCommand(const std::vector<char>& data)
{
    return m_client.Send(data.data(), data.size());
}

void IPCClientBin::RecvThread()
{
    char recvBuff[128]{};
    std::vector<std::uint8_t> totalBuff;
    totalBuff.reserve(128);

    while (true)
    {
        int nRet = m_client.Receive(recvBuff, sizeof(recvBuff), false);
        if (nRet <= 0)
        {
            Disconnect();
            return;
        }

        for (int i = 0; i < nRet; ++i)
        {
            totalBuff.push_back(recvBuff[i]);
        }

        auto cmds = ExtractCommands(totalBuff);

        if (::EraseHB(cmds))
        {
            m_nWatchDog = 0;
        }

        auto concreteCmd = ToConcreteCmds(cmds);

        if (concreteCmd.empty())
        {
            continue;
        }

        if (OnCmdRecv)
        {
            std::thread(OnCmdRecv, std::move(concreteCmd)).detach();
        }
    }
}
