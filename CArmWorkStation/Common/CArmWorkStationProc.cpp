#include "CArmWorkStationProc.h"
#include "WorkStationMsgCtrl.h"
#include "DcmUidDef.h"
#include "Process2D/Process2D.h"
#include "CommonUtility.h"
#include "GlobalMemMapDataDef.h"
#include "CArmScanCtrl.h"
#include "CArmCommon.h"

CArmWorkStationProc::CArmWorkStationProc()
    : m_tpComm{}
{

}

CArmWorkStationProc::~CArmWorkStationProc()
{
    delete m_tpComm;
}

bool CArmWorkStationProc::LoadConfig()
{
    return true;
}

bool CArmWorkStationProc::StartServices()
{
    // 扫描模块和消息控制模块建立关系
    WORK_STATION_MSG_CTRL->RegisterObserver(ClientType::SCAN_CTRL, [](const auto arg) { CARM_SCAN_CTRL->ExternalCmdHandler(arg); });

    ConnectDevMsg();

    if (!InitTPCtrl())
    {
        return false;
    }

    return UCArmDeviceComm::StartServices();
}

void CArmWorkStationProc::ConnectDevMsg()
{
    OnExpBtnChanged = [this](CArmExpBtnDef eBtnStep, bool bPressed)
    {
        const auto status = bPressed ? CArmBtnStatus::Pressed : CArmBtnStatus::Released;

        if (CArmExpBtnDef::Level1 == eBtnStep)
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdExpBtn1Pressed>(status));
        }
        else if (CArmExpBtnDef::Level2 == eBtnStep)
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdExpBtn2Pressed>(status));
        }
    };
    OnPedalChanged = [this](CArmPedalDef eBtnStep, CArmExpBtnDef level, bool bPressed)
    {
        ScanInfoServerMsg(std::make_unique<CArmCmdPedalPressed>(eBtnStep, level, bPressed ? CArmBtnStatus::Pressed : CArmBtnStatus::Released));
    };
    OnAxisStatusChanged = [this](CArmAxisDef eAxis, CArmAxisMotionDef eStatus)
    {
        if (CArmAxisDef::CArm == eAxis)
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdMotionStatusY>(eStatus));
        }
        else if (CArmAxisDef::ForBackward == eAxis)
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdMotionStatusX>(eStatus));
        }
        else
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdMotionStatusZ>(eStatus));
        }
    };
    OnCArmCollimatorInPlace = [this](OptionCollimator coll, float fPos)
    {
        CArmCmdDeviceValue req({});
        if (OptionCollimator::Hor == coll)
        {
            req.m_arg.Set<CArmDeviceValueTypeDef::CURRENT_COLL_H_POS>(fPos);
        }
        else if (OptionCollimator::Ver == coll)
        {
            req.m_arg.Set<CArmDeviceValueTypeDef::CURRENT_COLL_V_POS>(fPos);
        }
        else
        {
            return;
        }

        WorkStationMsgCtrl::GetInstance()->Request(ClientType::SCAN_INFO_SERVER, &req);
    };
    OnCArmAxisManAutoChanged = [this](CArmAxisDef eAxis, CArmAxisCtrlDef eStatus)
    {
        if (CArmAxisDef::CArm == eAxis)
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdAMChangedY>(eStatus));
        }
        else if (CArmAxisDef::ForBackward == eAxis)
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdAMChangedX>(eStatus));
        }
        else
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdAMChangedZ>(eStatus));
        }
    };
    OnCArmLaserStatusChanged = [this](OptionLaserSwitch eLaser, bool bOn)
    {
        const auto status = bOn ? CArmOnOffDef::On : CArmOnOffDef::Off;

        if (OptionLaserSwitch::CrossCurve == eLaser)
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdLaserStatusChangedCross>(status));
        }
        else
        {
            ScanInfoServerMsg(std::make_unique<CArmCmdLaserStatusChangedView>(status));
        }
    };
    OnCArmAxisPosChanged = [this](CArmAxisDef eAxis, float fPos)
    {
        CArmCmdDeviceValue req({});
        if (CArmAxisDef::CArm == eAxis)
        {
            req.m_arg.Set<CArmDeviceValueTypeDef::CURRENT_Y_AXIS_POS>(fPos);
        }
        else if (CArmAxisDef::ForBackward == eAxis)
        {
            req.m_arg.Set<CArmDeviceValueTypeDef::CURRENT_X_AXIS_POS>(fPos);
        }
        else if (CArmAxisDef::LiftingCol == eAxis)
        {
            req.m_arg.Set<CArmDeviceValueTypeDef::CURRENT_Z_AXIS_POS>(fPos);
        }
        else
        {
            return;
        }
        WorkStationMsgCtrl::GetInstance()->Request(ClientType::SCAN_INFO_SERVER, &req);
    };
    OnCArmDevResetCompleted = [this](ErrorCode errcode)
    {
        ScanInfoServerMsg(std::make_unique<CArmCmdDevResetDone>((unsigned short)errcode));
    };

    OnCArmRasterStatusChanged = [this](const bool open_)
    {
        ScanInfoServerMsg(std::make_unique<CArmCmdRasterStatusChanged>(open_));
    };

    OnNewImageReceived = [this](int nIdx, int nWidth, int nHeight, unsigned short*pImageData)
    {
        const auto imdId = UDcmUid::CreateImageUID(DeviceType::DR);
        auto calibratedImg = std::make_unique<std::uint16_t[]>(nWidth*nHeight);
        GetCalibratedImage(calibratedImg.get(), pImageData, nWidth*nHeight);
        ScanInfoServerMsg(std::make_unique<NewImgReceived>(imdId.data(), nIdx, nWidth, nHeight, calibratedImg.get(), std::uint16_t(std::pow(2, 12)), std::uint16_t(std::pow(2, 12)/2)));
    };

    // 设备状态发生变化
    OnDevStatusChanged = [this](const ErrorCode code_)
    {
        auto cmd = std::make_unique<CArmCmdDevStatusChanged>(code_);
        WORK_STATION_MSG_CTRL->Request(ClientType::DEV_STSTUS_SERVER, cmd.get());
    };

    // 扫描信息服务接收工作的消息的函数
    WORK_STATION_MSG_CTRL->RegisterObserver(ClientType::SCAN_INFO_SERVER, std::bind(&CArmWorkStationProc::Workstation2ScanInfoServer, this, std::placeholders::_1));
}

