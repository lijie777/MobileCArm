#pragma once

#include <vector>
#include <fstream>
#include <experimental/filesystem>

#include "cereal/archives/xml.hpp"
#include "cereal/types/vector.hpp"
#include "cereal/types/utility.hpp"

#include "IPCDefinitions.h"

#define CARM_CONFIG_FILE "\\Config\\special.xml"
#define CARM_DEFAULT_CONFIG_FILE "\\Config\\default.xml"

namespace fs = std::experimental::filesystem;

#pragma pack(push)
#pragma pack(1)

struct CArmPanelCalibArg
{
    PanelCalibType CalibType;               //!< PanelCalibType类型
    int FrameCount;                         //!< 帧数
    int SingleExposureTime;                 //!< 单张曝光时间（ms）
    int Framerate;                          //!< 帧率
    int Voltage;                            //!< 管电压
    int Current;                            //!< 管电流
    int Fov;                                //!< fov枚举值，对应类型为CtFov
};

struct CArmCalib
{
    std::vector<CArmPanelCalibArg> m_panelCalib;    //!< 面板校准
};

struct AutoCalib
{
    bool m_enabled;                     //!< 自动校准是否使能
    std::uint64_t m_intervalTime;       //!< 校准间隔的时间，单位：秒
};

struct FPDConfig
{
    enum DetectorMode
    {
        DarkCorrection, GainCorrection, DeffectCorrection
    };

    enum CalibMode
    {
        DarkCalib, GainCalib, DeffectCalib
    };

    FPDConfig::DetectorMode     m_detectorMode;     //!< 探测器模式
    AutoCalib                   m_autoCalib;        //!< 自动校准
    FPDConfig::CalibMode        m_calibMode;        //!< 校准模式
};

struct ImageInfoDisplay
{
    bool        m_patientName;      //!< 是否显示患者信息
    bool        m_patientBirthDate; //!< 是否显示患者出生日期
    bool        m_patientGender;    //!< 是否显示患者性别
    bool        m_patientID;        //!< 是否显示患者ID

    bool        m_studyPart;        //!< 是否显示拍摄部位
    bool        m_scanParam;        //!< 是否显示扫描参数
    bool        m_scanMode;         //!< 是否显示扫描模式

    bool        m_scanDate;         //!< 是否显示拍摄日期
    bool        m_hospitalName;     //!< 是否显示医院名称
    bool        m_hospitalNumber;   //!< 是否显示医院单号
    bool        m_marker;           //!< 是否显示标记信息
    bool        m_imgType;          //!< 是否显示影像类型
    bool        m_imgNum;           //!< 是否显示当前影像号码
    bool        m_doctor;           //!< 是否显示检查医生
    bool        m_accession;        //!< 是否显示accession

    bool        m_imgAdjust;        //!< 是否显示图像调节信息

    bool        m_doNotUseDicom;    //!< 急诊患者，禁用DICOM信息
};

enum class ScanPart
{
    UNKNOWN = -1,         //!< 未知部位
    LIMBS = 0,           //!< 四肢
    NECK,            //!< 颈部
    HEAD,            //!< 头部
    TRUNCK,          //!< 躯干
	HEART,           //!< 心脏
	ABDOMEN,         //!< 腹部
	URINARY,         //!< 泌尿系统
	SOFT_TISSUE,     //!< 软组织 
    ENDOSCOPE,	     //!< 内窥镜
    TOTAL
};

enum class DoseType
{
    REGULAR,        //!< 常规
    HIGH,           //!< 剂量增强
    LOW,             //!< 低剂量
    TOTAL
};

enum class ScanMode
{
    POINT,                  //!< 点拍
    FLUORO_MPP,             //!< 脉冲透视
    VIDEO,                  //!< 视频
    TOTAL
};

enum ServerType
{
    WORK_LIST,
    STORE,
    DICOM,
    MPPS,
    QUERY_RETRIEVE
};

struct ServerCommonCfg
{
    std::string name;           //!< 名称
    std::string AETitle;        //!< ae title
    std::string ip;             //!< ip地址
    int port;                   //!< 端口号
    std::size_t timeoutLimit;   //!< 超时设定
};

struct DicomPrinterCfg
{
    enum MediumType
    {

    };

    enum PrintDirection
    {

    };

    enum FilmSize
    {

    };

