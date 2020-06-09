#include "IPC.h"
#include <log4cplus/loggingmacros.h>
#include <regex>
#include <IPC_CMD_Def_CArm.h>

const std::string USubProcessBase::ModuleName = "DevCArmProc";
const std::string USubProcessBase::LogHeader = "[DevCArmProc]";

extern Logger g_logger;

#define INPUT_ARG_COUNT 3

UDevCArmProc::UDevCArmProc()
{
    m_pConnHeartBeat = nullptr;
    m_pConnDevStatus = nullptr;
    m_pConnScanComm = nullptr;

    m_pDev = nullptr;

    m_eLatestDevStatus = ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;
}

UDevCArmProc::~UDevCArmProc()
{
    if (m_pConnHeartBeat != nullptr)
    {
        m_pConnHeartBeat->Disconnect();

        delete m_pConnHeartBeat;
        m_pConnHeartBeat = nullptr;
    }

    if (m_pConnDevStatus != nullptr)
    {
        m_pConnDevStatus->Disconnect();

        delete m_pConnDevStatus;
        m_pConnDevStatus = nullptr;
    }

    if (m_pConnScanComm != nullptr)
    {
        m_pConnScanComm->Disconnect();

        delete m_pConnScanComm;
        m_pConnScanComm = nullptr;
    }

    if (m_pDev != nullptr) delete m_pDev;
}

//解析输入参数
bool UDevCArmProc::ArgsParse(int argc, char *argv[])
{
    //检查启动参数数量
    if (argc != INPUT_ARG_COUNT + 1)
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Wrong input argument count:%d", argc);
        return false;
    }

#pragma region //第一个参数:向Launcher进程发送心跳包端口

    m_strHeartbeatPort = argv[1];
    if (!std::regex_match(m_strHeartbeatPort, std::regex("^([0-9]{1,})$")))
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Wrong heartbeat port:%s", TOWS(m_strHeartbeatPort));
        return false;
    }

    int nPort = std::stoi(m_strHeartbeatPort);
    if (nPort < MIN_USER_PORT || nPort > MAX_USER_PORT)
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Wrong heartbeat port:%s", TOWS(m_strHeartbeatPort));
        return false;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Heartbeat port:%s", TOWS(m_strHeartbeatPort));

#pragma endregion

#pragma region //第二个参数:端口服务端口
    m_strPortServerPort = argv[2];
    if (!std::regex_match(m_strPortServerPort, std::regex("^([0-9]{1,})$")))
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Wrong port server port:%s", TOWS(m_strPortServerPort));
        return false;
    }

    nPort = std::stoi(m_strPortServerPort);
    if (nPort < MIN_USER_PORT || nPort > MAX_USER_PORT)
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Wrong port server port:%s", TOWS(m_strPortServerPort));
        return false;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Port server port:%s", TOWS(m_strPortServerPort));

#pragma endregion

    m_strDeviceID = argv[3];
    LOG4CPLUS_INFO_FMT(g_logger, L"Device started with device ID:%s", TOWS(m_strDeviceID));

    return true;
}

//连接父进程心跳服务
bool UDevCArmProc::StartHeartbeat()
{
    m_pConnHeartBeat = new IPCClientBase(ModuleName, "127.0.0.1", m_strHeartbeatPort);
    m_pConnHeartBeat->OnCmdRecv = std::bind(&UDevCArmProc::HeartbeatChecker, this, std::placeholders::_1);

    if (!m_pConnHeartBeat->Connect())
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to connect parent process at heartbeat port:%s", TOWS(m_strHeartbeatPort));
        return false;
    }

    LOG4CPLUS_INFO(g_logger, L"Parent process connected");
    return true;
}

//连接日志服务(包括获取日志端口)
bool UDevCArmProc::StartLogging()
{
    std::thread thd(&USubProcessBase::LoggingServerMonitorNew, this, ModuleName);
    thd.detach();

    return true;
}

