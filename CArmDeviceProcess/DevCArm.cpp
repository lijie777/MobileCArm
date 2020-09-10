#include "DevCArm.h"
#include <IPCDefinitions.h>
#include <thread>


UDevCArm::UDevCArm(const IPCDeviceConfig &e, const std::string &strRawImgDir) : m_Det(GetComponent(DeviceComponentType::DetectorCT, e))
{
    m_DevCfg = e;
    m_pScCfg = GetComponent(DeviceComponentType::SlaveComputerCArm);

    m_hScDLL = NULL;
    m_pSC = nullptr;

    m_eDetStatus = ErrorCode::ERR_3IN1_CT_NOT_READY;
    m_eScStatus = ErrorCode::ERR_SC_DISCONNECTED;
}

UDevCArm::~UDevCArm()
{
    m_Det.Release();

    if (m_pSC != nullptr)
    {
        m_pSC->OnDevStatusChanged = nullptr;
        m_pSC->OnExpBtnChanged = nullptr;
        m_pSC->OnPedalChanged = nullptr;
        m_pSC->OnAxisStatusChanged = nullptr;
        m_pSC->OnXRayReady = nullptr;
        m_pSC->OnCArmWarn = nullptr;
        m_pSC->OnCArmCollimatorInPlace = nullptr;
        m_pSC->OnCArmAxisManAutoChanged = nullptr;
        m_pSC->OnCArmLaserStatusChanged = nullptr;
        m_pSC->OnCArmAxisPosChanged = nullptr;
        m_pSC->OnCArmDevResetCompleted = nullptr;

        m_pSC->Disconnect();
        m_pSC = nullptr;
    }

    if (m_hScDLL != NULL)
    {
        FreeLibrary(m_hScDLL);
        m_hScDLL = NULL;
    }
}

