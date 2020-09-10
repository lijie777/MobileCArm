#include "CArmTrackingWidget.h"
#include <QDateTime>
#include <QDir>
#include "Common\CArmPage.h"

#pragma execution_character_set("utf-8")

#define _CDISK_DEVICENAME   L"C:/"     //<! 文件存储磁盘分区路径

#define _1024               1024       //<! 字节单位换算
#define _MILLI              0.001     //<! 千分之一

CArmTrackingWidget::CArmTrackingWidget(QWidget *parent)
    : QFrame(parent)
    , m_pStatus(nullptr)
    , m_pTrackViewer(new TrackView(0, this))
    , m_pTextureImportor(ViewToolApi::createImage2DTextureImporter(m_pTrackViewer))
{
    initUI();
    initLogic();

    // 注册到列表，接收指令
    CArmObject::registerObject(this);

    //// 造假数据
    //_CArmPatientID ptID = "ptID";
    //_CArmStudyUID studyUID = { ptID ,"studyUID" };
    //_CArmSeriesUID seriesUID = { studyUID , "seriesUID" };
    //_CArmImageUID imageUID = { seriesUID, "imageUID" };
    //CP_ImageInfo image = { imageUID, 1,1,1,1,1,0.1,0.1,0.1,0.1,1,true, nullptr };
    //CP_SeriesInfo series = { seriesUID , "seriesNumber", "seriesDate", "seriesTime","bodyPart", "descp" };
    //CP_StudyInfo study = { studyUID , "studyID", "acsNumber", "studyDate","studyTime", "modality","descp" };
    //CP_PatientInfo patient = { ptID , "ptName", "age", "sex"};

    //series._imageInfo[imageUID] = image;
    //study._seriesInfo[seriesUID] = series;
    //patient._studyInfo[studyUID] = study;
    //m_CurrPatientInfo = patient;

    //m_InstanceInfo._imageUID = imageUID;
    //m_InstanceInfo._seriesUID = seriesUID;
    //m_InstanceInfo._studyUID = studyUID;
    //m_InstanceInfo._patientID = ptID;

}

CArmTrackingWidget::~CArmTrackingWidget()
{
    if (m_pTrackViewer != nullptr)      delete m_pTrackViewer;
}

void CArmTrackingWidget::updateConfig(const CArmConfig & config)
{
}




void CArmTrackingWidget::clockTimeOut()
{
    updateTime();
    updateDiskCapacity();
    updatePictureInfo();
    updateMovieInfo();
}

void CArmTrackingWidget::showScreen()
{
    ui.stackedWidget->currentWidget() == ui.screenView
        ? ui.stackedWidget->setCurrentWidget(ui.trackingView)
        : ui.stackedWidget->setCurrentWidget(ui.screenView);
}

void CArmTrackingWidget::transferImage()
{
    CArmPage::notifyReviewInfo(m_ImageInfo[m_CurrSeriesInfo._seriesUID], CArmCommon::GetInstanceInfo(m_CurrImageInfo->_imageData));
}

void CArmTrackingWidget::initUI()
{
    ui.setupUi(this);
    ui.viewerLayout->addWidget(m_pTrackViewer);
    ui.statusWidget->hide();
    setWindowFlags(Qt::FramelessWindowHint);
    m_ClockTimer.start(3000);
    clockTimeOut();

    setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetLogoutStatus(this, std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetIdleStatus(this)))));

}

void CArmTrackingWidget::initLogic()
{
    connect(&m_ClockTimer, SIGNAL(timeout()), this, SLOT(clockTimeOut()));
    connect(ui.screenBtn, SIGNAL(clicked()), this, SLOT(showScreen()));
    connect(ui.transferBtn, SIGNAL(clicked()), this, SLOT(transferImage()));
}

void CArmTrackingWidget::doCmd(const CArmCMD& cmd)
{
    if (m_pStatus != nullptr)
    {
        m_pStatus->doCmd(cmd);
    }
}

void CArmTrackingWidget::setWidgetStatus(std::shared_ptr<TrackingWidgetStatus> status)
{
    if (status != nullptr)  status->dealStatus();
    m_pStatus = status;
}