void CArmWorkStationProc::GetCalibratedImage(unsigned short *pOut, unsigned short *pIn, int nImgPixelCount)
{
    Process2D imgProc2d;

    //static std::string strCalibDir = UCommonUtility::GetExeDir() + CALIB_CONFIG_DIR_CT + '\\';
    GlobalMemMapping globalScanConfig;
    GlobalMemMapping::Get(globalScanConfig);

    size_t szGainSize = 0;
    static unsigned short *pGain = nullptr;
    if (pGain == nullptr) UCommonUtility::ReadFile(TONS(globalScanConfig.RangeCT[0].sGainFile), szGainSize, (char**)&pGain);

    size_t szDarkSize = 0;
    static unsigned short *pDark = nullptr;
    if (pDark == nullptr) UCommonUtility::ReadFile(TONS(globalScanConfig.RangeCT[0].sDarkFile), szDarkSize, (char**)&pDark);

    //static int nnn = 0;
    //std::string strFile = "D:\\Temp\\" + std::to_string(nnn) + "_In.dat";
    //UCommonUtility::WriteFile(strFile, pIn, nImgPixelCount * sizeof(short));

    float *pfOut = new float[nImgPixelCount];
    imgProc2d.imageGainCorrectionAndLog(pfOut, pIn, pDark + 1024, pGain + 1024, nImgPixelCount, 1);

    //static int nnn = 0;
    //strFile = "D:\\Temp\\" + std::to_string(nnn++) + "_Out.dat";
    //UCommonUtility::WriteFile(strFile, pfOut, nImgPixelCount * sizeof(float));

    for (int i = 0; i < nImgPixelCount; i++)
    {
        pOut[i] = UCommonUtility::ScalingPixel(12, PIXEL_WND::WND_MAX_DR, PIXEL_WND::WND_MIN_DR, pfOut[i]);
    }

    delete[] pfOut;
}

