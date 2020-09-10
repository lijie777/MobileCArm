/**
 * @file DrawOperationTool.h
 * @author 贺兴怡
 * @brief 绘制窗口操作工具类头文件
 */
#pragma once
#include "CArmDT.h"
#include "BaseView.h"
#include <qt\QtWidgets\QAbstractButton>





/**
* @brief 绘制窗口操作工具类
* 该类主要对窗口对象进行鼠标绘制操作事件处理
*/

class DrawOperationTool : public IOperationTool
{

public:

    /**
     * @brief 绘制窗口操作工具构造函数
     *
     * @param drawTool 绘制工具对象
     * @param viewer 绘制窗口对象
     * @param cursor 鼠标样式工具对象
     * @param btn 操作工具控制按键对象
     */
    DrawOperationTool(CArmDrawToolPtr drawTool, BaseView* viewer, QAbstractButton* btn = nullptr);


    /**
    * @brief 绘制窗口操作工具析构函数
    */
    ~DrawOperationTool();
    
    /**
    * @brief 调用该函数，结束绘制窗口操作
    */
    void finishOperation() override;

    
    /**
    * @brief 调用该函数，开始绘制窗口操作
    */
    void startOperation() override;

    

    /**
    * @brief 单击鼠标右键绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMousePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

    

    /**
    * @brief 单击鼠标左键绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMousePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

    

    /**
    * @brief 双击鼠标右键绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseDoublePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

   
    /**
    * @brief 双击鼠标左键绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseDoublePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

   
    /**
    * @brief 鼠标右键按下并移动绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标左键按下并移动绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标移动绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void MouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;

    
    /**
    * @brief 鼠标右键释放绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseRelease(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标左键释放绘制窗口事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseRelease(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标滚轮滚动绘制窗口事件处理函数
    *
    * @param d 滚轮滚动方向，-1表示向上滚动，1表示向下滚动
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void wheelRolling(int d, glm::vec3 point, QWheelEvent * ev = nullptr) override;


    /**
    * @brief 键盘按下绘制窗口事件处理函数
    *
    * @param ev 鼠标事件对象
    */
    void keyPressed(QKeyEvent * ev = nullptr) override;


    /**
    * @brief 鼠标移出窗口绘制窗口事件处理函数
    *
    * @param ev 鼠标事件对象
    */
    virtual void mouseLeaveView(QEvent* ev = nullptr) override;


protected:


    /**
    * @brief 调用该函数， 更新绘制窗口操作工具状态
    */
    void updateStatus();


    /**
     * @brief 调用该函数，判断目标点是否在矩形区域内
     *
     * @param pos 目标点
     * @return 当目标点在矩形区域内时返回true，否则返回false
     */
    bool pointInTextureRect(glm::vec3 pos);

    /**
    * @brief 调用该函数，判断目标点是否在矩形区域内
    *
    * @param pos 目标点
    * @return 当目标点在矩形区域内时返回true，否则返回false
    */
    bool pointInWindowRect(glm::vec3 pos);






protected:


    /// 控制按键对象
    QAbstractButton* m_pCtrlButton;

    /// 绘制工具对象
    CArmDrawToolPtr m_pDrawTool;

};



