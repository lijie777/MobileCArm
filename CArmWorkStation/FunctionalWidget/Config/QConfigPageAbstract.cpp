#include "QConfigPageAbstract.h"
#include "ui_SystemConfigWidget.h"

CArmConfig QConfigPageAbstract::cfg = CArmConfigHandler::Deserialize();
Ui::SystemConfigWidget *QConfigPageAbstract::ui;

QConfigPageAbstract::QConfigPageAbstract(QWidget *parent) : QWidget(parent)
{
}

QConfigPageAbstract::~QConfigPageAbstract()
{

}

void QConfigPageAbstract::init()
{
    //this->updatePage();
}

void QConfigPageAbstract::setUiPtr(Ui::SystemConfigWidget *ui_)
{
    ui = ui_;
//    if(ui->stackedWidget->currentIndex() == 0){
//        this->updatePage();
//    }
//    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [=](const int &currentIndex){
//        ui->stackedWidget->currentIndex() == currentIndex ? this->updatePage() :NULL;
//    });
}

void QConfigPageAbstract::saveSystemCfg()
{
    CArmConfigHandler::Serialize(cfg);
}

void QConfigPageAbstract::updateSysCfg()
{
    cfg = CArmConfigHandler::Deserialize();
}
