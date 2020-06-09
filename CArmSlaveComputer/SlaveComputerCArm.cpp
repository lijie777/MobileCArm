#include "SlaveComputerCArm.h"
#include <CommonUtility.h>
#include <thread>
#include <IPC_CMD_Def_CArm.h>

Logger *g_pLogger = nullptr;

USlaveComputerCArm::USlaveComputerCArm()
{
    m_bShutdown = false;
}

USlaveComputerCArm::~USlaveComputerCArm()
{
    Disconnect();

    std::lock_guard<std::mutex> lg(m_mtxCmdSend);

    for (auto iter1 = m_CmdMap.begin(); iter1 != m_CmdMap.end(); iter1++)
    {
        for (auto iter2 = iter1->second.begin(); iter2 != iter1->second.end(); iter2++)
        {
            CloseHandle(iter2->second);
        }
    }

    m_CmdMap.clear();
}

#pragma region 私有成员函数

void USlaveComputerCArm::ConnectThread()
{
    LOG4CPLUS_INFO(*g_pLogger, L"Connection thread started");

    while (!m_bShutdown)
    {
        if (m_socClient.Connect(m_strIP, m_strPort))
        {
            LOG4CPLUS_INFO(*g_pLogger, L"Connection established with slave computer");

            if (OnDevStatusChanged) OnDevStatusChanged(ErrorCode::ERR_SUCCESS);
            std::thread thd(&USlaveComputerCArm::ReceiveThread, this);
            thd.detach();

            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(5000));
    }

    LOG4CPLUS_INFO(*g_pLogger, L"Connection thread exit");
}

void USlaveComputerCArm::ReceiveThread()
{
    char recvBuff[128];
    memset(recvBuff, 0, sizeof(recvBuff));

    std::vector<BYTE> totalBuff;
    totalBuff.reserve(128);

    LOG4CPLUS_INFO(*g_pLogger, L"Reception thread started");

    while (true)
    {
        int nRet = m_socClient.Receive(recvBuff, sizeof(recvBuff), false);
        if (nRet <= 0)
        {
            m_socClient.Disconnect();
            LOG4CPLUS_ERROR(*g_pLogger, L"Connection with slave computer is lost");

            if(OnDevStatusChanged) OnDevStatusChanged(ErrorCode::ERR_SC_DISCONNECTED);

            if (!m_bShutdown)
            {
                std::thread thd(&USlaveComputerCArm::ConnectThread, this);
                thd.detach();
            }

            return;
        }

        for (int i = 0; i < nRet; i++)
        {
            totalBuff.push_back(recvBuff[i]);
        }

        std::list<CArmRawCmd> cmds = ExtractCommands(totalBuff);

        if (cmds.size() == 0) continue;

        std::thread thd(&USlaveComputerCArm::ParseCommands, this, cmds);
        thd.detach();
    }
}

std::list<CArmRawCmd> USlaveComputerCArm::ExtractCommands(std::vector<BYTE> &recvBuff)
{
    std::list<CArmRawCmd> ret;

    WORD u16BaseCmdLen = sizeof(CArmCmdBase) - SIZE_VTABLE_PTR;

    while (true)
    {
        //查找第一个SC_CMD_MARK_CARM的位置
        auto iter = recvBuff.begin();
        for (; iter != recvBuff.end(); iter++)
        {
            if (*iter == SC_CMD_MARK_CARM) break;
        }

        //删除前面不是SC_CMD_MARK_CARM的数据
        if (iter != recvBuff.begin()) recvBuff.erase(recvBuff.begin(), iter);

        //接收到的数据长度不足一个命令的长度
        if (recvBuff.size() < u16BaseCmdLen) break;

        BYTE *pData = recvBuff.data();

        std::uint32_t u16MsgLen = *(std::uint32_t*)(pData + sizeof(BYTE));
        WORD u16CmdID = *(WORD*)(pData + sizeof(BYTE) + sizeof(std::uint32_t));
        WORD u16Cmd = *(WORD*)(pData + sizeof(BYTE) + sizeof(std::uint32_t) + sizeof(WORD));

        //接收到的数据长度不足整个命令的长度
        if (recvBuff.size() < u16MsgLen) break;

        std::uint32_t u16ParamLen = u16MsgLen - u16BaseCmdLen;
        BYTE *pCmdData = nullptr;

        if (u16ParamLen > 0)
        {
            pCmdData = new BYTE[u16ParamLen];
            memcpy(pCmdData, pData + u16BaseCmdLen, u16ParamLen);
        }

        ret.push_back(CArmRawCmd(u16MsgLen, u16CmdID, u16Cmd, pCmdData, u16ParamLen));

        recvBuff.erase(recvBuff.begin(), recvBuff.begin() + u16MsgLen);
    }

    return ret;
}

