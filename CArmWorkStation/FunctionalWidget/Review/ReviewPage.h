#pragma once
#include "Common\CArmWorkStationProc.h"
#include "Common\CArmPage.h"
#include "ReviewPageWidget.h"
#include "ReviewToolWidget.h"
#include "CArmDragButton.h"

class ReviewPage : public CArmPage
{
    Q_OBJECT

public:
    ReviewPage(CArmWorkStationProc* proc, QWidget* parent = nullptr);
    ~ReviewPage();

    QWidget* getPageWidget();

    QWidget* getToolWidget();

    /**
    * @brief 调用该函数，更新患者
    * @param info 患者信息
    */
    void updatePatientInfo(const CP_PatientData& info);

    /**
    * @brief 调用该函数，清空患者信息
    */
    void clearPatientInfo();


protected:

    void initLogic();

    /**
    * @brief 调用该函数，更新阅片信息，图像来自于左屏
    * @param info 阅片信息
    * @param inst 当前阅片实例
    */
    void updateReviewInfo(const std::list<CP_ReviewData*>& info, const CP_InstanceData& inst) override;

    /**
    * @brief 调用该函数，更新拍摄信息，图像来自于患者目录
    * @param info 拍摄信息
    */
    void updateSeriesInfo(const CP_SeriesData& info) override;

private:
    
    ReviewPageWidget* m_page;
    ReviewToolWidget* m_tool;

};


