#include "CArmCaptureMode.h"
#include "CArmData.h"
#include "CArmDataFlow.h"
#include <QGridLayout>
#include <fstream>
#include "cereal/archives/xml.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/vector.hpp"

#pragma execution_character_set("utf-8")

CArmCaptureMode::CArmCaptureMode(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);

    init();



}

CArmCaptureMode::~CArmCaptureMode()
{
    if (m_pPartChooseDialog != nullptr)
    {
        delete m_pPartChooseDialog;
        m_pPartChooseDialog = nullptr;
    }
    if (m_pPulseChooseDialog != nullptr)
    {
        delete m_pPulseChooseDialog;
        m_pPulseChooseDialog = nullptr;
    }
}

void CArmCaptureMode::init(void)
{
    // 身体部位选择界面初始化
    m_pPartChooseDialog = new CArmPartChooseDialog(this);

    // 计量选择界面初始化
    m_pPulseChooseDialog = new CArmPulseChooseDialog;
    QGridLayout *layout = new QGridLayout;
    layout->addWidget(m_pPulseChooseDialog);
    layout->setMargin(0);
    ui.pulseLbe->setLayout(layout);

    // 部位选择界面
    connect(ui.togglePartBtn, &QPushButton::clicked, this, &CArmCaptureMode::chooseBodyPartSlot);
    connect(ui.partLbe, &QPushButton::clicked, this, &CArmCaptureMode::chooseBodyPartSlot);
    connect(ui.togglePart_2Btn, &QPushButton::clicked, this, &CArmCaptureMode::chooseBodyPartSlot);
    // 选择某个部位后的处理
    connect(m_pPartChooseDialog, &CArmPartChooseDialog::bodyPartSignal, this, &CArmCaptureMode::bodyPartShowSlot);

    // 从配置文件读取默认身体部位
   // setDefaultBodyPart();

    // 拍摄模式选择（点拍，脉冲透视，视频）后显示对应的脉冲频率，剂量及电压电流
    connect(ui.pointCaptureBtn, &QPushButton::clicked, [this]() {pointCaptureBtnClicked(); });
    connect(ui.pulsePerspectiveBtn, &QPushButton::clicked, [this]() {pulsePerspectiveBtnClicked(); });
    connect(ui.cineBtn, &QPushButton::clicked, [this]() {cineBtnClicked(); });

    // 剂量选择（标准剂量，增强，儿童）后电压电流会发生变化
    connect(m_pPulseChooseDialog->ui.normalDoseBtn, &QPushButton::clicked, [this]() {normalDoseBtnClicked(); });
    connect(m_pPulseChooseDialog->ui.enhanceDoseBtn, &QPushButton::clicked, [this]() {enhanceDoseBtnClicked(); });
    connect(m_pPulseChooseDialog->ui.childDoseBtn, &QPushButton::clicked, [this]() {childDoseBtnClicked(); });

    // 修改脉冲频率
    connect(m_pPulseChooseDialog, &CArmPulseChooseDialog::ppsValueSignal, [this](int ppsValue) {ppsValueChoose(ppsValue); });

}

void CArmCaptureMode::ppsValueChoose(int ppsValue)
{
    CArmManager::scanConfigConvert()->m_pps = ppsValue;
    CArmManager::scanConfigConvert()->m_scanParam.m_exposureTime = 1.0 / ppsValue * 1000;
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
    // 发送信号告诉框架更新底部病人信息,并发送扫描参数给工作站
    emit scanInfoUpdateSignal(*CArmManager::scanConfigConvert());

}

void CArmCaptureMode::pointCaptureBtnClicked(void)
{
    if ((!ui.pulsePerspectiveBtn->isChecked() || !ui.cineBtn->isChecked()))
    {
        SCANSETTING_STYLE_STRUCT scanStyle;
        ui.pointCaptureBtn->setChecked(true);
        ui.pulsePerspectiveBtn->setChecked(false);
        ui.cineBtn->setChecked(false);
        ui.pulseLbe->setStyleSheet(scanStyle.underFrameLeft);

        // 设置扫描模式来计算剩余剂量能拍的帧数
        CArmManager::setScanMode(ScanMode::POINT);

        scanModeHandle(ScanMode::POINT);

        // 不能选择脉冲频率
        setPPSBtnStatus(false);
    }
}


