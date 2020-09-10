#ifndef CARMWORKSTATIONPROC_H
#define CARMWORKSTATIONPROC_H

#pragma once

#include "CArmDeviceComm.h"
#include "Config.h"
#include "TouchPanelComm.h"

class CArmWorkStationProc : public UCArmDeviceComm
{
public:
    CArmWorkStationProc();
    ~CArmWorkStationProc();

    bool LoadConfig();

    /**
    * @brief 启动自身提供的服务
    * @return bool 是否启动成功
    */
    bool StartServices() override;

    bool getLocalDataSource(IPCLocalDataSource &pSource);

    //检查本地数据库患者ID是否已经存在
    ErrorCode CheckPatientExistence(const std::string &strPatientID, bool &bExist);

    //已执行信息完成拍摄,本次检查下第一次完成拍摄
    ErrorCode UpdatePatientCompleteOver(const std::string &patient_complete_id, const std::string &studyUID);

    //更新执行信息状态
    ErrorCode UpdatePatientCompleteStatus(const std::string &patient_complete_id, StudyStatus sStudyStatus);

    // 添加新患者信息到本地数据库
    ErrorCode AddNewPatients(int nCount, const DB_Patient *pPatientInfo);

    // 添加新患者信息到正在拍片,得到已执行信息ID
    ErrorCode AddPatientToComplete(const std::string &patientID, std::string &completeID);

    //更新患者信息到本地数据库
    ErrorCode UpdatePatient(const std::string &patientID, const DB_Patient *pPatientInfo);

    // 添加新患者预约信息到本地数据库
    ErrorCode AddNewPatientsSchedule(int nCount, const DB_PatientSchedule *pPatientScheduleInfo);

    // 新增预约信息到已执行信息
    ErrorCode AddPatientScheduleToComplete(const std::string &patient_schedule_id, std::string &patient_complete_id);

    // 删除患者预约信息
    ErrorCode DeletePatientsSchedule(const std::string &patientScheduleId);

    // 删除患者已执行信息
    ErrorCode DeletePatientsComplete(const std::string &patientCompleteId);

    //查询本地检查日期列表信息（小C）
    ErrorCode GetTmpStudyDateByPatient(const std::string &patientID, int &nCount, DB_StudyDate **ppDB_StudyDate);

    //通过患者执行ID查询本地拍摄列表信息（小C）
    ErrorCode GetTmpSeriesByComplete(const std::string &patientCompleteID, int &nCount, DB_TmpSeries **ppDB_TmpSeries);

    //通过检查UID查询本地拍摄列表信息（小C）
    ErrorCode GetTmpSeriesByStudy(const std::string &studyUID, int &nCount, DB_TmpSeries **ppDB_TmpSeries);

    ErrorCode GetLocalPatients(const SearchCondition &lstQueryCondition, int &nPatientCount, DB_Patient **pPatientInfo);

    ErrorCode GetLocalPatientsSchedule(const SearchCondition &lstQueryCondition, int &nPatientCount, DB_PatientSchedule **pPatientScheduleInfo);

    ErrorCode GetLocalPatientsComplete(const SearchCondition &lstQueryCondition, int &nPatientCount, DB_PatientComplete **pPatientCompleteInfo);

private:
    ErrorCode GetPatients(const std::string &strPos, const SearchCondition &lstQueryCondition, int &nPatientCount, DB_Patient **pPatientInfo);

    ErrorCode GetPatientsSchedule(const std::string &strPos, const SearchCondition &lstQueryCondition, int &nPatientCount, DB_PatientSchedule **pPatientScheduleInfo);

    ErrorCode GetPatientsComplete(const std::string &strPos, const SearchCondition &lstQueryCondition, int &nPatientCount, DB_PatientComplete **pPatientCompleteInfo);

    void GetCalibratedImage(unsigned short *pOut, unsigned short *pIn, int nImgPixelCount);
    /**
     * @brief 连接设备进程传来的消息
     * @return void 
     */
    void ConnectDevMsg();
    bool InitTPCtrl();

    /**
     * @brief 通知观察者新的消息
     * @param up 指向消息的智能指针
     * @return void 
     */
    template <typename T>
    void ScanInfoServerMsg(std::unique_ptr<T> up)
    {
        WorkStationMsgCtrl::GetInstance()->Request(ClientType::SCAN_INFO_SERVER, up.get());
    }

    void Workstation2ScanInfoServer(const CArmCmdBase* cmd);

protected:
    CArmConfig              m_carmConfig;       //!< 配置信息
    TouchPanelComm*         m_tpComm;           //!< 触摸屏对象
};


#endif // CARMWORKSTATIONPROC_H