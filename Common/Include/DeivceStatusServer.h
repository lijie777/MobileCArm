#ifndef DEVICE_STATUS_SERVER_H
#define DEVICE_STATUS_SERVER_H

#include <IPCServerBase.h>

class UDeivceStatusServer : public IPCServerBase
{
public:

    /**
    * @brief 构造函数
    * @param strPort 端口号
    * @param bSingleClient 是否单个客户端连接
    * @return  
    */
    UDeivceStatusServer(const std::string &strPort, bool bSingleClient = false);

    /**
    * @brief 析构函数
    * @return  
    */
    ~UDeivceStatusServer();

    /**
    * @brief 解析收到的子进程发来的消息
    * @param soc 子进程连接的socket
    * @param strClientName 子进程名称
    * @param lstCmd 收到的消息
    * @return void 
    */
    virtual void ParseCmdsRecv(SOCKET soc, const std::string &strClientName, const std::vector<std::string> &lstCmd);

    /**
    * @brief 客户端与服务端建立连接后的处理
    * @param soc DICOM服务申请连接socket
    * @param strModuleName DICOM服务申请进程名称
    * @return void 
    */
    virtual void OnConnected(SOCKET soc, const std::string &strModuleName) {};

    /**
    * @brief 客户端与服务端连接中断时的处理
    * @param soc DICOM服务申请连接socket
    * @return void
    */
    virtual void OnDisconnected(SOCKET soc) {};

    std::function<void(ErrorCode)> OnDevStatusChanged;
};

#endif//DEVICE_STATUS_SERVER_H