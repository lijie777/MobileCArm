#include "StationCommManager.h"
#include "CArmScCmdDef.h"
#include "Common/WorkStationMsgCtrl.h"


StationCommManager::StationCommManager(CArmWorkStationProc* proc)
    : m_pProcInst(proc)
    , m_pStatus(std::shared_ptr<StationManagerStatus>(new CommManagerLogoutStatus(this, std::shared_ptr<StationManagerStatus>(new CommManagerIdleStatus(this)))))
{
    // 注册到列表，接收指令
    CArmObject::registerObject(this);

    if (m_pProcInst != nullptr)
    {
        std::function<void(const CArmCmdBase*)> regsFunc = std::bind(&StationCommManager::deviceCommEvent, this, std::placeholders::_1);
        WorkStationMsgCtrl::GetInstance()->RegisterObserver(ClientType::WORKSTATION_GUI, regsFunc);
    }
    initLogic();
}


StationCommManager::~StationCommManager()
{
}




void StationCommManager::deviceCommEvent(const CArmCmdBase * cmdObject)
{
    CArmCmdBase* cmd = nullptr;
    // 激光灯状态变化
    if (cmdObject->Type() == SC_CARM_LSR_STS_CHANGED_CROSS)
    {
        const CArmCmdLaserStatusChangedCross* temp = static_cast<const CArmCmdLaserStatusChangedCross*>(cmdObject);
        cmd = new CArmCmdLaserStatusChangedCross{ temp->m_arg };
    }
    // 激光灯状态变化
    else if (cmdObject->Type() == SC_CARM_LSR_STS_CHANGED_VIEW)
    {
        const CArmCmdLaserStatusChangedCross* temp = static_cast<const CArmCmdLaserStatusChangedCross*>(cmdObject);
        cmd = new CArmCmdLaserStatusChangedCross{ temp->m_arg };
    }
    else if (cmdObject->Type() == WS_CARM_TP_WS_NET_STATUS)
    {
        const CArmCmdTpWsNetStatus* temp = static_cast<const CArmCmdTpWsNetStatus*>(cmdObject);
        cmd = new CArmCmdTpWsNetStatus{ temp->m_arg };
    }
    else if (cmdObject->Type() == WS_CARM_IPC_DEV_STATUS)
    {
        const CArmCmdDevStatusChanged* temp = static_cast<const CArmCmdDevStatusChanged*>(cmdObject);
        cmd = new CArmCmdDevStatusChanged{ temp->m_arg };
    }
    else if (cmdObject->Type() == WS_CARM_IPC_DEVICE_VALUE)
    {
        const CArmCmdDeviceValue* temp = static_cast<const CArmCmdDeviceValue*>(cmdObject);
        cmd = new CArmCmdDeviceValue{ temp->m_arg };
    }
    else if (cmdObject->Type() == WS_CARM_IMG_ADJUST_PARAM)
    {
        const CArmCmdImgAdjustParam* temp = static_cast<const CArmCmdImgAdjustParam*>(cmdObject);
        cmd = new CArmCmdImgAdjustParam{ temp->m_arg };
    }
    else if (cmdObject->Type() == WS_CARM_SET_SCAN_PARAM)
    {
        const CArmCmdScanParam* temp = static_cast<const CArmCmdScanParam*>(cmdObject);
        cmd = new CArmCmdScanParam{ temp->m_arg };
    }
    else if (cmdObject->Type() == SC_CARM_EXP_BTN1_PRESSED)
    {
        const CArmCmdExpBtn1Pressed* temp = static_cast<const CArmCmdExpBtn1Pressed*>(cmdObject);
        cmd = new CArmCmdExpBtn1Pressed{ temp->m_arg };
    }
    else if (cmdObject->Type() == WS_CARM_NEW_IMG_RECEIVED)
    {
        const NewImgReceived* temp = static_cast<const NewImgReceived*>(cmdObject);
        cmd = new NewImgReceived{ temp->m_imgId, temp->m_idx, temp->m_w, temp->m_h, temp->m_img, temp->m_windowWidth, temp->m_windowCenter };
        std::memcpy(dynamic_cast<NewImgReceived*>(cmd)->m_seriesId, temp->m_seriesId, sizeof(temp->m_seriesId));
    }

    emit receiveDevCommSig(std::shared_ptr<CArmCmdBase>(cmd));
}

void StationCommManager::setStatus(std::shared_ptr<StationManagerStatus> status)
{
    m_pStatus = status;
}

void StationCommManager::initLogic()
{
    qRegisterMetaType<std::shared_ptr<CArmCmdBase>>("std::shared_ptr<CArmCmdBase>");

    connect(this, SIGNAL(receiveDevCommSig(std::shared_ptr<CArmCmdBase>)), this, SLOT(receiveDevComm(std::shared_ptr<CArmCmdBase>)));
}

void StationCommManager::receiveDevComm(std::shared_ptr<CArmCmdBase> cmdObject)
{
    if (m_pStatus != nullptr)
    {
        m_pStatus->doProcCmd(cmdObject);
    }
}

