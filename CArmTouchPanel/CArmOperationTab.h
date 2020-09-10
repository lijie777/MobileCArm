#pragma once
/**
* @file CArmOperationTab.h
* @brief 小C图像处理页面
* @author 路旺明
*/
#include <QWidget>
#include "ui_CArmImageOperationTab.h"
#include "CArmData.h"
#include "CArmControl.h"
#include "../Common/Include/Config.h"

class CArmImageTab : public QWidget, public Ui::CArmImage
{
    Q_OBJECT
public:
    explicit CArmImageTab(QWidget *parent = Q_NULLPTR);

    /**
    * @brief								初始化界面控件值
    *
    * @param				parm			初始值结构
    * @return								无
    *   @retval							    无
    */
    void setWidgetInit(ImageParam parm);

    /**
    * @brief								金属操作
    *
    * @param		gear			        金属操作等级
    * @return								无
    *   @retval							    无
    */
    void metalGearOperation(CARM_METAL_GEAR_ENUM gear);

    /**
    * @brief								获取自动金属校正的状态
    *
    * @return			bool		        是否进行金属校正
    *   @retval			m_bMetaRevise		true表示进行金属校正，false表示不进行金属校正
    */
    inline bool getMetaReviseStatus() { return m_bMetaRevise; }

    /**
    * @brief								获取是否进行负片操作
    *
    * @return			bool		        是否进行负片操作
    *   @retval			m_bNegative		    true表示进行负片操作，false表示不进行负片操作
    */
    inline bool getNegativeStatus() { return m_bNegative; }

    /**
    * @brief								获取水平翻转状态
    *
    * @return			bool		        是否进行水平翻转
    *   @retval			m_bLevel		    true表示进行水平翻转操作，false表示不进行水平翻转操作
    */
    inline bool getLevelStatus() { return m_bLevel; }

    /**
    * @brief								获取是否进行垂直翻转
    *
    * @return			bool		        是否进行垂直翻转
    *   @retval			m_bVertical		    true表示垂直翻转，false表示不进行垂直翻转
    */
    inline bool getVerticalStatus() { return m_bVertical; }
private:
    /**
    * @brief								设置图像页面的风格
    *
    * @return								无
    *   @retval							    无
    */
    void setImageWidgetStyle();

    /**
    * @brief								链接槽函数
    *
    * @return								无
    *   @retval							    无
    */
    void linkSlot();

    /**
    * @brief								图像操作
    *
    * @param		imageOperation			操作对象
    * @return								无
    *   @retval							    无
    */
    void imageOperation(CARM_IMAGE_OPERATION_ENUM imageOperation);

    

private slots:
    /**
    * @brief								金属操作槽函数
    *
    * @return								无
    *   @retval							    无
    */
    void metaRevise_slot();

    /**
    * @brief								负片操作槽函数
    *
    * @return								无
    *   @retval							    无
    */
    void negative_slot();

    /**
    * @brief								翻转操作
    *
    * @return								无
    *   @retval							    无
    */
    void rotate_slot();

    /**
    * @brief								水平翻转
    *
    * @return								无
    *   @retval							    无
    */
    void level_slot();

    /**
    * @brief								垂直翻转
    *
    * @return								无
    *   @retval							    无
    */
    void vertical_slot();

    /**
    * @brief								重置操作
    *
    * @return								无
    *   @retval							    无
    */
    void reset_slot();

    /**
    * @brief								金属0等级
    *
    * @return								无
    *   @retval							    无
    */
    void metalZero_slot();

    /**
    * @brief								金属1等级
    *
    * @return								无
    *   @retval							    无
    */
    void metalOne_slot();

    /**
    * @brief								金属2等级
    *
    * @return								无
    *   @retval							    无
    */
    void metalTwo_slot();

    /**
    * @brief								金属3等级
    *
    * @return								无
    *   @retval							    无
    */
    void metalThree_slot();

    /**
    * @brief								锐化等级选择
    *
    * @return								无
    *   @retval							    无
    */
    void sharpenSelectGear_slot();

    /**
    * @brief								降噪等级选择
    *
    * @return								无
    *   @retval							    无
    */
    void denoiseSelectGear_slot();

    /**
    * @brief								锐化等级
    *
    * @param		gear			        锐化等级
    * @return								无
    *   @retval							    无
    */
    void sharpenGear_slot(int gear);

    /**
    * @brief								降噪等级
    *
    * @param		gear			        降噪等级
    * @return								无
    *   @retval							    无
    */
    void denoiseGear_slot(int gear);

    /**
    * @brief								确定重置图像
    *   
    * @return								无
    *   @retval							    无
    */
    void okResetImage_slot();

    void setMetalGearOperation_slot(CARM_METAL_GEAR_ENUM);

signals:
    /**
    * @brief								金属操作信号，通知主框架
    *
    * @return								无
    *   @retval							    无
    */
    void metaRevise_sig();

    /**
    * @brief								负片操作信号，通知主框架
    *
    * @return								无
    *   @retval							    无
    */
    void negative_sig();

    /**
    * @brief								旋转操作信号，通知主框架
    *
    * @return								无
    *   @retval							    无
    */
    void rotate_sig(bool status);

    /**
    * @brief								水平翻转操作信号，通知主框架
    *
    * @return								无
    *   @retval							    无
    */
    void level_sig();

    /**
    * @brief								垂直翻转操作信号，通知主框架
    *
    * @return								无
    *   @retval							    无
    */
    void vertical_sig();

    /**
    * @brief								重置操作信号，通知主框架
    *
    * @return								无
    *   @retval							    无
    */
    void reset_sig();

    /**
    * @brief								锐化等级信号
    *
    * @param		gear			        锐化等级
    * @return								无
    *   @retval							    无
    */
    void sharpenGear_sig(CARM_GEAR_ENUM gear);

    /**
    * @brief								降噪等级信号
    *
    * @param		gear			        锐化等级
    * @return								无
    *   @retval							    无
    */
    void denoiseGear_sig(CARM_GEAR_ENUM gear);

    /**
    * @brief								金属等级信号
    *
    * @param		metalGear			    金属校正等级
    * @return								无
    *   @retval							    无
    */
    void metalGear_sig(CARM_METAL_GEAR_ENUM metalGear);

private:
    ///界面风格结构体
    CARM_STYLE_STRUCT       m_styleStruct;

    ///金属等级标签链表
    QList<QLabel*>          m_metalList;

    ///锐化等级选择弹窗
    CArmGearSelectDialog*   m_pSelectSharpenGear;

    ///降噪等级选择弹窗
    CArmGearSelectDialog*   m_pSelectDenoiseGear;

    ///锐化按钮组
    QButtonGroup*           m_pSharpenButtonGroup;

    ///降噪按钮组
    QButtonGroup*           m_pDenoiseButtonGroup;

    ///金属校正按钮状态
    bool                    m_bMetaRevise;

    ///负片按钮状态
    bool                    m_bNegative;

    ///旋转按钮状态
    bool                    m_bRotate;

    ///垂直翻转按钮状态
    bool                    m_bLevel;

    ///水平翻转按钮状态
    bool                    m_bVertical;

    ///重置按钮状态
    bool                    m_bReset;

};
