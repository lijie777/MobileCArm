#pragma once

#include <string>
#include <vector>
#include "CArmScCmdDef.h"

class CArmCmdBase;

#define CARM_SCAN_CTRL CArmScanCtrl::GetInstance()

class CArmScanCtrl final
{
public:
    static CArmScanCtrl* GetInstance();

    void ExternalCmdHandler(const CArmCmdBase* cmd);

    enum ScanStatus
    {
        NO_SCAN_PARAM,                      //!< 空闲状态
        WAIT_FOR_1_LEVEL_BTN,               //!< 等待一级按钮按下
        WAIT_FOR_2_LEVEL_BTN,               //!< 等待二级按钮按下
        IN_SCAN,                            //!< 在扫描中
        SCAN_PAUSE = WAIT_FOR_2_LEVEL_BTN,  //!< 扫描暂停状态
        SCAN_CANCEL = WAIT_FOR_1_LEVEL_BTN, //!< 扫描取消状态
    };

private:
    CArmScanCtrl();


    /**
     * @brief 获取当前的扫描状态
     * @return CArmScanCtrl::ScanStatus 当前扫描状态
     */
    ScanStatus CurrentStatus() const;

    /**
     * @brief 设置当前扫描状态
     * @param s 最新的扫描状态
     * @return void 
     */
    void SetScanStatus(const ScanStatus s);

    /**
     * @brief 将扫描状态枚举转化为可读字符串
     * @param val 扫描状态枚举值
     */
    std::string ToString(const ScanStatus val) const
    {
        static std::map<ScanStatus, std::string> mymap
        {
            { ScanStatus::NO_SCAN_PARAM, "NO_SCAN_PARAM"},
            { ScanStatus::WAIT_FOR_1_LEVEL_BTN, "WAIT_FOR_1_LEVEL_BTN"},
            { ScanStatus::WAIT_FOR_2_LEVEL_BTN, "WAIT_FOR_2_LEVEL_BTN"},
            { ScanStatus::IN_SCAN, "IN_SCAN" },
            { ScanStatus::SCAN_PAUSE, "SCAN_PAUSE"},
            { ScanStatus::SCAN_CANCEL, "SCAN_CANCEL"}
        };

        return mymap[val];
    }

    std::wstring GetDarkFilePath();
    std::wstring GetGainFilePath();

    /**
     * @brief 判断此曝光按钮是否有效
     * @param cmd 带有按钮信息的命令
     * @return bool 按钮信息是否有效
     */
    bool ExpBtnHandler(const CArmCmdBase* cmd);

    bool SetFilePath();

    void OnScanStatusChanged(const ScanStatus& current, const ScanStatus& previous);

    /**
     * @brief 判断当前是否处于扫描中
     * @return bool 
     */
    bool IsScanning() const;

    struct CurrentScanAttr
    {
        std::string m_seriesID;
        std::size_t frameCount;
        std::size_t currentFrameIndex;

        /**
         * @brief 设置当前的帧索引
         * @param idx 当前帧索引
         * @return bool 是否已经接收完全
         */
        bool SetCurrentIndex(const std::size_t idx)
        {
            currentFrameIndex = idx;
            return (currentFrameIndex >= (frameCount - 1));
        }

        void Reset()
        {
            m_seriesID.clear();
            frameCount = 0;
            currentFrameIndex = 0;
        }

        bool Valid() const
        {
            return (!m_seriesID.empty() && frameCount);
        }
    };

    bool Scannable()
    {
        return (/*m_isLogin && */!m_scanParam.empty());
    }

private:
    mutable ScanStatus m_status;                    //!< 当前的扫描状态
    std::vector<CArmLastScanConfig> m_scanParam;
    CurrentScanAttr m_currentScanCfg;
    bool m_isLogin;                                 //!< 是否处于登录状态
};