ErrorCode UDevCArm::Init()
{
    if (m_pScCfg == nullptr)
    {
        LOG4CPLUS_ERROR(g_logger, L"No slave computer configuration");
        return ErrorCode::ERR_3IN1_NO_SC_CONFIG;
    }

    if (m_Det.pDetConfig == nullptr)
    {
        LOG4CPLUS_WARN(g_logger, L"No detector configuration");
        return ErrorCode::ERR_3IN1_NO_CT_CONFIG;
    }

    //为康众探测器运行环境添加环境变量
    std::stringstream ss;
    ss << "Path=" << getenv("Path") << ';' << UCommonUtility::GetExeDir().c_str() << "\\CareView";
    int nRet = _putenv(ss.str().c_str());

    ErrorCode errCT = InitDetector();
    ErrorCode errSC = InitSlaveComputer();

    if (IS_ERR(errCT)) return errCT;
    if (IS_ERR(errSC)) return errSC;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode UDevCArm::Run()
{
    return ErrorCode::ERR_SUCCESS;
}

const IPCDeviceComponent *UDevCArm::GetComponent(DeviceComponentType e)
{
    for (int i = 0; i < m_DevCfg.m_nCompCount; i++)
    {
        if (m_DevCfg.m_lstDevComponent[i].m_Type == e) return &m_DevCfg.m_lstDevComponent[i];
    }

    return nullptr;
}

const IPCDeviceComponent *UDevCArm::GetComponent(DeviceComponentType e, const IPCDeviceConfig &devCfg)
{
    for (int i = 0; i < devCfg.m_nCompCount; i++)
    {
        if (devCfg.m_lstDevComponent[i].m_Type == e) return &devCfg.m_lstDevComponent[i];
    }

    return nullptr;
}

//初始化下位机连接
ErrorCode UDevCArm::InitSlaveComputer()
{
    PluginGetInfo funcDevInfo;
    PluginGetInstance funcDevInst;

    if (m_pScCfg == nullptr || !m_pScCfg->m_bSelected)
    {
        LOG4CPLUS_INFO(g_logger, L"Slave computer is disabled");
        return ErrorCode::ERR_3IN1_NO_SC_CONFIG;
    }

    LOG4CPLUS_INFO(g_logger, L"Initializing slave master");

    //加载动态库
    m_hScDLL = LoadPlugin(m_pScCfg->m_strPluginName, &funcDevInfo, &funcDevInst);
    if (m_hScDLL == NULL) return ErrorCode::ERR_3IN1_LOAD_SC_PLUGIN;

    //从动态库获取插件信息
    const PLUGIN_INFO *pPluginInfo = funcDevInfo();
    if (pPluginInfo != nullptr)
    {
        LOG4CPLUS_INFO(g_logger, L"Plugin info:");
        LOG4CPLUS_INFO_FMT(g_logger, L"Is singleton:%s", pPluginInfo->m_bSingleton ? L"Yes" : L"No");

        std::stringstream ss;
        for (int i = 0; i < MAX_COMP_NUM; i++)
        {
            if (std::strlen(pPluginInfo->m_strModels[i]) == 0) break;

            ss << pPluginInfo->m_strModels[i] << ',';
        }

        LOG4CPLUS_INFO_FMT(g_logger, L"Supported Models:%s", TOWS(ss.str()));
        LOG4CPLUS_INFO_FMT(g_logger, L"Vendor:%s", TOWS(pPluginInfo->m_strVendor));
        LOG4CPLUS_INFO_FMT(g_logger, L"Plugin version:%s", TOWS(pPluginInfo->m_strVersion));
        LOG4CPLUS_INFO_FMT(g_logger, L"Plugin type:%s", TOWS(UCommonUtility::PluginTypeToString(pPluginInfo->m_Type)));
    }

    //检查插件类型是否正确
    if (pPluginInfo->m_Type != PluginType::SlaveComputerCArm)
    {
        LOG4CPLUS_ERROR(g_logger, L"Wrong plugin type");
        return ErrorCode::ERR_3IN1_SC_PLUGIN_TYPE;
    }

    //获取下位机通信接口实例
    m_pSC = (USlaveComputerCArmBase*)funcDevInst(0);
    if (m_pSC == nullptr)
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get device instance from plugin <%s>", TOWS(m_pScCfg->m_strPluginName));
        return ErrorCode::ERR_3IN1_GET_SC_INST;
    }

#pragma region 下位机消息处理函数赋值

    //主控板工作状态变化的处理函数
    m_pSC->OnDevStatusChanged = std::bind(&UDevCArm::OnScDevStatusChanged, this, std::placeholders::_1);

    //曝光手闸按下状态变化的处理函数
    m_pSC->OnExpBtnChanged = std::bind(&UDevCArm::OnScExpBtnChanged, this, std::placeholders::_1, std::placeholders::_2);

    //脚踏板踏下状态变化的处理函数
    m_pSC->OnPedalChanged = std::bind(&UDevCArm::OnScPedalChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    //各轴运动状态变化（启停）的处理函数
    m_pSC->OnAxisStatusChanged = std::bind(&UDevCArm::OnScAxisStatusChanged, this, std::placeholders::_1, std::placeholders::_2);

    //X射线开始发射的消息处理函数
    m_pSC->OnXRayReady = std::bind(&UDevCArm::OnScXRayReady, this);

    //设备报警的处理函数
    m_pSC->OnCArmWarn = std::bind(&UDevCArm::OnScCArmWarn, this, std::placeholders::_1);

    //下位机在限束器运动到位后发送位置信息的处理函数
    m_pSC->OnCArmCollimatorInPlace = std::bind(&UDevCArm::OnScCArmCollimatorInPlace, this, std::placeholders::_1, std::placeholders::_2);

    //下位机在各轴自动/手动控制状态变化时发送消息的处理函数
    m_pSC->OnCArmAxisManAutoChanged = std::bind(&UDevCArm::OnScCArmAxisManAutoChanged, this, std::placeholders::_1, std::placeholders::_2);

    //下位机在激光灯开关状态变化时发送消息的处理函数
    m_pSC->OnCArmLaserStatusChanged = std::bind(&UDevCArm::OnScCArmLaserStatusChanged, this, std::placeholders::_1, std::placeholders::_2);

    //下位机在各轴发生位移时发送新位置的消息的处理函数
    m_pSC->OnCArmAxisPosChanged = std::bind(&UDevCArm::OnScCArmAxisPosChanged, this, std::placeholders::_1, std::placeholders::_2);

    //设备复位完成后的处理函数
    m_pSC->OnCArmDevResetCompleted = std::bind(&UDevCArm::OnScCArmDevResetCompleted, this, std::placeholders::_1);

    m_pSC->OnCArmRasterStatusChanged = [this](const bool open_)
    {
        if (OnCArmRasterStatusChanged)
        {
            OnCArmRasterStatusChanged(open_);
        }
    };

#pragma endregion

    std::thread thd(&USlaveComputerCArmBase::Connect, m_pSC, &g_logger, m_pScCfg->m_strIp, m_pScCfg->m_strPort);
    thd.detach();

    return ErrorCode::ERR_SUCCESS;
}

//初始化CT探测器连接
ErrorCode UDevCArm::InitDetector(bool bReconn)
{
    if (!m_Det.Selected())
    {
        LOG4CPLUS_WARN(g_logger, L"Detector is disabled");
        return ErrorCode::ERR_3IN1_CT_DISABLE;
    }

    LOG4CPLUS_INFO(g_logger, L"Initializing detector");

    //加载动态库
    if (!bReconn && !LoadDetectorPlugin(m_Det)) return ErrorCode::ERR_3IN1_LOAD_CT_PLUGIN;

    //从动态库获取插件信息
    const PLUGIN_INFO *pPluginInfo = m_Det.apiDevInfo();
    if (pPluginInfo != nullptr)
    {
        LOG4CPLUS_INFO(g_logger, L"Plugin info:");
        LOG4CPLUS_INFO_FMT(g_logger, L"Is singleton:%s", pPluginInfo->m_bSingleton ? L"Yes" : L"No");

        std::stringstream ss;
        for (int i = 0; i < MAX_COMP_NUM; i++)
        {
            if (std::strlen(pPluginInfo->m_strModels[i]) == 0) break;

            ss << pPluginInfo->m_strModels[i] << ',';
        }

        LOG4CPLUS_INFO_FMT(g_logger, L"Supported Models:%s", TOWS(ss.str()));
        LOG4CPLUS_INFO_FMT(g_logger, L"Vendor:%s", TOWS(pPluginInfo->m_strVendor));
        LOG4CPLUS_INFO_FMT(g_logger, L"Plugin version:%s", TOWS(pPluginInfo->m_strVersion));
        LOG4CPLUS_INFO_FMT(g_logger, L"Plugin type:%s", TOWS(UCommonUtility::PluginTypeToString(pPluginInfo->m_Type)));
    }

    //检查插件类型是否正确
    if (pPluginInfo->m_Type != PluginType::Detector && pPluginInfo->m_Type != PluginType::DetectorSimulator)
    {
        LOG4CPLUS_ERROR(g_logger, L"Wrong plugin type");
        return ErrorCode::ERR_3IN1_CT_PLUGIN_TYPE;
    }

    std::thread thd(&UDevCArm::DetectorThread, this);
    thd.detach();

    return ErrorCode::ERR_SUCCESS;
}

HMODULE UDevCArm::LoadPlugin(const std::string &strPlugin, PluginGetInfo *pAPIInfo, PluginGetInstance *pAPIInst)
{
    LOG4CPLUS_INFO_FMT(g_logger, L"Loading plugin <%s>", TOWS(strPlugin));

    std::stringstream pluginstrm;
    pluginstrm << UCommonUtility::GetExeDir() << "\\" << PLUGIN_FOLDER << "\\" << strPlugin << ".dll";
    HMODULE hDLL = LoadLibraryExA(pluginstrm.str().c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (hDLL == NULL)
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to load plugin <%s>, %s", TOWS(strPlugin), UCommonUtility::GetSystemErrorMessageW());
        return NULL;
    }

    *pAPIInfo = (PluginGetInfo)GetProcAddress(hDLL, "GetInfo");
    if (*pAPIInfo == nullptr)
    {
        FreeLibrary(hDLL);
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get plugin API GetInfo, plugin name:%s", TOWS(strPlugin));
        return NULL;
    }

    *pAPIInst = (PluginGetInstance)GetProcAddress(hDLL, "GetInstance");
    if (*pAPIInst == nullptr)
    {
        FreeLibrary(hDLL);
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get plugin API GetInstance, plugin name:%s", TOWS(strPlugin));
        return NULL;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Load plugin <%s> in success", TOWS(strPlugin));
    return hDLL;
}

bool UDevCArm::LoadDetectorPlugin(DetectorPlugin &dp)
{
    LOG4CPLUS_INFO_FMT(g_logger, L"Loading detector plugin <%s>", TOWS(dp.pDetConfig->m_strPluginName));

    std::stringstream pluginstrm;
    pluginstrm << UCommonUtility::GetExeDir() << "\\" << PLUGIN_FOLDER << "\\" << dp.pDetConfig->m_strPluginName << ".dll";

    dp.hDLL = LoadLibraryExA(pluginstrm.str().c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
    if (dp.hDLL == NULL)
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to load plugin <%s>, %s", TOWS(dp.pDetConfig->m_strPluginName), UCommonUtility::GetSystemErrorMessageW());
        return false;
    }

    dp.apiDevInfo = (PluginGetInfo)GetProcAddress(dp.hDLL, "GetInfo");
    if (dp.apiDevInfo == nullptr)
    {
        FreeLibrary(dp.hDLL);
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get plugin API GetInfo, plugin name:%s", TOWS(dp.pDetConfig->m_strPluginName));

        return false;
    }

    dp.apiInit = (PluginInitAPI)GetProcAddress(dp.hDLL, "InitApi");
    if (dp.apiInit == nullptr)
    {
        FreeLibrary(dp.hDLL);
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get plugin API InitApi, plugin name:%s", TOWS(dp.pDetConfig->m_strPluginName));
        return false;
    }

    dp.apiReinit = (PluginReinitAPI)GetProcAddress(dp.hDLL, "ReinitApi");
    if (dp.apiReinit == nullptr)
    {
        FreeLibrary(dp.hDLL);
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get plugin API ReinitApi, plugin name:%s", TOWS(dp.pDetConfig->m_strPluginName));
        return false;
    }

    dp.apiUninit = (PluginUninitAPI)GetProcAddress(dp.hDLL, "UninitApi");
    if (dp.apiUninit == nullptr)
    {
        FreeLibrary(dp.hDLL);
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get plugin API UninitApi, plugin name:%s", TOWS(dp.pDetConfig->m_strPluginName));
        return false;
    }

    dp.apiDevInst = (PluginGetDetectorInstance)GetProcAddress(dp.hDLL, "GetInstance");
    if (dp.apiDevInst == nullptr)
    {
        FreeLibrary(dp.hDLL);
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get plugin API GetInstance, plugin name:%s", TOWS(dp.pDetConfig->m_strPluginName));
        return false;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Load plugin <%s> in success", TOWS(dp.pDetConfig->m_strPluginName));
    return true;
}

//根据探测器厂商和型号，从配置文件获取完整探测器参数
ErrorCode UDevCArm::QueryDetectorConfig(DetectorInfoIPC &detInfo)
{
    std::string strResp;
    if (!RequestService(PortName::SysConfigInfoQuery, CMD::TO_CMD(CMD::QUERY_DETECTOR_INFO, { detInfo.strVendor, detInfo.strModel }), strResp))
    {
        return ErrorCode::ERR_3IN1_QUERY_DET_INFO;
    }

    auto subCmds = UCommonUtility::StringSplit(strResp, CMD::PARAM_SEPARATOR);
    if (subCmds[0] != CMD::QUERY_DETECTOR_INFO_RESP)
    {
        return ErrorCode::ERR_3IN1_QUERY_DET_INFO;
    }

    int nIdx = 1;
    std::strcpy(detInfo.strNickName, subCmds[nIdx++].c_str());

    detInfo.s16ColNum = std::stoi(subCmds[nIdx++]);
    detInfo.s16RowNum = std::stoi(subCmds[nIdx++]);
    detInfo.s16PixelSize = std::stoi(subCmds[nIdx++]);

    return ErrorCode::ERR_SUCCESS;
}

void UDevCArm::DetectorThread()
{
    while (true)
    {
        ErrorCode err = m_Det.apiInit(&g_logger);
        if (IS_ERR(err))
        {
            OnDetectorStatus(err);
            return;
        }

        //获取探测器通信接口实例
        m_Det.pDetector = (UDetectorBase*)m_Det.apiDevInst(m_Det.pDetConfig->m_strModel);
        if (m_Det.pDetector == nullptr)
        {
            LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to get detector instance from plugin <%s>", TOWS(m_Det.pDetConfig->m_strPluginName));
            OnDetectorStatus(ErrorCode::ERR_3IN1_GET_TP_INST);
            continue;;
        }

        LOG4CPLUS_INFO(g_logger, L"Find detector");

        m_Det.pDetector->OnDetectorError = std::bind(&UDevCArm::OnDetectorStatus, this, std::placeholders::_1);
        m_Det.pDetector->OnImageAcquired = std::bind(&UDevCArm::_onImageAcquired, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

        err = m_Det.pDetector->openLink(m_Det.pDetConfig->m_strPort);
        if (IS_ERR(err))
        {
            OnDetectorStatus(err);
            continue;
        }

        DetectorInfoIPC detInfo;
        err = m_Det.pDetector->getDetectorInfo(detInfo);
        if (IS_ERR(err))
        {
            OnDetectorStatus(err);
            continue;
        }

        err = QueryDetectorConfig(detInfo);
        if (IS_ERR(err))
        {
            OnDetectorStatus(err);
            return;
        }

        LOG4CPLUS_INFO_FMT(g_logger, L"Detector vendor:%s", TOWS(detInfo.strVendor));
        LOG4CPLUS_INFO_FMT(g_logger, L"Detector model:%s", TOWS(detInfo.strModel));
        LOG4CPLUS_INFO_FMT(g_logger, L"Detector nickname:%s", TOWS(detInfo.strNickName));

        GlobalMemMapping::SetDetectorInfo(DetectorType::CT, detInfo);

        break;
    }
}

void UDevCArm::OnDetectorStatus(ErrorCode e)
{
    m_eDetStatus = e;

    ErrorCode eStatus = IS_OK(m_eDetStatus) ? (IS_OK(m_eScStatus) ? ErrorCode::ERR_SUCCESS : m_eScStatus) : m_eDetStatus;

    OnDevStatusChanged(eStatus);
}

void UDevCArm::_onImageAcquired(int nIdx, unsigned short *pImgData, int nImgWid, int nImgHgt, bool bLossed)
{
    if(OnImageAcquired) OnImageAcquired(nIdx, pImgData, nImgWid, nImgHgt, bLossed);
}

#pragma region 下位机消息处理函数

//主控板工作状态变化的处理函数
void UDevCArm::OnScDevStatusChanged(ErrorCode e)
{
    m_eScStatus = e;

    ErrorCode eStatus = IS_OK(m_eScStatus) ? (IS_OK(m_eDetStatus) ? ErrorCode::ERR_SUCCESS : m_eDetStatus) : m_eScStatus;

    if(OnDevStatusChanged) OnDevStatusChanged(eStatus);
}

//曝光手闸按下状态变化的处理函数
void UDevCArm::OnScExpBtnChanged(CArmExpBtnDef eBtnStep, bool bPressed)
{
    if (OnExpBtnChanged) OnExpBtnChanged(eBtnStep, bPressed);
}

//脚踏板踏下状态变化的处理函数
void UDevCArm::OnScPedalChanged(CArmPedalDef ePedal, CArmExpBtnDef level, bool bPressed)
{
    if (OnPedalChanged) OnPedalChanged(ePedal, level, bPressed);
}

//各轴运动状态变化（启停）的处理函数
void UDevCArm::OnScAxisStatusChanged(CArmAxisDef eAxis, CArmAxisMotionDef eStatus)
{
    if (OnAxisStatusChanged) OnAxisStatusChanged(eAxis, eStatus);
}

//X射线开始发射
void UDevCArm::OnScXRayReady()
{
    if (IS_ERR(m_Det.GetDetStatus())) return;

    m_Det.pDetector->StartAcquisition();
}

//设备报警的处理函数
void UDevCArm::OnScCArmWarn(ErrorCode e)
{
    if(OnDevStatusChanged) OnDevStatusChanged(e);
}

//下位机在限束器运动到位后发送位置信息的处理函数
void UDevCArm::OnScCArmCollimatorInPlace(OptionCollimator coll, float fPos)
{
    if (OnCArmCollimatorInPlace) OnCArmCollimatorInPlace(coll, fPos);
}

//下位机在各轴自动/手动控制状态变化时发送消息的处理函数
void UDevCArm::OnScCArmAxisManAutoChanged(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus)
{
    if (OnCArmAxisManAutoChanged) OnCArmAxisManAutoChanged(eAxis, eStatus);
}

//下位机在激光灯开关状态变化时发送消息的处理函数
void UDevCArm::OnScCArmLaserStatusChanged(OptionLaserSwitch eLaser, bool bOn)
{
    if (OnCArmLaserStatusChanged) OnCArmLaserStatusChanged(eLaser, bOn);
}

//下位机在各轴发生位移时发送新位置的消息的处理函数
void UDevCArm::OnScCArmAxisPosChanged(CArmAxisDef eAxis, float fPos)
{
    if (OnCArmAxisPosChanged) OnCArmAxisPosChanged(eAxis, fPos);
}

//设备复位完成后的处理函数
void UDevCArm::OnScCArmDevResetCompleted(ErrorCode res)
{
    if (OnCArmDevResetCompleted) OnCArmDevResetCompleted(res);
}

#pragma endregion

#pragma region 外部设置接口

ErrorCode UDevCArm::StartScan2D(bool bXRay)
{
    if (m_pSC == nullptr) return ErrorCode::ERR_3IN1_DEV_NOT_READY;

    ErrorCode err = m_Det.GetDetStatus();
    if (IS_ERR(err)) return err;

    err = m_pSC->Start2DScan(bXRay);
    if (IS_ERR(err)) return err;

    //初始化探测器扫描
    GlobalMemMapping gmm;
    GlobalMemMapping::Get(gmm);

    SCAN_PARAM_STRUCT sps;
    sps.iROIRowStartPos = 0;
    sps.iROIRowEndPos = (gmm.DetectorInfoCT.s16RowNum / gmm.RangeCT->nBinningMode) - 1;
    sps.iROIColStartPos = 0;
    sps.iROIColEndPos = (gmm.DetectorInfoCT.s16ColNum / gmm.RangeCT->nBinningMode) - 1;
    sps.iFrameNum = gmm.RangeCT->nFrameNum;
    sps.dIntegration_Time = gmm.RangeCT->nExposure[0];

    return m_Det.pDetector->InitAcquisition(sps);
}

ErrorCode UDevCArm::StartScan3D(OptionCArmMove scanDirect, bool bXRay)
{
    if (m_pSC == nullptr) return ErrorCode::ERR_3IN1_DEV_NOT_READY;

    ErrorCode err = m_Det.GetDetStatus();
    if (IS_ERR(err)) return err;

    err = m_pSC->Start3DScan(scanDirect, bXRay);
    if (IS_ERR(err)) return err;

    //初始化探测器扫描
    GlobalMemMapping gmm;
    GlobalMemMapping::Get(gmm);

    SCAN_PARAM_STRUCT sps;
    sps.iROIRowStartPos = 0;
    sps.iROIRowEndPos = (gmm.DetectorInfoCT.s16RowNum / gmm.RangeCT->nBinningMode) - 1;
    sps.iROIColStartPos = 0;
    sps.iROIColEndPos = (gmm.DetectorInfoCT.s16ColNum / gmm.RangeCT->nBinningMode) - 1;
    sps.iFrameNum = gmm.RangeCT->nFrameNum;
    sps.dIntegration_Time = gmm.RangeCT->nExposure[0];

    return m_Det.pDetector->InitAcquisition(sps);
}

ErrorCode UDevCArm::GetScStatus()
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->GetStatus();
}

ErrorCode UDevCArm::ResetDevice()
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->DeviceReset();
}

ErrorCode UDevCArm::ResetWarn()
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->WarnReset();
}

ErrorCode UDevCArm::LaserSwitch(OptionLaserSwitch opt, bool bOn)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->LaserSwitch(opt, bOn);
}

