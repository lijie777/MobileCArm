#include "CArmFunctionalWidget.h"

CArmFunctionalWidget::CArmFunctionalWidget(CArmWorkStationProc* proc, QWidget *parent)
    : QFrame(parent)
    , m_pStatus((new FunctionalWidgetLogoutStatus(this, std::shared_ptr<FunctionalWidgetStatus>(new FunctionalWidgetIdleStatus(this)))))
    , m_loginWidget(new CArmLoginWidget(proc))
    , m_logoutWidget(new CArmLogOutWidget(this))
    , WorkStationProc(proc)
    , m_dataPage(new DataPage(proc))
    , m_patientPage(new PatientPage(proc))
    , m_reviewPage(new ReviewPage(proc, this))
{
    initUI();
    initLogic();

    // 注册到列表，接收指令
    CArmObject::registerObject(this);
}

CArmFunctionalWidget::~CArmFunctionalWidget()
{
    if (m_reviewPage != nullptr) delete m_reviewPage;
}

void CArmFunctionalWidget::prepareScan(const CP_StudyData & info)
{
    m_reviewPage->updatePatientInfo(info._patientInfo);
}

void CArmFunctionalWidget::clearPtInfo()
{
    m_reviewPage->clearPatientInfo();
}

void CArmFunctionalWidget::updateConfig(const CArmConfig & config)
{
}

bool CArmFunctionalWidget::UpdatePatientCompleteOver(const std::string & patient_complete_id, const std::string & studyUID)
{
    ErrorCode errUpdatePatientCompleteOver = WorkStationProc->UpdatePatientCompleteOver(patient_complete_id, studyUID);
    if (errUpdatePatientCompleteOver != ErrorCode::ERR_SUCCESS)
    {
        return false;
    }
    return true;
}

bool CArmFunctionalWidget::UpdatePatientCompleteStatus(const std::string & patient_complete_id, StudyStatus sStudyStatus)
{
    ErrorCode errUpdatePatientCompleteStatus = WorkStationProc->UpdatePatientCompleteStatus(patient_complete_id, sStudyStatus);
    if (errUpdatePatientCompleteStatus != ErrorCode::ERR_SUCCESS)
    {
        return false;
    }
    return true;
}


void CArmFunctionalWidget::setWidgetStatus(std::shared_ptr<FunctionalWidgetStatus> status)
{
    if (status != nullptr) status->dealStatus();
    m_pStatus = status;
}

void CArmFunctionalWidget::paintEvent(QPaintEvent *event)
{

}


void CArmFunctionalWidget::loginStation()
{
    CArmObject::executeCmd(CArmInitCMD());
}

void CArmFunctionalWidget::logoutStation()
{
    CArmObject::executeCmd(CArmLogoutCMD());
}

void CArmFunctionalWidget::exitSystemConfig()
{
    m_sysConfigWidget.hide();
    this->show();
}

void CArmFunctionalWidget::showReviewPage()
{
    ui.reviewBtn->setChecked(true);
    ui.pageList->setCurrentWidget(m_reviewPage->getPageWidget());
    ui.toolList->setCurrentWidget(m_reviewPage->getToolWidget());
}

void CArmFunctionalWidget::showDataPage()
{
    ui.dataBtn->setChecked(true);
    ui.pageList->setCurrentWidget(m_dataPage->getPageWidget());
    ui.toolList->setCurrentWidget(m_dataPage->getToolWidget());
}

void CArmFunctionalWidget::showPatientPage()
{
    ui.patientBtn->setChecked(true);
    ui.pageList->setCurrentWidget(m_patientPage->getPageWidget());
    ui.toolList->setCurrentWidget(m_patientPage->getToolWidget());
}

void CArmFunctionalWidget::enterSystemConfig()
{
    //this->hide();
    m_sysConfigWidget.show();
}

void CArmFunctionalWidget::initUI()
{
    ui.setupUi(this);
    m_sysConfigWidget.setParent(this);
    m_sysConfigWidget.hide();

    ui.pageList->addWidget(m_reviewPage->getPageWidget());
    ui.pageList->addWidget(m_dataPage->getPageWidget());
    ui.pageList->addWidget(m_patientPage->getPageWidget());

    ui.toolList->addWidget(m_reviewPage->getToolWidget());
    ui.toolList->addWidget(m_dataPage->getToolWidget());
    ui.toolList->addWidget(m_patientPage->getToolWidget());

}

