#include "ScanInfoServer.h"
#include <log4cplus/loggingmacros.h>
#include <IPC_CMD_Def_CArm.h>
#include <IPCDefinitions.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

extern Logger g_logger;

UScanInfoServer::UScanInfoServer(const std::string &strPort, bool bSingleClient)
    : IPCServerBase("ScanInfoServer", strPort, bSingleClient)
{
    m_soc = INVALID_SOCKET;
}

UScanInfoServer::~UScanInfoServer()
{
}

void UScanInfoServer::ParseCmdsRecv(SOCKET soc, const std::string &strClientName, const std::vector<std::string> &lstCmd)
{
    for (auto iter = lstCmd.begin(); iter != lstCmd.end(); iter++)
    {
        LOG4CPLUS_INFO_FMT(g_logger, TOWS(*iter));

        auto subCmds = UCommonUtility::StringSplit(*iter, CMD::CMD_SEPARATOR);

        if (IsUIOriginatedCommand(subCmds[0]))
        {
            std::lock_guard<std::mutex> lg(m_mtxCmdMap);
            auto iterResp = m_mapCmdResp.find(subCmds[0]);
            if (iterResp == m_mapCmdResp.end()) continue;

            iterResp->second.second = subCmds;
            SetEvent(iterResp->second.first);
        }
        else if (subCmds[0] == CArmIPCCMD::EXP_BTN_PRESSED)
        {
            if (OnExpBtnChanged) OnExpBtnChanged((CArmExpBtnDef)std::stoi(subCmds[1]), std::stoi(subCmds[2]) != 0);
        }
        else if (subCmds[0] == CArmIPCCMD::PEDAL_PRESSED)
        {
            if (OnPedalChanged) OnPedalChanged((CArmPedalDef)std::stoi(subCmds[1]), CArmExpBtnDef(std::stoi(subCmds[2])), std::stoi(subCmds[3]) != 0);
        }
        else if (subCmds[0] == CArmIPCCMD::AXIS_MOTION_STATUS_CHGD)
        {
            if (OnAxisStatusChanged) OnAxisStatusChanged((CArmAxisDef)std::stoi(subCmds[1]), (CArmAxisMotionDef)std::stoi(subCmds[2]));
        }
        else if (subCmds[0] == CArmIPCCMD::COLLIMATOR_IN_PLACE)
        {
            if (OnCArmCollimatorInPlace) OnCArmCollimatorInPlace((OptionCollimator)std::stoi(subCmds[1]), std::stof(subCmds[2]));
        }
        else if (subCmds[0] == CArmIPCCMD::AXIS_OPER_MODE_CHGD)
        {
            if (OnCArmAxisManAutoChanged) OnCArmAxisManAutoChanged((CArmAxisDef)std::stoi(subCmds[1]), (CArmAxisCtrlDef)std::stoi(subCmds[2]));
        }
        else if (subCmds[0] == CArmIPCCMD::LASER_SWITCH_CHGD)
        {
            if (OnCArmLaserStatusChanged) OnCArmLaserStatusChanged((OptionLaserSwitch)std::stoi(subCmds[1]), std::stoi(subCmds[2]) != 0);
        }
        else if (subCmds[0] == CArmIPCCMD::RASTER_STATUS_CHGD)
        {
            if (OnCArmRasterStatusChanged) OnCArmRasterStatusChanged(std::stoi(subCmds[1]) != 0);
        }
        else if (subCmds[0] == CArmIPCCMD::AXIS_POS_CHGD)
        {
            if (OnCArmAxisPosChanged) OnCArmAxisPosChanged((CArmAxisDef)std::stoi(subCmds[1]), std::stof(subCmds[2]));
        }
        else if (subCmds[0] == CArmIPCCMD::DEV_RESET_DONE)
        {
            if (OnCArmDevResetCompleted) OnCArmDevResetCompleted((ErrorCode)std::stoi(subCmds[1]));
        }
        else if (subCmds[0] == CArmIPCCMD::SEND_NEW_IMAGE)
        {
            int nIdx = std::stoi(subCmds[1]);
            int nWidth = std::stoi(subCmds[2]);
            int nHeight = std::stoi(subCmds[3]);

            unsigned short *pImgData = new unsigned short[nWidth * nHeight];
            UCommonUtility::CopyFromMappedMemory(subCmds[4], nWidth * nHeight * sizeof(unsigned short), pImgData);

            if (OnNewImageReceived) OnNewImageReceived(nIdx, nWidth, nHeight, pImgData);
        }
        else
        {
            LOG4CPLUS_ERROR_FMT(g_logger, L"Unkown command received:%s", TOWS(subCmds[0]));
        }
    }

    
}

