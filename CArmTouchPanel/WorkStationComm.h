#pragma once

#include "Common/Include/IPCClientBin.h"

class WorkStationComm
{
public:
    WorkStationComm(std::string strModuleName, std::string strIp, std::string strPort);

    std::function<void(const CArmCmdBase* cmd)> OnCmdRecv;
    bool Request(const CArmCmdBase* cmd);

    void Connect();

private:
    void OnCmdRecvPrivate(const std::list<std::unique_ptr<CArmCmdBase>> cmds);
    void OnConnected();
    void OnDisconnected();

private:
    IPCClientBin* m_client;
};