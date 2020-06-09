#include "CArmKVp_mA.h"
#include "CArmManager.h"

constexpr float MIX_MA = 5.6;
constexpr float MAX_MA = 16.0;
constexpr float MAX_KV = 90.0;
constexpr float MIX_KV = 50.0;
constexpr float EPS = 0.000001;

CArmKVp_mA::CArmKVp_mA(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    init();
}

CArmKVp_mA::~CArmKVp_mA()
{
}
void CArmKVp_mA::setkV_mA(const CArmLastScanConfig & config)
{
    ui.kVpLbe->setText(QString::number(config.m_scanParam.m_kVp, 'f', 1));
    ui.mALbe->setText(QString::number(config.m_scanParam.m_mA, 'f', 1));
}
void CArmKVp_mA::setAuto(const CArmLastScanConfig & config)
{
    if (config.m_auto)
    {
        ui.autoBtn->setChecked(true);
        ui.manualBtn->setChecked(false);
    }
    else
    {
        ui.autoBtn->setChecked(false);
        ui.manualBtn->setChecked(true);
    }
}
// 设置电流电压
void CArmKVp_mA::setkV_mA(const CArmScanConfig &scanConfig)
{
    int scanModeIndex = int(scanConfig.m_defaultScanMode);
    int doseTypeIndex = int(scanConfig.m_scanModeArray[scanModeIndex].m_defaultDoseType);
    //int typeIndex = int(scanConfig.m_scanModeArray[scanModeIndex].m_doseArray[doseTypeIndex].m_defaultType);

    float kV = scanConfig.m_scanModeArray[scanModeIndex].m_scanParamArray[doseTypeIndex].m_kVp;
    float mA = scanConfig.m_scanModeArray[scanModeIndex].m_scanParamArray[doseTypeIndex].m_mA;

    ui.kVpLbe->setText(QString::number(kV, 'f', 1));
    ui.mALbe->setText(QString::number(mA, 'f', 1));
}

void CArmKVp_mA::init(void)
{
    // 自动设置电流电压/手动设置电流电压
    connect(ui.autoBtn, &QPushButton::clicked, [this]() {autoBtnClicked(); });
    connect(ui.manualBtn, &QPushButton::clicked, [this]() {manualBtnClicked(); });

    // 增加或者减小电流电压
    connect(ui.kvpAddBtn, &QPushButton::clicked, [this]() {kvpAddBtnClicked(); });
    connect(ui.kvpReduceBtn, &QPushButton::clicked, [this]() {kvpReduceBtnClicked(); });
    connect(ui.mAAddBtn, &QPushButton::clicked, [this]() {mAAddBtnClicked(); });
    connect(ui.mAReduceBtn, &QPushButton::clicked, [this]() {mAReduceBtnClicked(); });

}

// 自动按钮按下
void CArmKVp_mA::autoBtnClicked(void)
{
    if (ui.manualBtn->isChecked())
    {
        ui.autoBtn->setChecked(true);
        ui.manualBtn->setChecked(false);

        float kV;
        float mA;
        CArmManager::getKv_mA(kV, mA);

        ui.kVpLbe->setText(QString::number(kV, 'f', 1));
        ui.mALbe->setText(QString::number(mA, 'f', 1));

        if (CArmManager::scanConfigConvert() != nullptr)
        {
            CArmManager::scanConfigConvert()->m_auto = true;
            CArmManager::scanConfigConvert()->m_scanParam.m_kVp = kV;
            CArmManager::scanConfigConvert()->m_scanParam.m_mA = mA;
            CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
            // 发送信号告诉框架更新底部病人信息,并发送扫描参数给工作站
            emit scanInfoUpdateSignal(*CArmManager::scanConfigConvert());
        }

        m_bAuto = true;
    }
}
// 手动按钮按下
void CArmKVp_mA::manualBtnClicked(void)
{
    if (ui.autoBtn->isChecked())
    {
        ui.manualBtn->setChecked(true);
        ui.autoBtn->setChecked(false);

        CArmManager::scanConfigConvert()->m_auto = false;
        if (CArmManager::scanConfigConvert() != nullptr)
        {
            // 发送信号告诉框架更新底部病人信息,并发送扫描参数给工作站
            emit scanInfoUpdateSignal(*CArmManager::scanConfigConvert());
        }

        m_bAuto = false;
    }
}

// 电压增加按钮按下
void CArmKVp_mA::kvpAddBtnClicked(void)
{
    if (ui.autoBtn->isChecked())
    {
        ui.manualBtn->setChecked(true);
        ui.autoBtn->setChecked(false);
    }
    float kV_value = ui.kVpLbe->text().toFloat() - MAX_KV;
    if (kV_value <= -EPS)
    {
        ui.kVpLbe->setText(QString::number((ui.kVpLbe->text().toFloat() + 1.0), 'f', 1));
        writeToFileAndSendData(CArmKVp_mA::KV_MODE);
    }

}
// 电压减小按钮按下
void CArmKVp_mA::kvpReduceBtnClicked(void)
{
    if (ui.autoBtn->isChecked())
    {
        ui.manualBtn->setChecked(true);
        ui.autoBtn->setChecked(false);
    }
    float kV_value = ui.kVpLbe->text().toFloat() - MIX_KV;
    if (kV_value >= EPS)
    {
        ui.kVpLbe->setText(QString::number((ui.kVpLbe->text().toFloat() - 1.0), 'f', 1));
        writeToFileAndSendData(CArmKVp_mA::KV_MODE);
    }
}
// 电流增加按钮按下
void CArmKVp_mA::mAAddBtnClicked(void)
{
    if (ui.autoBtn->isChecked())
    {
        ui.manualBtn->setChecked(true);
        ui.autoBtn->setChecked(false);
    }
    float mA_value = ui.mALbe->text().toFloat() - MAX_MA;
    if (mA_value <= -EPS)
    {
        ui.mALbe->setText(QString::number((ui.mALbe->text().toFloat() + 1.0), 'f', 1));
        writeToFileAndSendData(CArmKVp_mA::MA_MODE);
    }
}
// 电流减小按钮按下
void CArmKVp_mA::mAReduceBtnClicked(void)
{
    if (ui.autoBtn->isChecked())
    {
        ui.manualBtn->setChecked(true);
        ui.autoBtn->setChecked(false);
    }
    float mA_value = ui.mALbe->text().toFloat() - MIX_MA;

    if (mA_value >= EPS)
    {
        ui.mALbe->setText(QString::number((ui.mALbe->text().toFloat() - 1.0), 'f', 1));
        writeToFileAndSendData(CArmKVp_mA::MA_MODE);
    }

}

void CArmKVp_mA::writeToFileAndSendData(CArmKVp_mA::MODE mode)
{
    if (mode == CArmKVp_mA::KV_MODE)
    {       
        CArmManager::scanConfigConvert()->m_scanParam.m_kVp = ui.kVpLbe->text().toFloat();   
    }
    if (mode == CArmKVp_mA::MA_MODE)
    {
        CArmManager::scanConfigConvert()->m_scanParam.m_mA = ui.mALbe->text().toFloat();
    }
    if (CArmManager::scanConfigConvert() != nullptr)
    {
        CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
        // 发送信号告诉框架更新底部病人信息,并发送扫描参数给工作站
        emit scanInfoUpdateSignal(*CArmManager::scanConfigConvert());
    }

}

void CArmKVp_mA::styleSetting(bool status)
{
    ui.kvpAddBtn->setEnabled(status);
    ui.kvpReduceBtn->setEnabled(status);
    ui.mAAddBtn->setEnabled(status);
    ui.mAReduceBtn->setEnabled(status);
}
