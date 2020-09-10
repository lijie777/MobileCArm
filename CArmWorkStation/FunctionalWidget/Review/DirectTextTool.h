#pragma once
#include "TextDrawingTool.h"
#include "CArmDT.h"
#include "BaseView.h"
#include "QMenu"

class DirectTextMenu;
class DirectTextStatus;
class DirectTextTool : public CArmDrawTool
{
public:

    DirectTextTool(int type, std::wstring dirct);
    ~DirectTextTool();

    /**
    * @brief 删除文本
    */
    void deleteText();

    /**
    * @brief 画刷初始化函数
    */
    void initTool();

    /**
    * @brief 处理点数据
    * @param point 点坐标
    * @param callbackData 回调对象
    * @param callback 回调函数
    */
    void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void deletePress();

    std::wstring getDirectString();

    TextObjectPtr getTextObj();

    /**
    * @brief 设置标记是否可见
    * @param en 是否可见
    */
    void setVisible(bool en);

    /**
    * @brief 设置绘制状态
    * @param status 状态编码
    */
    void setStatus(int code);

    /**
    * @brief 设置绘制状态
    * @param status 状态对象
    */
    void setStatus(std::shared_ptr<DirectTextStatus> status);


    /**
    * @brief 添加文本对象
    * @param object 文本对象
    */
    void addTextObject(TextObjectPtr object);

    /**
    * @brief 调用该函数，在指定OpenGL上下文中渲染绘制对象
    *
    * @param MVP 模型视图投影矩阵
    * @param w 渲染屏幕宽度
    * @param h 渲染屏幕高度
    * @param sx 屏幕像素的水平分辨率
    * @param sy 屏幕像素的竖直分辨率
    * @attention 调用该函数前，必须在指定的OpenGL上下文中调用initBrush()函数，否则将出现异常
    */
    void render(glm::mat4 MVP, float w, float h, float sx, float sy);

    /**
    * @brief 设置文本颜色
    * @param r 红色通道，取值范围在[0,1]区间
    * @param g l绿色通道，取值范围在[0,1]区间
    * @param b 蓝色通道，取值范围在[0,1]区间
    * @param a 透明度通道，取值范围在[0,1]区间
    */
    void setTextColor(float r, float g, float b, float a);

    /**
    * @brief 设置文本尺寸
    * @param size 文本尺寸
    */
    void setTextSize(int size);

    /**
    * @brief 设置文本背景颜色
    * @param r 红色通道，取值范围在[0,1]区间
    * @param g l绿色通道，取值范围在[0,1]区间
    * @param b 蓝色通道，取值范围在[0,1]区间
    * @param a 透明度通道，取值范围在[0,1]区间
    */
    void setBackgroundColor(float r, float g, float b, float a);


    /**
    * @brief 设置轮廓线宽度
    * @param width 宽度
    */
    void setOutlineWidth(int width);

    /**
    * @brief 设置文本轮廓线背景颜色
    * @param r 红色通道，取值范围在[0,1]区间
    * @param g l绿色通道，取值范围在[0,1]区间
    * @param b 蓝色通道，取值范围在[0,1]区间
    * @param a 透明度通道，取值范围在[0,1]区间
    */
    void setOutlineColor(float r, float g, float b, float a);

    

protected:



protected:

    TextBrush* m_pTextBrush;        //!< 文本画刷
    TextObjectPtr m_text;           //!< 当前文本实体
    std::shared_ptr<DirectTextStatus> m_pDrawStatus;//!< 绘制状态
    std::wstring m_string;          //!< 方位字符
};


/**
* @brief 文本输入窗口类
*/
class DirectTextMenu : public QMenu
{
    Q_OBJECT

public:
    DirectTextMenu()
        : m_pDeleteOneAc(new QAction(QObject::tr("删除"), this))
    {
        this->addAction(m_pDeleteOneAc);
        this->setFixedSize(70, 32);

        this->setStyleSheet("background:rgba(14,14,14,1);\
            font: 14px Microsoft YaHei;\
            color: #B4C4D9;\
            border:1px solid rgba(74, 82, 90, 1);");

        // 连接信号槽
        connect(m_pDeleteOneAc, SIGNAL(triggered(bool)), this, SLOT(deleteOneObejct(bool)));
    }

