#include "CArmLoginWidget.h"
#include "CommonUtility.h"
#include <cmath>
#include "IPCDefinitions.h"

#include <windows.h>
#include <sstream>
#include <QtCore/QSettings.h>
#include <jdbc\mysql_connection.h>
#include <jdbc\mysql_driver.h>
#include <jdbc\cppconn\statement.h>
#include <jdbc\cppconn\prepared_statement.h>

CArmLoginWidget::CArmLoginWidget(CArmWorkStationProc *proc, QWidget *parent)
    : QFrame(parent)
    , WorkStationProc(proc)
{
    initUI();
    initLogic();
    //initRememberUser();
}

CArmLoginWidget::~CArmLoginWidget()
{

}

void CArmLoginWidget::initDataConfig()
{
    IPCLocalDataSource mIPCLocalDataSource;
    if (!WorkStationProc->getLocalDataSource(mIPCLocalDataSource))
    {
        m_login_ui.warningLab->setText(tr("无法获取数据库配置！"));
        return;
    }

    strDbIP = mIPCLocalDataSource.m_strIP;
    strDbPort = mIPCLocalDataSource.m_strPort;
    strDbUserName = mIPCLocalDataSource.m_strUser;
    strDbUserPwd = mIPCLocalDataSource.m_strPassword;
    strDbName = mIPCLocalDataSource.m_strDBName;
}

void CArmLoginWidget::initUI()
{
    m_login_ui.setupUi(this);
    //设置窗口无边框
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setTabOrder(m_login_ui.nameEdit, m_login_ui.pwdEdit);
    setTabOrder(m_login_ui.pwdEdit, m_login_ui.exitBtn);
    setTabOrder(m_login_ui.exitBtn, m_login_ui.submitBtn);

    //自动填充默认的，方便测试
    userCode = "root";
    userPwd = "123456";
    m_login_ui.nameEdit->setText(userCode);
    m_login_ui.pwdEdit->setText(userPwd);
    m_login_ui.submitBtn->setEnabled(true);
}

void CArmLoginWidget::initLogic()
{
    connect(m_login_ui.exitBtn, SIGNAL(clicked()), this, SLOT(closeDialog()));  //点击“退出”
    connect(m_login_ui.submitBtn, SIGNAL(clicked()), this, SLOT(checkLogin()));   //点击“确认”
    connect(m_login_ui.nameEdit, SIGNAL(textChanged(QString)), this, SLOT(nameInput(QString)));  //用户名输入
    connect(m_login_ui.pwdEdit, SIGNAL(textChanged(QString)), this, SLOT(pwdInput(QString)));    //用户密码输入

    m_login_ui.submitBtn->setFocus();    //设置默认焦点
    m_login_ui.submitBtn->setShortcut(QKeySequence::InsertParagraphSeparator);  //设置快捷键为键盘的“回车”键
    m_login_ui.submitBtn->setShortcut(Qt::Key_Enter);  //设置快捷键为enter键
    m_login_ui.submitBtn->setShortcut(Qt::Key_Return); //设置快捷键为小键盘上的enter键
}

void CArmLoginWidget::closeDialog()
{
    this->close();
}

void CArmLoginWidget::checkLogin()
{
    //免数据库直接登录
    if (userCode == "ueg" && userPwd == "ueg")
    {
        m_login_ui.warningLab->setText("");
        emit sigLoginSuccess();
        return;
    }
    initDataConfig();
    //用户名、密码是否存在
    bool isExist;
    if (!checkLoginDB(isExist))
    {
        m_login_ui.warningLab->setText(tr("数据库连接异常！"));
    }
    else
    {
        if (isExist)
        {
            //setSystemPrivilege(); //设置权限
            //saveCfg(); //保存登录配置
            m_login_ui.warningLab->setText("");
            emit sigLoginSuccess();
        }
        else
        {
            m_login_ui.warningLab->setText(tr("用户名或密码输入错误！"));
        }
    }
}

void CArmLoginWidget::nameInput(QString text)
{
    userCode = text;
    if (!userCode.isEmpty() && !userPwd.isEmpty())
    {
        m_login_ui.submitBtn->setEnabled(true);
    }
    else
    {
        m_login_ui.submitBtn->setEnabled(false);
    }
}

void CArmLoginWidget::pwdInput(QString text)
{
    userPwd = text;
    if (!userCode.isEmpty() && !userPwd.isEmpty())
    {
        m_login_ui.submitBtn->setEnabled(true);
    }
    else
    {
        m_login_ui.submitBtn->setEnabled(false);
    }
}

void CArmLoginWidget::mousePressEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton &&
        this->rect().contains(e->pos()))
    {
        m_MousePressPos = e->globalPos();
        mousePressed = true;
        this->raise();
    }
    e->ignore();

}

