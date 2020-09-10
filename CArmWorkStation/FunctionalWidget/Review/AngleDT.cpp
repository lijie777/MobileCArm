#include "AngleDT.h"


#define _DefaultLineLength 0.1f

AngleDT::AngleDT()
    : CArmDrawTool(CArmDTEnum::Angle)
    , m_pDrawStatus(nullptr)
    , m_pAngleBrush(new AngleBrush())
    , m_pTextBrush(new TextBrush())
    , m_pLineBrush(new LineBrush())
    , m_pCircleBrush(new CircleBrush())
{
}


AngleDT::~AngleDT()
{
    delete m_pAngleBrush;
    delete m_pTextBrush;
    delete m_pLineBrush;
    delete m_pCircleBrush;

}

int AngleDT::getCheckedPointIndex(glm::vec3 * pointEnds, int endsNumber, glm::vec3 point, float distance)
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

bool AngleDT::isPointInRect(glm::vec3 corner[4], glm::vec3 point)
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



void AngleDT::deleteAngle(AngleObjectPtr obj)
{
    m_aglLst.remove_if([obj](AngleObjectPtr Object) { return (obj._Object == Object._Object);});
    setStatus(std::shared_ptr<AngleDTStatus>(new AngleDTIdleStatus(std::shared_ptr<AngleDT>(this))));
}

void AngleDT::deleteAllAngle()
{
    m_aglLst.clear();
}

void AngleDT::initTool()
{
    m_pAngleBrush->initRender();
    m_pLineBrush->initBrush();
    m_pTextBrush->init_resources();
    m_pCircleBrush->initBrush();
}

std::list<AngleObjectPtr> AngleDT::getAngLst()
{
    return m_aglLst;
}

void AngleDT::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftPress(point, ev, callbackData, callback);
}

void AngleDT::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void AngleDT::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftMove(point, ev, callbackData, callback);
}

void AngleDT::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void AngleDT::deletePress()
{
    if (m_pDrawStatus != nullptr) m_pDrawStatus->deletePress();
}

void AngleDT::setVisible(bool en)
{
    for (auto & i : m_aglLst)
    {
        if (i._Object == nullptr) continue;

        i._Object->_visibleMode = en;
    }
}

void AngleDT::setStatus(int code)
{
    if (code == _CArmDrawToolIdle) setStatus(std::shared_ptr<AngleDTStatus>(new AngleDTIdleStatus(std::shared_ptr<AngleDT>(this))));
}



void AngleDT::setStatus(std::shared_ptr<AngleDTStatus> status)
{
    m_pDrawStatus = status;
}

void AngleDT::addAngleObject(AngleObjectPtr object)
{
    m_aglLst.push_back(object);
}


void AngleDT::draw(glm::mat4 MVP, float w, float h, float sx, float sy)
{
    for (auto & i : m_aglLst)
    {
        if (i._Object == nullptr) continue;

        if (i._Object->_visibleMode == false) continue;

        m_pAmendTool.amendMeasureObject(i._Object);
        draw_substance(i, MVP);
        draw_legend(i, MVP, w, h, sx, sy);
        draw_handle(i, MVP);
    }
}

void AngleDT::setAngleColor(float r, float g, float b, float a)
{
    m_color = glm::vec4(r, g, b, a);
}

void AngleDT::draw_substance(AngleObjectPtr obj, glm::mat4 MVP)
{
    if (obj == nullptr) return;

    if (obj._Object->_checkedMode == true) m_pAngleBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    else m_pAngleBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a / 2);

    m_pAngleBrush->set_endVisible(false);
    if (obj._Object->_pointCount < 2)
    {
        m_pAngleBrush->set_2vertices(obj._Object->_pPoints);
        m_pAngleBrush->RenderHalfAngle(glm::value_ptr(MVP));
    }
    else
    {
        m_pAngleBrush->set_vertices(obj._Object->_pPoints);
        m_pAngleBrush->RenderAngle(glm::value_ptr(MVP));
    }
}

