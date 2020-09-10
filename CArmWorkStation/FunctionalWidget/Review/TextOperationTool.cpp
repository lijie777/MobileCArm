#include "TextOperationTool.h"

#pragma execution_character_set("utf-8")



TextOperationTool::TextOperationTool(std::shared_ptr<CommentTextDT> drawTool, BaseView* viewer, QAbstractButton* btn, IOperationToolPtr nxtOP)
    : IOperationTool(viewer)
    , m_pDrawTool(drawTool)
    , m_pCtrlButton(btn)
    , m_nxtOP(nxtOP)
{
}


TextOperationTool::~TextOperationTool()
{
}




// 结束操作
void TextOperationTool::finishOperation()
{
    if (m_pCtrlButton != nullptr)
    {
        m_pCtrlButton->setChecked(false);
        m_pCtrlButton = nullptr;
    }

    if (m_pDrawTool != nullptr)
    {
        m_pDrawTool = nullptr;
    }

    if (m_pViewer != nullptr)
    {
        m_pViewer->updateGL();
        m_pViewer = nullptr;
    }
}

// 开始操作
void TextOperationTool::startOperation()
{
    m_pDrawTool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxCreateStatus(m_pDrawTool, m_pViewer, this)));
}

void TextOperationTool::rightMousePressed(glm::vec3 point, QMouseEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    TextDataCallback callback;
    callback.setView(m_pViewer);
    callback.setOperationTool(this);
    m_pDrawTool->rightPress(point, ev, &callback, DrawCallback);

    updateStatus();
}

void TextOperationTool::leftMousePressed(glm::vec3 point, QMouseEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    TextDataCallback callback;
    callback.setView(m_pViewer);
    callback.setOperationTool(this);
    m_pDrawTool->leftPress(point, ev, &callback, DrawCallback);
    updateStatus();
}

void TextOperationTool::rightMouseDoublePressed(glm::vec3 point, QMouseEvent * ev)
{
}

void TextOperationTool::leftMouseDoublePressed(glm::vec3 point, QMouseEvent * ev)
{
}

void TextOperationTool::rightMouseMoving(glm::vec3 point, QMouseEvent * ev)
{
}

void TextOperationTool::leftMouseMoving(glm::vec3 point, QMouseEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    TextDataCallback callback;
    callback.setView(m_pViewer);
    callback.setOperationTool(this);
    m_pDrawTool->leftMove(point, ev, &callback, DrawCallback);

    m_pViewer->updateGL();
}

void TextOperationTool::MouseMoving(glm::vec3 point, QMouseEvent * ev)
{

}

void TextOperationTool::rightMouseRelease(glm::vec3 point, QMouseEvent * ev)
{
}

void TextOperationTool::leftMouseRelease(glm::vec3 point, QMouseEvent * ev)
{
}

void TextOperationTool::wheelRolling(int d, glm::vec3 point, QWheelEvent * ev)
{
}

void TextOperationTool::keyPressed(QKeyEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    if (ev->key() == Qt::Key_Delete)
    {
        m_pDrawTool->deletePress();
    }

    m_pViewer->updateGL();
}

// 鼠标移出窗口时调用此函数
void TextOperationTool::mouseLeaveView(QEvent* ev)
{

}

// 更新操作状态
void TextOperationTool::updateStatus()
{

    if (m_pViewer != nullptr)
    {
        m_pViewer->updateGL();
    }
}

// 判断点是否在矩形区域内
bool TextOperationTool::pointInTextureRect(glm::vec3 point)
{
    if (m_pViewer == nullptr)
    {
        return false;
    }

    //判断点是否在图像上
    IImageBrush* imageBrush = m_pViewer->getImageBrush();
    if (imageBrush == nullptr)
    {
        return false;
    }

    TextureObject3DPtr texture = imageBrush->getTextureObject();

    if (texture == nullptr)
    {
        return false;
    }

    glm::vec3* corner = texture._Object->_image_vertices;
    if (corner == nullptr)
    {
        return false;
    }

    float a = glm::cross(corner[0] - point, corner[1] - point).z;
    float b = glm::cross(corner[1] - point, corner[2] - point).z;
    float c = glm::cross(corner[2] - point, corner[3] - point).z;
    float d = glm::cross(corner[3] - point, corner[0] - point).z;
    if ((a < 0 && b < 0 && c < 0 && d < 0) || (a > 0 && b > 0 && c > 0 && d > 0))
    {
        return true;
    }

    return false;

}













