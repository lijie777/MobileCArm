#include "CArmView.h"
#include "CreateImageBrush.h"
#include "ViewToolCreator.h"

CArmView::CArmView(int type, QWidget *parent)
    : CArmBaseView(type, parent)
    , m_pAnnoTool(CArmAnnotation::createTool())
    , m_bIsTwoPoint(false)
    , m_currentStepScaleFactor(1)
    , m_scaleFactor(1)
    , m_bIsLoadImageData(false)
    , m_ImageDataInfo(nullptr)
{
    this->setMouseTracking(true);
    _imageBrush = ShaderToolApi::createImageBrush(IImageBrush::IMAGE_TYPE::IT_IMAGE2D);
    setAnnotationTool(m_pAnnoTool.getObject());
    
    /*设置接收触屏事件*/
    setAttribute(Qt::WA_AcceptTouchEvents);
}

CArmView::~CArmView()
{
    if (_imageBrush != nullptr)
    {
        delete _imageBrush;
        _imageBrush = nullptr;
    }
}

void CArmView::resizeGL(int width, int height)
{
    _window_width = width;
    _window_height = height;
    _window_hwratio = (float)_window_width / _window_height;
    glViewport(0, 0, width, height);
    
    resetProjectMatrix();
}

void CArmView::draw(glm::mat4 p, glm::mat4 mv, int window_width, int window_height)
{
    if (m_ImageDataInfo == nullptr) return;
    m_pAnnoTool.setImageData(m_ImageDataInfo->_imageData, m_pImgVT->getImageParam());
    m_pAnnoTool.getObject().display(window_width, window_height);
}

void CArmView::glInitBase()
{
    if (_imageBrush != nullptr)
    {
        _imageBrush->initBrush();
    }
    m_pAnnoTool.initTool();
}

void CArmView::notifyTextureLoaded()
{
    //resetProjectMatrix();
    ////计算矩形顶点
    //glm::vec3 guideWindowCoord[4];
    //glm::vec3 projectCoord[4];
    //projectCoord[_NEAR_BOTTONLEFT] = glm::vec3(-1.0, -1.0, 0.0);
    //projectCoord[_NEAR_BOTTONRIGHT] = glm::vec3(1.0, -1.0, 0.0);
    //projectCoord[_NEAR_TOPRIGHT] = glm::vec3(1.0, 1.0, 0.0);
    //projectCoord[_NEAR_TOPLEFT] = glm::vec3(-1.0, 1.0, 0.0);


    //for (int i = 0; i < 4; i++)
    //{
    //    guideWindowCoord[i] = glm::vec3(glm::inverse(_modelViewMatrix) * glm::inverse(_projectMatrix) * glm::vec4(projectCoord[i], 1.0));
    //}
}

void CArmView::resetProjectMatrix()
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

void CArmView::flipViewHorizontal(bool status)
{
    IViewingToolPtr viewTool = getViewingTool(IViewingTool::VIEWTOOL_TYPE::VT_FLIPH);
    if (viewTool != nullptr)
    {
        viewTool.setValue(status, _FLIP_VALUE);
      //  viewTool.disposeViewer(this);
    }
    updateGL();
}

void CArmView::flipViewVertical(bool status)
{
    IViewingToolPtr viewTool = getViewingTool(IViewingTool::VIEWTOOL_TYPE::VT_FLIPV);
    if (viewTool != nullptr)
    {
        viewTool.setValue(status, _FLIP_VALUE);
    //    viewTool.disposeViewer(this);
    }
    updateGL();
}

void CArmView::deleteCTTexture()
{
    if (_imageBrush == nullptr)
    {
        return;
    }
}

void CArmView::paintGL()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    if (m_bIsLoadImageData == true)
    {
        if (_imageBrush != nullptr)
        {
            _imageBrush->RenderImage(_projectMatrix, _modelViewMatrix);
        }
        if (m_ImageDataInfo == nullptr) return;
        m_pAnnoTool.setImageData(m_ImageDataInfo->_imageData, m_ImageDataInfo->_originParam);
        m_pAnnoTool.getObject().display(_window_width, _window_height);
    }
 
}

