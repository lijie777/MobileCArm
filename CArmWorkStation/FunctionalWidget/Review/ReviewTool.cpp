#include <WinSock2.h>
#include "ReviewTool.h"
#include "FunctionalView.h"
#include "DrawOperationTool.h"
#include "TextOperationTool.h"
#include "DirTxOperationTool.h"

IOperationCursorTool* createMeasureCursorTool(IDrawingTool::TOOL_TYPE type)
{
    IOperationCursorTool* tool = new IOperationCursorTool();
    return tool;
}



/*************************************************************
* class   MagnifyTool   放大镜工具
**************************************************************/
IReadToolPtr MagnifyTool::create(CArmBasePage* page, QAbstractButton* btn)
{
    return new MagnifyTool(page, btn);
}

// 启动工具
void MagnifyTool::startWork()
{
    IStatusReceiverPtr statusReceiver = IStatusReceiver::createTool(0);
    for (auto & i : m_pReadViewerList)
    {
        // 获取绘图工具
        IDrawingToolPtr tool = i->getDrawingTool(IDrawingTool::TOOL_TYPE::TT_MAGNIFY);
        tool.setEnable(true);

        // 获取鼠标光标工具
        IOperationCursorTool* cursorTool = createMeasureCursorTool(IDrawingTool::TOOL_TYPE::TT_MAGNIFY);



        // 创建鼠标操作工具


    }

}





/*************************************************************
* class   ZoomTool   缩放工具
**************************************************************/
IReadToolPtr ZoomTool::create(CArmBasePage* page, QAbstractButton* btn)
{
    return new ZoomTool(page, btn);
}

// 启动工具
void ZoomTool::startWork()
{
    IStatusReceiverPtr statusReceiver = IStatusReceiver::createTool(0);

    for (auto & i : m_pReadViewerList)
    {
        // 获取绘图工具
        IViewingToolPtr tool = i->getViewingTool(IViewingTool::VIEWTOOL_TYPE::VT_ZOOM);
        tool.setEnable(true);

        // 获取鼠标光标工具
        IOperationCursorTool* cursorTool = new IOperationCursorTool();
        cursorTool->addMouseStyle(DRAWTOOL_ON, ":/ReadingProcess/Resource/cursor/zoom.png");

    }
}




/*************************************************************
* class   HideTool   隐藏工具
**************************************************************/
IReadToolPtr HideTool::create()
{
    return new HideTool();
}

// 启动工具
void HideTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        view->gobeVisibleOrHided();
        i->setOperationTool(nullptr);
        i->updateGL();
    }
}



/*************************************************************
* class   RecoverTool   复原工具
**************************************************************/
IReadToolPtr RecoverTool::create()
{
    return new RecoverTool();
}

// 启动工具
void RecoverTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        i->recover();
        i->setOperationTool(nullptr);
        i->updateGL();
    }
}


/*************************************************************
* class   DeleteTool   删除标记工具
**************************************************************/
IReadToolPtr DeleteTool::create()
{
    return new DeleteTool();
}

// 启动工具
void DeleteTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        view->getDirectADT()->deleteText();
        view->getDirectPDT()->deleteText();
        view->getDirectLDT()->deleteText();
        view->getDirectRDT()->deleteText();
        view->getDirectHDT()->deleteText();
        view->getDirectFDT()->deleteText();
        view->getAngleDT()->deleteAllAngle();
        view->getArrowDT()->deleteAllArrow();
        view->getCommentDT()->deleteAllText();
        view->getLineDT()->deleteAllLine();

        i->setOperationTool(nullptr);
        i->updateGL();
    }
}




/*************************************************************
* class   BrightTool   明亮度工具
**************************************************************/
//创建工具
IReadToolPtr BrightTool::create(CArmBasePage* page,float value, QAbstractButton* btn)
{
    return new BrightTool(page, value, btn);
}

// 启动工具
void BrightTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        imgVT->updateBright(m_value);
    }

}




/*************************************************************
* class   ContrastTool   对比度工具
**************************************************************/
//创建工具
IReadToolPtr ContrastTool::create(CArmBasePage* page, float value, QAbstractButton* btn)
{
    return new ContrastTool(page, value, btn);
}

// 启动工具
void ContrastTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        imgVT->updateContrast(m_value);
    }
}







/*************************************************************
* class   NegativeTool   图像反转工具
**************************************************************/
//创建工具
IReadToolPtr NegativeTool::create(CArmBasePage* page, bool en)
{
    return new NegativeTool(page, en);
}


// 启动工具
void NegativeTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        imgVT->setNegative(m_param);
        view->setOperationTool(nullptr);
    }
}





/*************************************************************
* class   RotateTool   旋转工具
**************************************************************/
//创建工具
IReadToolPtr RotateTool::create(CArmBasePage* page, int angle)
{
    return new RotateTool(page, angle);
}


// 启动工具
void RotateTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        view->setOperationTool(nullptr);

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        imgVT->setRotateParam(m_angle);
    }
}




/*************************************************************
* class   FlipTool   翻转工具
**************************************************************/
//创建工具
IReadToolPtr FlipTool::create(CArmBasePage* page, int type, bool en)
{
    return new FlipTool(page, type, en);
}