void USlaveComputerCArm::ParseCommands(std::list<CArmRawCmd> cmds)
{
    for (auto iter = cmds.begin(); iter != cmds.end(); iter++)
    {
        LOG4CPLUS_INFO_FMT(*g_pLogger, L"[PC <= SCM][%04X][%04X]%s", iter->m_u16MsgID, iter->m_u16Cmd, TOWS(iter->ToString()));

        //下位机发来的响应消息
        switch (iter->m_u16Cmd)
        {
            case SC_CARM_GET_STATUS:
            case SC_CARM_DEV_RESET:
            case SC_CARM_WARN_RESET:
            case SC_CARM_LASER_SWITCH:
            case SC_CARM_SET_KV:
            case SC_CARM_SET_MA:
            case SC_CARM_SET_FPS:
            case SC_CARM_SET_EXPOSURE:
            case SC_CARM_SET_FRAME_NUM:
            case SC_CARM_SET_HOR_COLL:
            case SC_CARM_SET_VER_COLL:
            case SC_CARM_MOVE_LIFT_COL:
            case SC_CARM_MOVE_X_AXIS:
            case SC_CARM_MOVE_C_ARM:
            case SC_CARM_GET_POS_LIFT_COL:
            case SC_CARM_GET_POS_X_AXIS:
            case SC_CARM_GET_POS_C_ARM:
            case SC_CARM_SET_POS_C_ARM:
            case SC_CARM_SET_POS_X_AXIS:
            case SC_CARM_SET_POS_LIFT_COL:
            case SC_CARM_SET_MAN_C_ARM:
            case SC_CARM_SET_MAN_LIFT_COL:
            case SC_CARM_SET_MAN_X_AXIS:
            case SC_CARM_GET_DAP_VALUE:
            case SC_CARM_SET_3D_TRACK_DATA:
            case SC_CARM_START_SCAN_2D:
            case SC_CARM_START_SCAN_3D:
            case SC_CARM_FETCH_3D_TRACK:
            case SC_CARM_ENABLE_DAP:
            case SC_CARM_GET_KV:
            case SC_CARM_GET_MA:
            case SC_CARM_GET_CROSS_LINE_LSR_STS:
            case SC_CARM_GET_VIEW_RANGE_LSR_STS:
            case SC_CARM_GET_MB_TEMP:
            case SC_CARM_ENABLE_TUBE_CTRL:
            case SC_CARM_RADIATION_PERMISSION:
            case SC_CARM_GET_BULB_TUBE_TEMP:
            case SC_CARM_GET_BATTERY_PERCENT:
            case SC_CARM_GET_RASTER_STATUS:
            case SC_CARM_CANCEL_SCAN:
            case SC_CARM_SET_COLL_H_CONTINUE_MOVE:
            case SC_CARM_SET_COLL_V_CONTINUE_MOVE:
            case SC_CARM_GET_COLL_H_POS:
            case SC_CARM_GET_COLL_V_POS:
            {
                {
                    std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
                    m_CmdRecv[iter->m_u16MsgID] = *iter;
                }

                {
                    std::lock_guard<std::mutex> lg(m_mtxCmdSend);
                    SetEvent(m_CmdMap[iter->m_u16Cmd][iter->m_u16MsgID]);
                }

                continue;
            }
        }

        //下位机发来的通知消息
        switch (iter->m_u16Cmd)
        {
            case SC_CARM_EXP_BTN1_PRESSED:
            {
                bool bPressed = (*iter->m_pu8Data) != 0;
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Exposure button step1 %s", bPressed ? L"pressed" : L"released");

                std::thread thd(OnExpBtnChanged, CArmExpBtnDef::Level1, bPressed);
                thd.detach();

                break;
            }
            case SC_CARM_EXP_BTN2_PRESSED:
            {
                bool bPressed = (*iter->m_pu8Data) != 0;
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Exposure button step2 %s", bPressed ? L"pressed" : L"released");

                std::thread thd(OnExpBtnChanged, CArmExpBtnDef::Level2, (*iter->m_pu8Data) != 0);
                thd.detach();

                break;
            }
            case SC_CARM_PEDAL_PRESSED:
            {
                CArmPedalDef ePedal = (CArmPedalDef)(*iter->m_pu8Data);
                CArmExpBtnDef level = CArmExpBtnDef(*(iter->m_pu8Data + 1));
                bool bPressed = *(iter->m_pu8Data + 2) != 0;
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"%s pedal %s", ePedal == CArmPedalDef::Left ? L"Left" : L"Right", level == CArmExpBtnDef::Level1?"first":"second", bPressed ? L"pressed" : L"released");

                std::thread thd(OnPedalChanged, ePedal, level, bPressed);
                thd.detach();

                break;
            }
            case SC_CARM_LIFT_STS_CHANGED:
            {
                CArmAxisMotionDef eMotion = (CArmAxisMotionDef)(*iter->m_pu8Data);
                std::wstring strMotion = (eMotion == CArmAxisMotionDef::Moving) ? L"moving" : L"stopped";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Lifting column %s", strMotion);

                std::thread thd(OnAxisStatusChanged, CArmAxisDef::LiftingCol, eMotion);
                thd.detach();

                break;
            }
            case SC_CARM_XAXIS_STS_CHANGED:
            {
                CArmAxisMotionDef eMotion = (CArmAxisMotionDef)(*iter->m_pu8Data);
                std::wstring strMotion = (eMotion == CArmAxisMotionDef::Moving) ? L"moving" : L"stopped";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Axis X %s", strMotion);

                std::thread thd(OnAxisStatusChanged, CArmAxisDef::ForBackward, eMotion);
                thd.detach();

                break;
            }
            case SC_CARM_CARM_STS_CHANGED:
            {
                CArmAxisMotionDef eMotion = (CArmAxisMotionDef)(*iter->m_pu8Data);
                std::wstring strMotion = (eMotion == CArmAxisMotionDef::Moving) ? L"moving" : L"stopped";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"C-Arm %s", strMotion);

                std::thread thd(OnAxisStatusChanged, CArmAxisDef::CArm, eMotion);
                thd.detach();

                break;
            }
            case SC_CARM_XRAY_READY:
            {
                LOG4CPLUS_INFO(*g_pLogger, L"X-Ray ready");

                std::thread thd(OnXRayReady);
                thd.detach();

                break;
            }
            case SC_CARM_WARN_REPORT:
            {
                ErrorCode err = ToSysErrorCode(*(WORD*)iter->m_pu8Data);
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Device reporting error: %04X", (int)err);

                std::thread thd(OnCArmWarn, err);
                thd.detach();

                break;
            }
            case SC_CARM_COL_IN_PLACE_HOR:
            {
                float fPos = *((float*)iter->m_pu8Data);
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Horizontal collimator in place: %.2f", fPos);

                std::thread thd(OnCArmCollimatorInPlace, OptionCollimator::Hor, fPos);
                thd.detach();

                break;
            }
            case SC_CARM_COL_IN_PLACE_VER:
            {
                float fPos = *((float*)iter->m_pu8Data);
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Veritical collimator in place: %.2f", fPos);

                std::thread thd(OnCArmCollimatorInPlace, OptionCollimator::Ver, fPos);
                thd.detach();

                break;
            }
            case SC_CARM_MANU_AUTO_CHGD_CARM:
            {
                CArmAxisCtrlDef eAxisCtrl = (CArmAxisCtrlDef)(*iter->m_pu8Data);
                std::wstring strMode = (eAxisCtrl == CArmAxisCtrlDef::Auto) ? L"Auto" : L"Manu";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"C-Arm new operation mode: %s", strMode);

                std::thread thd(OnCArmAxisManAutoChanged, CArmAxisDef::CArm, eAxisCtrl);
                thd.detach();

                break;
            }
            case SC_CARM_MANU_AUTO_CHGD_Z:
            {
                CArmAxisCtrlDef eAxisCtrl = (CArmAxisCtrlDef)(*iter->m_pu8Data);
                std::wstring strMode = (eAxisCtrl == CArmAxisCtrlDef::Auto) ? L"Auto" : L"Manu";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Lifting column new operation mode: %s", strMode);

                std::thread thd(OnCArmAxisManAutoChanged, CArmAxisDef::LiftingCol, eAxisCtrl);
                thd.detach();

                break;
            }
            case SC_CARM_MANU_AUTO_CHGD_X:
            {
                CArmAxisCtrlDef eAxisCtrl = (CArmAxisCtrlDef)(*iter->m_pu8Data);
                std::wstring strMode = (eAxisCtrl == CArmAxisCtrlDef::Auto) ? L"Auto" : L"Manu";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Axis X new operation mode: %s", strMode);

                std::thread thd(OnCArmAxisManAutoChanged, CArmAxisDef::ForBackward, eAxisCtrl);
                thd.detach();

                break;
            }
            case SC_CARM_LSR_STS_CHANGED_CROSS:
            {
                bool bOn = (*iter->m_pu8Data) != 0;
                std::wstring strMode = bOn ? L"on" : L"off";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Cross curve laser is switched %s", strMode);

                std::thread thd(OnCArmLaserStatusChanged, OptionLaserSwitch::CrossCurve, bOn);
                thd.detach();

                break;
            }
            case SC_CARM_LSR_STS_CHANGED_VIEW:
            {
                bool bOn = (*iter->m_pu8Data) != 0;
                std::wstring strMode = bOn ? L"on" : L"off";
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Range view laser is switched %s", strMode);

                std::thread thd(OnCArmLaserStatusChanged, OptionLaserSwitch::ViewRange, bOn);
                thd.detach();

                break;
            }
            case SC_CARM_CURR_POS_CARM:
            {
                float fPos = *((float*)iter->m_pu8Data);
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"C-Arm real time angle: %.2f", fPos);

                std::thread thd(OnCArmAxisPosChanged, CArmAxisDef::CArm, fPos);
                thd.detach();

                break;
            }
            case SC_CARM_CURR_POS_Z:
            {
                float fPos = *((float*)iter->m_pu8Data);
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Lifting column real time height: %.2f", fPos);

                std::thread thd(OnCArmAxisPosChanged, CArmAxisDef::LiftingCol, fPos);
                thd.detach();

                break;
            }
            case SC_CARM_RASTER_STATUS_CHANGED:
            {
                bool open_ = (1 == *((std::uint8_t*)iter->m_pu8Data));
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"raster open close status: %d", open_);
                std::thread(OnCArmRasterStatusChanged, open_).detach();
                break;
            }
            case SC_CARM_CURR_POS_X:
            {
                float fPos = *((float*)iter->m_pu8Data);
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Axis X real time position: %.2f", fPos);

                std::thread thd(OnCArmAxisPosChanged, CArmAxisDef::ForBackward, fPos);
                thd.detach();

                break;
            }
            case SC_CARM_DEV_RESET_RES:
            {
                ErrorCode err = ToSysErrorCode(*(WORD*)iter->m_pu8Data);
                LOG4CPLUS_INFO_FMT(*g_pLogger, L"Device reset result: %04X", (int)err);

                std::thread thd(OnCArmDevResetCompleted, err);
                thd.detach();

                break;
            }
            default:
            {
                LOG4CPLUS_ERROR_FMT(*g_pLogger, L"Unknown message received, message id[%04X], message command[%04X]", iter->m_u16MsgID, iter->m_u16Cmd);
                break;
            }
        }
    }
}

