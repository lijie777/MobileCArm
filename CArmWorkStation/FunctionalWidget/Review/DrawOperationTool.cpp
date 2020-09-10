#include "DrawOperationTool.h"


#pragma execution_character_set("utf-8")





/*************************************************************
* class   DrawOperationTool   绘图操作工具
**************************************************************/
DrawOperationTool::DrawOperationTool(CArmDrawToolPtr drawTool, BaseView* viewer,QAbstractButton* btn)
    : IOperationTool(viewer)
    , m_pDrawTool(drawTool)
    , m_pCtrlButton(btn)
{
}



DrawOperationTool::~DrawOperationTool()
{
}



// 结束操作
void DrawOperationTool::finishOperation()
{
    if (m_pCtrlButton != nullptr)
    {
        m_pCtrlButton->setChecked(false);
        m_pCtrlButton = nullptr;
    }

    if (m_pDrawTool != nullptr)
    {
        m_pViewer->updateGL();
        m_pDrawTool = nullptr;
    }


    if (m_pViewer != nullptr)
    {
        m_pViewer->updateGL();
        m_pViewer = nullptr;
    }

}

// 开始操作
void DrawOperationTool::startOperation()
{
    m_pDrawTool.setStatus(_CArmDrawToolIdle);
}



void DrawOperationTool::rightMousePressed(glm::vec3 point, QMouseEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    m_pDrawTool.rightPress(point, ev);

}

void DrawOperationTool::leftMousePressed(glm::vec3 point, QMouseEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    if (pointInWindowRect(point) == false)
    {
        return;
    }

    m_pDrawTool.leftPress(point, ev);

}

void DrawOperationTool::rightMouseDoublePressed(glm::vec3 point, QMouseEvent * ev)
{
}

void DrawOperationTool::leftMouseDoublePressed(glm::vec3 point, QMouseEvent * ev)
{
}

void DrawOperationTool::rightMouseMoving(glm::vec3 point, QMouseEvent * ev)
{
    m_pDrawTool.move(point, ev);
}

void DrawOperationTool::leftMouseMoving(glm::vec3 point, QMouseEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    if (pointInWindowRect(point) == false)
    {
        return;
    }

    m_pDrawTool.leftMove(point, ev);


    m_pViewer->updateGL();
}

void DrawOperationTool::MouseMoving(glm::vec3 point, QMouseEvent * ev)
{
    if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }

    m_pDrawTool.move(point, ev);



    m_pViewer->updateGL();
}

void DrawOperationTool::rightMouseRelease(glm::vec3 point, QMouseEvent * ev)
{
}

void DrawOperationTool::leftMouseRelease(glm::vec3 point, QMouseEvent * ev)
{
}

void DrawOperationTool::wheelRolling(int d, glm::vec3 point, QWheelEvent * ev)
{
    if (1 == d)
    {
        if (nullptr != m_pViewer->getImageBrush())
        {
            m_pViewer->getImageBrush()->gotoPreviousSlice();
        }
    }
    if (-1 == d)
    {
        if (nullptr != m_pViewer->getImageBrush())
        {
            m_pViewer->getImageBrush()->gotoNextSlice();
        }
    }
}

void DrawOperationTool::keyPressed(QKeyEvent * ev)
{
   if (m_pDrawTool == nullptr || m_pViewer == nullptr)
    {
        return;
    }
    
    if (ev->key() == Qt::Key_Delete)
    {

        updateStatus();
    }

    m_pViewer->updateGL();
}


// 鼠标移出窗口时调用此函数
void DrawOperationTool::mouseLeaveView(QEvent* ev)
{

}

// 更新操作状态
void DrawOperationTool::updateStatus()
{

}

// 判断点是否在矩形区域内
bool DrawOperationTool::pointInTextureRect(glm::vec3 point)
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

// 判断点是否在矩形区域内
bool DrawOperationTool::pointInWindowRect(glm::vec3 pos)
{
    if (m_pViewer == nullptr)
    {
        return false;
    }

    return m_pViewer->isPointInWindows(pos);

}









