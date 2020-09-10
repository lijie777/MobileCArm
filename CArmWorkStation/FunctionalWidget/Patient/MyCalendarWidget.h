/**
 * @file MyCalendarWidget.h
 * @author AndyGao
 * @brief 自定义日期控件
 */
#pragma once
#include <QFrame>
#include <QCalendarWidget>
#include <QPushButton>
#include <QDate>
#include <QLabel>
#include "QLineEdit"

/**
 * @brief 自定义日期控件类
 */
class MyCalendarWidget :
	public QFrame
{
	Q_OBJECT
public:
	/**
	 * @brief 自定义日期控件类构造函数
	 * @param w 父窗口
	 * @param e 选择日期后自动填入的输入框
	 * @param iconPath 日期左右按钮图标路径
	 * @return  
	 */
	MyCalendarWidget(QWidget* w = nullptr, QLineEdit* e = nullptr, QString iconPath = NULL);

	/**
	 * @brief 自定义日期控件类析构函数
	 * @return  
	 */
	~MyCalendarWidget();

    void setMinMaxDate(QDate minDate, QDate maxDate);

	protected slots:
	/**
	 * @brief 日期翻页
	 * @param year 输入年
	 * @param month 输入月
	 * @return void 
	 */
	void PageChanged(int year, int month);

	/**
	 * @brief 将选择的日期填入输入框
	 * @return void 
	 */
	void setLineEditDate();

protected:
	/**
	 * @brief 初始化UI
	 * @return void 
	 */
	void initUI();

	/**
	 * @brief 初始化信号槽连接
	 * @return void 
	 */
	void initLogic();

	/**
	 * @brief 初始化UI风格
	 * @return void 
	 */
	void initStyle();

private:
	QCalendarWidget* m_calendar;  //!< 日期控件对象
	QLineEdit *dateEdit;          //!< 日期控件对应的输入框
	QString currentIconPath;      //!< 当前日期控件对应的图标路径

	QString m_caltrans = tr("chinese");    //!< 当前日期控件语言

protected:
    /**
    * @brief 鼠标按下事件
    * @param e 鼠标事件
    * @return void?
    */
    void mousePressEvent(QMouseEvent *e);
};