void CArmFunctionalWidget::initLogic()
{
    connect(m_loginWidget, SIGNAL(sigLoginSuccess()), this, SLOT(loginStation()));
    connect(m_logoutWidget, SIGNAL(sigLogOut()), this, SLOT(logoutStation()));
    connect(m_logoutWidget, SIGNAL(sigEntrySystemConfig()), this, SLOT(enterSystemConfig()));
    connect(ui.menuButton, SIGNAL(clicked()), this, SLOT(showLogoutWidget()));

    connect(m_sysConfigWidget.ui->backBtn, SIGNAL(clicked()), this, SLOT(exitSystemConfig()));

    connect(ui.reviewBtn, SIGNAL(clicked()), this, SLOT(showReviewPage()));
    connect(ui.dataBtn, SIGNAL(clicked()), this, SLOT(showDataPage()));
    connect(ui.dataBtn, SIGNAL(clicked()), m_dataPage, SIGNAL(sigClickDataBtn()));
    connect(ui.patientBtn, SIGNAL(clicked()), this, SLOT(showPatientPage()));

    connect(m_patientPage, SIGNAL(sigStudyImage(DB_PatientComplete)), this, SLOT(showDataPage()));  //显示图像目录
    connect(m_patientPage, SIGNAL(sigStudyImage(DB_PatientComplete)), m_dataPage, SIGNAL(sigStudyImage(DB_PatientComplete)));  //查看检查
}

void CArmFunctionalWidget::createUI()
{
}

void CArmFunctionalWidget::doCmd(const CArmCMD& cmd)
{
    if (m_pStatus != nullptr)
    {
        m_pStatus->doCmd(cmd);
    }
}







void FunctionalWidgetStatus::initLogic()
{

}






void FunctionalWidgetIdleStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        if (m_pWidget != nullptr)
        {
            m_pWidget->setWidgetStatus(std::shared_ptr<FunctionalWidgetStatus>(new FunctionalWidgetLogoutStatus(m_pWidget, m_pWidget->m_pStatus)));
        }
    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {

    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {
        const CArmPrepareScanCMD& cmd = static_cast<const CArmPrepareScanCMD&>(cmdObject);
        // 更新患者信息
        CP_StudyData study;
        study._studyUID = cmd._scanInfo._stdUID;
        study._patientInfo = cmd._patientInfo;
        m_pWidget->prepareScan(study);
    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void FunctionalWidgetIdleStatus::dealStatus()
{
    m_pWidget->m_loginWidget->hide();
    m_pWidget->showPatientPage();
    m_pWidget->show();
}


void FunctionalWidgetReadyStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {

    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {

    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {

    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void FunctionalWidgetReadyStatus::dealStatus()
{

}


void FunctionalWidgetExposureStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {

    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {

    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {

    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void FunctionalWidgetExposureStatus::dealStatus()
{
}


void FunctionalWidgetAbortStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {

    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {

    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {

    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void FunctionalWidgetAbortStatus::dealStatus()
{
}



void FunctionalWidgetLogoutStatus::doCmd(const CArmCMD& cmdObject)
{
    // 处理注销命令
    if (cmdObject._type == CARMST_CMD::CMD_LOGOUT)
    {
        dealStatus();
    }
    // 处理退出命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXIT)
    {

    }
    // 处理初始化命令
    else if (cmdObject._type == CARMST_CMD::CMD_INITIALIZE)
    {
        m_pWidget->setWidgetStatus(m_pNxtStatus);
    }
    // 初始化设备消息处理命令
    else if (cmdObject._type == CARMST_CMD::CMD_DEVICE)
    {

    }
    // 曝光命令
    else if (cmdObject._type == CARMST_CMD::CMD_EXPOSE)
    {

    }
    // 拍片准备命令
    else if (cmdObject._type == CARMST_CMD::CMD_PREPARESCAN)
    {

    }
    // 更新配置
    else if (cmdObject._type == CARMST_CMD::CMD_CONFIG)
    {
        // 更新配置
        m_pWidget->updateConfig(static_cast<const CArmConfigCMD&>(cmdObject)._config);
    }
}

void FunctionalWidgetLogoutStatus::dealStatus()
{
    m_pWidget->hide();
    m_pWidget->m_sysConfigWidget.hide();
    m_pWidget->m_loginWidget->show();
}

void CArmFunctionalWidget::showLogoutWidget()
{
    m_logoutWidget->show();
    m_logoutWidget->move(ui.menuButton->mapToGlobal(ui.menuButton->pos()).x() - 440, ui.menuButton->mapToGlobal(ui.menuButton->pos()).y() + 50);
}
