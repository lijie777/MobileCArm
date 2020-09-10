#pragma once

#include <QtWidgets/QFrame>
#include <QFile>
#include "ui_SystemConfigWidget.h"
#include "configdataprovider.h"
#include "singleton.h"
#include "qnavigationwidget.h"
#include "qconfigpage.h"
#include "Config.h"

#define ToQSTR(v) (QString::fromStdString(v))
#define QStr2Std(v) (v.QString::toStdString())

namespace SysConfig {
class SystemConfigWidget : public QFrame
{
    Q_OBJECT
public:
    SystemConfigWidget(QWidget *parent = 0);
    ~SystemConfigWidget();

private slots:
    void on_saveBtn_clicked();

    void on_resetBtn_clicked();

private:
    void initUI();
    void initLogic();

    void updateSysConfigUi();
    void saveSysConfig();
    void resetSysConfig();

private:

public:
    Ui::SystemConfigWidget *ui;
};

} // namespace SysConfig


