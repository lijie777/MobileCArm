#include "configdataprovider.h"

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

namespace SysConfig {

ConfigDataProvider::ConfigDataProvider(QObject *parent) : QObject(parent)
{
	configMenuList<<"系统信息"
		<<"图像采集"
		<<"患者信息设置"
		<<"时间区域设置"
		<<"C型臂设置"
        <<"拍摄设置"
		<<"仿G臂操作"
        <<"网络&DICOM"
        <<"Worklist"
        <<"DICOM打印"
        <<"存储服务器"
        <<"Query&Retrieve"
        <<"MPPS Server"
        <<"平板探测器配置"
        <<"自动调节窗宽窗位"
        <<"日志查看"
		<<"用户设置"
		<<"角色设置"
        //<<"传输状态查看"
        //<<"安全设置"
        //<<"系统维护"
        <<"恢复出厂设置";

    scanOrganList<<"四肢"
        <<"颈部"
        <<"头部"
        <<"躯干"
        <<"心脏"
        <<"腹部"
        <<"泌尿系统"
        <<"软组织"
        <<"内窥镜";

    scanModeList<<"点拍"
                <<"脉冲"
                <<"视频";
}

void ConfigDataProvider::setConfigMenuList(const QList<QString> menuList)
{
    configMenuList = menuList;
}

const QList<QString> ConfigDataProvider::getConfigMenuList()
{
    return configMenuList;
}

void ConfigDataProvider::setScanOrganList(const QList<QString> scanOrganList_)
{
    scanOrganList = scanOrganList_;
}

const QList<QString> ConfigDataProvider::getScanOrganList()
{
    return scanOrganList;
}

void ConfigDataProvider::setScanModeList(const QList<QString> scanModeList_)
{
    scanModeList = scanModeList_;
}

const QList<QString> ConfigDataProvider::getScanModeList()
{
    return scanModeList;
}

}// namespace SysConfig

