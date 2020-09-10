#include "CArmPrinter.h"

CArmPrintData::CArmPrintData(const CP_ReviewData* data)
{
    CP_ImageData image = data->_imageData;
    CP_SeriesData series = image._sereisInfo;
    CP_StudyData study = series._studyInfo;
    CP_PatientData patient = study._patientInfo;
    CArmImgParam orgParam = data->_originParam;
    CArmImgParam currParam = data->_reviewParam;
    CArmMarkObject mark = data->_markObject;

    // 填充标识信息
    InsertID(CArmReviewIdent::PatientID, patient._patientID);
    InsertID(CArmReviewIdent::StudyUID, study._studyUID);
    InsertID(CArmReviewIdent::SeriesUID, series._seriesUID);
    InsertID(CArmReviewIdent::SOPIntanceUID, image._imageUID);

    // 填充图像处理参数
    InsertImgParam(CArmImgParamEnum::Origin, orgParam);
    InsertImgParam(CArmImgParamEnum::Review, currParam);

    // 填充标记信息
    InsertMarker(mark);
}