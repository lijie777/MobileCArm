#include "CArmDeviceComm.h"
#include <log4cplus/loggingmacros.h>
#include <regex>

using namespace log4cplus;
using namespace log4cplus::helpers;

extern Logger g_logger;

#ifdef C_ARM_WORK_STATION
const std::string USubProcessBase::ModuleName = "CArmWorkStationProc";
const std::string USubProcessBase::LogHeader = "[CArmWorkStationProc]";
#else
const std::string USubProcessBase::ModuleName = "MobileCArmTest";
const std::string USubProcessBase::LogHeader = "[MobileCArmTest]";
#endif

UCArmDeviceComm::UCArmDeviceComm()
{
    m_pHBConn = nullptr;
    m_pDevStatusServer = nullptr;
    m_pScanInfoServer = nullptr;

    m_nDevStatusPort = -1;
    m_nScanInfoPort = -1;
}

UCArmDeviceComm::~UCArmDeviceComm()
{
    Uninitialize();
}

#pragma region 父类虚函数实现
//解析输入参数
bool UCArmDeviceComm::ArgsParse(int argc, char *argv[])
{
    //检查启动参数数量
    if (argc != 3)
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Wrong input argument count:%d, should be %d", argc - 1, 2);
        return false;
    }

#pragma region 第一个参数:向Launcher进程发送心跳包端口

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

#pragma region 第二个参数:端口服务端口
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

    return true;
}

//连接父进程心跳服务
bool UCArmDeviceComm::StartHeartbeat()
{
    if (m_strHeartbeatPort.empty()) return false;

    m_pHBConn = new IPCClientBase(ModuleName, "127.0.0.1", m_strHeartbeatPort);
    m_pHBConn->OnCmdRecv = std::bind(&UCArmDeviceComm::HeartbeatParser, this, std::placeholders::_1);

    if (!m_pHBConn->Connect())
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to connect parent process at heartbeat port:%s", TOWS(m_strHeartbeatPort));
        return false;
    }

    LOG4CPLUS_INFO(g_logger, L"Parent process connected");

    std::thread thd(&UCArmDeviceComm::HBCheckThread, this);
    thd.detach();

    return true;
}

//连接日志服务(包括获取日志端口)
bool UCArmDeviceComm::StartLogging()
{
    std::thread thd(&USubProcessBase::LoggingServerMonitorNew, this, ModuleName);
    thd.detach();

    return true;
}

