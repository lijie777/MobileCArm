/**
 * @file TextOperationTool.h
 * @author 贺兴怡
 * @brief 文本绘制操作工具类头文件
 */

#pragma once
#include "IStatusTool.h"
#include <qt\QtWidgets\QAbstractButton>
#include "CommentTextDT.h"


 /**
 * @brief 文本绘制操作工具类
 * 该类主要对窗口对象进行鼠标操作事件处理
 */
class TextOperationTool : public IOperationTool
{

public:

    /**
     * @brief 文本绘制操作工具类构造函数
     *
     * @param drawTool 绘制工具对象
     * @param viewer 绘制窗口对象
     * @param cursor 鼠标样式工具对象
     * @param btn 绘制操作工具按键对象
     */
    TextOperationTool(std::shared_ptr<CommentTextDT> drawTool, BaseView* viewer, QAbstractButton* btn = nullptr, IOperationToolPtr nxtOP = nullptr);
  
    
    
    /**
    * @brief 文本绘制操作工具类析构函数
    */
    ~TextOperationTool();




    /**
    * @brief 调用该函数，结束文本绘制操作
    */
    void finishOperation() override;

    
    /**
    * @brief 调用该函数，开始文本绘制操作
    */
    void startOperation() override;


    /**
    * @brief 单击鼠标右键文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMousePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 单击鼠标左键文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMousePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 双击鼠标右键文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseDoublePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

  
    /**
    * @brief 双击鼠标左键文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseDoublePressed(glm::vec3 point, QMouseEvent * ev = nullptr) override;

  
    /**
    * @brief 鼠标右键按下并移动文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标左键按下并移动文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;

    /**
    * @brief 鼠标移动文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void MouseMoving(glm::vec3 point, QMouseEvent * ev = nullptr) override;

    
    /**
    * @brief 鼠标右键释放文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void rightMouseRelease(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标左键释放文本绘制事件处理函数
    *
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void leftMouseRelease(glm::vec3 point, QMouseEvent * ev = nullptr) override;


    /**
    * @brief 鼠标滚轮滚动文本绘制事件处理函数
    *
    * @param d 滚轮滚动方向，-1表示向上滚动，1表示向下滚动
    * @param point 鼠标在窗口坐标系中的位置，取值区间为[-1,+1]
    * @param ev 鼠标事件对象
    */
    void wheelRolling(int d, glm::vec3 point, QWheelEvent * ev = nullptr) override;


    /**
    * @brief 键盘按下文本绘制事件处理函数
    *
    * @param ev 鼠标事件对象
    */
    void keyPressed(QKeyEvent * ev = nullptr) override;


    /**
    * @brief 鼠标移出窗口文本绘制事件处理函数
    *
    * @param ev 鼠标事件对象
    */
    void mouseLeaveView(QEvent* ev = nullptr) override;

protected:


    /**
    * @brief 调用该函数， 更新文本绘制操作工具状态
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

    /// 控制按键对象
    QAbstractButton* m_pCtrlButton;

    /// 文本绘制工具对象
    std::shared_ptr<CommentTextDT> m_pDrawTool;

    /// 下一鼠标操作
    IOperationToolPtr m_nxtOP;
};





/**
* @brief 文本绘制工具回调处理类
*/
class TextDataCallback : public IDrawCallbackContext
{
public:

    /**
    * @brief 文本绘制工具回调处理类构造函数
    */
    TextDataCallback();


    /**
    * @brief 文本绘制工具回调处理类析构函数
    */
    ~TextDataCallback();

    /**
    * @brief 调用该函数，设置窗口对象
    *
    * @param pBaseView 窗口对象
    */
    void setView(BaseView* pBaseView);


    /**
    * @brief 调用该函数，进行文本绘制工具回调处理
    *
    * @param drawData 绘制对象智能指针
    * @param status 当前绘制状态
    */
    void drawCallback(MeasureObjectPtr drawData, int status);


    /**
    * @brief 调用该函数，设置文本绘制操作工具对象，该对象用于执行真实的回调处理
    *
    * @param tool 文本绘制操作工具对象
    */
    void setOperationTool(TextOperationTool* tool);

protected:

    /// 窗口对象
    BaseView* mBaseView;

    /// 文本绘制操作工具对象
    TextOperationTool* m_pOperationTool;
};