#include "ReviewOperationTool.h"

#pragma execution_character_set("utf-8")



ReviewOperationTool::ReviewOperationTool(BaseView* viewer)
    : IOperationTool(viewer)
{
}


ReviewOperationTool::~ReviewOperationTool()
{
}

void ReviewOperationTool::addDrawTool(CArmDrawToolPtr tool)
{
    m_lst.push_back(tool);
}




void ReviewOperationTool::removeDrawTool(CArmDrawToolPtr tool)
{
    m_lst.remove(tool);
}

// 结束操作
void ReviewOperationTool::finishOperation()
{

}

// 开始操作
void ReviewOperationTool::startOperation()
{

}

void ReviewOperationTool::rightMousePressed(glm::vec3 point, QMouseEvent * ev)
{

}

void ReviewOperationTool::leftMousePressed(glm::vec3 point, QMouseEvent * ev)
{
    m_currTool = nullptr;

    for (auto & i : m_lst)
    {
        // 重置画图工具状态为闲置
        i.setStatus(_CArmDrawToolIdle);
    }

    for (auto & i : m_lst)
    {
        i.leftPress(point, ev);
        if (i.getStatusCode() == _CArmDrawToolChecked)
        {
            m_currTool = i;
            break;
        }
    }
}

void ReviewOperationTool::rightMouseDoublePressed(glm::vec3 point, QMouseEvent * ev)
{
}

void ReviewOperationTool::leftMouseDoublePressed(glm::vec3 point, QMouseEvent * ev)
{
}

void ReviewOperationTool::rightMouseMoving(glm::vec3 point, QMouseEvent * ev)
{
}

void ReviewOperationTool::leftMouseMoving(glm::vec3 point, QMouseEvent * ev)
{
    m_currTool.leftMove(point, ev);
}

void ReviewOperationTool::MouseMoving(glm::vec3 point, QMouseEvent * ev)
{

}

void ReviewOperationTool::rightMouseRelease(glm::vec3 point, QMouseEvent * ev)
{
}

void ReviewOperationTool::leftMouseRelease(glm::vec3 point, QMouseEvent * ev)
{
}

void ReviewOperationTool::wheelRolling(int d, glm::vec3 point, QWheelEvent * ev)
{
}

void ReviewOperationTool::keyPressed(QKeyEvent * ev)
{

}

// 鼠标移出窗口时调用此函数
void ReviewOperationTool::mouseLeaveView(QEvent* ev)
{

}

// 更新操作状态
void ReviewOperationTool::updateStatus()
{

    if (m_pViewer != nullptr)
    {
        m_pViewer->updateGL();
    }
}

// 判断点是否在矩形区域内
bool ReviewOperationTool::pointInTextureRect(glm::vec3 point)
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












ReviewOperationToolPtr::ReviewOperationToolPtr(ReviewOperationTool * obj)
    : _Object(obj)
{
    if (obj != nullptr)
    {
        obj->_refCount++;
    }
    _Object = obj;
}

ReviewOperationToolPtr::ReviewOperationToolPtr(const ReviewOperationToolPtr & ptr)
{
    if (ptr._Object != nullptr)
    {
        ptr._Object->_refCount++;
    }
    _Object = ptr._Object;
}

ReviewOperationToolPtr::~ReviewOperationToolPtr()
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


ReviewOperationToolPtr & ReviewOperationToolPtr::operator=(const ReviewOperationToolPtr & ptr)
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

bool ReviewOperationToolPtr::operator!=(const ReviewOperationToolPtr& ptr)
{
    // TODO: 在此处插入 return 语句
    return (this->_Object != ptr._Object);
}

bool ReviewOperationToolPtr::operator==(const ReviewOperationToolPtr& ptr)
{
    return (this->_Object == ptr._Object);
}