    ServerCommonCfg commonCfg;  //!< 服务器通用配置
    int minDensity;             //!< 最小打印密度
    int maxDensity;             //!< 最大打印密度
    std::size_t copies;         //!< 打印份数
    MediumType medium;          //!< 打印介质
    PrintDirection direction;   //!< 方向
    FilmSize filmSize;          //!< 胶片大小
};

struct QueryRetrieveCfg
{
    enum InfoType
    {
        PatientRoot,
        StudyRoot
    };

    ServerCommonCfg commonCfg;  //!< 服务器通用配置
    InfoType infoType;          //!< 信息类型
};

struct StoreServerCfg
{
    enum class ImgType
    {
        RF,XA,CR,SC
    };

    enum class DoseInfo
    {
        RDSR,SC
    };

    enum OverlaySendPolicy
    {
        Never,      //!< 从不发送
        Available   //!< 可用时发送
    };

    ServerCommonCfg commonCfg;  //!< 服务器通用配置
    bool sendRDSROnStudyDone;   //!< 当完成检查或者中止检查时发送RDSR
    ImgType imgType;            //!< 检查图像类型
    DoseInfo doseInfo;          //!< 剂量信息
    OverlaySendPolicy overlaySendPolicy;    //!< overlay信息
    bool activateStoreConfirmServer;       //!< 激活存储确认服务器
};

struct NetworkCfg
{
    std::string localAETitle;   //!< 本地AE title

    struct Local
    {
        bool dhcp;              //!< dhcp是否可用
        std::string ip;         //!< ip地址
        std::string subnetMask; //!< 子网掩码
        std::string macAddr;    //!< mac地址
    }local;   //!< 本地网络配置

    struct Wireless
    {
        std::string ip;         //!< ip地址
        std::string port;       //!< 端口号
        std::string subnetMask; //!< 子网掩码
        std::string macAddr;    //!< mac地址
    }wireless; //!< 无线网络配置
};

struct TimeZoneCfg
{
    enum TimeZoneType
    {

    };

    enum LanguageType
    {

    };

    enum DateFormat
    {
        MMDDYYYY,   //!< MM / DD / YYYY
        DDMMYYYY,   //!< DD / MM / YYYY
        YYYYMMDD    //!< YYYY.MM.DD
    };

    enum TimeForm
    {
        _12, _24
    };

    enum DigitFormat
    {

    };

    enum HeightUnit
    {
        cm,inch
    };

    enum WeightUnit
    {
        kg,lb
    };

    enum RadiationUnit
    {
        gray, roentgen
    };

    enum DAPUint
    {
        Gycm2, mGym2, mGycm2, Rcm2
    };

    std::string organizationName;   //!< 机构名称
    std::string organizationAddr;   //!< 机构地址
    std::string date;               //!< 日期
    std::string sn;                 //!< 序列化
    TimeZoneType timezone;          //!< 时区
    LanguageType lang;              //!< 语言
    DateFormat dateFormat;          //!< 日期显示格式
    TimeForm timeformat;            //!< 时间格式
    DigitFormat digitformat;        //!< 数字格式
    HeightUnit heightunit;          //!< 高度单位
    WeightUnit weightuint;          //!< 重量单位
    RadiationUnit radiationunit;    //!< 辐射单位
    DAPUint dapunit;                //!< dap单位
};

struct SystemInfo
{
    struct InFo
    {
        std::string hostInfo;       //!< 主机信息
        std::string sn;             //!< 序列号
        std::string maintenanceTrackingNumber;  //!< 维修追踪号
        std::string version;        //!< 当前版本
        std::string versionDate;    //!< 版本日期
    }info;                          //!< 系统信息

    struct UsageStatistics
    {
        int executedStudyNum;       //!< 已执行的检查数
        int reservedStudyNum;       //!< 已预约的检查数
        int SavedPicNumber;         //!< 保存图像的数量
    }usageStatistics;               //!< 当前系统使用统计

    struct HWInfo
    {
        std::string cArmModel;      //!< C型臂型号
        std::string sn;             //!< 序列化
    }hwInfo;                        //!< 当前系统硬件信息
};

