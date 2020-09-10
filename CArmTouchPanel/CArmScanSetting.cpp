#include "CArmScanSetting.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <QApplication>
#include <QRect>
#include <QDesktopWidget>
#include "CommonUtility.h"

#pragma execution_character_set("utf-8")

CArmScanSetting::CArmScanSetting(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    init();
}

CArmScanSetting::~CArmScanSetting()
{
}

void CArmScanSetting::init(void)
{
    m_pCaptureModeWidget = new CArmCaptureMode;
    m_pKVp_mAWidget = new CArmKVp_mA;
    m_pHardWareControlWidget = new CArmHardwareControl;

    // 拍摄模式选择
    {
        QGridLayout *layout = new QGridLayout;
        layout->addWidget(m_pCaptureModeWidget);
        layout->setMargin(0);
        ui.captureModeWidget->setLayout(layout);
    }
    // 电流电压调节
    {
        QGridLayout *layout = new QGridLayout;
        layout->addWidget(m_pKVp_mAWidget);
        layout->setMargin(0);
        ui.KVp_mAWidget->setLayout(layout);
    }
    // 机械臂调节
    {
        QGridLayout *layout = new QGridLayout;
        layout->addWidget(m_pHardWareControlWidget);
        layout->setMargin(0);
        ui.hardwareControlWidget->setLayout(layout);
    }

#if 0
    {

        CArmConfig config = CArmConfigHandler::Deserialize();

        config.m_defaultScanPart = ScanPart::LIMBS;

        ImageParam  imageParam;
        imageParam.m_brightness = 0.0;
        imageParam.m_contrast = 0.0;
        imageParam.m_denoise = 2.0;
        imageParam.m_denoiseLevel = 1.0;
        imageParam.m_metalCalib = 0.0;
        imageParam.m_negative = false;
        imageParam.m_reversalH = false;
        imageParam.m_reversalV = false;
        imageParam.m_rotate = 0.0;
        imageParam.m_sharpen = 6.0;
        imageParam.m_sharpenLevel = 1.0;
        imageParam.m_smartMetalCorrection = false;

        CArmFOV fov;
        fov.m_leftRight = 10.0;
        fov.m_topBottom = 10.0;

        for (int i = 0; i < 9; i++)
        {
            config.m_scanPartArray[i].m_imageParam = imageParam;
            if (i < 4)
            {
                config.m_scanPartArray[i].m_imageParam.m_metalCalib = 2;
            }
            else
            {
                config.m_scanPartArray[i].m_imageParam.m_metalCalib = 0;
            }

            config.m_scanPartArray[i].m_fov = fov;
            
            config.m_scanPartArray[i].m_defaultScanMode = ScanMode::FLUORO_MPP;
            // 0点拍---0常规剂量
            config.m_scanPartArray[i].m_scanModeArray[0].m_defaultDoseType = DoseType::REGULAR;
            config.m_scanPartArray[i].m_scanModeArray[0].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[0].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[0].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[0].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[0].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[0].m_mA = 10.0;
            // 0点拍---1增强剂量
            config.m_scanPartArray[i].m_scanModeArray[0].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[1].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[1].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[1].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[1].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[1].m_mA = 10.0;
            // 0点拍---2低剂量
            config.m_scanPartArray[i].m_scanModeArray[0].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[2].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[2].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[2].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[2].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[0].m_scanParamArray[2].m_mA = 5.0;
            // 1透视---0常规剂量
            config.m_scanPartArray[i].m_scanModeArray[1].m_defaultDoseType = DoseType::REGULAR;
            config.m_scanPartArray[i].m_scanModeArray[1].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[0].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[0].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[0].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[0].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[0].m_mA = 6.0;
            // 1透视---1增强常规剂量
            config.m_scanPartArray[i].m_scanModeArray[1].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[1].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[1].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[1].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[1].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[1].m_mA = 12.0;
            // 1透视---2低剂量
            config.m_scanPartArray[i].m_scanModeArray[1].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[2].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[2].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[2].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[2].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[1].m_scanParamArray[2].m_mA = 3.0;
            // 2视频---0常规剂量
            config.m_scanPartArray[i].m_scanModeArray[2].m_defaultDoseType = DoseType::REGULAR;
            config.m_scanPartArray[i].m_scanModeArray[2].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[0].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[0].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[0].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[0].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[0].m_mA = 6.0;
            // 2视频---1增强常规剂量
            config.m_scanPartArray[i].m_scanModeArray[2].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[1].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[1].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[1].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[1].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[1].m_mA = 12.0;
            // 2视频---2低剂量
            config.m_scanPartArray[i].m_scanModeArray[2].m_pps = 25;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[2].m_exposureTime = 20;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[2].m_fps = 25;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[2].m_frameCount = 100;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[2].m_kVp = 60.0;
            config.m_scanPartArray[i].m_scanModeArray[2].m_scanParamArray[2].m_mA = 3.0;
        }

        CArmConfigHandler::Serialize(config);

    }
#endif

    // 调节电流电压
    connect(m_pCaptureModeWidget, &CArmCaptureMode::kVmASettingSignal, [this](const CArmLastScanConfig & config) {kVmASetting(config); });

    connect(m_pCaptureModeWidget, &CArmCaptureMode::autoSignal, [this](const CArmLastScanConfig & config) {autoSetting(config); });

    // 发送信号告诉框架更新底部病人信息
    connect(m_pCaptureModeWidget, &CArmCaptureMode::scanInfoUpdateSignal, this, &CArmScanSetting::scanInfoUpdateSignal);
    connect(m_pKVp_mAWidget, &CArmKVp_mA::scanInfoUpdateSignal, this, &CArmScanSetting::scanInfoUpdateSignal);

    // 激光灯状态信号传递
    connect(m_pHardWareControlWidget, &CArmHardwareControl::laserLampStatusSignal, this, &CArmScanSetting::laserLampStatusSignal);

    // C型臂调节（长按按钮）
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(OptionLiftColMove, float)>(&CArmHardwareControl::CArmUpDownSignal), this,
        static_cast<void(CArmScanSetting::*)(OptionLiftColMove, float)>(&CArmScanSetting::CArmUpDownSignal));
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(OptionXAxisMove, float)>(&CArmHardwareControl::CArmFrontBackSignal), this,
        static_cast<void(CArmScanSetting::*)(OptionXAxisMove, float)>(&CArmScanSetting::CArmFrontBackSignal));
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(OptionCArmMove, float)>(&CArmHardwareControl::CArmRotateSignal), this,
        static_cast<void(CArmScanSetting::*)(OptionCArmMove, float)>(&CArmScanSetting::CArmRotateSignal));
    // C型臂调节（非长按按钮）
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(float, float)>(&CArmHardwareControl::CArmUpDownSignal), this,
        static_cast<void(CArmScanSetting::*)(float, float)>(&CArmScanSetting::CArmUpDownSignal));
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(float, float)>(&CArmHardwareControl::CArmFrontBackSignal), this,
        static_cast<void(CArmScanSetting::*)(float, float)>(&CArmScanSetting::CArmFrontBackSignal));
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(float, float)>(&CArmHardwareControl::CArmRotateSignal), this,
        static_cast<void(CArmScanSetting::*)(float, float)>(&CArmScanSetting::CArmFrontBackSignal));

    // 限束器调节调节（长按按钮）
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(OptionCollimatorMoveDirection, CArmDeviceMoveTypeDef, float)>(&CArmHardwareControl::collimatorMotionSignal), this,
        static_cast<void(CArmScanSetting::*)(OptionCollimatorMoveDirection, CArmDeviceMoveTypeDef, float)>(&CArmScanSetting::collimatorMotionSignal));

    // 限束器调节（非长按按钮）
    connect(m_pHardWareControlWidget, static_cast<void(CArmHardwareControl::*)(CArmDeviceMoveTypeDef, float, float)>(&CArmHardwareControl::collimatorMotionSignal), this,
        static_cast<void(CArmScanSetting::*)(CArmDeviceMoveTypeDef, float, float)>(&CArmScanSetting::collimatorMotionSignal));

}