    ~DirectTextMenu() {}

    void setTxObj(TextObjectPtr text) { m_text = text; }

    void setTool(std::shared_ptr<DirectTextTool> tool) { m_txTool = tool; }

    protected slots:

    void deleteOneObejct(bool en) { m_txTool->deleteText(); }

protected:

    std::shared_ptr<DirectTextTool> m_txTool;  //!< 绘图工具
    TextObjectPtr m_text;        //!< 文本对象
    QAction* m_pDeleteOneAc;     //!< 删除该文本
};


class DirectTextStatus : public CArmDrawStatus
{
public:
    DirectTextStatus(std::shared_ptr<DirectTextTool> tool, int code) : CArmDrawStatus{ code }, m_tool{ tool } { }
    virtual ~DirectTextStatus() {}

    /**
    * @brief 调用该函数，处理绘图点坐标
    *
    * @param point 点坐标，来自于窗口，取值区间为[-1,+1]
    * @param callbackData 绘图工具回调对象指针
    * @param callback 绘图工具回调函数
    */
    virtual void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void deletePress() {}


    /**
    * @brief 调用该函数，进入状态
    */
    virtual void startWork() {}

    int getCode() { return m_code; }

protected:

    /**
    * @brief 调用该函数，判断点坐标是否位于文本区域
    * @param point 点坐标
    * @param text 文本对象
    */
    bool isPointOnObject(glm::vec3 point, TextObjectPtr text);

protected:
    std::shared_ptr<DirectTextTool> m_tool;     //!< 绘图工具
};


class DirectTextIdleStatus : public DirectTextStatus
{
public:
    DirectTextIdleStatus(std::shared_ptr<DirectTextTool> tool)
        : DirectTextStatus{ tool , _CArmDrawToolIdle }
    { }

    ~DirectTextIdleStatus() {}

    /**
    * @brief 调用该函数，进入状态
    */
    void startWork() override;

    /**
    * @brief 调用该函数，处理绘图点坐标
    *
    * @param point 点坐标，来自于窗口，取值区间为[-1,+1]
    * @param callbackData 绘图工具回调对象指针
    * @param callback 绘图工具回调函数
    */
    void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);


protected:

};



class DirectTextCreateStatus : public DirectTextStatus
{
public:
    DirectTextCreateStatus(std::shared_ptr<DirectTextTool> tool, BaseView* view, IOperationToolPtr nxtOP)
        : DirectTextStatus{ tool , _CArmDrawToolCreate }
        , m_view{ view }
        , m_nxtOP { nxtOP }
    { }

    ~DirectTextCreateStatus() {}

    /**
    * @brief 调用该函数，进入状态
    */
    void startWork() override;

    /**
    * @brief 调用该函数，处理绘图点坐标
    *
    * @param point 点坐标，来自于窗口，取值区间为[-1,+1]
    * @param callbackData 绘图工具回调对象指针
    * @param callback 绘图工具回调函数
    */
    void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);


protected:

    BaseView* m_view;           //!< 绘图窗口
    IOperationToolPtr m_nxtOP;  //!< 绘图结束后的操作工具
};



class DirectTextCheckedStatus : public DirectTextStatus
{
public:
    DirectTextCheckedStatus(TextObjectPtr text, std::shared_ptr<DirectTextTool> tool)
        : DirectTextStatus{ tool , _CArmDrawToolChecked }
        , m_text{ text }
        , m_menu(new DirectTextMenu())
    {
        m_menu->setTool(tool);
        m_menu->setTxObj(text);
    }

    ~DirectTextCheckedStatus()
    {
        delete m_menu;
    }

    /**
    * @brief 调用该函数，进入状态
    */
    void startWork() override;

    /**
    * @brief 调用该函数，处理绘图点坐标
    *
    * @param point 点坐标，来自于窗口，取值区间为[-1,+1]
    * @param callbackData 绘图工具回调对象指针
    * @param callback 绘图工具回调函数
    */
    void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void deletePress();


protected:

    TextObjectPtr m_text;       //!< 文本对象
    glm::vec3 m_oldPt;          //!< 上一次的移动位置
    DirectTextMenu* m_menu;     //!< 菜单弹窗
};