//开始运行，如果此函数退出则进程退出
bool UDevCArmProc::Run()
{
    if (m_pDev != nullptr) delete m_pDev;

    //从设备信息服务获取设备配置
    IPCDeviceConfig devConfig;
    if (!GetDeviceConfig(m_strDeviceID, devConfig)) return false;

    //从设备信息服务原始影像存储路径
    std::string strRawImgDir = GetRawImageDir();
    LOG4CPLUS_INFO_FMT(g_logger, L"Raw image dir:%s", TOWS(strRawImgDir));

    //将设备信息复制到共享内存，界面进程生成DICOM时从共享内存获取设备信息
    GlobalMemMapping::SetDeviceInfo(devConfig.m_DevInfo);

    m_pDev = new UDevCArm(devConfig, strRawImgDir);
    using namespace std::placeholders;
    m_pDev->OnDevStatusChanged = std::bind(&UDevCArmProc::OnDevStatusChanged, this, std::placeholders::_1);
    m_pDev->OnImageAcquired = std::bind(&UDevCArmProc::OnImageAcquired, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);
    m_pDev->RequestService = std::bind(&USubProcessBase::RequestService, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_pDev->OnExpBtnChanged = std::bind(&UDevCArmProc::OnExpBtnChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_pDev->OnPedalChanged = std::bind(&UDevCArmProc::OnPedalChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_pDev->OnAxisStatusChanged = std::bind(&UDevCArmProc::OnAxisStatusChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_pDev->OnCArmCollimatorInPlace = std::bind(&UDevCArmProc::OnCArmCollimatorInPlace, this, std::placeholders::_1, std::placeholders::_2);
    m_pDev->OnCArmAxisManAutoChanged = std::bind(&UDevCArmProc::OnCArmAxisManAutoChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_pDev->OnCArmLaserStatusChanged = std::bind(&UDevCArmProc::OnCArmLaserStatusChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_pDev->OnCArmAxisPosChanged = std::bind(&UDevCArmProc::OnCArmAxisPosChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_pDev->OnCArmDevResetCompleted = std::bind(&UDevCArmProc::OnCArmDevResetCompleted, this, std::placeholders::_1);
    m_pDev->OnCArmRasterStatusChanged = std::bind(&UDevCArmProc::OnCArmRasterStatusChanged, this, _1);

    std::thread thd1(&UDevCArmProc::DevStatusUpdateThread, this);
    thd1.detach();

    std::thread thd2(&UDevCArmProc::ScanCommThread, this);
    thd2.detach();

    //等一会儿设备状态连接
    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    ErrorCode eRetCode = m_pDev->Init();
    if (IS_ERR(eRetCode))
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Device initated failed, error code:%04X", (int)eRetCode);
        OnDevStatusChanged(eRetCode);

        return false;
    }

    return true;
}

bool UDevCArmProc::IsAlive()
{
    return m_pConnHeartBeat != nullptr && m_pConnHeartBeat->IsConnected();
}

//设备连接状态监控
void UDevCArmProc::DevStatusUpdateThread()
{
    int nPort = MAX_USER_PORT + 1;

    while (IsAlive())
    {
        if(m_pConnDevStatus != nullptr && m_pConnDevStatus->IsConnected())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        //查询设备状态更新服务端口
        nPort = UPortManagerBase::PortQuery(ModuleName, m_strPortServerPort, PortName::DeviceStatus);
        if (nPort <= 0)
        {
            LOG4CPLUS_ERROR(g_logger, L"Failed to query device status server port");

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        LOG4CPLUS_INFO_FMT(g_logger, L"Device status port:%d", nPort);

        if (m_pConnDevStatus == nullptr)
        {
            m_pConnDevStatus = new IPCClientBase(ModuleName, "127.0.0.1", std::to_string(nPort));
            m_pConnDevStatus->OnCmdRecv = std::bind(&UDevCArmProc::OnDevStatusCmdRecv, this, std::placeholders::_1);
        }
        else
            m_pConnDevStatus->Disconnect();

        if (!m_pConnDevStatus->Connect(std::to_string(nPort)))
        {
            LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to connect device status server at port %d", nPort);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        m_pConnDevStatus->SendCommand(CMD::TO_CMD(CMD::DEVICE_ID, { m_strDeviceID, std::to_string(GetCurrentProcessId()) }));
        m_pConnDevStatus->SendCommand(CMD::TO_CMD(CArmIPCCMD::UPDATE_DEV_STATUS, { std::to_string((int)m_eLatestDevStatus) }));

        LOG4CPLUS_INFO_FMT(g_logger, L"Device status server connected at port %d", nPort);
    }
}

//工作站发来的设备消息处理
void UDevCArmProc::OnDevStatusCmdRecv(std::list<std::string> lstCmds)
{
    //for (auto cmd = lstCmds.begin(); cmd != lstCmds.end(); cmd++)
    //{
    //    LOG4CPLUS_INFO_FMT(g_logger, L"Device command received from UI process:%s", TOWS(*cmd));

    //    std::vector<std::string> subCmds = UCommonUtility::StringSplit(*cmd, CMD::CMD_SEPARATOR);
    //    if (subCmds[0] == CMD::DEVICE_STATUS_RESP)
    //    {

    //    }
    //}
}

//扫描通信连接状态监控
void UDevCArmProc::ScanCommThread()
{
    int nPort = MAX_USER_PORT + 1;

    while (IsAlive())
    {
        if (m_pConnScanComm != nullptr && m_pConnScanComm->IsConnected())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        //查询采集状态服务端口
        nPort = UPortManagerBase::PortQuery(ModuleName, m_strPortServerPort, PortName::AcqStatus);
        if (nPort <= 0)
        {
            LOG4CPLUS_ERROR(g_logger, L"Failed to query acq status server port");

            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        LOG4CPLUS_INFO_FMT(g_logger, L"Acq status port:%d", nPort);

        if (m_pConnScanComm == nullptr)
        {
            m_pConnScanComm = new IPCClientBase(ModuleName, "127.0.0.1", std::to_string(nPort));
            m_pConnScanComm->OnCmdRecv = std::bind(&UDevCArmProc::OnScanStatusCmdRecv, this, std::placeholders::_1);
        }
        else
            m_pConnScanComm->Disconnect();

        if (!m_pConnScanComm->Connect(std::to_string(nPort)))
        {
            LOG4CPLUS_ERROR(g_logger, L"Failed to connect scan status server");
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        }

        LOG4CPLUS_INFO_FMT(g_logger, L"Scan status server connected at port %d", nPort);
    }
}

//工作站发来的扫描消息处理
void UDevCArmProc::OnScanStatusCmdRecv(std::list<std::string> lstCmds)
{
    for (auto cmd = lstCmds.begin(); cmd != lstCmds.end(); cmd++)
    {
        LOG4CPLUS_INFO_FMT(g_logger, L"Command received from UI process:%s", TOWS(*cmd));

        std::vector<std::string> subCmds = UCommonUtility::StringSplit(*cmd, CMD::CMD_SEPARATOR);

        //开始2D扫描
        if (subCmds[0] == CArmIPCCMD::START_SCAN_2D)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::START_SCAN_2D_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::START_SCAN_2D_RESP, {std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY)}));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::START_SCAN_2D_RESP, { std::to_string((int)StartScan2D((DevWorkMode)std::stoi(subCmds[1]), std::stoi(subCmds[2]) != 0)) }));
        }
        //开始3D扫描
        else if (subCmds[0] == CArmIPCCMD::START_SCAN_3D)
        {
            if (subCmds.size() != 4)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::START_SCAN_3D_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::START_SCAN_3D_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                DevWorkMode wMode = (DevWorkMode)std::stoi(subCmds[1]);
                bool bXRay = std::stoi(subCmds[2]) != 0;
                OptionCArmMove mv = (OptionCArmMove)std::stoi(subCmds[3]);

                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::START_SCAN_3D_RESP, { std::to_string((int)StartScan3D(wMode, mv, bXRay)) }));
            }
        }
        //获取下位机工作状态
        else if (subCmds[0] == CArmIPCCMD::GET_SC_STATUS)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_SC_STATUS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_SC_STATUS_RESP, { std::to_string((int)m_pDev->GetScStatus()) }));
        }
        //设备复位
        else if (subCmds[0] == CArmIPCCMD::DEV_RESET)
        {
            if(m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::DEV_RESET_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::DEV_RESET_RESP, { std::to_string((int)m_pDev->ResetDevice()) }));
        }
        // 取消扫描
        else if (subCmds[0] == CArmIPCCMD::CANCEL_SCAN)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(subCmds[0]+"_RESP", { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::DEV_RESET_RESP, { std::to_string((int)m_pDev->Request(subCmds[0])) }));
        }
        //报警复位
        else if (subCmds[0] == CArmIPCCMD::WARN_RESET)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::WARN_RESET_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::WARN_RESET_RESP, { std::to_string((int)m_pDev->ResetWarn()) }));
        }
        //激光灯开关
        else if (subCmds[0] == CArmIPCCMD::LASER_SWITCH)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::LASER_SWITCH_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            OptionLaserSwitch opt = (OptionLaserSwitch)std::stoi(subCmds[1]);
            bool bOn = std::stoi(subCmds[2]) != 0;

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::LASER_SWITCH_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::LASER_SWITCH_RESP, { std::to_string((int)m_pDev->LaserSwitch(opt, bOn)) }));
        }
        //设置管电压
        else if (subCmds[0] == CArmIPCCMD::SET_KV)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_KV_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_KV_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_KV_RESP, { std::to_string((int)m_pDev->SetKV(std::stof(subCmds[1]))) }));
        }
        //设置管电流
        else if (subCmds[0] == CArmIPCCMD::SET_MA)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MA_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MA_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MA_RESP, { std::to_string((int)m_pDev->SetMA(std::stof(subCmds[1]))) }));
        }
        //设置采集帧率
        else if (subCmds[0] == CArmIPCCMD::SET_FPS)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_FPS_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_FPS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_FPS_RESP, { std::to_string((int)m_pDev->SetFPS(std::stoi(subCmds[1]))) }));
        }
        //设置单帧曝光时间
        else if (subCmds[0] == CArmIPCCMD::SET_EXPOSURE)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_EXPOSURE_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_EXPOSURE_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_EXPOSURE_RESP, { std::to_string((int)m_pDev->SetExposure(std::stoi(subCmds[1]))) }));
        }
        //设置采集帧数
        else if (subCmds[0] == CArmIPCCMD::SET_FRAME_NUM)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_FRAME_NUM_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_FRAME_NUM_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_FRAME_NUM_RESP, { std::to_string((int)m_pDev->SetFrameNum(std::stoi(subCmds[1]))) }));
        }
        //设置水平方向限束器位置
        else if (subCmds[0] == CArmIPCCMD::SET_HOR_COLL)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_HOR_COLL_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_HOR_COLL_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_HOR_COLL_RESP, { std::to_string((int)m_pDev->SetCollimator(OptionCollimator::Hor, std::stof(subCmds[1]), std::stof(subCmds[2]))) }));
        }
        //设置垂直方向限束器位置
        else if (subCmds[0] == CArmIPCCMD::SET_VER_COLL)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_VER_COLL_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_VER_COLL_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_VER_COLL_RESP, { std::to_string((int)m_pDev->SetCollimator(OptionCollimator::Ver, std::stof(subCmds[1]), std::stof(subCmds[2]))) }));
        }
        // 设置限束器连续运动
        else if (subCmds[0] == CArmIPCCMD::SET_COLL_CONTINUE_MOVE)
        {
            if (subCmds.size() != 4)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_COLL_CONTINUE_MOVE_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_COLL_CONTINUE_MOVE_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_COLL_CONTINUE_MOVE_RESP, { std::to_string(int(m_pDev->SetCollimatorContinueMove(OptionCollimator(stoi(subCmds[1])), OptionCollimatorMoveDirection(stoi(subCmds[2])), std::stof(subCmds[3]))) )}));
        }
        //升降柱连续运动的启停控制
        else if (subCmds[0] == CArmIPCCMD::MOVE_LIFT_COL)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_LIFT_COL_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_LIFT_COL_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_LIFT_COL_RESP, { std::to_string((int)m_pDev->MoveAxis((OptionLiftColMove)std::stoi(subCmds[1]), std::stof(subCmds[2]))) }));
        }
        //x轴连续运动的启停控制
        else if (subCmds[0] == CArmIPCCMD::MOVE_X_AXIS)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_X_AXIS_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_X_AXIS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_X_AXIS_RESP, { std::to_string((int)m_pDev->MoveAxis((OptionXAxisMove)std::stoi(subCmds[1]), std::stof(subCmds[2]))) }));
        }
        //C型臂连续运动的启停控制
        else if (subCmds[0] == CArmIPCCMD::MOVE_C_ARM)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_C_ARM_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_C_ARM_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::MOVE_C_ARM_RESP, { std::to_string((int)m_pDev->MoveAxis((OptionCArmMove)std::stoi(subCmds[1]), std::stof(subCmds[2]))) }));
        }
        //获取升降柱位置
        else if (subCmds[0] == CArmIPCCMD::GET_POS_LIFT_COL)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_LIFT_COL_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal = 0;
                ErrorCode res = m_pDev->GetPosAxis(CArmAxisDef::LiftingCol, fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_LIFT_COL_RESP, { std::to_string((int)res), std::to_string(fVal)}));
            }
        }
        //获取前后轴（x轴）位置
        else if (subCmds[0] == CArmIPCCMD::GET_POS_X_AXIS)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_X_AXIS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal = 0;
                ErrorCode res = m_pDev->GetPosAxis(CArmAxisDef::ForBackward, fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_X_AXIS_RESP, { std::to_string((int)res), std::to_string(fVal) }));
            }
        }
        //获取C型臂转动到指定位置
        else if (subCmds[0] == CArmIPCCMD::GET_POS_C_ARM)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_C_ARM_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal = 0;
                ErrorCode res = m_pDev->GetPosAxis(CArmAxisDef::CArm, fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_C_ARM_RESP, { std::to_string((int)res), std::to_string(fVal) }));
            }
        }
        //控制C型臂运动到指定位置
        else if (subCmds[0] == CArmIPCCMD::SET_POS_C_ARM)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_C_ARM_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_C_ARM_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->SetPosAxis(CArmAxisDef::CArm, std::stof(subCmds[1]), std::stof(subCmds[2]));
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_C_ARM_RESP, { std::to_string((int)res) }));
            }
        }
        //控制前后轴（x轴）运动到指定位置
        else if (subCmds[0] == CArmIPCCMD::SET_POS_X_AXIS)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_X_AXIS_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_X_AXIS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->SetPosAxis(CArmAxisDef::ForBackward, std::stof(subCmds[1]), std::stof(subCmds[2]));
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_X_AXIS_RESP, { std::to_string((int)res) }));
            }
        }
        //控制升降柱运动到指定位置
        else if (subCmds[0] == CArmIPCCMD::SET_POS_LIFT_COL)
        {
            if (subCmds.size() != 3)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_LIFT_COL_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_LIFT_COL_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->SetPosAxis(CArmAxisDef::LiftingCol, std::stof(subCmds[1]), std::stof(subCmds[2]));
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_POS_LIFT_COL_RESP, { std::to_string((int)res) }));
            }
        }
        //使能C型臂人工转动
        else if (subCmds[0] == CArmIPCCMD::SET_MAN_C_ARM)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_C_ARM_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_C_ARM_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->SetManuAxis(CArmAxisDef::CArm, (CArmAxisCtrlDef)std::stoi(subCmds[1]));
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_C_ARM_RESP, { std::to_string((int)res) }));
            }
        }
        //使能升降柱运动人工升降
        else if (subCmds[0] == CArmIPCCMD::SET_MAN_LIFT_COL)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_LIFT_COL_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_LIFT_COL_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->SetManuAxis(CArmAxisDef::LiftingCol, (CArmAxisCtrlDef)std::stoi(subCmds[1]));
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_LIFT_COL_RESP, { std::to_string((int)res) }));
            }
        }
        //使能前后轴（x轴）人工移动
        else if (subCmds[0] == CArmIPCCMD::SET_MAN_X_AXIS)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_X_AXIS_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_X_AXIS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->SetManuAxis(CArmAxisDef::ForBackward, (CArmAxisCtrlDef)std::stoi(subCmds[1]));
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_MAN_X_AXIS_RESP, { std::to_string((int)res) }));
            }
        }
        //获取DAP当前读数