ErrorCode USlaveComputerCArm::SendCmd2Sc(CArmCmdBase *pCmd, const std::wstring &strLog)
{
    HANDLE hEvt = CreateEventA(NULL, TRUE, FALSE, ("CArmScCmdEvt_" + std::to_string(pCmd->m_u16MsgID)).c_str());
    ResetEvent(hEvt);

    {
        std::lock_guard<std::mutex> lg(m_mtxCmdSend);
        m_CmdMap[pCmd->m_u16Cmd][pCmd->m_u16MsgID] = hEvt;
    }

    static std::mutex mtxSendOper;

    {
        std::lock_guard<std::mutex> lg(mtxSendOper);
        if (!m_socClient.Send((const char*)(pCmd->GetData()), pCmd->GetSize()))
        {
            {
                std::lock_guard<std::mutex> lg(m_mtxCmdSend);
                m_CmdMap[pCmd->m_u16Cmd].erase(pCmd->m_u16MsgID);
                CloseHandle(hEvt);
            }

            LOG4CPLUS_ERROR_FMT(*g_pLogger, L"Failed to tell SC %s", strLog.c_str());
            return ErrorCode::ERR_SC_ALL_IMG_ACQ;
        }
    }

    LOG4CPLUS_DEBUG_FMT(*g_pLogger, L"[PC => SC][%04X][%04X]%s", pCmd->m_u16MsgID, pCmd->m_u16Cmd, TOWS(pCmd->ToString()));
    LOG4CPLUS_INFO_FMT(*g_pLogger, L"Sending message [%04X] to SC %s", pCmd->m_u16MsgID, strLog.c_str());

    DWORD dwRet = WaitForSingleObject(hEvt, SC_RESP_TMO);

    {
        std::lock_guard<std::mutex> lg(m_mtxCmdSend);
        m_CmdMap[pCmd->m_u16Cmd].erase(pCmd->m_u16MsgID);
        CloseHandle(hEvt);
    }

    switch (dwRet)
    {
    case WAIT_OBJECT_0:
        LOG4CPLUS_INFO_FMT(*g_pLogger, L"Sending message [%04X] done", pCmd->m_u16MsgID);
        return ErrorCode::ERR_SUCCESS;
    case WAIT_TIMEOUT:
        LOG4CPLUS_ERROR_FMT(*g_pLogger, L"Wait SC response timeout, message id [%04X]", pCmd->m_u16MsgID);
        return ErrorCode::ERR_SC_RESP_TMO;
    default:
        LOG4CPLUS_ERROR_FMT(*g_pLogger, L"WaitForSingleObject fail, %s", UCommonUtility::GetSystemErrorMessageW());
        return ErrorCode::ERR_SC_WAIT_RESP;
    }
}