void CArmTrackingWidget::updateTime()
{
    QDate date = QDate::currentDate();
    ui.dateLab->setText(date.toString("yyyy-MM-dd"));
    ui.dateLab_2->setText(date.toString("yyyy-MM-dd"));
    QTime time = QTime::currentTime();
    ui.timeLab->setText(time.toString("hh:mm"));
    ui.hourLab->setText(QString("%1").arg(time.hour(), 2, 10, QLatin1Char('0')));
    ui.minLab->setText(QString("%1").arg(time.minute(), 2, 10, QLatin1Char('0')));
}

void CArmTrackingWidget::updateDiskCapacity()
{
    // 磁盘分区路径
    LPCWSTR lpcwstrDriver = _CDISK_DEVICENAME;

    ULARGE_INTEGER liFreeBytesAvailable;        //分区的可用字节数
    ULARGE_INTEGER liTotalBytes;                //总字节数
    ULARGE_INTEGER liTotalFreeBytes;            //空闲字节数

    if (!GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes))
    {
        //qDebug() << "ERROR: Call to GetDiskFreeSpaceEx() failed.";
        return;
    }

    quint64 availableDisk = liFreeBytesAvailable.QuadPart / _1024 / _1024 / _1024; // 单位为GB
    quint64 totalDisk = liTotalBytes.QuadPart / _1024 / _1024 / _1024; // 单位为GB

    ui.usefulValue->setText(QString::number(availableDisk));
    ui.capacityValue->setText(QString::number(totalDisk));
}

void CArmTrackingWidget::updatePictureInfo()
{
    // 在指定文件夹下索引
    QString storagePath;

    QDir dir(storagePath);
    QFileInfoList list = dir.entryInfoList(QStringList("*.dcm"), QDir::Files | QDir::Readable, QDir::Name);
    quint64 picSize = 0;  // 图像数据大小
    for (auto & i : list) picSize += i.size();

    quint64 totalSize = ui.capacityValue->text().toInt();
    int totalWidth = ui.capacityBar->width();
    int picWidth = (float)picSize / totalSize * totalWidth;

    ui.picCount->setText(QString::number(list.size()));
    ui.picValue->setText(QString::number(totalSize));
    ui.picValue->setFixedWidth(picWidth);
}

void CArmTrackingWidget::updateMovieInfo()
{
    // 在指定文件夹下索引
    QString storagePath;

    QDir dir(storagePath);
    QFileInfoList list = dir.entryInfoList(QStringList("*.dcm"), QDir::Files | QDir::Readable, QDir::Name);
    quint64 movieSize = 0;  // 视频数据大小
    for (auto & i : list) movieSize += i.size();

    quint64 totalSize = ui.capacityValue->text().toInt();
    int totalWidth = ui.capacityBar->width();
    int picWidth = (float)movieSize / totalSize * totalWidth;

    ui.movieCount->setText(QString::number(list.size()));
    ui.movieValue->setText(QString::number(totalSize));
    ui.movieValue->setFixedWidth(picWidth);

}


