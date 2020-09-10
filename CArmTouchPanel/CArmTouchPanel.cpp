#include <WinSock2.h>
#include "qapplication.h"
#include "qdesktopwidget.h"
#include "CArmTouchPanel.h"
#include "WorkStationComm.h"
#include "../Common/Include/CArmScCmdDef.h"
#include "CArmManager.h"

#define GEOMETRY                300
#define GEOMETRY_X              200
#define GEOMETRY_Y              80
#define HISTORY_WIDGET          218
#define HISTORY_CHECK_WIDGET    48

constexpr int ALARMTIME = 5;

CArmTouchPanel::CArmTouchPanel(QWidget *parent)
    : QWidget(parent)
    , m_pCurrentTabLabel(nullptr)
    , m_pScanDialog(nullptr)
    , m_pGArmCheckDialog(nullptr)
    , m_pRotateDialog(nullptr)
    , m_pImageOperationTab(nullptr)
    , m_pImageTextureImporter(nullptr)
    , m_pCArmImageView(nullptr)
    , m_pHistoryWidget(nullptr)
    , m_pTimeClock(nullptr)
    , m_CurrentShootData(nullptr)
    , m_bScaningStatus(false)
    , m_iCountTime(0)
    , m_workStationComm(new WorkStationComm("tpclient", "127.0.0.1", "6688"))
    , m_imageFilePthStr("Temp/StudyUID/SeriesUID/")
{
    setupUi(this);

    /*创建小C页面用到的对象*/
    creatCArmWidgetObj();

    /*设置窗口风格*/
    setWidgetStyle();

    /*设置窗口风格*/
    styleSetting();

    /*链接槽函数*/
    linkSlotFunc();

    /*客户端链接*/
    m_workStationComm->Connect();

    /*注册回调接口*/
    registerCArmCallBack();

    /*初始化界面显示数据*/
    setContorlInitData();

    /*曝光提示窗口*/
    if (m_pScanDialog == nullptr)
    {
        m_pScanDialog = new CArmDialog(imageWidget);
        m_pScanDialog->move((imageWidget->width() / 2) - (m_pScanDialog->width() / 2), 0);
        m_pScanDialog->setImage(m_styleStruct.scaningFalse, QString("禁止曝光"));
    }




    m_ImageInfoVec.clear();
}

CArmTouchPanel::~CArmTouchPanel()
{
    if (m_pCurrentTabLabel != NULL)
    {
        delete m_pCurrentTabLabel;
        m_pCurrentTabLabel = NULL;
    }

    if (m_pScanDialog != NULL)
    {
        delete m_pScanDialog;
        m_pScanDialog = NULL;
    }

    if (m_pGArmCheckDialog != NULL)
    {
        delete m_pGArmCheckDialog;
        m_pGArmCheckDialog = NULL;
    }
    if (m_pScanSettingWidget != nullptr)
    {
        delete m_pScanSettingWidget;
        m_pScanSettingWidget = nullptr;
    }

    if (m_pRotateDialog != nullptr)
    {
        delete m_pRotateDialog;
        m_pRotateDialog = nullptr;
    }

    if (m_pHistoryWidget != nullptr)
    {
        delete m_pHistoryWidget;
        m_pHistoryWidget = nullptr;
    }
}

void CArmTouchPanel::setContorlInitData()
{

    if (CArmManager::scanConfigConvert() != nullptr)
    {
        m_pImageOperationTab->contrastSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_contrast * 10);
        m_pImageOperationTab->contrastValue->setText(QString("%1%").arg(m_pImageOperationTab->contrastSlider->value()));

        m_pImageOperationTab->brightnessSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_brightness * 10);
        m_pImageOperationTab->brightnessValue->setText(QString("%1%").arg(m_pImageOperationTab->brightnessSlider->value()));

        m_pImageOperationTab->sharpenGear->setText(QString("%1").arg(CArmManager::scanConfigConvert()->m_imageParam.m_sharpenLevel));
        m_pImageOperationTab->sharpenSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_sharpen * 10);
        m_pImageOperationTab->sharpenValue->setText(QString("%1%").arg(m_pImageOperationTab->sharpenSlider->value()));


        m_pImageOperationTab->denoiseGear->setText(QString("%1").arg(CArmManager::scanConfigConvert()->m_imageParam.m_denoiseLevel));
        m_pImageOperationTab->denoiseSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_denoise * 10);
        m_pImageOperationTab->denoiseValue->setText(QString("%1%").arg(m_pImageOperationTab->denoiseSlider->value()));

        m_pImageOperationTab->metalGearOperation((CARM_METAL_GEAR_ENUM)CArmManager::scanConfigConvert()->m_imageParam.m_metalCalib);
        m_iRecordMetalGear = CArmManager::scanConfigConvert()->m_imageParam.m_metalCalib;



        setCurrentPatientShootData();
    }
}

IPCImgIdTransfer CArmTouchPanel::getTransferInfo(CP_ImageData imageInfo)
{
    CP_InstanceData instanceInfo = CArmCommon::GetInstanceInfo(imageInfo);

    IPCImgIdTransfer transferData;
    memset(&transferData, '\0', sizeof(IPCImgIdTransfer));
    memcmp(transferData.imgID, instanceInfo.imageID.c_str(), 256);
    memcmp(transferData.patientID, instanceInfo.patientID.c_str(), 256);
    memcmp(transferData.seriesID, instanceInfo.seriesID.c_str(), 256);
    memcmp(transferData.studyID, instanceInfo.studyID.c_str(), 256);

    return transferData;
}


void CArmTouchPanel::scanTabInfo()
{
    /*  QPixmap x_ray(m_styleStruct.x_ray);
      x_ray = x_ray.scaled(pulseLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      pulseLabel->setPixmap(x_ray);*/

    pulse->setStyleSheet(m_styleStruct.pulse);
    pulse->setText(tr("Fluoro"));

    pulseValue->setStyleSheet(m_styleStruct.kvpLabel);
    //  pulseValue->setText(tr("2PPS"));

      //QPixmap modeMap(m_styleStruct.highQuality);
      //modeMap = modeMap.scaled(modeLabl->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      //modeLabl->setPixmap(modeMap);

      //QPixmap posMap(m_styleStruct.armAndLegs);
      //posMap = posMap.scaled(posLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
      //posLabel->setPixmap(posMap);

}

