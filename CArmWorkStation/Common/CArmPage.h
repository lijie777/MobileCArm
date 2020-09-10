#pragma once
#include "Common\CArmCMD.h"
#include <QObject>
#include "CArmWorkStationCommon.h"

class CArmPage : public QObject
{

    Q_OBJECT

public:


public:
    CArmPage() { }
    virtual ~CArmPage() { }

    /**
    * @brief 调用该函数，注册功能页对象
    * @param page 功能页对象
    */
    static void registerPage(CArmPage* page);
    
    /**
    * @brief 调用该函数，检查信息广播给已注册的功能页对象
    * @param info 检查信息
    */
    static void notifyStudyInfo(const CP_StudyData& info);

    /**
    * @brief 调用该函数，拍摄信息广播给已注册的功能页对象
    * @param info 拍摄信息
    */
    static void notifySeriesInfo(const CP_SeriesData& info);

    /**
    * @brief 调用该函数，图像信息广播给已注册的功能页对象
    * @param info 图像信息
    * @param data 阅片数据
    */
    static void notifyImageInfo(const CP_ImageData& info);

    /**
    * @brief 调用该函数，阅片信息广播给已注册的功能页对象
    * @param info 阅片信息
    * @param inst 当前阅片实例
    */
    static void notifyReviewInfo(const std::list<CP_ReviewData*>& info, const CP_InstanceData& inst);

protected:

    /**
    * @brief 调用该函数，更新检查信息
    * @param info 检查信息
    */
    virtual void updateStudyInfo(const CP_StudyData& info) { }

    /**
    * @brief 调用该函数，更新拍摄信息
    * @param info 拍摄信息
    */
    virtual void updateSeriesInfo(const CP_SeriesData& info) { }

    /**
    * @brief 调用该函数，更新图像信息
    * @param info 图像信息
    */
    virtual void updateImageInfo(const CP_ImageData& info) { }

    /**
    * @brief 调用该函数，更新阅片信息
    * @param info 阅片信息
    * @param inst 当前阅片实例
    */
    virtual void updateReviewInfo(const std::list<CP_ReviewData*>& info, const CP_InstanceData& inst) { }


protected:

    static std::list<CArmPage*> m_Lst;  //!< 广播列表
};