ErrorCode UDevCArm::SetKV(float fKV)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->SetKV(fKV);
}

ErrorCode UDevCArm::SetMA(float fMA)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->SetMA(fMA);
}

ErrorCode UDevCArm::SetFPS(int nFPS)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->SetFrameRate(nFPS);
}

ErrorCode UDevCArm::SetExposure(int nExp)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->SetExposure(nExp);
}

ErrorCode UDevCArm::SetFrameNum(int nFrameNum)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->SetFrameNum(nFrameNum);
}

ErrorCode UDevCArm::SetCollimator(OptionCollimator coll, float speed, float fPos)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->SetCollimator(coll,speed, fPos);
}

ErrorCode UDevCArm::SetCollimatorContinueMove(OptionCollimator coll, OptionCollimatorMoveDirection d, float speed)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->SetCollimatorContinueMove(coll, d, speed);
}

ErrorCode UDevCArm::MoveAxis(OptionLiftColMove opt, float fSpeed)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->MoveLiftColomn(opt, fSpeed);
}

ErrorCode UDevCArm::MoveAxis(OptionXAxisMove opt, float fSpeed)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->MoveXAxis(opt, fSpeed);
}

ErrorCode UDevCArm::MoveAxis(OptionCArmMove opt, float fSpeed)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->MoveCArm(opt, fSpeed);
}