ErrorCode USlaveComputerCArm::ToSysStatusCode(WORD devSts)
{
    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::ToSysErrorCode(WORD operSts)
{
    return ErrorCode::ERR_SUCCESS;
}

#pragma endregion

#pragma region 外部接口

ErrorCode USlaveComputerCArm::Connect(Logger *pLogger, const std::string &strIP, const std::string& strPort)
{
    m_bShutdown = false;
    g_pLogger = pLogger;
    m_strIP = strIP;
    m_strPort = strPort;

    if (m_socClient.Connect(strIP, strPort))
    {
        LOG4CPLUS_INFO(*g_pLogger, L"Connection established with slave computer");

        if (OnDevStatusChanged) OnDevStatusChanged(ErrorCode::ERR_SUCCESS);

        std::thread thd(&USlaveComputerCArm::ReceiveThread, this);
        thd.detach();

        return ErrorCode::ERR_SUCCESS;
    }

    std::thread thd(&USlaveComputerCArm::ConnectThread, this);
    thd.detach();

    return ErrorCode::ERR_SC_CONN_FAIL;
}

ErrorCode USlaveComputerCArm::Disconnect()
{
    m_bShutdown = true;
    m_socClient.Disconnect();

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::GetStatus()
{
    if(!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_STATUS);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get status");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysStatusCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::DeviceReset()
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_DEV_RESET);

    ErrorCode res = SendCmd2Sc(&cmd, L"to reset device");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::WarnReset()
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_WARN_RESET);

    ErrorCode res = SendCmd2Sc(&cmd, L"to clear device warnning");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::LaserSwitch(OptionLaserSwitch opt, bool bOn)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdLaserSwitch cmd(opt, bOn);

    ErrorCode res = SendCmd2Sc(&cmd, L"to switch laser");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetKV(float fKv)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetKV cmd(fKv);

    ErrorCode res = SendCmd2Sc(&cmd, L"to set kV");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetMA(float fMa)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetMA cmd(fMa);

    ErrorCode res = SendCmd2Sc(&cmd, L"to set mA");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetFrameRate(unsigned char nFPS)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetFPS cmd(nFPS);

    ErrorCode res = SendCmd2Sc(&cmd, L"to set frame rate");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetExposure(unsigned char nExp)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetExposure cmd(nExp);

    ErrorCode res = SendCmd2Sc(&cmd, L"to set single frame exposure");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetFrameNum(unsigned short nFrameNum)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetFrameNum cmd(nFrameNum);

    ErrorCode res = SendCmd2Sc(&cmd, L"to set frame number");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetCollimator(OptionCollimator opt, float speed, float fPos)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetCollimator cmd(opt, speed, fPos);

    ErrorCode res = SendCmd2Sc(&cmd, L"to set collimator position");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetCollimatorContinueMove(OptionCollimator coll, OptionCollimatorMoveDirection d, float speed)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    std::unique_ptr<CArmCmdBase> cmd;
    if (OptionCollimator::Hor == coll)
    {
        cmd = std::unique_ptr<CArmCmdCollContinueMoveH>(new CArmCmdCollContinueMoveH({d, speed}));
    }
    else
    {
        cmd = std::unique_ptr<CArmCmdCollContinueMoveV>(new CArmCmdCollContinueMoveV({ d, speed }));
    }

    ErrorCode res = SendCmd2Sc(cmd.get(), L"to set collimator continue move");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd->m_u16MsgID];
        m_CmdRecv.erase(cmd->m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    return res;
}

