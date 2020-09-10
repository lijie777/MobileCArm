#include "CArmScanCtrl.h"
#include "CommonUtility.h"
#include "SystemConfigHandler.h"
#include "GlobalMemMapDataDef.h"
#include "IPCDefinitions.h"
#include "Config.h"
#include "CArmScCmdDef.h"
#include "CArmCommon.h"
#include "WorkStationMsgCtrl.h"
#include "IPC_CMD_Def_CArm.h"
#include "DcmUidDef.h"

#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

extern Logger g_logger;

CArmScanCtrl* CArmScanCtrl::GetInstance()
{
    static auto ptr = new CArmScanCtrl;
    return ptr;
}

CArmScanCtrl::CArmScanCtrl()
    : m_status{CArmScanCtrl::NO_SCAN_PARAM}
    , m_isLogin{false}
{

}

void CArmScanCtrl::ExternalCmdHandler(const CArmCmdBase* cmd)
{
    if (UegContains({ SC_CARM_PEDAL_PRESSED ,SC_CARM_EXP_BTN1_PRESSED, SC_CARM_EXP_BTN2_PRESSED }, cmd->Type()))
    {
        if (ExpBtnHandler(cmd))
        {
            WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, cmd);
        }
    }
    else if (auto cmd_ = dynamic_cast<const CArmCmdWorkStationStatus*>(cmd))
    {
        if (CArmWorkStationStatusTypeDef::USER_LOGIN == cmd_->m_arg)
        {
            m_isLogin = true;
        }
        else if (CArmWorkStationStatusTypeDef::USER_LOGOUT == cmd_->m_arg)
        {
            m_isLogin = false;
            m_status = ScanStatus::WAIT_FOR_1_LEVEL_BTN;
        }
    }
    else if (auto cmd_ = dynamic_cast<const NewImgReceived*>(cmd))
    {
        std::memcpy((void*)(cmd_->m_seriesId), m_currentScanCfg.m_seriesID.data(), sizeof(cmd_->m_seriesId));

        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, cmd);

        if (m_currentScanCfg.SetCurrentIndex(cmd_->m_idx))
        {
            m_currentScanCfg.Reset();
            CArmCmdWorkStationStatus cmd__(CArmWorkStationStatusTypeDef::SCAN_FINISH);
            WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd__);
        }
    }
    else if (WS_CARM_SET_SCAN_PARAM == cmd->Type())
    {
        if (auto cmd_ = dynamic_cast<const CArmCmdScanParam*>(cmd))
        {
            if (CArmScanCtrl::NO_SCAN_PARAM == m_status)
            {
                SetScanStatus(CArmScanCtrl::WAIT_FOR_1_LEVEL_BTN);
            }

            m_scanParam.clear();
            m_scanParam.push_back(cmd_->m_arg);

            // 将拍片配置写入共享内存
            GlobalMemMapping gScanConfig;
            GlobalMemMapping::Get(gScanConfig);
            auto& cfg = gScanConfig.RangeCT;
            cfg->fKV[0] = cmd_->m_arg.m_scanParam.m_kVp;
            cfg->fMA[0] = cmd_->m_arg.m_scanParam.m_mA;
            cfg->nExposure[0] = cmd_->m_arg.m_scanParam.m_exposureTime;
            cfg->nFPS = cmd_->m_arg.m_scanParam.m_fps;
            cfg->nFrameNum = cmd_->m_arg.m_scanParam.m_frameCount;
            cfg->nBinningMode = 1; // TODO
            GlobalMemMapping::Set(gScanConfig);

            LOG4CPLUS_INFO_FMT(g_logger, L"Latest Scan Config: kv - %f, mA - %f, FrameCount - %d, FPS - %d, ExposureTime - %d", cfg->fKV[0], cfg->fMA[0], cfg->nFrameNum, cfg->nFPS, cfg->nExposure[0]);

            SetFilePath();
        }
    }
}

std::wstring CArmScanCtrl::GetDarkFilePath()
{
    GlobalMemMapping gScanConfig;
    GlobalMemMapping::Get(gScanConfig);
    return UCommonUtility::GetDarkFileNameW(int(DetectorType::CT), gScanConfig.RangeCT[0].nBinningMode);
}

std::wstring CArmScanCtrl::GetGainFilePath()
{
    GlobalMemMapping gScanConfig;
    GlobalMemMapping::Get(gScanConfig);

    const auto binningMode = gScanConfig.RangeCT[0].nBinningMode;

    PanelCalibType t = (1 == binningMode) ? PanelCalibType::NO_OFFSET_AIR_1_1 : PanelCalibType::NO_OFFSET_AIR_2_2;

    int voltage{};
    int current{};

    const auto cfg = CArmConfigHandler::Deserialize();
    for (auto i : cfg.m_calib.m_panelCalib)
    {
        if (i.CalibType == t)
        {
            voltage = i.Voltage;
            current = i.Current;

            return UCommonUtility::GetGainFileNameW(int(DetectorType::CT), voltage, current, binningMode, false);
        }
    }

    return {};
}