ErrorCode UScanInfoServer::SendCmd2DevProc(const std::string &strCmd, const std::string &strRespCmd, const std::vector<std::string> &vecParam, std::vector<std::string> &vecResp)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    HANDLE hEvt = CreateEventA(NULL, TRUE, FALSE, UCommonUtility::GetNewGUID().c_str());

    {
        std::lock_guard<std::mutex> lg(m_mtxCmdMap);

        m_mapCmdResp[strRespCmd].first = hEvt;
        m_mapCmdResp[strRespCmd].second.clear();
    }

    if (!SendCommand(m_soc, CMD::TO_CMD(strCmd, vecParam)))
    {
        CloseHandle(hEvt);

        {
            std::lock_guard<std::mutex> lg(m_mtxCmdMap);
            m_mapCmdResp.erase(strRespCmd);
        }

        return ErrorCode::ERR_SCAN_UI_SEND_CMD_DEV_PROC;
    }

    DWORD dwRet = WaitForSingleObject(hEvt, 2000);

    CloseHandle(hEvt);

    {
        std::lock_guard<std::mutex> lg(m_mtxCmdMap);

        vecResp = m_mapCmdResp[strRespCmd].second;
        m_mapCmdResp.erase(strRespCmd);
    }

    switch (dwRet)
    {
    case WAIT_OBJECT_0:
        return ErrorCode::ERR_SUCCESS;
    case WAIT_TIMEOUT:
        return ErrorCode::ERR_DEV_PROC_RESP_TMO;
    default:
        LOG4CPLUS_ERROR_FMT(g_logger, L"WaitForSingleObject fail, %s", UCommonUtility::GetSystemErrorMessageW());
        return ErrorCode::ERR_SC_WAIT_RESP;
    }
}

bool UScanInfoServer::IsUIOriginatedCommand(const std::string &strCmd)
{
    static std::map<std::string, int> localMap =
    {
        { CArmIPCCMD::GET_SC_STATUS_RESP, 0 },
        { CArmIPCCMD::DEV_RESET_RESP, 0 },
        { CArmIPCCMD::WARN_RESET_RESP, 0 },
        { CArmIPCCMD::LASER_SWITCH_RESP, 0 },
        { CArmIPCCMD::SET_KV_RESP, 0 },
        { CArmIPCCMD::SET_MA_RESP, 0 },
        { CArmIPCCMD::SET_FPS_RESP, 0 },
        { CArmIPCCMD::SET_EXPOSURE_RESP, 0 },
        { CArmIPCCMD::SET_FRAME_NUM_RESP, 0 },
        { CArmIPCCMD::SET_HOR_COLL_RESP, 0 },
        { CArmIPCCMD::SET_VER_COLL_RESP, 0 },
        { CArmIPCCMD::MOVE_LIFT_COL_RESP, 0 },
        { CArmIPCCMD::MOVE_X_AXIS_RESP, 0 },
        { CArmIPCCMD::MOVE_C_ARM_RESP, 0 },
        { CArmIPCCMD::GET_POS_LIFT_COL_RESP, 0 },
        { CArmIPCCMD::GET_POS_X_AXIS_RESP, 0 },
        { CArmIPCCMD::GET_POS_C_ARM_RESP, 0 },
        { CArmIPCCMD::SET_POS_C_ARM_RESP, 0 },
        { CArmIPCCMD::SET_POS_X_AXIS_RESP, 0 },
        { CArmIPCCMD::SET_POS_LIFT_COL_RESP, 0 },
        { CArmIPCCMD::SET_MAN_C_ARM_RESP, 0 },
        { CArmIPCCMD::SET_MAN_LIFT_COL_RESP, 0 },
        { CArmIPCCMD::SET_MAN_X_AXIS_RESP, 0 },
        { CArmIPCCMD::GET_DAP_VALUE_RESP, 0 },
        { CArmIPCCMD::SET_3D_TRACK_DATA_RESP, 0 },
        { CArmIPCCMD::START_SCAN_2D_RESP, 0 },
        { CArmIPCCMD::START_SCAN_3D_RESP, 0 },
        { CArmIPCCMD::FETCH_3D_TRACK_RESP, 0 },
        { CArmIPCCMD::ENABLE_DAP_RESP, 0 },
        { CArmIPCCMD::GET_KV_RESP, 0 },
        { CArmIPCCMD::GET_MA_RESP, 0 },
        { CArmIPCCMD::GET_CROSS_LINE_LSR_STS_RESP, 0 },
        { CArmIPCCMD::GET_VIEW_RANGE_LSR_STS_RESP, 0 },
        { CArmIPCCMD::GET_MB_TEMP_RESP, 0 },
        { CArmIPCCMD::ENABLE_TUBE_CTRL_RESP, 0 },
        { CArmIPCCMD::GET_Bulb_Tube_Temp_RESP, 0 },
        { CArmIPCCMD::GET_BATTERY_PERCENT_RESP, 0},
        { CArmIPCCMD::RADIARION_PERMISSION_CTRL_RESP, 0},
        { CArmIPCCMD::GET_RASTER_STATUS_RESP, 0},
        { CArmIPCCMD::CANCEL_SCAN_RESP, 0 },
        { CArmIPCCMD::SLAVE_SCAN_CTRL_RESP, 0},
        { CArmIPCCMD::SET_COLL_CONTINUE_MOVE_RESP, 0},
        { CArmIPCCMD::GET_POS_COLLIMATOR_RESP, 0 }
    };

    return localMap.find(strCmd) != localMap.end();
}