bool CArmWorkStationProc::InitTPCtrl()
{
    m_tpComm = new TouchPanelComm(std::to_string(6688), true);

    m_tpComm->m_newMsg = [this](const std::list<CArmCmdBase*>& cmds)
    {
        for (auto i : cmds)
        {
            WorkStationMsgCtrl::GetInstance()->Request(ClientType::TP, i);
        }
    };
    WorkStationMsgCtrl::GetInstance()->RegisterObserver(ClientType::TP, std::bind(&TouchPanelComm::SendCMD, m_tpComm, std::placeholders::_1));

    return m_tpComm->SetupServer();
}

void CArmWorkStationProc::Workstation2ScanInfoServer(const CArmCmdBase* cmd)
{
    if (auto ptr = dynamic_cast<const CArmCmdDeviceMoveCmd*>(cmd))
    {
        static std::map<CArmDeviceMoveTypeDef, std::function<void(const CArmCmdDeviceMoveCmd*)>> mymap
        {
            {
                CArmDeviceMoveTypeDef::SET_X_AXIS_POS,[this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->SetPosAxis(CArmAxisDef::ForBackward, arg.velocity, arg.pos);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_Y_AXIS_POS,[this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->SetPosAxis(CArmAxisDef::CArm, arg.velocity, arg.pos);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_Z_AXIS_POS, [this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->SetPosAxis(CArmAxisDef::LiftingCol, arg.velocity, arg.pos);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_X_AXIS_CONTINUE_MOVE, [this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->MoveAxisX(OptionXAxisMove(arg.direction), arg.velocity);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_Y_AXIS_CONTINUE_MOVE, [this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->MoveAxisY(OptionCArmMove(arg.direction), arg.velocity);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_Z_AXIS_CONTINUE_MOVE,[this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->MoveAxisZ(OptionLiftColMove(arg.direction), arg.velocity);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_H,[this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->SetCollimator(OptionCollimator::Hor, arg.velocity, arg.pos);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_COLLIMATOR_POS_V,[this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->SetCollimator(OptionCollimator::Ver, arg.velocity, arg.pos);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_COLLIMATOR_H_CONTINUE_MOVE,[this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->SetCollimatorContinueMove(OptionCollimator::Hor, OptionCollimatorMoveDirection(arg.direction), arg.velocity);
                }
            },
            {
                CArmDeviceMoveTypeDef::SET_COLLIMATOR_V_CONTINUE_MOVE,[this](const CArmCmdDeviceMoveCmd* ptr)
                {
                    const auto& arg = ptr->m_arg;
                    m_pScanInfoServer->SetCollimatorContinueMove(OptionCollimator::Ver, OptionCollimatorMoveDirection(arg.direction), arg.velocity);
                }
            }
        };

        if (mymap[ptr->m_arg.m_type])
        {
            (mymap[ptr->m_arg.m_type])(ptr);
        }
    }
    else if (auto cmd_ = dynamic_cast<const CArmCmdSlaveScanCtrl*>(cmd))
    {
        const auto errcode = m_pScanInfoServer->SlaveScanCtrl(cmd_->m_arg);
    }
    else if (auto cmd_ = dynamic_cast<const CArmCmdGetAllDevInfo*>(cmd))
    {
        CArmCmdDeviceValue ptr({});

        m_pScanInfoServer->GetBulbTubeTemp(ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_BULB_TUBE_TEMP>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetRasterStatus(ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_RASTER_STATUS>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetBatteryPercent(ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_BATTERY_PERCENT>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetPosAxis(CArmAxisDef::ForBackward, ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_X_AXIS_POS>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetPosAxis(CArmAxisDef::CArm, ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_Y_AXIS_POS>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetPosAxis(CArmAxisDef::LiftingCol, ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_Z_AXIS_POS>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetPosColl(OptionCollimator::Hor, ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_COLL_H_POS>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetPosColl(OptionCollimator::Ver, ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_COLL_V_POS>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);

        m_pScanInfoServer->GetDAPValue(ptr.m_arg.GetValueRef<CArmDeviceValueTypeDef::CURRENT_DAP_VALUE>());
        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_INFO_SERVER, &ptr);
    }
}

//检查本地数据库患者ID是否已经存在
ErrorCode CArmWorkStationProc::CheckPatientExistence(const std::string &strPatientID, bool &bExist)
{
    std::string strResp;
    if (!RequestService(PortName::DataService, CMD::TO_CMD(CMD::CHECK_PATIENT, { strPatientID }), strResp))
        return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST_REQUEST_SERVER;

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::CHECK_PATIENT_RESP)
    {
        bExist = atoi(lstSubCmds[1].c_str()) != 0;
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST;
}

ErrorCode CArmWorkStationProc::UpdatePatientCompleteOver(const std::string & patient_complete_id, const std::string & studyUID)
{
    std::string strResp;
    if (!RequestService(PortName::DataService, CMD::TO_CMD(CMD::UPDATE_PATIENT_COMPLETE_OVER, { patient_complete_id, studyUID }), strResp))
        return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST_REQUEST_SERVER;

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::UPDATE_PATIENT_COMPLETE_OVER_RESP)
    {
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST;
}

ErrorCode CArmWorkStationProc::UpdatePatientCompleteStatus(const std::string & patient_complete_id, StudyStatus sStudyStatus)
{
    std::string strResp;
    if (!RequestService(PortName::DataService, CMD::TO_CMD(CMD::UPDATE_PATIENT_COMPLETE_STATUS, { patient_complete_id, std::to_string((int)sStudyStatus) }), strResp))
        return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST_REQUEST_SERVER;

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::UPDATE_PATIENT_COMPLETE_STATUS_RESP)
    {
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST;
}

// 添加新患者信息到本地数据库
ErrorCode CArmWorkStationProc::AddNewPatients(int nCount, const DB_Patient *pPatientInfo)
{
    size_t nDataSize = sizeof(DB_Patient) * nCount;

    std::string strMapName = UCommonUtility::GetNewMemoryMapName();
    HANDLE hMemMap = UCommonUtility::CreateNewMappedMemory(strMapName, nDataSize, pPatientInfo);

    std::string strCmd = CMD::TO_CMD(CMD::NEW_PATIENT, { std::to_string(nCount), strMapName });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        CloseHandle(hMemMap);
        return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT_REQUEST_SERVER;
    }

    CloseHandle(hMemMap);
    hMemMap = INVALID_HANDLE_VALUE;

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::NEW_PATIENT_RESP)
    {
        if (atoi(lstSubCmds[1].c_str()) == 1)
            return ErrorCode::ERR_SUCCESS;
        else
            return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT;
}

ErrorCode CArmWorkStationProc::AddPatientToComplete(const std::string & patientID, std::string & completeID)
{
    std::string strResp;
    if (!RequestService(PortName::DataService, CMD::TO_CMD(CMD::PATIENT_TO_COMPLETE, { patientID }), strResp))
        return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST_REQUEST_SERVER;

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::PATIENT_TO_COMPLETE_RESP)
    {
        completeID = lstSubCmds[1];
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_READING_CHECK_PATIENT_EXIST;
}

//更新患者信息到本地数据库
ErrorCode CArmWorkStationProc::UpdatePatient(const std::string &patientID, const DB_Patient *pPatientInfo)
{
    size_t nDataSize = sizeof(DB_Patient);

    std::string strMapName = UCommonUtility::GetNewMemoryMapName();
    HANDLE hMemMap = UCommonUtility::CreateNewMappedMemory(strMapName, nDataSize, pPatientInfo);

    std::string strCmd = CMD::TO_CMD(CMD::UPDATE_PATIENT, { patientID, strMapName });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        CloseHandle(hMemMap);
        return ErrorCode::ERR_PATIENT_MANAGE_UPDATE_PATIENT_REQUEST_SERVER;
    }

    CloseHandle(hMemMap);
    hMemMap = INVALID_HANDLE_VALUE;

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::UPDATE_PATIENT_RESP)
    {
        if (atoi(lstSubCmds[1].c_str()) == 1)
            return ErrorCode::ERR_SUCCESS;
        else
            return ErrorCode::ERR_PATIENT_MANAGE_UPDATE_PATIENT;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_UPDATE_PATIENT;
}

ErrorCode CArmWorkStationProc::AddNewPatientsSchedule(int nCount, const DB_PatientSchedule * pPatientScheduleInfo)
{
    size_t nDataSize = sizeof(DB_PatientSchedule) * nCount;

    std::string strMapName = UCommonUtility::GetNewMemoryMapName();
    HANDLE hMemMap = UCommonUtility::CreateNewMappedMemory(strMapName, nDataSize, pPatientScheduleInfo);

    std::string strCmd = CMD::TO_CMD(CMD::NEW_PATIENT_SCHEDULE, { std::to_string(nCount), strMapName });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        CloseHandle(hMemMap);
        return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT_SCHEDULE_REQUEST_SERVER;
    }

    CloseHandle(hMemMap);
    hMemMap = INVALID_HANDLE_VALUE;

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::NEW_PATIENT_SCHEDULE_RESP)
    {
        if (atoi(lstSubCmds[1].c_str()) == 1)
            return ErrorCode::ERR_SUCCESS;
        else
            return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT_SCHEDULE;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT_SCHEDULE;
}

ErrorCode CArmWorkStationProc::AddPatientScheduleToComplete(const std::string & patient_schedule_id, std::string & patient_complete_id)
{
    std::string strResp;
    if (!RequestService(PortName::DataService, CMD::TO_CMD(CMD::PATIENT_SCHEDULE_TO_COMPLETE, { patient_schedule_id }), strResp))
        return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT_SCHEDULE_TO_COMPLETE;

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::PATIENT_SCHEDULE_TO_COMPLETE_RESP)
    {
        patient_complete_id = lstSubCmds[1];
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_ADD_PATIENT_SCHEDULE_TO_COMPLETE;
}

bool CArmWorkStationProc::getLocalDataSource(IPCLocalDataSource &pSource)
{
    std::string strResp;
    if (!RequestService(PortName::SysConfigInfoQuery, CMD::TO_CMD(CMD::LOCAL_DATA_CONFIG_QUERY, { std::to_string(GetCurrentProcessId()) }), strResp))
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to query local data source info");
        //OnDevStatusChanged(ErrorCode::ERR_3IN1_QUERY_DEV_INFO);
        return false;
    }

    //解析服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::LOCAL_DATA_CONFIG_RESP)
    {
        bool b = UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[1].c_str(), nullptr, 10), sizeof(IPCLocalDataSource), &pSource);
        if (!b)
        {
            LOG4CPLUS_ERROR_FMT(g_logger, L"%s", TOWS(UCommonUtility::GetLastError()));
            return false;
        }
        return true;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
        LOG4CPLUS_ERROR(g_logger, L"Error received from device info server");
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from device info server");

    //OnDevStatusChanged(ErrorCode::ERR_3IN1_QUERY_DEV_INFO);
    return false;
}

ErrorCode CArmWorkStationProc::DeletePatientsSchedule(const std::string &patientScheduleId)
{
    std::string strResp;
    if (!RequestService(PortName::DataService, CMD::TO_CMD(CMD::DELETE_PATIENT_SCHEDULE, { patientScheduleId }), strResp))
        return ErrorCode::ERR_PATIENT_MANAGE_DELETE_PATIENT_SCHEDULE;

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::DELETE_PATIENT_SCHEDULE_RESP)
    {
        //atoi(lstSubCmds[1].c_str()) != 0;
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_DELETE_PATIENT_SCHEDULE;
}

ErrorCode CArmWorkStationProc::DeletePatientsComplete(const std::string & patientCompleteId)
{
    std::string strResp;
    if (!RequestService(PortName::DataService, CMD::TO_CMD(CMD::DELETE_PATIENT_COMPLETE, { patientCompleteId }), strResp))
        return ErrorCode::ERR_PATIENT_MANAGE_DELETE_PATIENT_COMPLETE;

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::DELETE_PATIENT_COMPLETE_RESP)
    {
        //atoi(lstSubCmds[1].c_str()) != 0;
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_DELETE_PATIENT_COMPLETE;
}

ErrorCode CArmWorkStationProc::GetTmpStudyDateByPatient(const std::string & patientID, int & nCount, DB_StudyDate ** ppDB_StudyDate)
{
    std::string strCmd = CMD::TO_CMD(CMD::TMP_STUDY_DATE_BY_PATIENT_QUERY, { patientID, std::to_string(GetCurrentProcessId()) });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_REQUEST_SERVER;
    }

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::TMP_STUDY_DATE_BY_PATIENT_RESP)
    {
        nCount = atoi(lstSubCmds[1].c_str());
        if (nCount <= 0) return ErrorCode::ERR_SUCCESS;

        *ppDB_StudyDate = new DB_StudyDate[nCount];

        UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[2].c_str(), nullptr, 10), nCount * sizeof(DB_StudyDate), *ppDB_StudyDate);
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT;
}

ErrorCode CArmWorkStationProc::GetTmpSeriesByComplete(const std::string & patientCompleteID, int & nCount, DB_TmpSeries ** ppDB_TmpSeries)
{
    std::string strCmd = CMD::TO_CMD(CMD::TMP_SERIES_BY_COMPLETE_QUERY, { patientCompleteID, std::to_string(GetCurrentProcessId()) });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_REQUEST_SERVER;
    }

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::TMP_SERIES_BY_COMPLETE_RESP)
    {
        nCount = atoi(lstSubCmds[1].c_str());
        if (nCount <= 0) return ErrorCode::ERR_SUCCESS;

        *ppDB_TmpSeries = new DB_TmpSeries[nCount];

        UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[2].c_str(), nullptr, 10), nCount * sizeof(DB_TmpSeries), *ppDB_TmpSeries);
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT;
}

ErrorCode CArmWorkStationProc::GetTmpSeriesByStudy(const std::string & studyUID, int & nCount, DB_TmpSeries ** ppDB_TmpSeries)
{
    std::string strCmd = CMD::TO_CMD(CMD::TMP_SERIES_BY_STUDY_QUERY, { studyUID, std::to_string(GetCurrentProcessId()) });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_REQUEST_SERVER;
    }

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::TMP_SERIES_BY_STUDY_RESP)
    {
        nCount = atoi(lstSubCmds[1].c_str());
        if (nCount <= 0) return ErrorCode::ERR_SUCCESS;

        *ppDB_TmpSeries = new DB_TmpSeries[nCount];

        UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[2].c_str(), nullptr, 10), nCount * sizeof(DB_TmpSeries), *ppDB_TmpSeries);
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT;
}

