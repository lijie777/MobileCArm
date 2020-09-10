#include "ArrowDT.h"



ArrowDT::ArrowDT()
    : CArmDrawTool(CArmDTEnum::Arrow)
    , m_pDrawStatus(nullptr)
    , m_pArrowBrush(new ArrowBrush())
{
}


ArrowDT::~ArrowDT()
{
    delete m_pArrowBrush;
}

int ArrowDT::getCheckedPointIndex(glm::vec3 * pointEnds, int endsNumber, glm::vec3 point, float distance)
{
    int point_hit_index = -1;
    for (int i = 0;i < endsNumber;i++)
    {
        if (abs(point.x - pointEnds[i].x)<distance && abs(point.y - pointEnds[i].y)<distance)
        {
            point_hit_index = i;
            return point_hit_index;
        }
    }
    return point_hit_index;
}

bool ArrowDT::isPointInRect(glm::vec3 corner[4], glm::vec3 point)
{
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



void ArrowDT::deleteArrow(ArrowObjectPtr obj)
{
    m_arrLst.remove_if([obj](ArrowObjectPtr Object) { return (obj._Object == Object._Object);});
    setStatus(std::shared_ptr<ArrowDTStatus>(new ArrowDTIdleStatus(std::shared_ptr<ArrowDT>(this))));
}

void ArrowDT::deleteAllArrow()
{
    m_arrLst.clear();
}

void ArrowDT::initTool()
{
    m_pArrowBrush->initBrush();
}

std::list<ArrowObjectPtr> ArrowDT::getArrLst()
{
    return m_arrLst;
}

void ArrowDT::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftPress(point, ev, callbackData, callback);
}

void ArrowDT::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void ArrowDT::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftMove(point, ev, callbackData, callback);
}

void ArrowDT::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void ArrowDT::deletePress()
{
    if (m_pDrawStatus != nullptr) m_pDrawStatus->deletePress();
}

void ArrowDT::setVisible(bool en)
{
    for (auto & i : m_arrLst)
    {
        if (i._Object == nullptr) continue;

        i._Object->_visibleMode = en;
    }
}

void ArrowDT::setStatus(int code)
{
    if (code == _CArmDrawToolIdle) setStatus(std::shared_ptr<ArrowDTStatus>(new ArrowDTIdleStatus(std::shared_ptr<ArrowDT>(this))));
}



void ArrowDT::setStatus(std::shared_ptr<ArrowDTStatus> status)
{
    m_pDrawStatus = status;
    if (m_pDrawStatus != nullptr) m_pDrawStatus->startWork();
}

void ArrowDT::addArrowObject(ArrowObjectPtr object)
{
    m_arrLst.push_back(object);
}


void ArrowDT::render(glm::mat4 MVP, float w, float h, float sx, float sy)
{

}

void ArrowDT::setArrowColor(float r, float g, float b, float a)
{
    m_pArrowBrush->set_color(r, g, b, a);
}


bool ArrowDTStatus::isPointOnObject(glm::vec3 point, ArrowObjectPtr obj)
{
    if (obj == nullptr) return false;

    int head_is_hit = -1;
    glm::vec3* ptLst = obj._Object->_pPoints;
    head_is_hit = ArrowDT::getCheckedPointIndex(ptLst, _MAX_ARROW_POINTS, point, obj._Object->_threshDistance);

    if (head_is_hit != -1)
    {
        return true;
    }

    if (ArrowDT::isPointInRect(obj._Object->_pBoxPoints, point) == true)
    {
        return true;
    }

    return false;
}

ArrowDTStatus::EditMode ArrowDTStatus::checkEditMode(glm::vec3 point, ArrowObjectPtr obj, int& idx)
{
    if (obj == nullptr) return ArrowDTStatus::EditMode::EM_None;

    int head_is_hit = -1;
    glm::vec3* ptLst = obj._Object->_pPoints;
    head_is_hit = ArrowDT::getCheckedPointIndex(ptLst, _MAX_ARROW_POINTS, point, obj._Object->_threshDistance);

    if (head_is_hit != -1)
    {
        idx = head_is_hit;
        return ArrowDTStatus::EditMode::EM_Point;
    }

    if (ArrowDT::isPointInRect(obj._Object->_pBoxPoints, point) == true)
    {
        idx = -1;
        return ArrowDTStatus::EditMode::EM_Line;
    }
    return  ArrowDTStatus::EditMode::EM_None;
}

void ArrowDTIdleStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getArrLst())
    {
        i._Object->_checkedMode = false;
    }
}

void ArrowDTIdleStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    std::list<ArrowObjectPtr> list = m_tool->getArrLst();
    for (auto & i : list)
    {
        //重置文本状态为未击中状态
        i._Object->_checkedMode = false;
    }

    // 判断鼠标是否击中文本对象
    for (auto & i : list)
    {
        if (isPointOnObject(point, i) == true)
        {
            // 击中文本，进入激活状态
            i._Object->_checkedMode = true;
            m_tool->setStatus(std::shared_ptr<ArrowDTStatus>(new ArrowDTCheckedStatus(point, i, m_tool)));
            return;
        }
    }

}

void ArrowDTIdleStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void ArrowDTIdleStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void ArrowDTIdleStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}



void ArrowDTCreateStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getArrLst())
    {
        i._Object->_checkedMode = false;
    }
}

void ArrowDTCreateStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_arrow == nullptr)
    {
        // 创建箭头对象
        m_arrow = ArrowObject::createArrowObject();
        if (m_arrow == nullptr) return;

        // 更新箭头数据
        m_arrow._Object->_checkedMode = true;
        m_arrow._Object->_visibleMode = true;
        m_arrow._Object->_threshDistance = 0.010;
        m_arrow._Object->_pPoints[0] = point;
        m_arrow._Object->_pPoints[1] = point;
        m_arrow._Object->_pointCount = 1;
        return;
    }
    
    m_arrow._Object->_pPoints[m_arrow._Object->_pointCount++] = point;

    if (m_arrow._Object->_pointCount >= _MAX_ARROW_POINTS)
    {
        // 添加箭头到列表
        m_tool->addArrowObject(m_arrow);

        // 进入编辑模式
        m_tool->setStatus(std::shared_ptr<ArrowDTStatus>(new ArrowDTCheckedStatus(point, m_arrow, m_tool)));

        // 更新鼠标操作
        if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
    }
}

void ArrowDTCreateStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 进入闲置模式
    m_tool->setStatus(std::shared_ptr<ArrowDTStatus>(new ArrowDTIdleStatus(m_tool)));

    // 更新鼠标操作
    if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
}

void ArrowDTCreateStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void ArrowDTCreateStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void ArrowDTCreateStatus::move(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (m_arrow == nullptr) return;

    m_arrow._Object->_pPoints[m_arrow._Object->_pointCount] = point;
}




void ArrowDTCheckedStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    if (m_arrow._Object != nullptr)
    {
        m_arrow._Object->_checkedMode = true;
    }
}

void ArrowDTCheckedStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{

    // 鼠标击中当前文本
    if (isPointOnObject(point, m_arrow) == true)
    {
        return;
    }

    std::list<ArrowObjectPtr> list = m_tool->getArrLst();
    for (auto & i : list)
    {
        //重置文本状态为未击中状态
        i._Object->_checkedMode = false;
    }

    // 鼠标是否击中文本
    for (auto & i : list)
    {
        if (isPointOnObject(point, i) == true)
        {
            m_editMode = checkEditMode(point, i, m_checkedPtIdx);
            // 击中文本，进入激活状态
            i._Object->_checkedMode = true;
            m_oldPt = point;
            return;
        }
    }

    // 鼠标未击中文本
    if (isPointOnObject(point, m_arrow) == false)
    {
        m_arrow._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<ArrowDTStatus>(new ArrowDTIdleStatus(m_tool)));
    }
}

void ArrowDTCheckedStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 右键击中当前文本
    if (isPointOnObject(point, m_arrow) == true)
    {
        m_arrow._Object->_checkedMode = true;
        // 弹窗提示删除
        m_menu->move(ev->globalPos());
        m_menu->show();
    }
    // 未击中当前文本
    else
    {
        m_arrow._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<ArrowDTStatus>(new ArrowDTIdleStatus(m_tool)));
    }
}




void ArrowDTCheckedStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_arrow == nullptr) return;

    if (m_editMode == EditMode::EM_Point)
    {
        glm::vec3 translateVector = point - m_oldPt;
        m_arrow._Object->_pPoints[m_checkedPtIdx] += translateVector;
    }
    else if (m_editMode == EditMode::EM_Line)
    {
        glm::vec3 translateVector = point - m_oldPt;
        m_arrow._Object->_pPoints[0] += translateVector;
        m_arrow._Object->_pPoints[1] += translateVector;
        m_arrow._Object->_pBoxPoints[0] += translateVector;
        m_arrow._Object->_pBoxPoints[1] += translateVector;
        m_arrow._Object->_pBoxPoints[2] += translateVector;
        m_arrow._Object->_pBoxPoints[3] += translateVector;
    }

    m_oldPt = point;
}

void ArrowDTCheckedStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void ArrowDTCheckedStatus::deletePress()
{
    m_tool->deleteArrow(m_arrow);
}





void ArrowMenu::deleteOneObejct(bool en)
{
    m_arrTool->deleteArrow(m_arrow);
}


