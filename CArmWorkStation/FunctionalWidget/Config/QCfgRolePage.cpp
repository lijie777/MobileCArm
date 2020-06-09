#include "QCfgRolePage.h"
#include "ui_SystemConfigWidget.h"
#include <QTreeWidgetItem>
#include <QTreeWidget>

QCfgRolePage::QCfgRolePage(QWidget *parent) : QConfigPageAbstract(parent)
{

}

QCfgRolePage::~QCfgRolePage()
{
}
void QCfgRolePage::init()
{  
    initTitle();

    ui->treeWidget->setColumnCount(1);
    ui->treeWidget->headerItem()->setHidden(true);

   

}

void QCfgRolePage::updatePage()
{

}
void QCfgRolePage::savePage()
{
}

void QCfgRolePage::enteryPage()
{
}
void QCfgRolePage::exitPage()
{
}

void QCfgRolePage::initTitle()
{
    titleList.push_back("患者管理");
    titleList.push_back("图像目录");
    titleList.push_back("阅片");
    titleList.push_back("登录");
    titleList.push_back("任务中心");
    titleList.push_back("触摸屏");
    titleList.push_back("左屏");
    titleList.push_back("系统设置");

    // 二级菜单
    {
        QVector<QString> v = { "急诊患者录入" , "新建患者", "编辑患者" ,"已预约患者" ,"已执行患者" };
        titleInfoMap.insert("患者管理", v);
    }
    {
        QVector<QString> v = { "当前患者图像目录" , "日期筛选和查询", "图像操作" ,"导入" ,"导出"  ,"打印" ,"归档" };
        titleInfoMap.insert("图像目录", v);
    }
    {
        QVector<QString> v = { "图像标记" , "图像测量", "图像查看" ,"图像编辑" ,"图像切换", "图像移动和缩放",  "图像显示" };
        titleInfoMap.insert("阅片", v);
    }
    {
        QVector<QString> v = { "登录"};
        titleInfoMap.insert("登录", v);
    }
    {
        QVector<QString> v = { "任务中心" };
        titleInfoMap.insert("任务中心", v);
    }
    {
        QVector<QString> v = { "触摸屏" };
        titleInfoMap.insert("触摸屏", v);
    }
    {
        QVector<QString> v = { "左屏" };
        titleInfoMap.insert("左屏", v);
    }
    {
        QVector<QString> v = { "DICOM设置" , "日志查看", "时间区域设置" , "拍摄设置" ,"患者信息设置", "恢复出厂设置",  "平板探测器设置", "用户设置", "图像采集", "仿G臂设置", "设置C型臂" };
        titleInfoMap.insert("系统设置", v);
    }

    // 三级菜单
    {
        QVector<QString> v = { "新建" };
        titleMeauMap.insert("新建患者", v);
    }
    {
        QVector<QString> v = { "新建并拍片" };
        titleMeauMap.insert("新建预约", v);
    }
    {
        QVector<QString> v = { "确认更改" };
        titleMeauMap.insert("编辑患者", v);
    }
    {
        QVector<QString> v = { "查询", "清空", "预约历史", "删除", "编辑", "确认" };
        titleMeauMap.insert("已预约患者", v);
    }
    {
        QVector<QString> v = {"返回", "查询", "清空" };
        titleMeauMap.insert("预约历史界面", v);
    }
    {
        QVector<QString> v = { "查询", "清空", "发送MPPS", "恢复检查", "删除", "编辑", "新检查", "查看监察"};
        titleMeauMap.insert("已执行患者", v);
    }


}

