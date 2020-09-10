#pragma once

#pragma execution_character_set("utf-8")

#include <cstdint>
#include <vector>
#include <filesystem>
#include <functional>

#include <QDebug>
#include <QTcpSocket>

//#include <glog_x64\Include\logging.h>

#include "SeqFileBase.h"

class Msg_t;

namespace fs = std::experimental::filesystem;

// seq文件中最小的数据长度
using SeqUnit_t = std::uint16_t;
class SeqFile;
//using SeqFile_t = SeqFile;
using SeqFileVec_t = std::vector<SeqFileBase*>;

constexpr static char* LOG_DIR = "./log";               // log文件目录
constexpr static uint32_t PER_PACKAGE_BYTE = 10000;     // 每次发送的数据包有多少字节

template <typename C, typename V>
bool ueg_contains(const C& c, const V v)
{
    return std::end(c) != std::find(std::begin(c), std::end(c), v);
}

namespace UegNetwork
{
    // socket是否已经连接
    bool isConnected(const QTcpSocket* s);
    // 向socket s发送指定长度的数据
    bool sendData(QTcpSocket* s, const void* data, const size_t length);
    bool sendData(QTcpSocket* s, const Msg_t& msg);
}

namespace UEG_DIR
{
    // 判断目录是否存在
    bool exists(const fs::path& path);
    // 判断目录是否为空
    bool empty(const fs::path& path);
    // 创建目录
    void make(const fs::path& path);
}

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> job) : job_(job) {}
    ~ScopeGuard()
    {
        if (job_)
        {
            job_();
        }
    }

private:
    std::function<void()> job_;

private: // noncopyable
    ScopeGuard(ScopeGuard const&) = delete;
    ScopeGuard& operator=(ScopeGuard const&) = delete;
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)