/**
* @file CArmPulseChooseDialog.h
* @brief 剂量选择界面，包括默认脉冲频率，低剂量，高剂量，儿童剂量
* @author 李杰
*/
#pragma once

#include <QDialog>
#include "ui_CArmPulseChooseDialog.h"
#include "CArmDose.h"

class CArmPulseChooseDialog : public QDialog
{
    Q_OBJECT

public:
    CArmPulseChooseDialog(QWidget *parent = Q_NULLPTR);
    ~CArmPulseChooseDialog();

    Ui::CArmPulseChooseDialog ui;


    CArmDose*         m_pDoseDialog;   //!< 脉冲频率选择界面（暂时包括2PPS,4PPS,8PPS,12PPS）

signals:
    /// 修改pps频率
    void ppsValueSignal(int);

public slots:
    /**
    * @brief 显示当前的脉冲频率信号界面
    * @param void 
    * @return void
    */
    void showPPSDialog(void);

    /**
    * @brief 当选择了一个脉冲频率后执行的操作
    * @param void
    * @return void
    */
    void doseChoose(void);

private:
    /**
    * @brief 初始化及槽函数连接
    * @param void
    * @return void
    */
    void init(void);

private:
    

    


};
