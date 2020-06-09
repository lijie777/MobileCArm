#include "CArmAnnotation.h"
#include "CArmCommon.h"

#include <comutil.h>

#pragma comment(lib,"comsuppw.lib")

#define _ANNOLINE_SPACING 1.5       //!< 四角信息行间距
#define _ANNOMARGIN_X     20        //!< 四角信息水平留白余量
#define _ANNOMARGIN_Y     20        //!< 四角信息竖直留白余量
#define _ANNOTEXTSIZE     16        //!< 四角信息字号
#define _ANNOPICDIR       "Pic"     //!< 四角插图文件夹

CArmAnnotation::CArmAnnotation()
    : m_pTextBrush(new TextBrush())
    , m_PictureBrush(new PictureBrush())
    , m_brightPic(nullptr)
    , m_contrastPic(nullptr)
    , m_sharpenPic(nullptr)
    , m_rotatePic(nullptr)
    , m_metalPic(nullptr)
    , m_denoisePic(nullptr)
    , m_reversePic(nullptr)
    , m_fliphPic(nullptr)
    , m_flipvPic(nullptr)
{
}


CArmAnnotation::~CArmAnnotation()
{
    delete m_pTextBrush;

    if (m_brightPic != nullptr) delete m_brightPic;
    if (m_contrastPic != nullptr) delete m_contrastPic;
    if (m_sharpenPic != nullptr) delete m_sharpenPic;
    if (m_rotatePic != nullptr) delete m_rotatePic;
    if (m_metalPic != nullptr) delete m_metalPic;
    if (m_denoisePic != nullptr) delete m_denoisePic;
    if (m_reversePic != nullptr) delete m_reversePic;
    if (m_fliphPic != nullptr) delete m_fliphPic;
    if (m_flipvPic != nullptr) delete m_flipvPic;
}

CArmAnnotationPtr CArmAnnotation::createTool()
{
    return new CArmAnnotation();
}

void CArmAnnotation::initTool()
{
    m_pTextBrush->init_resources();
    m_pTextBrush->set_color(0.463f, 0.525f, 0.6f, 1.0f);

    m_PictureBrush->initBrush();

    std::string path = UCommonUtility::GetExeDir() + "\\" + _ANNOPICDIR + "\\";
    m_brightPic = new AnnotationPictureData(m_PictureBrush, path + "Bricon.png", 910, 100, true);
    m_contrastPic = new AnnotationPictureData(m_PictureBrush, path + "Contrasticon.png", 952, 100, true);
    m_sharpenPic = new AnnotationPictureData(m_PictureBrush, path + "Shicon.png", 910, 71, true);
    m_rotatePic = new AnnotationPictureData(m_PictureBrush, path + "", 952, 71, true);
    m_metalPic = new AnnotationPictureData(m_PictureBrush, path + "Mticon.png", 910, 41, true);
    m_denoisePic = new AnnotationPictureData(m_PictureBrush, path + "Densicon.png", 952, 41, true);
    m_reversePic = new AnnotationPictureData(m_PictureBrush, path + "", 600, 400, true);
    m_fliphPic = new AnnotationPictureData(m_PictureBrush, path + "", 700, 400, true);
    m_flipvPic = new AnnotationPictureData(m_PictureBrush, path + "", 800, 400, true);

}

void CArmAnnotation::setImageData(const CP_ImageData& data, const CArmImgParam& param)
{
    m_imageInfo = data;
    m_imgParam = param;
}