//获取服务端口（包括请求分配自身提供的服务和其他进程提供的服务（不包含日志服务））
bool UCArmDeviceComm::GetServicePorts()
{
    if (m_strPortServerPort.empty()) return false;

    m_nDevStatusPort = UPortManagerBase::PortAllocReq(ModuleName, m_strPortServerPort, PortName::DeviceStatus);
    if (m_nDevStatusPort < MIN_USER_PORT || m_nDevStatusPort > MAX_USER_PORT)
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to allocate device status server port");

        return false;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Device status server port:%d", m_nDevStatusPort);

    m_nScanInfoPort = UPortManagerBase::PortAllocReq(ModuleName, m_strPortServerPort, PortName::AcqStatus);
    if (m_nScanInfoPort < MIN_USER_PORT || m_nScanInfoPort > MAX_USER_PORT)
    {
        LOG4CPLUS_ERROR(g_logger, L"Failed to allocate acq status server port");

        return false;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Scan info server port:%d", m_nScanInfoPort);

    return true;
}

//启动自身提供的服务
bool UCArmDeviceComm::StartServices()
{
    if (m_nDevStatusPort < MIN_USER_PORT || m_nDevStatusPort > MAX_USER_PORT)
    {
        LOG4CPLUS_ERROR(g_logger, L"Can't start device status server: invalid server port");
        return false;
    }

    //启动设备状态服务
    if (m_pDevStatusServer != nullptr)
    {
        m_pDevStatusServer->Shutdown();
        delete m_pDevStatusServer;
    }

    m_pDevStatusServer = new UDeivceStatusServer(std::to_string(m_nDevStatusPort), true);

    if (!m_pDevStatusServer->SetupServer())
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to setup device status server at port:%d", m_nDevStatusPort);
        return false;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Device status server started at port %d", m_nDevStatusPort);

    m_pDevStatusServer->OnDevStatusChanged = [this](const ErrorCode code_)
    {
        if (OnDevStatusChanged)
        {
            OnDevStatusChanged(code_);
        }
    };

    //启动扫描信息服务
    if (m_nScanInfoPort < MIN_USER_PORT || m_nScanInfoPort > MAX_USER_PORT)
    {
        LOG4CPLUS_ERROR(g_logger, L"Can't start acq status server: invalid server port");
        return false;
    }

    if (m_pScanInfoServer != nullptr)
    {
        m_pScanInfoServer->Shutdown();
        delete m_pScanInfoServer;
    }

    m_pScanInfoServer = new UScanInfoServer(std::to_string(m_nScanInfoPort), true);

    m_pScanInfoServer->OnExpBtnChanged = OnExpBtnChanged;
    m_pScanInfoServer->OnPedalChanged = OnPedalChanged;
    m_pScanInfoServer->OnAxisStatusChanged = OnAxisStatusChanged;
    m_pScanInfoServer->OnCArmCollimatorInPlace = OnCArmCollimatorInPlace;
    m_pScanInfoServer->OnCArmAxisManAutoChanged = OnCArmAxisManAutoChanged;
    m_pScanInfoServer->OnCArmLaserStatusChanged = OnCArmLaserStatusChanged;
    m_pScanInfoServer->OnCArmAxisPosChanged = OnCArmAxisPosChanged;
    m_pScanInfoServer->OnCArmDevResetCompleted = OnCArmDevResetCompleted;
    m_pScanInfoServer->OnCArmRasterStatusChanged = OnCArmRasterStatusChanged;;

    m_pScanInfoServer->OnNewImageReceived = OnNewImageReceived;

    using namespace std::placeholders;
    StartScan2D = std::bind(&UScanInfoServer::StartScan2D, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    StartScan3D = std::bind(&UScanInfoServer::StartScan3D, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    GetScStatus = std::bind(&UScanInfoServer::GetScStatus, m_pScanInfoServer);
    ResetDevice = std::bind(&UScanInfoServer::ResetDevice, m_pScanInfoServer);
    ResetWarn = std::bind(&UScanInfoServer::ResetWarn, m_pScanInfoServer);
    LaserSwitch = std::bind(&UScanInfoServer::LaserSwitch, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    SetKV = std::bind(&UScanInfoServer::SetKV, m_pScanInfoServer, std::placeholders::_1);
    SetMA = std::bind(&UScanInfoServer::SetMA, m_pScanInfoServer, std::placeholders::_1);
    SetFPS = std::bind(&UScanInfoServer::SetFPS, m_pScanInfoServer, std::placeholders::_1);
    SetExposure = std::bind(&UScanInfoServer::SetExposure, m_pScanInfoServer, std::placeholders::_1);
    SetFrameNum = std::bind(&UScanInfoServer::SetFrameNum, m_pScanInfoServer, std::placeholders::_1);
    SetCollimator = std::bind(&UScanInfoServer::SetCollimator, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    MoveAxisZ = std::bind(&UScanInfoServer::MoveAxisZ, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    MoveAxisX = std::bind(&UScanInfoServer::MoveAxisX, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    MoveAxisY = std::bind(&UScanInfoServer::MoveAxisY, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    GetPosAxis = std::bind(&UScanInfoServer::GetPosAxis, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    SetPosAxis = std::bind(&UScanInfoServer::SetPosAxis, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    SetManuAxis = std::bind(&UScanInfoServer::SetManuAxis, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    GetDAPValue = std::bind(&UScanInfoServer::GetDAPValue, m_pScanInfoServer, std::placeholders::_1);
    EnableDAP = std::bind(&UScanInfoServer::EnableDAP, m_pScanInfoServer, std::placeholders::_1);
    GetKV = std::bind(&UScanInfoServer::GetKV, m_pScanInfoServer, std::placeholders::_1);
    GetMA = std::bind(&UScanInfoServer::GetMA, m_pScanInfoServer, std::placeholders::_1);
    GetLaserStatus = std::bind(&UScanInfoServer::GetLaserStatus, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    GetMainboardTemp = std::bind(&UScanInfoServer::GetMainboardTemp, m_pScanInfoServer, std::placeholders::_1);
    Set3DTrack = std::bind(&UScanInfoServer::Set3DTrack, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    Get3DTrack = std::bind(&UScanInfoServer::Get3DTrack, m_pScanInfoServer, std::placeholders::_1, std::placeholders::_2);
    EnableTubeCtrl = std::bind(&UScanInfoServer::EnableTubeCtrl, m_pScanInfoServer, std::placeholders::_1);

    if (!m_pScanInfoServer->SetupServer())
    {
        LOG4CPLUS_ERROR_FMT(g_logger, L"Failed to setup scan info server at port:%d", m_nScanInfoPort);
        return false;
    }

    LOG4CPLUS_INFO_FMT(g_logger, L"Scan info server started at port %d", m_nScanInfoPort);

    return true;
}

#pragma endregion

void UCArmDeviceComm::Uninitialize()
{
    OnHBDisconnected = nullptr;
    OnDevStatusChanged = nullptr;
    OnExpBtnChanged = nullptr;
    OnPedalChanged = nullptr;
    OnAxisStatusChanged = nullptr;
    OnCArmCollimatorInPlace = nullptr;
    OnCArmAxisManAutoChanged = nullptr;
    OnCArmLaserStatusChanged = nullptr;
    OnCArmAxisPosChanged = nullptr;
    OnCArmDevResetCompleted = nullptr;
    OnNewImageReceived = nullptr;

    if (m_pScanInfoServer != nullptr)
    {
        m_pScanInfoServer->OnExpBtnChanged = nullptr;
        m_pScanInfoServer->OnPedalChanged = nullptr;
        m_pScanInfoServer->OnAxisStatusChanged = nullptr;
        m_pScanInfoServer->OnCArmCollimatorInPlace = nullptr;
        m_pScanInfoServer->OnCArmAxisManAutoChanged = nullptr;
        m_pScanInfoServer->OnCArmLaserStatusChanged = nullptr;
        m_pScanInfoServer->OnCArmAxisPosChanged = nullptr;
        m_pScanInfoServer->OnCArmDevResetCompleted = nullptr;
        m_pScanInfoServer->OnNewImageReceived = nullptr;
    }

    if(m_pDevStatusServer != nullptr) m_pDevStatusServer->OnDevStatusChanged = nullptr;

    if (m_pHBConn != nullptr)
    {
        IPCClientBase *p = m_pHBConn;
        m_pHBConn = nullptr;

        p->Disconnect();

        delete p;
    }

    if (m_pDevStatusServer != nullptr)
    {
        m_pDevStatusServer->Shutdown();

        delete m_pDevStatusServer;
        m_pDevStatusServer = nullptr;
    }

    if (m_pScanInfoServer != nullptr)
    {
        m_pScanInfoServer->Shutdown();

        delete m_pScanInfoServer;
        m_pScanInfoServer = nullptr;
    }
}

void UCArmDeviceComm::HBCheckThread()
{
    while (true)
    {
        if (m_pHBConn == nullptr || !m_pHBConn->IsConnected())
        {
            if(OnHBDisconnected) OnHBDisconnected();
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

void UCArmDeviceComm::ExitApp()
{
    if (m_pHBConn == nullptr || !m_pHBConn->IsConnected()) return;

    m_pHBConn->SendCommand(CMD::TO_CMD(CMD::HB_SUB_PROC_END, { std::to_string(GetCurrentProcessId()), std::to_string((int)SubProcEndOption::ExitSystem) }));
}