void CArmTouchPanel::GArmTabInfo()
{
    QPixmap GRamInfo(m_styleStruct.GArm);
    GRamInfo = GRamInfo.scaled(GRamLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    GRamLabel->setPixmap(GRamInfo);

    GArmText->setStyleSheet(m_styleStruct.kvpLabel);
    GArmText->setText(tr("仿G臂"));

    GArmValue->setStyleSheet(m_styleStruct.kvpLabel);
    GArmValue->setText(tr("-/-°-/-°"));

}

void CArmTouchPanel::linkSlotFunc()
{
    /*曝光页面*/
    connect(ScanDataTab, SIGNAL(mouseClicked()), this, SLOT(scanTabClicked_slot()));

    /*图像操作页面*/
    connect(CArmImageOperTab, SIGNAL(mouseClicked()), this, SLOT(imageOperaterTabClicked_slot()));

    /*仿G臂操作页面*/
    connect(GArmTab, SIGNAL(mouseClicked()), this, SLOT(GArmTabClicked_slot()));

    /*传输图像按钮*/
    connect(CArmtransferImageBut, SIGNAL(clicked()), this, SLOT(transferButtonClicked_slot()));

    /*金属校正*/
    connect(m_pImageOperationTab, SIGNAL(metaRevise_sig()), this, SLOT(brainpowerMetalAdjusting_slot()));

    /*负片操作*/
    connect(m_pImageOperationTab, SIGNAL(negative_sig()), this, SLOT(negativeFilm_slot()));

    /*旋转操作*/
    connect(m_pImageOperationTab, SIGNAL(rotate_sig(bool)), this, SLOT(rotate_slot(bool)));

    /*水平翻转*/
    connect(m_pImageOperationTab, SIGNAL(level_sig()), this, SLOT(flipHorizontal_slot()));

    /*垂直翻转*/
    connect(m_pImageOperationTab, SIGNAL(vertical_sig()), this, SLOT(flipVertical_slot()));

    /*重置所有*/
    connect(m_pImageOperationTab, SIGNAL(reset_sig()), this, SLOT(resetImage_slot()));

    /*对比度调节*/
    connect(m_pImageOperationTab->contrastSlider, SIGNAL(valueChanged(int)), this, SLOT(contrastDataChanged_slot(int)));

    /*亮度调节*/
    connect(m_pImageOperationTab->brightnessSlider, SIGNAL(valueChanged(int)), this, SLOT(brightnessDataChanged_slot(int)));

    /*锐度调节*/
    connect(m_pImageOperationTab->sharpenSlider, SIGNAL(valueChanged(int)), this, SLOT(sharpenDataChanged_slot(int)));

    /*降噪*/
    connect(m_pImageOperationTab->denoiseSlider, SIGNAL(valueChanged(int)), this, SLOT(denoiseDataChanged_slot(int)));

    /*金属调节*/
    connect(m_pImageOperationTab, SIGNAL(metalGear_sig(CARM_METAL_GEAR_ENUM)), this, SLOT(metalAdjustingDataChanged_slot(CARM_METAL_GEAR_ENUM)));

    connect(this, SIGNAL(startScaning(bool)), this, SLOT(scanStateShoe_slot(bool)));

    /*注册结构体*/
    qRegisterMetaType<CP_ReviewData*>("CP_ReviewData*");

    connect(this, SIGNAL(sendImageData(CP_ReviewData*)), this, SLOT(CArmViewLoadImageData_slot(CP_ReviewData*)));
    // 未连接工作站时消息传递
    connect(this, &CArmTouchPanel::splashDialogShowSignal, this, &CArmTouchPanel::splashDilogShow);
    connect(this, &CArmTouchPanel::splashDialogCloseSignal, this, &CArmTouchPanel::splashDilogClose);

    // 右屏登录和登出消息传递
    connect(this, &CArmTouchPanel::loginSignal, this, &CArmTouchPanel::loginSlot);
    connect(this, &CArmTouchPanel::logoutSignal, this, &CArmTouchPanel::logoutSlot);
    // 右屏选择患者信息可以选择拍摄
    connect(this, &CArmTouchPanel::scanReadySignal, this, &CArmTouchPanel::scanReadySlot);


    // 更新底部病人信息(上一次配置)及发送消息给工作站
    connect(m_pScanSettingWidget, &CArmScanSetting::scanInfoUpdateSignal, [this](const CArmLastScanConfig& scanConfig) {updateInfo(scanConfig); });

    // 设置激光灯状态
    connect(m_pScanSettingWidget, &CArmScanSetting::laserLampStatusSignal, [this](bool status) {laserLampStatusSetting(status); });

    // C型臂调节（长按按钮）
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(OptionLiftColMove, float)>(&CArmScanSetting::CArmUpDownSignal), this,
        static_cast<void(CArmTouchPanel::*)(OptionLiftColMove, float)>(&CArmTouchPanel::CArmUpDown));
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(OptionXAxisMove, float)>(&CArmScanSetting::CArmFrontBackSignal), this,
        static_cast<void(CArmTouchPanel::*)(OptionXAxisMove, float)>(&CArmTouchPanel::CArmFrontBack));
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(OptionCArmMove, float)>(&CArmScanSetting::CArmRotateSignal), this,
        static_cast<void(CArmTouchPanel::*)(OptionCArmMove, float)>(&CArmTouchPanel::CArmRotate));
    // C型臂调节（非长按按钮）
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(float, float)>(&CArmScanSetting::CArmUpDownSignal), this,
        static_cast<void(CArmTouchPanel::*)(float, float)>(&CArmTouchPanel::CArmUpDown));
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(float, float)>(&CArmScanSetting::CArmFrontBackSignal), this,
        static_cast<void(CArmTouchPanel::*)(float, float)>(&CArmTouchPanel::CArmFrontBack));
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(float, float)>(&CArmScanSetting::CArmRotateSignal), this,
        static_cast<void(CArmTouchPanel::*)(float, float)>(&CArmTouchPanel::CArmRotate));

    // 限束器调节（长按按钮）
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(OptionCollimatorMoveDirection, CArmDeviceMoveTypeDef, float)>(&CArmScanSetting::collimatorMotionSignal), this, static_cast<void(CArmTouchPanel::*)(OptionCollimatorMoveDirection, CArmDeviceMoveTypeDef, float)>(&CArmTouchPanel::collimatorMotion));

    // 限束器调节（非长按按钮）
    connect(m_pScanSettingWidget, static_cast<void(CArmScanSetting::*)(CArmDeviceMoveTypeDef, float, float)>(&CArmScanSetting::collimatorMotionSignal), this, static_cast<void(CArmTouchPanel::*)(CArmDeviceMoveTypeDef, float, float)>(&CArmTouchPanel::collimatorMotion));

    // 曝光时间计算
    connect(&m_alarmTimer, &QTimer::timeout, this, &CArmTouchPanel::expTimeUpdateSlot);
    // 曝光时间实时显示
    connect(m_timer, SIGNAL(timeout()), this, SLOT(expTimeDisplaySlot()));

    connect(this, SIGNAL(alarmLightSignal()), this, SLOT(alarmLight()));
    connect(this, SIGNAL(flouroTimeSignal()), this, SLOT(calcFlouroTime()));
    
    // 初始化扫描信息
    m_pScanSettingWidget->initScanInfo();

}


void CArmTouchPanel::creatCArmWidgetObj()
{
    if (m_pImageTextureImporter == nullptr)
    {
        m_pImageTextureImporter = ViewToolApi::createImageTextureImporter(IImageTextureImporter::TEXTURE_TYPE::TT_DEFAULT);
    }

    if (m_pCurrentTabLabel == nullptr)
    {
        m_pCurrentTabLabel = new QLabel(TabWidget);
        m_pCurrentTabLabel->setFixedSize(42, 4);
        m_pCurrentTabLabel->setStyleSheet(m_styleStruct.currentTab);
        m_pCurrentTabLabel->setGeometry(widget_2->x() + 12, widget_2->y() + widget_2->height() + 26, 42, 4);

    }

    if (m_pImageOperationTab == nullptr)
    {
        m_pImageOperationTab = new CArmImageTab();
        if (m_pImageOperationTab != nullptr)
        {
            QGridLayout *layout = new QGridLayout;
            layout->addWidget(m_pImageOperationTab);
            layout->setSpacing(0);
            layout->setMargin(0);
            imagePage->setLayout(layout);
        }
    }

    if (m_pCArmImageView == nullptr)
    {
        m_pCArmImageView = new CArmView(CARM_TOUCH_PANEL, this);
        if (m_pCArmImageView != nullptr)
        {
            QGridLayout *viewLayout = new QGridLayout();
            m_pCArmImageView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            viewLayout->addWidget(m_pCArmImageView);
            viewLayout->setSpacing(0);
            viewLayout->setMargin(0);
            imageWidget->setLayout(viewLayout);
        }
    }
    m_pTextureImportor =ViewToolApi::createImage2DTextureImporter(m_pCArmImageView);

    if (m_pRotateDialog == nullptr)
    {
        m_pRotateDialog = new CArmRotateDialog(imageWidget);

    }

    if (m_pHistoryWidget == nullptr)
    {
        m_pHistoryWidget = new CArmHistoryWidget(imageWidget);

        if (m_pHistoryWidget != nullptr)
        {
            connect(m_pHistoryWidget, SIGNAL(sendCheckButtonStatus(bool)), this, SLOT(showHistoryList_slot(bool)));
            connect(m_pHistoryWidget, SIGNAL(sendChangedImageIndex(int)), this, SLOT(showCurrentIndexImage_slot(int)));
            m_pHistoryWidget->move(imageWidget->width() - HISTORY_CHECK_WIDGET, 0);
        }
    }
}

void CArmTouchPanel::registerCArmCallBack()
{
    m_workStationComm->OnCmdRecv = std::bind(&CArmTouchPanel::workStationData, this, std::placeholders::_1);
}

// 激光灯状态设置
void CArmTouchPanel::laserLampStatusSetting(bool lampStatus)
{
    CArmCmdLaserStatusChangedCross laserStatusChangedCross({});
    CArmCmdLaserStatusChangedView laserStatusChangedView({});
    if (lampStatus)
    {
        laserStatusChangedCross.m_arg = CArmOnOffDef::On;
        laserStatusChangedView.m_arg = CArmOnOffDef::On;
    }
    else
    {
        laserStatusChangedCross.m_arg = CArmOnOffDef::Off;
        laserStatusChangedView.m_arg = CArmOnOffDef::Off;
    }
    {
        CArmCmdBase* cmd = &laserStatusChangedCross;
        m_workStationComm->Request(cmd);
    }
    {
        CArmCmdBase* cmd = &laserStatusChangedView;
        m_workStationComm->Request(cmd);
    }
}