void StationCommManager::doCmd(const CArmCMD & cmd)
{
    if (m_pStatus != nullptr)
    {
        // 当前状态下执行指令
        m_pStatus->doCmd(cmd);
    }
}





void CommManagerIdleStatus::doCmd(const CArmCMD& cmdObject)
{
    // 注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        //通知触摸屏退出登录
        WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmCmdLoginStatusChanged(false));
        m_pStManager->setStatus(std::shared_ptr<StationManagerStatus>(new CommManagerLogoutStatus(m_pStManager, m_pStManager->m_pStatus)));
    }
    // 设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {
        handleDeviceCMD(static_cast<const CArmDeviceCMD&>(cmdObject));
    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {
        if (m_pStManager != nullptr)
        {
            // 处理指令
            handlePrepareScanCMD(static_cast<const CArmPrepareScanCMD&>(cmdObject));

            // 更新状态
            m_pStManager->setStatus(std::shared_ptr<StationManagerStatus>(new CommManagerReadyStatus(m_pStManager)));
        }
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 发送配置给触屏
        std::vector<char> config = CArmConfigHandler::GetCfgText();
        BYTE* data = (BYTE*)config.data();
        int len = config.size();
        WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmUpdateConfigInfo(data, len));
    }
}

void CommManagerIdleStatus::doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject)
{
    if (cmdObject == nullptr)
    {
        return;
    }

    // 处理设备通讯指令
    CArmObject::executeCmd(CArmDeviceCMD(cmdObject));
}

void CommManagerIdleStatus::handlePrepareScanCMD(const CArmPrepareScanCMD& cmdObject)
{
    if (m_pStManager == nullptr || m_pStManager->m_pProcInst == nullptr) return;

    IPCPatientInfo scanInfo;

    //患者信息
    strcpy(scanInfo.patientID, cmdObject._patientInfo._patientID.c_str());
    strcpy(scanInfo.firstName, cmdObject._patientInfo._patientFirstName.c_str());
    strcpy(scanInfo.lastName, cmdObject._patientInfo._patientLastName.c_str());
    scanInfo.age = atoi(cmdObject._patientInfo._patientAge.c_str());
    if (cmdObject._patientInfo._patientSex == "F") scanInfo.gender = CArmGenderDef::F;
    else if(cmdObject._patientInfo._patientSex == "M") scanInfo.gender = CArmGenderDef::M;
    else scanInfo.gender = CArmGenderDef::Other;
    //拍摄信息
    strcpy(scanInfo.studyID, cmdObject._scanInfo._stdUID.c_str());
    scanInfo.scanPart = ScanPart::UNKNOWN;


    // 发送拍摄信息给触摸屏
    CArmCmdNewPatientInfo resp{ scanInfo };
    WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &resp);
}


void CommManagerIdleStatus::handleDeviceCMD(const CArmDeviceCMD& cmdObject)
{
    // 处理设备通讯指令
    if (cmdObject._cmdObject != nullptr)
    {
        // 设备异常状态
        if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)   updateDevStatus(cmdObject._cmdObject);
        // 一级手闸状态
        else if (cmdObject._cmdObject->m_u16Cmd == SC_CARM_EXP_BTN1_PRESSED) startExpose(cmdObject._cmdObject);
    }
}

void CommManagerIdleStatus::startExpose(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdExpBtn1Pressed* temp = static_cast<const CArmCmdExpBtn1Pressed*>(object.get());
    if (temp == nullptr) return;
    if (temp->m_arg == CArmBtnStatus::Pressed)
    {
        m_pStManager->setStatus(std::shared_ptr<StationManagerStatus>(new CommManagerExposureStatus(m_pStManager)));
    }
}



void CommManagerReadyStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        //通知触摸屏退出登录
        WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmCmdLoginStatusChanged(false));
        m_pStManager->setStatus(std::shared_ptr<StationManagerStatus>(new CommManagerLogoutStatus(m_pStManager, m_pStManager->m_pStatus)));
    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {

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
        // 处理指令
        handlePrepareScanCMD(static_cast<const CArmPrepareScanCMD&>(cmdObject));
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 发送配置给触屏
        std::vector<char> config = CArmConfigHandler::GetCfgText();
        BYTE* data = (BYTE*)config.data();
        int len = config.size();
        WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmUpdateConfigInfo(data, len));
    }
}

void CommManagerReadyStatus::doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject)
{
    if (cmdObject == nullptr)
    {
        return;
    }

    // 处理设备通讯指令
    CArmObject::executeCmd(CArmDeviceCMD(cmdObject));
}

void CommManagerReadyStatus::handlePrepareScanCMD(const CArmPrepareScanCMD& cmdObject)
{
    if (m_pStManager == nullptr || m_pStManager->m_pProcInst == nullptr) return;

    IPCPatientInfo scanInfo;

    //患者信息
    strcpy(scanInfo.patientID, cmdObject._patientInfo._patientID.c_str());
    strcpy(scanInfo.firstName, cmdObject._patientInfo._patientFirstName.c_str());
    strcpy(scanInfo.lastName, cmdObject._patientInfo._patientLastName.c_str());
    scanInfo.age = atoi(cmdObject._patientInfo._patientAge.c_str());
    if (cmdObject._patientInfo._patientSex == "F") scanInfo.gender = CArmGenderDef::F;
    else if (cmdObject._patientInfo._patientSex == "M") scanInfo.gender = CArmGenderDef::M;
    else scanInfo.gender = CArmGenderDef::Other;
    //拍摄信息
    strcpy(scanInfo.studyID, cmdObject._scanInfo._stdUID.c_str());
    scanInfo.scanPart = ScanPart::UNKNOWN;


    // 发送拍摄信息给触摸屏
    CArmCmdNewPatientInfo resp{ scanInfo };
    WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &resp);
}

