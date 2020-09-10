/**
* @file CArmCaptureMode.h
* @brief 触屏拍摄模式界面，包括部位选择，拍摄模式选择及剂量选择
* @author 李杰
*/
#pragma once

#include <QWidget>
#include "ui_CArmCaptureMode.h"
#include "CArmPartChooseDialog.h"
#include "CArmPulseChooseDialog.h"
#include "../Common/Include/Config.h"
#include "CArmManager.h"

class CArmCaptureMode : public QWidget
{
    Q_OBJECT

public:
    CArmCaptureMode(QWidget *parent = Q_NULLPTR);
    ~CArmCaptureMode();

    /// 初始化借槽函数连接
    void init(void);

    bool isVideo() { return ui.cineBtn->isChecked(); }

    /**
    * @brief 从默认的配置文件读取扫描的身体部位
    * @param part 扫描部位
    * @return void
    */
    void scanPartUpdate(const ScanPart& part);

    /**
    * @brief 获取身体部位窗口状态
    * @param void
    * @return 获取身体部位窗口状态
    *     @retval true 窗口隐藏
    *     @retval false 窗口显示
    */
    bool scanPartDialogIsshow(void);
    /**
    * @brief 隐藏身体部位选择窗口
    * @param void
    * @return void
    */
    void hideScanPartDialog(void);

    /**
    * @brief 获取脉冲频率选择窗口状态
    * @param void
    * @return 获取脉冲频率选择窗口状态
    *     @retval true 窗口隐藏
    *     @retval false 窗口显示
    */
    bool scanPpsDialogIsshow(void);
    
    /**
    * @brief 隐藏脉冲频率选择窗口
    * @param void
    * @return void
    */
    void hidePpsDialog(void);

    /**
    * @brief 脉冲频率按钮是否可点击
    * @param status true:可以选择脉冲频率 false:不能选择脉冲频率
    * @return void
    */
    void setPPSBtnStatus(bool status);

signals:
    // 设置对应电流电压
    void kVmASettingSignal(const CArmLastScanConfig & config);

    // 发送信号告诉框架更新底部病人信息
    void scanInfoUpdateSignal(const CArmLastScanConfig&);

    // 自动还是手动
    void autoSignal(const CArmLastScanConfig&);

public slots:

    /**
    * @brief 身体部位选择（点击选择部位按钮）
    * @param void
    * @return
    */
    void chooseBodyPartSlot(void);

    /**
    * @brief 在多种身体部位选择一个扫描部位
    * @param partWidget 选择的是哪个身体部位
    * @return void
    */
    void bodyPartShowSlot(CArmPartWidget* partWidget);

private:
    /**
    * @brief 读取配置文件获取当前的拍色模式（点拍，透视，视频）
    * @param scanMode 当前扫描模式
    * @return void
    */
    void setScanMode(const ScanMode &scanMode);

    /**
    * @brief 设置脉冲频率
    * @param  ppsMode 脉冲频率
    * @return void
    */
    void setPPS(int pps);

    /**
    * @brief 设置剂量选择
    * @param  ppsMode 剂量
    * @return void
    */
    void setDose(const DoseType &doseMode);

    /**
    * @brief 当选择了一个部位后其它的界面变化（包括解析xml文件及修改xml文件）
    * @param scanPart 选择的部位
    * @return void
    */
    void bodyPartChanged(const ScanPart &scanPart);

    /**
    * @brief 点拍按钮按下
    * @param  void
    * @return void
    */
    void pointCaptureBtnClicked(void);

    /**
    * @brief 脉冲透视按钮按下
    * @param  void
    * @return void
    */
    void pulsePerspectiveBtnClicked(void);

    /**
    * @brief 视频按钮按下
    * @param  void
    * @return void
    */
    void cineBtnClicked(void);

    /**
    * @brief 常规剂量按钮按下
    * @param  void
    * @return void
    */
    void normalDoseBtnClicked(void);

    /**
    * @brief 增强剂量按钮按下
    * @param  void
    * @return void
    */
    void enhanceDoseBtnClicked(void);

    /**
    * @brief 儿童剂量按钮按下
    * @param  void
    * @return void
    */
    void childDoseBtnClicked(void);

    /**
    * @brief 脉冲频率修改
    * @param  ppsValue 脉冲频率
    * @return void
    */
    void ppsValueChoose(int ppsValue);

    /**
    * @brief 修改剂量时的逻辑处理
    * @param  doseType 选择的剂量类型
    * @return void
    */
    void scanDoseHandle(const DoseType& doseType);

    /**
    * @brief 修改扫描模式的逻辑处理
    * @param  ppsValue 选择的扫描模式
    * @return void
    */
    void scanModeHandle(const ScanMode& scanMode);

private:
    Ui::CArmCaptureMode ui;

    CArmPartChooseDialog*      m_pPartChooseDialog;   //!< 身体部位选择界面，暂时包括九种部位

    CArmPulseChooseDialog*     m_pPulseChooseDialog;  //!< 剂量选择界面，包括默认脉冲频率，低剂量，高剂量，儿童剂量

    bool m_bFlag = false; //!< xml里面找不到对应的参数则启用参数默认值
};
