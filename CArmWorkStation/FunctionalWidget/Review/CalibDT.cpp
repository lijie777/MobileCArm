#include "CalibDT.h"


#define _DefaultLineLength 0.1f


int CalibDT::getCheckedPointIndex(glm::vec3 * pointEnds, int endsNumber, glm::vec3 point, float distance)
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

bool CalibDT::isPointInRect(glm::vec3 corner[4], glm::vec3 point)
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




void CalibDT::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftPress(point, ev, callbackData, callback);
}

void CalibDT::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CalibDT::leftPressDouble(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
}

void CalibDT::rightPressDouble(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
{
}

void CalibDT::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftMove(point, ev, callbackData, callback);
}

void CalibDT::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}





void CalibDT::setStatus(std::shared_ptr<CalibDTStatus> status)
{

    m_pDrawStatus = status;
    if (m_pDrawStatus != nullptr) m_pDrawStatus->startWork();
}

void CalibDT::addLineObject(LineObjectPtr object)
{
   
}

void CalibDT::calibImage(LineObjectPtr line)
{
    if (line == nullptr) return;
    if (m_tx == nullptr) return;
    if (m_proc == nullptr) return;

    ImgCalibWidget dlg;
    dlg.setLineMatric(line._Object->_linePhysicalLength);
    if (dlg.exec() == QDialog::Accepted)
    {
        // 获取校准结果
        float calibRt = dlg.getCalibResult();

        // 校准计算
        int imageWidth = m_tx._Object->width;
        float imagePhyWidth = imageWidth * m_tx._Object->spacingInWidth;
        float measureLength = line._Object->_linePhysicalLength;
        // 计算误差
        float imageCheckWidth = calibRt * imagePhyWidth / measureLength;
        // 计算分辨率
        float pixelSpacing = imageCheckWidth / imageWidth;

        //更新像素分辨率值
        m_tx._Object->spacingInWidth = pixelSpacing;
        m_tx._Object->spacingInHeight = pixelSpacing;
        m_tx._Object->spacingInDepth = pixelSpacing;
        m_image._pixelSpacingX = pixelSpacing;
        m_image._pixelSpacingY = pixelSpacing;

        // 校正所有线段的长度
        IMetricMeasureTool* tool = DrawToolApi::createMeasureTool(line._Object);
        if (tool != nullptr)
        {
            for (auto& i : m_tool->getLineLst())
            {
                tool->measure(i._Object, m_tx);
            }
            delete tool;
        }

        AngleObjectPtr ang = m_angDT->getAngLst().front();
        IMetricMeasureTool* angTool = DrawToolApi::createMeasureTool(ang._Object);
        if (angTool != nullptr)
        {
            for (auto& i : m_angDT->getAngLst())
            {
                angTool->measure(i._Object, m_tx);
            }
            delete angTool;
        }

        //保存到数据库

    }
}

void CalibDT::loadImage(const CP_ImageData & image, TextureObject3DPtr texture)
{
    m_image = image;
    m_tx = texture;
}


bool CalibDTStatus::isPointOnObject(glm::vec3 point, LineObjectPtr obj)
{
    if (obj == nullptr) return false;

    int head_is_hit = -1;
    glm::vec3* ptLst = obj._Object->_pPoints;
    head_is_hit = CalibDT::getCheckedPointIndex(ptLst, _MAX_LINE_POINTS, point, obj._Object->_threshDistance);

    // 判断是否击中端点
    if (head_is_hit != -1)
    {
        return true;
    }

    // 判断是否击中线段
    if (CalibDT::isPointInRect(obj._Object->_pBoxPoints, point) == true)
    {
        return true;
    }

    // 判断是否击中标签
    if (CalibDT::isPointInRect(obj._Object->_textRange, point) == true)
    {
        return true;
    }

    return false;
}


void CalibDTIdleStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getLineTool()->getLineLst())
    {
        i._Object->_checkedMode = false;
    }
}

void CalibDTIdleStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    std::list<LineObjectPtr> list = m_tool->getLineTool()->getLineLst();
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

            // 弹出校准窗
            m_tool->calibImage(i);
            m_view->setOperationTool(m_nxtOP);
            
            return;
        }
    }

    m_tool->setStatus(std::shared_ptr<CalibDTStatus>(new CalibDTCreateStatus(m_tool, m_view, m_nxtOP)));
}

void CalibDTIdleStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CalibDTIdleStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CalibDTIdleStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}



void CalibDTCreateStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getLineTool()->getLineLst())
    {
        i._Object->_checkedMode = false;
    }
}

void CalibDTCreateStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
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

        // 弹出校准窗
        m_tool->calibImage(m_line);

    }
}

void CalibDTCreateStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 更新鼠标操作
    if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);
}


void CalibDTCreateStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CalibDTCreateStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CalibDTCreateStatus::move(glm::vec3 point, QMouseEvent * ev, void * callbackData, DrawToolCallback callback)
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





