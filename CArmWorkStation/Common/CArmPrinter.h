#pragma once

#include <map>
#include "Config.h"
#include "CArmVT.h"
#include "CArmDT.h"

class CArmReviewPrinter;

/**
* @brief 阅片校验数据结构
*/
enum class CArmReviewIdent : int
{
    PatientID,      //!< 患者ID
    StudyUID,       //!< 检查标识号
    SeriesUID,
    SOPIntanceUID
};

/**
* @brief 图像参数类型
*/
enum class CArmImgParamEnum : int
{
    Origin,
    Review
};


struct CArmMarkPrint
{
    inline void InsertDirectL(const CArmDirectDTObject& data)
    {
        _directMark[CArmDTEnum::DirectL].first = "RirectL";
        _directMark[CArmDTEnum::DirectL].second = data;
    }

    inline void InsertDirectR(const CArmDirectDTObject& data)
    {
        _directMark[CArmDTEnum::DirectR].first = "RirectR";
        _directMark[CArmDTEnum::DirectR].second = data;
    }

    inline void InsertDirectA(const CArmDirectDTObject& data)
    {
        _directMark[CArmDTEnum::DirectA].first = "RirectA";
        _directMark[CArmDTEnum::DirectA].second = data;
    }

    inline void InsertDirectP(const CArmDirectDTObject& data)
    {
        _directMark[CArmDTEnum::DirectP].first = "RirectL";
        _directMark[CArmDTEnum::DirectP].second = data;
    }

    inline void InsertDirectH(const CArmDirectDTObject& data)
    {
        _directMark[CArmDTEnum::DirectH].first = "RirectH";
        _directMark[CArmDTEnum::DirectH].second = data;
    }

    inline void InsertDirectF(const CArmDirectDTObject& data)
    {
        _directMark[CArmDTEnum::DirectF].first = "RirectF";
        _directMark[CArmDTEnum::DirectF].second = data;
    }

    //inline void InsertCalibPatam(float x, float y)
    //{
    //    _imgCalib.first = "ImgCalib";
    //    _imgCalib.second = std::make_pair(x, y);
    //}

    inline void InsertArrow(std::list<CArmArrowDTObject> data)
    {
        _arrowMark.first = "Arrow";
        _arrowMark.second = data;
    }

    inline void InsertComment(std::list<CArmCommentDTObject> data)
    {
        _commentMark.first = "Commnet";
        _commentMark.second = data;
    }

    inline void InsertLine(std::list<CArmLineDTObject> data)
    {
        _lineMark.first = "Line";
        _lineMark.second = data;
    }

    inline void InsertAngle(std::list<CArmAngleDTObject> data)
    {
        _angleMark.first = "Angle";
        _angleMark.second = data;
    }



    std::map<CArmDTEnum, std::pair<std::string, CArmDirectDTObject>> _directMark;
    //std::pair<std::string, std::pair<float, float>> _imgCalib;
    std::pair<std::string, std::list<CArmArrowDTObject>> _arrowMark;
    std::pair<std::string, std::list<CArmCommentDTObject>> _commentMark;
    std::pair<std::string, std::list<CArmLineDTObject>> _lineMark;
    std::pair<std::string, std::list<CArmAngleDTObject>> _angleMark;

    //friend class cereal::access;
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(cereal::make_nvp("Direct", _directMark),
            cereal::make_nvp("Arrow", _arrowMark),
            cereal::make_nvp("Comment", _commentMark),
            cereal::make_nvp("Line", _lineMark),
            cereal::make_nvp("Angle", _angleMark));
    }

};


class CArmPrintData
{
public:

    CArmPrintData(const CP_ReviewData* data);
    CArmPrintData() {};

    inline void InsertID(CArmReviewIdent id, std::string data)
    {
        if (id == CArmReviewIdent::PatientID)
        {
            _id[CArmReviewIdent::PatientID].first = "PatientID";
            _id[CArmReviewIdent::PatientID].second = data;
        }
        else if (id == CArmReviewIdent::StudyUID)
        {
            _id[CArmReviewIdent::StudyUID].first = "StudyUID";
            _id[CArmReviewIdent::StudyUID].second = data;
        }
        else if (id == CArmReviewIdent::SeriesUID)
        {
            _id[CArmReviewIdent::SeriesUID].first = "SeriesUID";
            _id[CArmReviewIdent::SeriesUID].second = data;
        }
        else if (id == CArmReviewIdent::SOPIntanceUID)
        {
            _id[CArmReviewIdent::SOPIntanceUID].first = "SOPIntanceUID";
            _id[CArmReviewIdent::SOPIntanceUID].second = data;
        }
    }

