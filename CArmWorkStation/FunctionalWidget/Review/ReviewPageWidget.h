#pragma once

#include <QtWidgets/QFrame>
#include "ui_ReviewPageWidget.h"
#include "FunctionalView.h"
#include "IReadTool.h"
#include "CArmBaseView.h"

class ReviewPageWidget : public CArmBasePage
{
    Q_OBJECT

public:
    ReviewPageWidget(CArmWorkStationProc* proc, QWidget *parent = 0);
    ~ReviewPageWidget();

    /**
    * @brief 调用该函数，启用阅片工具
    * @param tool 阅片工具
    */
    void enableTool(IReadToolPtr tool) override;

    /**
    * @brief 调用该函数，保存阅片记录
    */
    void saveToFile() override;

    /**
    * @brief 调用该函数，更新患者
    * @param info 患者信息
    */
    void updatePatientInfo(const CP_PatientData& info);

    /**
    * @brief 调用该函数，清空患者信息
    */
    void clearPatientInfo();

    /**
    * @brief 调用该函数，清空阅片信息
    */
    void clearReviewInfo();

    /**
    * @brief 调用该函数，更新阅片信息（图像来自于左屏）
    * @param info 阅片信息
    */
    void loadReviewData(CP_ReviewData* info);

    /**
    * @brief 调用该函数，加载图像（图像来自于数据服务）
    * @param lstTx 图像数据
    */
    void loadNewScanData(const std::list<NewScanData> &lstTx);

private:

    IImage2DTextureImportorComponentPtr m_pTextureImportor;     //!< 图像纹理加载工具
    Ui::ReviewPageWidget ui;            //!< 界面布局
    FunctionalView* m_pImageViewer;     //!< 图像窗口
    CP_ReviewData* m_currReview;        //!< 正在显示的阅片数据
    std::list<CP_ReviewData*> m_reviewData;  //!< 阅片数据列表
};