// C型臂升降柱上升/下降，Z轴(长按)
void CArmTouchPanel::CArmUpDown(OptionLiftColMove opt, float speed)
{
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = CArmDeviceMoveTypeDef::SET_Z_AXIS_CONTINUE_MOVE;
    request.m_arg.direction = (int)opt;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}
// C型臂前后轴向前/向后，X轴(长按)
void CArmTouchPanel::CArmFrontBack(OptionXAxisMove opt, float speed)
{
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = CArmDeviceMoveTypeDef::SET_X_AXIS_CONTINUE_MOVE;
    request.m_arg.direction = (int)opt;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}
// C型臂角度调节，Y轴（顺时针/逆时针）(长按)
void CArmTouchPanel::CArmRotate(OptionCArmMove opt, float speed)
{
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = CArmDeviceMoveTypeDef::SET_Y_AXIS_CONTINUE_MOVE;
    request.m_arg.direction = (int)opt;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}

// C型臂升降柱上升/下降，Z轴(非长按)
void CArmTouchPanel::CArmUpDown(float pos, float speed)
{
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = CArmDeviceMoveTypeDef::SET_Z_AXIS_POS;
    request.m_arg.pos = pos;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}

// C型臂前后轴向前/向后，X轴(非长按)
void CArmTouchPanel::CArmFrontBack(float pos, float speed)
{
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = CArmDeviceMoveTypeDef::SET_X_AXIS_POS;
    request.m_arg.pos = pos;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}

// C型臂角度调节，Y轴（顺时针/逆时针）(非长按)
void CArmTouchPanel::CArmRotate(float pos, float speed)
{
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = CArmDeviceMoveTypeDef::SET_Y_AXIS_POS;
    request.m_arg.pos = pos;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}

// 限束器水平调节(长按)
void CArmTouchPanel::collimatorMotion(OptionCollimatorMoveDirection option, CArmDeviceMoveTypeDef pos, float speed)
{
    
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = pos;
    request.m_arg.direction = (int)option;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}

// 限束器水平调节（非长按）
void CArmTouchPanel::collimatorMotion(CArmDeviceMoveTypeDef opt, float pos, float speed)
{
    CArmCmdDeviceMoveCmd request({});
    request.m_arg.m_type = opt;
    request.m_arg.pos = pos;
    request.m_arg.velocity = speed;

    CArmCmdBase* cmd = &request;
    m_workStationComm->Request(cmd);
}


void CArmTouchPanel::sendData2WorkStation(const CArmLastScanConfig& scanfig)
{
    CArmCmdScanParam scanParm({});
    scanParm.m_arg = scanfig;

    CArmCmdBase* cmd = &scanParm;
    m_workStationComm->Request(cmd);
}

// 有数据更新时执行
void CArmTouchPanel::workStationData(const CArmCmdBase* cmdBase)
{

    // 检查触摸屏与工作站是否连接
    netWorkStatus(cmdBase);

    // 影像更新
    updateImage(cmdBase);

    /*曝光开始*/
    if (cmdBase->m_u16Cmd == SC_CARM_EXP_BTN1_PRESSED)
    {
        auto startScan = static_cast<const CArmCmdExpBtn1Pressed *>(cmdBase);
        if (startScan->m_arg == CArmBtnStatus::Pressed)
        {
            scanStatus(true);
        }
    }
    else if (cmdBase->m_u16Cmd == WS_CARM_IPC_WS_STATUS)/*曝光结束*/
    {
        const CArmCmdWorkStationStatus *scanOver = static_cast<const CArmCmdWorkStationStatus *>(cmdBase);
        if (scanOver->m_arg == CArmWorkStationStatusTypeDef::SCAN_FINISH)
        {
            scanStatus(false);
        }
        
    }

    // 有患者信息录入，扫描信息更新
    updateScanInfo(cmdBase);

   // 扫描状态（登录登出扫描结束）
    scanStatus(cmdBase);

    // 十字线激光灯状态更新
    updateLaserLamp(cmdBase);

    // 曝光时间更新
    updateFluoroTime(cmdBase);

    // 更新升降柱Z方向位置
    updateCArmPos_Z(cmdBase);

    // 更新前后轴X方向位置
    updateCArmPos_X(cmdBase);

    // 更新C型臂角度
    updateCArmAngle(cmdBase);

    // 更新水平限束器的位置
    updateCollimatorH(cmdBase);

    // 更新垂直限束器的位置
    updateCollimatorV(cmdBase);

    // 通过DAP获取当前study已经用掉的剂量
    updateDoseUsedByDAP(cmdBase);

    // 曝光按钮按下/松开（二级手闸）
    exposeBtnStatus(cmdBase);

    
}

void CArmTouchPanel::scanStatus(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_IPC_WS_STATUS)
    {
        auto workStationStatus = static_cast<const CArmCmdWorkStationStatus*>(cmdBase);
        if (workStationStatus->m_arg == CArmWorkStationStatusTypeDef::USER_LOGIN)
        {
            // 右屏登录
            emit loginSignal();
            
        }
        if (workStationStatus->m_arg == CArmWorkStationStatusTypeDef::USER_LOGOUT)
        {
            // 右屏登出
            emit logoutSignal();
        }
        if (workStationStatus->m_arg == CArmWorkStationStatusTypeDef::SCAN_FINISH)
        {
            // 曝光时间停止
            m_alarmTimer.stop();
            // 时间戳停止
            m_timer->stop();
        }      
    }    
}
// 更新已经用掉的剂量
void CArmTouchPanel::updateDoseUsedByDAP(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)
    {
        auto DAPinfo = static_cast<const CArmCmdDeviceValue *>(cmdBase);
        if (DAPinfo->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_DAP_VALUE)
        {
           CArmManager::m_doseUsed =  DAPinfo->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_DAP_VALUE>().DAP;
            
        }
    }
}
// 曝光按钮按下
void CArmTouchPanel::exposeBtnStatus(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == SC_CARM_EXP_BTN2_PRESSED)
    {
        auto pressed = static_cast<const CArmCmdExpBtn2Pressed*>(cmdBase);
        if (pressed->m_arg == CArmBtnStatus::Pressed)
        {
            // 警告灯
            emit alarmLightSignal();
            // 曝光时间
            emit flouroTimeSignal();
        }
      
    }
}
//报警功能
void CArmTouchPanel::alarmLight()
{
    m_alarmTime.start();
    m_alarmTimer.start(100);
}

//计算曝光时间
void CArmTouchPanel::calcFlouroTime()
{
    // 清空上次的时间戳
    m_expTime->setHMS(0, 0, 0);
    m_timer->start(1000);
}

void CArmTouchPanel::expTimeDisplaySlot()
{
    *m_expTime = m_expTime->addMSecs(1000);
    timeValue->setText(m_expTime->toString("mm:ss"));
}

// 报警
void CArmTouchPanel::expTimeUpdateSlot()
{
    CArmConfig config = CArmConfigHandler::Deserialize();
    int videoTimeLimit = config.m_videoModeTimeLimit;
    bool bIsUseWarning = config.m_movieCountDownTip;
    if (bIsUseWarning)
    {
        if (m_pScanSettingWidget->isVideo())
        {
            int time = CArmManager::remainingFrameCount() / CArmManager::scanConfigConvert()->m_pps;

            if (time == videoTimeLimit) 
            {
                if (m_alarmTime.elapsed() > (videoTimeLimit - ALARMTIME) * 1000)
                {
                    m_pScanSettingWidget->setAlarmLight(true);
                }
                if (m_alarmTime.elapsed() > videoTimeLimit * 1000)
                {
                    m_pScanSettingWidget->setAlarmLight(false);

                    m_alarmTimer.stop();
                }
            }//当前剂量能够拍摄
            else
            {
                if (m_alarmTime.elapsed() > (time - ALARMTIME) * 1000)
                {
                    m_pScanSettingWidget->setAlarmLight(true);
                }
                if (m_alarmTime.elapsed() > time * 1000)
                {
                    m_pScanSettingWidget->setAlarmLight(false);

                    m_alarmTimer.stop();
                }
            }//当前剂量不够拍摄
        }
    }

}


void CArmTouchPanel::updateImage(const CArmCmdBase* cmdBase)
{
    /*一帧影像数据*/
    if (cmdBase->m_u16Cmd != WS_CARM_NEW_IMG_RECEIVED)
    {
        return;
    }
    else
    {
        const NewImgReceived* imageData = static_cast<const NewImgReceived *>(cmdBase);
        NewImgReceived* newImageData = new NewImgReceived{ imageData ->m_imgId, imageData->m_idx, imageData->m_w, imageData->m_h, imageData->m_img, imageData->m_windowWidth, imageData->m_windowCenter};

        loadImageTexture(newImageData);
    }
}