struct ImageParam
{
    int   m_contrast;                   //!< 对比度
    int   m_brightness;                 //!< 亮度
    int   m_sharpen;                    //!< 锐化
    int   m_sharpenLevel;               //!< 锐化等级
    int   m_denoise;                    //!< 降噪
    int   m_denoiseLevel;               //!< 降噪等级
    int   m_metalCalib;                 //!< 金属校准
    bool  m_smartMetalCorrection;       //!< 是否智能金属校正
    bool  m_negative;                   //!< 是否负片
    int   m_rotate;                     //!< 旋转角度
    bool  m_reversalH;                  //!< 是否水平翻转
    bool  m_reversalV;                  //!< 是否垂直翻转
};

struct CArmFOV
{
    float m_leftRight;    //!< 左右调节
    float m_topBottom;    //!< 上下调节
};

template<class Archive>
void serialize(Archive & archive, CArmFOV & obj)
{
    archive(cereal::make_nvp("LeftRight", obj.m_leftRight),
        cereal::make_nvp("TopBottom", obj.m_topBottom));
}

struct CArmScanParam
{
    float       m_kVp;              //!< 电压
    float       m_mA;               //!< 电流
    int         m_fps;              //!< 帧率
    int         m_exposureTime;     //!< 单帧曝光时间，单位ms
    int         m_frameCount;       //!< 帧数
};

template<class Archive>
void serialize(Archive & archive, CArmScanParam & obj)
{
    archive(cereal::make_nvp("kVp", obj.m_kVp),
        cereal::make_nvp("mA", obj.m_mA),
        cereal::make_nvp("FPS", obj.m_fps),
        cereal::make_nvp("ExposureTime", obj.m_exposureTime),
        cereal::make_nvp("FrameCount", obj.m_frameCount));
}

struct CArmScanMode
{
    int m_pps;                      //!< 默认pps
    DoseType m_defaultDoseType;     //!< 默认剂量类型
    CArmScanParam m_scanParamArray[int(DoseType::TOTAL)];      //!< 剂量
};

template<class Archive>
void serialize(Archive & archive, CArmScanMode & obj)
{
    archive(cereal::make_nvp("PPS", obj.m_pps),
        cereal::make_nvp("DefaultDoseType", obj.m_defaultDoseType),
        cereal::make_nvp("ScanParamArray", obj.m_scanParamArray));
}

struct CArmScanConfig
{
    CArmFOV     m_fov;              //!< FOV视野大小
    ScanMode    m_defaultScanMode;  //!< 默认扫描模式
    CArmScanMode m_scanModeArray[int(ScanMode::TOTAL)];   //!< 扫描模式数组
    ImageParam  m_imageParam;       //!< 与此拍片配置对象的图像参数
};

struct CArmMotionCfg
{
    float m_velocity;       //!< 运动速度
    int m_timeThreshold;    //!< 判断为连续运动的时间阈值，单位ms
    float m_step;           //!< 步进值
};

struct CArmLastScanConfig
{
    ScanPart    m_scanPart;         //!< 扫描部位
    CArmFOV     m_fov;              //!< FOV视野大小
    ScanMode    m_scanModeType;     //!< 扫描模式
    int         m_pps;              //!< pps
    DoseType    m_doseType;         //!< 默认剂量类型
    bool        m_auto;             //!< 是否选择自动电压电流
    CArmScanParam m_scanParam;      //!< 拍片参数
    ImageParam  m_imageParam;       //!< 与此拍片配置对象的图像参数
};

template<class Archive>
void serialize(Archive & archive, CArmLastScanConfig & obj)
{
    archive(cereal::make_nvp("ScanPart", obj.m_scanPart),
        cereal::make_nvp("FOV", obj.m_fov),
        cereal::make_nvp("ScanModeType", obj.m_scanModeType),
        cereal::make_nvp("PPS", obj.m_pps),
        cereal::make_nvp("DoseType", obj.m_doseType),
        cereal::make_nvp("Auto", obj.m_auto),
        cereal::make_nvp("ScanParam", obj.m_scanParam),
        cereal::make_nvp("ImageParam", obj.m_imageParam));
}

