#include "CArmPage.h"

std::list<CArmPage*> CArmPage::m_Lst;

void CArmPage::registerPage(CArmPage* page)
{
    m_Lst.push_back(page); 
}
void CArmPage::notifyStudyInfo(const CP_StudyData & info)
{
    for (auto & i : m_Lst)
    {
        i->updateStudyInfo(info);
    }
}

void CArmPage::notifySeriesInfo(const CP_SeriesData & info)
{
    for (auto & i : m_Lst)
    {
        i->updateSeriesInfo(info);
    }
}

void CArmPage::notifyImageInfo(const CP_ImageData & info)
{
    for (auto & i : m_Lst)
    {
        i->updateImageInfo(info);
    }
}

void CArmPage::notifyReviewInfo(const std::list<CP_ReviewData*>& info, const CP_InstanceData& inst)
{
    for (auto & i : m_Lst)
    {
        i->updateReviewInfo(info, inst);
    }
}