ErrorCode UDevCArm::GetPosAxis(CArmAxisDef axis, float &fVal)
{
    if (m_pSC == nullptr) return ErrorCode::ERR_3IN1_DEV_NOT_READY;

    switch (axis)
    {
    case CArmAxisDef::ForBackward:
        return m_pSC->GetXAxisPos(fVal);
    case CArmAxisDef::LiftingCol:
        return m_pSC->GetLiftColumnPos(fVal);
    case CArmAxisDef::CArm:
        return m_pSC->GetCArmAngle(fVal);
    default:
        return ErrorCode::ERR_GENERAL_UNKNOWN_AXIS;
    }
}

ErrorCode UDevCArm::SetPosAxis(CArmAxisDef axis, float fSpeed, float fPos)
{
    if (m_pSC == nullptr) return ErrorCode::ERR_3IN1_DEV_NOT_READY;

    switch (axis)
    {
    case CArmAxisDef::ForBackward:
        return m_pSC->SetXAxisPos(fSpeed, fPos);
    case CArmAxisDef::LiftingCol:
        return m_pSC->SetLiftColumnPos(fSpeed, fPos);
    case CArmAxisDef::CArm:
        return m_pSC->SetCArmAngle(fSpeed, fPos);
    default:
        return ErrorCode::ERR_GENERAL_UNKNOWN_AXIS;
    }
}

