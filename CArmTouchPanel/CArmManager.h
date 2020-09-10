#pragma once
#include <QObject>
#include <functional>

#include "../Common/Include/Config.h"
#include "cereal/archives/xml.hpp"
#include "cereal/types/map.hpp"
#include "cereal/types/list.hpp"
#include "cereal/types/vector.hpp"

class CArmManager : public QObject
{
    Q_OBJECT
public:
    ~CArmManager();

    static CArmManager* getInstance();


    // 写入xml 序列化（数据->文件）
    static void toXML(std::string filename, CArmConfig &config);
    // 解析xml 反序列化（文件->数据）
    static void fromXML(std::string filename, CArmConfig &config);

    /**
    * @brief 计算帧数
    * @param kv 电压值
    * @param mA 电流值
    * @param fps 帧率
    * @param exposureTime 曝光时间
    * @param frameCount 帧数
    * @return void
    */
    static int calcFrameCount(float kv, float mA, int fps, int exposureTime, DoseType doseType /*fov*/);
    
    static std::string getDefaultXMLPath();

    static std::string getSpecialXMLPath();

    static CArmLastScanConfig* scanConfigConvert();

    static CArmLastScanConfig* scanConfigConvert(const ScanPart& scanPart);

    static CArmLastScanConfig* scanConfigConvert(const ScanMode& scanMode);

    static CArmLastScanConfig* scanConfigConvert(const DoseType& doseType);

 
    // 获取默认的电流电压
    static void getKv_mA(float &kV, float &mA);
    // 计算剩余的剂量可拍摄的帧数
    static float remainingFrameCount();

    static void setScanMode(const ScanMode& scanMode);

    static float m_doseUsed;

private:
    CArmManager();

    static CArmManager* m_instance;  //!< 当前类的实例

    static std::unique_ptr<CArmLastScanConfig> m_scanConfig;

    

    static ScanMode m_scanMode;
};