struct CArmConfig
{
    bool                        m_autoTransfer;         //!< 是否在曝光开始时自动交换
    bool                        m_autoSave;             //!< 是否在曝光结束时自动保存
    bool                        m_autoPlayback;         //!< 是否在获得电影时自动回放
    bool                        m_useSmartMetal;        //!< 是否使用自动金属
    bool                        m_needToLogin;          //!< 是否需要登录
    int                         m_defaultPPS;           //!< 脉冲频率的默认值
    int                         m_TPServerPort;         //!< 触摸屏要连接的TCP服务器端口号
    int                         m_doseWarningLimit;     //!< 剂量警告限值
    int                         m_videoModeTimeLimit;   //!< 拍摄视频模式时，设定视频时间，单位s
    bool                        m_movieCountDownTip;    //!< 使用视频功能时提前5s报警提示
    std::size_t                 m_studyReserveDays;     //!< 检查保留天数
    std::string                 m_emergencyPatientName; //!< 急诊患者名称
    std::string                 m_emergencyPatientID;   //!< 急诊患者ID
    std::pair<bool, int>        m_systemSleep;          //!< 系统休眠
    ScanPart                    m_defaultScanPart;     //!< 成像模式的默认值
    FPDConfig                   m_FPDConfig;            //!< 平板探测器配置
    ImageInfoDisplay            m_imageInfoDisplay;     //!< 影像四角信息显示
    CArmScanConfig m_scanPartArray[int(ScanPart::TOTAL)];           //!< 扫描部位数组
    std::vector<ServerCommonCfg> m_worklistServerCfg;   //!< Worklist服务器配置
    std::vector<DicomPrinterCfg> m_dicomPrinterCfg;     //!< Dicom打印机配置
    std::vector<QueryRetrieveCfg> m_queryRetrieveCfg;   //!< query retrieve配置
    std::vector<StoreServerCfg> m_storeServerCfg;       //!< 存储服务器配置
    NetworkCfg                  m_networkCfg;           //!< 本地网络配置
    TimeZoneCfg                 m_timeZoneCfg;          //!< 时间区域设置
    SystemInfo                  m_systemInfo;           //!< 系统信息
    CArmMotionCfg               m_axisMotionX;          //!< x轴运动控制
    CArmMotionCfg               m_axisMotionY;          //!< y轴运动控制
    CArmMotionCfg               m_axisMotionZ;          //!< z轴运动控制
    CArmMotionCfg               m_collimatorMotionH;    //!< 限束器水平运动控制
    CArmMotionCfg               m_collimatorMotionV;    //!< 限束器垂直运动控制
    int                         m_collimatorStep;       //!< 限束器运动步长设定，单位mm
    std::map<float, float>      m_KV_mGymAs_map;        //!< 电压和每毫安秒的剂量的映射关系
    CArmCalib                   m_calib;                //!< 校准配置
};

#pragma pack(pop)

class CArmConfigHandler
{
public:
    /**
     * @brief 将配置文件中的信息转化为C++对象
     * @return CArmConfig 和配置文件中信息对应的C++对象
     */
    static CArmConfig Deserialize()
    {
        if (!fs::exists(CArmConfigHandler::CfgFilePath()))
        {
            if (!fs::exists(CArmConfigHandler::DefaultCfgFilePath()))
            {
                return {};
            }
            fs::copy(CArmConfigHandler::DefaultCfgFilePath(), CArmConfigHandler::CfgFilePath());
        }

        std::ifstream is(CArmConfigHandler::CfgFilePath());
        cereal::XMLInputArchive iarchive(is);
        CArmConfig cfg;
        iarchive(cfg);
        return cfg;
    }

    /**
     * @brief 将C++对象中写入配置文件
     * @param cfg 存有配置信息的C++对象
     * @return void 
     */
    static void Serialize(const CArmConfig& cfg)
    {
        std::ofstream os(CArmConfigHandler::CfgFilePath());
        cereal::XMLOutputArchive oarchive(os);
        oarchive(cereal::make_nvp("CArmConfig", cfg));
    }

    /**
     * @brief 重置配置文件
     * @return CArmConfig 重置后的配置信息
     */
    static CArmConfig Reset()
    {
        fs::remove(CArmConfigHandler::CfgFilePath());
        return CArmConfigHandler::Deserialize();
    }

    /**
     * @brief 配置文件路径
     * @return std::string 配置文件绝对路径
     */
    static std::string CfgFilePath()
    {
        return UCommonUtility::GetExeDir() + CARM_CONFIG_FILE;
    }

    /**
     * @brief 默认配置文件路径
     * @return std::string 默认配置文件绝对路径
     */
    static std::string DefaultCfgFilePath()
    {
        return UCommonUtility::GetExeDir() + CARM_DEFAULT_CONFIG_FILE;;
    }