void CArmLoginWidget::mouseMoveEvent(QMouseEvent * e)
{
    if (mousePressed)
    {
        m_MouseMovePos = e->globalPos();
        int height = GetSystemMetrics(SM_CYFULLSCREEN);
        if (m_MouseMovePos.y() >= height) return;
        this->move(this->pos() + m_MouseMovePos - m_MousePressPos);
        m_MousePressPos = m_MouseMovePos;
        return;
    }
    e->ignore();
}

void CArmLoginWidget::mouseReleaseEvent(QMouseEvent * e)
{
    if (e->button() == Qt::LeftButton)
    {
        mousePressed = false;
    }
    e->ignore();

}

bool CArmLoginWidget::checkLoginDB(bool &isExist)
{
    sql::mysql::MySQL_Driver *driver = NULL;
    sql::Connection *con = NULL;
    sql::Statement *state = NULL;
    sql::ResultSet *result = NULL;
    sql::PreparedStatement *prepStmt = NULL;
    try
    {
        // 初始化驱动  
        driver = sql::mysql::get_mysql_driver_instance();
        // 建立链接  
        std::string str_host_name = "tcp://" + strDbIP + ":" + strDbPort;
        con = driver->connect(str_host_name, strDbUserName, strDbUserPwd);
        state = con->createStatement();
        std::string str_use_db = "use " + strDbName;
        state->execute(str_use_db);
        state = con->createStatement();
        state->execute("set character set GB2312");

        std::string vsql;

        vsql = "call pro_check_user_login(?,?)";
        prepStmt = con->prepareStatement(vsql.c_str());

        prepStmt->setString(1, std::string(userCode.toLocal8Bit()));
        prepStmt->setString(2, std::string(userPwd.toLocal8Bit()));

        result = prepStmt->executeQuery();

        // 得到查询结果
        do
        {
            while (result->next())
            {
                if (list_privilege.size() == 0)
                {
                    loginUserName = result->getString(1);
                }
                int strPrivilege = atoi(result->getString(2).c_str());
                list_privilege.push_back(strPrivilege);
            }
        } while (state->getMoreResults());

        //存在相同ID返回false，否则返回true
        if (list_privilege.size()>0)
            isExist = true;
        else
            isExist = false;

        delete result;
        delete prepStmt;
        delete state;
        delete con;
        return true;
    }

    catch (sql::SQLException& e)
    {
        if (result != nullptr)
        {
            delete result;
        }
        if (prepStmt != nullptr)
        {
            delete prepStmt;
        }

        if (state != nullptr)
        {
            delete state;
        }
        if (con != nullptr)
        {
            delete con;
        }
        return false;
    }
}

//void CArmLoginWidget::initRememberUser()
//{
//    QSettings cfg("user.ini", QSettings::IniFormat);
//    QString cfg_usersname = cfg.value("usersname").toString();
//    QString cfg_passswd = cfg.value("passswd").toString();
//    QString cfg_RemeberPasswd = cfg.value("remeberPasswd").toString();
//    if (cfg_RemeberPasswd == "true")
//    {
//        m_login_ui.nameEdit->setText(cfg_usersname);
//        m_login_ui.pwdEdit->setText(cfg_passswd);
//        m_login_ui.submitBtn->setEnabled(true);
//        m_login_ui.rememberBox->setChecked(true);
//    }
//    else if (cfg_RemeberPasswd == "false" && !cfg_usersname.isEmpty())
//    {
//        m_login_ui.nameEdit->setText(cfg_usersname);
//    }
//}

//void CArmLoginWidget::saveCfg()
//{
//    QSettings cfg("user.ini", QSettings::IniFormat);
//    QString cfg_usersname = m_login_ui.nameEdit->text();
//    QString cfg_passswd = m_login_ui.pwdEdit->text();
//    cfg.setValue("usersname", cfg_usersname);
//    if (m_login_ui.rememberBox->isChecked())
//    {
//        cfg.setValue("passswd", cfg_passswd);
//        cfg.setValue("remeberPasswd", "true");
//    }
//    else
//    {
//        cfg.setValue("passswd", "");
//        cfg.setValue("remeberPasswd", "false");
//    }
//    cfg.sync();
//}

void CArmLoginWidget::setSystemPrivilege()
{
    size_t m_u64Privilege = 0;	// 管理员m_u64Privilege = ‭131071‬;
    for (int it : list_privilege)
    {
        m_u64Privilege |= 1ULL << it;
    }

    GlobalMemMapping::SetUserInfo(userCode.toLocal8Bit().data(), UCommonUtility::string2UTF8(loginUserName), m_u64Privilege);
}