//查询本地患者列表
ErrorCode CArmWorkStationProc::GetLocalPatients(const SearchCondition &lstQueryCondition, int &nPatientCount, DB_Patient **pPatientInfo)
{
    return GetPatients(CMD::LOCAL, lstQueryCondition, nPatientCount, pPatientInfo);
}

ErrorCode CArmWorkStationProc::GetLocalPatientsSchedule(const SearchCondition & lstQueryCondition, int & nPatientCount, DB_PatientSchedule ** pPatientScheduleInfo)
{
    return GetPatientsSchedule(CMD::LOCAL, lstQueryCondition, nPatientCount, pPatientScheduleInfo);
}

ErrorCode CArmWorkStationProc::GetLocalPatientsComplete(const SearchCondition & lstQueryCondition, int & nPatientCount, DB_PatientComplete ** pPatientCompleteInfo)
{
    return GetPatientsComplete(CMD::LOCAL, lstQueryCondition, nPatientCount, pPatientCompleteInfo);
}

//获取本地或者远程的患者信息
ErrorCode CArmWorkStationProc::GetPatients(const std::string &strPos, const SearchCondition &lstQueryCondition, int &nPatientCount, DB_Patient **pPatientInfo)
{
    std::string strMapName = UCommonUtility::GetNewMemoryMapName();
    HANDLE hMemMap = UCommonUtility::CreateNewMappedMemory(strMapName, sizeof(SearchCondition), &lstQueryCondition);

    std::string strCmd = CMD::TO_CMD(CMD::PATIENT_QUERY, { strPos, strMapName, std::to_string(GetCurrentProcessId()) });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        CloseHandle(hMemMap);
        return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_REQUEST_SERVER;
    }

    CloseHandle(hMemMap);
    hMemMap = INVALID_HANDLE_VALUE;

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::PATIENT_RESP)
    {
        nPatientCount = atoi(lstSubCmds[1].c_str());
        if (nPatientCount <= 0) return ErrorCode::ERR_SUCCESS;

        *pPatientInfo = new DB_Patient[nPatientCount];

        UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[2].c_str(), nullptr, 10), nPatientCount * sizeof(DB_Patient), *pPatientInfo);
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT;
}

