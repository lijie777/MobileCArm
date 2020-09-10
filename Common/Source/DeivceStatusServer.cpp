#include "DeivceStatusServer.h"
#include <log4cplus/loggingmacros.h>
#include <IPC_CMD_Def_CArm.h>
#include <IPCDefinitions.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

extern Logger g_logger;

UDeivceStatusServer::UDeivceStatusServer(const std::string &strPort, bool bSingleClient)
    : IPCServerBase("DeviceStatusServer", strPort, bSingleClient)
{
}

UDeivceStatusServer::~UDeivceStatusServer()
{
}

void UDeivceStatusServer::ParseCmdsRecv(SOCKET soc, const std::string &strClientName, const std::vector<std::string> &lstCmd)
{
    for (auto iter = lstCmd.begin(); iter != lstCmd.end(); iter++)
    {
        LOG4CPLUS_INFO_FMT(g_logger, TOWS(*iter));

        auto subCmds = UCommonUtility::StringSplit(*iter, CMD::CMD_SEPARATOR);
        if (subCmds[0] == CArmIPCCMD::UPDATE_DEV_STATUS)
        {
            if (OnDevStatusChanged) OnDevStatusChanged((ErrorCode)std::stoi(subCmds[1]));
        }
    }
}