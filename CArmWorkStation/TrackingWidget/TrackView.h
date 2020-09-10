#pragma once
#include "ViewToolCreator.h"
#include "CArmBaseView.h"
#include "CArmAnnotation.h"


/**
* @brief 移动C型臂工作站实时图像窗口
*/
class TrackView : public CArmBaseView
{

public:
    TrackView(int type, QWidget* p);
    ~TrackView();

    /**
    * @brief 调用该函数，加载阅片数据
    * @param data 阅片数据
    */
    void loadReviewData(CP_ReviewData* data) override;

protected:

    /**
    * @brief 调用该函数，绘制窗口
    *
    * @param p 投影矩阵
    * @param mv 模型视图矩阵
    * @param window_width 渲染屏幕宽度
    * @param window_height 渲染屏幕高度
    */
    void draw(glm::mat4 p, glm::mat4 mv, int window_width, int window_height) override;

    /**
    * @brief 调用该函数，初始化Opengl绘图环境
    */
    void glInitBase() override;

    /**
    * @brief 调用该函数，删除体数据结构
    */
    void deleteCTTexture() override;

    void paintGL();

    void resizeGL(int width, int height);

    void resetProjectMatrix();

private:


    CArmAnnotationPtr m_pAnnoTool;            //!< 四角信息显示工具
    CP_ReviewData* m_reviewData;              //!< 阅片数据
    glm::mat4 m_normalProjectMatrix; //!< 默认投影矩阵
};