void CArmTrackingWidget::updateScanInfo(CArmLastScanConfig data)
{
    m_ScanData = data;

    // 更新拍摄模式
    switch (ScanMode(data.m_scanPart))
    {
        //点拍
    case ScanMode::POINT:
        ui.pulseLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/pointScan.png);}");
        break;
        //脉冲透视
    case ScanMode::FLUORO_MPP:
        ui.pulseLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/pulse.png);}");
        break;
        //视频
    case ScanMode::VIDEO:
        ui.pulseLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/movie.png);}");
        break;
    default:
        break;
    }



    // 更新帧率
    ui.ppsValue->setText(QString::number(data.m_pps));

    // 更新剂量
    switch (data.m_doseType)
    {
        //常规剂量
    case DoseType::REGULAR:
        ui.doseLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/normaldose.png);}");
        break;
        //高剂量
    case DoseType::HIGH:
        ui.doseLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/highdose.png);}");
        break;
        //低剂量
    case DoseType::LOW:
        ui.doseLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/lowdose.png);}");
        break;
    default:
        break;
    }

    // 更新拍摄部位
    switch (data.m_scanPart)
    {
        //腹部
    case ScanPart::ABDOMEN:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/abdomen.png);}");
        break;
        //四肢
    case ScanPart::LIMBS:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/arms.png);}");
        break;
        //胃部
    case ScanPart::ENDOSCOPE:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/endoscope.png);}");
        break;
        //头部
    case ScanPart::HEAD:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/head.png);}");
        break;
        //心脏
    case ScanPart::HEART:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/heart.png);}");
        break;
        //软组织
    case ScanPart::SOFT_TISSUE:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/muscle.png);}");
        break;
        //颈部
    case ScanPart::NECK:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/neck.png);}");
        break;
        //躯干
    case ScanPart::TRUNCK:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/trunk.png);}");
        break;
        //泌尿系统
    case ScanPart::URINARY:
        ui.bodyLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/urinary.png);}");
        break;
    default:
        break;
    }

    // 更新曝光时间
    ui.fluTimeValue->setText(CArmCommon::GetTimeString(data.m_scanParam.m_exposureTime).c_str());

    // 更新电压值
    ui.kvpValue->setText(QString::number(data.m_scanParam.m_kVp));

    // 更新电流值
    ui.maValue->setText(QString::number(data.m_scanParam.m_mA,'f', 1));

    // 更新图像处理参数
    updateImgAdjustParms(data.m_imageParam);
}

void CArmTrackingWidget::updateExposeInfo(float gycm, float mgymin, float mgy)
{
    ui.gycmValue->setText(QString::number(gycm, 'f', 3));
    ui.mgyperValue->setText(QString::number(mgymin, 'f', 3));
    ui.mgyValue->setText(QString::number(mgy, 'f', 2));
}

void CArmTrackingWidget::updateImgAdjustParms(ImageParam parms)
{
    if (m_pTrackViewer == nullptr) return;

    CArmImgVT* imgVT = m_pTrackViewer->getImageVTInst();

    if (imgVT == nullptr) return;
    
    imgVT->updateBright(parms.m_brightness);
    imgVT->updateContrast(parms.m_contrast);
    imgVT->setNegative(parms.m_negative);
    imgVT->enableFlipH(parms.m_reversalH);
    imgVT->enableFlipV(parms.m_reversalV);
    imgVT->setNegative(parms.m_negative);
    imgVT->setRotateParam(parms.m_rotate);

    imgVT->setSharpenParam({ (float)parms.m_sharpen ,CArmSharpenEnum::IT_Sharpen_0 });//待定
    imgVT->setDenoiseParam({ (float)parms.m_denoise ,CArmDenoiseEnum::IT_Denoise_0 });//待定
    imgVT->setMetalCalibParam({ (float)parms.m_metalCalib ,CArmMetalCalibEnum::IT_MetalCalib_0 });//待定


}

void CArmTrackingWidget::updatLasterStatus(bool on)
{
    on == true ? ui.laserLab->show() : ui.laserLab->hide();
}

void CArmTrackingWidget::updateNetworkStatus(bool on)
{
    on == true ? ui.networkLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/network.png)}")
        : ui.networkLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/errnetwork.png)}");
}

void CArmTrackingWidget::updateDeviceStatus(ErrorCode code)
{
    code == ErrorCode::ERR_SUCCESS ? ui.abortLab->show() : ui.abortLab->hide();
    
    if (code == ErrorCode::ERR_SUCCESS)
    {
        ui.statusWidget->hide();
    }
    else
    {
        // 显示曝光提示
        ui.statusTip->setText("禁止曝光");
        ui.statusPix->setStyleSheet(
            "QLabel{\
        background:transparent;\
        font: 16px Microsoft YaHei;\
        image:url(:/TrackingWidget/TrackingWidget/Resource/abort.png);}");
        ui.statusTip->setStyleSheet(
            "QLabel{\
        background:transparent;\
        border:none;\
        color:#FC3923;\
        font: 16px Microsoft YaHei;}");
        ui.statusWidget->show();
    }

}

void CArmTrackingWidget::updateRasterStatus(bool on)
{
    on == true ? ui.rasterLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/install.png)}")
        : ui.rasterLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/uninstall.png)}");
}

