#include "CArmHardwareControl.h"
#include "CArmManager.h"

CArmHardwareControl::CArmHardwareControl(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    init();

}

CArmHardwareControl::~CArmHardwareControl()
{
}

void CArmHardwareControl::init(void)
{
    // 激光灯调节
    connect(ui.lampBtn, &QPushButton::clicked, [this]() {emit laserLampStatusSignal(ui.lampBtn->isChecked());});

    // C型臂升降柱上升
    connect(ui.upBtn, &QPushButton::pressed, [this]() {CArmUpPress(); });
    // C型臂升降柱下降
    connect(ui.downBtn, &QPushButton::pressed, [this]() { CArmDownPress(); });
    // C型臂前后轴向前
    connect(ui.frontBtn, &QPushButton::pressed, [this]() {CArmFrontPress(); });
    // C型臂前后轴向后
    connect(ui.backBtn, &QPushButton::pressed, [this]() {CArmBackPress(); });
    // C型臂角度调节（顺时针）
    connect(ui.clockwiseBtn, &QPushButton::pressed, [this]() {CArmRotateClockwisePress(); });
    // C型臂角度调节（逆时针）
    connect(ui.anticlockwiseBtn, &QPushButton::pressed, [this]() {CArmRotateAnticlockwizePress(); });

    // C型臂升降柱上升
    connect(ui.upBtn, &QPushButton::released, [this]() {CArmUpRelease(); });
    // C型臂升降柱下降
    connect(ui.downBtn, &QPushButton::released, [this]() {CArmDownRelease(); });
    // C型臂前后轴向前
    connect(ui.frontBtn, &QPushButton::released, [this]() {CArmFrontRelease(); });
    // C型臂前后轴向后
    connect(ui.backBtn, &QPushButton::released, [this]() {CArmBackRelease(); });
    // C型臂角度调节（顺时针）
    connect(ui.clockwiseBtn, &QPushButton::released, [this]() {CArmRotateClockwiseRelease(); });
    // C型臂角度调节（逆时针）
    connect(ui.anticlockwiseBtn, &QPushButton::released, [this]() {
        CArmRotateAnticlockwizeRelease(); });

    // 限束器水平向外调节按钮按下
    connect(ui.limiterHwideBtn, &QPushButton::pressed, [this]() {collimatorMotionHWPress(); });
    // 限束器水平向内调节按钮按下
    connect(ui.limiterHnarrowBtn, &QPushButton::pressed, [this]() { collimatorMotionHNPress(); });
    // 限束器垂直向外调节按钮按下
    connect(ui.limiterVwideBtn, &QPushButton::pressed, [this]() {collimatorMotionVWPress(); });
    // 限束器垂直向外调节按钮按下
    connect(ui.limiterVnarrowBtn, &QPushButton::pressed, [this]() {collimatorMotionVNPress(); });

    // 限束器水平向外调节按钮松开
    connect(ui.limiterHwideBtn, &QPushButton::released, [this]() {collimatorMotionHWRelease(); });
    // 限束器水平向内调节按钮松开
    connect(ui.limiterHnarrowBtn, &QPushButton::released, [this]() {collimatorMotionHNRelease(); });
    // 限束器垂直向外调节按钮松开
    connect(ui.limiterVwideBtn, &QPushButton::released, [this]() {collimatorMotionVWRelease(); });
    // 限束器垂直向外调节按钮松开
    connect(ui.limiterVnarrowBtn, &QPushButton::released, [this]() {collimatorMotionVNRelease(); });

    // C型臂按钮按下时间计算
    connect(&m_timerUp, SIGNAL(timeout()), this, SLOT(updateTimeUp()));
    connect(&m_timerDown, SIGNAL(timeout()), this, SLOT(updateTimeDown()));
    connect(&m_timerFront, SIGNAL(timeout()), this, SLOT(updateTimeFront()));
    connect(&m_timerBack, SIGNAL(timeout()), this, SLOT(updateTimeBack()));
    connect(&m_timerClockwise, SIGNAL(timeout()), this, SLOT(updateTimeClockwise()));
    connect(&m_timerAnticlockwize, SIGNAL(timeout()), this, SLOT(updateTimeAnticlockwize()));

    // 限束器按钮按下时间计算
    connect(&m_timerMotionHW, SIGNAL(timeout()), this, SLOT(updateTimeMotionHW()));
    connect(&m_timerMotionHN, SIGNAL(timeout()), this, SLOT(updateTimeMotionHN()));
    connect(&m_timerMotionVW, SIGNAL(timeout()), this, SLOT(updateTimeMotionVW()));
    connect(&m_timerMotionVN, SIGNAL(timeout()), this, SLOT(updateTimeMotionVN()));
   

    connect(ui.warnBtn, &QPushButton::clicked, this, [this]() {warningHandle(); });
}