ErrorCode USlaveComputerCArm::MoveLiftColomn(OptionLiftColMove opt, float fSpeed)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdMoveLiftCol cmd(opt, fSpeed);

    ErrorCode res = SendCmd2Sc(&cmd, L"to move lifting column");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::MoveXAxis(OptionXAxisMove opt, float fSpeed)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdMoveXAxis cmd(opt, fSpeed);

    ErrorCode res = SendCmd2Sc(&cmd, L"to move X-Axis");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::MoveCArm(OptionCArmMove opt, float fSpeed)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdMoveCArm cmd(opt, fSpeed);

    ErrorCode res = SendCmd2Sc(&cmd, L"to move C-Arm");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::GetLiftColumnPos(float &fPos)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_POS_LIFT_COL);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get position of lifting column");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fPos = *(float*)recvCmd.m_pu8Data;

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::GetXAxisPos(float &fPos)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_POS_X_AXIS);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get position of X-Axis");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fPos = *(float*)recvCmd.m_pu8Data;

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::GetCArmAngle(float &fAng)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_POS_C_ARM);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get current angle of C-Arm");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fAng = *(float*)recvCmd.m_pu8Data;

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::GetCollPos(OptionCollimator c, float& fVal)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd((OptionCollimator::Hor == c) ? SC_CARM_GET_COLL_H_POS : SC_CARM_GET_COLL_V_POS);

    auto res = SendCmd2Sc(&cmd, L"to get current collimator pos");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fVal = *(float*)recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::SetCArmAngle(float fSpeed, float fAng)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetCArmAngle cmd(fSpeed, fAng);

    ErrorCode res = SendCmd2Sc(&cmd, L"to move C-Arm to specified angle");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetXAxisPos(float fSpeed, float fPos)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetXAxisPos cmd(fSpeed, fPos);

    ErrorCode res = SendCmd2Sc(&cmd, L"to move X-Axis to specified position");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetLiftColumnPos(float fSpeed, float fPos)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetLiftColPos cmd(fSpeed, fPos);

    ErrorCode res = SendCmd2Sc(&cmd, L"to move lifting column to specified height");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetCArmCtrlMode(CArmAxisCtrlDef m)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetManuCArm cmd(m);

    std::wstringstream ss;
    ss << L"to set C-Arm under " << (m == CArmAxisCtrlDef::Auto ? L"auto" : L"manu") << "-ctrl";

    ErrorCode res = SendCmd2Sc(&cmd, ss.str().c_str());
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetLiftColumnCtrlMode(CArmAxisCtrlDef m)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetManuLiftCol cmd(m);

    std::wstringstream ss;
    ss << L"to set lifting column under " << (m == CArmAxisCtrlDef::Auto ? L"auto" : L"manu") << "-ctrl";

    ErrorCode res = SendCmd2Sc(&cmd, ss.str().c_str());
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::SetXAxisCtrlMode(CArmAxisCtrlDef m)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSetManuXAxis cmd(m);

    std::wstringstream ss;
    ss << L"to set x-axis under " << (m == CArmAxisCtrlDef::Auto ? L"auto" : L"manu") << "-ctrl";

    ErrorCode res = SendCmd2Sc(&cmd, ss.str().c_str());
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::GetCurrentDapValue(float &fDapVal)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_DAP_VALUE);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get current DAP value");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fDapVal = *(float*)recvCmd.m_pu8Data;

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::SlaveScanCtrl(const SlaveScanCtrlType t)
{
    if (SlaveScanCtrlType::SendScanParam == t)
    {
        //std::thread([this] {
            GlobalMemMapping gScanConfig;
            GlobalMemMapping::Get(gScanConfig);
            auto& cfg = gScanConfig.RangeCT;
            SetKV(cfg->fKV[0]);
            SetMA(cfg->fMA[0]);
            SetFrameRate(cfg->nFPS);
            SetFrameNum(cfg->nFrameNum);
            SetExposure(cfg->nExposure[0]);
        //}).detach();
        return ErrorCode::ERR_SUCCESS;
    }
    else if (SlaveScanCtrlType::Send2DScanCmd_XRay == t)
    {
        return Start2DScan(true);
    }
    else if ((SlaveScanCtrlType::XRayAllow == t) || (SlaveScanCtrlType::XRayNotAllow == t))
    {
        return RadiationPermission(SlaveScanCtrlType::XRayAllow == t);
    }
    else if (SlaveScanCtrlType::CancelScan == t)
    {
        return CancelScan();
    }

    return ErrorCode::ERR_SC_FUNC_UNSUPPORTED;
}

