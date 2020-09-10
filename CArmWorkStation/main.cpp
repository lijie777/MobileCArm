#include <WinSock2.h>

#include "CArmWorkStation.h"
#include <QtWidgets/QApplication>

#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <CommonUtility.h>
#include <windows.h>

#include "Common/CArmWorkStationProc.h"

using namespace log4cplus;
using namespace log4cplus::helpers;

Logger g_logger;

void InitConsoleLogger()
{
#ifdef CONSOLE_LOG

    AllocConsole();//为当前的窗口程序申请一个Console窗口
    freopen("CONOUT$", "a+", stdout);

    PropertyConfigurator::doConfigure(TOWS(UCommonUtility::GetExeDir()) + std::wstring(L"\\Config\\log.cfg"));
#endif
    g_logger = Logger::getInstance(L"CArmWorkStation");
}

int main(int argc, char *argv[])
{
    InitConsoleLogger();

    CArmWorkStationProc proc;

    if (!proc.LoadConfig())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -1;
    }

    if (!proc.ArgsParse(argc, argv))
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -2;
    }

    if (!proc.StartHeartbeat())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -3;
    }

    if (!proc.StartLogging())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -4;
    }

    if (!proc.GetServicePorts() || !proc.StartServices())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -4;
    }

    if (!proc.Run())
    {
#ifdef _DEBUG
        system("pause");
#endif
        return -5;
    }

    QApplication a(argc, argv);

    CArmWorkStation w(&proc);
    return a.exec();
}