void CArmHardwareControl::warningHandle(void)
{
    if (ui.warnBtn->isChecked())
    {
        ui.warnBtn->setEnabled(true);
        ui.warnBtn->setChecked(false);
        ui.warnBtn->setEnabled(false);
    }
}


void CArmHardwareControl::updateTimeUp()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_axisMotionZ.m_timeThreshold)
    {
        m_timerUp.stop();
        emit CArmUpDownSignal(OptionLiftColMove::Upward, config.m_axisMotionZ.m_velocity);

    }    
}

void CArmHardwareControl::updateTimeDown()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_axisMotionZ.m_timeThreshold)
    {
        m_timerDown.stop();
        emit CArmUpDownSignal(OptionLiftColMove::Downward, config.m_axisMotionZ.m_velocity);
    }
}
void CArmHardwareControl::updateTimeFront()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_axisMotionX.m_timeThreshold)
    {
        m_timerFront.stop();
        emit CArmFrontBackSignal(OptionXAxisMove::Forward, config.m_axisMotionX.m_velocity);
    }
}
void CArmHardwareControl::updateTimeBack()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_axisMotionX.m_timeThreshold)
    {
        m_timerBack.stop();
        emit CArmFrontBackSignal(OptionXAxisMove::Backward, config.m_axisMotionX.m_velocity);
    }
}
void CArmHardwareControl::updateTimeClockwise()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_axisMotionY.m_timeThreshold)
    {
        m_timerClockwise.stop();
        emit CArmRotateSignal(OptionCArmMove::Clockwise, config.m_axisMotionY.m_velocity);
    }
}
void CArmHardwareControl::updateTimeAnticlockwize()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_axisMotionY.m_timeThreshold)
    {
        m_timerAnticlockwize.stop();
        emit CArmRotateSignal(OptionCArmMove::Anticlockwize, config.m_axisMotionY.m_velocity);
    }
}
void CArmHardwareControl::CArmUpPress()
{
    m_timerUp.start();
    m_time.start();
}

void CArmHardwareControl::CArmDownPress()
{
    m_timerDown.start();
    m_time.start();
}
void CArmHardwareControl::CArmFrontPress()
{
    m_timerFront.start();
    m_time.start();
}
void CArmHardwareControl::CArmBackPress()
{
    m_timerBack.start();
    m_time.start();
}
void CArmHardwareControl::CArmRotateClockwisePress()
{
    m_timerClockwise.start();
    m_time.start();
}
void CArmHardwareControl::CArmRotateAnticlockwizePress()
{
    m_timerAnticlockwize.start();
    m_time.start();
}

