#pragma once

#include "TextureData.h"
#include "TextureObject3D.h"
#include "Config.h"
#include "CArmDT.h"
#include <map>

// 某元素是否存在于容器中
template <typename C, typename V>
bool UegContains(const C& c, const V v)
{
    return std::end(c) != std::find(std::begin(c), std::end(c), v);
}

template <typename T, typename V>
bool UegContains(const std::initializer_list<T>& c, const V v)
{
    return UegContains<std::initializer_list<T>, V>(c, v);
}


/**
* @brief 拍摄数据结构
*/
struct CP_ScanRespInfo
{
    std::string _stdUID;    //!< 检查UID
    ScanPart _bodyPart;     //!< 部位
};


struct CP_PatientData
{
    std::string _patientID;             //!< 患者编号
    std::string _patientName;           //!< 患者姓名
    std::string _patientFirstName;      //!< 患者名
    std::string _patientLastName;       //!< 患者姓
    std::string _patientAge;            //!< 患者年龄
    std::string _ptBirthDate;           //!< 患者生日
    std::string _patientSex;            //!< 患者性别
    bool _isEmergency;                  //!< 紧急标志
};

struct CP_StudyData
{
    std::string _studyUID;          //!< 检查标识号
    std::string _studyID;           //!< 检查编号
    std::string _accessNumber;      //!< 检查序号
    std::string _studyDate;         //!< 检查日期
    std::string _studyTime;         //!< 检查时间
    std::string _modality;          //!< 检查设备
    std::string _description;       //!< 检查描述
    std::string _doctor;            //!< 检查医师
    std::string _patientCompleteID; //!< 患者已执行信息实例ID（数据库中用到）
    CP_PatientData _patientInfo;    //!< 患者信息
};

struct CP_SeriesData
{
    std::string _seriesUID;          //!< 拍摄标识号
    std::string _seriesPath;         //!< 拍摄文件目录
    std::string _seriesNumber;       //!< 拍摄序号
    std::string _seriesDate;         //!< 拍摄日期
    std::string _seriesTime;         //!< 拍摄时间
    std::string _bodypart;           //!< 拍摄部位
    std::string _dose;               //!< 拍摄剂量
    std::string _description;        //!< 拍摄描述
    std::string _type;               //!< 图像类型
    std::string _pulse;              //!< 脉冲频率
    bool _autoMode;                  //!< 自动模式
    ImageParam _imageParam;          //!< 图像参数
    CP_StudyData _studyInfo;         //!< 检查信息
};

struct CP_ImageData
{
    std::string _imageUID;          //!< 图像标识号
    std::string _filePath;          //!< 图像路径
    std::string _accessoryPath;     //!< 操作记录文件路径
    int _imageIdx;                  //!< 图像序号
    int _imageWidth;                //!< 图像宽度
    int _imageHeight;               //!< 图像高度
    int _windowCenter;              //!< 窗位
    int _windowWidth;               //!< 窗宽
    float _pixelSpacingX;           //!< 像素水平分辨率
    float _pixelSpacingY;           //!< 像素竖直分辨率
    float _KVP;                     //!< 电压
    float _XrayCurrent;             //!< 管电流
    int _exposureTime;              //!< 曝光时间(ms)
    int _bitsAllocated;             //!< 每个像素占用的位数
    int _bitsStored;                //!< 每个像素的有效位数
    int _highBit;                   //!< 符号位
    bool _pixelIssigned;            //!< 像素数据是否有符号
    unsigned short* _imageData;     //!< 图像像素
    CP_SeriesData _sereisInfo;      //!< 拍摄信息
};



/**
* @brief 实例信息数据结构
*/
struct CP_InstanceData
{
    std::string patientID;  //!< 患者编号
    std::string studyID;    //!< 检查编号
    std::string seriesID;   //!< 拍摄编号
    std::string imageID;    //!< 图像编号
};

/**
* @brief 扫描信息数据结构
*/
struct CP_ScanData
{
    CArmScanConfig scanParms;       //!< 拍摄参数
    bool isAutoParms;               //!< 是否为自动参数
    int scanCount;                  //!< 拍摄帧数
    int currentIdx;                 //!< 当前拍摄计数
};


/**
* @brief 移动C型臂图像锐化工具枚举
*/
enum CArmVTEnum
{
    VT_Sharpen,   //!< 锐化
    VT_Denoise,   //!< 降噪
    VT_MetalCalib,//!< 金属校正
    VT_FlipH,     //!< 水平翻转
    VT_FlipV,     //!< 竖直翻转
    VT_Bright,    //!< 明亮度
    VT_Contrast,  //!< 对比度
    VT_Negative,  //!< 负片
    VT_Rotate     //!< 旋转
};



