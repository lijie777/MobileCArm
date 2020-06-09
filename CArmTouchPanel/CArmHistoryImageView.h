#pragma once
#include "BaseView.h"
#include "../Common/Include/Config.h"
#include "CArmBaseView.h"

class CArmHistoryImageView : public CArmBaseView
{
    Q_OBJECT
public:
    CArmHistoryImageView(int type, QWidget *parent = Q_NULLPTR);
    ~CArmHistoryImageView();

    void loadReviewData(CP_ReviewData* imageData) override;

    CP_ReviewData getReviewData() override;
     
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

    void paintGL() override;

    void resetProjectMatrix();


private:
    glm::mat4   m_normalProjectMatrix;

    CP_ReviewData*   m_ReviewData;
};