void CArmCaptureMode::pulsePerspectiveBtnClicked(void)
{
    if ((!ui.pointCaptureBtn->isChecked() || !ui.cineBtn->isChecked()))
    {
        SCANSETTING_STYLE_STRUCT scanStyle;
        ui.pulsePerspectiveBtn->setChecked(true);
        ui.pointCaptureBtn->setChecked(false);
        ui.cineBtn->setChecked(false);
        ui.pulseLbe->setStyleSheet(scanStyle.underFrameMiddle);

        // 设置扫描模式来计算剩余剂量能拍的帧数
        CArmManager::setScanMode(ScanMode::FLUORO_MPP);

        scanModeHandle(ScanMode::FLUORO_MPP);

        setPPSBtnStatus(true);
    }
}


void CArmCaptureMode::cineBtnClicked(void)
{
    if ((!ui.pointCaptureBtn->isChecked() || !ui.pulsePerspectiveBtn->isChecked()) )
    {
        SCANSETTING_STYLE_STRUCT scanStyle;
        ui.cineBtn->setChecked(true);
        ui.pointCaptureBtn->setChecked(false);
        ui.pulsePerspectiveBtn->setChecked(false);
        ui.pulseLbe->setStyleSheet(scanStyle.underFrameRight);

        // 设置扫描模式来计算剩余剂量能拍的帧数
        CArmManager::setScanMode(ScanMode::VIDEO);

        scanModeHandle(ScanMode::VIDEO);

        setPPSBtnStatus(true);
    }
}

void CArmCaptureMode::normalDoseBtnClicked(void)
{
    if ((!m_pPulseChooseDialog->ui.enhanceDoseBtn->isChecked() || !m_pPulseChooseDialog->ui.childDoseBtn->isChecked()))
    {
        m_pPulseChooseDialog->ui.normalDoseBtn->setChecked(true);
        m_pPulseChooseDialog->ui.enhanceDoseBtn->setChecked(false);
        m_pPulseChooseDialog->ui.childDoseBtn->setChecked(false);

        // 扫描剂量处理
        scanDoseHandle(DoseType::REGULAR);
    }
}


void CArmCaptureMode::enhanceDoseBtnClicked(void)
{
    if ((!m_pPulseChooseDialog->ui.normalDoseBtn->isChecked() || !m_pPulseChooseDialog->ui.childDoseBtn->isChecked()))
    {
        m_pPulseChooseDialog->ui.enhanceDoseBtn->setChecked(true);
        m_pPulseChooseDialog->ui.normalDoseBtn->setChecked(false);
        m_pPulseChooseDialog->ui.childDoseBtn->setChecked(false);

        // 扫描剂量处理
        scanDoseHandle(DoseType::HIGH);
    }
}


void CArmCaptureMode::childDoseBtnClicked(void)
{
    if ((!m_pPulseChooseDialog->ui.enhanceDoseBtn->isChecked() || !m_pPulseChooseDialog->ui.normalDoseBtn->isChecked()))
    {
        m_pPulseChooseDialog->ui.childDoseBtn->setChecked(true);
        m_pPulseChooseDialog->ui.enhanceDoseBtn->setChecked(false);
        m_pPulseChooseDialog->ui.normalDoseBtn->setChecked(false);

        // 扫描剂量处理
        scanDoseHandle(DoseType::LOW);
    }
}

