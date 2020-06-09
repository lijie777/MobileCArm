#pragma once

#include <QtWidgets/QFrame>
#include <QMouseEvent>
#include "ui_CArmLoginWidget.h"
#include "Common\CArmWorkStationProc.h"

#pragma execution_character_set("utf-8")

class CArmLoginWidget : public QFrame
{
    Q_OBJECT

public:

    Ui::CArmLoginWidget m_login_ui;

public:
    CArmLoginWidget(CArmWorkStationProc *proc = nullptr, QWidget *parent = 0);
    ~CArmLoginWidget();

private:
    QString userCode;      //!< 已输入的用户名（用户编码）
    QString userPwd;       //!< 已输入的用户密码
    std::string strDbIP;     //!< 数据库IP
    std::string strDbPort;   //!< 数据库端口号
    std::string strDbUserName; //!< 数据库用户名
    std::string strDbUserPwd;  //!< 数据库密码
    std::string strDbName;   //!< 数据库名称

    QString cfg_RemeberPasswd; //!< 是否记住密码

    std::string loginUserName;  //!< 登录的用户名称
    std::list<int> list_privilege;  //!< 用户对应的权限列表

    CArmWorkStationProc *WorkStationProc;

private:
    void initDataConfig();

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
     * @brief 初始化保存的用户、密码信息
     * @return void
     */
    //void initRememberUser();

    /**
     * @brief 通过连接检查输入用户名密码是否正确
     * @return bool
     */
    bool checkLoginDB(bool &isExist);

    /**
    * @brief 保存用户名、密码
    * @return void
    */
    //void saveCfg();
signals:
    void sigLoginSuccess();

    private slots:
    /**
    * @brief 关闭窗口
    * @return void
    */
    void closeDialog();

    /**
    * @brief 验证登录
    * @return void
    */
    void checkLogin();

    /**
    * @brief 用户名输入
    * @param text 输入值
    * @return void
    */
    void nameInput(QString text);

    /**
    * @brief 密码输入
    * @param text  输入值
    * @return void
    */
    void pwdInput(QString text);

    /**
    * @brief 生成用户权限
    * @return void
    */
    void setSystemPrivilege();

protected:
	bool mousePressed = false;   //!< 标题击中标志
	QPoint m_MousePressPos;      //!< 鼠标按下位置
	QPoint m_MouseMovePos;       //!< 鼠标移动位置

protected:
	/**
	 * @brief 鼠标按下事件
	 * @param e 鼠标事件
	 * @return void 
	 */
	void mousePressEvent(QMouseEvent *e);

	/**
	 * @brief 鼠标移动事件
	 * @param e 鼠标事件
	 * @return void 
	 */
	void mouseMoveEvent(QMouseEvent *e);

	/**
	 * @brief 鼠标释放事件
	 * @param e 鼠标事件
	 * @return void 
     */
	void mouseReleaseEvent(QMouseEvent *e);
};