#if 0
        else if (subCmds[0] == CArmIPCCMD::GET_DAP_VALUE)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_DAP_VALUE_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal = 0;
                ErrorCode res = m_pDev->GetDAPValue(fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_DAP_VALUE_RESP, { std::to_string((int)res), std::to_string(fVal) }));
            }
        }
#endif
        //设置3D扫描运动轨迹
        else if (subCmds[0] == CArmIPCCMD::SET_3D_TRACK_DATA)
        {
            if (m_pDev == nullptr)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_3D_TRACK_DATA_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
                return;
            }

            if (subCmds.size() < 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_3D_TRACK_DATA_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            int nNum = std::stoi(subCmds[1]);//轨迹点个数
            if (subCmds.size() != (3 * nNum + 2))//每个轨迹点由xyz轴的位置组成
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_3D_TRACK_DATA_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            float *pPoints = new float[3 * nNum];
            for (int i = 2; i < (3 * nNum + 2); i++)
            {
                pPoints[i - 2] = std::stof(subCmds[i]);
            }

            m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SET_3D_TRACK_DATA_RESP, { std::to_string((int)m_pDev->Set3DTrack(nNum, pPoints)) }));

            delete[] pPoints;
        }
        //获取上次3D扫描记录的实际轨迹
        else if (subCmds[0] == CArmIPCCMD::FETCH_3D_TRACK)
        {
            if (m_pDev == nullptr)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::FETCH_3D_TRACK_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY), "0" }));
                return;
            }

            WORD nNum = 0;
            float *pfPoints = nullptr;

            ErrorCode err = m_pDev->Get3DTrack(nNum, &pfPoints);
            if (IS_ERR(err))
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::FETCH_3D_TRACK_RESP, { std::to_string((int)err), "0" }));
                return;
            }

            std::vector<std::string> vals;
            vals.push_back(std::to_string((int)err));
            vals.push_back(std::to_string(nNum));

            for (int i = 0; i < nNum * 3; i++)
            {
                vals.push_back(std::to_string(pfPoints[i]));
            }

            m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::FETCH_3D_TRACK_RESP, vals));
        }
        //启用/禁用DAP
        else if (subCmds[0] == CArmIPCCMD::ENABLE_DAP)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::ENABLE_DAP_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::ENABLE_DAP_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->EnableDAP(std::stoi(subCmds[1]) != 0);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::ENABLE_DAP_RESP, { std::to_string((int)res) }));
            }
        }
        //查询当前管电压
        else if (subCmds[0] == CArmIPCCMD::GET_KV)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_KV_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal;
                ErrorCode res = m_pDev->GetKV(fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_KV_RESP, { std::to_string((int)res), std::to_string(fVal) }));
            }
        }
        //查询当前管电流
        else if (subCmds[0] == CArmIPCCMD::GET_MA)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_MA_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal;
                ErrorCode res = m_pDev->GetMA(fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_MA_RESP, { std::to_string((int)res), std::to_string(fVal) }));
            }
        }
        // 获取当前限束器位置
        else if (subCmds[0] == CArmIPCCMD::GET_POS_COLLIMATOR)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_COLLIMATOR_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal;
                auto res = m_pDev->GetCollPos(OptionCollimator(std::stoi(subCmds[1])), fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_POS_COLLIMATOR_RESP, { std::to_string((int)res), std::to_string(fVal) }));
            }
        }
        // 允许/不允许下位机放射线
        else if (subCmds[0] == CArmIPCCMD::RADIARION_PERMISSION_CTRL)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::RADIARION_PERMISSION_CTRL_RESP, { std::to_string(int(ErrorCode::ERR_3IN1_DEV_NOT_READY)) }));
            else
            {
                auto res = m_pDev->RadiationPermission(1 == std::stoi(subCmds[1]));
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::RADIARION_PERMISSION_CTRL_RESP, { std::to_string((int)res) }));
            }
        }
        //查询当前球管温度
