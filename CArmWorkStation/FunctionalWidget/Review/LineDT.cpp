#include "LineDT.h"


#define _DefaultLineLength 0.1f

LineDT::LineDT()
    : CArmDrawTool(CArmDTEnum::Line)
    , m_pDrawStatus(nullptr)
    , m_pLineBrush(new LineBrush())
    , m_pTextBrush(new TextBrush())
    , m_pCircleBrush(new CircleBrush())
{
}


LineDT::~LineDT()
{
    delete m_pLineBrush;
    delete m_pTextBrush;
    delete m_pCircleBrush;
}

int LineDT::getCheckedPointIndex(glm::vec3 * pointEnds, int endsNumber, glm::vec3 point, float distance)
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

bool LineDT::isPointInRect(glm::vec3 corner[4], glm::vec3 point)
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

void LineDT::box(LineObjectPtr obj)
{
    if (obj._Object == nullptr) return;

    if (glm::length(obj._Object->_pPoints[1] - obj._Object->_pPoints[0]) < 0.001)
    {
        return;//跳过重合的点
    }

    glm::vec3 tang = glm::normalize(obj._Object->_pPoints[1] - obj._Object->_pPoints[0]);
    glm::vec3 norm = glm::normalize(glm::vec3(tang.y, -tang.x, 0.0f));
    obj._Object->_pBoxPoints[0] = obj._Object->_pPoints[0] + obj._Object->_threshDistance*norm;
    obj._Object->_pBoxPoints[1] = obj._Object->_pPoints[0] - obj._Object->_threshDistance*norm;
    obj._Object->_pBoxPoints[2] = obj._Object->_pPoints[1] - obj._Object->_threshDistance*norm;
    obj._Object->_pBoxPoints[3] = obj._Object->_pPoints[1] + obj._Object->_threshDistance*norm;
}



void LineDT::deleteLine(LineObjectPtr obj)
{
    m_lineLst.remove_if([obj](LineObjectPtr Object) { return (obj._Object == Object._Object);});
    setStatus(std::shared_ptr<LineDTStatus>(new LineDTIdleStatus(std::shared_ptr<LineDT>(this))));
}

void LineDT::deleteAllLine()
{
    m_lineLst.clear();
}

void LineDT::initTool()
{
    m_pLineBrush->initBrush();
    m_pTextBrush->init_resources();
    m_pCircleBrush->initBrush();
}

std::list<LineObjectPtr> LineDT::getLineLst()
{
    return m_lineLst;
}

void LineDT::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftPress(point, ev, callbackData, callback);
}

void LineDT::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void LineDT::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftMove(point, ev, callbackData, callback);
}

void LineDT::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void LineDT::deletePress()
{
    if (m_pDrawStatus != nullptr) m_pDrawStatus->deletePress();
}

void LineDT::setVisible(bool en)
{
    for (auto & i : m_lineLst)
    {
        if (i._Object == nullptr) continue;

        i._Object->_visibleMode = en;
    }
}

void LineDT::setStatus(int code)
{
    if (code == _CArmDrawToolIdle) setStatus(std::shared_ptr<LineDTStatus>(new LineDTIdleStatus(std::shared_ptr<LineDT>(this))));
}



void LineDT::setStatus(std::shared_ptr<LineDTStatus> status)
{

    m_pDrawStatus = status;
    if (m_pDrawStatus != nullptr) m_pDrawStatus->startWork();
}

void LineDT::addLineObject(LineObjectPtr object)
{
    m_lineLst.push_back(object);
}


void LineDT::draw(glm::mat4 MVP, float w, float h, float sx, float sy)
{
    for (auto & i : m_lineLst)
    {
        if (i == nullptr) continue;
        if (i._Object->_visibleMode == false) continue;
        
        m_pAmendTool.amendMeasureObject(i._Object);
        LineDT::box(i);

        draw_substance(i, MVP);
        draw_legend(i, MVP, w, h, sx, sy);
        draw_handle(i, MVP);
    }
}