/*************************************************************
* class   TextDataCallback   文本工具回调接口
**************************************************************/
TextDataCallback::TextDataCallback()
    : mBaseView(nullptr)
    , m_pOperationTool(nullptr)
{
}

TextDataCallback::~TextDataCallback()
{
}


void TextDataCallback::setView(BaseView* pBaseView)
{
    mBaseView = pBaseView;
}

void TextDataCallback::drawCallback(MeasureObjectPtr drawData, int status)
{
    if (mBaseView == nullptr || mBaseView->getImageBrush() == nullptr)
    {
        return;
    }

    TextureObject3DPtr texture = mBaseView->getImageBrush()->getTextureObject();

    if (texture == nullptr)
    {
        return;
    }

    if (drawData._Object != nullptr)
    {
        if (drawData._Object->_type != MEASUREOBJECT_TYPE::MT_TEXT)
        {
            return;
        }



        //获取计数尺寸
        int imageIdxWidth = texture._Object->width;
        int imageIdxHeight = texture._Object->height;
        int imageIdxDepth = texture._Object->depth;

        //获取物理尺寸
        float phyWidth = texture._Object->width * texture._Object->spacingInWidth;
        float phyHeight = texture._Object->height * texture._Object->spacingInHeight;
        float phyDepth = texture._Object->depth * texture._Object->spacingInDepth;


        glm::vec3* windowVertices = texture._Object->_image_vertices;
        glm::vec3* textureCoord = texture._Object->_texture_coordinate;

        if (windowVertices == nullptr || textureCoord == nullptr)
        {
            return;
        }

        //获取视口尺寸
        float windowWidth = (windowVertices[_NEAR_BOTTONLEFT].x - windowVertices[_NEAR_BOTTONRIGHT].x);
        float windowHeight = (windowVertices[_NEAR_BOTTONLEFT].y - windowVertices[_NEAR_TOPLEFT].y);
        float windowDepth = (windowVertices[_NEAR_BOTTONLEFT].z - windowVertices[_FAR_BOTTONLEFT].z);


        if (drawData._Object->_pPoints == nullptr ||
            drawData._Object->_pIndexPoints == nullptr ||
            drawData._Object->_pTextureCoordPoints == nullptr)
        {
            return;
        }


        //坐标变换，计算计数点坐标
        glm::vec3 linePoint0 = drawData._Object->_pPoints[0];
        glm::vec3 refPoint = windowVertices[_NEAR_BOTTONLEFT];
        glm::vec3 pointRefDiff0 = linePoint0 - refPoint;


        glm::vec3 lineTextureCoord0;


        glm::vec3 normalH = textureCoord[_NEAR_BOTTONRIGHT] - textureCoord[_NEAR_BOTTONLEFT];
        glm::vec3 normalV = textureCoord[_NEAR_TOPLEFT] - textureCoord[_NEAR_BOTTONLEFT];

        lineTextureCoord0 = textureCoord[_NEAR_BOTTONLEFT] +
            ((pointRefDiff0.x) / windowWidth) * normalH +
            ((pointRefDiff0.y) / windowHeight) * normalV;


        glm::vec3 lineIdxPoint0;
        glm::vec3 lineIdxPoint1;

        lineIdxPoint0 =
            glm::vec3(lineTextureCoord0.x * imageIdxWidth,
                lineTextureCoord0.y * imageIdxHeight,
                lineTextureCoord0.z * imageIdxDepth);



        //记录索引点坐标
        drawData._Object->_pIndexPoints[0] = lineIdxPoint0;


        //记录点的纹理坐标
        drawData._Object->_pTextureCoordPoints[0] = lineTextureCoord0;

    }



}


void TextDataCallback::setOperationTool(TextOperationTool * tool)
{
    m_pOperationTool = tool;
}