// 默认扫描部位
void CArmCaptureMode::scanPartUpdate(const ScanPart& part)
{
    SCANSETTING_STYLE_STRUCT scanStyle;

    switch (part)
    {
    case ScanPart::UNKNOWN:
        // do nothing
        break;
    case ScanPart::LIMBS:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.limbsStyle_check);
        ui.partLbe->setText(tr("四肢"));
        // 读取文件寻找最佳匹配数据
        bodyPartChanged(ScanPart::LIMBS);
        break;
    case ScanPart::NECK:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.neckStyle_check);
        ui.partLbe->setText(tr("颈部"));
        
        bodyPartChanged(ScanPart::NECK);
        break;
    case ScanPart::HEAD:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.headStyle_check);
        ui.partLbe->setText(tr("头部"));
        
        bodyPartChanged(ScanPart::HEAD);
        break;
    case ScanPart::TRUNCK:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.trunkStyle_check);
        ui.partLbe->setText(tr("躯干"));
        
        bodyPartChanged(ScanPart::TRUNCK);
        break;
    case ScanPart::HEART:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.heartStyle_check);
        ui.partLbe->setText(tr("HEART"));
        
        bodyPartChanged(ScanPart::HEAD);
        break;
    case ScanPart::ABDOMEN:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.abdomenStyle_check);
        ui.partLbe->setText(tr("ABDOMEN"));

        bodyPartChanged(ScanPart::HEAD);
        break;
    case ScanPart::URINARY:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.urinaryStyle_check);
        ui.partLbe->setText(tr("泌尿系统"));

        bodyPartChanged(ScanPart::URINARY);
        break;
    case ScanPart::SOFT_TISSUE:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.soft_tissueStyle_check);
        ui.partLbe->setText(tr("软组织"));

        bodyPartChanged(ScanPart::SOFT_TISSUE);
        break;
    case ScanPart::ENDOSCOPE:
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.endoscopeStyle_check);
        ui.partLbe->setText(tr("内窥镜"));

        bodyPartChanged(ScanPart::ENDOSCOPE);
        break;
    default:
        break;
    }

}
void CArmCaptureMode::setPPS(int pps)
{
    m_pPulseChooseDialog->ui.PPSBtn->setText(QString::number(pps) + QString("PPS"));
}

void CArmCaptureMode::setDose(const DoseType &doseMode)
{
    switch (doseMode)
    {
    case DoseType::REGULAR:
        m_pPulseChooseDialog->ui.normalDoseBtn->setChecked(true);
        m_pPulseChooseDialog->ui.enhanceDoseBtn->setChecked(false);
        m_pPulseChooseDialog->ui.childDoseBtn->setChecked(false);
        break;
    case DoseType::HIGH:
        m_pPulseChooseDialog->ui.enhanceDoseBtn->setChecked(true);
        m_pPulseChooseDialog->ui.normalDoseBtn->setChecked(false);
        m_pPulseChooseDialog->ui.childDoseBtn->setChecked(false);
        break;
    case DoseType::LOW:
        m_pPulseChooseDialog->ui.childDoseBtn->setChecked(true);
        m_pPulseChooseDialog->ui.normalDoseBtn->setChecked(false);
        m_pPulseChooseDialog->ui.enhanceDoseBtn->setChecked(false);
        break;

    default:
        break;
    }
}
void CArmCaptureMode::setScanMode(const ScanMode &scanMode)
{
    SCANSETTING_STYLE_STRUCT scanStyle;
    switch (scanMode)
    {
    case ScanMode::POINT:
        ui.pointCaptureBtn->setChecked(true);
        ui.pulseLbe->setStyleSheet(scanStyle.underFrameLeft);
        ui.pulsePerspectiveBtn->setChecked(false);
        ui.cineBtn->setChecked(false);
        break;
    case ScanMode::FLUORO_MPP:
        ui.pulsePerspectiveBtn->setChecked(true);
        ui.pulseLbe->setStyleSheet(scanStyle.underFrameMiddle);
        ui.pointCaptureBtn->setChecked(false);
        ui.cineBtn->setChecked(false);
        break;
    case ScanMode::VIDEO:
        ui.cineBtn->setChecked(true);
        ui.pulseLbe->setStyleSheet(scanStyle.underFrameRight);
        ui.pointCaptureBtn->setChecked(false);
        ui.pulsePerspectiveBtn->setChecked(false);
        break;

    default:
        break;
    }
}

// 选择了一个扫描部位
void CArmCaptureMode::bodyPartChanged(const ScanPart &scanPart)
{
    CArmManager::scanConfigConvert(scanPart)->m_auto = true;
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
    CArmLastScanConfig lastScanConfig = *CArmManager::scanConfigConvert();
    // 设置电流电压
    emit kVmASettingSignal(lastScanConfig);
    // 设置自动手动
    emit autoSignal(lastScanConfig);
    // 设置扫描模式
    setScanMode(lastScanConfig.m_scanModeType);
    // 设置剂量模式
    setDose(lastScanConfig.m_doseType);
    // 设置PPS
    setPPS(lastScanConfig.m_pps);

    // 发送数据给工作站
    scanInfoUpdateSignal(lastScanConfig);

}