// 激光灯状态
void CArmTouchPanel::updateLaserLamp(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_CROSS || cmdBase->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_VIEW)
    {
        auto laserLamp = static_cast<const CArmCmdLaserStatusChangedCross *>(cmdBase);

        if (laserLamp->m_arg == CArmOnOffDef::Off)
        {
            m_pScanSettingWidget->setLaserLampStatus(false);
        }
        if (laserLamp->m_arg == CArmOnOffDef::On)
        {
            m_pScanSettingWidget->setLaserLampStatus(true);
        }

    }
}


// 网络连接状态
void CArmTouchPanel::netWorkStatus(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_TP_WS_NET_STATUS)
    {
        auto connectStatus = static_cast<const CArmCmdTpWsNetStatus*>(cmdBase);
        if (connectStatus->m_arg == CArmNetStatus::Disconnected)
        {
            emit splashDialogShowSignal();

        }
        if (connectStatus->m_arg == CArmNetStatus::Connected)
        {
            emit splashDialogCloseSignal();
        }
    }

}

void CArmTouchPanel::updateScanInfo(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_NEW_PATIENT_INFO)
    {
        auto patientInfo = static_cast<const CArmCmdNewPatientInfo *>(cmdBase);

        // 选择患者信息后才可以拍摄
        emit scanReadySignal();

        setDataInfoToUi(patientInfo);
        // 切换扫描模式界面刷新
        m_pScanSettingWidget->scanPartUpdate(patientInfo->m_arg.scanPart);

        /*设置当前患者拍摄图像初始值*/
        setCurrentPatientShootData();

    }
}

void CArmTouchPanel::updateFluoroTime(const CArmCmdBase* cmdBase)
{
    // 曝光按钮按下
    if (cmdBase->m_u16Cmd == SC_CARM_PEDAL_PRESSED)
    {
        auto patientInfo = static_cast<const CArmCmdPedalPressed *>(cmdBase);
        // 计算曝光时间
    }
}

// 更新升降柱Z方向位置
void CArmTouchPanel::updateCArmPos_Z(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)
    {
        auto pos = static_cast<const CArmCmdDeviceValue *>(cmdBase);
        if (pos->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_Z_AXIS_POS)
        {
            m_pScanSettingWidget->updateCArmPos_z(pos->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_Z_AXIS_POS>());
        }
    }
}

// 更新前后轴X方向位置
void CArmTouchPanel::updateCArmPos_X(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)
    {
        auto pos = static_cast<const CArmCmdDeviceValue *>(cmdBase);
        if (pos->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_X_AXIS_POS)
        {
            m_pScanSettingWidget->updateCArmPos_x(pos->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_X_AXIS_POS>());
        }
    }
}

// 更新C型臂角度
void CArmTouchPanel::updateCArmAngle(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)
    {
        auto pos = static_cast<const CArmCmdDeviceValue *>(cmdBase);
        if (pos->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_Y_AXIS_POS)
        {
            m_pScanSettingWidget->updateCArmPos_angle(pos->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_Y_AXIS_POS>());
        }
    }
}
// 更新水平限束器的位置
void CArmTouchPanel::updateCollimatorH(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)
    {
        auto pos = static_cast<const CArmCmdDeviceValue *>(cmdBase);
        if (pos->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_COLL_H_POS)
        {
            m_pScanSettingWidget->updateCollimatorPosH(pos->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_COLL_H_POS>());
        }
    }
}
// 更新垂直限束器的位置
void CArmTouchPanel::updateCollimatorV(const CArmCmdBase* cmdBase)
{
    if (cmdBase->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)
    {
        auto pos = static_cast<const CArmCmdDeviceValue *>(cmdBase);
        if (pos->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_COLL_V_POS)
        {
            m_pScanSettingWidget->updateCollimatorPosV(pos->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_COLL_V_POS>());
        }
    }
}
void CArmTouchPanel::updateInfo(const CArmLastScanConfig& scanConfig)
{
    // 扫描部位更新
    baseScanPartUpdate(scanConfig.m_scanPart);
    // 扫描模式更新
    baseScanModeUpdate(scanConfig.m_scanModeType);
    // 扫描剂量更新
    baseScanDoseUpdate(scanConfig.m_doseType);
    // 脉冲频率更新
    basePpsUpdate(scanConfig.m_pps);
    // 电压电流更新
    basekVmAUpdate(scanConfig.m_scanParam.m_kVp, scanConfig.m_scanParam.m_mA);

    // 当已连接，已登录，已经新建患者时才把扫描参数发送给工作站
    if (m_bScanInfoFlag)
    {
        sendData2WorkStation(scanConfig);
    }

    // 临时添加，等到贺兴怡改过命令后可删除
    sendData2WorkStation(scanConfig);
}

// 扫描模式更新
void CArmTouchPanel::baseScanModeUpdate(const ScanMode &scanMode)
{
    CARM_STYLE_STRUCT style;
    switch (scanMode)
    {
    case ScanMode::POINT:
        pulseLabel->setPixmap(QPixmap(style.pointCapture));
        break;
    case ScanMode::FLUORO_MPP:
        pulseLabel->setPixmap(QPixmap(style.x_ray));
        break;
    case ScanMode::VIDEO:
        pulseLabel->setPixmap(QPixmap(style.video));
        break;
    default:
        break;
    }
}
// 扫描部位更新
void CArmTouchPanel::baseScanPartUpdate(const ScanPart &scanPart)
{
    CARM_STYLE_STRUCT style;
    switch (scanPart)
    {
    case ScanPart::UNKNOWN:
        // do nothing
        break;
    case ScanPart::LIMBS:
        posLabel->setPixmap(QPixmap(style.armAndLegs));
        break;
    case ScanPart::NECK:
        posLabel->setPixmap(QPixmap(style.neck));
        break;
    case ScanPart::HEAD:
        posLabel->setPixmap(QPixmap(style.head));
        break;
    case ScanPart::TRUNCK:
        posLabel->setPixmap(QPixmap(style.trunk));
        break;
    case ScanPart::HEART:
        posLabel->setPixmap(QPixmap(style.heart));
        break;
    case ScanPart::ABDOMEN:
        posLabel->setPixmap(QPixmap(style.abdomen));
        break;
    case ScanPart::URINARY:
        posLabel->setPixmap(QPixmap(style.urinary));
        break;
    case ScanPart::SOFT_TISSUE:
        posLabel->setPixmap(QPixmap(style.flab));
        break;
    case ScanPart::ENDOSCOPE:
        posLabel->setPixmap(QPixmap(style.endoscope));
        break;
    default:
        break;
    }
}

// 扫描剂量更新
void CArmTouchPanel::baseScanDoseUpdate(const DoseType &doseType)
{
    CARM_STYLE_STRUCT style;
    switch (doseType)
    {
    case DoseType::REGULAR:
        modeLabl->setPixmap(QPixmap(style.normalDose));
        break;
    case DoseType::HIGH:
        modeLabl->setPixmap(QPixmap(style.highQuality));
        break;
    case DoseType::LOW:
        modeLabl->setPixmap(QPixmap(style.lowDose));
        break;

    default:
        break;
    }
}
// 脉冲频率更新
void CArmTouchPanel::basePpsUpdate(int pps)
{
    pulseValue->setText(QString::number(pps) + QString("PPS"));
}

// 电压更新
void CArmTouchPanel::basekVmAUpdate(float kv, float ma)
{
    kvpValue->setText(QString::number(kv, 'f', 1));
    MaValue->setText(QString::number(ma, 'f', 1));
}

