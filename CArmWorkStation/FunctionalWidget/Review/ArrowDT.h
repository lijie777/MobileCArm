#pragma once
#include "QWidget"
#include "QMenu"
#include "QKeyEvent"
#include "ArrowDrawingTool.h"
#include "CArmDT.h"
#include "BaseView.h"

class ArrowMenu;
class ArrowDTStatus;

class ArrowDT : public CArmDrawTool
{
public:

    ArrowDT();
    ~ArrowDT();

    static int getCheckedPointIndex(glm::vec3* pointEnds, int endsNumber, glm::vec3 point, float distance = 0.04);

    static bool isPointInRect(glm::vec3 corner[4], glm::vec3 point);

    /**
    * @brief 删除箭头
    */
    void deleteArrow(ArrowObjectPtr object);

    /**
    * @brief 删除箭头
    */
    void deleteAllArrow();

    /**
    * @brief 画刷初始化函数
    */
    void initTool();


    /**
    * @brief 获取箭头列表
    */
    std::list<ArrowObjectPtr> getArrLst();

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
    void setStatus(std::shared_ptr<ArrowDTStatus> status);


    /**
    * @brief 添加箭头对象
    * @param object 箭头对象
    */
    void addArrowObject(ArrowObjectPtr object);

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
    * @brief 设置箭头颜色
    * @param r 红色通道，取值范围在[0,1]区间
    * @param g l绿色通道，取值范围在[0,1]区间
    * @param b 蓝色通道，取值范围在[0,1]区间
    * @param a 透明度通道，取值范围在[0,1]区间
    */
    void setArrowColor(float r, float g, float b, float a);



protected:



protected:

    ArrowBrush* m_pArrowBrush;          //!< 箭头画刷
    std::list<ArrowObjectPtr> m_arrLst; //!< 当前箭头实体
    std::shared_ptr<ArrowDTStatus> m_pDrawStatus;       //!< 绘制状态
};







/**
* @brief 箭头输入窗口类
*/
class ArrowMenu : public QMenu
{
    Q_OBJECT

public:
    ArrowMenu()
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

    ~ArrowMenu() {}

    void setArrObj(ArrowObjectPtr arrow) { m_arrow = arrow; }

    void setTool(std::shared_ptr<ArrowDT> tool) { m_arrTool = tool; }

protected slots:
    void deleteOneObejct(bool en);

protected:

    std::shared_ptr<ArrowDT> m_arrTool;        //!< 绘图工具
    ArrowObjectPtr m_arrow;      //!< 箭头对象
    QAction* m_pDeleteOneAc;     //!< 删除该箭头
};


class ArrowDTStatus : public CArmDrawStatus
{
public:


    enum EditMode
    {
        EM_Point,
        EM_Line,
        EM_None
    };


public:
    ArrowDTStatus(std::shared_ptr<ArrowDT> tool, int code) :CArmDrawStatus{ code }, m_tool{ tool } { }
    virtual ~ArrowDTStatus() {}

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
    * @brief 调用该函数，判断点坐标是否位于箭头区域
    * @param point 点坐标
    * @param arrow 箭头对象
    */
    static bool isPointOnObject(glm::vec3 point, ArrowObjectPtr arrow);

    static EditMode checkEditMode(glm::vec3 point, ArrowObjectPtr arrow, int& idx);

protected:
    std::shared_ptr<ArrowDT> m_tool;     //!< 绘图工具
};


class ArrowDTCreateStatus : public ArrowDTStatus
{
public:
    ArrowDTCreateStatus(std::shared_ptr<ArrowDT> tool, BaseView* view, IOperationToolPtr op)
        : ArrowDTStatus{ tool , _CArmDrawToolCreate }
        , m_view{ view }
        , m_nxtOP { op }
    {
    }

    ~ArrowDTCreateStatus()
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

    ArrowObjectPtr m_arrow;     //!< 箭头对象
    BaseView* m_view;           //!< 绘制窗口
    IOperationToolPtr m_nxtOP;  //!< 创建完成后的鼠标操作
};



class ArrowDTIdleStatus : public ArrowDTStatus
{
public:
    ArrowDTIdleStatus(std::shared_ptr<ArrowDT> tool)
        : ArrowDTStatus{ tool , _CArmDrawToolIdle }
    {
    }

    ~ArrowDTIdleStatus()
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



class ArrowDTCheckedStatus : public ArrowDTStatus
{
public:
    ArrowDTCheckedStatus(glm::vec3 pt, ArrowObjectPtr arrow, std::shared_ptr<ArrowDT> tool)
        : ArrowDTStatus{ tool , _CArmDrawToolChecked }
        , m_arrow{ arrow }
        , m_menu(new ArrowMenu())
        , m_oldPt{ pt }
        , m_editMode(EditMode::EM_Point)
        , m_checkedPtIdx(-1)
    {

        m_menu->setTool(tool);
        m_menu->setArrObj(arrow);
    }

    ~ArrowDTCheckedStatus()
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


    ArrowObjectPtr m_arrow;       //!< 箭头对象
    ArrowMenu* m_menu;           //!< 菜单弹窗
    glm::vec3 m_oldPt;          //!< 上一次的移动位置
    EditMode m_editMode;        //!< 编辑模式
    int m_checkedPtIdx;         //!< 被激活的点索引值

};