//         else if (subCmds[0] == CArmIPCCMD::GET_Bulb_Tube_Temp)
//         {
//             if (m_pDev == nullptr)
//                 m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_Bulb_Tube_Temp_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
//             else
//             {
//                 float temp;
//                 ErrorCode res = m_pDev->GetBulbTubeTemp(temp);
//                 m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_Bulb_Tube_Temp_RESP, { std::to_string(int(res)), std::to_string(temp) }));
//             }
//         }
        // 查询:当前球管温度|电池电量
        else if ((subCmds[0] == CArmIPCCMD::GET_BATTERY_PERCENT)
            || (subCmds[0] == CArmIPCCMD::GET_Bulb_Tube_Temp)
            || (subCmds[0] == CArmIPCCMD::GET_RASTER_STATUS)
            || (subCmds[0] == CArmIPCCMD::GET_DAP_VALUE))
        {
            const auto respStr = subCmds[0] + "_RESP";

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(respStr, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                //std::string ret;
                //ErrorCode res;
                std::vector<std::string> arg;
                if (subCmds[0] == CArmIPCCMD::GET_RASTER_STATUS)
                {
                    std::uint8_t val{};
                    auto res = m_pDev->Get(respStr, val);
                    arg.push_back(std::to_string(int(res)));
                    arg.push_back(std::to_string(val));
                }
                else if (subCmds[0] == CArmIPCCMD::GET_DAP_VALUE)
                {
                    DAPValue val{};
                    auto res = m_pDev->Get(respStr, val);
                    arg.push_back(std::to_string(int(res)));
                    arg.push_back(std::to_string(val.Gy_per_min));
                    arg.push_back(std::to_string(val.Gy));
                    arg.push_back(std::to_string(val.DAP));
                }
                else
                {
                    float val{};
                    auto res = m_pDev->Get(respStr, val);
                    arg.push_back(std::to_string(int(res)));
                    arg.push_back(std::to_string(val));
                }
                m_pConnScanComm->SendCommand(CMD::TO_CMD((subCmds[0] + "_RESP"), arg));
            }
        }
        // 下位机拍片控制
        else if (CArmIPCCMD::SLAVE_SCAN_CTRL == subCmds[0])
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD((subCmds[0] + "_RESP"), { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                auto res = m_pDev->SlaveScanCtrl(SlaveScanCtrlType(std::stoi(subCmds[1])));
                m_pConnScanComm->SendCommand(CMD::TO_CMD((subCmds[0] + "_RESP"), { std::to_string(int(res)) }));
            }
        }
        //查询十字线激光灯开关状态
        else if (subCmds[0] == CArmIPCCMD::GET_CROSS_LINE_LSR_STS)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_CROSS_LINE_LSR_STS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                bool bOn = false;
                float fVal = 0;
                ErrorCode res = m_pDev->GetLaserStatus( OptionLaserSwitch::CrossCurve, bOn, fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_CROSS_LINE_LSR_STS_RESP, { std::to_string((int)res), std::to_string(bOn), std::to_string(fVal) }));
            }
        }
        //查询视野范围激光灯开关状态
        else if (subCmds[0] == CArmIPCCMD::GET_VIEW_RANGE_LSR_STS)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_VIEW_RANGE_LSR_STS_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                bool bOn = false;
                float fVal = 0;
                ErrorCode res = m_pDev->GetLaserStatus(OptionLaserSwitch::ViewRange, bOn, fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_VIEW_RANGE_LSR_STS_RESP, { std::to_string((int)res), std::to_string(bOn), std::to_string(fVal) }));
            }
        }
        //查询设备主板温度
        else if (subCmds[0] == CArmIPCCMD::GET_MB_TEMP)
        {
            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_MB_TEMP_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                float fVal = 0;
                ErrorCode res = m_pDev->GetMainboardTemp(fVal);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::GET_MB_TEMP_RESP, { std::to_string((int)res), std::to_string(fVal) }));
            }
        }
        //暂停/恢复球管控制
        else if (subCmds[0] == CArmIPCCMD::ENABLE_TUBE_CTRL)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::ENABLE_TUBE_CTRL_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            if (m_pDev == nullptr)
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::ENABLE_TUBE_CTRL_RESP, { std::to_string((int)ErrorCode::ERR_3IN1_DEV_NOT_READY) }));
            else
            {
                ErrorCode res = m_pDev->EnableTubeCtrl(std::stoi(subCmds[1]) != 0);
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::ENABLE_TUBE_CTRL_RESP, { std::to_string((int)res) }));
            }
        }
        //发送影像消息的反馈
        else if (subCmds[0] == CArmIPCCMD::SEND_NEW_IMAGE_RESP)
        {
            if (subCmds.size() != 2)
            {
                m_pConnScanComm->SendCommand(CMD::TO_CMD(CMD::ERR_RESP, { std::to_string((int)ErrorCode::ERR_GENERAL_WRONG_CMD_ARG_NUM) }));
                return;
            }

            std::lock_guard<std::mutex> lg(m_mtxMemMap);
            auto iter = m_ImgMemMap.find(subCmds[1]);
            if (iter != m_ImgMemMap.end())
            {
                CloseHandle(iter->second);
                m_ImgMemMap.erase(iter);
            }

            break;
        }
        else if (IsIgnoredCmd(subCmds[0]))
        {
            break;
        }
        else
        {
            LOG4CPLUS_ERROR_FMT(g_logger, L"Unknwn command:%s", TOWS(subCmds[0]));
            m_pConnScanComm->SendCommand(CMD::TO_CMD(CMD::ERR_RESP, { "Unknwn command:" + subCmds[0] }));
        }
    }
}

