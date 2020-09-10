#pragma
/**
* @file RotateDialog.h
* @brief 小C图像旋转调节控件
* @author 贺兴怡
*/

#include "ui_RotateDialog.h"
#include "CArmBaseView.h"
#pragma execution_character_set("utf-8")


/**
* @brief 旋转角度选择窗口
* 进行旋转操作的时候，进行旋转角度的选择
*/
class RotateDialog :public QWidget
{
    Q_OBJECT
public:
    explicit RotateDialog(CArmBasePage *page, QAbstractButton* ctlBtn, QWidget* parent = nullptr);
    ~RotateDialog();


protected slots:

    /**
    * @brief 关闭按钮槽函数
    */
    void pressCloseBtn();

    /**
    * @brief 滑块值改变槽函数
    */
    void sliderValueChanged(int value);

protected:

    /**
    * @brief 调用该函数，初始化窗口UI样式
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化信号槽连接
    */
    void initLogic();

private:

    Ui::RotateDialog ui;    //!< 窗口布局
    QAbstractButton* m_pCtlBtn;  //!< 控制按钮
    CArmBasePage* m_pPage;      //!< 功能页
};


