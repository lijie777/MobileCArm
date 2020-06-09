#pragma once

//#include <WinSock2.h>
#include <string>
#include "TCPClient.h"
#include <thread>
#include <list>
#include "IPCDefinitions.h"

class CArmCmdBase;

/**
 * @brief 通讯客户端类
 */
class IPCClientBin
{
public:
    /**
     * @brief 通讯客户端类构造函数
     * @param strModuleName 模块名称（进程名称）
     * @param strIp IP
     * @param strPort 端口号
     * @return  
     */
    IPCClientBin(std::string strModuleName, std::string strIp, std::string strPort);

    /**
     * @brief 通讯客户端类析构函数
     * @return  
     */
    ~IPCClientBin();

    void ConnectThd();

    /**
     * @brief 断开连接
     * @return bool 是否断开连接成功
     */
    bool Disconnect();

    /**
     * @brief 查询是否已连接
     * @return bool 是否已连接
     */
    bool IsConnected();

    /**
     * @brief 
     * @param strCmd 发送命令
     * @return bool 是否发送成功
     */
    bool SendCommand(const std::vector<char>& data);

    ///处理收到的命令
    std::function<void(const std::list<std::unique_ptr<CArmCmdBase>>)> OnCmdRecv;

    ///连接断开时的回调函数
    std::function<void(void)> OnDisconnected;

    std::function<void()> OnConnected;

protected:
    CTCPClient m_client;         //!< 客户端
    std::string m_strModuleName; //!< 模块名称（进程名称）

private:
    void SendHeartbeatThd();

    /**
     * @brief 连接服务器
     * @param strPort 端口号
     * @return bool 是否连接成功
     */
    bool Connect();

private:
	std::string m_strIp;      //!< 服务器IP地址
	std::string m_strPort;    //!< 服务器端口
	//std::string m_strLogHdr;  //!< 日志消息头
	//bool m_bConnClosed;     //!< 连接是否已主动关闭
	int m_nWatchDog;          //!< 心跳包计数
	void RecvThread();        //!< 读取线程
	//std::list<std::string> ExtractCmds(std::string &strCmdBuff); //!< 从读取到的数据中提取完整命令
	//void SendHeartbeatThd();  //!< 发送心跳包的线程
};