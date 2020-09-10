#include <QDate>
#include "Common\CArmCMD.h"
#include "DcmUidDef.h"
#include "CArmScCmdDef.h"
#include "CArmWorkStationCommon.h"


CArmWorkStationCommon::CArmWorkStationCommon()
{
}


CArmWorkStationCommon::~CArmWorkStationCommon()
{
}


void CArmWorkStationCommon::ToStartScan(const DB_Patient &sPatient, const std::string &sPatientCompleteID)
{
    int age = QDate::currentDate().year() - QDate::fromString(sPatient.patient_birth_date, "yyyyMMdd").year();
    std::string name = std::string(sPatient.patient_first_name) + " " + std::string(sPatient.patient_last_name);
    //发送患者信息
    CP_PatientData info{
        std::string(sPatient.patient_id),          name,
        std::string(sPatient.patient_first_name),  std::string(sPatient.patient_last_name),
        std::to_string(age),                            std::string(sPatient.patient_birth_date),
        std::string(sPatient.patient_sex),         sPatient.is_emergency };

    std::string stdUID = UDcmUid::CreateStudyUID(DeviceType::MobileCArm);
    CP_StudyData stdInfo{
        stdUID ,std::string(sPatient.study_id),                   
        std::string(sPatient.aession),QDate::currentDate().toString("yyyyMMdd").toLocal8Bit().data(),
        QTime::currentTime().toString("hhmmss").toLocal8Bit().data(),std::string(""),                                      
        std::string(sPatient.patient_desc),std::string(sPatient.referral_doctor),
        sPatientCompleteID , info };
    

    CP_ScanRespInfo scanInfo{ stdUID, ScanPart::UNKNOWN };
    CArmObject::executeCmd(CArmPrepareScanCMD(scanInfo, info));
}

