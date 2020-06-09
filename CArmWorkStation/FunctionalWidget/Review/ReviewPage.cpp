#include "ReviewPage.h"

ReviewPage::ReviewPage(CArmWorkStationProc* proc, QWidget* parent)
    : m_page(new ReviewPageWidget(proc, parent))
    , m_tool(new ReviewToolWidget(m_page, parent))
{
    // 注册到列表，接收指令
    CArmPage::registerPage(this);
}

ReviewPage::~ReviewPage()
{
    delete m_page;
    delete m_tool;
}

QWidget * ReviewPage::getPageWidget()
{
    return m_page;
}

QWidget * ReviewPage::getToolWidget()
{
    return m_tool;
}

void ReviewPage::updatePatientInfo(const CP_PatientData & info)
{
    m_tool->updatePatientInfo(info);
    m_page->updatePatientInfo(info);
}

void ReviewPage::clearPatientInfo()
{
    m_tool->clearPatientInfo();
    m_page->clearPatientInfo();
}


void ReviewPage::initLogic()
{
}

void ReviewPage::updateReviewInfo(const std::list<CP_ReviewData*>& info, const CP_InstanceData& inst)
{
    // 清空当前患者数据

    // 查询数据服务

    //m_page->loadReviewData
}

void ReviewPage::updateSeriesInfo(const CP_SeriesData & info)
{
    
}

