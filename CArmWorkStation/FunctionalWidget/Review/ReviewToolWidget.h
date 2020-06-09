#pragma once

#include <QtWidgets/QFrame>
#include "ui_ReviewToolWidget.h"
#include "PageBase.h"
#include "CArmCommon.h"
#include "ImageProcessingWidget.h"
#include "RotateDialog.h"

/**
* @brief 阅片工具按钮管理类
*/
class ButtonGroup
{
public:

    /**
    * @brief 添加二选一阅片按钮
    * @param button 按钮对象
    */
    void addAlternationButton(QAbstractButton* button);

    /**
    * @brief 添加不选中阅片按钮
    * @param button 按钮对象
    */
    void addNeitherButton(QAbstractButton* button);

    /**
    * @brief 移除阅片按钮
    * @param button 按钮对象
    */
    void removeButtom(QAbstractButton* button);

    /**
    * @brief 点击阅片按钮
    * @param button 按钮对象
    */
    bool clickedButton(QAbstractButton* button);

    /**
    * @brief 关闭所有阅片按钮
    */
    void closeButtons();

    void setButtonEnable(bool en);

private:
    /// 二选一阅片按钮
    std::list<QAbstractButton*> alt_button_list;

    /// 不选中阅片按钮
    std::list<QAbstractButton*> net_button_list;
};


class ReviewToolWidget : public QFrame
{
    Q_OBJECT

public:

public:
    ReviewToolWidget(CArmBasePage* page, QWidget *parent = 0);
    ~ReviewToolWidget();


    /**
    * @brief 调用该函数，更新患者
    * @param info 患者信息
    */
    void updatePatientInfo(const CP_PatientData& info);

    /**
    * @brief 调用该函数，清空患者信息
    */
    void clearPatientInfo();

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
    * @brief 调用该函数，初始化按钮管理
    */
    void initBtnGroup();


protected slots:

    /**
    * @brief 方位L标记槽函数
    */
    void pressLBtn();

    /**
    * @brief 方位A标记槽函数
    */
    void pressABtn();

    /**
    * @brief 方位H标记槽函数
    */
    void pressHBtn();

    /**
    * @brief 方位R标记槽函数
    */
    void pressRBtn();

    /**
    * @brief 方位P标记槽函数
    */
    void pressPBtn();

    /**
    * @brief 方位F标记槽函数
    */
    void pressFBtn();

    /**
    * @brief 箭头标记槽函数
    */
    void pressArrowBtn();

    /**
    * @brief 文本标记槽函数
    */
    void pressTextBtn();

    /**
    * @brief 标记隐藏槽函数
    */
    void pressHideBtn();

    /**
    * @brief 直线测量槽函数
    */
    void pressLineBtn();

    /**
    * @brief 角度测量槽函数
    */
    void pressAngleBtn();

    /**
    * @brief 图像校准槽函数
    */
    void pressCalibBtn();

    /**
    * @brief 删除标记槽函数
    */
    void pressDeleteBtn();

    /**
    * @brief 展开历史槽函数
    */
    void pressHisBtn();

    /**
    * @brief 图像调节槽函数
    */
    void pressPrcBtn();

    /**
    * @brief 图像旋转槽函数
    */
    void pressRtBtn();

    /**
    * @brief 图像负片槽函数
    */
    void pressRvBtn();

    /**
    * @brief 图像水平翻转槽函数
    */
    void pressFHBtn();

    /**
    * @brief 图像竖直翻转槽函数
    */
    void pressFVBtn();

    /**
    * @brief 图像复原槽函数
    */
    void pressRcBtn();

    /**
    * @brief 保存槽函数
    */
    void pressSvBtn();

private:

    Ui::ReviewToolWidget ui;        //!< UI控件
    ImageProcessingWidget* m_imgPrcWidget;       //!< 图像调节窗口
    RotateDialog* m_rotateWidget;                //!< 图像旋转窗口
    CArmBasePage* m_pPageWidget;        //!< 功能页
    ButtonGroup m_buttonGroup;      //!< 按钮管理
};