void CArmTrackingWidget::updateTPStatus(bool on)
{
    on == true ? ui.connLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/connect.png)}")
        : ui.connLab->setStyleSheet("QLabel{image:url(:/TrackingWidget/TrackingWidget/Resource/disconnect.png)}");
}

void CArmTrackingWidget::updatePowerStatus(POWER_STATUS status)
{
    // 功能待定
    switch (status)
    {
    case POWER_STATUS::PS_HIGH:
        break;
    case POWER_STATUS::PS_LOW:
        break;
    default:
        break;
    }
}


void CArmTrackingWidget::updateTemperatureStatus(TEMPRATURE_STATUS status)
{
    // 功能待定

    switch (status)
    {
        //温度正常
    case TEMPRATURE_STATUS::TS_NORMAL:
        ui.statusWidget->hide();
        break;
        //温度超标
    case TEMPRATURE_STATUS::TS_OVERLIMIT:
        ui.statusTip->setText("球管温度过高");
        ui.statusPix->setStyleSheet(
        "QLabel{\
        background:transparent;\
        font: 16px Microsoft YaHei;\
        image:url(:/TrackingWidget/TrackingWidget/Resource/highttemp.png);}");
        ui.statusTip->setStyleSheet(
        "QLabel{\
        background:transparent;\
        border:none;\
        color:#FC3923;\
        font: 16px Microsoft YaHei;}");
        ui.stackedWidget->show();
        break;
        //温度告警
    case TEMPRATURE_STATUS::TS_WARNING:
        ui.stackedWidget->hide();
        break;
    default:
        break;
    }
}


void CArmTrackingWidget::updateStudyInfo(const CP_StudyData & info)
{
    m_CurrStudyInfo = info;
    m_ImageInfo.clear();

    CP_PatientData pt = info._patientInfo;

    QString strPatientName = QString::fromLocal8Bit(pt._patientName.c_str());
    QFontMetrics fontMetrics(ui.nameLab->font());
    if (fontMetrics.width(strPatientName) > ui.nameLab->maximumWidth())
    {
        QString n_strPatientName = QFontMetrics(ui.nameLab->font()).elidedText(strPatientName, Qt::ElideRight, ui.nameLab->maximumWidth());
        ui.nameLab->setText(n_strPatientName);
    }
    else
    {
        ui.nameLab->setText(strPatientName);
    }

    ui.nameLab->setToolTip(strPatientName);
    ui.sexLab->setText(QString::fromLocal8Bit(pt._patientSex.c_str()));
    ui.ageLab->setText(QString::fromLocal8Bit(pt._patientAge.c_str()));

}

void CArmTrackingWidget::updateSeriesInfo(const CP_SeriesData & info)
{
    m_CurrSeriesInfo = info;
    m_CurrSeriesInfo._studyInfo = m_CurrStudyInfo;

}

void CArmTrackingWidget::updateImage(const CP_ImageData & info)
{
    // 获取图像调节参数
    CArmImgParam imgParam = CArmCommon::GetImageParam(info);

    // 填充图像数据
    CP_ImageData image = info;
    image._sereisInfo = m_CurrSeriesInfo;

    //更新阅片数据
    m_CurrImageInfo = new CP_ReviewData{ image,imgParam,imgParam };

    //添加阅片数据到类别
    m_ImageInfo[m_CurrSeriesInfo._seriesUID].push_back(m_CurrImageInfo);

    // 创建图像纹理
    TextureObject3DPtr texture = CArmCommon::CreateTextureObject(m_CurrImageInfo->_imageData);
    m_CurrImageInfo->_texture = texture;
    if (m_pTextureImportor.createTexture(texture) == true)
    {
        // 显示图像到窗口
        m_pTrackViewer->loadReviewData(m_CurrImageInfo);

    }
}




void TrackingWidgetStatus::exposeXRay()
{
    m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetExposureStatus(m_pWidget)));
}

void TrackingWidgetStatus::exposeAbort()
{
    //m_pWidget->setWidgetStatus(new TrackingWidgetAbortStatus(m_pWidget));
}

void TrackingWidgetStatus::updateLaster(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdLaserStatusChangedCross* temp = static_cast<const CArmCmdLaserStatusChangedCross*>(object.get());
    if (temp == nullptr) return;
    m_pWidget->updatLasterStatus(temp->m_arg == CArmOnOffDef::On ? true : false);
}

