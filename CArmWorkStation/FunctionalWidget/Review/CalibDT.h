#pragma once
#include "Common\CArmWorkStationProc.h"
#include "QDialog"
#include "QKeyEvent"
#include "LineDT.h"
#include "AngleDT.h"
#include "CArmDT.h"
#include "CArmCommon.h"
#include "BaseView.h"
#include "ui_ImgCalibDialog.h"

class CalibDTStatus;

class CalibDT : public CArmDrawTool
{
public:

    CalibDT(std::shared_ptr<LineDT> tool, std::shared_ptr<AngleDT> angDT, CArmWorkStationProc* proc)
        : CArmDrawTool(CArmDTEnum::Calib)
        , m_pDrawStatus(nullptr)
        , m_tool(tool)
        , m_angDT(angDT)
        , m_proc(proc)
    {

    }
    ~CalibDT(){ }

    static int getCheckedPointIndex(glm::vec3* pointEnds, int endsNumber, glm::vec3 point, float distance = 0.04);

    static bool isPointInRect(glm::vec3 corner[4], glm::vec3 point);

    /**
    * @brief 处理点数据
    * @param point 点坐标
    * @param callbackData 回调对象
    * @param callback 回调函数
    */
    void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void leftPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);
    
    void leftMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    void rightMove(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr);

    /**
    * @brief 设置绘制状态
    * @param status 状态对象
    */
    void setStatus(std::shared_ptr<CalibDTStatus> status);


    /**
    * @brief 添加直线对象
    * @param object 直线对象
    */
    void addLineObject(LineObjectPtr object);



    std::shared_ptr<LineDT> getLineTool() { return m_tool; }

    /**
    * @brief 校准图像
    * @param object 直线对象
    */
    void calibImage(LineObjectPtr object);

    /**
    * @brief 加载图像和纹理
    * @param image 图像数据
    * @param texture 纹理数据
    */
    void loadImage(const CP_ImageData& image, TextureObject3DPtr texture);

protected:

    CArmWorkStationProc* m_proc;        //!< 通讯接口
    std::shared_ptr<LineDT> m_tool;     //!< 绘图工具
    std::shared_ptr<AngleDT> m_angDT;   //!< 角度工具
    TextureObject3DPtr m_tx;            //!< 纹理对象
    CP_ImageData m_image;               //!< 图像信息
    std::shared_ptr<CalibDTStatus> m_pDrawStatus;       //!< 绘制状态
};










class CalibDTStatus : public CArmDrawStatus
{

public:
    CalibDTStatus(std::shared_ptr<CalibDT> tool, int code) :CArmDrawStatus{ code }, m_tool{ tool } { }
    virtual ~CalibDTStatus() {}

    /**
    * @brief 调用该函数，处理绘图点坐标
    *
    * @param point 点坐标，来自于窗口，取值区间为[-1,+1]
    * @param callbackData 绘图工具回调对象指针
    * @param callback 绘图工具回调函数
    */
    virtual void leftPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void rightPress(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr) {}

    virtual void leftPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr){}

    virtual void rightPressDouble(glm::vec3 point, QMouseEvent* ev, void* callbackData = nullptr, DrawToolCallback callback = nullptr){}

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
    * @brief 调用该函数，判断点坐标是否位于直线区域
    * @param point 点坐标
    * @param line 直线对象
    */
    static bool isPointOnObject(glm::vec3 point, LineObjectPtr line);


protected:
    std::shared_ptr<CalibDT> m_tool;     //!< 绘图工具
};


class CalibDTCreateStatus : public CalibDTStatus
{
public:
    CalibDTCreateStatus(std::shared_ptr<CalibDT> tool, BaseView* view, IOperationToolPtr op)
        : CalibDTStatus{ tool , _CArmDrawToolCreate }
        , m_view{ view }
        , m_nxtOP { op }
    {
    }

    ~CalibDTCreateStatus()
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

    LineObjectPtr m_line;     //!< 直线对象
    BaseView* m_view;           //!< 绘制窗口
    IOperationToolPtr m_nxtOP;  //!< 创建完成后的鼠标操作
};



class CalibDTIdleStatus : public CalibDTStatus
{
public:
    CalibDTIdleStatus(std::shared_ptr<CalibDT> tool, BaseView* view, IOperationToolPtr op)
        : CalibDTStatus{ tool , _CArmDrawToolIdle }
        , m_nxtOP(op)
    {
    }

    ~CalibDTIdleStatus()
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

private:

    IOperationToolPtr m_nxtOP;  //!< 创建完成后的鼠标操作
    BaseView* m_view;           //!< 操作窗口
};





/**
* @brief 调用该函数，进入状态
*/
class ImgCalibWidget : public QDialog
{
    Q_OBJECT

public:
    ImgCalibWidget(QWidget *parent = 0) : QDialog{parent}
    {
        ui.setupUi(this);

        QAction* matricAc = new QAction(ui.lengthEdit);
        matricAc->setText("mm");
        ui.lengthEdit->addAction(matricAc, QLineEdit::TrailingPosition);

        connect(ui.close_btn, SIGNAL(clicked()), this, SLOT(closePressed()));
        connect(ui.cancelBtn, SIGNAL(clicked()), this, SLOT(closePressed()));
        connect(ui.okBtn, SIGNAL(clicked()), this, SLOT(okPressed()));

    }
    ~ImgCalibWidget() { }

    void setLineMatric(float mm) { ui.lengthEdit->setText(QString::number(mm)); }
    float getCalibResult() { return ui.lengthEdit->text().toFloat(); }

protected:

    void closePressed() { this->reject(); }
    void okPressed() { this->accept(); }
private:
    Ui::ImgCalibDialog ui;
};









