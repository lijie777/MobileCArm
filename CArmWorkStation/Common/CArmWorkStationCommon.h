#ifndef CARMWORKSTATIONCOMMON_H
#define CARMWORKSTATIONCOMMON_H

#pragma once
#include "DbDataStructures.h"




/**
* @brief 移动C型臂工作站通用功能函数
*/
class CArmWorkStationCommon
{
public:
    CArmWorkStationCommon();
    ~CArmWorkStationCommon();

    /**
    * @brief 开始拍片
    * @param sPatient 患者信息
    * @param sPatientCompleteID 患者已执行信息实例ID（数据库中用到）
    * @return 返回状态
    */
    static void ToStartScan(const DB_Patient &sPatient,const std::string &sPatientCompleteID);


};

#endif // CARMWORKSTATIONCOMMON_H