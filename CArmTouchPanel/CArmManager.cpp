#include "CArmManager.h"
#include <fstream>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <qapplication.h>
#include <cmath>

CArmManager * CArmManager::m_instance = nullptr;

std::unique_ptr<CArmLastScanConfig> CArmManager::m_scanConfig(new CArmLastScanConfig());

float CArmManager::m_doseUsed = 0.0;

ScanMode CArmManager::m_scanMode = ScanMode::FLUORO_MPP;

CArmManager::CArmManager()
{
    
}

CArmManager::~CArmManager()
{
    
}
CArmManager* CArmManager::getInstance()
{
    if (nullptr == m_instance)
    {
        m_instance = new CArmManager;
    }
    return m_instance;
}

// 写入xml 序列化（数据->文件）
void CArmManager::toXML(std::string filename, CArmConfig &config)
{
    if (QFileInfo::exists(QString(filename.c_str())))
    {
        std::ofstream is(filename);
        cereal::XMLOutputArchive archive(is);
        //archive(config);
        archive(cereal::make_nvp("CArmConfig", config));
    }
}
// 解析xml 反序列化（文件->数据）
void CArmManager::fromXML(std::string filename, CArmConfig &config)
{
    if (QFileInfo::exists(QString(filename.c_str())))
    {
        std::ifstream is(filename);
        cereal::XMLInputArchive archive(is);
        archive(config);
    }
    
}
std::string CArmManager::getDefaultXMLPath()
{
    return (QCoreApplication::applicationDirPath() + "/Config/default.xml").toStdString();
}

std::string CArmManager::getSpecialXMLPath()
{
    return (QCoreApplication::applicationDirPath() + "/Config/special.xml").toStdString();
}

int CArmManager::calcFrameCount(float kv, float mA, int fps, int exposureTime, DoseType doseType/*fov*/)
{
    return 100;
}

CArmLastScanConfig* CArmManager::scanConfigConvert()
{
    return m_scanConfig.get();
}

CArmLastScanConfig* CArmManager::scanConfigConvert(const ScanPart& scanPart)
{
    CArmConfig config = CArmConfigHandler::Deserialize();

    ScanMode scanMode = config.m_scanPartArray[(int)scanPart].m_defaultScanMode;
    DoseType doseType = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_defaultDoseType;
    int pps = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_pps;
   
    CArmFOV fov = config.m_scanPartArray[(int)scanPart].m_fov;
    CArmScanParam scanPara = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_scanParamArray[(int)doseType];
    ImageParam  imageParam = config.m_scanPartArray[(int)scanPart].m_imageParam;
    
    m_scanConfig->m_scanPart = scanPart;
    m_scanConfig->m_fov = fov;
    m_scanConfig->m_imageParam = imageParam;
    m_scanConfig->m_doseType = doseType;
    m_scanConfig->m_pps = pps;
    m_scanConfig->m_scanModeType = scanMode;
    m_scanConfig->m_scanParam = scanPara;
    m_scanConfig->m_auto = true;

    m_scanMode = ScanMode::FLUORO_MPP;

    return m_scanConfig.get();
}

CArmLastScanConfig* CArmManager::scanConfigConvert(const ScanMode& scanMode)
{
    CArmConfig config = CArmConfigHandler::Deserialize();

    ScanPart scanPart = m_scanConfig->m_scanPart;
    DoseType doseType = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_defaultDoseType;
    int pps = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_pps;
    CArmFOV fov = config.m_scanPartArray[(int)scanPart].m_fov;
    CArmScanParam scanPara = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_scanParamArray[(int)doseType];
    ImageParam  imageParam = config.m_scanPartArray[(int)scanPart].m_imageParam;

    m_scanConfig->m_scanPart = scanPart;
    m_scanConfig->m_fov = fov;
    m_scanConfig->m_imageParam = imageParam;
    m_scanConfig->m_doseType = doseType;
    m_scanConfig->m_pps = pps;
    m_scanConfig->m_scanModeType = scanMode;
    m_scanConfig->m_scanParam = scanPara;
    m_scanConfig->m_auto = true;

    return m_scanConfig.get();
}

CArmLastScanConfig* CArmManager::scanConfigConvert(const DoseType& doseType)
{
    CArmConfig config = CArmConfigHandler::Deserialize();

    ScanPart scanPart = m_scanConfig->m_scanPart;
    ScanMode scanMode = m_scanConfig->m_scanModeType;

    int pps = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_pps;

    CArmFOV fov = config.m_scanPartArray[(int)scanPart].m_fov;
    CArmScanParam scanPara = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_scanParamArray[(int)doseType];
    ImageParam  imageParam = config.m_scanPartArray[(int)scanPart].m_imageParam;

    m_scanConfig->m_scanPart = scanPart;
    m_scanConfig->m_fov = fov;
    m_scanConfig->m_imageParam = imageParam;
    m_scanConfig->m_doseType = doseType;
    m_scanConfig->m_pps = pps;
    m_scanConfig->m_scanModeType = scanMode;
    m_scanConfig->m_scanParam = scanPara;
    m_scanConfig->m_auto = true;

    return m_scanConfig.get();
}

void CArmManager::getKv_mA(float &kV, float &mA)
{
    CArmConfig config = CArmConfigHandler::Deserialize();

    ScanPart scanPart = m_scanConfig->m_scanPart;
    ScanMode scanMode = m_scanConfig->m_scanModeType;
    DoseType doseType = m_scanConfig->m_doseType;
  

    kV = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_scanParamArray[(int)doseType].m_kVp;
    mA = config.m_scanPartArray[(int)scanPart].m_scanModeArray[(int)scanMode].m_scanParamArray[(int)doseType].m_mA;
}

float CArmManager::remainingFrameCount()
{
    CArmConfig config = CArmConfigHandler::Deserialize();

    float totalDose = config.m_doseWarningLimit;//一个study的剂量
    float fovArea = m_scanConfig.get()->m_fov.m_leftRight * m_scanConfig.get()->m_fov.m_topBottom;
    float remainingDose = totalDose - m_doseUsed;
    float mGy_mAs = config.m_KV_mGymAs_map[m_scanConfig.get()->m_scanParam.m_kVp];
    float mA = m_scanConfig.get()->m_scanParam.m_mA;
    float expTime = m_scanConfig.get()->m_scanParam.m_exposureTime / 1000.0;
    
    float dosePerFrame = (float)(mGy_mAs * mA * expTime * fovArea);

    int remainingFrameCount = 0;
    if (dosePerFrame != 0 && !std::isinf(dosePerFrame))
    {
        remainingFrameCount = (int)(remainingDose / dosePerFrame);
    }
    else
    {
        return -1;
    }
    

    if (m_scanMode == ScanMode::VIDEO)
    {
        // 指定时间内视频模式下能拍摄的帧数
        int videoFrames = config.m_videoModeTimeLimit * m_scanConfig.get()->m_pps;

        if (videoFrames > remainingFrameCount)
        {
            return remainingFrameCount;
        }// 指定时间内当前剂量不够拍摄
        else
        {
            return videoFrames;
        }// 指定时间内当前剂量能够拍摄
    }
    else
    {
        return remainingFrameCount;
    }
    

}

void CArmManager::setScanMode(const ScanMode& scanMode)
{
    m_scanMode = m_scanMode;
}

