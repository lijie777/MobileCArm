#include "CArmCommon.h"


#define _60                 60         //<! 时间单位换算
#define _MILLI              0.001     //<! 千分之一

CArmCommon::CArmCommon()
{
}


CArmCommon::~CArmCommon()
{
}




NewScanData CArmCommon::GetNewScanData(CP_ImageData info)
{
    NewScanData result;

    // 填充患者信息
    CP_PatientData pt = info._sereisInfo._studyInfo._patientInfo;
    strcpy(result.m_PatientId, pt._patientID.c_str());
    strcpy(result.m_PatientFirstName, pt._patientFirstName.c_str());
    strcpy(result.m_PatientLastName, pt._patientLastName.c_str());
    strcpy(result.m_PatientBirthDate, pt._ptBirthDate.c_str());
    strcpy(result.m_PatientSex, pt._patientSex.c_str());

    // 填充检查信息
    CP_StudyData st = info._sereisInfo._studyInfo;
    strcpy(result.m_StudyInstanceUid, st._studyUID.c_str());
    strcpy(result.m_StudyID, st._studyID.c_str());
    strcpy(result.m_AccessionNumber, st._accessNumber.c_str());
    strcpy(result.m_StudyDate, st._studyDate.c_str());
    strcpy(result.m_StudyTime, st._studyTime.c_str());
    strcpy(result.m_Modality, st._modality.c_str());
    strcpy(result.m_StudyDesc, st._description.c_str());
    strcpy(result.m_PerformingPhysicianName, st._doctor.c_str());

    // 填充拍摄信息
    strcpy(result.m_SeriesInstanceUid, info._sereisInfo._seriesUID.c_str());
    strcpy(result.m_SeriesNumber, info._sereisInfo._seriesNumber.c_str());
    strcpy(result.m_SeriesDate, info._sereisInfo._seriesDate.c_str());
    strcpy(result.m_SeriesTime, info._sereisInfo._seriesTime.c_str());
    strcpy(result.m_BodyPartExamined, info._sereisInfo._bodypart.c_str());
    strcpy(result.m_SeriesDesc, info._sereisInfo._description.c_str());

    // 填充影像信息
    strcpy(result.m_SopInstanceUid, info._imageUID.c_str());
    result.m_ImageColmn = info._imageWidth;
    result.m_ImageRow = info._imageHeight;
    result.m_WindowCenter = info._windowCenter;
    result.m_WindowWidth = info._windowWidth;
    result.m_PixelSpacingX = info._pixelSpacingX;
    result.m_PixelSpacingY = info._pixelSpacingY;
    result.m_kvp = info._KVP;
    result.m_XrayTubeCurrent = info._XrayCurrent;
    result.m_ExposureTime = info._exposureTime;
    result.m_PixelRepresentation = info._pixelIssigned;
    result.m_pixel = (short*)info._imageData;

    return result;
}

CP_ReviewData* CArmCommon::GetReviewData(NewScanData lst)
{
    CP_PatientData ptData;
    CP_StudyData stdData;
    CP_SeriesData serData;
    CP_ImageData imgData;
    CArmImgParam orgParm;
    CArmImgParam currParm;

    // 填充患者信息
    ptData = CP_PatientData{
        lst.m_PatientId,
        SpliceFstAndLstName(lst.m_PatientFirstName, lst.m_PatientLastName) ,
        lst.m_PatientFirstName,
        lst.m_PatientLastName,
        "0",
        lst.m_PatientBirthDate,
        lst.m_PatientSex,
        false };

    // 填充检查信息
    stdData = CP_StudyData{
        lst.m_StudyInstanceUid,
        lst.m_StudyID,
        lst.m_AccessionNumber,
        lst.m_StudyDate,
        lst.m_StudyTime,
        lst.m_Modality,
        lst.m_StudyDesc,
        lst.m_PerformingPhysicianName,
        "",
        ptData };
    
    // 填充拍摄信息
    serData = CP_SeriesData{
        lst.m_SeriesInstanceUid,
        lst.m_SeriesDir,
        lst.m_SeriesNumber,
        lst.m_SeriesDate,
        lst.m_SeriesTime,
        lst.m_BodyPartExamined,
        "",
        "",
        "",
        "",
        false,
        ImageParam(),
        stdData };

    // 填充图像信息
    imgData = CP_ImageData{
        lst.m_SopInstanceUid,
        lst.strDicomDir,
        lst.strOperatingPath,
        lst.m_InstanceNumber,
        lst.m_ImageColmn,
        lst.m_ImageRow,
        (int)lst.m_WindowCenter,
        (int)lst.m_WindowWidth,
        lst.m_PixelSpacingX,
        lst.m_PixelSpacingY,
        lst.m_kvp,
        (float)lst.m_XrayTubeCurrent,
        lst.m_ExposureTime,
        lst.m_BitsAllocated,
        lst.m_BitsStored,
        lst.m_HighBit,
        (lst.m_PixelRepresentation == 1 ? true : false),
        (unsigned short*)lst.m_pixel,
        serData };

    // 读取操作记录文件
    CArmMarkObject mark;

    //填充阅片数据
    return new CP_ReviewData{ imgData,orgParm,currParm, mark, nullptr };

}