//根据设备ID，从Launcher进程获取设备配置
bool UDevCArmProc::GetDeviceConfig(const std::string &strDevID, IPCDeviceConfig &devConfig)
{
    std::string strResp;
    if (!RequestService(PortName::SysConfigInfoQuery, CMD::TO_CMD(CMD::DEV_INFO_QUERY, { std::to_string(GetCurrentProcessId()), strDevID }), strResp))
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to query device info");
        OnDevStatusChanged(ErrorCode::ERR_3IN1_QUERY_DEV_INFO);
        return false;
    }

    //解析服务器返回的消息
    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);

    if (lstSubCmds[0] == CMD::DEV_INFO_QUERY_RESP)
    {
        bool b = UCommonUtility::CopyFromMappedMemory((HANDLE)strtoull(lstSubCmds[1].c_str(), nullptr, 10), sizeof(IPCDeviceConfig), &devConfig);
        if (!b)
        {
            LOG4CPLUS_ERROR_FMT(g_logger, L"%s", TOWS(UCommonUtility::GetLastError()));
            return false;
        }

        LOG4CPLUS_INFO_FMT(g_logger, L"Device ID:%s", TOWS(devConfig.m_DevInfo.m_strDevID));
        return true;
    }
    else if (lstSubCmds[0] == CMD::ERR_RESP)
        LOG4CPLUS_ERROR(g_logger, L"Error received from device info server");
    else
        LOG4CPLUS_ERROR(g_logger, L"Unrecognized response from device info server");

    OnDevStatusChanged(ErrorCode::ERR_3IN1_QUERY_DEV_INFO);
    return false;
}

