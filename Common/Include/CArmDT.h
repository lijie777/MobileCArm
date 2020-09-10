#pragma once
#include "IDrawingTool.h"
#include "QMouseEvent"

#define _CArmDrawToolIdle       0
#define _CArmDrawToolCreate     1
#define _CArmDrawToolChecked    2


/**
* @brief 绘图工具枚举值
*/
enum CArmDTEnum
{
    DirectL,    //!< 方向L标记
    DirectR,    //!< 方向R标记
    DirectA,    //!< 方向A标记
    DirectP,    //!< 方向P标记
    DirectH,    //!< 方向H标记
    DirectF,    //!< 方向F标记
    Arrow,      //!< 箭头标记
    Comment,    //!< 备注标记
    Line,       //!< 直线标记
    Angle,      //!< 角度标记
    Calib       //!< 图像校准
};



/**
* @brief 方向标记对象数据结构
*/
struct CArmDirectDTObject
{
    PositionXMLStruct _points;
};

/**
* @brief 箭头标记对象数据结构
*/
struct CArmArrowDTObject
{
    std::list<PositionXMLStruct> _points;
};

/**
* @brief 备注标记对象数据结构
*/
struct CArmCommentDTObject
{
    PositionXMLStruct _point;
    std::string _text;
};

/**
* @brief 直线标记对象数据结构
*/
struct CArmLineDTObject
{
    std::list<PositionXMLStruct> _point;
    PositionXMLStruct   _mmPos;
};

/**
* @brief 角度标记对象数据结构
*/
struct CArmAngleDTObject
{
    std::list<PositionXMLStruct> _point;
    PositionXMLStruct   _anglePos;
};


/**
* @brief 移动C型臂标记对象数据结构
*/
struct CArmMarkObject
{
    CArmDirectDTObject _directL;     //!< L方位标记
    CArmDirectDTObject _directR;     //!< R方位标记
    CArmDirectDTObject _directA;     //!< A方位标记
    CArmDirectDTObject _directF;     //!< F方位标记
    CArmDirectDTObject _directH;     //!< H方位标记
    CArmDirectDTObject _directP;     //!< P方位标记
    std::list<CArmArrowDTObject> _arrow;        //!< 箭头标记
    std::list<CArmCommentDTObject> _comment;    //!< 备注标记
    std::list<CArmLineDTObject> _line;          //!< 直线标记
    std::list<CArmAngleDTObject> _angle;        //!< 角度标记
};


class CArmDrawStatus;
class CArmDrawTool : public IDrawingTool
{
    friend class CArmDrawToolPtr;

public:
    CArmDrawTool(int type);
    ~CArmDrawTool();

    /**
    * @brief 处理点数据
    * @param point 点坐标
    * @param callbackData 回调对象
    * @param callback 回调函数
    */
    virtual void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void leftPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr){}

    virtual void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr){}

    virtual void move(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void deletePress(){}

    virtual int getStatusCode() { return -1; }

    /**
    * @brief 设置绘制状态
    * @param status 状态对象
    */
    virtual void setStatus(int statusCode) {}
};

class CArmDrawToolPtr
{

public:

    void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void move(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void deletePress();

    int getStatusCode();

    void setStatus(int statusCode);


    /**
    * @brief 绘图工具智能指针构造函数
    *
    * @param obj 绘图工具对象指针
    */
    CArmDrawToolPtr(CArmDrawTool* obj = nullptr);


    /**
    * @brief 绘图工具智能指针构造函数
    *
    * @param ptr 绘图工具对象智能指针
    */
    CArmDrawToolPtr(const CArmDrawToolPtr& ptr);


    /**
    * @brief 绘图工具智能指针析构函数
    */
    ~CArmDrawToolPtr();


    /**
    * @brief 绘图工具智能指针=重载函数
    */
    CArmDrawToolPtr& operator=(const CArmDrawToolPtr& ptr);


    /**
    * @brief 绘图工具智能指针!=重载函数
    */
    bool operator!=(const CArmDrawToolPtr& ptr);


    /**
    * @brief 绘图工具智能指针==重载函数
    */
    bool operator==(const CArmDrawToolPtr& ptr);

protected:

    CArmDrawTool* _Object;

};



class CArmDrawStatus
{
public:

    CArmDrawStatus(int code) : m_code{ code } {}
    virtual ~CArmDrawStatus() {}

    /**
    * @brief 调用该函数，处理绘图点坐标
    *
    * @param point 点坐标，来自于窗口，取值区间为[-1,+1]
    * @param callbackData 绘图工具回调对象指针
    * @param callback 绘图工具回调函数
    */
    virtual void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void leftPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void move(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void deletePress() {}

    virtual void enterPress() {}

    /**
    * @brief 调用该函数，进入状态
    */
    virtual void startWork() {}

protected:
    const int m_code;           //!< 状态编码
};