void CommManagerReadyStatus::handleDeviceCMD(const CArmDeviceCMD& cmdObject)
{
    // 处理设备通讯指令
    if (cmdObject._cmdObject != nullptr)
    {
        // 设备异常状态
        if (cmdObject._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)   updateDevStatus(cmdObject._cmdObject);
        // 一级手闸状态
        else if(cmdObject._cmdObject->m_u16Cmd == SC_CARM_EXP_BTN1_PRESSED) startExpose(cmdObject._cmdObject);
    }
}

void CommManagerReadyStatus::startExpose(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdExpBtn1Pressed* temp = static_cast<const CArmCmdExpBtn1Pressed*>(object.get());
    if (temp == nullptr) return;
    if (temp->m_arg == CArmBtnStatus::Pressed)
    {
        m_pStManager->setStatus(std::shared_ptr<StationManagerStatus>(new CommManagerExposureStatus(m_pStManager)));
    }
}


void CommManagerExposureStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理退出命令
    if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理注销命令
    else if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        //曝光中不允许注销登录
    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {

    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 曝光中不允许更新配置
    }
}

void CommManagerExposureStatus::doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject)
{
    /*接收图像*/
    CArmObject::executeCmd(CArmDeviceCMD(cmdObject));
    /*结束曝光*/
    CArmObject::executeCmd(CArmDeviceCMD(cmdObject));
}

void CommManagerExposureStatus::handleDeviceCMD(const CArmDeviceCMD & object)
{
    // 处理设备通讯指令
    if (object._cmdObject == nullptr) return;
    
    // 设备异常状态
    if (object._cmdObject->m_u16Cmd == WS_CARM_IPC_DEV_STATUS)   updateDevStatus(object._cmdObject);
    // 曝光结束
    else if (object._cmdObject->m_u16Cmd == WS_CARM_SCAN_FINISHED) finishExpose(object._cmdObject);
    
}

void CommManagerExposureStatus::finishExpose(std::shared_ptr<CArmCmdBase> object)
{
    m_pStManager->setStatus(std::shared_ptr<StationManagerStatus>(new CommManagerReadyStatus(m_pStManager)));
}




void CommManagerAbortStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        //通知触摸屏退出登录
        WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmCmdLoginStatusChanged(false));
        m_pStManager->setStatus(std::shared_ptr<StationManagerStatus>(new CommManagerLogoutStatus(m_pStManager, m_pStManager->m_pStatus)));
    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {
        m_pStManager->setStatus(m_pNxtStatus);
    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {

    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 发送配置给触屏
        std::vector<char> config = CArmConfigHandler::GetCfgText();
        BYTE* data = (BYTE*)config.data();
        int len = config.size();
        WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmUpdateConfigInfo(data, len));
    }
}

void CommManagerAbortStatus::doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject)
{
    if (cmdObject == nullptr)
    {
        return;
    }

    // 处理设备通讯指令
    CArmObject::executeCmd(CArmDeviceCMD(cmdObject));
}




void CommManagerLogoutStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        // 不处理注销命令
    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {
        //通知触摸屏登录状态
        WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmCmdLoginStatusChanged(true));
        m_pStManager->setStatus(m_pNxtStatus);//进入到下一状态
    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {

    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        //注销登录状态不允许更新配置
    }
}

void CommManagerLogoutStatus::doProcCmd(std::shared_ptr<CArmCmdBase> cmdObject)
{
    if (cmdObject == nullptr)
    {
        return;
    }

    // 处理设备通讯指令
    CArmObject::executeCmd(CArmDeviceCMD(cmdObject));
}

void StationManagerStatus::updateDevStatus(std::shared_ptr<CArmCmdBase> object)
{
    const CArmCmdDevStatusChanged* temp = static_cast<const CArmCmdDevStatusChanged*>(object.get());
    if (temp == nullptr) return;
    if (temp->m_arg == ErrorCode::ERR_SUCCESS)
    {
       // // 设备状态正常时，查询光栅状态、电量、球管温度等信息
       //WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmCmdDeviceValue(Request_{ CArmDeviceValueTypeDef::GET_RASTER_STATUS }));
       //WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmCmdDeviceValue(Request_{ CArmDeviceValueTypeDef::GET_BATTERY_PERCENT }));
       //WORK_STATION_MSG_CTRL->Request(ClientType::WORKSTATION_GUI, &CArmCmdDeviceValue(Request_{ CArmDeviceValueTypeDef::GET_BULB_TUBE_TEMP }));
    }
}