ErrorCode CArmWorkStationProc::GetPatientsSchedule(const std::string & strPos, const SearchCondition & lstQueryCondition, int & nPatientCount, DB_PatientSchedule ** pPatientScheduleInfo)
{
    std::string strMapName = UCommonUtility::GetNewMemoryMapName();
    HANDLE hMemMap = UCommonUtility::CreateNewMappedMemory(strMapName, sizeof(SearchCondition), &lstQueryCondition);

    std::string strCmd = CMD::TO_CMD(CMD::PATIENT_SCHEDULE_QUERY, { strPos, strMapName, std::to_string(GetCurrentProcessId()) });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        CloseHandle(hMemMap);
        return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_SCHEDULE_REQUEST_SERVER;
    }

    CloseHandle(hMemMap);
    hMemMap = INVALID_HANDLE_VALUE;

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::PATIENT_SCHEDULE_RESP)
    {
        nPatientCount = atoi(lstSubCmds[1].c_str());
        if (nPatientCount <= 0) return ErrorCode::ERR_SUCCESS;

        *pPatientScheduleInfo = new DB_PatientSchedule[nPatientCount];

        UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[2].c_str(), nullptr, 10), nPatientCount * sizeof(DB_PatientSchedule), *pPatientScheduleInfo);
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_SCHEDULE;
}

