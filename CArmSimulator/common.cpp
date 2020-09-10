#include "common.h"
#include "UEG_CMD.h"

// socket是否已经连接
bool UegNetwork::isConnected(const QTcpSocket* s)
{
    return ((nullptr != s) && (QAbstractSocket::ConnectedState == s->state()));
}

// 向socket s发送指定长度的数据
bool UegNetwork::sendData(QTcpSocket* s, const void* data, const size_t length)
{
    if (UegNetwork::isConnected(s))
    {
        const auto bytes = s->write((const char*)data, length);
        s->flush();
        return (bytes > 0);
    }

    return false;
}

bool UegNetwork::sendData(QTcpSocket* s, const Msg_t& msg)
{
    const auto data = msg.ToVec();
    return UegNetwork::sendData(s, data.data(), data.size());
}

// 判断目录是否存在
bool UEG_DIR::exists(const fs::path& path)
{
    return fs::exists(path);
}

// 判断目录是否为空
bool UEG_DIR::empty(const fs::path& path)
{
    return fs::is_empty(path);
}

// 创建目录
void UEG_DIR::make(const fs::path& path)
{
    fs::create_directory(path);
}
