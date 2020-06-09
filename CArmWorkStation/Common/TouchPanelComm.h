#pragma once

#include "IPCServerBin.h"

class TouchPanelComm : public IPCServerBin
{
public:
    TouchPanelComm(const std::string &strPort, bool bSingleClient = false);
    ~TouchPanelComm();

    /**
    * @brief 解析收到的子进程发来的消息
    * @param soc 子进程连接的socket
    * @param strClientName 子进程名称
    * @param lstCmd 收到的消息
    * @return void 
    */
    void ParseCmdsRecv(const std::list<std::unique_ptr<CArmCmdBase>> cmds) override;

    /**
    * @brief 客户端与服务端建立连接后的处理
    * @param soc DICOM服务申请连接socket
    * @param strModuleName DICOM服务申请进程名称
    * @return void 
    */
    void OnConnected(SOCKET soc) override;

    /**
    * @brief 客户端与服务端连接中断时的处理
    * @param soc DICOM服务申请连接socket
    * @return void
    */
    void OnDisconnected(SOCKET soc) override;

    void Notify(const CArmCmdBase* msg);

    void SendCMD(const CArmCmdBase* cmd);

    bool IsClientConnected() { return m_soc != INVALID_SOCKET; }

    std::function<void(const std::list<CArmCmdBase*>&)> m_newMsg;

private:
    SOCKET m_soc;
};