ErrorCode CArmWorkStationProc::GetPatientsComplete(const std::string & strPos, const SearchCondition & lstQueryCondition, int & nPatientCount, DB_PatientComplete ** pPatientCompleteInfo)
{
    std::string strMapName = UCommonUtility::GetNewMemoryMapName();
    HANDLE hMemMap = UCommonUtility::CreateNewMappedMemory(strMapName, sizeof(SearchCondition), &lstQueryCondition);

    std::string strCmd = CMD::TO_CMD(CMD::PATIENT_COMPLETE_QUERY, { strPos, strMapName, std::to_string(GetCurrentProcessId()) });

    std::string strResp;
    if (!RequestService(PortName::DataService, strCmd, strResp))
    {
        CloseHandle(hMemMap);
        return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_COMPLETE_REQUEST_SERVER;
    }

    CloseHandle(hMemMap);
    hMemMap = INVALID_HANDLE_VALUE;

    //解析数据服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::PATIENT_COMPLETE_RESP)
    {
        nPatientCount = atoi(lstSubCmds[1].c_str());
        if (nPatientCount <= 0) return ErrorCode::ERR_SUCCESS;

        *pPatientCompleteInfo = new DB_PatientComplete[nPatientCount];

        UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[2].c_str(), nullptr, 10), nPatientCount * sizeof(DB_PatientComplete), *pPatientCompleteInfo);
        return ErrorCode::ERR_SUCCESS;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
    {
        LOG4CPLUS_ERROR(g_logger, L"Error received from data server");
        return (ErrorCode)(atoi(lstSubCmds[1].c_str()));
    }
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from data server");

    return ErrorCode::ERR_PATIENT_MANAGE_GET_PATIENT_COMPLETE;
}