void LineDT::setLineColor(float r, float g, float b, float a)
{
    m_color = glm::vec4(r, g, b, a);
}

void LineDT::draw_substance(LineObjectPtr obj, glm::mat4 MVP)
{
    if (obj._Object == nullptr) return;

    if (obj._Object->_checkedMode == false)
    {
        m_pLineBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a);
        m_pLineBrush->set_endVisible(false);
    }
    else
    {
        m_pLineBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a / 2);
        m_pLineBrush->set_endVisible(true);
    }
    m_pLineBrush->set_vertices(obj._Object->_pPoints);
    m_pLineBrush->RenderLine(glm::value_ptr(MVP));
}

void LineDT::draw_legend(LineObjectPtr obj, glm::mat4 MVP, float w, float h, float sx, float sy)
{
    if (obj == nullptr) return;
    
    char dist_str[32];
    sprintf_s(dist_str, "%.2f mm", obj._Object->_linePhysicalLength);
    glm::vec4 a = MVP*glm::vec4(obj._Object->_textPos.x, obj._Object->_textPos.y, 0, 1);

    m_pTextBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    m_pTextBrush->set_background_visible(false);
    m_pTextBrush->render_text(dist_str, a.x, a.y, sx, sy);

    // 获取文本位置
    float x0, x1, y0, y1;
    m_pTextBrush->get_range(x0, x1, y0, y1);
    ////反变换成窗口坐标
    glm::vec3 leftBotton = glm::vec3(x0, y0, 0);
    glm::vec3 rightBotton = glm::vec3(x1, y0, 0);
    glm::vec3 rightTop = glm::vec3(x1, y1, 0);
    glm::vec3 leftTop = glm::vec3(x0, y1, 0);

    leftBotton = glm::vec3(glm::inverse(MVP) *glm::vec4(leftBotton, 1));
    rightBotton = glm::vec3(glm::inverse(MVP) *glm::vec4(rightBotton, 1));
    rightTop = glm::vec3(glm::inverse(MVP) *glm::vec4(rightTop, 1));
    leftTop = glm::vec3(glm::inverse(MVP) *glm::vec4(leftTop, 1));

    obj._Object->_textRange[0] = leftBotton;//obj._Object->_textPos.x;
    obj._Object->_textRange[1] = rightBotton;//obj._Object->_textPos.x + (x1 - x0) * 2;
    obj._Object->_textRange[2] = rightTop;//obj._Object->_textPos.y;
    obj._Object->_textRange[3] = leftTop;//obj._Object->_textPos.y + (y1 - y0) * 2;

    glm::vec3 pConnect[2];
    pConnect[0] = glm::vec3(obj._Object->_textLinkPos.x, obj._Object->_textLinkPos.y, 0);
    pConnect[1] = glm::vec3(obj._Object->_textPos.x, obj._Object->_textPos.y, 0);
    m_pLineBrush->set_vertices(pConnect);
    m_pLineBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    m_pLineBrush->RenderLine(glm::value_ptr(MVP));


}

void LineDT::draw_handle(LineObjectPtr obj, glm::mat4 MVP)
{
    if (obj == nullptr) return;
    if (obj._Object->_checkedMode == false) return;

    m_pCircleBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    m_pCircleBrush->set_center(obj._Object->_pPoints[0]);
    m_pCircleBrush->computeCirclePoints();
    m_pCircleBrush->RenderCircle(glm::value_ptr(MVP), CircleBrush::CIRCLE_FILLED::CF_NO);
    m_pCircleBrush->set_center(obj._Object->_pPoints[1]);
    m_pCircleBrush->computeCirclePoints();
    m_pCircleBrush->RenderCircle(glm::value_ptr(MVP), CircleBrush::CIRCLE_FILLED::CF_NO);
}