ErrorCode UDevCArm::SetManuAxis(CArmAxisDef axis, CArmAxisCtrlDef ctrl)
{
    if (m_pSC == nullptr) return ErrorCode::ERR_3IN1_DEV_NOT_READY;

    switch (axis)
    {
    case CArmAxisDef::ForBackward:
        return m_pSC->SetXAxisCtrlMode(ctrl);
    case CArmAxisDef::LiftingCol:
        return m_pSC->SetLiftColumnCtrlMode(ctrl);
    case CArmAxisDef::CArm:
        return m_pSC->SetCArmCtrlMode(ctrl);
    default:
        return ErrorCode::ERR_GENERAL_UNKNOWN_AXIS;
    }
}

ErrorCode UDevCArm::GetDAPValue(float &fVal)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->GetCurrentDapValue(fVal);
}

ErrorCode UDevCArm::SlaveScanCtrl(const SlaveScanCtrlType t)
{
    if (!m_pSC)
    {
        return ErrorCode::ERR_3IN1_DEV_NOT_READY;
    }

    if (SlaveScanCtrlType::Send2DScanCmd_XRay == t)
    {
        GlobalMemMapping::SetScanMode(ScanType::CT_Scan);

        ErrorCode err = m_Det.GetDetStatus();
        if (IS_ERR(err)) return err;

        //初始化探测器扫描
        GlobalMemMapping gmm;
        GlobalMemMapping::Get(gmm);

        SCAN_PARAM_STRUCT sps;

        sps.iROIRowStartPos = 0;
        sps.iROIRowEndPos = (gmm.DetectorInfoCT.s16RowNum / gmm.RangeCT[0].nBinningMode) - 1;
        sps.iROIColStartPos = 0;
        sps.iROIColEndPos = (gmm.DetectorInfoCT.s16ColNum / gmm.RangeCT[0].nBinningMode) - 1;
        sps.iFrameNum = gmm.RangeCT[0].nFrameNum;
        sps.dIntegration_Time = gmm.RangeCT[0].nExposure[0]/1000.0;

        auto retval = m_Det.pDetector->InitAcquisition(sps);

        if (IS_ERR(retval))
        {
            return retval;
        }
    }

    return m_pSC->SlaveScanCtrl(t);
}

