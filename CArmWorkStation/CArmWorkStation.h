#pragma once

#include "FunctionalWidget\CArmFunctionalWidget.h"
#include "TrackingWidget\CArmTrackingWidget.h"
#include "StationManager\StationCommManager.h"
#include "Common\CArmWorkStationProc.h"

#pragma execution_character_set("utf-8")


/**
* @brief 移动C型臂工作站类
*/
class CArmWorkStation
{

public:


public:

    /**
    * @brief 移动C型臂工作站类构造函数
    * @param proc 外部通讯对象
    */
    CArmWorkStation(CArmWorkStationProc* proc);

    /**
    * @brief 移动C型臂工作站类析构函数
    */
    ~CArmWorkStation();


protected:


private:

    CArmWorkStationProc* m_pProcInstance;   //!< 外部通讯对象

    CArmFunctionalWidget* m_pRightWidget;   //!< 工作站右屏窗口
    CArmTrackingWidget* m_pLeftWidget;      //!< 工作站左屏窗口
    StationCommManager* m_pStManager;       //!< 工作站管理

    
};