#pragma region 设备控制函数

//开始2D扫描
ErrorCode UScanInfoServer::StartScan2D(DevWorkMode m, bool bXRay)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::START_SCAN_2D;
    std::string strRespCMD = CArmIPCCMD::START_SCAN_2D_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string((int)m));
    vecParam.push_back(std::to_string(bXRay));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//开始3D扫描
ErrorCode UScanInfoServer::StartScan3D(DevWorkMode m, OptionCArmMove direct, bool bXRay)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::START_SCAN_3D;
    std::string strRespCMD = CArmIPCCMD::START_SCAN_3D_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string((int)m));
    vecParam.push_back(std::to_string((int)direct));
    vecParam.push_back(std::to_string(bXRay));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//查询下位机当前状态
ErrorCode UScanInfoServer::GetScStatus()
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::GET_SC_STATUS;
    std::string strRespCMD = CArmIPCCMD::GET_SC_STATUS_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//重置设备
ErrorCode UScanInfoServer::ResetDevice()
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::DEV_RESET;
    std::string strRespCMD = CArmIPCCMD::DEV_RESET_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

ErrorCode UScanInfoServer::CancelScan()
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::vector<std::string> vecParam, vecResp;
    auto res = SendCmd2DevProc(CArmIPCCMD::CANCEL_SCAN, CArmIPCCMD::CANCEL_SCAN_RESP, vecParam, vecResp);

    return IS_ERR(res) ? res : ErrorCode(std::stoi(vecResp[1]));
}

//重置设备报警状态
ErrorCode UScanInfoServer::ResetWarn()
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::WARN_RESET;
    std::string strRespCMD = CArmIPCCMD::WARN_RESET_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//激光灯开关控制
ErrorCode UScanInfoServer::LaserSwitch(OptionLaserSwitch opt, bool bOn)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::LASER_SWITCH;
    std::string strRespCMD = CArmIPCCMD::LASER_SWITCH_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string((int)opt));
    vecParam.push_back(std::to_string(bOn));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//设置管电压
ErrorCode UScanInfoServer::SetKV(float fKV)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::SET_KV;
    std::string strRespCMD = CArmIPCCMD::SET_KV_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(fKV));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//设置管电流
ErrorCode UScanInfoServer::SetMA(float fMA)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::SET_MA;
    std::string strRespCMD = CArmIPCCMD::SET_MA_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(fMA));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//设置采集帧率
ErrorCode UScanInfoServer::SetFPS(int nFPS)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::SET_FPS;
    std::string strRespCMD = CArmIPCCMD::SET_FPS_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(nFPS));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//设置曝光时间