    /**
     * @brief 获取配置文件的文本内容
     * @return std::vector<char> 存放配置文件内存的对象
     */
    static std::vector<char> GetCfgText()
    {
        if (!fs::exists(CfgFilePath()))
        {
            return{};
        }

        std::vector<char> retval(fs::file_size(CfgFilePath()));
        if (std::ifstream ifs{ CfgFilePath() })
        {
            ifs.read(retval.data(), retval.size());
        }
        return retval;
    }
};

template<class Archive>
void serialize(Archive & archive, CArmConfig & obj)
{
    archive(cereal::make_nvp("DefaultScanPart", obj.m_defaultScanPart),
        cereal::make_nvp("ScanPartArray", obj.m_scanPartArray),
        //    cereal::make_nvp("LastScanConfig", obj.m_lastScanConfig),
        cereal::make_nvp("AutoTransfer", obj.m_autoTransfer),
        cereal::make_nvp("AutoSave", obj.m_autoSave),
        cereal::make_nvp("AutoPlayback", obj.m_autoPlayback),
        cereal::make_nvp("MovieCountDownTip", obj.m_movieCountDownTip),
        cereal::make_nvp("UseSmartMetal", obj.m_useSmartMetal),
        cereal::make_nvp("NeedToLogin", obj.m_needToLogin),
        cereal::make_nvp("TimeZoneCfg", obj.m_timeZoneCfg),
        cereal::make_nvp("TPServerPort", obj.m_TPServerPort),
        cereal::make_nvp("DoseWarningLimit", obj.m_doseWarningLimit),
        cereal::make_nvp("VideoModeTimeLimit", obj.m_videoModeTimeLimit),
        cereal::make_nvp("StudyReserveDays", obj.m_studyReserveDays),
        cereal::make_nvp("EmergencyPatientName", obj.m_emergencyPatientName),
        cereal::make_nvp("EmergencyPatientID", obj.m_emergencyPatientID),
     //   cereal::make_nvp("UseDefaultConfig", obj.m_useDefaultConfig),
        cereal::make_nvp("SystemSleep", obj.m_systemSleep),
        cereal::make_nvp("DefaultPPS", obj.m_defaultPPS),
        cereal::make_nvp("FPDConfig", obj.m_FPDConfig),
        cereal::make_nvp("ImageInfoDisplay", obj.m_imageInfoDisplay),
        cereal::make_nvp("WorklistServerCfg", obj.m_worklistServerCfg),
        cereal::make_nvp("DicomPrinterCfg", obj.m_dicomPrinterCfg),
        cereal::make_nvp("QueryRetrieveCfg", obj.m_queryRetrieveCfg),
        cereal::make_nvp("StoreServerCfg", obj.m_storeServerCfg),
        cereal::make_nvp("NetworkCfg", obj.m_networkCfg),
        cereal::make_nvp("SystemInfo", obj.m_systemInfo),
        cereal::make_nvp("AxisMotionX", obj.m_axisMotionX),
        cereal::make_nvp("AxisMotionY", obj.m_axisMotionY),
        cereal::make_nvp("AxisMotionZ", obj.m_axisMotionZ),
        cereal::make_nvp("CollimatorMotionH", obj.m_collimatorMotionH),
        cereal::make_nvp("CollimatorMotionV", obj.m_collimatorMotionV),
        cereal::make_nvp("CollimatorStep", obj.m_collimatorStep),
        cereal::make_nvp("KV_mGymAs_map", obj.m_KV_mGymAs_map),
        cereal::make_nvp("Calib", obj.m_calib));
}

template<class Archive>
void serialize(Archive & archive, CArmMotionCfg & obj)
{
    archive(cereal::make_nvp("Velocity", obj.m_velocity),
        cereal::make_nvp("TimeThreshold", obj.m_timeThreshold),
        cereal::make_nvp("Step", obj.m_step));
}

template<class Archive>
void serialize(Archive & archive, ServerCommonCfg & obj)
{
    archive(cereal::make_nvp("Name", obj.name),
        cereal::make_nvp("AETitle", obj.AETitle),
        cereal::make_nvp("Ip", obj.ip),
        cereal::make_nvp("Port", obj.port),
        cereal::make_nvp("TimeoutLimit", obj.timeoutLimit));
}

