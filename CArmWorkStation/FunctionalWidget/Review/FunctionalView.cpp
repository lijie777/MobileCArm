#include "FunctionalView.h"
#include "CreateImageBrush.h"
#include "ToolCreator.h"



FunctionalView::FunctionalView(CArmWorkStationProc* proc, int type, QWidget* p)
    : CArmBaseView(type, p)
    , m_reviewOP(new ReviewOperationTool(this))
    , m_pAnnoTool(CArmAnnotation::createTool())
    , m_DirectL(new DirectTextTool(CArmDTEnum::DirectL, L"L"))
    , m_DirectA(new DirectTextTool(CArmDTEnum::DirectA, L"A"))
    , m_DirectH(new DirectTextTool(CArmDTEnum::DirectH, L"H"))
    , m_DirectR(new DirectTextTool(CArmDTEnum::DirectR, L"R"))
    , m_DirectP(new DirectTextTool(CArmDTEnum::DirectP, L"P"))
    , m_DirectF(new DirectTextTool(CArmDTEnum::DirectF, L"F"))
    , m_CommentDT(new CommentTextDT())
    , m_ArrowDT(new ArrowDT())
    , m_LineDT(new LineDT())
    , m_AngleDT(new AngleDT())
    , m_CalibDT(new CalibDT(m_LineDT, m_AngleDT, proc))
    , m_bMarkShowMode(true)
    , m_reviewData(nullptr)
{
    _imageBrush = ShaderToolApi::createImageBrush(IImageBrush::IMAGE_TYPE::IT_IMAGE2D);
    setMouseTracking(true);

    initReviewOP(); //初始化阅片操作工具
}


FunctionalView::~FunctionalView()
{
    delete _imageBrush;

}

void FunctionalView::recover()
{
    if (m_reviewData == nullptr) return;
    CArmImgParam param = m_reviewData->_originParam;
    m_pImgVT->setSharpenParam(param._sharpen);
    m_pImgVT->setDenoiseParam(param._denoise);
    m_pImgVT->setMetalCalibParam(param._mtcalib);
}

void FunctionalView::loadReviewData(CP_ReviewData* data)
{
    m_reviewData = data;

    m_CalibDT->loadImage(data->_imageData, data->_texture);

    m_pImgVT->updateBright(data->_reviewParam._bright);
    m_pImgVT->updateContrast(data->_reviewParam._contrast);
    m_pImgVT->setNegative(data->_reviewParam._negative);
    m_pImgVT->enableFlipH(data->_reviewParam._flipH);
    m_pImgVT->enableFlipV(data->_reviewParam._flipV);
    m_pImgVT->setNegative(data->_reviewParam._negative);
    m_pImgVT->setRotateParam(data->_reviewParam._rotate);

    m_pImgVT->setSharpenParam(data->_reviewParam._sharpen);
    m_pImgVT->setDenoiseParam(data->_reviewParam._denoise);
    m_pImgVT->setMetalCalibParam(data->_reviewParam._mtcalib);

    _imageBrush->loadTexture(data->_texture);
}




IOperationToolPtr FunctionalView::getReviewOP()
{
    return m_reviewOP._Object;
}

std::shared_ptr<DirectTextTool> FunctionalView::getDirectLDT()
{
    return m_DirectL;
}

std::shared_ptr<DirectTextTool> FunctionalView::getDirectADT()
{
    return m_DirectA;
}

std::shared_ptr<DirectTextTool> FunctionalView::getDirectHDT()
{
    return m_DirectH;
}

std::shared_ptr<DirectTextTool> FunctionalView::getDirectRDT()
{
    return m_DirectR;
}

std::shared_ptr<DirectTextTool> FunctionalView::getDirectPDT()
{
    return m_DirectP;
}

std::shared_ptr<DirectTextTool> FunctionalView::getDirectFDT()
{
    return m_DirectF;
}

std::shared_ptr<CommentTextDT> FunctionalView::getCommentDT()
{
    return m_CommentDT;
}

std::shared_ptr<ArrowDT> FunctionalView::getArrowDT()
{
    return m_ArrowDT;
}

std::shared_ptr<LineDT> FunctionalView::getLineDT()
{
    return m_LineDT;
}

std::shared_ptr<AngleDT> FunctionalView::getAngleDT()
{
    return m_AngleDT;
}