std::string UDevCArmProc::GetRawImageDir()
{
    std::string strResp;
    if (!RequestService(PortName::SysConfigInfoQuery, CMD::TO_CMD(CMD::GET_RAW_IMG_DIR, {}), strResp))
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to query raw image dir, use temp dir in exe dir");
        return UCommonUtility::GetExeDir() + "\\Temp";
    }

    auto lstSubCmds = UCommonUtility::StringSplit(strResp, CMD::CMD_SEPARATOR);
    if (lstSubCmds[0] == CMD::GET_RAW_IMG_DIR_RESP)
        return lstSubCmds[1];
    else
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to query raw image dir, use temp dir in exe dir");
        return UCommonUtility::GetExeDir() + "\\Temp";
    }
}

ErrorCode UDevCArmProc::SetScanParam()
{
    CT_Range rng;
    GlobalMemMapping::GetScanConfigXML(rng);

    ErrorCode res = m_pDev->SetKV(rng.fKV[0]);
    if (IS_ERR(res)) return res;

    res = m_pDev->SetMA(rng.fMA[0]);
    if (IS_ERR(res)) return res;

    res = m_pDev->SetFPS(rng.nFPS);
    if (IS_ERR(res)) return res;

    res = m_pDev->SetFrameNum(rng.nFrameNum);
    if (IS_ERR(res)) return res;

    res = m_pDev->SetExposure(rng.nExposure[0]);
    if (IS_ERR(res)) return res;

    return res;
}

