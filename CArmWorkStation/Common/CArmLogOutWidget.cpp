#include "Common\CArmCMD.h"
#include "CArmLogOutWidget.h"
#include "GlobalMemMapDataDef.h"

CArmLogOutWidget::CArmLogOutWidget(QWidget * parent)
    : QFrame(parent)
{
    initUI();
    initLogic();
}

CArmLogOutWidget::~CArmLogOutWidget()
{
}

void CArmLogOutWidget::initUI()
{
    logout_ui.setupUi(this);
    this->setWindowFlags(Qt::Popup);  //点击其他区域，自动关闭
    initLoginUser();
}

void CArmLogOutWidget::initLogic()
{
    connect(logout_ui.exitButton, SIGNAL(clicked()), this, SLOT(closeDialog()));  //点击“退出”
	connect(logout_ui.setButton, SIGNAL(clicked()), this, SLOT(entrySystemConfig()));  //点击“设置”
}

void CArmLogOutWidget::closeDialog()
{
    emit sigLogOut();
    CArmObject::executeCmd(CArmExitCMD());
    this->close();
}

void CArmLogOutWidget::entrySystemConfig()
{
	emit sigEntrySystemConfig();
	this->close();
}

void CArmLogOutWidget::initLoginUser()
{
    //获取指定内存中的用户登录信息
    std::string strAccount, strName;
    size_t szPriv;

    GlobalMemMapping::GetUserInfo(strAccount, strName, szPriv);

    QString strUserName = QString::fromUtf8(strName.c_str());
    QFontMetrics fontMetrics(logout_ui.userLabel->font());
    if (fontMetrics.width(strUserName) > logout_ui.userLabel->maximumWidth())
    {
        QString n_strUserName = QFontMetrics(logout_ui.userLabel->font()).elidedText(strUserName, Qt::ElideRight, logout_ui.userLabel->width());
        logout_ui.userLabel->setText(n_strUserName);
    }
    else
    {
        logout_ui.userLabel->setText(strUserName);
    }

    logout_ui.userLabel->setToolTip(strUserName);
}

void CArmLogOutWidget::mousePressEvent(QMouseEvent * e)
{
    setAttribute(Qt::WA_NoMouseReplay);
    QWidget::mousePressEvent(e);
}