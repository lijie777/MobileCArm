#pragma once
#include "QWidget"
#include "QMenu"
#include "QKeyEvent"
#include "AngleDrawingTool.h"
#include "CArmDT.h"
#include "BaseView.h"

class AngleMenu;
class AngleDTStatus;

class AngleDT : public CArmDrawTool
{
public:

    AngleDT();
    ~AngleDT();

    static int getCheckedPointIndex(glm::vec3* pointEnds, int endsNumber, glm::vec3 point, float distance = 0.04);

    static bool isPointInRect(glm::vec3 corner[4], glm::vec3 point);

    /**
    * @brief 调用该函数，测量线段的外边框
    */
   static void box(AngleObjectPtr object);

    /**
    * @brief 删除角度
    */
    void deleteAngle(AngleObjectPtr object);

    /**
    * @brief 删除角度
    */
    void deleteAllAngle();

    /**
    * @brief 画刷初始化函数
    */
    void initTool();


    /**
    * @brief 获取角度列表
    */
    std::list<AngleObjectPtr> getAngLst();

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
    void setStatus(std::shared_ptr<AngleDTStatus> status);


    /**
    * @brief 添加角度对象
    * @param object 角度对象
    */
    void addAngleObject(AngleObjectPtr object);

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
    * @brief 设置角度颜色
    * @param r 红色通道，取值范围在[0,1]区间
    * @param g l绿色通道，取值范围在[0,1]区间
    * @param b 蓝色通道，取值范围在[0,1]区间
    * @param a 透明度通道，取值范围在[0,1]区间
    */
    void setAngleColor(float r, float g, float b, float a);



protected:


    /**
    * @brief 调用该函数，在指定OpenGL上下文中渲染绘制角度对象的夹角线段
    *
    * @param obj 角度对象
    * @param MVP 模型视图投影矩阵
    * @attention 调用该函数前，必须在指定的OpenGL上下文中调用initBrush()函数，否则将出现异常
    */
    void draw_substance(AngleObjectPtr obj, glm::mat4 MVP);


    /**
    * @brief 调用该函数，在指定OpenGL上下文中渲染绘制角度对象的注释标签
    *
    * @param obj 角度对象
    * @param MVP 模型视图投影矩阵
    * @param w 渲染屏幕宽度
    * @param h 渲染屏幕高度
    * @param sx 屏幕像素的水平分辨率
    * @param sy 屏幕像素的竖直分辨率
    * @attention 调用该函数前，必须在指定的OpenGL上下文中调用initBrush()函数，否则将出现异常
    */
    void draw_legend(AngleObjectPtr obj, glm::mat4 MVP, float w, float h, float sx, float sy);


    /**
    * @brief 调用该函数，在指定OpenGL上下文中渲染绘制角度对象的锚点
    *
    * @param obj 角度对象
    * @param MVP 模型视图投影矩阵
    * @attention 调用该函数前，必须在指定的OpenGL上下文中调用initBrush()函数，否则将出现异常
    */
    void draw_handle(AngleObjectPtr obj, glm::mat4 MVP);


protected:

    glm::vec4 m_color;                  //!< 颜色值
    CircleBrush* m_pCircleBrush;        //!< 圆形画刷
    TextBrush* m_pTextBrush;            //!< 文本画刷
    LineBrush* m_pLineBrush;            //!< 线段画刷
    AngleBrush* m_pAngleBrush;          //!< 角度画刷
    std::list<AngleObjectPtr> m_aglLst; //!< 当前角度实体
    std::shared_ptr<AngleDTStatus> m_pDrawStatus;       //!< 绘制状态
};







/**
* @brief 角度输入窗口类
*/
class AngleMenu : public QMenu
{
    Q_OBJECT

public:
    AngleMenu()
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

    ~AngleMenu() {}

    void setArrObj(AngleObjectPtr aglow) { m_aglow = aglow; }

    void setTool(std::shared_ptr<AngleDT> tool) { m_aglTool = tool; }

protected slots:
    void deleteOneObejct(bool en);

protected:

    std::shared_ptr<AngleDT> m_aglTool;        //!< 绘图工具
    AngleObjectPtr m_aglow;      //!< 角度对象
    QAction* m_pDeleteOneAc;     //!< 删除该角度
};


class AngleDTStatus : public CArmDrawStatus
{
public:


    enum EditMode
    {
        EM_Point,   //!< 编辑端点
        EM_Line,    //!< 编辑线段
        EM_Text,    //!< 编辑文本
        EM_None     //!< 不编辑
    };


public:
    AngleDTStatus(std::shared_ptr<AngleDT> tool, int code) :CArmDrawStatus{ code }, m_tool{ tool } { }
    virtual ~AngleDTStatus() {}

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
    * @brief 调用该函数，判断点坐标是否位于角度区域
    * @param point 点坐标
    * @param aglow 角度对象
    */
    static bool isPointOnObject(glm::vec3 point, AngleObjectPtr aglow);

    static EditMode checkEditMode(glm::vec3 point, AngleObjectPtr aglow, int& idx);

protected:
    std::shared_ptr<AngleDT> m_tool;     //!< 绘图工具
};


class AngleDTCreateStatus : public AngleDTStatus
{
public:
    AngleDTCreateStatus(std::shared_ptr<AngleDT> tool, BaseView* view, IOperationToolPtr op)
        : AngleDTStatus{ tool , _CArmDrawToolCreate }
        , m_view{ view }
        , m_nxtOP { op }
    {
    }

    ~AngleDTCreateStatus()
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

    AngleObjectPtr m_aglow;     //!< 角度对象
    BaseView* m_view;           //!< 绘制窗口
    IOperationToolPtr m_nxtOP;  //!< 创建完成后的鼠标操作
};



class AngleDTIdleStatus : public AngleDTStatus
{
public:
    AngleDTIdleStatus(std::shared_ptr<AngleDT> tool)
        : AngleDTStatus{ tool , _CArmDrawToolIdle }
    {
    }

    ~AngleDTIdleStatus()
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



class AngleDTCheckedStatus : public AngleDTStatus
{
public:
    AngleDTCheckedStatus(glm::vec3 pt, AngleObjectPtr aglow, std::shared_ptr<AngleDT> tool)
        : AngleDTStatus{ tool , _CArmDrawToolChecked }
        , m_angle{ aglow }
        , m_menu(new AngleMenu())
        , m_oldPt{ pt }
        , m_editMode(EditMode::EM_Point)
        , m_checkedPtIdx(-1)
    {

        m_menu->setTool(tool);
        m_menu->setArrObj(aglow);
    }

    ~AngleDTCheckedStatus()
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


    AngleObjectPtr m_angle;      //!< 角度对象
    AngleMenu* m_menu;           //!< 菜单弹窗
    glm::vec3 m_oldPt;          //!< 上一次的移动位置
    EditMode m_editMode;        //!< 编辑模式
    int m_checkedPtIdx;         //!< 被激活的点索引值

};