ErrorCode UDevCArmProc::StartScan2D(DevWorkMode workMode, bool bXRay)
{
    ScanType scanMode = ScanType::CT_Scan;
    GlobalMemMapping::SetScanMode(scanMode);

    // 正常模式下，将共享内存中的扫描参数批量设置到下位机
    if (workMode == DevWorkMode::Normal)
    {
        ErrorCode res = SetScanParam();
        if (IS_ERR(res)) return res;
    }

    return m_pDev->StartScan2D(bXRay);
}

ErrorCode UDevCArmProc::StartScan3D(DevWorkMode workMode, OptionCArmMove opt, bool bXRay)
{
    ScanType scanMode = ScanType::CT_Scan;
    GlobalMemMapping::SetScanMode(scanMode);

    // 正常模式下，将共享内存中的扫描参数批量设置到下位机
    if (workMode == DevWorkMode::Normal)
    {
        ErrorCode res = SetScanParam();
        if (IS_ERR(res)) return res;
    }

    return m_pDev->StartScan3D(opt, bXRay);
}

#pragma region 设备对象回调函数

//设备状态变化的通知消息
void UDevCArmProc::OnDevStatusChanged(ErrorCode c)
{
    m_eLatestDevStatus = c;

    if (m_pConnDevStatus == nullptr || !m_pConnDevStatus->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to update device status due to no device status connection");
        return;
    }

    m_pConnDevStatus->SendCommand(CMD::TO_CMD(CArmIPCCMD::UPDATE_DEV_STATUS, {std::to_string((int)c)}));
}

