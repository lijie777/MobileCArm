#include "DirectTextTool.h"



DirectTextTool::DirectTextTool(int type, std::wstring dir)
    : CArmDrawTool(type)
    , m_pDrawStatus(nullptr)
    , m_pTextBrush(new TextBrush())
    , m_string(dir)
{
}


DirectTextTool::~DirectTextTool()
{
    delete m_pTextBrush;
}



void DirectTextTool::deleteText()
{
    m_text = nullptr;
    setStatus(std::shared_ptr<DirectTextStatus>(new DirectTextIdleStatus(std::shared_ptr<DirectTextTool>(this))));
}

void DirectTextTool::initTool()
{
    m_pTextBrush->init_resources();
}

void DirectTextTool::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftPress(point, ev, callbackData, callback);
}

void DirectTextTool::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->rightPress(point, ev, callbackData, callback);
}

void DirectTextTool::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftMove(point, ev, callbackData, callback);
}

void DirectTextTool::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void DirectTextTool::deletePress()
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->deletePress();
}

std::wstring DirectTextTool::getDirectString()
{
    return m_string;
}

TextObjectPtr DirectTextTool::getTextObj()
{
    return m_text;
}

void DirectTextTool::setVisible(bool en)
{
    if (m_text == nullptr) return;

    m_text._Object->_visibleMode = en;
}

void DirectTextTool::setStatus(int code)
{
    if (code == _CArmDrawToolIdle) setStatus(std::shared_ptr<DirectTextStatus>(new DirectTextIdleStatus(std::shared_ptr<DirectTextTool>(this))));
}



void DirectTextTool::setStatus(std::shared_ptr<DirectTextStatus> status)
{
    m_pDrawStatus = status;
    if (m_pDrawStatus != nullptr) m_pDrawStatus->startWork();
}

void DirectTextTool::addTextObject(TextObjectPtr object)
{
    m_text = object;
}


void DirectTextTool::render(glm::mat4 MVP, float w, float h, float sx, float sy)
{

}

void DirectTextTool::setTextColor(float r, float g, float b, float a)
{
    m_pTextBrush->set_color(r, g, b, a);
}

void DirectTextTool::setTextSize(int size)
{
    m_pTextBrush->set_fonts_size(size);
}

void DirectTextTool::setBackgroundColor(float r, float g, float b, float a)
{
    m_pTextBrush->set_background_color(r, g, b, a);
}

void DirectTextTool::setOutlineWidth(int width)
{
}

void DirectTextTool::setOutlineColor(float r, float g, float b, float a)
{
}


bool DirectTextStatus::isPointOnObject(glm::vec3 point, TextObjectPtr text)
{
    if (text == nullptr) return false;

    glm::vec3* textRect = text._Object->_textRange;

    float a = glm::cross(textRect[0] - point, textRect[1] - point).z;
    float b = glm::cross(textRect[1] - point, textRect[2] - point).z;
    float c = glm::cross(textRect[2] - point, textRect[3] - point).z;
    float d = glm::cross(textRect[3] - point, textRect[0] - point).z;
    if ((a < 0 && b < 0 && c < 0 && d < 0) || (a > 0 && b > 0 && c > 0 && d > 0))
    {
        return true;    //在区域内
    }

    return false;
}

void DirectTextIdleStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    if(m_tool->getTextObj()._Object != nullptr)
    {
        m_tool->getTextObj()._Object->_checkedMode = false;
    }
}

void DirectTextIdleStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    TextObjectPtr text = m_tool->getTextObj();
    if (isPointOnObject(point, text) == false)
    {
        text._Object->_checkedMode = false;
    }
    else
    {
        text._Object->_checkedMode = true;
        m_tool->setStatus(std::shared_ptr<DirectTextStatus>(new DirectTextCheckedStatus(text, m_tool)));
    }
}

void DirectTextIdleStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void DirectTextIdleStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void DirectTextIdleStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}



void DirectTextCreateStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    if (m_tool->getTextObj()._Object != nullptr)
    {
        m_tool->getTextObj()._Object->_checkedMode = false;
    }
}

void DirectTextCreateStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    TextObjectPtr obj = TextObject::createTextObject();
    if (obj._Object != nullptr)
    {
        obj._Object->_visibleMode = true;
        obj._Object->_checkedMode = false;
        obj._Object->_textBody = m_tool->getDirectString();
        m_tool->addTextObject(obj);
        m_tool->setStatus(std::shared_ptr<DirectTextStatus>(new DirectTextCheckedStatus(obj, m_tool)));
    }
}

void DirectTextCreateStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    //结束绘制
    m_tool->setStatus(std::shared_ptr<DirectTextStatus>(new DirectTextIdleStatus(m_tool)));
    if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
}

void DirectTextCreateStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void DirectTextCreateStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}



void DirectTextCheckedStatus::startWork()
{
    // 改变光标样式

    // 激活对象
    if (m_tool->getTextObj()._Object != nullptr)
    {
        m_tool->getTextObj()._Object->_checkedMode = true;
    }
}

void DirectTextCheckedStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (isPointOnObject(point, m_text) == false)
    {
        m_text._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<DirectTextStatus>(new DirectTextIdleStatus(m_tool)));
    }
}

void DirectTextCheckedStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (isPointOnObject(point, m_text) == true)
    {
        m_text._Object->_checkedMode = true;
        // 弹窗提示删除
        m_menu->move(ev->globalPos());
        m_menu->show();
    }
    else
    {
        m_text._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<DirectTextStatus>(new DirectTextIdleStatus(m_tool)));
    }
}

void DirectTextCheckedStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_text == nullptr) return;

    glm::vec3 translateVector = point - m_oldPt;
    m_text._Object->_pPoints[0] += translateVector;

    m_oldPt = point;
}

void DirectTextCheckedStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void DirectTextCheckedStatus::deletePress()
{
    m_tool->deleteText();
}