// 启动工具
void FlipTool::startWork()
{

    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        i->setOperationTool(nullptr);

        if (m_mirrorType == MT_HORIZONTAL)
        {
            // 水平翻转
            imgVT->enableFlipH(m_param);
        }
        else if (m_mirrorType == MT_VERTICAL)
        {
            // 竖直翻转
            imgVT->enableFlipV(m_param);
        }

    }
}



/*************************************************************
* class   CommentTextTool   文本工具
**************************************************************/
IReadToolPtr CommentTextTool::create(QAbstractButton* btn)
{
    return new CommentTextTool(btn);
}

// 启动工具
void CommentTextTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        // 获取绘图工具
        std::shared_ptr<CommentTextDT> tool = view->getCommentDT();

        // 获取鼠标操作工具
        IOperationToolPtr reviewOP = view->getReviewOP();

        // 创建鼠标操作工具
        TextOperationTool* operationTool = new TextOperationTool(tool, i, m_pCtrlButton, reviewOP);

        i->setOperationTool(operationTool);
    }
}




/*************************************************************
* class   CloseTool   关闭阅片工具
**************************************************************/
//创建工具
IReadToolPtr CloseTool::create()
{
    return new CloseTool();
}

// 启动工具
void CloseTool::startWork()
{

    for (auto & i : m_pReadViewerList)
    {
        i->setOperationTool(nullptr);
    }
}

/*************************************************************
* class   DirTxMarkTool   方位标记工具
**************************************************************/
void DirTxMarkTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        // 获取绘图工具
        std::shared_ptr<DirectTextTool> tool;
        if (m_type == DirL)      tool = view->getDirectLDT();
        else if (m_type == DirA) tool = view->getDirectADT();
        else if (m_type == DirH) tool = view->getDirectHDT();
        else if (m_type == DirR) tool = view->getDirectRDT();
        else if (m_type == DirP) tool = view->getDirectPDT();
        else if (m_type == DirF) tool = view->getDirectFDT();

        // 创建鼠标操作工具
        DirTxOperationTool* operationTool = new DirTxOperationTool(tool, i, m_pCtrlButton);

        i->setOperationTool(operationTool);

    }
}

/*************************************************************
* class   SharpenTool   锐化工具
**************************************************************/
void SharpenTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        CArmBaseView* view = static_cast<CArmBaseView*>(i);
        if (view == nullptr) continue;

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        imgVT->setSharpenParam(CArmSharpenVTObject{(float) m_value, m_mode });

        i->setOperationTool(nullptr);
    }
}


/*************************************************************
* class   DenoiseTool   降噪工具
**************************************************************/
void DenoiseTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        CArmBaseView* view = static_cast<CArmBaseView*>(i);
        if (view == nullptr) continue;

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        imgVT->setDenoiseParam(CArmDenoiseVTObject{ (float)m_value, m_mode });

        i->setOperationTool(nullptr);
    }
}


/*************************************************************
* class   MetalCalibTool   金属校正工具
**************************************************************/
void MetalCalibTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        CArmBaseView* view = static_cast<CArmBaseView*>(i);
        if (view == nullptr) continue;

        CArmImgVT* imgVT = view->getImageVTInst();
        if (imgVT == nullptr) continue;

        imgVT->setMetalCalibParam(CArmMetalCalibVTObject{ (float)m_value, m_mode});

        i->setOperationTool(nullptr);
    }
}


/*************************************************************
* class   ArrowMarkTool   箭头标记工具
**************************************************************/
void ArrowMarkTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        std::shared_ptr<ArrowDT> arrowDT = view->getArrowDT();
        DrawOperationTool* op = new DrawOperationTool(arrowDT.get(),view,m_pCtrlButton);
        view->setOperationTool(op);
    }
}

/*************************************************************
* class   LineMeasureTool   直线测量工具
**************************************************************/
void LineMeasureTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        std::shared_ptr<LineDT> lineDT = view->getLineDT();
        DrawOperationTool* op = new DrawOperationTool(lineDT.get(), view, m_pCtrlButton);
        view->setOperationTool(op);
    }
}

/*************************************************************
* class   AngleMeasureTool   角度测量工具
**************************************************************/
void AngleMeasureTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        std::shared_ptr<AngleDT> angleDT = view->getAngleDT();
        DrawOperationTool* op = new DrawOperationTool(angleDT.get(), view, m_pCtrlButton);
        view->setOperationTool(op);
    }
}

/*************************************************************
* class   ImgCalibTool   图像校准工具
**************************************************************/
void ImgCalibTool::startWork()
{
    for (auto & i : m_pReadViewerList)
    {
        FunctionalView* view = static_cast<FunctionalView*>(i);
        if (view == nullptr) continue;

        std::shared_ptr<CalibDT> calibDT = view->getCalibDT();
        DrawOperationTool* op = new DrawOperationTool(calibDT.get(), view, m_pCtrlButton);
        view->setOperationTool(op);

        calibDT->setStatus(std::shared_ptr<CalibDTStatus>(new CalibDTIdleStatus(calibDT, view, view->getReviewOP())));
    }
}