void CArmTouchPanel::initBaseScanInfo(void)
{
    //CArmConfig specialConfig;
    ////反序列化（文件->数据）
    //CArmFileManager::fromXML(CArmFileManager::getSpecialXMLPath(), specialConfig);
    //if (specialConfig.m_useDefaultConfig)
    //{
    //    CArmConfig config;
    //    //反序列化（文件->数据）
    //    CArmFileManager::fromXML(CArmFileManager::getDefaultXMLPath(), config);

    //    updateInfo(config.m_specialScanConfig);
    //}
    //else
    //{
    //    updateInfo(specialConfig.m_specialScanConfig);
    //}
}
void CArmTouchPanel::collectImageData()
{
    ImageParam imageParam;
    /*亮度值*/
    imageParam.m_brightness = m_pImageOperationTab->brightnessSlider->value();
    /*对比度*/
    imageParam.m_contrast = m_pImageOperationTab->contrastSlider->value();
    /*锐化值和等级*/
    imageParam.m_sharpen = m_pImageOperationTab->sharpenSlider->value();
    imageParam.m_sharpenLevel = m_pImageOperationTab->sharpenGear->text().toInt();
    /*降噪值和等级*/
    imageParam.m_denoise = m_pImageOperationTab->denoiseSlider->value();
    imageParam.m_denoiseLevel = m_pImageOperationTab->denoiseGear->text().toInt();
    /*金属校正等级*/
    imageParam.m_metalCalib = m_iRecordMetalGear;

    /*是否自动金属校正*/
    imageParam.m_negative = m_pImageOperationTab->getNegativeStatus();

    /*是否垂直*/
    imageParam.m_reversalH = m_pImageOperationTab->getVerticalStatus();

    /*是否水平*/
    imageParam.m_reversalV = m_pImageOperationTab->getLevelStatus();

    /*旋转角度*/
    imageParam.m_rotate = m_pImageOperationTab->ValueLabel->text().toInt();

    auto ImageData = new CArmCmdImgAdjustParam(imageParam);

    bool state = m_workStationComm->Request(ImageData);
    if (state == true)
    {
        imageDataChangedToXML();
    }

    if (ImageData != nullptr)
    {
        delete ImageData;
        ImageData = nullptr;
    }
}

void CArmTouchPanel::imageDataChangedToXML()
{
    if (CArmManager::scanConfigConvert() != nullptr)
    {
        CArmManager::scanConfigConvert()->m_imageParam.m_contrast = m_pImageOperationTab->contrastSlider->value() / 10;
        CArmManager::scanConfigConvert()->m_imageParam.m_brightness = m_pImageOperationTab->brightnessSlider->value() / 10;
        CArmManager::scanConfigConvert()->m_imageParam.m_sharpen = m_pImageOperationTab->sharpenSlider->value() / 10;
        CArmManager::scanConfigConvert()->m_imageParam.m_sharpenLevel = m_pImageOperationTab->sharpenGear->text().toInt();
        CArmManager::scanConfigConvert()->m_imageParam.m_denoise = m_pImageOperationTab->denoiseSlider->value() / 10;
        CArmManager::scanConfigConvert()->m_imageParam.m_denoiseLevel = m_pImageOperationTab->denoiseGear->text().toInt();
        CArmManager::scanConfigConvert()->m_imageParam.m_metalCalib = m_iRecordMetalGear;
        CArmManager::scanConfigConvert()->m_imageParam.m_smartMetalCorrection = m_pImageOperationTab->getMetaReviseStatus();
        CArmManager::scanConfigConvert()->m_imageParam.m_negative = m_pImageOperationTab->getNegativeStatus();
        CArmManager::scanConfigConvert()->m_imageParam.m_rotate = m_pImageOperationTab->ValueLabel->text().toInt();
        CArmManager::scanConfigConvert()->m_imageParam.m_reversalH = m_pImageOperationTab->getLevelStatus();
        CArmManager::scanConfigConvert()->m_imageParam.m_reversalV = m_pImageOperationTab->getVerticalStatus();


    }
}

void CArmTouchPanel::setCurrentPatientShootData()
{
    if (CArmManager::scanConfigConvert() != nullptr)
    {
        m_pImageOperationTab->contrastSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_contrast * 10);
        m_pImageOperationTab->contrastValue->setText(QString("%1%").arg(m_pImageOperationTab->contrastSlider->value()));

        m_pImageOperationTab->brightnessSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_brightness * 10);
        m_pImageOperationTab->brightnessValue->setText(QString("%1%").arg(m_pImageOperationTab->brightnessSlider->value()));

        m_pImageOperationTab->sharpenGear->setText(QString("%1").arg(CArmManager::scanConfigConvert()->m_imageParam.m_sharpenLevel));
        m_pImageOperationTab->sharpenSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_sharpen * 10);
        m_pImageOperationTab->sharpenValue->setText(QString("%1%").arg(m_pImageOperationTab->sharpenSlider->value()));


        m_pImageOperationTab->denoiseGear->setText(QString("%1").arg(CArmManager::scanConfigConvert()->m_imageParam.m_denoiseLevel));
        m_pImageOperationTab->denoiseSlider->setValue(CArmManager::scanConfigConvert()->m_imageParam.m_denoise * 10);
        m_pImageOperationTab->denoiseValue->setText(QString("%1%").arg(m_pImageOperationTab->denoiseSlider->value()));

         m_pImageOperationTab->metalGearOperation((CARM_METAL_GEAR_ENUM)CArmManager::scanConfigConvert()->m_imageParam.m_metalCalib);
        m_iRecordMetalGear = CArmManager::scanConfigConvert()->m_imageParam.m_metalCalib;
    }
}

void CArmTouchPanel::scanTabClicked_slot()
{
    /*进度条*/
    //QProgressBar *progressBar = new QProgressBar();
    //progressBar->setOrientation(Qt::Horizontal);  // 水平方向  
    //progressBar->setMinimum(0);  // 最小值  
    //progressBar->setMaximum(4800);  // 最大值  
    //progressBar->setValue(2000);  // 当前进度  
    //double dProgress = (progressBar->value() - progressBar->minimum()) * 100.0
    //    / (progressBar->maximum() - progressBar->minimum()); // 百分比计算公式  
    //progressBar->setFormat(QString::fromLocal8Bit("当前进度为：%1%").arg(QString::number(dProgress, 'f', 1)));
    //    progressBar->setFormat(tr("Current progress : %1%").arg(QString::number(dProgress, 'f', 1)));
    //    progressBar->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);  // 对齐方式
    //    progressBar->show();


    setCurrentTab(SCAN_TAB);
    m_pCurrentTabLabel->setGeometry(widget_2->x() - 1, widget_2->y() + widget_2->height() + 3, 42, 4);
}

void CArmTouchPanel::imageOperaterTabClicked_slot()
{
    if (m_pScanSettingWidget != nullptr)
    {
        if (m_pScanSettingWidget->getIsAuto())
        {
            m_pImageOperationTab->contrastText->show();
            m_pImageOperationTab->brightnessText->show();
        }
        else
        {
            m_pImageOperationTab->contrastText->hide();
            m_pImageOperationTab->brightnessText->hide();
        }
    }
    setCurrentTab(IMAGE_TAB);
}
void CArmTouchPanel::GArmTabClicked_slot()
{
    setCurrentTab(GARM_TAB);
}

void CArmTouchPanel::transferButtonClicked_slot()
{
    CP_ReviewData currentImage = m_pHistoryWidget->getCurrentImageData();
    IPCImgIdTransfer transferData = getTransferInfo(currentImage._imageData);
    auto transfer = new CArmCmdImgTransfer(transferData);

    /*调用传输图像接口获取传输图像是否成功*/
    transferResulte(m_workStationComm->Request(transfer));

    if (transfer != nullptr)
    {
        delete transfer;
        transfer = nullptr;
    }
}

void CArmTouchPanel::transferResulte(bool state)
{
    //传输结果
    if (state == true)
    {
        CArmTransferDialog *dialog = new CArmTransferDialog(true, tr("传输成功"));
        dialog->move(this->x() + imageWidget->width() + 8, this->y() + imageWidget->height());
        dialog->show();
    }
    else
    {
        CArmTransferDialog *dialog = new CArmTransferDialog(false, tr("传输失败"));
        dialog->move(this->x() + imageWidget->width() + 8, this->y() + imageWidget->height());
        dialog->show();
    }
}

/*对比度调节*/
void CArmTouchPanel::contrastDataChanged_slot(int value)
{
    m_pImageOperationTab->contrastValue->setText(QString("%1%").arg(value));
    m_pCArmImageView->setContrast(value);

    collectImageData();
}

/*亮度调节*/
void CArmTouchPanel::brightnessDataChanged_slot(int value)
{
    m_pImageOperationTab->brightnessValue->setText(QString("%1%").arg(value));
    m_pCArmImageView->setBrightness(value);

    collectImageData();
}

/*金属校正*/
void CArmTouchPanel::metalAdjustingDataChanged_slot(CARM_METAL_GEAR_ENUM gearValue)
{
    m_iRecordMetalGear = (int)gearValue;
    m_pCArmImageView->setMetalGear((int)gearValue, true);

    collectImageData();
}

/*锐化*/
void CArmTouchPanel::sharpenDataChanged_slot(int value)
{
    m_pImageOperationTab->sharpenValue->setText(QString("%1%").arg(value));

    /*获取锐度当前等级*/
    QString gear = m_pImageOperationTab->sharpenGear->text();

    /*将锐度的当前等级和变化值发给着色器，进行图像处理*/
    m_pCArmImageView->setSharpen(gear.toInt(), value);

    collectImageData();
}