ErrorCode UDevCArm::EnableDAP(bool bEnable)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->EnableDAP(bEnable);
}

ErrorCode UDevCArm::RadiationPermission(bool bEnable)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_CT_NOT_READY : m_pSC->RadiationPermission(bEnable);
}

ErrorCode UDevCArm::GetKV(float &fKV)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->GetCurrentKV(fKV);
}

ErrorCode UDevCArm::GetMA(float &fMA)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->GetCurrentMA(fMA);
}

ErrorCode UDevCArm::GetCollPos(OptionCollimator c, float& fVal)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_CEPH_NOT_READY : m_pSC->GetCollPos(c, fVal);
}

// ErrorCode UDevCArm::GetBulbTubeTemp(float &fTemp)
// {
//     return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->GetBulbTubeTemp(fTemp);
// }

// ErrorCode UDevCArm::Get(const std::string& type, float& val)
// {
//     return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->Get(type, val);
// }

ErrorCode UDevCArm::GetLaserStatus(OptionLaserSwitch opt, bool &bOn, float &fTime)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->GetLaserStatus(opt, bOn, fTime);
}

ErrorCode UDevCArm::GetMainboardTemp(float &fTemp)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->GetMainboardTemp(fTemp);
}

ErrorCode UDevCArm::Set3DTrack(int nNum, const float *pPoints)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->Send3DTrackData(nNum, pPoints);
}

ErrorCode UDevCArm::Get3DTrack(WORD &nNum, float **ppPoints)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->Retrieve3DTrackData(nNum, ppPoints);
}

//暂停/恢复球管控制
ErrorCode UDevCArm::EnableTubeCtrl(bool bEnable)
{
    return (m_pSC == nullptr) ? ErrorCode::ERR_3IN1_DEV_NOT_READY : m_pSC->EnableTubeCtrl(bEnable);
}

#pragma endregion