ErrorCode UScanInfoServer::SetExposure(int nExp)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::SET_EXPOSURE;
    std::string strRespCMD = CArmIPCCMD::SET_EXPOSURE_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(nExp));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//设置采集帧数
ErrorCode UScanInfoServer::SetFrameNum(int nFrameNum)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::SET_FRAME_NUM;
    std::string strRespCMD = CArmIPCCMD::SET_FRAME_NUM_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(nFrameNum));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//设置限束器位置
ErrorCode UScanInfoServer::SetCollimator(OptionCollimator coll, float speed, float fPos)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = (coll == OptionCollimator::Hor) ? CArmIPCCMD::SET_HOR_COLL : CArmIPCCMD::SET_VER_COLL;
    std::string strRespCMD = (coll == OptionCollimator::Hor) ? CArmIPCCMD::SET_HOR_COLL_RESP : CArmIPCCMD::SET_VER_COLL_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(speed));
    vecParam.push_back(std::to_string(fPos));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

ErrorCode UScanInfoServer::SetCollimatorContinueMove(OptionCollimator coll, OptionCollimatorMoveDirection d, float speed)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(int(coll)));
    vecParam.push_back(std::to_string(int(d)));
    vecParam.push_back(std::to_string(speed));

    auto res = SendCmd2DevProc(CArmIPCCMD::SET_COLL_CONTINUE_MOVE, CArmIPCCMD::SET_COLL_CONTINUE_MOVE_RESP, vecParam, vecResp);

    return IS_ERR(res) ? res : ErrorCode(std::stoi(vecResp[1]));
}

//控制升降柱开始连续运动
ErrorCode UScanInfoServer::MoveAxisZ(OptionLiftColMove opt, float fSpeed)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::MOVE_LIFT_COL;
    std::string strRespCMD = CArmIPCCMD::MOVE_LIFT_COL_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string((int)opt));
    vecParam.push_back(std::to_string(fSpeed));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//控制前后轴开始连续运动
ErrorCode UScanInfoServer::MoveAxisX(OptionXAxisMove opt, float fSpeed)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::MOVE_X_AXIS;
    std::string strRespCMD = CArmIPCCMD::MOVE_X_AXIS_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string((int)opt));
    vecParam.push_back(std::to_string(fSpeed));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//控制C型臂开始连续运动
ErrorCode UScanInfoServer::MoveAxisY(OptionCArmMove opt, float fSpeed)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::MOVE_C_ARM;
    std::string strRespCMD = CArmIPCCMD::MOVE_C_ARM_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string((int)opt));
    vecParam.push_back(std::to_string(fSpeed));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);

    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//获取某个轴的当前位置（角度）
ErrorCode UScanInfoServer::GetPosAxis(CArmAxisDef axis, float &fVal)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::map<CArmAxisDef, std::string> axisCmdMap =
    {
        { CArmAxisDef::CArm, CArmIPCCMD::GET_POS_C_ARM },
        { CArmAxisDef::ForBackward, CArmIPCCMD::GET_POS_X_AXIS },
        { CArmAxisDef::LiftingCol, CArmIPCCMD::GET_POS_LIFT_COL }
    };

    std::map<CArmAxisDef, std::string> axisRespCmdMap =
    {
        { CArmAxisDef::CArm, CArmIPCCMD::GET_POS_C_ARM_RESP },
        { CArmAxisDef::ForBackward, CArmIPCCMD::GET_POS_X_AXIS_RESP },
        { CArmAxisDef::LiftingCol, CArmIPCCMD::GET_POS_LIFT_COL_RESP }
    };

    std::string strCMD = axisCmdMap[axis];
    std::string strRespCMD = axisRespCmdMap[axis];

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    fVal = std::stof(vecResp[2]);

    return  (ErrorCode)std::stoi(vecResp[1]);
}

ErrorCode UScanInfoServer::GetPosColl(OptionCollimator coll, float& fVal)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(int(coll)));
    vecParam.push_back(std::to_string(fVal));

    ErrorCode res = SendCmd2DevProc(CArmIPCCMD::GET_POS_COLLIMATOR, CArmIPCCMD::GET_POS_COLLIMATOR_RESP, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    fVal = std::stof(vecResp[2]);

    return ErrorCode(std::stoi(vecResp[1]));
}