void CArmAnnotation::display(int window_width, int window_height)
{
    if (m_bIsVisible == false) return;

    // 窗口水平/竖直像素大小
    float sx = 1.0 / window_width * 2.0;
    float sy = 1.0 / window_height * 2.0;

    // 行高
    float h = _ANNOTEXTSIZE * _ANNOLINE_SPACING * sy;

    // 窗口边界留白量
    float xoffset = _ANNOMARGIN_X * sx;
    float yoffset = _ANNOMARGIN_Y * sy;

    CP_PatientData patientInfo = m_imageInfo._sereisInfo._studyInfo._patientInfo;
    CP_StudyData studyInfo = m_imageInfo._sereisInfo._studyInfo;
    CP_SeriesData seriesInfo = m_imageInfo._sereisInfo;
    CP_ImageData imageInfo = m_imageInfo;

    // 左上角
    std::wstring name = std::wstring(_bstr_t(patientInfo._patientFirstName.c_str())) + L"  " + std::wstring(_bstr_t(patientInfo._patientLastName.c_str()));
    AnnotationTextData ptName = AnnotationTextData(m_pTextBrush, name, _ANNOTEXTSIZE, - 1.0 + xoffset, 1.0 - yoffset, true);
    AnnotationTextData ptSex = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(patientInfo._patientSex.c_str())), _ANNOTEXTSIZE, -1.0 + xoffset, 1.0 - yoffset - h, true);
    AnnotationTextData ptBirth = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(patientInfo._ptBirthDate.c_str())), _ANNOTEXTSIZE, -1.0 + xoffset, 1.0 - yoffset - 2 * h, true);
    AnnotationTextData ptID = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(patientInfo._patientID.c_str())), _ANNOTEXTSIZE, -1.0 + xoffset, 1.0 - yoffset - 3 * h, true);
    ptName.display(window_width, window_height);
    ptSex.display(window_width, window_height);
    ptBirth.display(window_width, window_height);
    ptID.display(window_width, window_height);

    // 右上角
    AnnotationTextData dateTM = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(studyInfo._studyDate.c_str())) + L"  " + std::wstring(_bstr_t(studyInfo._studyTime.c_str())), _ANNOTEXTSIZE, 1.0 - xoffset, 1.0 - yoffset, true, 1);
    AnnotationTextData type = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(seriesInfo._type.c_str())), _ANNOTEXTSIZE, 1.0 - xoffset, 1.0 - yoffset - h, true, 1);
    AnnotationTextData imageID = AnnotationTextData(m_pTextBrush, std::to_wstring(imageInfo._imageIdx), _ANNOTEXTSIZE, 1.0 - xoffset, 1.0 - yoffset- 2 * h, true, 1);
    AnnotationTextData descp = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(studyInfo._description.c_str())), _ANNOTEXTSIZE, 1.0 - xoffset, 1.0 - yoffset - 3 * h, true, 1);
    AnnotationTextData doct = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(studyInfo._doctor.c_str())), _ANNOTEXTSIZE, 1.0 - xoffset, 1.0 - yoffset - 4 * h, true, 1);
    AnnotationTextData acesNum = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(studyInfo._accessNumber.c_str())), _ANNOTEXTSIZE, 1.0 - xoffset, 1.0 - yoffset - 5 * h, true, 1);
    dateTM.display(window_width, window_height);
    type.display(window_width, window_height);
    imageID.display(window_width, window_height);
    descp.display(window_width, window_height);
    doct.display(window_width, window_height);
    acesNum.display(window_width, window_height);
    
    // 左下角
    AnnotationTextData expTMDose = AnnotationTextData(m_pTextBrush, std::wstring(_bstr_t(CArmCommon::GetTimeString(imageInfo._exposureTime).c_str()))+ L"   " + std::wstring(_bstr_t(seriesInfo._dose.c_str())), _ANNOTEXTSIZE, -1.0 + xoffset , -1.0 + yoffset, true);
    AnnotationTextData maPulse = AnnotationTextData(m_pTextBrush, std::to_wstring(imageInfo._XrayCurrent) + L"mA   " + std::wstring(_bstr_t(seriesInfo._pulse.c_str())), _ANNOTEXTSIZE, -1.0 + xoffset, -1.0 + yoffset + h, true);
    AnnotationTextData kvpBody = AnnotationTextData(m_pTextBrush, std::to_wstring((int)imageInfo._KVP) + L"kVp   " + std::wstring(_bstr_t(seriesInfo._bodypart.c_str())), _ANNOTEXTSIZE, -1.0 + xoffset, -1.0 + yoffset + 2 * h, true);
    kvpBody.display(window_width, window_height);
    maPulse.display(window_width, window_height);
    expTMDose.display(window_width, window_height);

    // 处理信息
    //AnnotationTextData flipH = AnnotationTextData(m_pTextBrush, L"", _ANNOTEXTSIZE, -1.0, -1.0, true);
    //AnnotationTextData reverse = AnnotationTextData(m_pTextBrush, L"", _ANNOTEXTSIZE, -1.0, -1.0, true);
    //AnnotationTextData flipV = AnnotationTextData(m_pTextBrush, L"", _ANNOTEXTSIZE, -1.0, -1.0, true);

    float h1 = 1.2 * h;
    AnnotationTextData denoise = AnnotationTextData(m_pTextBrush, std::to_wstring((int)seriesInfo._imageParam.m_denoise), _ANNOTEXTSIZE, 1.0 - xoffset, -1.0 + yoffset + h1, true, 1);
    AnnotationTextData metal = AnnotationTextData(m_pTextBrush, std::to_wstring((int)seriesInfo._imageParam.m_metalCalib), _ANNOTEXTSIZE, 1.0 - xoffset - 5 * h, -1.0 + yoffset + h1, true, 1);
    AnnotationTextData rotate = AnnotationTextData(m_pTextBrush, std::to_wstring((int)seriesInfo._imageParam.m_rotate), _ANNOTEXTSIZE, -1.0 - xoffset, -1.0 + yoffset + 2 * h1, true, 1);
    AnnotationTextData sharpen = AnnotationTextData(m_pTextBrush, std::to_wstring((int)seriesInfo._imageParam.m_sharpen), _ANNOTEXTSIZE, 1.0 - xoffset - 5 * h, -1.0 + yoffset + 2 * h1, true, 1);
    AnnotationTextData contrast = AnnotationTextData(m_pTextBrush, std::to_wstring((int)seriesInfo._imageParam.m_contrast), _ANNOTEXTSIZE, 1.0 - xoffset, -1.0 + yoffset + 3 * h1, true, 1);
    AnnotationTextData bright = AnnotationTextData(m_pTextBrush, std::to_wstring((int)seriesInfo._imageParam.m_brightness), _ANNOTEXTSIZE, 1.0 - xoffset - 5 * h, -1.0 + yoffset + 3 * h1, true, 1);

    bright.display(window_width, window_height);
    contrast.display(window_width, window_height);
    sharpen.display(window_width, window_height);
    rotate.display(window_width, window_height);
    metal.display(window_width, window_height);
    //reverse.display(window_width, window_height);
    //flipH.display(window_width, window_height);
    //flipV.display(window_width, window_height);

    // 图片
    if(m_brightPic!=nullptr)  m_brightPic->display(window_width, window_height);
    if (m_contrastPic != nullptr) m_contrastPic->display(window_width, window_height);
    if (m_sharpenPic != nullptr) m_sharpenPic->display(window_width, window_height);
    if (m_rotatePic != nullptr)  m_rotatePic->display(window_width, window_height);
    if (m_metalPic != nullptr) m_metalPic->display(window_width, window_height);
    if (m_denoisePic != nullptr)  m_denoisePic->display(window_width, window_height);
    if (m_reversePic != nullptr)  m_reversePic->display(window_width, window_height);
    if (m_fliphPic != nullptr)  m_fliphPic->display(window_width, window_height);
    if (m_flipvPic != nullptr)  m_flipvPic->display(window_width, window_height);

}


