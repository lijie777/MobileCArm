#pragma once

#include <QtWidgets/QFrame>
#include "CArmBaseView.h"
#include "ui_ImageProcessingWidget.h"
#include "ui_SharpenMenu.h"
#include "ui_DenoiseMenu.h"
#include "QMenu"

class ImageProcessingWidget : public QFrame
{
    Q_OBJECT

public:
    ImageProcessingWidget(CArmBasePage* page, QAbstractButton* btn, QWidget *parent = 0);
    ~ImageProcessingWidget();


protected:



    /**
    * @brief 调用该函数，初始化窗口UI样式
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

    /**
    * @brief 调用该函数，初始化按钮控制
    */
    void initBtnGroup();

    /**
    * @brief 事件过滤处理函数
    *
    * @param obj 事件目标对象
    * @param event 事件处理对象
    * @return false表示消息不下发给窗口子控件，true表示下发消息
    */
    bool eventFilter(QObject *obj, QEvent *event);

    /**
    * @brief 调用该函数，调节锐化度
    */
    void adjustSharpen();

    /**
    * @brief 调用该函数，调节对比度
    */
    void adjustContrast();

    /**
    * @brief 调用该函数，调节明亮度
    */
    void adjustBright();

    /**
    * @brief 调用该函数，进行降噪
    */
    void adjustDenoise();

    /**
    * @brief 调用该函数，进行金属校正
    */
    void adjustMetalCalib();

protected slots:

    /**
    * @brief 滑动对比度滑动条槽函数
    */
    void contsValueChanged(int value);

    /**
    * @brief 滑动明亮度滑动条槽函数
    */
    void brtValueChanged(int value);

    /**
    * @brief 滑动锐度滑动条槽函数
    */
    void shpValueChanged(int value);

    /**
    * @brief 滑动金属校正滑动条槽函数
    */
    void densValueChanged(int value);

    /**
    * @brief 点击锐化菜单槽函数
    */
    void sharpenMenuClicked(QAbstractButton* btn);

    /**
    * @brief 点击降噪菜单槽函数
    */
    void denoiseMenuClicked(QAbstractButton* btn);

    /**
    * @brief 点击金属校正槽函数
    */
    void metalCalibClicked(QAbstractButton* btn);

    /**
    * @brief 点击锐化按钮槽函数
    */
    void sharpenClicked();

    /**
    * @brief 点击降噪按钮槽函数
    */
    void denoiseClicked();


private:
    Ui::ProcessingWidget m_ui;  //!< 布局
    Ui::DenoiseMenu m_dnsUI;    //!< 降噪菜单布局
    Ui::SharpenMenu m_shpUI;    //!< 锐化菜单布局
    CArmBasePage* m_pPageWidget;    //!< 功能页
    QButtonGroup m_shpBtnGroup;    //!< 锐化按钮控制
    QButtonGroup m_dnsBtnGroup;    //!< 降噪按钮控制
    QButtonGroup m_calibBtnGroup;  //!< 校正按钮控制
    QAbstractButton* m_pCtrlBtn;//!< 控制按钮
    QFrame* m_pShpMenu;         //!< 锐化菜单
    QFrame* m_pDnsMenu;         //!< 降噪菜单

};


