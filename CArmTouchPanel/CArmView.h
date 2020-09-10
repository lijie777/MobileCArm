#pragma once

/**
* @file CArmView.h
* @brief 小C图像显示View
* @author 路旺明
*/
#include "CArmBaseView.h"
#include "ViewToolCreator.h"
#include "CArmAnnotation.h"
#include <QGesTure>
#include <QGesTureEvent>
#include <QPinchGesture>
#include <QSwipeGesture>

class CArmView : public CArmBaseView
{
public:
    CArmView(int type, QWidget *parent = nullptr);
    ~CArmView();

protected:

    /**
    * @brief 初始化窗口大小
    *
    * @param width 宽度
    * @param height 高度
    */
     void resizeGL(int width, int height)override;

     /**
     * @brief 调用该函数，绘制窗口
     *
     * @param p 投影矩阵
     * @param mv 模型视图矩阵
     * @param window_width 渲染屏幕宽度
     * @param window_height 渲染屏幕高度
     */
     void draw(glm::mat4 p, glm::mat4 mv, int window_width, int window_height)override;

     /**
     * @brief 调用该函数，初始化Opengl绘图环境
     */
     void glInitBase() override;

     /**
     * @brief 图像载入事件处理函数
     */
     void notifyTextureLoaded() override;

     /**
     * @brief 调用该函数，删除体数据结构
     */
     void deleteCTTexture() override;

     void paintGL();

     bool event(QEvent *event);

     void mouseMoveEvent(QMouseEvent *event);

     void mousePressEvent(QMouseEvent *event);

public:

    void loadReviewData(CP_ReviewData* imageData) override;
    /**
    * @brief 调用该接口，设置图像的对比度
    *
    * @param value 对比度变化值
    */
    void setContrast(int value);

    /**
    * @brief 调用该接口，设置图像的亮度
    *
    * @param value 亮度变化值
    */
    void setBrightness(int value);

    /**
    * @brief 调用该接口，设置图像的锐度
    *
    * @param gear   锐度等级
    * @param value  锐度值
    */
    void setSharpen(int gear, int value);

    /**
    * @brief 调用该接口，对图像进行降噪
    *
    * @param gear   降噪等级
    * @param value  降噪值
    */
    void setDenoise(int gear, int value);

    /**
    * @brief 调用该接口，对图像进行金属处理
    *
    * @param value 金属处理等级
    */
    void setMetalGear(int gearValue, bool state);

    /**
    * @brief 调用该接口，对图像进行智能金属校正
    *
    */
    void capacityMetal(int gearValue, bool state);

    /**
    * @brief 调用该接口，对图像进行旋转操作
    *
    * @param rotateValue 旋转的角度
    */
    void setRotateValue(int rotateValue);

    /**
    * @brief 调用该接口，对图像进行负片操作
    *
    * @param state 是否进行负片操作
    */
    void setNegativeValue(bool state);

    /**
    * @brief 调用该函数，窗口水平翻转
    *
    * @param en 启用/关闭功能
    */
    void flipViewHorizontal(bool en) override;

    /**
    * @brief 调用该函数，窗口竖直翻转
    *
    * @param en 启用/关闭功能
    */
    void flipViewVertical(bool en) override;

    /**
    * @brief 调用该函数，设置新的患者数据要来了
    *
    * @param void
    */
    void updatePatient(void);
private:

    /**
    * @brief 调用该接口，对图像进行重置操作
    *
    */
    void resetProjectMatrix();


    void translate(QPointF point);

private:
    ///默认投影矩阵
    glm::mat4  m_normalProjectMatrix;

    CArmAnnotationPtr   m_pAnnoTool;

    CP_ReviewData*      m_ImageDataInfo;

    bool                m_bIsTwoPoint;

    qreal               m_currentStepScaleFactor;

    qreal               m_scaleFactor;

    qreal               m_horizontalOffset;

    qreal               m_verticalOffset;

    QPoint             m_lastMousePos;

    bool               m_bIsLoadImageData;
};