TextureObject3DPtr CArmCommon::CreateTextureObject(CP_ImageData info)
{
    TextureObject3DPtr texture = TextureObject3D::createObject();

    // 对象赋值
    texture._Object->pData = (short*)info._imageData;
    texture._Object->pSourceData = (short*)info._imageData;
    texture._Object->height = info._imageHeight;
    texture._Object->width = info._imageWidth;
    texture._Object->depth = 1;
    texture._Object->spacingInHeight = info._pixelSpacingX;
    texture._Object->spacingInWidth = info._pixelSpacingY;
    texture._Object->spacingInDepth = 0;

    texture._Object->_pDataIsUnsigned = !info._pixelIssigned;
    texture._Object->bitsAllocated = info._bitsAllocated;
    texture._Object->windowCenter = info._windowCenter;
    texture._Object->windowWidth = info._windowWidth;

    int grayCenter = info._windowCenter;
    int grayWidth = info._windowWidth / 2;
    unsigned short sign = info._pixelIssigned ? 1 : 0;
    texture._Object->mingray = (grayCenter - grayWidth) / (float)(1 << info._bitsStored);
    texture._Object->maxgray = (grayCenter + grayWidth) / (float)(1 << info._bitsStored);

    return texture;
}

CP_InstanceData CArmCommon::GetInstanceInfo(CP_ImageData imageInfo)
{
    CP_InstanceData instance;

    instance.imageID = imageInfo._imageUID;
    instance.seriesID = imageInfo._sereisInfo._seriesUID;
    instance.studyID = imageInfo._sereisInfo._studyInfo._studyUID;
    instance.patientID = imageInfo._sereisInfo._studyInfo._patientInfo._patientID;

    return instance;
}


std::string CArmCommon::GetTimeString(int time)
{
    int timeInSec = time * _MILLI;
    std::string min = std::to_string(timeInSec / _60);
    std::string sec = std::to_string(timeInSec % _60);

    if (min.size() < 2) min = "0" + min;
    if (sec.size() < 2) sec = "0" + sec;

    return min + ":" + sec;
}


std::string CArmCommon::SpliceFstAndLstName(std::string fstName, std::string lstName)
{
    return fstName + " " + lstName;
}

CArmImgParam CArmCommon::GetImageParam(const CP_ImageData & image)
{
    //CArmSharpenVTObject{ (float)image._sereisInfo._imageParam.m_sharpen, CArmSharpenEnum::IT_Sharpen_0 };
    //CArmDenoiseVTObject{ (float)image._sereisInfo._imageParam.m_denoise, CArmDenoiseEnum::IT_Denoise_0 };
    //CArmMetalCalibVTObject{ (float)image._sereisInfo._imageParam.m_metalCalib, CArmMetalCalibEnum::IT_MetalCalib_0 };


    CArmImgParam param
    {
        image._sereisInfo._imageParam.m_negative,   //负片参数
        image._sereisInfo._imageParam.m_reversalH,  //水平翻转参数
        image._sereisInfo._imageParam.m_reversalV,  //竖直翻转参数
        image._sereisInfo._imageParam.m_brightness, //明亮度
        image._sereisInfo._imageParam.m_contrast,   //对比度
        image._sereisInfo._imageParam.m_rotate,     //旋转角度
        CArmSharpenVTObject{ (float)image._sereisInfo._imageParam.m_sharpen, CArmSharpenEnum::IT_Sharpen_0 },   //锐化参数
        CArmDenoiseVTObject{ (float)image._sereisInfo._imageParam.m_denoise, CArmDenoiseEnum::IT_Denoise_0 },   //降噪参数
        CArmMetalCalibVTObject{ (float)image._sereisInfo._imageParam.m_metalCalib, CArmMetalCalibEnum::IT_MetalCalib_0 }//金属校正参数
    };

    return param;
}

template<typename T, typename S>
S CArmCommon::Nomalize(T value, T Srcmin, T Srcmax, S Desmin, S Desmax)
{
    return S(1) * value / (Srcmax - Srcmin) * (Desmax - Desmin);
}
