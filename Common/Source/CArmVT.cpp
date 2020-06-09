#include "CArmVT.h"


CArmVT::CArmVT(int tp)
    : IViewingTool(tp)
{
}


CArmVT::~CArmVT()
{
}

void CArmImgVT::setSharpenParam(CArmSharpenVTObject value)
{
    m_param._sharpen = value;
    processImage();
}

void CArmImgVT::setDenoiseParam(CArmDenoiseVTObject value)
{
    m_param._denoise = value;
    processImage();
}

void CArmImgVT::setMetalCalibParam(CArmMetalCalibVTObject value)
{
    m_param._mtcalib = value;
    processImage();
}

void CArmImgVT::updateBright(int value)
{
    m_param._bright = value;
    CArmBrightVT bright{ (float)value };
    bright.disposeViewer(m_pViewer);
}

void CArmImgVT::updateContrast(int value)
{
    m_param._contrast = value;
    CArmContrastVT contrst{ (float)value };
    contrst.disposeViewer(m_pViewer);
}

void CArmImgVT::enableFlipH(bool en)
{
    m_param._flipH = en;
    CArmFlipHVT flip{ en };
    flip.disposeViewer(m_pViewer);
}

void CArmImgVT::enableFlipV(bool en)
{
    m_param._flipV = en;
    CArmFlipVVT flip{ en };
    flip.disposeViewer(m_pViewer);
}

void CArmImgVT::setNegative(bool en)
{
    m_param._negative = en;
    CArmNegativeVT ng{ en };
    ng.disposeViewer(m_pViewer);
}

void CArmImgVT::setRotateParam(int value)
{
    m_param._rotate = value;
    CArmRotateVT rt{ value };
    rt.disposeViewer(m_pViewer);
}

CArmImgParam CArmImgVT::getImageParam()
{
    return m_param;
}

void CArmImgVT::processImage()
{
    if (m_pViewer == nullptr) return;

    CArmSharpenVT sharpen{ m_param._sharpen };
    CArmDenoiseVT denoise{ m_param._denoise };
    CArmMetalCalibVT mtcalib{ m_param._mtcalib };
    sharpen.disposeViewer(m_pViewer);
    sharpen.disposeViewer(m_pViewer);
    sharpen.disposeViewer(m_pViewer);
}




void CArmSharpenVT::disposeViewer(BaseView * view)
{
    TextureObject3DPtr texture = view->getImageBrush()->getTextureObject();

    if (texture == nullptr)
    {
        return;
    }

    float value = m_param._value;
    if (CArmSharpenEnum::IT_Sharpen_0 == m_param._mode)
    {
        view->getImageBrush()->adjustSharpness(value);
    }
    else if (CArmSharpenEnum::IT_Sharpen_1 == m_param._mode)
    {
        view->getImageBrush()->adjustSharpness(1.0);
        // 调用算法库
    }
    else if (CArmSharpenEnum::IT_Sharpen_2 == m_param._mode)
    {
        view->getImageBrush()->adjustSharpness(1.0);
        // 调用算法库
    }

    // 刷新图像
}



void CArmDenoiseVT::disposeViewer(BaseView * view)
{
    if (view == nullptr || view->getImageBrush() == nullptr)
    {
        return;
    }

    TextureObject3DPtr texture = view->getImageBrush()->getTextureObject();

    if (texture == nullptr)
    {
        return;
    }

    float value = m_param._value;
    if (CArmDenoiseEnum::IT_Denoise_0 == m_param._mode)
    {
        // 调用算法库
    }
    else if (CArmDenoiseEnum::IT_Denoise_1 == m_param._mode)
    {
        // 调用算法库
    }
    else if (CArmDenoiseEnum::IT_Denoise_2 == m_param._mode)
    {
        // 调用算法库
    }

    //刷新图像
}


void CArmMetalCalibVT::disposeViewer(BaseView * view)
{
    if (view == nullptr || view->getImageBrush() == nullptr)
    {
        return;
    }

    TextureObject3DPtr texture = view->getImageBrush()->getTextureObject();

    if (texture == nullptr)
    {
        return;
    }

    float value = m_param._value;
    if (CArmMetalCalibEnum::IT_MetalCalib_0 == m_param._mode)
    {
        // 调用算法库
    }
    else if (CArmMetalCalibEnum::IT_MetalCalib_1 == m_param._mode)
    {
        // 调用算法库
    }
    else if (CArmMetalCalibEnum::IT_MetalCalib_2 == m_param._mode)
    {
        // 调用算法库
    }
    else if (CArmMetalCalibEnum::IT_MetalCalib_3 == m_param._mode)
    {
        // 调用算法库
    }

    //刷新图像
}

void CArmFlipHVT::disposeViewer(BaseView * view)
{
    if(view!=nullptr)    view->flipViewHorizontal(m_param);
}

void CArmFlipVVT::disposeViewer(BaseView * view)
{
    if (view != nullptr)    view->flipViewVertical(m_param);
}

void CArmBrightVT::disposeViewer(BaseView * view)
{
    if (view != nullptr || view->getImageBrush() != nullptr)
    {
        view->getImageBrush()->adjustBrightness(m_param);
        view->updateGL();
    }
}

void CArmContrastVT::disposeViewer(BaseView * view)
{
    if (view != nullptr || view->getImageBrush() != nullptr)
    {
        view->getImageBrush()->adjustContrast(m_param);
        view->updateGL();
    }
}

void CArmNegativeVT::disposeViewer(BaseView * view)
{
    if (view != nullptr || view->getImageBrush() != nullptr)
    {
        view->getImageBrush()->reverseBright(m_param);
        view->updateGL();
    }
}

void CArmRotateVT::disposeViewer(BaseView * view)
{
    if (view == nullptr || view->getImageBrush() == nullptr)
    {
        return;
    }

    TextureObject3DPtr texture = view->getImageBrush()->getTextureObject();

    if (texture == nullptr)
    {
        return;
    }

    glm::mat4 modelMatric = view->getModelViewMatrix();

    glm::vec3* coord = texture._Object->_texture_coordinate;
    glm::vec3* windowCoord = texture._Object->_image_vertices;
    glm::vec3 normalH = windowCoord[_NEAR_BOTTONRIGHT] - windowCoord[_NEAR_BOTTONLEFT];
    glm::vec3 normalV = windowCoord[_NEAR_TOPLEFT] - windowCoord[_NEAR_BOTTONLEFT];
    glm::vec3 normalPlane = glm::cross(normalH, normalV);

    if (coord == nullptr || windowCoord == nullptr)
    {
        return;
    }

    modelMatric = glm::rotate(glm::mat4(1), (float)m_param, normalPlane);
    view->setModelViewMatrix(modelMatric * view->getModelViewMatrix());
    view->updateGL();
}
