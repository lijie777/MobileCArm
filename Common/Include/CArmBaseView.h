#pragma once

#include "PageBase.h"
#include "BaseView.h"
#include "CArmVT.h"

/**
* @brief 移动C型臂工作站窗口基类
*/
class CArmBaseView : public BaseView
{
public:
    CArmBaseView(int type, QWidget* parent = nullptr) : BaseView{ type, parent }, m_pImgVT{new CArmImgVT(this)} {}
    virtual ~CArmBaseView() 
    {
        delete m_pImgVT;
    }

    /**
    * @brief 调用该函数，加载阅片数据
    * @param data 阅片数据
    */
    virtual void loadReviewData(CP_ReviewData* data){ }

    /**
    * @brief 调用该函数，获取阅片数据
    * @return data 阅片数据
    */
    virtual CP_ReviewData getReviewData() { return CP_ReviewData(); }

    /**
    * @brief 调用该函数，获取图像处理工具实例
    * @return 图像处理工具实例
    */
    CArmImgVT* getImageVTInst() { return m_pImgVT; }

    /**
    * @brief 调用该函数，复原图像
    */
    virtual void recover() { }

protected:

    CArmImgVT* m_pImgVT;            //!< 图像处理工具
};



/**
* @brief 移动C型臂功能页窗口接口类
* 该类主要负责管理阅片功能相关的若干显示窗口对象
*/
class CArmBasePage : public PageBase
{
public:
    CArmBasePage(QWidget* parent = nullptr, int type = 0, std::string name = "") : PageBase(type, name, parent) {}
    ~CArmBasePage() {}

    /**
    * @brief 调用该函数，启用阅片工具
    * @param tool 阅片工具
    */
    virtual void enableTool(IReadToolPtr tool) { }

    /**
    * @brief 调用该函数，保存阅片记录
    */
    virtual void saveToFile() { }
};