// 手动选择一个部位
void CArmCaptureMode::bodyPartShowSlot(CArmPartWidget* partWidget)
{
    m_pPartChooseDialog->hide();

    SCANSETTING_STYLE_STRUCT scanStyle;
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[0])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.limbsStyle_check);
        ui.partLbe->setText(tr("四肢"));
        // 读取文件寻找最佳匹配数据
        bodyPartChanged(ScanPart::LIMBS);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[1])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.neckStyle_check);
        ui.partLbe->setText(tr("颈部"));

        bodyPartChanged(ScanPart::NECK);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[2])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.headStyle_check);
        ui.partLbe->setText(tr("头部"));

        bodyPartChanged(ScanPart::HEAD);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[3])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.trunkStyle_check);
        ui.partLbe->setText(tr("躯干"));

        bodyPartChanged(ScanPart::TRUNCK);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[4])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.heartStyle_check);
        ui.partLbe->setText(tr("心脏"));

        bodyPartChanged(ScanPart::HEART);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[5])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.abdomenStyle_check);
        ui.partLbe->setText(tr("腹部"));

        bodyPartChanged(ScanPart::ABDOMEN);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[6])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.urinaryStyle_check);
        ui.partLbe->setText(tr("泌尿系统"));

        bodyPartChanged(ScanPart::URINARY);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[7])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.soft_tissueStyle_check);
        ui.partLbe->setText(tr("软组织"));

        bodyPartChanged(ScanPart::SOFT_TISSUE);

    }
    if (partWidget == &m_pPartChooseDialog->m_pPartWidget[8])
    {
        ui.togglePartBtn->setStyleSheet(scanStyle.captureModeStyle + scanStyle.endoscopeStyle_check);
        ui.partLbe->setText(tr("内窥镜"));

        bodyPartChanged(ScanPart::ENDOSCOPE);

    }
}

bool CArmCaptureMode::scanPartDialogIsshow()
{ 
    return !m_pPartChooseDialog->isHidden();
}

// 隐藏身体部位选择窗口
void CArmCaptureMode::hideScanPartDialog(void)
{
    m_pPartChooseDialog->hide(); 
}

// 获取脉冲频率选择窗口状态
bool CArmCaptureMode::scanPpsDialogIsshow()
{ 
    return !m_pPulseChooseDialog->m_pDoseDialog->isHidden();
}
// 隐藏脉冲频率选择窗口
void CArmCaptureMode::hidePpsDialog(void)
{
    m_pPulseChooseDialog->m_pDoseDialog->hide();
}

void CArmCaptureMode::scanDoseHandle(const DoseType& doseType)
{
    CArmManager::scanConfigConvert(doseType)->m_auto = true;
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
    CArmLastScanConfig lastScanConfig = *CArmManager::scanConfigConvert();

    // 设置电流电压
    emit kVmASettingSignal(lastScanConfig);
    // 设置自动手动
    emit autoSignal(lastScanConfig);
    // 设置剂量模式
    setDose(lastScanConfig.m_doseType);

    // 发送数据给工作站
    scanInfoUpdateSignal(lastScanConfig);
}

void CArmCaptureMode::scanModeHandle(const ScanMode& scanMode)
{ 
    CArmManager::scanConfigConvert(scanMode)->m_auto = true;
    CArmManager::scanConfigConvert()->m_scanParam.m_frameCount = CArmManager::remainingFrameCount();
    CArmLastScanConfig lastScanConfig = *CArmManager::scanConfigConvert();

    // 设置电流电压
    emit kVmASettingSignal(lastScanConfig);
    // 设置自动手动
    emit autoSignal(lastScanConfig);
    // 设置扫描模式
    setScanMode(lastScanConfig.m_scanModeType);
    // 设置剂量模式
    setDose(lastScanConfig.m_doseType);

    // 发送数据给工作站
    scanInfoUpdateSignal(lastScanConfig);
}


// 选择身体部位时槽函数
void CArmCaptureMode::chooseBodyPartSlot(void)
{
    m_pPartChooseDialog->show();

    // 必须要在show之后才能有效
    QRect rect = this->geometry();
    QPoint pos = this->mapToGlobal(rect.topLeft());
    m_pPartChooseDialog->move(pos.x() + 20, pos.y() + 133);
}

// 是否可以点击脉冲频率按钮
void CArmCaptureMode::setPPSBtnStatus(bool status)
{
    m_pPulseChooseDialog->ui.PPSBtn->setEnabled(status);
    m_pPulseChooseDialog->ui.PPS_Btn->setEnabled(status);
}

