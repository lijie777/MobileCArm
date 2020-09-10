#include "Common/CArmCMD.h"
#include "SysConfigWidget.h"
#include "Dialog.h"

namespace SysConfig {

SystemConfigWidget::SystemConfigWidget(QWidget *parent)
    : QFrame(parent),
      ui(new Ui::SystemConfigWidget)
{
    ui->setupUi(this);

    initUI();
    initLogic();
}

SystemConfigWidget::~SystemConfigWidget()
{
    delete ui;
}

void SystemConfigWidget::initUI()
{
    //zhoupengju@ueg.com.cn: 给每个Page设置ui
    QConfigPageAbstract::setUiPtr(ui);
    for (int index =0; index < ui->stackedWidget->count(); index++) {
        dynamic_cast<QConfigPageAbstract*>(ui->stackedWidget->widget(index))->init();
    }
    //zhoupengju@ueg.com.cn: 初始化系统设置主菜单
    foreach (QString item, Singleton<ConfigDataProvider>::getInstance().getConfigMenuList()) {
        ui->configMenuView->addItem(item);
    }
    updateSysConfigUi();
}

void SystemConfigWidget::initLogic()
{
    //zhoupengju@ueg.com.cn: 关联主菜单和子页面
    dynamic_cast<QConfigPageAbstract*>(ui->stackedWidget->widget(0))->updatePage();
    connect(ui->configMenuView, &QNavigationWidget::currentItemChanged, this, [=](const int &current){
        ui->stackedWidget->setCurrentIndex(current);
        //dynamic_cast<QConfigPageAbstract*>(ui->stackedWidget->currentWidget())->updatePage();
    });
    Singleton<Dialog>::getInstance().setDialogParent(this);
}

void SystemConfigWidget::updateSysConfigUi()
{
    for (int index =0; index < ui->stackedWidget->count(); index++) {
        dynamic_cast<QConfigPageAbstract*>(ui->stackedWidget->widget(index))->updatePage();
    }
}

void SystemConfigWidget::saveSysConfig()
{
    for (int index =0; index < ui->stackedWidget->count(); index++) {
        dynamic_cast<QConfigPageAbstract*>(ui->stackedWidget->widget(index))->savePage();
    }
    QConfigPageAbstract::saveSystemCfg();
    CArmObject::executeCmd(CArmConfigCMD{ QConfigPageAbstract::cfg });
}

void SystemConfigWidget::resetSysConfig()
{
    CArmConfigHandler::Reset();
}

} // namespace SysConfig

void SysConfig::SystemConfigWidget::on_saveBtn_clicked()
{
    saveSysConfig();
}
//zhoupengju@ueg.com.cn:恢复出厂设置
void SysConfig::SystemConfigWidget::on_resetBtn_clicked()
{
    resetSysConfig();
}
