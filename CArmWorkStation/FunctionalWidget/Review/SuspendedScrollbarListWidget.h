#pragma once
#include <QScrollBar>
#include <QListWidget>






/**
* @brief 悬浮滚动条效果的列表控件
*/
class SuspendedScrollbarListWidget : public QListWidget
{
    Q_OBJECT

public:
    SuspendedScrollbarListWidget(QWidget* horizbar = nullptr);
    ~SuspendedScrollbarListWidget();

    /**
    * @brief 调用该函数，设置水平滚动条
    *
    * @param horizbar 水平滚动条
    */
    void setHorizontalScrollbar(QScrollBar* horizbar);

    /**
    * @brief 调用该函数，设置竖直滚动条
    *
    * @param vertbar 竖直滚动条
    */
    void setVerticalScrollbar(QScrollBar* vertbar);


protected slots:

    /**
    * @brief 水平滑动条数值变化槽函数
    */
    void updateHorizontalValue(int i);

    /**
    * @brief 竖直滑动条数值变化槽函数
    */
    void updateVerticalValue(int i);


    /**
    * @brief 水平滑动条范围变化槽函数
    */
    void updateHorizontalRange(int min, int max);

    /**
    * @brief 竖直滑动条范围变化槽函数
    */
    void updateVerticalRange(int min, int max);


    /**
    * @brief 水平滑动条数值变化槽函数
    */
    void trackHorizontalValue(int value);

    /**
    * @brief 竖直滑动条数值变化槽函数
    */
    void trackVerticalValue(int value);

protected:


    /**
    * @brief 调用该函数，初始化窗口
    */
    void initUI();

    /**
    * @brief 调用该函数，初始化导航窗口
    */
    void initLogic();

    /**
    * @brief 事件处理函数
    *
    * @param e 事件处理对象
    */
    void resizeEvent(QResizeEvent *e);

    /**
    * @brief 事件处理函数
    *
    * @param e 事件处理对象
    */
    void enterEvent(QEvent *e);

    /**
    * @brief 事件处理函数
    *
    * @param e 事件处理对象
    */
    void leaveEvent(QEvent *e);


private:

    /// 水平悬浮滚动条
    QScrollBar* m_pHorizScrollBar;

    /// 竖直悬浮滚动条
    QScrollBar* m_pVertScrollBar;
};



/**
* @brief 列表控件展开折叠控制窗口
*/
class ListWidgetFoldWidget : public QWidget
{
public:

    ListWidgetFoldWidget(QWidget * parent = nullptr);
    ~ListWidgetFoldWidget();


protected:

    /**
    * @brief 事件处理函数
    *
    * @param e 事件处理对象
    */
    void enterEvent(QEvent *e);
};