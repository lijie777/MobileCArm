/**
* @file CArmHardwareControl.h
* @brief 机械控制界面，包括C型臂调节，激光灯，限束器，报警灯
* @author 李杰
*/
#pragma once

#include <QWidget>
#include <QMouseEvent>
#include <QTime>
#include <QTimer>

#include "ui_CArmHardwareControl.h"
#include "../Common/Include/Config.h"
#include "CArmScCmdDef.h"

class CArmHardwareControl : public QWidget
{
    Q_OBJECT

public:
    CArmHardwareControl(QWidget *parent = Q_NULLPTR);
    ~CArmHardwareControl();

    Ui::CArmHardwareControl ui;

    // 更新水平限束器的位置
    void updateCollimatorPosH(float pos)const;
    // 更新水平限束器的位置 
    void updateCollimatorPosV(float pos)const;
    // 报警提示
    void setAlarmLight(bool status);

public slots:
    // C型臂上升按钮按下时间计算
    void updateTimeUp(void);   
    // C型臂下降升按钮按下时间计算
    void updateTimeDown(void);
    // C型臂前移按钮按下时间计算
    void updateTimeFront(void);
    // C型臂后移按钮按下时间计算
    void updateTimeBack(void);
    // C型臂顺时针按钮按下时间计算
    void updateTimeClockwise(void);
    // C型臂逆时针按钮按下时间计算
    void updateTimeAnticlockwize(void);

    // 限束器水平向外调节按钮按下时间计算
    void updateTimeMotionHW(void);
    // 限束器水平向内调节按钮按下时间计算
    void updateTimeMotionHN(void);
    // 限束器垂直向外调节按钮按下时间计算
    void updateTimeMotionVW(void);
    // 限束器垂直向内调节按钮按下时间计算
    void updateTimeMotionVN(void);

private:
    // C型臂上升按钮按下
    void CArmUpPress(void);
    // C型臂下降按钮按下
    void CArmDownPress(void);
    // C型臂前移按钮按下
    void CArmFrontPress(void);
    // C型臂后移按钮按下
    void CArmBackPress(void);
    // C型臂顺时针按钮按下
    void CArmRotateClockwisePress(void);
    // C型臂逆时针按钮按下
    void CArmRotateAnticlockwizePress(void);

    // C型臂上升按钮松开
    void CArmUpRelease(void);
    // C型臂下降按钮松开
    void CArmDownRelease(void);
    // C型臂前移按钮松开
    void CArmFrontRelease(void);
    // C型臂后移按钮松开
    void CArmBackRelease(void);
    // C型臂顺时针按钮松开
    void CArmRotateClockwiseRelease(void);
    // C型臂逆时针按钮松开
    void CArmRotateAnticlockwizeRelease(void);


    // 限束器水平向外调节按钮按下
    void collimatorMotionHWPress(void);
    // 限束器水平向内调节按钮按下
    void collimatorMotionHNPress(void);
    // 限束器垂直向外调节按钮按下
    void collimatorMotionVWPress(void);
    // 限束器垂直向外调节按钮按下
    void collimatorMotionVNPress(void);

    // 限束器水平向外调节按钮松开
    void collimatorMotionHWRelease(void);
    // 限束器水平向内调节按钮松开
    void collimatorMotionHNRelease(void);
    // 限束器垂直向外调节按钮松开
    void collimatorMotionVWRelease(void);
    // 限束器垂直向外调节按钮松开
    void collimatorMotionVNRelease(void);

    // 报警按钮按下
    void warningHandle(void);

signals:
    // 设置激光灯状态信号
    void laserLampStatusSignal(bool);

    // C型臂升降柱上升/下降信号(长按)
    void CArmUpDownSignal(OptionLiftColMove opt, float speed);
    // C型臂前后轴向前/向后信号(长按)
    void CArmFrontBackSignal(OptionXAxisMove opt, float speed);
    // C型臂角度调节（顺时针/逆时针）信号(长按)
    void CArmRotateSignal(OptionCArmMove opt, float speed);

   
    // C型臂升降柱上升/下降信号（非长按）
    void CArmUpDownSignal(float pos, float speed);
    // C型臂前后轴向前/向后信号（非长按）
    void CArmFrontBackSignal(float pos, float speed);
    // C型臂角度调节（顺时针/逆时针）信号（非长按）
    void CArmRotateSignal(float pos, float speed);


    // 限束器调节(长按)
    void collimatorMotionSignal(OptionCollimatorMoveDirection option, CArmDeviceMoveTypeDef opt, float speed);

    // 限束器调节信号（非长按）
    void collimatorMotionSignal(CArmDeviceMoveTypeDef opt, float pos, float speed);

private:
    void init(void);

    QTimer  m_timerUp;
    QTimer  m_timerDown;
    QTimer  m_timerFront;
    QTimer  m_timerBack;
    QTimer  m_timerClockwise;
    QTimer  m_timerAnticlockwize;

    QTime   m_time;
  
    QTimer  m_timerMotionHW;
    QTimer  m_timerMotionHN;
    QTimer  m_timerMotionVW;
    QTimer  m_timerMotionVN;

    mutable float m_collimatorPosH{ 0.0 };  //!< 水平限束器的位置
    mutable float m_collimatorPosV{ 0.0 };  //!< 垂直限束器的位置
};