void CArmHardwareControl::CArmUpRelease()
{  
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > config.m_axisMotionZ.m_timeThreshold)
    {
        emit CArmUpDownSignal(OptionLiftColMove::Stop, 0.0);
    }// 长按超过200ms后松开，发送停止运动命令
    else
    {
        m_timerUp.stop();
        // 发送C型臂Z轴的位置及速度
        float step = config.m_axisMotionZ.m_step;
        float speed = config.m_axisMotionZ.m_velocity;
        emit CArmUpDownSignal(ui.up_downLbe->text().toFloat()*10 + step, speed);

    }// 按下（不是长按）后松开

}
void CArmHardwareControl::CArmDownRelease()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > config.m_axisMotionZ.m_timeThreshold)
    {
        emit CArmUpDownSignal(OptionLiftColMove::Stop, 0.0);
    }// 长按超过200ms后松开，发送停止运动命令
    else
    {
        m_timerDown.stop();
        // 发送C型臂Z轴的位置及速度
        float step = config.m_axisMotionZ.m_step;
        float speed = config.m_axisMotionZ.m_velocity;
        emit CArmUpDownSignal(ui.up_downLbe->text().toFloat() * 10 - step, speed);

    }// 按下（不是长按）后松开

    
}
void CArmHardwareControl::CArmFrontRelease()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() >  config.m_axisMotionX.m_timeThreshold)
    {
        emit CArmFrontBackSignal(OptionXAxisMove::Stop, 0.0);
    }// 长按超过200ms后松开
    else
    {
        m_timerFront.stop();
        // 发送C型臂Z轴的位置及速度
        float step = config.m_axisMotionX.m_step;
        float speed = config.m_axisMotionX.m_velocity;
        emit CArmUpDownSignal(ui.left_rightLbe->text().toFloat() * 10 + step, speed);

    }// 按下（不是长按）后松开
    
}
void CArmHardwareControl::CArmBackRelease()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > config.m_axisMotionX.m_timeThreshold)
    {
        emit CArmFrontBackSignal(OptionXAxisMove::Stop, 0.0);
    }// 长按超过200ms后松开
    else
    {
        m_timerBack.stop();

        // 发送C型臂Z轴的位置及速度
        float step = config.m_axisMotionX.m_step;
        float speed = config.m_axisMotionX.m_velocity;
        emit CArmUpDownSignal(ui.left_rightLbe->text().toFloat() * 10 - step, speed);
    }// 按下（不是长按）后松开
}
void CArmHardwareControl::CArmRotateClockwiseRelease()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > config.m_axisMotionY.m_timeThreshold)
    {
        emit CArmRotateSignal(OptionCArmMove::Stop, 0.0);
    }// 长按超过200ms后松开
    else
    {
        m_timerClockwise.stop();
        // 发送C型臂Z轴的位置及速度
        float step = config.m_axisMotionY.m_step;
        float speed = config.m_axisMotionY.m_velocity;
        emit CArmUpDownSignal(ui.angleLbe->text().toFloat() * 10 + step, speed);

    }// 按下（不是长按）后松开
    
}
void CArmHardwareControl::CArmRotateAnticlockwizeRelease()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > config.m_axisMotionY.m_timeThreshold)
    {
        emit CArmRotateSignal(OptionCArmMove::Stop, 0.0);
    }// 长按超过200ms后松开，发送停止运动命令
    else
    {
        m_timerAnticlockwize.stop();
        // 发送C型臂Z轴的位置及速度
        float step = config.m_axisMotionY.m_step;
        float speed = config.m_axisMotionY.m_velocity;
        emit CArmUpDownSignal(ui.angleLbe->text().toFloat() * 10 - step, speed);

    }// 按下（不是长按）后松开
}


// 限束器水平向外调节按钮按下时间计算
void CArmHardwareControl::updateTimeMotionHW(void)
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_collimatorMotionH.m_timeThreshold)
    {
        m_timerMotionHW.stop();
              
        emit collimatorMotionSignal(OptionCollimatorMoveDirection::OUTWARD, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_H, config.m_collimatorMotionH.m_velocity);
    }
}
// 限束器水平向内调节按钮按下时间计算
void CArmHardwareControl::updateTimeMotionHN(void)
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > config.m_collimatorMotionH.m_timeThreshold)
    {
        m_timerMotionHN.stop();
      
        emit collimatorMotionSignal(OptionCollimatorMoveDirection::INWARD, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_H, config.m_collimatorMotionH.m_velocity);
    }
}
// 限束器垂直向外调节按钮按下时间计算
void CArmHardwareControl::updateTimeMotionVW(void)
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > CArmConfigHandler::Deserialize().m_collimatorMotionV.m_timeThreshold)
    {
        m_timerMotionVW.stop();
        emit collimatorMotionSignal(OptionCollimatorMoveDirection::OUTWARD, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_V, CArmConfigHandler::Deserialize().m_collimatorMotionV.m_velocity);
    }
}
// 限束器垂直向内调节按钮按下时间计算
void CArmHardwareControl::updateTimeMotionVN(void)
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    // 长按超过200ms
    if (m_time.elapsed() > CArmConfigHandler::Deserialize().m_collimatorMotionV.m_timeThreshold)
    {
        m_timerMotionVN.stop();
        emit collimatorMotionSignal(OptionCollimatorMoveDirection::INWARD, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_V, CArmConfigHandler::Deserialize().m_collimatorMotionV.m_velocity);
    }

}

// 限束器水平向外调节按钮按下
void CArmHardwareControl::collimatorMotionHWPress(void)
{
    m_timerMotionHW.start();
    m_time.start();
}
// 限束器水平向内调节按钮按下
void CArmHardwareControl::collimatorMotionHNPress(void)
{
    m_timerMotionHN.start();
    m_time.start();
}
// 限束器垂直向外调节按钮按下
void CArmHardwareControl::collimatorMotionVWPress(void)
{
    m_timerMotionVW.start();
    m_time.start();
}
// 限束器垂直向外调节按钮按下
void CArmHardwareControl::collimatorMotionVNPress(void)
{
    m_timerMotionVN.start();
    m_time.start();
}