void AngleDT::draw_legend(AngleObjectPtr obj, glm::mat4 MVP, float w, float h, float sx, float sy)
{
    if (obj == nullptr) return;
    if (obj._Object->_pointCount < _MAX_ANGLE_POINTS) return;

    wchar_t angle_str[32];
    swprintf_s(angle_str, L"%.2f °", obj._Object->_anglePhysicalValue);
    glm::vec4 a = (MVP)*glm::vec4(obj._Object->_textPos.x, obj._Object->_textPos.y, 0, 1);


    m_pTextBrush->set_background_visible(false);
    m_pTextBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    m_pTextBrush->render_text(angle_str, a.x, a.y, sx, sy);

    // 获取角度位置
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

    //draw legend origin
    glm::vec3 pConnect[2];
    pConnect[0] = glm::vec3(obj._Object->_textLinkPos.x, obj._Object->_textLinkPos.y, 0);
    pConnect[1] = glm::vec3(obj._Object->_textPos.x, obj._Object->_textPos.y, 0);
    m_pLineBrush->set_vertices(pConnect);
    m_pLineBrush->set_color(m_color.r, m_color.g, m_color.b, m_color.a);
    m_pLineBrush->RenderLine(glm::value_ptr(MVP));
}

void AngleDT::draw_handle(AngleObjectPtr obj, glm::mat4 MVP)
{
    if (obj == nullptr) return;
    if (obj._Object->_pointCount < 2) return;
    if (obj._Object->_checkedMode == true)
    {
        for (int j = 0;j < 3;j++)
        {
            m_pCircleBrush->set_color(m_color.r,m_color.g, m_color.b, m_color.a);
            m_pCircleBrush->set_center(obj._Object->_pPoints[j]);
            m_pCircleBrush->computeCirclePoints();
            m_pCircleBrush->RenderCircle(glm::value_ptr(MVP), CircleBrush::CIRCLE_FILLED::CF_NO);
        }
    }
}

void AngleDT::box(AngleObjectPtr object)
{
    if (object._Object == nullptr) return;

    //跳过重合的点
    if (glm::length(object._Object->_pPoints[1] - object._Object->_pPoints[0]) > 0.001)
    {
        glm::vec3 tang = glm::normalize(object._Object->_pPoints[1] - object._Object->_pPoints[0]);
        glm::vec3 norm = glm::normalize(glm::vec3(tang.y, -tang.x, 0.0f));
        object._Object->_pBoxPoints[0] = object._Object->_pPoints[0] + object._Object->_threshDistance*norm;
        object._Object->_pBoxPoints[1] = object._Object->_pPoints[0] - object._Object->_threshDistance*norm;
        object._Object->_pBoxPoints[2] = object._Object->_pPoints[1] - object._Object->_threshDistance*norm;
        object._Object->_pBoxPoints[3] = object._Object->_pPoints[1] + object._Object->_threshDistance*norm;
    }


    //跳过重合的点
    if (glm::length(object._Object->_pPoints[2] - object._Object->_pPoints[1]) > 0.001)
    {
        glm::vec3 tang = glm::normalize(object._Object->_pPoints[2] - object._Object->_pPoints[1]);
        glm::vec3 norm = glm::normalize(glm::vec3(tang.y, -tang.x, 0.0f));
        object._Object->_pBoxPoints[4] = object._Object->_pPoints[1] + object._Object->_threshDistance*norm;
        object._Object->_pBoxPoints[5] = object._Object->_pPoints[1] - object._Object->_threshDistance*norm;
        object._Object->_pBoxPoints[6] = object._Object->_pPoints[2] - object._Object->_threshDistance*norm;
        object._Object->_pBoxPoints[7] = object._Object->_pPoints[2] + object._Object->_threshDistance*norm;
    }

}


bool AngleDTStatus::isPointOnObject(glm::vec3 point, AngleObjectPtr obj)
{
    if (obj == nullptr) return false;

    int head_is_hit = -1;
    glm::vec3* ptLst = obj._Object->_pPoints;
    head_is_hit = AngleDT::getCheckedPointIndex(ptLst, _MAX_ANGLE_POINTS, point, obj._Object->_threshDistance);

    if (head_is_hit != -1)
    {
        return true;
    }

    if (AngleDT::isPointInRect(obj._Object->_pBoxPoints, point) == true)
    {
        return true;
    }

    if (AngleDT::isPointInRect(obj._Object->_pBoxPoints + 4, point) == true)
    {
        return true;
    }

    if (AngleDT::isPointInRect(obj._Object->_textRange, point) == true)
    {
        return true;
    }

    return false;
}

