#pragma once

#include <QtWidgets/QFrame>
#include <QMouseEvent>
#include "ui_CArmLogOutWidget.h"

class CArmLogOutWidget : public QFrame
{
    Q_OBJECT

public:

    Ui::CArmLogOutWidget logout_ui;

public:
    CArmLogOutWidget(QWidget *parent = 0);
    ~CArmLogOutWidget();


private:
    /**
	 * @brief 初始化界面UI
	 * @return void 
     */
    void initUI();

    /**
	 * @brief 初始化信号槽连接
	 * @return void 
	 */
    void initLogic();

    /**
     * @brief 初始化登录用户
     * @return void 
     */
    void initLoginUser();


    private slots:
    /**
    * @brief 关闭窗口
    * @return void
    */
    void closeDialog();
	/**
	* @brief 进入设置界面
	* @return void
	*/
	void entrySystemConfig();

signals:
    void sigLogOut();
	void sigEntrySystemConfig();

protected:
    /**
    * @brief 鼠标按下事件
    * @param e 鼠标事件
    * @return void?
    */
    void mousePressEvent(QMouseEvent *e);
};