// 限束器水平向外调节按钮松开
void CArmHardwareControl::collimatorMotionHWRelease(void)
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > CArmConfigHandler::Deserialize().m_collimatorMotionH.m_timeThreshold)
    {     
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_leftRight = CArmManager::scanConfigConvert()->m_fov.m_leftRight + config.m_collimatorMotionH.m_velocity;
       
        emit collimatorMotionSignal(OptionCollimatorMoveDirection::STOP, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_H, 0.0);
    }// 长按超过200ms后松开，发送停止运动命令
    else
    {
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_leftRight = CArmManager::scanConfigConvert()->m_fov.m_leftRight + config.m_collimatorMotionH.m_step;
       
        m_timerMotionHW.stop();
        // 发送水平限束器的位置及速度
        float step = CArmConfigHandler::Deserialize().m_collimatorMotionH.m_step;
        float speed = CArmConfigHandler::Deserialize().m_collimatorMotionH.m_velocity;
        emit collimatorMotionSignal(CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_H, m_collimatorPosH - step, speed);

    }// 按下（不是长按）后松开

     //计算帧数
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
}

// 限束器水平向内调节按钮松开
void CArmHardwareControl::collimatorMotionHNRelease(void)
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > CArmConfigHandler::Deserialize().m_collimatorMotionH.m_timeThreshold)
    {
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_leftRight = CArmManager::scanConfigConvert()->m_fov.m_leftRight - config.m_collimatorMotionH.m_velocity;

        emit collimatorMotionSignal(OptionCollimatorMoveDirection::STOP, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_H, 0.0);
    }// 长按超过200ms后松开，发送停止运动命令
    else
    {
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_leftRight = CArmManager::scanConfigConvert()->m_fov.m_leftRight - config.m_collimatorMotionH.m_step;

        m_timerMotionHN.stop();
        // 发送水平限束器的位置及速度
        float step = CArmConfigHandler::Deserialize().m_collimatorMotionH.m_step;
        float speed = CArmConfigHandler::Deserialize().m_collimatorMotionH.m_velocity;
        emit collimatorMotionSignal(CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_H, m_collimatorPosH + step, speed);

    }// 按下（不是长按）后松开
     //计算帧数
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
}

// 限束器垂直向外调节按钮松开
void CArmHardwareControl::collimatorMotionVWRelease(void) 
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > CArmConfigHandler::Deserialize().m_collimatorMotionV.m_timeThreshold)
    {
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_topBottom = CArmManager::scanConfigConvert()->m_fov.m_topBottom + config.m_collimatorMotionV.m_velocity;

        emit collimatorMotionSignal(OptionCollimatorMoveDirection::STOP, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_V, 0.0);
    }// 长按超过200ms后松开，发送停止运动命令
    else
    {
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_topBottom = CArmManager::scanConfigConvert()->m_fov.m_topBottom + config.m_collimatorMotionV.m_step;

        m_timerMotionVW.stop();
        // 发送垂直限束器的位置及速度
        float step = CArmConfigHandler::Deserialize().m_collimatorMotionV.m_step;
        float speed = CArmConfigHandler::Deserialize().m_collimatorMotionV.m_velocity;
        emit collimatorMotionSignal(CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_V, m_collimatorPosV - step, speed);

    }// 按下（不是长按）后松开

     //计算帧数
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
}

// 限束器垂直向内调节按钮松开
void CArmHardwareControl::collimatorMotionVNRelease(void) 
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    if (m_time.elapsed() > CArmConfigHandler::Deserialize().m_collimatorMotionV.m_timeThreshold)
    {
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_topBottom = CArmManager::scanConfigConvert()->m_fov.m_topBottom - config.m_collimatorMotionV.m_velocity;

        emit collimatorMotionSignal(OptionCollimatorMoveDirection::STOP, CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_V, 0.0);
    }// 长按超过200ms后松开，发送停止运动命令
    else
    {
        //计算FOV
        CArmManager::scanConfigConvert()->m_fov.m_topBottom = CArmManager::scanConfigConvert()->m_fov.m_topBottom - config.m_collimatorMotionV.m_step;

        m_timerMotionVN.stop();
        // 发送垂直限束器的位置及速度
        float step = CArmConfigHandler::Deserialize().m_collimatorMotionV.m_step;
        float speed = CArmConfigHandler::Deserialize().m_collimatorMotionV.m_velocity;
        emit collimatorMotionSignal(CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_V, m_collimatorPosV + step, speed);

    }// 按下（不是长按）后松开

     //计算帧数
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
}

void CArmHardwareControl::updateCollimatorPosH(float pos)const
{
    m_collimatorPosH = pos;
}

void CArmHardwareControl::updateCollimatorPosV(float pos)const
{
    m_collimatorPosV = pos;
}

void CArmHardwareControl::setAlarmLight(bool status)
{
    ui.warnBtn->setChecked(status);
}