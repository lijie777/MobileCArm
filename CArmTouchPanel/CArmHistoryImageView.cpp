#include "CArmHistoryImageView.h"
#include "CreateImageBrush.h"
#include "ViewToolCreator.h"
CArmHistoryImageView::CArmHistoryImageView(int type, QWidget *parent)
    : CArmBaseView(type, parent)
{
    if (_imageBrush == nullptr)
    {
        _imageBrush = ShaderToolApi::createImageBrush(IImageBrush::IMAGE_TYPE::IT_IMAGE2D);
    }
}

CArmHistoryImageView::~CArmHistoryImageView()
{

}

void CArmHistoryImageView::loadReviewData(CP_ReviewData* imageData)
{
    m_ReviewData = imageData;

    m_pImgVT->updateBright(imageData->_reviewParam._bright);
    m_pImgVT->updateContrast(imageData->_reviewParam._contrast);
    m_pImgVT->setNegative(imageData->_reviewParam._negative);
    m_pImgVT->enableFlipH(imageData->_reviewParam._flipH);
    m_pImgVT->enableFlipV(imageData->_reviewParam._flipV);
    m_pImgVT->setRotateParam(imageData->_reviewParam._rotate);
    m_pImgVT->setSharpenParam(imageData->_reviewParam._sharpen);
    m_pImgVT->setDenoiseParam(imageData->_reviewParam._denoise);
    m_pImgVT->setMetalCalibParam(imageData->_reviewParam._mtcalib);

    if (_imageBrush != nullptr)
    {

        _imageBrush->loadTexture(imageData->_texture);

        resetProjectMatrix();

    }
    updateGL();
}

CP_ReviewData CArmHistoryImageView::getReviewData()
{
    /*修改过的参数*/
    m_ReviewData->_reviewParam = m_pImgVT->getImageParam();
    

    return *m_ReviewData;
}

void CArmHistoryImageView::resizeGL(int width, int height)
{
    _window_width = width;
    _window_height = height;
    _window_hwratio = (float)_window_width / _window_height;
    glViewport(0, 0, width, height);

    resetProjectMatrix();
}

void CArmHistoryImageView::draw(glm::mat4 p, glm::mat4, int w_widget, int w_height)
{

}

void CArmHistoryImageView::glInitBase()
{
    if (_imageBrush != nullptr)
    {
        _imageBrush->initBrush();
    }
}

void CArmHistoryImageView::paintGL()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (_imageBrush != nullptr)
    {
        _imageBrush->RenderImage(_projectMatrix, _modelViewMatrix);
    }
}

void CArmHistoryImageView::resetProjectMatrix()
{
    TextureObject3DPtr texture = _imageBrush->getTextureObject();
    if (texture == nullptr)
    {
        return;
    }

    glm::mat4 projectTransform = _projectMatrix * glm::inverse(m_normalProjectMatrix);


    glm::vec3* imageCoord = texture._Object->_image_vertices;
    glm::vec3 modelImageCoord[4];
    for (int i = 0; i < 4; i++)
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