bool CArmView::event(QEvent *event)
{
    glm::vec3 point1;
    glm::vec3 point2;

    switch (event->type())
    {
    case QEvent::TouchBegin:
    case QEvent::TouchUpdate:
    case QEvent::TouchEnd:
    {
       

        QTouchEvent *touchEvent = static_cast<QTouchEvent *>(event);
        /*获取触屏屏幕上的点*/
        QList<QTouchEvent::TouchPoint> touchPoints = touchEvent->touchPoints();
        if (touchPoints.count() == 2)
        {
            m_bIsTwoPoint = true;//两指时不让移动
            const QTouchEvent::TouchPoint &touchPoint0 = touchPoints.first();
            const QTouchEvent::TouchPoint &touchPoint1 = touchPoints.last();

            point1.x = touchPoint0.pos().x();
            point1.y = touchPoint0.pos().y();
            point1.z = 0;
            point1 = glm::unProject(point1, _modelViewMatrix, _projectMatrix, glm::ivec4(0, 0, _window_width, _window_height));

            point2.x = touchPoint1.pos().x();
            point2.y = touchPoint1.pos().y();
            point2.z = 0;
            point2 = glm::unProject(point2, _modelViewMatrix, _projectMatrix, glm::ivec4(0, 0, _window_width, _window_height));
            /*开始点和结束点的比例*/
            qreal currentScaleFactor = QLineF(touchPoint0.pos(), touchPoint1.pos()).length() /
                QLineF(touchPoint0.startPos(), touchPoint1.startPos()).length();

            /*更新矩阵坐标*/
            _projectMatrix = glm::scale(_projectMatrix, glm::vec3(currentScaleFactor, currentScaleFactor, 1));

            if (touchEvent->touchPointStates() & Qt::TouchPointReleased) 
            {
                m_scaleFactor *= currentScaleFactor;
            }
            m_currentStepScaleFactor = currentScaleFactor;
            
            updateGL();
        }
        else if (touchPoints.count() == 1)
        {
            m_bIsTwoPoint = false;
        }
        return true;
    }
      
    default:
        return QOpenGLWidget::event(event);
    }
  
    
}

void CArmView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bIsTwoPoint == false)
    {
        QPointF point = event->pos() - m_lastMousePos;
        translate(point);
    }

}

void CArmView::mousePressEvent(QMouseEvent *event)
{
    if (m_bIsTwoPoint == false)
    {
        m_lastMousePos = event->pos();
    }
    QWidget::mousePressEvent(event);
}

void CArmView::translate(QPointF detal)
{
    m_horizontalOffset += detal.x();

    m_verticalOffset += detal.y();

    updateGL();
}

void CArmView::loadReviewData(CP_ReviewData* imageData)
{
    m_ImageDataInfo = imageData;

    //m_pImgVT->updateBright(imageData->_reviewParam._bright);
    //m_pImgVT->updateContrast(imageData->_reviewParam._contrast);
    //m_pImgVT->setNegative(imageData->_reviewParam._negative);
    //m_pImgVT->enableFlipH(imageData->_reviewParam._flipH);
    //m_pImgVT->enableFlipV(imageData->_reviewParam._flipV);
    //m_pImgVT->setRotateParam(imageData->_reviewParam._rotate);
    //m_pImgVT->setSharpenParam(imageData->_reviewParam._sharpen);
    //m_pImgVT->setDenoiseParam(imageData->_reviewParam._denoise);
    //m_pImgVT->setMetalCalibParam(imageData->_reviewParam._mtcalib);

    if (_imageBrush != nullptr)
    {

        _imageBrush->loadTexture(imageData->_texture);

        resetProjectMatrix();

        m_bIsLoadImageData = true;
       
    }
    updateGL();
}

void CArmView::updatePatient()
{
    m_bIsLoadImageData = false;



}

void CArmView::setContrast(int value)
{
    float fv = value / 50.0;
    if (_imageBrush != nullptr)
    {
        _imageBrush->adjustContrast(fv);
    }
    updateGL();
}

void CArmView::setBrightness(int value)
{
    float fv = value / 50.0;
    if (_imageBrush != nullptr)
    {
        _imageBrush->adjustSharpness(fv);
    }
    updateGL();
}

void CArmView::setSharpen(int gear, int value)
{
    CArmSharpenVTObject VTObj;
    VTObj._mode = (CArmSharpenEnum)(gear - 1);
    VTObj._value = value / 50.0;
    m_pImgVT->setSharpenParam(VTObj);

    this->updateGL();
}

void CArmView::setDenoise(int gear, int value)
{

    CArmDenoiseVTObject VTObj;
    VTObj._mode = (CArmDenoiseEnum)(gear - 1);
    VTObj._value = value / 50.0;
    m_pImgVT->setDenoiseParam(VTObj);
    
    this->updateGL();
}

void CArmView::setMetalGear(int gearValue, bool state)
{
    //金属校正
    CArmMetalCalibVTObject metalObj;
    metalObj._value = 1.0;
    metalObj._mode = CArmMetalCalibEnum(gearValue);
    m_pImgVT->setMetalCalibParam(metalObj);

    updateGL();
}

void CArmView::capacityMetal(int gearValue, bool state)
{
    CArmMetalCalibVTObject metalObj;
    metalObj._value = 1.0;
    metalObj._mode = CArmMetalCalibEnum(gearValue);
    m_pImgVT->setMetalCalibParam(metalObj);
    updateGL();
}

void CArmView::setRotateValue(int rotateValue)
{
    IViewingToolPtr viewTool = getViewingTool(IViewingTool::VIEWTOOL_TYPE::VT_ROTATE);
    if (viewTool != nullptr)
    {
        viewTool.adjustValue(rotateValue, _ROTATE_VALUE);
        viewTool.disposeViewer(this);
    }
    updateGL();
}

void CArmView::setNegativeValue(bool state)
{
    IViewingToolPtr viewTool = getViewingTool(IViewingTool::VIEWTOOL_TYPE::VT_REVERSE);
    if (viewTool != nullptr)
    {
        viewTool.setValue(state, _REVERSE_VALUE);
        viewTool.disposeViewer(this);
    }
}

