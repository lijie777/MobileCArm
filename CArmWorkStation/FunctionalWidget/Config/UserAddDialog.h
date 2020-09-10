/**
* @file UserAddDialog.h
* @brief 增加用户弹出框（设置用户名，密码，角色）
* @author 李杰
*/
#pragma once

#include "QDialogAbstract.h"
#include "ui_UserAddDialog.h"

#pragma execution_character_set("utf-8")

class UserAddDialog : public QDialogAbstract
{
    Q_OBJECT

public:
    explicit UserAddDialog(QWidget *parent = 0);
    ~UserAddDialog();

private slots:
    void closeBtnSlot();
    void certatinBtnSlot();

protected:
    Ui::UserAddDialog ui;
};