bool LineDTStatus::isPointOnObject(glm::vec3 point, LineObjectPtr obj)
{
    if (obj == nullptr) return false;

    int head_is_hit = -1;
    glm::vec3* ptLst = obj._Object->_pPoints;
    head_is_hit = LineDT::getCheckedPointIndex(ptLst, _MAX_LINE_POINTS, point, obj._Object->_threshDistance);

    // 判断是否击中端点
    if (head_is_hit != -1)
    {
        return true;
    }

    // 判断是否击中线段
    if (LineDT::isPointInRect(obj._Object->_pBoxPoints, point) == true)
    {
        return true;
    }

    // 判断是否击中标签
    if (LineDT::isPointInRect(obj._Object->_textRange, point) == true)
    {
        return true;
    }

    return false;
}

LineDTStatus::EditMode LineDTStatus::checkEditMode(glm::vec3 point, LineObjectPtr obj, int& idx)
{
    if (obj == nullptr) return LineDTStatus::EditMode::EM_None;

    int head_is_hit = -1;
    glm::vec3* ptLst = obj._Object->_pPoints;
    head_is_hit = LineDT::getCheckedPointIndex(ptLst, _MAX_LINE_POINTS, point, obj._Object->_threshDistance);

    if (head_is_hit != -1)
    {
        idx = head_is_hit;
        return LineDTStatus::EditMode::EM_Point;
    }

    if (LineDT::isPointInRect(obj._Object->_pBoxPoints, point) == true)
    {
        idx = -1;
        return LineDTStatus::EditMode::EM_Line;
    }
    return  LineDTStatus::EditMode::EM_None;
}

void LineDTIdleStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getLineLst())
    {
        i._Object->_checkedMode = false;
    }
}

void LineDTIdleStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    std::list<LineObjectPtr> list = m_tool->getLineLst();
    for (auto & i : list)
    {
        //重置线段状态为未击中状态
        i._Object->_checkedMode = false;
    }

    // 判断鼠标是否击中线段对象
    for (auto & i : list)
    {
        if (isPointOnObject(point, i) == true)
        {
            // 击中线段，进入激活状态
            i._Object->_checkedMode = true;
            m_tool->setStatus(std::shared_ptr<LineDTStatus>(new LineDTCheckedStatus(point, i, m_tool)));
            return;
        }
    }

}

void LineDTIdleStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void LineDTIdleStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void LineDTIdleStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}



void LineDTCreateStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getLineLst())
    {
        i._Object->_checkedMode = false;
    }
}

void LineDTCreateStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_line == nullptr)
    {
        // 创建直线对象
        m_line = LineObject::createLineObject();
        if (m_line == nullptr) return;

        // 更新直线数据
        m_line._Object->_checkedMode = true;
        m_line._Object->_visibleMode = true;
        m_line._Object->_threshDistance = 0.010;
        m_line._Object->_pPoints[0] = point;
        m_line._Object->_pPoints[1] = point;
        m_line._Object->_pointCount = 1;
        return;
    }
    
    m_line._Object->_pPoints[m_line._Object->_pointCount++] = point;

    if (m_line._Object->_pointCount >= _MAX_LINE_POINTS)
    {
        // 添加直线到列表
        LineDT::box(m_line);
        m_tool->addLineObject(m_line);

        // 进入编辑模式
        m_tool->setStatus(std::shared_ptr<LineDTStatus>(new LineDTCheckedStatus(point, m_line, m_tool)));

        // 更新鼠标操作
        if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
    }
}

void LineDTCreateStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 进入闲置模式
    m_tool->setStatus(std::shared_ptr<LineDTStatus>(new LineDTIdleStatus(m_tool)));

    // 更新鼠标操作
    if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
}

void LineDTCreateStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void LineDTCreateStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void LineDTCreateStatus::move(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (m_line == nullptr) return;

    m_line._Object->_pPoints[m_line._Object->_pointCount] = point;
    glm::vec3 d = m_line._Object->_pPoints[1] - m_line._Object->_pPoints[0];
    glm::vec3 n = glm::normalize(glm::vec3(d.y, -d.x, 0));

    m_line._Object->_textPos.x = (m_line._Object->_pPoints[1].x + _DefaultLineLength);
    m_line._Object->_textPos.y = (m_line._Object->_pPoints[1].y);
    m_line._Object->_textLinkPos.x = (m_line._Object->_pPoints[1].x);
    m_line._Object->_textLinkPos.y = (m_line._Object->_pPoints[1].y);

    LineDT::box(m_line);
}




void LineDTCheckedStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    if (m_line._Object != nullptr)
    {
        m_line._Object->_checkedMode = true;
    }
}

void LineDTCheckedStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    std::list<LineObjectPtr> list = m_tool->getLineLst();
    for (auto & i : list)
    {
        //重置线段状态为未击中状态
        i._Object->_checkedMode = false;
    }

    // 鼠标是否击中线段
    for (auto & i : list)
    {
        if (isPointOnObject(point, i) == true)
        {
            m_editMode = checkEditMode(point, i, m_checkedPtIdx);
            // 击中线段，进入激活状态
            i._Object->_checkedMode = true;
            m_line = i;
            m_oldPt = point;
            return;
        }
    }

    // 鼠标未击中线段
    if (isPointOnObject(point, m_line) == false)
    {
        m_line._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<LineDTStatus>(new LineDTIdleStatus(m_tool)));
    }
}

void LineDTCheckedStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 右键击中当前线段
    if (isPointOnObject(point, m_line) == true)
    {
        m_line._Object->_checkedMode = true;
        // 弹窗提示删除
        m_menu->move(ev->globalPos());
        m_menu->show();
    }
    // 未击中当前线段
    else
    {
        m_line._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<LineDTStatus>(new LineDTIdleStatus(m_tool)));
    }
}




void LineDTCheckedStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_line == nullptr) return;

    if (m_editMode == EditMode::EM_Point)
    {
        glm::vec3 translateVector = point - m_oldPt;
        m_line._Object->_pPoints[m_checkedPtIdx] += translateVector;
        glm::vec3 moveVec = m_line._Object->_pPoints[1] - m_line._Object->_textPos;
        m_line._Object->_textPos.x = (m_line._Object->_pPoints[1].x + _DefaultLineLength);
        m_line._Object->_textPos.y = (m_line._Object->_pPoints[1].y);
        m_line._Object->_textLinkPos.x = (m_line._Object->_pPoints[1].x);
        m_line._Object->_textLinkPos.y = (m_line._Object->_pPoints[1].y);

        m_line._Object->_textRange[0] += moveVec;
        m_line._Object->_textRange[1] += moveVec;
        m_line._Object->_textRange[2] += moveVec;
        m_line._Object->_textRange[3] += moveVec;

        LineDT::box(m_line);
    }
    else if (m_editMode == EditMode::EM_Line)
    {
        glm::vec3 translateVector = point - m_oldPt;
        m_line._Object->_pPoints[0] += translateVector;
        m_line._Object->_pPoints[1] += translateVector;
        m_line._Object->_textPos.x = (m_line._Object->_pPoints[1].x + _DefaultLineLength);
        m_line._Object->_textPos.y = (m_line._Object->_pPoints[1].y);
        m_line._Object->_textLinkPos.x = (m_line._Object->_pPoints[1].x);
        m_line._Object->_textLinkPos.y = (m_line._Object->_pPoints[1].y);

        LineDT::box(m_line);
    }
    else if (m_editMode == EditMode::EM_Text)
    {
        glm::vec3 translateVector = point - m_oldPt;
        m_line._Object->_textPos += translateVector;
        m_line._Object->_textRange[0] += translateVector;
        m_line._Object->_textRange[1] += translateVector;
        m_line._Object->_textRange[2] += translateVector;
        m_line._Object->_textRange[3] += translateVector;
    }

    m_oldPt = point;
}

void LineDTCheckedStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void LineDTCheckedStatus::deletePress()
{
    m_tool->deleteLine(m_line);
}


void LineMenu::deleteOneObejct(bool en)
{
    m_lineTool->deleteLine(m_line);
}