ErrorCode USlaveComputerCArm::Send3DTrackData(unsigned short u16Num, const float *pPoints)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdSend3DTrackData cmd(u16Num, pPoints);

    ErrorCode res = SendCmd2Sc(&cmd, L"to send 3D track points");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::Retrieve3DTrackData(unsigned short &u16Num, float **ppPoints)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_FETCH_3D_TRACK);

    ErrorCode res = SendCmd2Sc(&cmd, L"to fetch 3D track points recorded in last 3D scan");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];

        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    const BYTE *pRecvData = recvCmd.m_pu8Data;
    u16Num = *(WORD*)pRecvData;
    *ppPoints = new float[u16Num * 3];

    std::memcpy(*ppPoints, pRecvData + sizeof(WORD), u16Num * 3 * sizeof(float));

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::Start2DScan(bool bXRay)
{
    if (!m_socClient.IsConnected()) return ErrorCode::ERR_SC_NO_CONN;

    CArmCmdStartScan2D cmd(bXRay);

    ErrorCode res = SendCmd2Sc(&cmd, L"to start 2D scan");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::Request(const std::string& type)
{
    if (CArmIPCCMD::CANCEL_SCAN == type)
    {
        return CancelScan();
    }

    return ErrorCode::ERR_SC_FUNC_UNSUPPORTED;
}

ErrorCode USlaveComputerCArm::CancelScan()
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_CANCEL_SCAN);

    ErrorCode res = SendCmd2Sc(&cmd, L"to cancel scan");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::RadiationPermission(bool bEnable)
{
    if (!m_socClient.IsConnected()) return ErrorCode::ERR_SC_NO_CONN;

    CArmCmdRadiationPermission cmd(bEnable ? CArmPermissionDef::YES : CArmPermissionDef::NO);
    auto res = SendCmd2Sc(&cmd, L"to permission radiation");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::Start3DScan(OptionCArmMove dr, bool bXRay)
{
    if (!m_socClient.IsConnected()) return ErrorCode::ERR_SC_NO_CONN;

    CArmCmdStartScan3D cmd(dr, bXRay);

    ErrorCode res = SendCmd2Sc(&cmd, L"to start 3D scan");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::EnableDAP(bool bEnable)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdEnableDAP cmd(bEnable);

    ErrorCode res = SendCmd2Sc(&cmd, L"to enable DAP");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

ErrorCode USlaveComputerCArm::GetCurrentKV(float &fKV)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_KV);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get current kV");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fKV = *(float*)recvCmd.m_pu8Data;

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}