bool CArmScanCtrl::ExpBtnHandler(const CArmCmdBase* cmd)
{
    std::map<CArmScanCtrl::ScanStatus, std::function<void(const CArmCmdBase*)>> mymap;

    mymap[CArmScanCtrl::WAIT_FOR_1_LEVEL_BTN] = [this](const CArmCmdBase* cmd)
    {
        if (SC_CARM_EXP_BTN1_PRESSED == cmd->Type())
        {
            if (auto cmd_ = dynamic_cast<const CArmCmdExpBtn1Pressed*>(cmd))
            {
                if (CArmBtnStatus::Pressed == cmd_->m_arg)
                {
                    if (!Scannable())
                    {
                        LOG4CPLUS_INFO(g_logger, L"Button Level 1 pressed, But it is not scannable.");
                        return;
                    }

                    LOG4CPLUS_INFO(g_logger, L"Exposure btn1 pressed");
                    SetScanStatus(CArmScanCtrl::WAIT_FOR_2_LEVEL_BTN);
                }
            }
        }
        else if (SC_CARM_PEDAL_PRESSED == cmd->Type())
        {
            if (auto cmd_ = dynamic_cast<const CArmCmdPedalPressed*>(cmd))
            {
                if ((CArmExpBtnDef::Level1 == cmd_->m_level) && (CArmBtnStatus::Pressed == cmd_->m_status))
                {
                    if (!Scannable())
                    {
                        LOG4CPLUS_INFO(g_logger, L"Pedal Level 1 pressed, But it is not scannable.");
                        return;
                    }

                    LOG4CPLUS_INFO_FMT(g_logger, L"Pedal %s %s", TOWS(::ToString(cmd_->m_pedal)), TOWS(::ToString(cmd_->m_status)));
                    SetScanStatus(CArmScanCtrl::WAIT_FOR_2_LEVEL_BTN);
                }
            }
        }
    };

    mymap[CArmScanCtrl::WAIT_FOR_2_LEVEL_BTN] = [this](const CArmCmdBase* cmd)
    {
        if (SC_CARM_EXP_BTN1_PRESSED == cmd->Type())
        {
            if (auto cmd_ = dynamic_cast<const CArmCmdExpBtn1Pressed*>(cmd))
            {
                if ((CArmBtnStatus::Released == cmd_->m_arg))
                {
                    LOG4CPLUS_INFO(g_logger, L"Exposure btn1 released");
                    SetScanStatus(CArmScanCtrl::SCAN_CANCEL);
                }
            }
        }
        else if (SC_CARM_EXP_BTN2_PRESSED == cmd->Type())
        {
            if (auto cmd_ = dynamic_cast<const CArmCmdExpBtn2Pressed*>(cmd))
            {
                if ((CArmBtnStatus::Pressed == cmd_->m_arg))
                {
                    LOG4CPLUS_INFO(g_logger, L"Exposure btn2 pressed");
                    SetScanStatus(CArmScanCtrl::IN_SCAN);
                }
            }
        }
        else if (SC_CARM_PEDAL_PRESSED == cmd->Type())
        {
            if (auto cmd_ = dynamic_cast<const CArmCmdPedalPressed*>(cmd))
            {
                if ((CArmExpBtnDef::Level1 == cmd_->m_level) && (CArmBtnStatus::Released == cmd_->m_status))
                {
                    LOG4CPLUS_INFO_FMT(g_logger, L"Pedal %s %s", TOWS(::ToString(cmd_->m_pedal)), TOWS(::ToString(cmd_->m_status)));
                    SetScanStatus(CArmScanCtrl::SCAN_CANCEL);
                }
                else if ((CArmExpBtnDef::Level2 == cmd_->m_level) && (CArmBtnStatus::Pressed == cmd_->m_status))
                {
                    LOG4CPLUS_INFO_FMT(g_logger, L"Pedal %s %s", TOWS(::ToString(cmd_->m_pedal)), TOWS(::ToString(cmd_->m_status)));
                    SetScanStatus(CArmScanCtrl::IN_SCAN);
                }
            }
        }
    };

    mymap[CArmScanCtrl::IN_SCAN] = [this](const CArmCmdBase* cmd)
    {
        if (SC_CARM_EXP_BTN2_PRESSED == cmd->Type())
        {
            if (auto cmd_ = dynamic_cast<const CArmCmdExpBtn2Pressed*>(cmd))
            {
                if ((CArmBtnStatus::Released == cmd_->m_arg))
                {
                    LOG4CPLUS_INFO(g_logger, L"Exposure btn1 released");
                    SetScanStatus(CArmScanCtrl::SCAN_PAUSE);
                }
            }
        }
        else if (SC_CARM_PEDAL_PRESSED == cmd->Type())
        {
            if (auto cmd_ = dynamic_cast<const CArmCmdPedalPressed*>(cmd))
            {
                if ((CArmExpBtnDef::Level2 == cmd_->m_level) && (CArmBtnStatus::Released == cmd_->m_status))
                {
                    LOG4CPLUS_INFO_FMT(g_logger, L"Pedal %s %s", TOWS(::ToString(cmd_->m_pedal)), TOWS(::ToString(cmd_->m_status)));
                    SetScanStatus(CArmScanCtrl::SCAN_PAUSE);
                }
            }
        }
    };

    try
    {
        const auto old_status = m_status;
        (mymap.at(m_status))(cmd);
        return (old_status != m_status);
    }
    catch (const std::out_of_range&)
    {
        return false;
    }
}