template<class Archive>
void serialize(Archive & archive, SystemInfo & obj)
{
    archive(cereal::make_nvp("Info", obj.info),
        cereal::make_nvp("UsageStatistics", obj.usageStatistics),
        cereal::make_nvp("HWInfo", obj.hwInfo));
}

template<class Archive>
void serialize(Archive & archive, SystemInfo::InFo & obj)
{
    archive(cereal::make_nvp("HostInfo", obj.hostInfo),
        cereal::make_nvp("SN", obj.sn),
        cereal::make_nvp("MaintenanceTrackingNumber", obj.maintenanceTrackingNumber),
        cereal::make_nvp("version", obj.version),
        cereal::make_nvp("VersionDate", obj.versionDate));
}

template<class Archive>
void serialize(Archive & archive, SystemInfo::UsageStatistics & obj)
{
    archive(cereal::make_nvp("ExecutedStudyNum", obj.executedStudyNum),
        cereal::make_nvp("ReservedStudyNum", obj.reservedStudyNum),
        cereal::make_nvp("SavedPicNumber", obj.SavedPicNumber));
}

template<class Archive>
void serialize(Archive & archive, SystemInfo::HWInfo & obj)
{
    archive(cereal::make_nvp("CArmModel", obj.cArmModel),
        cereal::make_nvp("SN", obj.sn));
}

template<class Archive>
void serialize(Archive & archive, CArmScanConfig & obj)
{
    archive(cereal::make_nvp("FOV", obj.m_fov),
        cereal::make_nvp("DefaultScanMode", obj.m_defaultScanMode),
        cereal::make_nvp("ScanModeArray", obj.m_scanModeArray),
        cereal::make_nvp("ImageParam", obj.m_imageParam));
}

template <class Archive>
void serialize(Archive & archive, ImageParam & obj)
{
    archive(cereal::make_nvp("Contrast", obj.m_contrast),
        cereal::make_nvp("Brightness", obj.m_brightness),
        cereal::make_nvp("Sharpen", obj.m_sharpen),
        cereal::make_nvp("SharpenLevel", obj.m_sharpenLevel),
        cereal::make_nvp("Denoise", obj.m_denoise),
        cereal::make_nvp("DenoiseLevel", obj.m_denoiseLevel),
        cereal::make_nvp("MetalCalib", obj.m_metalCalib),
        cereal::make_nvp("SmartMetalCorrection", obj.m_smartMetalCorrection),
        cereal::make_nvp("Negative", obj.m_negative),
        cereal::make_nvp("Roatate", obj.m_rotate),
        cereal::make_nvp("ReversalH", obj.m_reversalH),
        cereal::make_nvp("ReversalV", obj.m_reversalV));
}

template <class Archive>
void serialize(Archive & archive, AutoCalib & obj)
{
    archive(cereal::make_nvp("Enabled", obj.m_enabled),
        cereal::make_nvp("IntervalTime", obj.m_intervalTime));
}

template <class Archive>
void serialize(Archive & archive, FPDConfig & obj)
{
    archive(cereal::make_nvp("DetectorMode", obj.m_detectorMode),
        cereal::make_nvp("AutoCalib", obj.m_autoCalib),
        cereal::make_nvp("CalibMode", obj.m_calibMode));
}

template<class Archive>
void serialize(Archive & archive, ImageInfoDisplay & obj)
{
    archive(cereal::make_nvp("PatientName", obj.m_patientName),
        cereal::make_nvp("PatientBirthDate", obj.m_patientBirthDate),
        cereal::make_nvp("PatientGender", obj.m_patientGender),
        cereal::make_nvp("PatientID", obj.m_patientID),

        cereal::make_nvp("StudyPart", obj.m_studyPart),
        cereal::make_nvp("ScanParam", obj.m_scanParam),
        cereal::make_nvp("ScanMode", obj.m_scanMode),

        cereal::make_nvp("ScanDate", obj.m_scanDate),
        cereal::make_nvp("HospitalName", obj.m_hospitalName),
        cereal::make_nvp("HospitalNumber", obj.m_hospitalNumber),
        cereal::make_nvp("Marker", obj.m_marker),
        cereal::make_nvp("ImgType", obj.m_imgType),
        cereal::make_nvp("ImgNum", obj.m_imgNum),
        cereal::make_nvp("Doctor", obj.m_doctor),
        cereal::make_nvp("Accession", obj.m_accession),

        cereal::make_nvp("ImgAdjust", obj.m_imgAdjust),

        cereal::make_nvp("DoNotUseDicom", obj.m_doNotUseDicom));
}