ErrorCode USlaveComputerCArm::GetCurrentMA(float &fMA)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_MA);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get current mA");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fMA = *(float*)recvCmd.m_pu8Data;

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

// ErrorCode USlaveComputerCArm::GetBulbTubeTemp(float &fTemp)
// {
//     if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;
// 
//     CArmCmdNoParam cmd(SC_CARM_GET_MB_TEMP);
// 
//     ErrorCode res = SendCmd2Sc(&cmd, L"to get bulb tube temp");
//     if (IS_ERR(res)) return res;
// 
//     CArmRawCmd recvCmd;
//     {
//         std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
//         recvCmd = m_CmdRecv[cmd.m_u16MsgID];
//         m_CmdRecv.erase(cmd.m_u16MsgID);
//     }
// 
//     fTemp = *((float*)(recvCmd.m_pu8Data));
// 
//     delete[] recvCmd.m_pu8Data;
// 
//     return ErrorCode::ERR_SUCCESS;
// }

ErrorCode USlaveComputerCArm::Get(const std::string& type, std::uint8_t& val)
{
    return GetVal(type, val);
}

ErrorCode USlaveComputerCArm::Get(const std::string& type, float& val)
{
    return GetVal(type, val);
}

ErrorCode USlaveComputerCArm::Get(const std::string& type, DAPValue& val)
{
    return GetVal(type, val);
}

