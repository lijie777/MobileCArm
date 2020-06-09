#pragma once

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <list>
#include "TCPServer.h"

#include "CArmScCmdDef.h"

class IPCServerBin
{
public:
    IPCServerBin(const std::string &strModuleName, const std::string &strPort, bool bSingleClient = false);
    virtual ~IPCServerBin();


    /**
     * @brief 连接服务器
      * @return bool 
       */
    bool SetupServer();

    /**
     * @brief 停止服务器
      * @return bool 
       */
    bool ShutdownServer();

    /**
     * @brief 停止服务器并断开所有客户端连接
      * @return void 
       */
    void Shutdown();

    /**
     * @brief 发送命令
      * @param soc 申请服务的进程socket
       * @param strCmd 命令
        * @return bool 是否发送命令成功
         */
    bool SendCommand(SOCKET soc, const std::vector<char> &cmd);

    /**
     * @brief 客户端与服务端建立连接后的处理
      * @param soc 申请服务的进程socket
       * @param strModuleName 模块名称（进程名称）
        * @return void 
         */
    virtual void OnConnected(SOCKET soc) = 0;

    /**
     * @brief 客户端与服务端连接中断时的处理
      * @param soc 申请服务的进程socket
       * @return void 
        */
    virtual void OnDisconnected(SOCKET soc) = 0;
    
    virtual void ParseCmdsRecv(const std::list<std::unique_ptr<CArmCmdBase>> cmds) {}

private:
    void Disconnect(const SOCKET socket_);
    void AddClient(const SOCKET socket_);
    void DeleteClient(const SOCKET socket_);
    bool IsConnected(const SOCKET socket_);

protected:
    CTCPServer m_server;    //!< 服务
    std::string ModuleName; //!< 模块名称（进程名称）
    std::string LogHdr;     //!< 日志消息头
private:
    bool m_bSingleClient;   //!< 是否只有一个客户端连接
    std::string m_strPort;  //!< 服务器端口

    std::map<SOCKET, std::unique_ptr<std::mutex>> m_socMutext;  //!< 当前连接的客户端SOCKET mutex

    std::mutex m_clientMutext;  //!< 客户端mutex

    bool m_bServerShutdown;    //!< 服务是否关闭
    bool m_bShutingdown;       //!< 服务是否正在关闭

    std::map<SOCKET, int> m_WatchDogs;   //!< 各客户端连接的WatchDog
    std::mutex m_watchDogMutex;          //!< 各客户端连接的WatchDogMutex

    /**
     * @brief 监听线程
     * @return void 
     */
    void AcceptThread();

    /**
     * @brief 各个连接的读取线程
     * @param soc 申请服务的进程socket
     * @return void 
     */
    void RecvThread(SOCKET soc);

    /**
     * @brief 检查心跳包WatchDog的线程
      * @param soc 申请服务的进程socket
       * @return void 
        */
    void HeartbeatCheckThd(SOCKET soc);
};