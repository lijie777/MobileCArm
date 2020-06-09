// SlaveComputerMobileCArm.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "SlaveComputerCArm.h"
#include <CommonUtility.h>
#include <IPCDefinitions.h>

USlaveComputerCArm g_Slave;

extern "C" __declspec(dllexport) const PLUGIN_INFO* GetInfo(void)
{
    static PLUGIN_INFO info;

    std::memset(&info, 0, sizeof(PLUGIN_INFO));

    info.m_Type = PluginType::SlaveComputerCArm;
    info.m_bSingleton = true;
    std::strcpy(info.m_strVendor, "UEG");
    std::strcpy(info.m_strVersion, "1.0.0.0");
    std::strcpy(info.m_strModels[0], "SCMobileCArm");

    return &info;
}

extern "C" __declspec(dllexport)void* GetInstance(int nIndex)
{
    return dynamic_cast<USlaveComputerCArmBase*>(&g_Slave);
}