/*降噪*/
void CArmTouchPanel::denoiseDataChanged_slot(int value)
{
    m_pImageOperationTab->denoiseValue->setText(QString("%1%").arg(value));

    QString gear = m_pImageOperationTab->denoiseGear->text();
    m_pCArmImageView->setDenoise(gear.toInt(), value);

    collectImageData();
}

/*智能金属校正*/
void CArmTouchPanel::brainpowerMetalAdjusting_slot()
{
    if (m_pCArmImageView != nullptr)
    {
        m_pCArmImageView->capacityMetal(4, true);
    }
    collectImageData();
}

/*负片*/
void CArmTouchPanel::negativeFilm_slot()
{
    if (m_pCArmImageView != nullptr)
        m_pCArmImageView->setNegativeValue(true);

    collectImageData();
}

/*旋转*/
void CArmTouchPanel::rotate_slot(bool status)
{
    if (m_pRotateDialog != nullptr)
    {
        if (status == true)
        {
            connect(m_pRotateDialog, SIGNAL(sendCurrentAngle_sig(int)), this, SLOT(setRotateAngle_slot(int)));
            m_pRotateDialog->move(imageWidget->x(), imageWidget->height() - m_pRotateDialog->height());
            m_pRotateDialog->show();
        }
        else
        {
            m_pRotateDialog->close();
        }
    }
}

void CArmTouchPanel::setRotateAngle_slot(int angle)
{
    /*将改变的值显示在界面上*/
    if (m_pImageOperationTab != nullptr)
        m_pImageOperationTab->ValueLabel->setText(QString("%1°").arg(angle));

    /*将旋转的角度设置给画刷*/
    if (m_pCArmImageView != nullptr)
        m_pCArmImageView->setRotateValue(angle);

    collectImageData();
}

/*水平翻转*/
void CArmTouchPanel::flipHorizontal_slot()
{
    if (m_pCArmImageView != nullptr)
    {
        m_pCArmImageView->flipViewHorizontal(true);
    }
    collectImageData();
}

/*垂直翻转*/
void CArmTouchPanel::flipVertical_slot()
{
    if (m_pCArmImageView != nullptr)
    {
        m_pCArmImageView->flipViewVertical(true);
    }
    collectImageData();
}

/*重置图像*/
void CArmTouchPanel::resetImage_slot()
{
    if (m_pCArmImageView != nullptr)
    {
        CP_ReviewData currentImage = m_pHistoryWidget->getCurrentImageData();

        if (m_pCArmImageView != nullptr)
        {       
            /*降噪*/
            m_pImageOperationTab->denoiseGear->setText(QString("%1").arg(currentImage._originParam._denoise._mode));
            m_pImageOperationTab->denoiseSlider->setValue(currentImage._originParam._denoise._value);
            m_pImageOperationTab->denoiseValue->setText(QString("%1%").arg(currentImage._originParam._denoise._value));

            /*对比度*/
            m_pImageOperationTab->contrastSlider->setValue(currentImage._originParam._contrast);
            m_pImageOperationTab->contrastValue->setText(QString("%1%").arg(currentImage._originParam._contrast));
            m_pImageOperationTab->contrastText->setText(tr("自动"));

            /*亮度*/
            m_pImageOperationTab->brightnessSlider->setValue(currentImage._originParam._bright);
            m_pImageOperationTab->brightnessValue->setText(QString("%1%").arg(currentImage._originParam._bright));
            m_pImageOperationTab->brightnessText->setText(tr("自动"));

            /*锐度*/
            m_pImageOperationTab->sharpenGear->setText(QString("%1").arg(currentImage._originParam._sharpen._mode));
            m_pImageOperationTab->sharpenSlider->setValue(currentImage._originParam._sharpen._value);
            m_pImageOperationTab->sharpenValue->setText(QString("%1%").arg(currentImage._originParam._sharpen._value));

            /*金属校正*/
            m_pImageOperationTab->metalGearOperation((CARM_METAL_GEAR_ENUM)currentImage._originParam._mtcalib._mode);
            m_pImageOperationTab->metalText->setText(tr("自动"));

            m_pImageOperationTab->ValueLabel->setText(QString("%1°").arg(currentImage._originParam._rotate));
            if (m_pRotateDialog != nullptr)
            {
                m_pRotateDialog->rotateValue->setText(QString("%1°").arg(currentImage._originParam._rotate));
                m_pImageOperationTab->ValueLabel->setText(QString("%1°").arg(currentImage._originParam._rotate));
                m_pRotateDialog->hide();
            }
        }
    }
    
}

/*开始仿G臂测试*/
void CArmTouchPanel::GArmCheckStart_slot()
{
    /*if (m_pGArmCheckDialog == nullptr)
    {
        m_pGArmCheckDialog = new CArmDialog(true);
        m_pGArmCheckDialog->setGeometry(this->x() + GEOMETRY_X, this->y() + GEOMETRY_Y, GEOMETRY, GEOMETRY);
        m_pGArmCheckDialog->show();
    }*/
}

void CArmTouchPanel::showHistoryList_slot(bool state)
{
    if (state == true)
    {
        m_pHistoryWidget->move(imageWidget->width() - HISTORY_WIDGET, 0);
    }
    else
    {
        m_pHistoryWidget->move(imageWidget->width() - HISTORY_CHECK_WIDGET, 0);
    }
}

void CArmTouchPanel::showCurrentIndexImage_slot(int index)
{
    if (m_ImageInfoVec.size() == 0)
    {
        return;
    }
    CP_ImageData curentImage = m_ImageInfoVec[index];

    IPCImgIdTransfer transferData = getTransferInfo(curentImage);
    auto transfer = new CArmCmdImgTransfer(transferData);
    if (m_workStationComm != nullptr)
    {
        m_workStationComm->Request(transfer);
    }

    if (transfer != nullptr)
    {
        delete transfer;
        transfer = nullptr;
    }
}

/*仿G臂测试结果*/
void CArmTouchPanel::checkGArmResult(bool state)
{

}

void CArmTouchPanel::scanStateShoe_slot(bool state)
{
    if (state)
    {
        m_pScanDialog->setImage(m_styleStruct.scaningTrue, tr("曝光中"));
        m_pScanDialog->show();

        m_CurrentShootData = new CArmLastScanConfig{};
        memcmp(m_CurrentShootData, CArmManager::scanConfigConvert(), sizeof(CArmLastScanConfig));

    }
    else
    {
        m_pScanDialog->setImage(m_styleStruct.scaningFalse, tr("禁止曝光"));

        m_pScanDialog->hide();
        m_pHistoryWidget->checkButton->setEnabled(true);
        m_pHistoryWidget->setMppHistoryImageData(m_MppImageInfoVec);
        if (m_CurrentShootData->m_scanModeType == ScanMode::FLUORO_MPP)
        {
            if (m_pHistoryWidget != nullptr)
            {
               
            }
        }
        else if (m_CurrentShootData->m_scanModeType == ScanMode::POINT)
        {
            if (m_pHistoryWidget != nullptr)
            {

            }
        }
        else if (m_CurrentShootData->m_scanModeType == ScanMode::VIDEO)
        {
            if (m_pHistoryWidget != nullptr)
            {

            }
            if (m_pTimeClock == nullptr)
            {
                m_pTimeClock = new QTimer(this);
                if (m_pTimeClock != nullptr)
                {
                    /*设置播放速度*/
                    m_pTimeClock->start(m_CurrentShootData->m_pps);
                    connect(m_pTimeClock, SIGNAL(timeout()), this, SLOT(videoPlay_slot()));
                } 
            }
            CArmVideoAdjustWidget *videoAdjust = new CArmVideoAdjustWidget(imageWidget);
            if (videoAdjust != nullptr)
            {
                connect(videoAdjust, SIGNAL(playOrPauseVideo(bool)), this, SLOT(playOrPauseVideo_slot(bool)));
                connect(videoAdjust->retreat, SIGNAL(mouseClicked()), this, SLOT(videoRetreat_slot()));
                connect(videoAdjust->advance, SIGNAL(mouseClicked()), this, SLOT(videoAdvance_slot()));
                videoAdjust->move(imageWidget->x(), imageWidget->height() - m_pRotateDialog->height());
                videoAdjust->show();
            }
        }
    }
}

/*曝光状态*/
void CArmTouchPanel::scanStatus(bool state)
{
    ///曝光开始显示提示窗口，曝光结束提示窗口隐藏

    if (state == true)
    {
        if (m_pScanDialog != nullptr)
        {
            emit startScaning(true);
            m_bScaningStatus = true;
        }
    }
    else
    {
        if (m_pScanDialog != nullptr)
        {
            emit startScaning(false);
            m_bScaningStatus = false;
        }
    }
}