//控制某个轴以指定速度运动到指定位置
ErrorCode UScanInfoServer::SetPosAxis(CArmAxisDef axis, float fSpeed, float fPos)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::map<CArmAxisDef, std::string> axisCmdMap =
    {
        { CArmAxisDef::CArm, CArmIPCCMD::SET_POS_C_ARM },
        { CArmAxisDef::ForBackward, CArmIPCCMD::SET_POS_X_AXIS },
        { CArmAxisDef::LiftingCol, CArmIPCCMD::SET_POS_LIFT_COL }
    };

    std::map<CArmAxisDef, std::string> axisRespCmdMap =
    {
        { CArmAxisDef::CArm, CArmIPCCMD::SET_POS_C_ARM_RESP },
        { CArmAxisDef::ForBackward, CArmIPCCMD::SET_POS_X_AXIS_RESP },
        { CArmAxisDef::LiftingCol, CArmIPCCMD::SET_POS_LIFT_COL_RESP }
    };

    std::string strCMD = axisCmdMap[axis];
    std::string strRespCMD = axisRespCmdMap[axis];

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(fSpeed));
    vecParam.push_back(std::to_string(fPos));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//设置某个轴自动/手动控制
ErrorCode UScanInfoServer::SetManuAxis(CArmAxisDef axis, CArmAxisCtrlDef ctrl)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::map<CArmAxisDef, std::string> axisCmdMap =
    {
        { CArmAxisDef::CArm, CArmIPCCMD::SET_MAN_C_ARM },
        { CArmAxisDef::ForBackward, CArmIPCCMD::SET_MAN_X_AXIS },
        { CArmAxisDef::LiftingCol, CArmIPCCMD::SET_MAN_LIFT_COL }
    };

    std::map<CArmAxisDef, std::string> axisRespCmdMap =
    {
        { CArmAxisDef::CArm, CArmIPCCMD::SET_MAN_C_ARM_RESP },
        { CArmAxisDef::ForBackward, CArmIPCCMD::SET_MAN_X_AXIS_RESP },
        { CArmAxisDef::LiftingCol, CArmIPCCMD::SET_MAN_LIFT_COL_RESP }
    };

    std::string strCMD = axisCmdMap[axis];
    std::string strRespCMD = axisRespCmdMap[axis];

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string((int)ctrl));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//获取DAP的当前读数
ErrorCode UScanInfoServer::GetDAPValue(DAPValue &fVal)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::GET_DAP_VALUE;
    std::string strRespCMD = CArmIPCCMD::GET_DAP_VALUE_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    fVal.Gy_per_min = std::stof(vecResp[2]);
    fVal.Gy = std::stof(vecResp[3]);
    fVal.DAP = std::stof(vecResp[4]);

    return  (ErrorCode)std::stoi(vecResp[1]);
}

//启用/禁用DAP
ErrorCode UScanInfoServer::EnableDAP(bool bEnable)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::ENABLE_DAP;
    std::string strRespCMD = CArmIPCCMD::ENABLE_DAP_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(bEnable));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

ErrorCode UScanInfoServer::SlaveScanCtrl(const SlaveScanCtrlType t)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(int(t)));

    auto res = SendCmd2DevProc(CArmIPCCMD::SLAVE_SCAN_CTRL, CArmIPCCMD::SLAVE_SCAN_CTRL_RESP, vecParam, vecResp);

    LOG4CPLUS_INFO_FMT(g_logger, L"Send slave scan ctrl [ %s ] - %s", TOWS(::ToString(t)), TOWS(IS_ERR(res) ? "failed" : "succeed"));

    return IS_ERR(res) ? res : ErrorCode(std::stoi(vecResp[1]));
}

//获取当前使用的管电压
ErrorCode UScanInfoServer::GetKV(float &fKV)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::GET_KV;
    std::string strRespCMD = CArmIPCCMD::GET_KV_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    fKV = std::stof(vecResp[2]);

    return  (ErrorCode)std::stoi(vecResp[1]);
}

//获取当前使用的管电流
ErrorCode UScanInfoServer::GetMA(float &fMA)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::GET_MA;
    std::string strRespCMD = CArmIPCCMD::GET_MA_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    fMA = std::stof(vecResp[2]);

    return  (ErrorCode)std::stoi(vecResp[1]);
}