void TrackingWidgetStatus::updateTPStatus(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdTpWsNetStatus* temp = static_cast<const CArmCmdTpWsNetStatus*>(object.get());
    if (temp == nullptr) return;
    m_pWidget->updateTPStatus(temp->m_arg == CArmNetStatus::Connected ? true : false);
}


void TrackingWidgetStatus::updateTemper()
{
    m_pWidget->updateTemperatureStatus(TEMPRATURE_STATUS::TS_NORMAL);
}

void TrackingWidgetStatus::updatePower()
{
    m_pWidget->updatePowerStatus(POWER_STATUS::PS_HIGH);
}

void TrackingWidgetStatus::updateImageParms(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdImgAdjustParam* temp = static_cast<const CArmCmdImgAdjustParam*>(object.get());
    if (temp == nullptr) return;
    m_pWidget->updateImgAdjustParms(temp->m_arg);
}

void TrackingWidgetStatus::updateScanParms(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdScanParam* temp = static_cast<const CArmCmdScanParam*>(object.get());
    if (temp == nullptr) return;
    m_pWidget->updateScanInfo(CArmLastScanConfig{ temp->m_arg });
}

void TrackingWidgetStatus::receiveImage(std::shared_ptr<CArmCmdBase> object)
{
    const NewImgReceived* temp = static_cast<const NewImgReceived*>(object.get());
    if (temp == nullptr) return;
    
    // 获取拍摄参数
    CArmLastScanConfig scanPm = m_pWidget->m_ScanData;

    // 拷贝图像数据
    unsigned short* data = new unsigned short[temp->m_h * temp->m_w];
    std::memcpy(data, temp->m_img, temp->m_h * temp->m_w * sizeof(unsigned short));

    // 填充图像信息
    CP_ImageData imageInfo;
    imageInfo._imageUID = temp->m_h;        //!< 图像标识号
    imageInfo._imageIdx = temp->m_idx;      //!< 图像序号
    imageInfo._imageWidth = temp->m_w;      //!< 图像宽度
    imageInfo._imageHeight = temp->m_h;     //!< 图像高度
    imageInfo._windowCenter = temp->m_windowCenter; //!< 窗位
    imageInfo._windowWidth = temp->m_windowWidth;   //!< 窗宽
    imageInfo._pixelSpacingX = 0.0;                 //!< 像素水平分辨率
    imageInfo._pixelSpacingY = 0.0;                 //!< 像素竖直分辨率
    imageInfo._KVP = scanPm.m_scanParam.m_kVp;      //!< 电压
    imageInfo._XrayCurrent = scanPm.m_scanParam.m_mA;             //!< 管电流
    imageInfo._exposureTime = scanPm.m_scanParam.m_exposureTime;  //!< 曝光时间(ms)
    imageInfo._pixelIssigned = false;   //!< 像素数据是否有符号
    imageInfo._bitsAllocated = 16;      //!< 每个像素所占位数
    imageInfo._bitsStored = 12;         //!< 每个像素的有效位数
    imageInfo._highBit = 11;            //!< 符号位
    imageInfo._imageData = data;        //!< 图像像素

    // 发送图像信息到左屏窗口
    m_pWidget->updateImage(imageInfo);

}

void TrackingWidgetStatus::receiveRequest(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdDeviceValue* temp = static_cast<const CArmCmdDeviceValue*>(object.get());
    if (temp == nullptr) return;
    //更新球管温度
    if (temp->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_BULB_TUBE_TEMP)
    {
        float value = temp->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_BULB_TUBE_TEMP>();
        m_pWidget->updateTemperatureStatus(TEMPRATURE_STATUS::TS_NORMAL); 
    }
    //更新电量
    else if (temp->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_BATTERY_PERCENT)
    {
        float value = temp->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_BATTERY_PERCENT>();
        m_pWidget->updatePowerStatus(POWER_STATUS::PS_HIGH);
    }
    //更新光栅状态
    else if (temp->m_arg.ValueType() == CArmDeviceValueTypeDef::CURRENT_RASTER_STATUS)
    {
        int value = temp->m_arg.Get<CArmDeviceValueTypeDef::CURRENT_RASTER_STATUS>();
        m_pWidget->updateRasterStatus(value == 1);
    }
}