template<class Archive>
void serialize(Archive & archive, CArmPanelCalibArg & obj)
{
    archive(cereal::make_nvp("CalibType", obj.CalibType),
        cereal::make_nvp("FrameCount", obj.FrameCount),
        cereal::make_nvp("SingleExposureTime", obj.SingleExposureTime),
        cereal::make_nvp("Framerate", obj.Framerate),
        cereal::make_nvp("Voltage", obj.Voltage),
        cereal::make_nvp("Current", obj.Current),
        cereal::make_nvp("Fov", obj.Fov));
}

template<class Archive>
void serialize(Archive & archive, CArmCalib & obj)
{
    archive(cereal::make_nvp("PanelCalib", obj.m_panelCalib));
}

template<class Archive>
void serialize(Archive & archive, TimeZoneCfg & obj)
{
    archive(cereal::make_nvp("organizationName", obj.organizationName),
        cereal::make_nvp("OrganizationAddr", obj.organizationAddr),
        cereal::make_nvp("Date", obj.date),
        cereal::make_nvp("SN", obj.sn),
        cereal::make_nvp("TimeZone", obj.timezone),
        cereal::make_nvp("Lang", obj.lang),
        cereal::make_nvp("DateFormat", obj.dateFormat),
        cereal::make_nvp("TimeFormat", obj.timeformat),
        cereal::make_nvp("DigitFormat", obj.digitformat),
        cereal::make_nvp("HeightUnit", obj.heightunit),
        cereal::make_nvp("WeightUint", obj.weightuint),
        cereal::make_nvp("RadiationUnit", obj.radiationunit),
        cereal::make_nvp("DapUnit", obj.dapunit));
}

template<class Archive>
void serialize(Archive & archive, DicomPrinterCfg & obj)
{
    archive(cereal::make_nvp("CommonCfg", obj.commonCfg),
        cereal::make_nvp("MinDensity", obj.minDensity),
        cereal::make_nvp("MaxDensity", obj.maxDensity),
        cereal::make_nvp("Copies", obj.copies),
        cereal::make_nvp("Medium", obj.medium),
        cereal::make_nvp("Direction", obj.direction),
        cereal::make_nvp("FilmSize", obj.filmSize));
}

template<class Archive>
void serialize(Archive & archive, QueryRetrieveCfg & obj)
{
    archive(cereal::make_nvp("CommonCfg", obj.commonCfg),
        cereal::make_nvp("InfoType", obj.infoType));
}

template<class Archive>
void serialize(Archive & archive, StoreServerCfg & obj)
{
    archive(cereal::make_nvp("CommonCfg", obj.commonCfg),
        cereal::make_nvp("SendRDSROnStudyDone", obj.sendRDSROnStudyDone),
        cereal::make_nvp("ImgType", obj.imgType),
        cereal::make_nvp("DoseInfo", obj.doseInfo),
        cereal::make_nvp("OverlaySendPolicy", obj.overlaySendPolicy),
        cereal::make_nvp("ActivateStoreConfirmServer", obj.activateStoreConfirmServer));
}

template<class Archive>
void serialize(Archive & archive, NetworkCfg & obj)
{
    archive(cereal::make_nvp("LocalAETitle", obj.localAETitle),
        cereal::make_nvp("Local", obj.local),
        cereal::make_nvp("Wireless", obj.wireless));
}

template<class Archive>
void serialize(Archive & archive, NetworkCfg::Local & obj)
{
    archive(cereal::make_nvp("DHCP", obj.dhcp),
        cereal::make_nvp("Ip", obj.ip),
        cereal::make_nvp("SubnetMask", obj.subnetMask),
        cereal::make_nvp("MacAddr", obj.macAddr));
}

template<class Archive>
void serialize(Archive & archive, NetworkCfg::Wireless & obj)
{
    archive(cereal::make_nvp("Ip", obj.ip),
        cereal::make_nvp("Port", obj.port),
        cereal::make_nvp("SubnetMask", obj.subnetMask),
        cereal::make_nvp("MacAddr", obj.macAddr));
}