/**
* @brief 移动C型臂图像锐化工具枚举
*/
enum CArmSharpenEnum
{
    IT_Sharpen_0,   //!< 锐化1
    IT_Sharpen_1,   //!< 锐化2
    IT_Sharpen_2    //!< 锐化3
};


/**
* @brief 移动C型臂图像降噪工具枚举
*/
enum CArmDenoiseEnum
{
    IT_Denoise_0,   //!< 降噪1
    IT_Denoise_1,   //!< 降噪2
    IT_Denoise_2    //!< 降噪3
};


/**
* @brief 移动C型臂图像金属校正工具枚举
*/
enum CArmMetalCalibEnum
{
    IT_MetalCalib_0,        //!< 金属校正0
    IT_MetalCalib_1,        //!< 金属校正1
    IT_MetalCalib_2,        //!< 金属校正2
    IT_MetalCalib_3,        //!< 金属校正3
    IT_MetalCalibAuto       //!< 自动金属校正
};



/**
* @brief 锐化参数对象数据结构
*/
struct CArmSharpenVTObject
{
    float _value;
    CArmSharpenEnum _mode;
};

/**
* @brief 降噪参数对象数据结构
*/
struct CArmDenoiseVTObject
{
    float _value;
    CArmDenoiseEnum _mode;
};

/**
* @brief 金属校正参数对象数据结构
*/
struct CArmMetalCalibVTObject
{
    float _value;
    CArmMetalCalibEnum _mode;
};



struct CArmImgParam
{
    bool _negative;     //!< 是否负片
    bool _flipH;        //!< 是否水平翻转
    bool _flipV;        //!< 是否竖直翻转
    float _bright;      //!< 明亮度
    float _contrast;    //!< 对比度
    float _rotate;      //!< 旋转角度
    CArmSharpenVTObject _sharpen;   //!< 锐化参数
    CArmDenoiseVTObject _denoise;   //!< 降噪参数
    CArmMetalCalibVTObject _mtcalib;//!< 金属校正参数

};



/**
* @brief 阅片信息数据结构
*/
struct CP_ReviewData
{
    CP_ImageData _imageData;        //!< 图像信息
    CArmImgParam _originParam;      //!< 原始图像参数
    CArmImgParam _reviewParam;      //!< 显示图像参数
    CArmMarkObject _markObject;     //!< 标记对象
    TextureObject3DPtr _texture;    //!< 图像纹理
};



/**
* @brief 移动C型臂通用功能函数
*/
class CArmCommon
{
public:
    CArmCommon();
    ~CArmCommon();


    /**
    * @brief 调用该函数，从图像信息中获取指定实例的NewScanData数据结构
    * @param info 图像信息数据结构
    * @return 返回NewScanData
    */
    static NewScanData GetNewScanData(CP_ImageData info);

    /**
    * @brief 调用该函数，从NewScanData数据结构中获取阅片数据
    * @param lst NewScanData数据结构
    * @return 阅片数据
    */
    static CP_ReviewData* GetReviewData(NewScanData lst);

    /**
    * @brief 调用该函数，根据图像信息创建图像纹理数据结构
    * @param info 图像信息数据结构
    * @return 返回纹理
    */
    static TextureObject3DPtr CreateTextureObject(CP_ImageData info);

    /**
    * @brief 调用该函数，获取instancInfo
    * @param imageInfo 当前阅片图像信息
    * @return 返回存放instance信息链表
    */
    static CP_InstanceData GetInstanceInfo(CP_ImageData imageInfo);

    /**
    * @brief 调用该函数，将时间转化为字符
    * @param time 时间（单位：ms）
    * @return 时间字符
    */
    static std::string GetTimeString(int time);

    /**
    * @brief 调用该函数，拼接姓名
    * @param fstName 名
    * @param lstName 姓
    * @return 姓名
    */
    static std::string SpliceFstAndLstName(std::string fstName, std::string lstName);

    /**
    * @brief 调用该函数，获取图像调节参数
    * @param image 图像信息
    * @return 图像调节参数
    */
    static CArmImgParam GetImageParam(const CP_ImageData& image);

    /**
    * @brief 调用该函数，将数值归一化到指定范围
    * @param value 数值
    * @param Srcmin 数值的最小取值
    * @param value 数值的最大取值
    * @param value 归一化区间的最小值
    * @param value 归一化区间的最大值
    * @return 归一化后的数值
    */
    template<typename T, typename S>
    static S Nomalize(T value, T Srcmin, T Srcmax, S Desmin, S Desmax);

};