void TrackingWidgetStatus::finishExpore()
{
    m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetReadyStatus(m_pWidget)));
}


void TrackingWidgetStatus::initLogic()
{
}





void TrackingWidgetIdleStatus::doCmd(const CArmCMD& cmdObject)
{
    // 注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetLogoutStatus(m_pWidget, m_pWidget->m_pStatus)));
    }
    // 初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {
    }
    // 设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {
        handleDeviceCMD(static_cast<const CArmDeviceCMD&>(cmdObject));
    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {
        if (m_pWidget != nullptr)
        {
            const CArmPrepareScanCMD& cmd = static_cast<const CArmPrepareScanCMD&>(cmdObject);
            // 更新患者信息
            CP_StudyData study;
            study._studyUID = cmd._scanInfo._stdUID;
            study._patientInfo = cmd._patientInfo;
            m_pWidget->updateStudyInfo(study);
        }
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}


void TrackingWidgetIdleStatus::dealStatus()
{
    // 更新左屏窗口显示
    //m_pWidget->updateScanInfo(CArmScanConfig{ ScanPart::LIMBS,DoseType::HIGH,ScanMode::FLUORO_MPP, 2, 55.0, 1.0,0,123800 });
    m_pWidget->updateExposeInfo(0.0, 0.0, 0.0);
    m_pWidget->ui.transferBtn->setEnabled(false);
    m_pWidget->show();
}

void TrackingWidgetIdleStatus::handleDeviceCMD(const CArmDeviceCMD & cmdObject)
{
    // 处理设备通讯指令
    if (cmdObject._cmdObject != nullptr)
    {
        // 激光灯状态变化
        if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_CROSS)    updateLaster(cmdObject._cmdObject);
        else if(cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_VIEW) updateLaster(cmdObject._cmdObject);
        // 触摸屏连接状态
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_TP_WS_NET_STATUS)    updateTPStatus(cmdObject._cmdObject);
        // 设备异常状态
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)      updateDevStatus(cmdObject._cmdObject);
        // 请求的数据
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)    receiveRequest(cmdObject._cmdObject);
        // 图像调节变化
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IMG_ADJUST_PARAM)    updateImageParms(cmdObject._cmdObject);
        // 拍片参数变化
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_SET_SCAN_PARAM)    updateScanParms(cmdObject._cmdObject);
        // 开始曝光
        else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_EXP_BTN1_PRESSED)        startExpose(cmdObject._cmdObject);

    }
}

void TrackingWidgetIdleStatus::updateDevStatus(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdDevStatusChanged* temp = static_cast<const CArmCmdDevStatusChanged*>(object.get());
    if (temp == nullptr) return;

    if (temp->m_arg != ErrorCode::ERR_SUCCESS)
    {
        // 更新状态
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetAbortStatus(m_pWidget, m_pWidget->m_pStatus, temp->m_arg)));
    }
}

void TrackingWidgetIdleStatus::startExpose(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdExpBtn1Pressed* temp = static_cast<const CArmCmdExpBtn1Pressed*>(object.get());
    if (temp == nullptr) return;

    if (temp->m_arg == CArmBtnStatus::Pressed)
    {
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetExposureStatus(m_pWidget)));
    }
}


