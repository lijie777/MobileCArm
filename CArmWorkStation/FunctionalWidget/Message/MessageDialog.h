
/**
 * @file MessageDialog.h
 * @author 贺兴怡
 * @brief 消息提示窗口类头文件
 */
#pragma


#include "ui_MessageDialog.h"

/**
 * @brief 消息提示窗口类
 */
class MessageDialog : public QDialog
{
    Q_OBJECT
public:

    /// 窗口部件
    Ui::messageDialog message_ui;

    /// 关闭窗口按钮被按下标志
    bool CloseBtnClicked;

public:

    /**
     * @brief 消息提示窗口类构造函数
     */
    MessageDialog();


    /**
    * @brief 消息提示窗口类析构函数
    */
	~MessageDialog();


private:

    /**
    * @brief 调用该函数，初始化窗口部件
    */
    void initUI();


    /**
    * @brief 调用该函数，初始化3D阅片信号槽连接
    */
    void initLogic();


    /**
     * @brief 鼠标按下操作处理事件
     *
     * @param e 鼠标事件对象
     */
    void mousePressEvent(QMouseEvent *e);

    /**
    * @brief 鼠标移动操作处理事件
    *
    * @param e 鼠标事件对象
    */
    void mouseMoveEvent(QMouseEvent *e);

    /**
    * @brief 鼠标释放操作处理事件
    *
    * @param e 鼠标事件对象
    */
    void mouseReleaseEvent(QMouseEvent *e);


    protected slots:

    /**
     * @brief 关闭窗口按钮槽函数
     */
    void closeDialog();

    void cancelPressed();

    void submitPressed();

private:

    // 标题栏操作参数

    /// 标题击中标志
    bool mousePressed;

    /// 鼠标按下位置
    QPoint m_MousePressPos;

    /// 鼠标移动位置
    QPoint m_MouseMovePos;
};

