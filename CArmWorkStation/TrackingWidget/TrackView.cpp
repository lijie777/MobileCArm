#include "TrackView.h"
#include "CreateImageBrush.h"


TrackView::TrackView(int type, QWidget* p)
    : CArmBaseView(type, p)
    , m_pAnnoTool(CArmAnnotation::createTool())
{
    _imageBrush = ShaderToolApi::createImageBrush(IImageBrush::IMAGE_TYPE::IT_IMAGE2D);
    setMouseTracking(true);
}


TrackView::~TrackView()
{
    delete _imageBrush;
}

void TrackView::loadReviewData(CP_ReviewData* data)
{
    m_reviewData = data;

    //m_pImgVT->updateBright(data._reviewParam._bright);
    //m_pImgVT->updateContrast(data._reviewParam._contrast);
    //m_pImgVT->setNegative(data._reviewParam._negative);
    //m_pImgVT->enableFlipH(data._reviewParam._flipH);
    //m_pImgVT->enableFlipV(data._reviewParam._flipV);
    //m_pImgVT->setNegative(data._reviewParam._negative);
    //m_pImgVT->setRotateParam(data._reviewParam._rotate);

    //m_pImgVT->setSharpenParam(data._reviewParam._sharpen);
    //m_pImgVT->setDenoiseParam(data._reviewParam._denoise);
    //m_pImgVT->setMetalCalibParam(data._reviewParam._mtcalib);

    _imageBrush->loadTexture(data->_texture);

    resetProjectMatrix();

    updateGL();
}


void TrackView::draw(glm::mat4 p, glm::mat4 mv, int window_width, int window_height)
{
    m_pAnnoTool.setImageData(m_reviewData->_imageData, m_pImgVT->getImageParam());
    m_pAnnoTool.getObject().display(window_width, window_height);
}

void TrackView::glInitBase()
{
    if (_imageBrush != nullptr)
    {
        _imageBrush->initBrush();
    }

    m_pAnnoTool.initTool();
}



void TrackView::deleteCTTexture()
{
    if (_imageBrush == nullptr)
    {
        return;
    }
}

void TrackView::paintGL()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _imageBrush->RenderImage(_projectMatrix, _modelViewMatrix);

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

void TrackView::resizeGL(int width, int height)
{
    _window_width = width;
    _window_height = height;
    _window_hwratio = (float)_window_width / _window_height;
    glViewport(0, 0, width, height);

    resetProjectMatrix();
}

void TrackView::resetProjectMatrix()
{
    TextureObject3DPtr texture = _imageBrush->getTextureObject();
    if (texture == nullptr)
    {
        return;
    }

    glm::mat4 projectTransform = _projectMatrix * glm::inverse(m_normalProjectMatrix);


    glm::vec3* imageCoord = texture._Object->_image_vertices;
    glm::vec3 modelImageCoord[4];
    for (int i = 0;i < 4;i++)
    {
        modelImageCoord[i] = glm::vec3(_modelViewMatrix * glm::vec4(imageCoord[i], 1));
    }

    float imageHeight = abs(modelImageCoord[_NEAR_TOPRIGHT].y - modelImageCoord[_NEAR_BOTTONLEFT].y) / 2;
    float imageWidth = abs(modelImageCoord[_NEAR_TOPRIGHT].x - modelImageCoord[_NEAR_BOTTONLEFT].x) / 2;

    if (imageWidth > imageHeight)
    {
        //图像宽高比较窗口宽高比小
        float a = -1.0f;
        float b = 1.0f;
        float c = -1.0 / _window_hwratio;
        float d = 1.0f / _window_hwratio;
        m_normalProjectMatrix = glm::ortho(a, b, c, d);
        _projectMatrix = projectTransform * m_normalProjectMatrix;
    }
    else
    {
        float a = -1.0f *_window_hwratio;
        float b = 1.0f * _window_hwratio;
        float c = -1.0;
        float d = 1.0f;
        m_normalProjectMatrix = glm::ortho(a, b, c, d);
        _projectMatrix = projectTransform * m_normalProjectMatrix;
    }

}