    inline void InsertImgParam(CArmImgParamEnum pm, CArmImgParam data)
    {
        if (pm == CArmImgParamEnum::Origin)
        {
            _imageParam[CArmImgParamEnum::Origin].first = "Origin";
            _imageParam[CArmImgParamEnum::Origin].second = data;
        }
        else
        {
            _imageParam[CArmImgParamEnum::Review].first = "Review";
            _imageParam[CArmImgParamEnum::Review].second = data;
        }
    }

    inline void InsertMarker(const CArmMarkObject& mark)
    {
        CArmMarkPrint out;
        out.InsertDirectL(mark._directL);
        out.InsertDirectR(mark._directR);
        out.InsertDirectA(mark._directA);
        out.InsertDirectF(mark._directF);
        out.InsertDirectH(mark._directH);
        out.InsertDirectP(mark._directP);
        out.InsertArrow(mark._arrow);
        out.InsertComment(mark._comment);
        out.InsertLine(mark._line);
        out.InsertAngle(mark._angle);
        _marker = out;
    }

    inline bool GetImageParam(CArmImgParamEnum em, CArmImgParam& result)
    {
        auto iter = _imageParam.find(em);
        if (iter == _imageParam.end()) return false;

        result = iter->second.second;
        return true;
    }





    std::map<CArmReviewIdent, std::pair<std::string, std::string>> _id;
    std::map<CArmImgParamEnum, std::pair< std::string, CArmImgParam>> _imageParam;
    CArmMarkPrint _marker;

    //friend class cereal::access;
    template<class Archive>
    void serialize(Archive & archive)
    {
        archive(cereal::make_nvp("ID", _id),
            cereal::make_nvp("ImageParam", _imageParam),
            cereal::make_nvp("Marker", _marker));
    }

};




class CArmReviewPrinter
{
public:

    static CArmPrintData Deserialize(std::string file)
    {
        //std::ifstream is(file);
        //cereal::XMLInputArchive iarchive(is);
        CArmPrintData review;
        //iarchive(review);
        return review;
    }
    
    static void Serialize(const CArmPrintData& data, std::string filename)
    {
        std::ofstream os(filename);
        cereal::XMLOutputArchive oarchive(os);
        oarchive(cereal::make_nvp("CArmReview", data));
    }
};

template <class Archive>
void serialize(Archive & archive, const CArmDirectDTObject& obj)
{
    archive(cereal::make_nvp("Pos", obj._points));
}

template <class Archive>
void serialize(Archive & archive, const CArmArrowDTObject& obj)
{
    archive(cereal::make_nvp("Pos", obj._points));
}

template <class Archive>
void serialize(Archive & archive, const CArmCommentDTObject& obj)
{
    archive(cereal::make_nvp("Pos", obj._point),
        cereal::make_nvp("Comment", obj._text));
}

template <class Archive>
void serialize(Archive & archive, const CArmLineDTObject& obj)
{
    archive(cereal::make_nvp("Pos", obj._point),
        cereal::make_nvp("LabelPos", obj._mmPos));
}

template <class Archive>
void serialize(Archive & archive, const CArmAngleDTObject& obj)
{
    archive(cereal::make_nvp("Pos", obj._point),
        cereal::make_nvp("LabelPos", obj._anglePos));
}

template <class Archive>
void serialize(Archive & archive, const CArmImgParam& obj)
{
    archive(cereal::make_nvp("Contrast", obj._contrast),
        cereal::make_nvp("Brightness", obj._bright),
        cereal::make_nvp("Sharpen", obj._sharpen),
        cereal::make_nvp("Denoise", obj._denoise),
        cereal::make_nvp("MetalCalib", obj._mtcalib),
        cereal::make_nvp("Negative", obj._negative),
        cereal::make_nvp("Roatate", obj._rotate),
        cereal::make_nvp("FlipH", obj._flipH),
        cereal::make_nvp("FlipV", obj._flipV));
}

template <class Archive>
void serialize(Archive & archive, const CArmSharpenVTObject& obj)
{
    archive(cereal::make_nvp("Value", obj._value),
        cereal::make_nvp("Mode", obj._mode));
}

template <class Archive>
void serialize(Archive & archive, const CArmDenoiseVTObject& obj)
{
    archive(cereal::make_nvp("Value", obj._value),
        cereal::make_nvp("Mode", obj._mode));
}

template <class Archive>
void serialize(Archive & archive, const CArmMetalCalibVTObject& obj)
{
    archive(cereal::make_nvp("Value", obj._value),
        cereal::make_nvp("Mode", obj._mode));
}