void CArmScanSetting::initScanInfo(void)
{
    CArmConfig specialConfig;
    //反序列化（文件->数据）
    CArmManager::fromXML(CArmManager::getSpecialXMLPath(), specialConfig);

    setScanInfo(specialConfig.m_defaultScanPart);
}
bool CArmScanSetting::getIsAuto(void)
{
    return m_pKVp_mAWidget->m_bAuto;
}

// 设置激光灯状态
void CArmScanSetting::setLaserLampStatus(bool status)
{
    m_pHardWareControlWidget->ui.lampBtn->setChecked(status);
}

// 更新前后轴X方向位置
void CArmScanSetting::updateCArmPos_x(float pos_x)
{
    m_pHardWareControlWidget->ui.left_rightLbe->setText(QString::number(pos_x, 'f', 1) + "cm");
}

// 更新升降柱Z方向位置
void CArmScanSetting::updateCArmPos_z(float pos_z)
{
    m_pHardWareControlWidget->ui.up_downLbe->setText(QString::number(pos_z, 'f', 1) + "cm");
}

// 更新C型臂角度
void CArmScanSetting::updateCArmPos_angle(float pos_angle)
{
    m_pHardWareControlWidget->ui.angleLbe->setText(QString::number(pos_angle) + "°");
}

// 设置电流电压
void CArmScanSetting::kVmASetting(const CArmLastScanConfig &scanConfig)
{
    m_pKVp_mAWidget->setkV_mA(scanConfig);
}
// 自动手动设置
void CArmScanSetting::autoSetting(const CArmLastScanConfig & config)
{
    m_pKVp_mAWidget->setAuto(config);
}

// 获取身体部位窗口状态
bool CArmScanSetting::scanPartDialogIsshow()
{
    return m_pCaptureModeWidget->scanPartDialogIsshow();
}
// 隐藏身体部位选择窗口
void CArmScanSetting::hideScanPartDialog(void)
{
    m_pCaptureModeWidget->hideScanPartDialog();
}

// 获取脉冲频率选择窗口状态
bool CArmScanSetting::scanPpsDialogIsshow()
{
    return m_pCaptureModeWidget->scanPpsDialogIsshow();
}

// 隐藏脉冲频率选择窗口
void CArmScanSetting::hidePpsDialog(void)
{
    m_pCaptureModeWidget->hidePpsDialog();
}


void CArmScanSetting::scanPartUpdate(const ScanPart& scanPart)
{
    m_pCaptureModeWidget->scanPartUpdate(scanPart);
}

// 更新水平限束器的位置
void CArmScanSetting::updateCollimatorPosH(float pos)
{
    m_pHardWareControlWidget->updateCollimatorPosH(pos);
}
// 更新水平限束器的位置 
void CArmScanSetting::updateCollimatorPosV(float pos)
{
    m_pHardWareControlWidget->updateCollimatorPosV(pos);
}

void CArmScanSetting::setScanInfo(const ScanPart & scanPart)
{
    // 设置扫描部位，扫描模式，脉冲频率,剂量类型，电压电流
    m_pCaptureModeWidget->scanPartUpdate(scanPart);

}

void CArmScanSetting::setAlarmLight(bool status)
{
    m_pHardWareControlWidget->setAlarmLight(status);
}