void CArmAnnotation::updateAnno()
{


}






void CArmAnnotationPtr::initTool()
{
    if (_Object != nullptr)
    {
        _Object->initTool();
    }
}

void CArmAnnotationPtr::setImageData(const CP_ImageData& data, const CArmImgParam& param)
{
    if (_Object != nullptr)
    {
        _Object->setImageData(data, param);
    }
}


CArmAnnotationPtr::CArmAnnotationPtr(CArmAnnotation * obj)
    : _Object(obj)
{
    if (obj != nullptr)
    {
        obj->_refCount++;
    }
    _Object = obj;
}

CArmAnnotationPtr::CArmAnnotationPtr(const CArmAnnotationPtr & ptr)
{
    if (ptr._Object != nullptr)
    {
        ptr._Object->_refCount++;
    }
    _Object = ptr._Object;
}

CArmAnnotationPtr::~CArmAnnotationPtr()
{
    if (_Object != nullptr)
    {
        if (--_Object->_refCount == 0)
        {
            delete _Object;
            _Object = nullptr;
        }
    }
}


CArmAnnotationPtr & CArmAnnotationPtr::operator=(const CArmAnnotationPtr & ptr)
{
    //TODO: 在此处插入 return 语句
    if (ptr._Object != nullptr)
    {
        ptr._Object->_refCount++;
    }
    if (_Object != nullptr)
    {
        if (--_Object->_refCount == 0)
        {
            delete _Object;
            _Object = nullptr;
        }
    }
    this->_Object = ptr._Object;
    return *this;
}

bool CArmAnnotationPtr::operator!=(const CArmAnnotationPtr& ptr)
{
    // TODO: 在此处插入 return 语句
    return (this->_Object != ptr._Object);
}

bool CArmAnnotationPtr::operator==(const CArmAnnotationPtr& ptr)
{
    return (this->_Object == ptr._Object);
}

