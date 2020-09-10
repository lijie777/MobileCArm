#include "CArmWorkStation.h"

CArmWorkStation::CArmWorkStation(CArmWorkStationProc *proc)
    : m_pLeftWidget(new CArmTrackingWidget(nullptr))
    , m_pRightWidget(new CArmFunctionalWidget(proc))
    , m_pStManager(new StationCommManager(proc))
    , m_pProcInstance(proc)
{
   CArmObject::executeCmd(CArmLogoutCMD());
}

CArmWorkStation::~CArmWorkStation()
{
    //if (m_pLeftWidget != nullptr) delete m_pLeftWidget;
    //if (m_pRightWidget != nullptr) delete m_pRightWidget;
    //if (m_pStManager != nullptr) delete m_pStManager;
}

