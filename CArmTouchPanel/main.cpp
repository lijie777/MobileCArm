#include "CArmTouchPanel.h"
#include <QtWidgets/QApplication>

#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

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
    g_logger = Logger::getInstance(L"CArmTouchPanel");
}


int main(int argc, char *argv[])
{
    InitConsoleLogger();

    QApplication a(argc, argv);
    CArmTouchPanel w;
    w.show();
    return a.exec();
}
