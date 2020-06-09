/**
* @file CArmDose.h
* @brief 脉冲频率选择界面（暂时包括2PPS,4PPS,8PPS,12PPS）
* @author 李杰
*/
#pragma once

#include <QWidget>
#include "ui_CArmDose.h"

class CArmDose : public QWidget
{
    Q_OBJECT

public:
    CArmDose(QWidget *parent = Q_NULLPTR);
    ~CArmDose();


    Ui::CArmDose ui;
};
