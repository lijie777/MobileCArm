#pragma once
#include "QWidget"
#include "QMenu"
#include "QKeyEvent"
#include "LineDrawingTool.h"
#include "CArmDT.h"
#include "BaseView.h"

class LineMenu;
class LineDTStatus;

class LineDT : public CArmDrawTool
{
public:

    LineDT();
    ~LineDT();

    static int getCheckedPointIndex(glm::vec3* pointEnds, int endsNumber, glm::vec3 point, float distance = 0.04);

    static bool isPointInRect(glm::vec3 corner[4], glm::vec3 point);

    /**
    * @brief 调用该函数，测量线段的外边框
    */
    static void box(LineObjectPtr obj);

    /**
    * @brief 删除直线
    */
    void deleteLine(LineObjectPtr object);

    /**
    * @brief 删除直线
    */
    void deleteAllLine();

    /**
    * @brief 画刷初始化函数
    */
    void initTool();


    /**
    * @brief 获取直线列表
    */
    std::list<LineObjectPtr> getLineLst();

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

    /**
    * @brief 设置标记是否可见
    * @param en 是否可见
    */
    void setVisible(bool en);

    /**
    * @brief 设置绘制状态
    * @param code 状态编码
    */
    void setStatus(int code);

    /**
    * @brief 设置绘制状态
    * @param status 状态对象
    */
    void setStatus(std::shared_ptr<LineDTStatus> status);


    /**
    * @brief 添加直线对象
    * @param object 直线对象
    */
    void addLineObject(LineObjectPtr object);

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
    void draw(glm::mat4 MVP, float w, float h, float sx, float sy);

    /**
    * @brief 设置直线颜色
    * @param r 红色通道，取值范围在[0,1]区间
    * @param g l绿色通道，取值范围在[0,1]区间
    * @param b 蓝色通道，取值范围在[0,1]区间
    * @param a 透明度通道，取值范围在[0,1]区间
    */
    void setLineColor(float r, float g, float b, float a);



protected:

    /**
    * @brief 调用该函数，在指定OpenGL上下文中渲染绘制直线测量对象的夹角线段
    *
    * @param obj 直线测量对象
    * @param MVP 模型视图投影矩阵
    * @attention 调用该函数前，必须在指定的OpenGL上下文中调用initBrush()函数，否则将出现异常
    */
    void draw_substance(LineObjectPtr obj, glm::mat4 MVP);


    /**
    * @brief 调用该函数，在指定OpenGL上下文中渲染绘制直线测量对象的注释标签
    *
    * @param obj 直线测量对象
    * @param MVP 模型视图投影矩阵
    * @param w 渲染屏幕宽度
    * @param h 渲染屏幕高度
    * @param sx 屏幕像素的水平分辨率
    * @param sy 屏幕像素的竖直分辨率
    * @attention 调用该函数前，必须在指定的OpenGL上下文中调用initBrush()函数，否则将出现异常
    */
    void draw_legend(LineObjectPtr obj, glm::mat4 MVP, float w, float h, float sx, float sy);


    /**
    * @brief 调用该函数，在指定OpenGL上下文中渲染绘制直线测量对象的锚点
    *
    * @param obj 直线测量对象
    * @param MVP 模型视图投影矩阵
    * @attention 调用该函数前，必须在指定的OpenGL上下文中调用initBrush()函数，否则将出现异常
    */
    void draw_handle(LineObjectPtr obj, glm::mat4 MVP);


protected:

    glm::vec4 m_color;                 //!< 颜色值
    LineBrush* m_pLineBrush;           //!< 直线画刷
    TextBrush* m_pTextBrush;           //!<文本画刷
    CircleBrush* m_pCircleBrush;       //!<圆形画刷
    std::list<LineObjectPtr> m_lineLst;//!< 当前直线实体
    std::shared_ptr<LineDTStatus> m_pDrawStatus;       //!< 绘制状态
};



/**
* @brief 直线输入窗口类
*/
class LineMenu : public QMenu
{
    Q_OBJECT

public:
    LineMenu()
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

    ~LineMenu() {}

    void setArrObj(LineObjectPtr line) { m_line = line; }

    void setTool(std::shared_ptr<LineDT> tool) { m_lineTool = tool; }

protected slots:
    void deleteOneObejct(bool en);

protected:

    std::shared_ptr<LineDT> m_lineTool;        //!< 绘图工具
    LineObjectPtr m_line;        //!< 直线对象
    QAction* m_pDeleteOneAc;     //!< 删除该直线
};


class LineDTStatus : public CArmDrawStatus
{
public:


    enum EditMode
    {
        EM_Point,   //!< 编辑端点
        EM_Line,    //!< 编辑直线
        EM_Text,    //!< 编辑文本
        EM_None     //!< 不编辑
    };


public:
    LineDTStatus(std::shared_ptr<LineDT> tool, int code) :CArmDrawStatus{ code }, m_tool{ tool } { }
    virtual ~LineDTStatus() {}

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

    virtual void enterPress() {}

    /**
    * @brief 调用该函数，进入状态
    */
    virtual void startWork() {}

    int getCode() { return m_code; }

protected:

    /**
    * @brief 调用该函数，判断点坐标是否位于直线区域
    * @param point 点坐标
    * @param line 直线对象
    */
    static bool isPointOnObject(glm::vec3 point, LineObjectPtr line);

    static EditMode checkEditMode(glm::vec3 point, LineObjectPtr line, int& idx);

protected:
    std::shared_ptr<LineDT> m_tool;     //!< 绘图工具
};


class LineDTCreateStatus : public LineDTStatus
{
public:
    LineDTCreateStatus(std::shared_ptr<LineDT> tool, BaseView* view, IOperationToolPtr op)
        : LineDTStatus{ tool , _CArmDrawToolCreate }
        , m_view{ view }
        , m_nxtOP { op }
    {
    }

    ~LineDTCreateStatus()
    {
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

    void move(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);


protected:

    LineObjectPtr m_line;     //!< 直线对象
    BaseView* m_view;           //!< 绘制窗口
    IOperationToolPtr m_nxtOP;  //!< 创建完成后的鼠标操作
};



class LineDTIdleStatus : public LineDTStatus
{
public:
    LineDTIdleStatus(std::shared_ptr<LineDT> tool)
        : LineDTStatus{ tool , _CArmDrawToolIdle }
    {
    }

    ~LineDTIdleStatus()
    {
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

};



class LineDTCheckedStatus : public LineDTStatus
{
public:
    LineDTCheckedStatus(glm::vec3 pt, LineObjectPtr line, std::shared_ptr<LineDT> tool)
        : LineDTStatus{ tool , _CArmDrawToolChecked }
        , m_line{ line }
        , m_menu(new LineMenu())
        , m_oldPt{ pt }
        , m_editMode(EditMode::EM_Point)
        , m_checkedPtIdx(-1)
    {

        m_menu->setTool(tool);
        m_menu->setArrObj(line);
    }

    ~LineDTCheckedStatus()
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



protected:


    LineObjectPtr m_line;       //!< 直线对象
    LineMenu* m_menu;           //!< 菜单弹窗
    glm::vec3 m_oldPt;          //!< 上一次的移动位置
    EditMode m_editMode;        //!< 编辑模式
    int m_checkedPtIdx;         //!< 被激活的点索引值

};