std::shared_ptr<CalibDT> FunctionalView::getCalibDT()
{
    return m_CalibDT;
}

void FunctionalView::gobeVisibleOrHided()
{
    m_bMarkShowMode = !m_bMarkShowMode;
    m_DirectL->setVisible(m_bMarkShowMode);
    m_DirectA->setVisible(m_bMarkShowMode);
    m_DirectH->setVisible(m_bMarkShowMode);
    m_DirectR->setVisible(m_bMarkShowMode);
    m_DirectP->setVisible(m_bMarkShowMode);
    m_DirectF->setVisible(m_bMarkShowMode);
    m_ArrowDT->setVisible(m_bMarkShowMode);
    m_LineDT->setVisible(m_bMarkShowMode);
    m_AngleDT->setVisible(m_bMarkShowMode);
    m_CommentDT->setVisible(m_bMarkShowMode);
}


void FunctionalView::draw(glm::mat4 p, glm::mat4 mv, int window_width, int window_height)
{


}

void FunctionalView::glInitBase()
{
    if (_imageBrush != nullptr)
    {
        _imageBrush->initBrush();
    }

    m_pAnnoTool.initTool();
    m_DirectL->initTool();
    m_DirectA->initTool();
    m_DirectH->initTool();
    m_DirectR->initTool();
    m_DirectP->initTool();
    m_DirectF->initTool();
    m_CommentDT->initTool();
    m_ArrowDT->initTool();
    m_LineDT->initTool();
    m_AngleDT->initTool();
}



void FunctionalView::notifyTextureLoaded()
{
    if (_imageBrush == nullptr)
    {
        return;
    }

}

void FunctionalView::deleteCTTexture()
{
    if (_imageBrush == nullptr)
    {
        return;
    }

}

void FunctionalView::paintGL()
{
    //glClearColor(0, 0, 0, 0);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //// 造假数据
    //_CArmPatientID ptID = "ptID";
    //_CArmStudyUID studyUID = { ptID ,"studyUID" };
    //_CArmSeriesUID seriesUID = { studyUID , "seriesUID" };
    //_CArmImageUID imageUID = { seriesUID, "imageUID" };
    //CP_ImageInfo image = { imageUID, 1,1,1,1,1,0.1f,0.1,55,1,1,true, nullptr };
    //CP_SeriesInfo series = { seriesUID , "seriesNumber", "seriesDate", "seriesTime","bodyPart","dose", "descp", "type", "pulse", true, ImageParam{30,30,30,30,30,true,true,true,true,true} };
    //CP_StudyInfo study = { studyUID , "studyID", "acsNumber", "studyDate","studyTime", "modality","descp", "doctor" };
    //CP_PatientInfo patient = { ptID , "ptName", "age", "birth", "sex" };

    //series._imageInfo[imageUID] = image;
    //study._seriesInfo[seriesUID] = series;
    //patient._studyInfo[studyUID] = study;

    //CP_InstanceInfo m_InstanceInfo;
    //m_InstanceInfo._imageUID = imageUID;
    //m_InstanceInfo._seriesUID = seriesUID;
    //m_InstanceInfo._studyUID = studyUID;
    //m_InstanceInfo._patientID = ptID;

    //m_pAnnoTool.setPatientInfo(&patient);
    //m_pAnnoTool.setCurrentInstance(&m_InstanceInfo);
    //m_pAnnoTool.getObject().display(_window_width, _window_height);
}

void FunctionalView::initReviewOP()
{
    if (m_reviewOP == nullptr) return;

    m_reviewOP._Object->addDrawTool(m_DirectL.get());
    m_reviewOP._Object->addDrawTool(m_DirectA.get());
    m_reviewOP._Object->addDrawTool(m_DirectH.get());
    m_reviewOP._Object->addDrawTool(m_DirectR.get());
    m_reviewOP._Object->addDrawTool(m_DirectP.get());
    m_reviewOP._Object->addDrawTool(m_DirectF.get());
    m_reviewOP._Object->addDrawTool(m_CommentDT.get());
    m_reviewOP._Object->addDrawTool(m_ArrowDT.get());
    m_reviewOP._Object->addDrawTool(m_LineDT.get());
    m_reviewOP._Object->addDrawTool(m_AngleDT.get());

}