AngleDTStatus::EditMode AngleDTStatus::checkEditMode(glm::vec3 point, AngleObjectPtr obj, int& idx)
{
    if (obj == nullptr) return AngleDTStatus::EditMode::EM_None;

    int head_is_hit = -1;
    glm::vec3* ptLst = obj._Object->_pPoints;
    head_is_hit = AngleDT::getCheckedPointIndex(ptLst, _MAX_ANGLE_POINTS, point, obj._Object->_threshDistance);

    if (head_is_hit != -1)
    {
        idx = head_is_hit;
        return AngleDTStatus::EditMode::EM_Point;
    }

    if (AngleDT::isPointInRect(obj._Object->_pBoxPoints, point) == true)
    {
        idx = -1;
        return AngleDTStatus::EditMode::EM_Line;
    }

    if (AngleDT::isPointInRect(obj._Object->_pBoxPoints + 4, point) == true)
    {
        idx = -1;
        return AngleDTStatus::EditMode::EM_Line;
    }

    if (AngleDT::isPointInRect(obj._Object->_textRange, point) == true)
    {
        idx = -1;
        return AngleDTStatus::EditMode::EM_Text;
    }

    return  AngleDTStatus::EditMode::EM_None;
}

void AngleDTIdleStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getAngLst())
    {
        i._Object->_checkedMode = false;
    }
}

void AngleDTIdleStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    std::list<AngleObjectPtr> list = m_tool->getAngLst();
    for (auto & i : list)
    {
        //重置角度状态为未击中状态
        i._Object->_checkedMode = false;
    }

    // 判断鼠标是否击中角度对象
    for (auto & i : list)
    {
        if (isPointOnObject(point, i) == true)
        {
            // 击中角度，进入激活状态
            i._Object->_checkedMode = true;
            m_tool->setStatus(std::shared_ptr<AngleDTStatus>(new AngleDTCheckedStatus(point, i, m_tool)));
            return;
        }
    }

}

void AngleDTIdleStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void AngleDTIdleStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void AngleDTIdleStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}



void AngleDTCreateStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getAngLst())
    {
        i._Object->_checkedMode = false;
    }
}

void AngleDTCreateStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_aglow == nullptr)
    {
        // 创建箭头对象
        m_aglow = AngleObject::createAngleObject();
        if (m_aglow == nullptr) return;

        // 更新箭头数据
        m_aglow._Object->_checkedMode = true;
        m_aglow._Object->_visibleMode = true;
        m_aglow._Object->_threshDistance = 0.010;
        m_aglow._Object->_pPoints[0] = point;
        m_aglow._Object->_pPoints[1] = point;
        m_aglow._Object->_pPoints[2] = point;
        m_aglow._Object->_pointCount = 1;

        m_aglow._Object->_textLinkPos.x = (m_aglow._Object->_pPoints[1].x);
        m_aglow._Object->_textLinkPos.y = (m_aglow._Object->_pPoints[1].y);
        m_aglow._Object->_textPos.x = (m_aglow._Object->_pPoints[1].x + _DefaultLineLength);
        m_aglow._Object->_textPos.y = (m_aglow._Object->_pPoints[1].y);

        return;
    }
    
    m_aglow._Object->_pPoints[m_aglow._Object->_pointCount++] = point;

    if (m_aglow._Object->_pointCount >= _MAX_ANGLE_POINTS)
    {
        //更新箭头数据
        AngleDT::box(m_aglow);
        m_aglow._Object->_checkedMode = true;

        // 添加箭头到列表
        m_tool->addAngleObject(m_aglow);

        // 进入编辑模式
        m_tool->setStatus(std::shared_ptr<AngleDTStatus>(new AngleDTCheckedStatus(point, m_aglow, m_tool)));

        // 更新鼠标操作
        if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
    }
}

void AngleDTCreateStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 进入闲置模式
    m_tool->setStatus(std::shared_ptr<AngleDTStatus>(new AngleDTIdleStatus(m_tool)));

    // 更新鼠标操作
    if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
}

void AngleDTCreateStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void AngleDTCreateStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void AngleDTCreateStatus::move(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
    if (m_aglow == nullptr) return;

    m_aglow._Object->_visibleMode = true;
    int index = m_aglow._Object->_pointCount;
    if (index > 2 || index < 1)
    {
        index = 0;
    }
    if (index == 1)
    {
        m_aglow._Object->_pPoints[index] = point;
        m_aglow._Object->_pPoints[index + 1] = m_aglow._Object->_pPoints[0];
    }
    else if (index == 2)
    {
        m_aglow._Object->_pPoints[index] = point;
    }

    glm::vec3 d = m_aglow._Object->_pPoints[1] - m_aglow._Object->_pPoints[0];
    glm::vec3 n = glm::normalize(glm::vec3(d.y, -d.x, 0));

    m_aglow._Object->_textPos.x = (m_aglow._Object->_pPoints[index].x + _DefaultLineLength);
    m_aglow._Object->_textPos.y = (m_aglow._Object->_pPoints[index].y);
    m_aglow._Object->_textLinkPos = (m_aglow._Object->_pPoints[index]);

}




void AngleDTCheckedStatus::startWork()
{
    // 重置激活对象
    if (m_angle._Object != nullptr)
    {
        m_angle._Object->_checkedMode = true;
    }
}

void AngleDTCheckedStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    std::list<AngleObjectPtr> list = m_tool->getAngLst();
    for (auto & i : list)
    {
        //重置角度状态为未击中状态
        i._Object->_checkedMode = false;
    }

    // 鼠标是否击中角度
    for (auto & i : list)
    {
        if (isPointOnObject(point, i) == true)
        {
            m_editMode = checkEditMode(point, i, m_checkedPtIdx);
            // 击中角度，进入激活状态
            i._Object->_checkedMode = true;
            m_angle = i;
            m_oldPt = point;
            return;
        }
    }

    // 鼠标未击中角度
    m_tool->setStatus(std::shared_ptr<AngleDTStatus>(new AngleDTIdleStatus(m_tool)));

}

void AngleDTCheckedStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 右键击中当前角度
    if (isPointOnObject(point, m_angle) == true)
    {
        m_angle._Object->_checkedMode = true;
        // 弹窗提示删除
        m_menu->move(ev->globalPos());
        m_menu->show();
    }
    // 未击中当前角度
    else
    {
        m_angle._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<AngleDTStatus>(new AngleDTIdleStatus(m_tool)));
    }
}




void AngleDTCheckedStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_angle == nullptr) return;

    if (m_editMode == EditMode::EM_Point)
    {
        glm::vec3 translateVector = point - m_oldPt;
        m_angle._Object->_pPoints[m_checkedPtIdx] += translateVector;
    }
    else if (m_editMode == EditMode::EM_Line)
    {
        glm::vec3 translateVector = point - m_oldPt;
        for (int i = 0;i < m_angle._Object->_pointCount; i++)
        {
            for (int i = 0;i < _MAX_ANGLE_POINTS;i++)
            {
                m_angle._Object->_pPoints[i] += translateVector;
                for (int j = 0;j < 4;j++)
                {
                    m_angle._Object->_pBoxPoints[i * 4 + j] += translateVector;
                }

            }

            m_angle._Object->_textPos.x = (m_angle._Object->_pPoints[2].x + _DefaultLineLength);
            m_angle._Object->_textPos.y = (m_angle._Object->_pPoints[2].y);
            m_angle._Object->_textLinkPos = (m_angle._Object->_pPoints[2]);
        }
        m_angle._Object->_pPoints[1] += translateVector;
        m_angle._Object->_pBoxPoints[0] += translateVector;
        m_angle._Object->_pBoxPoints[1] += translateVector;
        m_angle._Object->_pBoxPoints[2] += translateVector;
        m_angle._Object->_pBoxPoints[3] += translateVector;
    }
    else if(m_editMode == EditMode::EM_Text)
    {

        glm::vec3  translateVector = point - m_oldPt;
        translateVector.z = 0;
        m_angle._Object->_checkedMode = true;
        m_angle._Object->_textPos += translateVector;
        m_angle._Object->_textRange[0] += translateVector;
        m_angle._Object->_textRange[1] += translateVector;
        m_angle._Object->_textRange[2] += translateVector;
        m_angle._Object->_textRange[3] += translateVector;
    }

    m_oldPt = point;
}

void AngleDTCheckedStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void AngleDTCheckedStatus::deletePress()
{
    m_tool->deleteAngle(m_angle);
}
















void AngleMenu::deleteOneObejct(bool en)
{
    m_aglTool->deleteAngle(m_aglow);
}


