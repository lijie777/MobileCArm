#pragma once

#include <memory>

#include "CArmScCmdDef.h"

template <typename T>
void CArmMemCpy(T* obj, const CArmRawCmd& cmd)
{
    static_assert(std::is_pod<decltype(obj->m_arg)>::value, "This function applies only to POD types");

    const auto ptr = ((const char*)(cmd.m_pu8Data));
    std::memcpy(&(obj->m_arg), ptr, sizeof(obj->m_arg));
}

template <typename T>
T* CreateInitObj(const CArmRawCmd& cmd)
{
    auto req = new T({});
    ::CArmMemCpy(req, cmd);
    return req;
};

static std::list<std::unique_ptr<CArmCmdBase>> ToConcreteCmds(const std::list<CArmRawCmd>& cmds)
{
    const static auto mymap = []
    {
        std::map<int, std::function<CArmCmdBase*(const CArmRawCmd&)>> mymap;

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto status = CArmBtnStatus(*(cmd.m_pu8Data));
                return new CArmCmdExpBtn1Pressed(status);
            };
            mymap.insert({ SC_CARM_EXP_BTN1_PRESSED , f});
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto status = CArmBtnStatus(*(cmd.m_pu8Data));
                return new CArmCmdExpBtn2Pressed(status);
            };
            mymap.insert({ SC_CARM_EXP_BTN2_PRESSED , f});
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto ePedal = CArmPedalDef(*(cmd.m_pu8Data));
                const auto level = CArmExpBtnDef(*(cmd.m_pu8Data + 1));
                const auto status = CArmBtnStatus(*(cmd.m_pu8Data + 2));
                return new CArmCmdPedalPressed(ePedal, level, status);
            };
            mymap.insert({ SC_CARM_PEDAL_PRESSED, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto eMotion = (CArmAxisMotionDef)(*(cmd.m_pu8Data));
                return new CArmCmdMotionStatusZ(eMotion);
            };
            mymap.insert({ SC_CARM_LIFT_STS_CHANGED, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto eMotion = (CArmAxisMotionDef)(*(cmd.m_pu8Data));
                return new CArmCmdMotionStatusX(eMotion);
            };
            mymap.insert({ SC_CARM_XAXIS_STS_CHANGED, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto eMotion = (CArmAxisMotionDef)(*(cmd.m_pu8Data));
                return new CArmCmdMotionStatusY(eMotion);
            };
            mymap.insert({ SC_CARM_CARM_STS_CHANGED, f });
        }

        {
            auto f = [](const CArmRawCmd&) { return new CArmCmdXRayReady; };
            mymap.insert({ SC_CARM_XRAY_READY,  f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto err = ErrorCode(*((WORD*)(cmd.m_pu8Data)));
                return new CArmCmdDevWarnReport((unsigned short)err);
            };
            mymap.insert({SC_CARM_WARN_REPORT, f});
        }

//         {
//             auto f = [](const CArmRawCmd& cmd)
//             {
//                 const auto fPos = *((float*)(cmd.m_pu8Data));
//                 return new CArmCmdCollimatorPosH(fPos);
//             };
//             mymap.insert({ SC_CARM_COL_IN_PLACE_HOR, f });
//         }

//         {
//             auto f = [](const CArmRawCmd& cmd)
//             {
//                 const auto fPos = *((float*)(cmd.m_pu8Data));
//                 return new CArmCmdCollimatorPosV(fPos);
//             };
//             mymap.insert({ SC_CARM_COL_IN_PLACE_VER, f });
//         }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto eAxisCtrl = CArmAxisCtrlDef(*(cmd.m_pu8Data));
                return new CArmCmdAMChangedY(eAxisCtrl);
            };
            mymap.insert({ SC_CARM_MANU_AUTO_CHGD_CARM, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto eAxisCtrl = CArmAxisCtrlDef(*(cmd.m_pu8Data));
                return new CArmCmdAMChangedZ(eAxisCtrl);
            };
            mymap.insert({ SC_CARM_MANU_AUTO_CHGD_Z, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto eAxisCtrl = CArmAxisCtrlDef(*(cmd.m_pu8Data));
                return new CArmCmdAMChangedX(eAxisCtrl);
            };
            mymap.insert({ SC_CARM_MANU_AUTO_CHGD_X, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto status = CArmOnOffDef(*(cmd.m_pu8Data));
                return new CArmCmdLaserStatusChangedCross(status);
            };
            mymap.insert({ SC_CARM_LSR_STS_CHANGED_CROSS, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto status = CArmOnOffDef(*(cmd.m_pu8Data));
                return new CArmCmdLaserStatusChangedView(status);
            };
            mymap.insert({ SC_CARM_LSR_STS_CHANGED_VIEW, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto fPos = *((float*)(cmd.m_pu8Data));
                return new CArmCmdYCurPos(fPos);
            };
            mymap.insert({ SC_CARM_CURR_POS_CARM, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto fPos = *((float*)(cmd.m_pu8Data));
                return new CArmCmdZCurPos(fPos);
            };
            mymap.insert({ SC_CARM_CURR_POS_Z, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto fPos = *((float*)(cmd.m_pu8Data));
                return new CArmCmdXCurPos(fPos);
            };
            mymap.insert({ SC_CARM_CURR_POS_X, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                const auto err = ErrorCode(*((WORD*)(cmd.m_pu8Data)));
                return new CArmCmdDevResetDone((unsigned short)err);
            };
            mymap.insert({ SC_CARM_DEV_RESET_RES, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto patientInfo_ = new CArmCmdNewPatientInfo({});
//                 const auto ptr = ((const char*)(cmd.m_pu8Data));
//                 std::memcpy(&(patientInfo_->m_arg), ptr, sizeof(patientInfo_->m_arg));
                ::CArmMemCpy(patientInfo_, cmd);
                return patientInfo_;
            };
            mymap.insert({ WS_CARM_NEW_PATIENT_INFO, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto cmd_ = new CArmCmdWorkStationStatus({});
                ::CArmMemCpy(cmd_, cmd);
                return cmd_;
            };
            mymap.insert({ WS_CARM_IPC_WS_STATUS, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto imgTransfer = new CArmCmdImgTransfer({});
                ::CArmMemCpy(imgTransfer, cmd);
                return imgTransfer;
            };
            mymap.insert({ WS_CARM_IMG_TRANSFER, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto scanParam = new CArmCmdScanParam({});
                ::CArmMemCpy(scanParam, cmd);
                return scanParam;
            };
            mymap.insert({ WS_CARM_SET_SCAN_PARAM, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto imgAdjustParam = new CArmCmdImgAdjustParam({});
                ::CArmMemCpy(imgAdjustParam, cmd);
                return imgAdjustParam;
            };
            mymap.insert({ WS_CARM_IMG_ADJUST_PARAM, f });
        }

//         {
//             auto f = [](const CArmRawCmd& cmd)
//             {
//                 auto bulbTubeTemp = new CArmCmdBulbTubeTempResp({});
//                 ::CArmMemCpy(bulbTubeTemp, cmd);
//                 return bulbTubeTemp;
//             };
//             mymap.insert({ SC_CARM_GET_BULB_TUBE_TEMP, f });
//         }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto radiationPermission = new CArmCmdRadiationPermission({});
                ::CArmMemCpy(radiationPermission, cmd);
                return radiationPermission;
            };
            mymap.insert({ SC_CARM_RADIATION_PERMISSION , f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto open_ = new CArmCmdRasterStatusChanged({});
                ::CArmMemCpy(open_, cmd);
                return open_;
            };
            mymap.insert({ SC_CARM_RASTER_STATUS_CHANGED, f });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto req = new CArmCmdDeviceValue({});
                ::CArmMemCpy(req, cmd);
                return req;
            };
            mymap.insert({ WS_CARM_IPC_DEVICE_VALUE , f});
        }

//         {
//             auto f = [](const CArmRawCmd& cmd)
//             {
//                 auto req = new CArmCmdRequest({});
//                 ::CArmMemCpy(req, cmd);
//                 return req;
//             };
//             mymap.insert({ WS_CARM_IPC_REQUEST , f });
//         }

        {
            mymap.insert({ WS_CARM_LOGIN_STATUS_CHANGED, ::CreateInitObj<CArmCmdLoginStatusChanged> });
            mymap.insert({ WS_CARM_SCAN_FINISHED, [](auto cmd) { return new CArmCmdScanFinished{}; } });
        }

        {
            auto f = [](const CArmRawCmd& cmd)
            {
                auto cmd_ = new NewImgReceived;
                const auto ptr = ((const char*)(cmd.m_pu8Data));
                const auto len = (char*)(&(cmd_->m_img)) - (char*)(&(cmd_->m_imgId));
                std::memcpy(&(cmd_->m_seriesId), ptr, len);
                cmd_->m_img = new std::uint16_t[cmd_->m_w*cmd_->m_h]{};
                std::memcpy(cmd_->m_img, ptr + len, cmd_->m_w*cmd_->m_h*sizeof(*(cmd_->m_img)));
                AdjustCmdLenField(cmd_);
                return cmd_;
            };
            mymap.insert({ WS_CARM_NEW_IMG_RECEIVED, f });
        }

        return mymap;
    }();

    std::list<std::unique_ptr<CArmCmdBase>> concreateCmds;

    for (auto& i : cmds)
    {
        CArmCmdBase* pCmd{};

        try
        {
            pCmd = (mymap.at(i.m_u16Cmd))(i);
        }
        catch (const std::out_of_range&)
        {
            pCmd = new CArmRawCmd(i);
        }

        if (pCmd)
        {
            concreateCmds.push_back(std::unique_ptr<CArmCmdBase>(pCmd));
        }
    }

    return concreateCmds;
}

/// 擦除消息list种的心跳消息，并告知是否执行了至少一次擦除
static bool EraseHB(std::list<CArmRawCmd>& cmds)
{
    bool contaionHB = false;

    while (true)
    {
        const auto iter = std::find_if(std::begin(cmds), std::end(cmds), [](const CArmRawCmd& cmd)
        {
            return (WS_CARM_NET_HB == cmd.m_u16Cmd);
        });

        if (std::end(cmds) == iter)
        {
            break;
        }

        cmds.erase(iter);
        contaionHB = true;
    }

    return contaionHB;
}

static std::list<CArmRawCmd> ExtractCommands(std::vector<std::uint8_t> &recvBuff)
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
            std::memcpy(pCmdData, pData + u16BaseCmdLen, u16ParamLen);
        }

        ret.push_back(CArmRawCmd(u16MsgLen, u16CmdID, u16Cmd, pCmdData, u16ParamLen));

        recvBuff.erase(recvBuff.begin(), recvBuff.begin() + u16MsgLen);
    }

    return ret;
}