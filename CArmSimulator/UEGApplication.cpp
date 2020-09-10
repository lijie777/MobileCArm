#include "UEGApplication.h"
#include "common.h"
#include "SeqFileParser.h"

void SignalHandle(const char* data, int size)
{
    //LOG(ERROR) << "[Simulator]" << std::string(data, size);
}

UEGApplication::UEGApplication(int &argc, char **argv)
    : QApplication(argc, argv)
{

}

UEGApplication::~UEGApplication()
{
}

// 初始化应用程序
bool UEGApplication::init() const
{
    if (!SeqFileParser::dataDirCheck())
    {
        emit displayMessage("应用程序初始化失败: SEQ文件夹不存在或为空");
        return false;
    }
    if (!SeqFileParser::parseSeq())
    {
        emit displayMessage("应用程序初始化失败: 解析SEQ文件出错");
        return false;
    }
    if (PER_PACKAGE_BYTE <= 0)
    {
        emit displayMessage("应用程序初始化失败: 每次发送的字节数不能小于0");
        return false;
    }

    emit displayMessage("应用程序初始化成功");
    return true;
}

// 初始化log功能
bool UEGApplication::initLog() const
{
    return true;
}