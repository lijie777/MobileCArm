// DevMobileCArmProcess.cpp : 定义控制台应用程序的入口点。
//

//#include "stdafx.h"
#include <log4cplus/configurator.h>
#include <CommonUtility.h>
#include <thread>
#include "IPC.h"

using namespace log4cplus;
using namespace log4cplus::helpers;

Logger g_logger;

void InitConsoleLogger()
{
#ifdef CONSOLE_LOG
    PropertyConfigurator::doConfigure(TOWS(UCommonUtility::GetExeDir()) + std::wstring(L"\\Config\\log.cfg"));
#endif
    g_logger = Logger::getInstance(L"DevMobileCArm");
}

int main(int argc, char *argv[])
{
    InitConsoleLogger();

    UDevCArmProc *proc = new UDevCArmProc;

    if (!proc->ArgsParse(argc, argv))
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -2;
    }

    if (!proc->StartHeartbeat())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -3;
    }

    if (!proc->StartLogging())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -4;
    }

    if (!proc->GetServicePorts())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -5;
    }

    if (!proc->StartServices())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -6;
    }

    if (!proc->Run())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -7;
    }

    while (proc->IsAlive())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    delete proc;

    LOG4CPLUS_INFO(g_logger, L"Process exit");

    g_logger.closeNestedAppenders();
    g_logger.shutdown();

    return 0;
}

