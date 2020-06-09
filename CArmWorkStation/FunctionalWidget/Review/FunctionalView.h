#pragma once
#include "Common\CArmWorkStationProc.h"
#include "ViewToolCreator.h"
#include "CArmBaseView.h"
#include "CArmAnnotation.h"
#include "DirectTextTool.h"
#include "CommentTextDT.h"
#include "ArrowDT.h"
#include "LineDT.h"
#include "AngleDT.h"
#include "CalibDT.h"
#include "ReviewOperationTool.h"
#include "Common\CArmPrinter.h"

/**
* @brief 移动C型臂工作站阅览图像窗口
*/
class FunctionalView : public CArmBaseView
{

public:

    /**
    * @brief 调用该函数，复原图像
    */
    void recover() override;

    /**
    * @brief 调用该函数，加载阅片数据
    * @param data 阅片数据
    */
    void loadReviewData(CP_ReviewData* data) override;

    /**
    * @brief 调用该函数，删除体数据结构
    */
    void deleteCTTexture() override;

public:
    FunctionalView(CArmWorkStationProc* proc, int type, QWidget* p);
    ~FunctionalView();

    /**
    * @brief 调用该函数，获取阅片操作工具
    * @return 阅片操作工具
    */
    IOperationToolPtr getReviewOP();

    /**
    * @brief 调用该函数，获取L标记工具
    * @return L标记工具
    */
    std::shared_ptr<DirectTextTool> getDirectLDT();

    /**
    * @brief 调用该函数，获取A标记工具
    * @return A标记工具
    */
    std::shared_ptr<DirectTextTool> getDirectADT();

    /**
    * @brief 调用该函数，获取H标记工具
    * @return H标记工具
    */
    std::shared_ptr<DirectTextTool> getDirectHDT();

    /**
    * @brief 调用该函数，获取R标记工具
    * @return R标记工具
    */
    std::shared_ptr<DirectTextTool> getDirectRDT();

    /**
    * @brief 调用该函数，获取P标记工具
    * @return P标记工具
    */
    std::shared_ptr<DirectTextTool> getDirectPDT();

    /**
    * @brief 调用该函数，获取F标记工具
    * @return F标记工具
    */
    std::shared_ptr<DirectTextTool> getDirectFDT();

    /**
    * @brief 调用该函数，获取备注标记工具
    * @return 备注标记工具
    */
    std::shared_ptr<CommentTextDT> getCommentDT();

    /**
    * @brief 调用该函数，获取箭头标记工具
    * @return 箭头标记工具
    */
    std::shared_ptr<ArrowDT> getArrowDT();

    /**
    * @brief 调用该函数，获取直线测量工具
    * @return 直线测量工具
    */
    std::shared_ptr<LineDT> getLineDT();

    /**
    * @brief 调用该函数，获取角度测量工具
    * @return 角度测量工具
    */
    std::shared_ptr<AngleDT> getAngleDT();

    /**
    * @brief 调用该函数，获取图像校准工具
    * @return 涂销校准工具
    */
    std::shared_ptr<CalibDT> getCalibDT();

    /**
    * @brief 调用该函数，显示或隐藏窗口标记
    */
    void gobeVisibleOrHided();

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
    * @brief 图像载入事件处理函数
    */
    void notifyTextureLoaded() override;

    void paintGL();

    /**
    * @brief 调用该函数，初始化阅片操作工具
    */
    void initReviewOP();

private:


    CP_ReviewData* m_reviewData;    //!< 阅片参数

    ReviewOperationToolPtr m_reviewOP;  //!< 阅片操作工具
    CArmAnnotationPtr m_pAnnoTool;//!< 四角信息显示工具
    std::shared_ptr<DirectTextTool> m_DirectL;  //!< L方向标记
    std::shared_ptr<DirectTextTool> m_DirectA;  //!< A方向标记
    std::shared_ptr<DirectTextTool> m_DirectH;  //!< H方向标记
    std::shared_ptr<DirectTextTool> m_DirectR;  //!< R方向标记
    std::shared_ptr<DirectTextTool> m_DirectP;  //!< P方向标记
    std::shared_ptr<DirectTextTool> m_DirectF;  //!< F方向标记
    std::shared_ptr<CommentTextDT> m_CommentDT; //!< 备注工具
    std::shared_ptr<ArrowDT> m_ArrowDT;         //!< 角度工具
    std::shared_ptr<LineDT> m_LineDT;           //!< 直线测量工具
    std::shared_ptr<AngleDT> m_AngleDT;         //!< 角度测量工具
    std::shared_ptr<CalibDT> m_CalibDT;         //!< 图像校准工具

    bool m_bMarkShowMode;         //!< 标记显示模式
};

