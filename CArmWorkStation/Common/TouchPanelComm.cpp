#include "TouchPanelComm.h"
#include "CArmScCmdDef.h"

#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

extern Logger g_logger;

TouchPanelComm::TouchPanelComm(const std::string &strPort, bool bSingleClient /*= false*/)
    : IPCServerBin("TPServer", strPort, bSingleClient)
    , m_soc(INVALID_SOCKET)
{

}

TouchPanelComm::~TouchPanelComm()
{

}

void TouchPanelComm::ParseCmdsRecv(const std::list<std::unique_ptr<CArmCmdBase>> cmds)
{
    for (auto& i : cmds)
    {
        LOG4CPLUS_INFO_FMT(g_logger, L"[TP => WS][%04X][%04X]%s", i->m_u16MsgID, i->m_u16Cmd, TOWS(i->ToString()));
        if (m_newMsg)
        {
            m_newMsg({ i.get() });
        }
    }
}

void TouchPanelComm::OnConnected(SOCKET soc)
{
    m_soc = soc;

    LOG4CPLUS_INFO(g_logger, L"The touch screen and workstation are connected");

    if (m_newMsg)
    {
        auto msg = std::make_unique<CArmCmdTpWsNetStatus>(CArmNetStatus::Connected);
        m_newMsg({ msg.get()});
    }
}

void TouchPanelComm::OnDisconnected(SOCKET soc)
{
    if (soc == m_soc) m_soc = INVALID_SOCKET;

    if (m_newMsg)
    {
        auto msg = std::make_unique<CArmCmdTpWsNetStatus>(CArmNetStatus::Disconnected);
        m_newMsg({ msg.get() });
    }
}

void TouchPanelComm::Notify(const CArmCmdBase* msg)
{

}

void TouchPanelComm::SendCMD(const CArmCmdBase* cmd)
{
    if (IsClientConnected())
    {
        std::vector<char> vec;
        cmd->ToVec(vec);
        SendCommand(m_soc, vec);
    }
}