//从探测器采集到影像
void UDevCArmProc::OnImageAcquired(int nIdx, const unsigned short *pImgData, int nImgWid, int nImgHgt, bool bLossed)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send new image due to no acq status connection");
        return;
    }

    std::string strMemMapName = UCommonUtility::GetNewGUID();
    HANDLE hMemMap = UCommonUtility::CreateNewMappedMemory(strMemMapName, nImgWid * nImgHgt * sizeof(unsigned short), pImgData);

    std::thread thd([this, strMemMapName, hMemMap, nIdx, nImgWid, nImgHgt]
    {
        {
            std::lock_guard<std::mutex> lg(m_mtxMemMap);
            m_ImgMemMap[strMemMapName] = hMemMap;
        }

        std::vector<std::string> vecArg;
        vecArg.push_back(std::to_string(nIdx));
        vecArg.push_back(std::to_string(nImgWid));
        vecArg.push_back(std::to_string(nImgHgt));
        vecArg.push_back(strMemMapName);

        m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::SEND_NEW_IMAGE, vecArg));
    });

    thd.detach();
}

//曝光手闸按下状态变化处理函数
void UDevCArmProc::OnExpBtnChanged(CArmExpBtnDef eBtnStep, bool bPressed)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send exposure button status due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::EXP_BTN_PRESSED, { std::to_string((int)eBtnStep), std::to_string(bPressed) }));
}

//脚踏板踏下状态变化处理函数
void UDevCArmProc::OnPedalChanged(CArmPedalDef eBtnStep, CArmExpBtnDef level, bool bPressed)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send pedal status due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::PEDAL_PRESSED, { std::to_string((int)eBtnStep), std::to_string(int(level)), std::to_string(bPressed) }));
}

//各轴启停变化处理函数
void UDevCArmProc::OnAxisStatusChanged(CArmAxisDef eAxis, CArmAxisMotionDef eStatus)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send axis new status due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::AXIS_MOTION_STATUS_CHGD, { std::to_string((int)eAxis), std::to_string((int)eStatus) }));
}

//限束器运动到位消息处理函数
void UDevCArmProc::OnCArmCollimatorInPlace(OptionCollimator coll, float fPos)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send collimator status due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::COLLIMATOR_IN_PLACE, { std::to_string((int)coll), std::to_string(fPos) }));
}

//各轴自动手动状态变化处理函数
void UDevCArmProc::OnCArmAxisManAutoChanged(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send axis auto/manu status due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::AXIS_OPER_MODE_CHGD, { std::to_string((int)eAxis), std::to_string((int)eStatus) }));
}

//激光灯开关状态变化处理函数
void UDevCArmProc::OnCArmLaserStatusChanged(OptionLaserSwitch eLaser, bool bOn)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send new laser status due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::LASER_SWITCH_CHGD, { std::to_string((int)eLaser), std::to_string(bOn) }));
}

void UDevCArmProc::OnCArmRasterStatusChanged(const bool open_)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send new laser status due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::RASTER_STATUS_CHGD, { std::to_string(open_) }));
}

//各轴位置变化时发出的实时位置信息处理函数
void UDevCArmProc::OnCArmAxisPosChanged(CArmAxisDef eAxis, float fPos)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send axis new pos due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::AXIS_POS_CHGD, { std::to_string((int)eAxis), std::to_string(fPos) }));
}

//设备复位完成后的处理函数
void UDevCArmProc::OnCArmDevResetCompleted(ErrorCode res)
{
    if (m_pConnScanComm == nullptr || !m_pConnScanComm->IsConnected())
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to send device reset result due to no acq status connection");
        return;
    }

    m_pConnScanComm->SendCommand(CMD::TO_CMD(CArmIPCCMD::DEV_RESET_DONE, { std::to_string((int)res) }));
}

#pragma endregion

//检查消息是否不需要处理
bool UDevCArmProc::IsIgnoredCmd(const std::string &strCmd)
{
    static std::map<std::string, int> cmdMap = 
    {
        { CArmIPCCMD::EXP_BTN_PRESSED_RESP, 0},
        { CArmIPCCMD::PEDAL_PRESSED_RESP, 0 },
        { CArmIPCCMD::AXIS_MOTION_STATUS_CHGD_RESP, 0 },
        { CArmIPCCMD::COLLIMATOR_IN_PLACE_RESP, 0 },
        { CArmIPCCMD::AXIS_OPER_MODE_CHGD_RESP, 0 },
        { CArmIPCCMD::LASER_SWITCH_CHGD_RESP, 0 },
        { CArmIPCCMD::AXIS_POS_CHGD_RESP, 0 },
        { CArmIPCCMD::DEV_RESET_DONE_RESP, 0 }
    };

    return cmdMap.find(strCmd) != cmdMap.end();
}
