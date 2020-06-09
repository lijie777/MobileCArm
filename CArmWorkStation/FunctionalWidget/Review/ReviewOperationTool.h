/**
 * @file ReviewOperationTool.h
 * @author 贺兴怡
 * @brief 绘制操作工具类头文件
 */

#pragma once
#include "CArmDT.h"
#include "BaseView.h"

 /**
 * @brief 绘制操作工具类
 * 该类主要对窗口对象进行鼠标操作事件处理
 */
class ReviewOperationToolPtr;
class ReviewOperationTool : public IOperationTool
{
    friend class ReviewOperationToolPtr;
public:

    /**
     * @brief 绘制操作工具类构造函数
     *
     * @param viewer 绘制窗口对象
     */
    ReviewOperationTool(BaseView* viewer);
  
    
    
    /**
    * @brief 绘制操作工具类析构函数
    */
    ~ReviewOperationTool();

    /**
    * @brief 添加阅片操作
    * @param tool 工具
    */
    void addDrawTool(CArmDrawToolPtr tool);

    /**
    * @brief 移除阅片操作
    * @param tool 工具
    */
    void removeDrawTool(CArmDrawToolPtr tool);

    /**
    * @brief 调用该函数，结束绘制操作
    */
    void finishOperation() override;

    
    /**
    * @brief 调用该函数，开始绘制操作
    */
    void startOperation() override;


    /**
    * @brief 单击鼠标右键绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMousePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 单击鼠标左键绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMousePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 双击鼠标右键绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseDoublePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

  
    /**
    * @brief 双击鼠标左键绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseDoublePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

  
    /**
    * @brief 鼠标右键按下并移动绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标左键按下并移动绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;

    /**
    * @brief 鼠标移动绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void MouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;

    
    /**
    * @brief 鼠标右键释放绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseRelease(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标左键释放绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseRelease(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标滚轮滚动绘制事件处理函数
    *
    * @param d 滚轮滚动方向，-1表示向上滚动，1表示向下滚动
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void wheelRolling(int d, glm::vec3 point, QWheelEvent * ev = nullptr) override;


    /**
    * @brief 键盘按下绘制事件处理函数
    *
    * @param ev 鼠标事件对象
    */
    void keyPressed(QKeyEvent * ev = nullptr) override;


    /**
    * @brief 鼠标移出窗口绘制事件处理函数
    *
    * @param ev 鼠标事件对象
    */
    void mouseLeaveView(QEvent* ev = nullptr) override;

protected:


    /**
    * @brief 调用该函数， 更新绘制操作工具状态
    */
    void updateStatus();


    /**
    * @brief 调用该函数， 判断目标点是否在矩形区域内
    *
    * @param pos 目标点位置
    * @return 当目标点在矩形区域内时返回true，否则返回false
    */
    bool pointInTextureRect(glm::vec3 pos);


private:

    std::list<CArmDrawToolPtr> m_lst;   //!< 阅片操作列表
    CArmDrawToolPtr m_currTool;         //!< 当前工具
};





struct ReviewOperationToolPtr
{
    /**
    * @brief 阅片操作智能指针构造函数
    *
    * @param obj 阅片操作对象指针
    */
    ReviewOperationToolPtr(ReviewOperationTool* obj = nullptr);


    /**
    * @brief 阅片操作智能指针构造函数
    *
    * @param ptr 阅片操作对象智能指针
    */
    ReviewOperationToolPtr(const ReviewOperationToolPtr& ptr);


    /**
    * @brief 阅片操作智能指针析构函数
    */
    ~ReviewOperationToolPtr();


    /**
    * @brief 阅片操作智能指针=重载函数
    */
    ReviewOperationToolPtr& operator=(const ReviewOperationToolPtr& ptr);


    /**
    * @brief 阅片操作智能指针!=重载函数
    */
    bool operator!=(const ReviewOperationToolPtr& ptr);


    /**
    * @brief 阅片操作智能指针==重载函数
    */
    bool operator==(const ReviewOperationToolPtr& ptr);


    ReviewOperationTool* _Object;

};