#include "IPCServerBin.h"
#include "IPCDefinitions.h"
#include "IPCBinBase.h"
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

extern Logger g_logger;

using namespace std::literals::chrono_literals;

IPCServerBin::IPCServerBin(const std::string &strModuleName, const std::string &strPort, bool bSingleClient /*= false*/)
{
    m_bSingleClient = bSingleClient;
    ModuleName = strModuleName;
    m_strPort = strPort;
    m_bShutingdown = false;
}

IPCServerBin::~IPCServerBin()
{
    Shutdown();
}

bool IPCServerBin::SetupServer()
{
    std::thread(&IPCServerBin::AcceptThread, this).detach();
    return true;
}

bool IPCServerBin::ShutdownServer()
{
    m_bServerShutdown = true;
    m_server.StopServer();

    return true;
}

void IPCServerBin::Shutdown()
{
    m_bShutingdown = true;

    const auto mymap = (std::lock_guard<std::mutex>(m_clientMutext), std::exchange(m_socMutext, {}));
    for (auto& i : mymap)
    {
        Disconnect(i.first);
    }

    ShutdownServer();
}

bool IPCServerBin::SendCommand(SOCKET soc, const std::vector<char> &cmd)
{
    if (std::end(m_socMutext) == m_socMutext.find(soc))
    {
        return false;
    }

    std::lock_guard<std::mutex> lg1(m_clientMutext);
    std::lock_guard<std::mutex> lg(*m_socMutext[soc]);

    return m_server.Send(soc, cmd);
}

void IPCServerBin::Disconnect(const SOCKET socket_)
{
    m_server.Disconnect(socket_);

    DeleteClient(socket_);

    OnDisconnected(socket_);
}

void IPCServerBin::AcceptThread()
{
    while (true)
    {
        if (m_bShutingdown)
        {
            break;
        }

        if (m_bSingleClient)
        {
            if (std::lock_guard<std::mutex>(m_clientMutext), !m_socMutext.empty())
            {
                std::this_thread::sleep_for(1s);
                continue;
            }
        }

        m_bShutingdown = false;
        m_bServerShutdown = false;

        SOCKET soc = INVALID_SOCKET;
        if (!m_server.Listen(soc, m_strPort))
        {
            continue;
        }

        auto longConn = false;

        {
            CArmCmdC2SHandShake msg(false);
            const auto nRecv = m_server.Receive(soc,((char*)(msg.GetData())), msg.GetSize(), false);
            if (msg.GetSize() != nRecv)
            {
                Disconnect(soc);
                continue;
            }

            if (WS_CARM_C_2_S_HANDSHAKE != ((CArmCmdBase*)(&msg))->m_u16Cmd)
            {
                Disconnect(soc);
                continue;
            }

            longConn = msg.m_arg;
        }

        {
            CArmCmdS2CACK msg;
            const auto vec = ::ToVec(&msg);
            m_server.Send(soc, vec);
        }

        AddClient(soc);

        OnConnected(soc);

        std::thread(&IPCServerBin::RecvThread, this, soc).detach();

        if (longConn)
        {
            std::thread(&IPCServerBin::HeartbeatCheckThd, this, soc).detach();
        }
    }
}

void IPCServerBin::AddClient(const SOCKET socket_)
{
    {
        std::lock_guard<std::mutex> lg(m_clientMutext);
        m_socMutext.insert({ socket_, std::make_unique<std::mutex>()});
    }

    {
        std::lock_guard<std::mutex> lg(m_watchDogMutex);
        m_WatchDogs.insert({socket_, 0});
    }
}

void IPCServerBin::DeleteClient(const SOCKET socket_)
{
    {
        std::lock_guard<std::mutex> lg(m_clientMutext);
        m_socMutext.erase(socket_);
    }

    {
        std::lock_guard<std::mutex> lg(m_watchDogMutex);
        m_WatchDogs.erase(socket_);
    }
}

bool IPCServerBin::IsConnected(const SOCKET socket_)
{
    std::lock_guard<std::mutex> lg(m_clientMutext);
    return (std::end(m_socMutext) != m_socMutext.find(socket_));
}

void IPCServerBin::RecvThread(SOCKET soc)
{
    char recvBuff[128]{};

    std::vector <std::uint8_t> totalBuff;
    totalBuff.reserve(128);

    while (true)
    {
        int nRet = m_server.Receive(soc, recvBuff, sizeof(recvBuff), false);
        if (nRet <= 0)
        {
            Disconnect(soc);
            return;
        }

        for (int i = 0; i < nRet; i++)
        {
            totalBuff.push_back(recvBuff[i]);
        }

        auto cmds = ExtractCommands(totalBuff);

        if (::EraseHB(cmds))
        {
            CArmCmdNetHB hb;
            const auto msg = ::ToVec(&hb);
            m_server.Send(soc, msg);

            std::lock_guard<std::mutex> lg(m_watchDogMutex);
            if (std::end(m_WatchDogs) != m_WatchDogs.find(soc))
            {
                m_WatchDogs[soc] = 0;
            }
        }

        auto concreteCmd = ToConcreteCmds(cmds);

        if (!concreteCmd.empty())
        {
            std::thread(&IPCServerBin::ParseCmdsRecv, this, std::move(concreteCmd)).detach();
        }
    }
}

void IPCServerBin::HeartbeatCheckThd(SOCKET soc)
{
    while (IsConnected(soc))
    {
        {
            std::lock_guard<std::mutex> lg(m_watchDogMutex);

            if (std::end(m_WatchDogs) == m_WatchDogs.find(soc))
            {
                Disconnect(soc);
                break;
            }

            if (++(m_WatchDogs[soc]) >= MAX_MISSED_HEARTBEAT)
            {
                Disconnect(soc);
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(HEARTBEAT_INTERVAL_MS));
    }
}