bool CArmScanCtrl::SetFilePath()
{
    // 填充明场和暗场文件路径
    static std::wstring strCalibDir = UCommonUtility::GetExeDirW() + CALIB_CONFIG_DIR_CT_W + L"\\";
    GlobalMemMapping gScanConfig;
    GlobalMemMapping::Get(gScanConfig);
    std::wcscpy(gScanConfig.RangeCT[0].sDarkFile, (strCalibDir + GetDarkFilePath()).c_str());
    std::wcscpy(gScanConfig.RangeCT[0].sGainFile, (strCalibDir + GetGainFilePath()).c_str());
    GlobalMemMapping::Set(gScanConfig);

    return true;
}

void CArmScanCtrl::SetScanStatus(const ScanStatus s)
{
    if (s == m_status)
    {
        return;
    }

    const auto old = std::exchange(m_status, s);
    OnScanStatusChanged(m_status, old);
}

void CArmScanCtrl::OnScanStatusChanged(const ScanStatus& current, const ScanStatus& previous)
{
    LOG4CPLUS_INFO_FMT(g_logger, L"Status Changed : current [ %s ], previous [ %s ]", TOWS(ToString(current)), TOWS(ToString(previous)));

    // [上个扫描状态] - [当前扫描状态] - [此种状态跳转对应的处理函数]
    static const std::map<ScanStatus, std::map<ScanStatus, std::function<void()>>> mymap
    {
        {
            ScanStatus::WAIT_FOR_1_LEVEL_BTN, 
            {
                {
                    ScanStatus::WAIT_FOR_2_LEVEL_BTN, [this]
                    {
                        // 设置此次扫描信息
                        m_currentScanCfg.m_seriesID = UDcmUid::CreateImageUID(DeviceType::DR);
                        m_currentScanCfg.frameCount = m_scanParam.at(0).m_scanParam.m_frameCount;
                        m_currentScanCfg.currentFrameIndex = 0;
                        // 发送扫描参数
                        CArmCmdSlaveScanCtrl cmd0(SlaveScanCtrlType::SendScanParam);
                        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd0);
                        // 发送扫描命令
                        CArmCmdSlaveScanCtrl cmd1(SlaveScanCtrlType::Send2DScanCmd_XRay);
                        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd1);
                    }
                }
            }
        },
        {
            ScanStatus::WAIT_FOR_2_LEVEL_BTN,
            {
                {
                    ScanStatus::IN_SCAN, []
                    {
                        // 允许下位机发送射线
                        CArmCmdSlaveScanCtrl cmd(SlaveScanCtrlType::XRayAllow);
                        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd);
                    }
                },
                {
                    ScanStatus::SCAN_CANCEL, [this]
                    {
                        // 取消扫描
                        {
                            CArmCmdSlaveScanCtrl cmd(SlaveScanCtrlType::CancelScan);
                            WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd);
                        }
                        // 判断是否发生扫描结束命令
                        if (IsScanning())
                        {
                            m_currentScanCfg.Reset();
                            CArmCmdWorkStationStatus cmd(CArmWorkStationStatusTypeDef::SCAN_FINISH);
                            WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd);
                        }
                    }
                }
            }
        },
        {
            ScanStatus::IN_SCAN,
            {
                {
                    ScanStatus::SCAN_PAUSE, []
                    {
                        // 不允许下位机发送射线
                        CArmCmdSlaveScanCtrl cmd(SlaveScanCtrlType::XRayNotAllow);
                        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd);
                    }
                }
            }
        },
        {
            ScanStatus::SCAN_PAUSE,
            {
                {
                    ScanStatus::IN_SCAN, []
                    {
                        // 允许下位机发送射线
                        CArmCmdSlaveScanCtrl cmd(SlaveScanCtrlType::XRayAllow);
                        WORK_STATION_MSG_CTRL->Request(ClientType::SCAN_CTRL, &cmd);
                    }
                },
                {
                    ScanStatus::WAIT_FOR_1_LEVEL_BTN, []
                    {

                    }
                }
            }
        }
    };

    try
    {
        (mymap.at(previous).at(current))();
    }
    catch (const std::out_of_range&)
    {
        LOG4CPLUS_INFO_FMT(g_logger, L"===> Cannot handle transfer [ %s ] to [ %s ]", TOWS(ToString(previous)), TOWS(ToString(current)));
    }
}

CArmScanCtrl::ScanStatus CArmScanCtrl::CurrentStatus() const
{
    return m_status;
}

bool CArmScanCtrl::IsScanning() const
{
    return m_currentScanCfg.Valid();
}
