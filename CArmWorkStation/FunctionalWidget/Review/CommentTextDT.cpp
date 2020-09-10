#include "CommentTextDT.h"



CommentTextDT::CommentTextDT()
    : CArmDrawTool(CArmDTEnum::Comment)
    , m_pDrawStatus(nullptr)
    , m_pTextBrush(new TextBrush())
{
}


CommentTextDT::~CommentTextDT()
{
    delete m_pTextBrush;
}



void CommentTextDT::deleteText(TextObjectPtr obj)
{
    m_txLst.remove_if([obj](TextObjectPtr Object) { return (obj._Object == Object._Object);});
    setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxIdleStatus(std::shared_ptr<CommentTextDT>(this))));
}

void CommentTextDT::deleteAllText()
{
    m_txLst.clear();
}

void CommentTextDT::initTool()
{
    m_pTextBrush->init_resources();
}

std::list<TextObjectPtr> CommentTextDT::getTxLst()
{
    return m_txLst;
}

void CommentTextDT::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_pDrawStatus == nullptr)
    {
        return;
    }
    m_pDrawStatus->leftPress(point, ev, callbackData, callback);
}

void CommentTextDT::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CommentTextDT::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CommentTextDT::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CommentTextDT::deletePress()
{
    if (m_pDrawStatus != nullptr) m_pDrawStatus->deletePress();
}

void CommentTextDT::setVisible(bool en)
{
    for (auto& i : m_txLst)
    {
        if (i._Object == nullptr) continue;

        i._Object->_visibleMode = en;
    }
}

void CommentTextDT::setStatus(int code)
{
    if (code == _CArmDrawToolIdle) setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxIdleStatus(std::shared_ptr<CommentTextDT>(this))));
}



void CommentTextDT::setStatus(std::shared_ptr<CommentTxDTStatus> status)
{
    m_pDrawStatus = status;
    if (m_pDrawStatus != nullptr) m_pDrawStatus->startWork();
}

void CommentTextDT::addTextObject(TextObjectPtr object)
{
    m_txLst.push_back(object);
}


void CommentTextDT::render(glm::mat4 MVP, float w, float h, float sx, float sy)
{

}

void CommentTextDT::setTextColor(float r, float g, float b, float a)
{
    m_pTextBrush->set_color(r, g, b, a);
}

void CommentTextDT::setTextSize(int size)
{
    m_pTextBrush->set_fonts_size(size);
}

void CommentTextDT::setBackgroundColor(float r, float g, float b, float a)
{
    m_pTextBrush->set_background_color(r, g, b, a);
}

void CommentTextDT::setOutlineWidth(int width)
{
}

void CommentTextDT::setOutlineColor(float r, float g, float b, float a)
{
}


bool CommentTxDTStatus::isPointOnObject(glm::vec3 point, TextObjectPtr text)
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

void CommentTxIdleStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getTxLst())
    {
        i._Object->_checkedMode = false;
    }
}

void CommentTxIdleStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    std::list<TextObjectPtr> list = m_tool->getTxLst();
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
            m_tool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxCheckedStatus(i, m_tool)));
            return;
        }
    }

}

void CommentTxIdleStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CommentTxIdleStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CommentTxIdleStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}



void CommentTxCreateStatus::startWork()
{
    // 改变光标样式

    // 重置激活对象
    for (auto & i : m_tool->getTxLst())
    {
        i._Object->_checkedMode = false;
    }
}

void CommentTxCreateStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_box->isVisible())
    {
        if (m_box->text().isEmpty() == false && m_box->rect().contains(ev->globalPos()) == false)
        {
            // 创建文本
            TextObjectPtr text = TextObject::createTextObject();

            // 更新文本内容
            text._Object->_visibleMode = true;
            text._Object->_checkedMode = true;
            text._Object->_pPoints[0] = point;
            text._Object->_textBody = m_box->text().toStdWString();

            // 添加到备注列表
            m_tool->addTextObject(text);

            // 进入编辑模式
            m_tool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxCheckedStatus(text, m_tool)));

            // 更新鼠标操作
            if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);

            // 隐藏弹窗
            m_box->hide();

            return;
        }
    }

    //弹窗文本输入框
    m_box->move(ev->globalPos());
    m_box->show();

}

void CommentTxCreateStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_box->isVisible())
    {
        if (m_box->text().isEmpty() == false && m_box->rect().contains(ev->globalPos()) == false)
        {
            // 创建文本
            TextObjectPtr text = TextObject::createTextObject();

            // 更新文本内容
            text._Object->_visibleMode = true;
            text._Object->_checkedMode = true;
            text._Object->_pPoints[0] = point;
            text._Object->_textBody = m_box->text().toStdWString();

            // 添加到备注列表
            m_tool->addTextObject(text);

            // 进入编辑模式
            m_tool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxCheckedStatus(text, m_tool)));

            // 更新鼠标操作
            if (m_view != nullptr) m_view->setOperationTool(m_nxtOP);

            // 隐藏弹窗
            m_box->hide();
        }
    }
}

void CommentTxCreateStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CommentTxCreateStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}




void CommentTxCheckedStatus::startWork()
{
    // 改变光标样式

    // 激活对象
    m_text._Object->_checkedMode = true;

}

void CommentTxCheckedStatus::leftPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_box->isVisible()) return;

    // 鼠标击中当前文本
    if (isPointOnObject(point, m_text) == true)
    {
        return;
    }

    std::list<TextObjectPtr> list = m_tool->getTxLst();
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
            // 击中文本，进入激活状态
            i._Object->_checkedMode = true;
            m_tool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxCheckedStatus(i, m_tool)));
            return;
        }
    }

    // 鼠标未击中文本
    if (isPointOnObject(point, m_text) == false)
    {
        m_text._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxIdleStatus(m_tool)));
    }
}

void CommentTxCheckedStatus::rightPress(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    // 弹窗可见时隐藏弹窗
    if (m_box->isVisible())
    {
        m_box->hide();
        return;
    }

    // 右键击中当前文本
    if (isPointOnObject(point, m_text) == true)
    {
        m_text._Object->_checkedMode = true;
        // 弹窗提示删除
        m_menu->move(ev->globalPos());
        m_menu->show();
    }
    // 未击中当前文本
    else
    {
        m_text._Object->_checkedMode = false;
        m_tool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxIdleStatus(m_tool)));
    }
}

void CommentTxCheckedStatus::leftMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
    if (m_text == nullptr) return;
    if (m_box->isVisible())
    {
        m_box->hide();
        return;
    }

    glm::vec3 translateVector = point - m_oldPt;
    m_text._Object->_pPoints[0] += translateVector;

    m_oldPt = point;
}

void CommentTxCheckedStatus::rightMove(glm::vec3 point, QMouseEvent* ev, void * callbackData, DrawToolCallback callback)
{
}

void CommentTxCheckedStatus::deletePress()
{
    m_tool->deleteText(m_text);
}







void CommentTextBox::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_Enter)
    {
       QString tx = this->text();
       //文本为空时隐藏窗口
       if (tx.isEmpty())
       {
           this->hide();
       }
       //文本对象为空时创建文本对象
       else if (m_text._Object == nullptr)
       {
           TextObjectPtr obj = TextObject::createTextObject();
           if (obj._Object != nullptr)
           {
               obj._Object->_visibleMode = true;
               obj._Object->_checkedMode = false;
               m_txTool->addTextObject(obj);
               // 进入激活状态
               m_txTool->setStatus(std::shared_ptr<CommentTxDTStatus>(new CommentTxCheckedStatus(obj, m_txTool)));
           }
       }
       
    }
}



void CommentTextMenu::deleteOneObejct(bool en)
{
    m_txTool->deleteText(m_text);
}