ErrorCode USlaveComputerCArm::GetLaserStatus(OptionLaserSwitch lsr, bool &bOn, float &fRemainingDelay)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    BYTE c = (lsr == OptionLaserSwitch::CrossCurve) ? SC_CARM_GET_CROSS_LINE_LSR_STS : SC_CARM_GET_VIEW_RANGE_LSR_STS;

    CArmCmdNoParam cmd(c);

    std::wstringstream ss;
    ss << L"to get status of " << ((lsr == OptionLaserSwitch::CrossCurve) ? L"cross line laser" : L"range view laser");

    ErrorCode res = SendCmd2Sc(&cmd, ss.str().c_str());
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    bOn = (*(unsigned char*)recvCmd.m_pu8Data) != 0;
    fRemainingDelay = *(float*)(recvCmd.m_pu8Data + sizeof(unsigned char));

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::GetMainboardTemp(float &fCentigrade)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdNoParam cmd(SC_CARM_GET_MB_TEMP);

    ErrorCode res = SendCmd2Sc(&cmd, L"to get current temperature of mainboard");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    fCentigrade = *(float*)recvCmd.m_pu8Data;

    //delete[] recvCmd.m_pu8Data;

    return ErrorCode::ERR_SUCCESS;
}

ErrorCode USlaveComputerCArm::EnableTubeCtrl(bool bEnable)
{
    if (!m_socClient.IsConnected()) ErrorCode::ERR_SC_NO_CONN;

    CArmCmdEnableTubeCtrl cmd(bEnable);

    ErrorCode res = SendCmd2Sc(&cmd, L"to enable tube control");
    if (IS_ERR(res)) return res;

    CArmRawCmd recvCmd;
    {
        std::lock_guard<std::mutex> lg(m_mtxCmdRecv);
        recvCmd = m_CmdRecv[cmd.m_u16MsgID];
        m_CmdRecv.erase(cmd.m_u16MsgID);
    }

    res = ToSysErrorCode(*(WORD*)recvCmd.m_pu8Data);

    //delete[] recvCmd.m_pu8Data;

    return res;
}

#pragma endregion
