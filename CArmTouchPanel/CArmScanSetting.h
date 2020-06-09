/**
* @file CArmScanSetting.h
* @brief 整个扫描设置界面，包括扫描设置，电流电压设置及机械控制
* @author 李杰
*/
#pragma once

#include <QWidget>
#include <QGridLayout>
#include "cereal/archives/xml.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/vector.hpp"

#include "ui_CArmScanSetting.h"
#include "../Common/Include/Config.h"
#include "CArmCaptureMode.h"
#include "CArmKVp_mA.h"
#include "CArmHardwareControl.h"

class CArmScanSetting : public QWidget
{
    Q_OBJECT

public:

    CArmScanSetting(QWidget *parent = Q_NULLPTR);
    ~CArmScanSetting();

    Ui::CArmScanSetting ui; 

    bool isVideo() { return m_pCaptureModeWidget->isVideo(); }
    /**
    * @brief 报警提示
    * @param void
    * @return void
    */
    void setAlarmLight(bool status);
    /**
    * @brief 扫描部位更新
    * @param scanPart 扫描部位
    * @return void
    */
    void scanPartUpdate(const ScanPart& scanPart);

    /**
    * @brief 初始化扫描信息
    * @param void 
    * @return void
    */
    void initScanInfo(void);

    /**
    * @brief 电流电压是否为自动模式
    * @param void
    * @return bool
    *   @retval				    
    */
    bool getIsAuto(void);

    /**
    * @brief 设置激光灯状态
    * @param status 激光灯状态 true:激光灯开启 false:激光灯关闭 
    * @return void
    */
    void setLaserLampStatus(bool status);

    /**
    * @brief 更新前后轴X方向位置
    * @param pos_x 前后轴X方向位置
    * @return void
    */
    void updateCArmPos_x(float pos_x);
    /**
    * @brief 更新升降柱Z方向位置
    * @param pos_z 升降柱Z方向位置
    * @return void
    */
    void updateCArmPos_z(float pos_z);
    /**
    * @brief 更新C型臂角度
    * @param pos_angle C型臂角度
    * @return void
    */
    void updateCArmPos_angle(float pos_angle);

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

    // 更新水平限束器的位置
    void updateCollimatorPosH(float pos);
    // 更新水平限束器的位置 
    void updateCollimatorPosV(float pos);

signals:
    // 更新底部病人信息信号
    void scanInfoUpdateSignal(const CArmLastScanConfig&);

    // 设置激光灯状态信号
    void laserLampStatusSignal(bool);
    // C型臂升降柱上升/下降信号
    void CArmUpDownSignal(OptionLiftColMove opt, float speed);
    // C型臂前后轴向前/向后信号
    void CArmFrontBackSignal(OptionXAxisMove opt, float speed);
    // C型臂角度调节（顺时针/逆时针）信号
    void CArmRotateSignal(OptionCArmMove opt, float speed);

    // C型臂升降柱上升/下降信号（非长按）
    void CArmUpDownSignal(float pos, float speed);
    // C型臂前后轴向前/向后信号（非长按）
    void CArmFrontBackSignal(float pos, float speed);
    // C型臂角度调节（顺时针/逆时针）信号（非长按）
    void CArmRotateSignal(float pos, float speed);


    // 限束器调节(长按)
    void collimatorMotionSignal(OptionCollimatorMoveDirection option, CArmDeviceMoveTypeDef opt, float speed);
   
    // 限束器调节信号（非长按）
    void collimatorMotionSignal(CArmDeviceMoveTypeDef opt, float pos, float speed);

private:
    /**
    * @brief 初始化及槽函数连接
    * @param void
    * @return void
    */
    void init(void);

    /**
    * @brief 设置电流电压
    * @param config 数据
    * @return void
    */
    void kVmASetting(const CArmLastScanConfig & config);

    /**
    * @brief 自动手动设置
    * @param config 数据
    * @return void
    */
    void autoSetting(const CArmLastScanConfig & config);

    /**
    * @brief // 设置扫描部位，扫描模式，脉冲频率,剂量类型，电压电流
    * @param scanPart 上次的扫描部位
    * @return void
    */
    void setScanInfo(const ScanPart & config);
   
private:
    CArmCaptureMode*        m_pCaptureModeWidget;      //!< 扫描模式界面
    CArmKVp_mA*             m_pKVp_mAWidget;           //!< 电压电流调节界面
    CArmHardwareControl*    m_pHardWareControlWidget;  //!< C型臂调节界面
};