void CArmTouchPanel::setCurrentTab(CARM_TAB_ENUM currentTab)
{
    if (m_bScaningStatus == true)
        return;

    switch (currentTab)
    {
    case SCAN_TAB:
    {
        QPixmap scan(m_styleStruct.scanLabelTrue);
        scan = scan.scaled(ScanDataTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ScanDataTab->setPixmap(scan);
        QPixmap imageFalse(m_styleStruct.imageTbaFalse);
        imageFalse = imageFalse.scaled(CArmImageOperTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        CArmImageOperTab->setPixmap(imageFalse);

        QPixmap GArmFalse(m_styleStruct.GArmFalse);
        GArmFalse = GArmFalse.scaled(GArmTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        GArmTab->setPixmap(GArmFalse);
        m_pCurrentTabLabel->show();
        CArmInfostacked->setCurrentIndex(0);
        CArmToolstackedWidget->setCurrentIndex(0);

    }
    break;
    case IMAGE_TAB:
    {
        QPixmap scan(m_styleStruct.scanLabelFalse);
        scan = scan.scaled(ScanDataTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ScanDataTab->setPixmap(scan);
        QPixmap imageFalse(m_styleStruct.imageTabTrue);
        imageFalse = imageFalse.scaled(CArmImageOperTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        CArmImageOperTab->setPixmap(imageFalse);

        QPixmap GArmFalse(m_styleStruct.GArmFalse);
        GArmFalse = GArmFalse.scaled(GArmTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        GArmTab->setPixmap(GArmFalse);

        CArmInfostacked->setCurrentIndex(0);
        CArmToolstackedWidget->setCurrentIndex(1);

        m_pCurrentTabLabel->show();
        /*设置当前页下面提示标识的位置*/
        m_pCurrentTabLabel->setGeometry(widget_2->x() - 1 + 60, widget_2->y() + widget_2->height() + 3, 42, 4);
    }
    break;
    case GARM_TAB:
    {
        QPixmap scan(m_styleStruct.scanLabelFalse);
        scan = scan.scaled(ScanDataTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ScanDataTab->setPixmap(scan);
        QPixmap imageFalse(m_styleStruct.imageTbaFalse);
        imageFalse = imageFalse.scaled(CArmImageOperTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        CArmImageOperTab->setPixmap(imageFalse);

        QPixmap GArmFalse(m_styleStruct.GArmTrue);
        GArmFalse = GArmFalse.scaled(GArmTab->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        GArmTab->setPixmap(GArmFalse);
        m_pCurrentTabLabel->show();
        CArmInfostacked->setCurrentIndex(1);
        m_pCurrentTabLabel->setGeometry(widget_2->x() - 1 + 120, widget_2->y() + widget_2->height() + 3, 42, 4);
    }
    break;
    default:
        break;
    }

}

void CArmTouchPanel::loadImageTexture(const NewImgReceived *newData)
{
    CP_ReviewData* reviewData = new CP_ReviewData();
    reviewData->_imageData._imageIdx = newData->m_idx;
    reviewData->_imageData._imageHeight = newData->m_h;
    reviewData->_imageData._imageWidth = newData->m_w;
    reviewData->_imageData._exposureTime = m_CurrentShootData->m_scanParam.m_exposureTime;
    reviewData->_imageData._KVP = m_CurrentShootData->m_scanParam.m_kVp;

    reviewData->_imageData._XrayCurrent = m_CurrentShootData->m_scanParam.m_mA;
    reviewData->_imageData._pixelIssigned = 0;
    reviewData->_imageData._pixelSpacingX = 0.0;
    reviewData->_imageData._pixelSpacingY = 0.0;
    reviewData->_imageData._windowCenter = newData->m_windowCenter;
    reviewData->_imageData._windowWidth = newData->m_windowWidth;
    /*申请图像空间*/
    unsigned short* imageData = new unsigned short[newData->m_h * newData->m_w];

    /*将图像拷贝到申请好的空间中*/
    std::memcpy(imageData, newData->m_img, sizeof(unsigned short) * newData->m_h * newData->m_w);
    reviewData->_imageData._imageData = imageData;

    /*将患者信息存入影像数据中*/
    reviewData->_imageData._imageUID = newData->m_imgId;
    reviewData->_imageData._sereisInfo._studyInfo._studyUID = m_CurrentImageInfo._sereisInfo._studyInfo._studyUID;
    reviewData->_imageData._sereisInfo._studyInfo._patientInfo = m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo;

    reviewData->_originParam._bright = m_CurrentShootData->m_imageParam.m_brightness;
    reviewData->_originParam._contrast = m_CurrentShootData->m_imageParam.m_contrast;
    reviewData->_originParam._denoise._value = m_CurrentShootData->m_imageParam.m_denoise;
    reviewData->_originParam._denoise._mode = (CArmDenoiseEnum)m_CurrentShootData->m_imageParam.m_denoiseLevel;
    reviewData->_originParam._flipH = m_CurrentShootData->m_imageParam.m_reversalH;
    reviewData->_originParam._flipV = m_CurrentShootData->m_imageParam.m_reversalV;
    reviewData->_originParam._mtcalib._mode = (CArmMetalCalibEnum)m_CurrentShootData->m_imageParam.m_metalCalib;
    reviewData->_originParam._negative = m_CurrentShootData->m_imageParam.m_negative;
    reviewData->_originParam._rotate = m_CurrentShootData->m_imageParam.m_rotate;
    reviewData->_originParam._sharpen._value = m_CurrentShootData->m_imageParam.m_sharpen;
    reviewData->_originParam._sharpen._mode = (CArmSharpenEnum)m_CurrentShootData->m_imageParam.m_sharpenLevel;

    /*将每一张影像信息存入容器中*/
    m_ImageInfoVec.push_back(reviewData->_imageData);
    
    std::pair<CArmLastScanConfig*, CP_ImageData>pair{ m_CurrentShootData, reviewData->_imageData };

    m_PatientImageMap.insert(pair);

    m_MppImageInfoVec.push_back(reviewData);
    if (m_CurrentShootData->m_scanModeType == ScanMode::FLUORO_MPP)
    {
        m_MppImageInfoVec.push_back(reviewData);
    }
    else if (m_CurrentShootData->m_scanModeType == ScanMode::POINT)
    {
        m_PointImageInfoVec.push_back(reviewData);
    }
    else if (m_CurrentShootData->m_scanModeType == ScanMode::VIDEO)
    {
        m_VideoImageInfoVec.push_back(reviewData);
    }

    saveBinaryFile(reviewData->_imageData);
 
    emit sendImageData(reviewData);

}

void CArmTouchPanel::CArmViewLoadImageData_slot(CP_ReviewData* reviewData)
{
    /*获取影像数据*/
    TextureParams param;
    param.texture = CArmCommon::GetNewScanData(reviewData->_imageData);


    if (m_pImageTextureImporter != nullptr)
    {
        m_pImageTextureImporter->loadTexture(param, param.texture);
    }

    if (m_pCArmImageView != nullptr)
    {
        TextureObject3DPtr texture = CArmCommon::CreateTextureObject(reviewData->_imageData);
        reviewData->_texture = texture;
        if (m_pTextureImportor.createTexture(texture) == true)
        {
            m_pCArmImageView->loadReviewData(reviewData);
        }

    }
}

void CArmTouchPanel::videoPlay_slot()
{
    videoImageShow();

    m_iCountTime++;
}

void CArmTouchPanel::playOrPauseVideo_slot(bool state)
{
    if (state == false)
    {
        m_pTimeClock->stop();
    }
    else
    {
        m_pTimeClock->start();
    }
}

void CArmTouchPanel::videoImageShow()
{
    TextureObject3DPtr texture = CArmCommon::CreateTextureObject(m_VideoImageInfoVec[m_iCountTime]->_imageData);
    m_VideoImageInfoVec[m_iCountTime]->_texture = texture;
    if (m_pTextureImportor.createTexture(texture) == true)
    {
        m_pCArmImageView->loadReviewData(m_VideoImageInfoVec[m_iCountTime]);
    }
}

void CArmTouchPanel::videoRetreat_slot()
{
    if (m_pTimeClock != nullptr)
    {
        m_pTimeClock->stop();
        m_iCountTime--;
        videoImageShow();
        m_pTimeClock->start();
    }
}

void CArmTouchPanel::videoAdvance_slot()
{
    if (m_pTimeClock != nullptr)
    {
        m_pTimeClock->stop();
        m_iCountTime++;
        videoImageShow();
        m_pTimeClock->start();
    }
}

bool CArmTouchPanel::saveBinaryFile(CP_ImageData imageData)
{
    //  DeleteFile();

    std::string fileName = std::to_string(m_ImageInfoVec.size());
    std::string imageUid = "Temp/StudyUID/SeriesUID/" + fileName + ".dat";
    std::ofstream writeFile;
    writeFile.open(imageUid.c_str(), std::ios::binary | std::ios::out);
    if (writeFile.bad()) return false;

    writeFile.write(reinterpret_cast<char*>(&imageData), sizeof(CP_ImageData));
    writeFile.flush();
    writeFile.close();
    return true;
}

bool CArmTouchPanel::readBinaryFile(NewImgReceived* imageData)
{
    std::ifstream readFile;
    readFile.open("1.dat", std::ios::in | std::ios::binary);
    if (readFile.bad()) return false;

    readFile.read(reinterpret_cast<char*>(&imageData), sizeof(NewImgReceived));
    readFile.close();
    return true;
}

void CArmTouchPanel::setDataInfoToUi(const CArmCmdNewPatientInfo *datainfo)
{
    /*有新患者来，清空上个患者的数据*/
    m_ImageInfoVec.clear();
    m_PatientImageMap.clear();
    m_VideoImageInfoVec.clear();

    m_MppImageInfoVec.clear();

    m_PointImageInfoVec.clear();

    /*记录当前患者信息*/
    m_CurrentImageInfo._sereisInfo._studyInfo._studyUID = datainfo->m_arg.studyID;
    m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientAge = std::to_string(datainfo->m_arg.age);
    m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientID = datainfo->m_arg.patientID;
    m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientLastName = datainfo->m_arg.lastName;
    m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientFirstName = datainfo->m_arg.firstName;

    std::string firstN = datainfo->m_arg.firstName;
    std::string lastN = datainfo->m_arg.lastName;
    m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientName = lastN + firstN;
    NameLabel->setText(QString::fromLocal8Bit(m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientName.c_str()));
    if (datainfo->m_arg.gender == CArmGenderDef::F)
    {
        SexLabel->setText(tr("女"));
        m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientSex = "女";
    }
    else
    {
        SexLabel->setText(tr("男"));
        m_CurrentImageInfo._sereisInfo._studyInfo._patientInfo._patientSex = "男";
    }

    AgeLabel->setText(QString("%1").arg(datainfo->m_arg.age));

    if (m_pCArmImageView != nullptr)
        m_pCArmImageView->updatePatient();
}

void CArmTouchPanel::styleSetting(void)
{
    m_pScanSettingWidget = new CArmScanSetting();

    QGridLayout *layout = new QGridLayout;

    layout->addWidget(m_pScanSettingWidget);
    layout->setMargin(0);

    pageScanSetting->setLayout(layout);


    // 初始化底部病人信息
  //   initBaseScanInfo();

    initSplashDilog();


    m_timer = new QTimer(this);  //初始化定时器
    m_expTime = new QTime(0, 0, 0, 0); //初始化时间

    timeValue->setText(m_expTime->toString("mm:ss"));

   
}

void CArmTouchPanel::setWidgetStyle()
{
    CArmToolstackedWidget->setCurrentIndex(0);

    CArmToolWidget->setStyleSheet(m_styleStruct.widgetStyle2);
    CArmShowWidget->setStyleSheet(m_styleStruct.widgetStyle2);
    TabWidget->setStyleSheet(m_styleStruct.widgetStyle1);
    CArmToolstackedWidget->setStyleSheet(m_styleStruct.widgetStyle1);
    imageWidget->setStyleSheet(m_styleStruct.widgetStyle1);

    QPixmap ueg(m_styleStruct.UEGLabel);
    ueg = ueg.scaled(UEGLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    UEGLabel->setPixmap(ueg);

    setCurrentTab(SCAN_TAB);

    line->setStyleSheet(m_styleStruct.lineStyle);
    line_2->setStyleSheet(m_styleStruct.lineStyle);
    line_3->setStyleSheet(m_styleStruct.lineStyle);
    line_4->setStyleSheet(m_styleStruct.lineStyle);
    line_5->setStyleSheet(m_styleStruct.lineStyle);

    CArmtransferImageBut->setStyleSheet(m_styleStruct.toolButton);
    CArmtransferImageBut->setText(tr("图像传输"));
    CArmtransferImageBut->setIcon(QIcon(QString::fromLocal8Bit(m_styleStruct.transferFalse.toLatin1().data())));
    CArmtransferImageBut->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    kvpLabel->setText(tr("kVp"));
    kvpLabel->setAlignment(Qt::AlignCenter);
    kvpLabel->setStyleSheet(m_styleStruct.kvpLabel);

    MaLabel->setText(tr("mA"));
    MaLabel->setAlignment(Qt::AlignCenter);
    MaLabel->setStyleSheet(m_styleStruct.kvpLabel);

    timeLabel->setText(tr("Fluoro Time"));
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet(m_styleStruct.kvpLabel);

    kvpValue->setText(tr("-/-"));
    kvpValue->setAlignment(Qt::AlignCenter);
    kvpValue->setStyleSheet(m_styleStruct.valueLabel);

    MaValue->setText(tr("-/-"));
    MaValue->setAlignment(Qt::AlignCenter);
    MaValue->setStyleSheet(m_styleStruct.valueLabel);

    timeValue->setText(tr("-/-"));
    timeValue->setAlignment(Qt::AlignCenter);
    timeValue->setStyleSheet(m_styleStruct.valueLabel);

    scanTabInfo();

    QPixmap headMap(m_styleStruct.patientHead);
    headMap = headMap.scaled(HeadLabel->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    HeadLabel->setPixmap(headMap);

    NameLabel->setText(tr("-/-"));
    NameLabel->setStyleSheet(m_styleStruct.nameLabel);

    SexLabel->setText(tr("-/-"));
    SexLabel->setStyleSheet(m_styleStruct.kvpLabel);

    AgeLabel->setText(tr("-/-"));
    AgeLabel->setStyleSheet(m_styleStruct.kvpLabel);

    GArmTabInfo();

}

void CArmTouchPanel::mousePressEvent(QMouseEvent *e)
{  
    if (e->button() == Qt::LeftButton)
    {
       // calcFlouroTime();
        if (m_pScanSettingWidget->scanPartDialogIsshow())
        {
            m_pScanSettingWidget->hideScanPartDialog();
        }
        if (m_pScanSettingWidget->scanPpsDialogIsshow())
        {
            m_pScanSettingWidget->hidePpsDialog();
        }
    }
    QWidget::mousePressEvent(e);
}

void CArmTouchPanel::initSplashDilog()
{
    m_splashDialog = new QDialog(this);
    m_splashDialog->setStyleSheet("QDialog{background:rgba(0,0,0,1);}");
    m_splashDialog->setFixedSize(QSize(300, 50));
    m_splashDialog->setModal(true);
    m_splashDialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    m_splashLabel = new QLabel(m_splashDialog);
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_splashLabel);
    layout->setMargin(0);
    m_splashDialog->setLayout(layout);

    QRect deskRect = QApplication::desktop()->availableGeometry();
    int width = deskRect.width() / 2;
    int heigth = deskRect.height() / 2;
    QPoint point(width - 150, heigth - 50);


    m_splashLabel->setStyleSheet(QStringLiteral("QLabel{font-size:18px; color:white;} "));

    m_splashDialog->move(point);
}
// 未连接工作站
void CArmTouchPanel::splashDilogShow()
{
    m_bNetConnect = false;
    m_splashLabel->setText(tr("工作站未连接，正在尝试连接..."));
    m_splashDialog->show();

    m_bScanInfoFlag = false;
}
// 已连接工作站
void CArmTouchPanel::splashDilogClose()
{
    m_bNetConnect = true;
    m_splashDialog->hide();
}

// 右屏登录
void CArmTouchPanel::loginSlot()
{
    m_bLogin = true;
}
// 右屏登出
void CArmTouchPanel::logoutSlot()
{
    m_bLogin = false;

    m_bScanInfoFlag = false;
}

// 右屏选择患者信息后
void CArmTouchPanel::scanReadySlot()
{
    m_bLogin = true;// 临时添加，等到贺兴怡改过命令后可删除
    if (m_bNetConnect && m_bLogin)
    {
        m_splashDialog->hide();

        m_bScanInfoFlag = true;

        // 选择病人后发送扫描参数
        sendData2WorkStation(*CArmManager::scanConfigConvert());
    }
}