void TrackingWidgetReadyStatus::doCmd(const CArmCMD& cmdObject)
{
    // 注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetLogoutStatus(m_pWidget, m_pWidget->m_pStatus)));
    }
    // 退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {
        // 初始化左屏窗口
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetIdleStatus(m_pWidget)));
    }
    // 设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {
        handleDeviceCMD(static_cast<const CArmDeviceCMD&>(cmdObject));
    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetExposureStatus(m_pWidget)));
    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {
        if (m_pWidget != nullptr)
        {
            const CArmPrepareScanCMD& cmd = static_cast<const CArmPrepareScanCMD&>(cmdObject);
            // 更新患者信息
            CP_StudyData study;
            study._studyUID = cmd._scanInfo._stdUID;
            study._patientInfo = cmd._patientInfo;
            m_pWidget->updateStudyInfo(study);
        }
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void TrackingWidgetReadyStatus::dealStatus()
{
    m_pWidget->ui.statusWidget->hide();
}

void TrackingWidgetReadyStatus::handleDeviceCMD(const CArmDeviceCMD & cmdObject)
{
    if (cmdObject._cmdObject != nullptr)
    {
        // 开始发送射线
        if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_XRAY_READY)                   exposeXRay();
        // 激光灯状态变化
        else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_CROSS)   updateLaster(cmdObject._cmdObject);
        else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_VIEW)    updateLaster(cmdObject._cmdObject);
        // 触摸屏连接状态
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_TP_WS_NET_STATUS)        updateTPStatus(cmdObject._cmdObject);
        // 请求的数据
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)        receiveRequest(cmdObject._cmdObject);
        // 设备异常状态
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)          updateDevStatus(cmdObject._cmdObject);
        // 光栅状态
        // 图像调节变化
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IMG_ADJUST_PARAM)        updateImageParms(cmdObject._cmdObject);
        // 拍片参数变化
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_SET_SCAN_PARAM)          updateScanParms(cmdObject._cmdObject);
        // 开始曝光
        else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_EXP_BTN1_PRESSED)        startExpose(cmdObject._cmdObject);
    }
}

void TrackingWidgetReadyStatus::updateDevStatus(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdDevStatusChanged* temp = static_cast<const CArmCmdDevStatusChanged*>(object.get());
    if (temp == nullptr) return;

    if (temp->m_arg != ErrorCode::ERR_SUCCESS)
    {
        // 更新状态
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetAbortStatus(m_pWidget, m_pWidget->m_pStatus, temp->m_arg)));
    }
}

void TrackingWidgetReadyStatus::startExpose(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdExpBtn1Pressed* temp = static_cast<const CArmCmdExpBtn1Pressed*>(object.get());
    if (temp == nullptr) return;
    if (temp->m_arg == CArmBtnStatus::Pressed)
    {
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetExposureStatus(m_pWidget)));
    }
}





void TrackingWidgetExposureStatus::doCmd(const CArmCMD& cmdObject)
{
    // 设备消息处理命令
    if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {
        handleDeviceCMD(static_cast<const CArmDeviceCMD&>(cmdObject));
    }
}

void TrackingWidgetExposureStatus::dealStatus()
{
    if (m_pWidget == nullptr) return;

    // 显示曝光提示
    m_pWidget->ui.statusTip->setText("曝光中");
    m_pWidget->ui.statusPix->setStyleSheet(
        "QLabel{\
        background:transparent;\
        font: 16px Microsoft YaHei;\
        image:url(:/TrackingWidget/TrackingWidget/Resource/expose.png);}");
    m_pWidget->ui.statusTip->setStyleSheet(
        "QLabel{\
        background:transparent;\
        border:none;\
        color:#FFFFD42A;\
        font: 16px Microsoft YaHei;}");
    m_pWidget->ui.statusWidget->show();
}

void TrackingWidgetExposureStatus::handleDeviceCMD(const CArmDeviceCMD & cmdObject)
{
    if (cmdObject._cmdObject != nullptr)
    {
        // 接收图像
        if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_NEW_IMG_RECEIVED)         receiveImage(cmdObject._cmdObject);
        // 曝光异常
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)      updateDevStatus(cmdObject._cmdObject);
        // 请求的数据
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)    receiveRequest(cmdObject._cmdObject);
    }
}

void TrackingWidgetExposureStatus::updateDevStatus(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdDevStatusChanged* temp = static_cast<const CArmCmdDevStatusChanged*>(object.get());
    if (temp == nullptr) return;
    if (temp->m_arg != ErrorCode::ERR_SUCCESS)
    {
        // 更新状态
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetAbortStatus(m_pWidget, m_pWidget->m_pStatus, temp->m_arg)));//下一状态待定
    }
}




void TrackingWidgetAbortStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetLogoutStatus(m_pWidget, m_pNxtStatus)));
    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {
        // 初始化左屏窗口
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetIdleStatus(m_pWidget)));
    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {
        handleDeviceCMD(static_cast<const CArmDeviceCMD&>(cmdObject));
    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {
        if (m_pWidget != nullptr)
        {
            const CArmPrepareScanCMD& cmd = static_cast<const CArmPrepareScanCMD&>(cmdObject);
            // 更新患者信息
            CP_StudyData study;
            study._studyUID = cmd._scanInfo._stdUID;
            study._patientInfo = cmd._patientInfo;
            m_pWidget->updateStudyInfo(study);
        }
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void TrackingWidgetAbortStatus::dealStatus()
{
    if (m_pWidget == nullptr) return;

    // 显示曝光提示
    m_pWidget->ui.statusTip->setText("禁止曝光");
    m_pWidget->ui.statusPix->setStyleSheet(
        "QLabel{\
        background:transparent;\
        font: 16px Microsoft YaHei;\
        image:url(:/TrackingWidget/TrackingWidget/Resource/abort.png);}");
    m_pWidget->ui.statusTip->setStyleSheet(
        "QLabel{\
        background:transparent;\
        border:none;\
        color:#FC3923;\
        font: 16px Microsoft YaHei;}");
    m_pWidget->ui.statusWidget->show();
}

void TrackingWidgetAbortStatus::handleDeviceCMD(const CArmDeviceCMD & cmdObject)
{
    // 激光灯状态变化
    if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_CROSS)    updateLaster(cmdObject._cmdObject);
    else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_VIEW) updateLaster(cmdObject._cmdObject);
    // 触摸屏连接状态
    else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_TP_WS_NET_STATUS)    updateTPStatus(cmdObject._cmdObject);
    // 设备异常状态
    else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)      updateDevStatus(cmdObject._cmdObject);
    // 请求的数据
    else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEVICE_VALUE)    receiveRequest(cmdObject._cmdObject);
    // 图像调节变化
    else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IMG_ADJUST_PARAM)    updateImageParms(cmdObject._cmdObject);
    // 拍片参数变化
    else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_SET_SCAN_PARAM)      updateScanParms(cmdObject._cmdObject);

}

void TrackingWidgetAbortStatus::updateDevStatus(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdDevStatusChanged* temp = static_cast<const CArmCmdDevStatusChanged*>(object.get());
    if (temp->m_arg == ErrorCode::ERR_SUCCESS)
    {
        // 更新状态
        m_pWidget->setWidgetStatus(m_pNxtStatus);
    }
}


void TrackingWidgetLogoutStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        dealStatus();
    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {
        // 初始化左屏窗口
        m_pWidget->setWidgetStatus(m_pNxtStatus);
    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {
        handleDeviceCMD(static_cast<const CArmDeviceCMD&>(cmdObject));
    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {
        //收到指令不作处理
    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {
        //更新患者信息
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void TrackingWidgetLogoutStatus::dealStatus()
{
    if(m_pWidget != nullptr) m_pWidget->hide();
}

void TrackingWidgetLogoutStatus::handleDeviceCMD(const CArmDeviceCMD & cmdObject)
{
    if (cmdObject._cmdObject != nullptr)
    {
        // 开始发送射线
        if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_XRAY_READY)                   exposeXRay();
        // 激光灯状态变化
        else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_CROSS)   updateLaster(cmdObject._cmdObject);
        else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_LSR_STS_CHANGED_VIEW)    updateLaster(cmdObject._cmdObject);
        // 触摸屏连接状态
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_TP_WS_NET_STATUS)        updateTPStatus(cmdObject._cmdObject);
        // 设备异常状态
        else if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)          updateDevStatus(cmdObject._cmdObject);
        // 光栅状态
        // 电压变化
        // 电量变化
        // 曝光时间变化
        // 图像调节变化
    }
}

void TrackingWidgetLogoutStatus::updateDevStatus(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdDevStatusChanged* temp = static_cast<const CArmCmdDevStatusChanged*>(object.get());
    if (temp == nullptr) return;
    if (temp->m_arg != ErrorCode::ERR_SUCCESS)
    {
        // 更新状态
        m_pWidget->setWidgetStatus(std::shared_ptr<TrackingWidgetStatus>(new TrackingWidgetAbortStatus(m_pWidget, m_pNxtStatus, temp->m_arg)));
    }
}