ErrorCode UScanInfoServer::GetBulbTubeTemp(float &fTemp)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_3IN1_DEV_NOT_READY;

    std::string strCMD = CArmIPCCMD::GET_Bulb_Tube_Temp;
    std::string strRespCMD = CArmIPCCMD::AXIS_MOTION_STATUS_CHGD_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    fTemp = std::stof(vecResp[2]);

    return ErrorCode(std::stoi(vecResp[1]));
}

//获取激光灯的开关状态
ErrorCode UScanInfoServer::GetLaserStatus(OptionLaserSwitch opt, bool &bOn, float &fTime)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = (opt == OptionLaserSwitch::CrossCurve) ? CArmIPCCMD::GET_CROSS_LINE_LSR_STS : CArmIPCCMD::GET_VIEW_RANGE_LSR_STS;
    std::string strRespCMD = (opt == OptionLaserSwitch::CrossCurve) ? CArmIPCCMD::GET_CROSS_LINE_LSR_STS_RESP : CArmIPCCMD::GET_VIEW_RANGE_LSR_STS_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    bOn = std::stoi(vecResp[2]) != 0;
    fTime = std::stof(vecResp[3]);

    return  (ErrorCode)std::stoi(vecResp[1]);
}

//获取下位机主板温度
ErrorCode UScanInfoServer::GetMainboardTemp(float &fTemp)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::GET_MB_TEMP;
    std::string strRespCMD = CArmIPCCMD::GET_MB_TEMP_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    fTemp = std::stof(vecResp[2]);

    return  (ErrorCode)std::stoi(vecResp[1]);
}

//发送3D扫描运动轨迹
ErrorCode UScanInfoServer::Set3DTrack(int nNum, const float *pPoints)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::SET_3D_TRACK_DATA;
    std::string strRespCMD = CArmIPCCMD::SET_3D_TRACK_DATA_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(nNum));
    for (int i = 0; i < nNum * 3; i++)
    {
        vecParam.push_back(std::to_string(pPoints[i]));
    }

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

//获取上次3D扫描的运动轨迹
ErrorCode UScanInfoServer::Get3DTrack(WORD &nNum, float **ppPoints)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::FETCH_3D_TRACK;
    std::string strRespCMD = CArmIPCCMD::FETCH_3D_TRACK_RESP;

    std::vector<std::string> vecParam, vecResp;

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    if (IS_ERR(res)) return res;

    nNum = std::stoi(vecResp[2]);
    *ppPoints = new float[nNum * 3];

    for (int i = 0 ; i < nNum * 3; i++)
    {
        (*ppPoints)[i] = std::stof(vecResp[i + 3]);
    }

    return  (ErrorCode)std::stoi(vecResp[1]);
}

//暂停/恢复球管控制
ErrorCode UScanInfoServer::EnableTubeCtrl(bool bEnable)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::string strCMD = CArmIPCCMD::ENABLE_TUBE_CTRL;
    std::string strRespCMD = CArmIPCCMD::ENABLE_TUBE_CTRL_RESP;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(bEnable));

    ErrorCode res = SendCmd2DevProc(strCMD, strRespCMD, vecParam, vecResp);
    return IS_ERR(res) ? res : (ErrorCode)std::stoi(vecResp[1]);
}

ErrorCode UScanInfoServer::GetBatteryPercent(float &fVal)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(fVal));

    ErrorCode res = SendCmd2DevProc(CArmIPCCMD::GET_BATTERY_PERCENT, CArmIPCCMD::GET_BATTERY_PERCENT_RESP, vecParam, vecResp);
    return IS_ERR(res) ? res : ErrorCode(std::stoi(vecResp[1]));
}

ErrorCode UScanInfoServer::RadiationPermissionCtrl(bool bPermission)
{
    if (m_soc == INVALID_SOCKET) return ErrorCode::ERR_SCAN_UI_NO_DEV_CONN;

    std::vector<std::string> vecParam, vecResp;
    vecParam.push_back(std::to_string(bPermission));

    auto res = SendCmd2DevProc(CArmIPCCMD::RADIARION_PERMISSION_CTRL, CArmIPCCMD::RADIARION_PERMISSION_CTRL_RESP, vecParam, vecResp);
    return IS_ERR(res) ? res : ErrorCode(std::stoi(vecResp[1]));
}

#pragma endregion