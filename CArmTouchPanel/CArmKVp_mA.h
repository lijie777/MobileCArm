/**
* @file CArmKVp_mA.h
* @brief 电压电流调节界面，包括自动和手动
* @author 李杰
*/

#pragma once

#include <QWidget>
#include "ui_CArmKVp_mA.h"
#include "../Common/Include/Config.h"

class CArmKVp_mA : public QWidget
{
    Q_OBJECT

public:
    CArmKVp_mA(QWidget *parent = Q_NULLPTR);
    ~CArmKVp_mA();

    /**
    * @brief 设置电流电压值
    * @param 要设置的值
    * @return void
    */
    void setkV_mA(const CArmScanConfig &scanConfig);

    /**
    * @brief 设置电流电压值
    * @param 要设置的值
    * @return void
    */
    void setkV_mA(const CArmLastScanConfig & config);

    /**自动手动设置
    * @param 要设置的值
    * @return void
    */
    void setAuto(const CArmLastScanConfig & config);


    enum MODE
    {
        KV_MODE,
        MA_MODE
    };

 
    bool m_bAuto = true;//!< 是否为自动模式

signals:
    // 发送信号告诉框架更新底部病人信息
    void scanInfoUpdateSignal(const CArmLastScanConfig&);


private:
    /**
    * @brief 初始化界面状态
    * @param void
    * @return void
    */
    void init(void);

    /**
    * @brief 自动按钮按下
    * @param void
    * @return void
    */
    void autoBtnClicked(void);
    /**
    * @brief 手动按钮按下
    * @param void
    * @return void
    */
    void manualBtnClicked(void);
    /**
    * @brief 样式设置
    * @param status
    * @return void
    */
    void styleSetting(bool status);
    /**
    * @brief 电压增加按钮按下
    * @param void
    * @return void
    */
    void kvpAddBtnClicked(void);
    /**
    * @brief 电压减小按钮按下
    * @param void
    * @return void
    */
    void kvpReduceBtnClicked(void);
    /**
    * @brief 电流添增加按钮按下
    * @param void
    * @return void
    */
    void mAAddBtnClicked(void);
    /**
    * @brief 电流减少按钮按下
    * @param void
    * @return void
    */
    void mAReduceBtnClicked(void);

    /**
    * @brief 把更改的数据写入xml文件中并发送数据到工作站
    * @param mode 电压/电流模式
    * @return void
    */
    void writeToFileAndSendData(MODE mode);

private:
    Ui::CArmKVp_mA ui;

    
};
