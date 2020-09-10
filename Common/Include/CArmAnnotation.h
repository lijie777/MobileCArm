#pragma once
#include "AnnotationBase.h"
#include "CArmCommon.h"

/**
* @brief 移动C型臂阅片窗口四角信息显示工具类
*/
class CArmAnnotation : public AnnotationBase
{
    friend class CArmAnnotationPtr;

public:
    CArmAnnotation();
    ~CArmAnnotation();

    /**
    * @brief 创建四角信息对象
    * @return 四角信息对象
    */
    static CArmAnnotationPtr createTool();

    /**
    * @brief 显示工具初始化函数
    */
    void initTool();

    /**
    * @brief 调用该函数，设置四角信息
    * @param data 图像信息
    * @param param 图像参数
    */
    void setImageData(const CP_ImageData& data, const CArmImgParam& param);

 

    /**
    * @brief 调用该函数，绘制四角信息到窗口
    *
    * @param window_width 屏幕宽度，单位位px
    * @param window_height 屏幕高度，单位位px
    */
    void display(int window_width, int window_height) override;

protected:


    /**
    * @brief 调用该函数，更新四角信息
    *
    */
    void updateAnno();

private:

    TextBrush* m_pTextBrush;                     //!< 文本画刷
    PictureBrush* m_PictureBrush;                //!< 图片画刷

    CP_ImageData m_imageInfo;                   //!< 当前阅片实例
    CArmImgParam m_imgParam;                    //!< 图像显示参数

    AnnotationPictureData* m_brightPic;          //!< 明亮度图标
    AnnotationPictureData* m_contrastPic;        //!< 对比度图标
    AnnotationPictureData* m_sharpenPic;         //!< 锐度图标
    AnnotationPictureData* m_rotatePic;          //!< 旋转图标
    AnnotationPictureData* m_metalPic;           //!< 金属工具图标
    AnnotationPictureData* m_denoisePic;         //!< 降噪图标
    AnnotationPictureData* m_reversePic;         //!< 反转图标
    AnnotationPictureData* m_fliphPic;           //!< 水平翻转图标
    AnnotationPictureData* m_flipvPic;           //!< 竖直翻转图标
};



class CArmAnnotationPtr
{
public:


    /**
    * @brief 显示工具初始化函数
    */
    void initTool();

    /**
    * @brief 调用该函数，设置四角信息
    * @param data 图像信息
    * @param param 图像参数
    */
    void setImageData(const CP_ImageData& data, const CArmImgParam& param);


    /**
    * @brief 调用该函数，获取四角信息对象
    *
    * @param obj 四角信息对象指针
    */
    AnnotationBasePtr getObject() { return _Object; }

    /**
    * @brief 四角信息智能指针构造函数
    *
    * @param obj 四角信息对象指针
    */
    CArmAnnotationPtr(CArmAnnotation* obj = nullptr);


    /**
    * @brief 四角信息智能指针构造函数
    *
    * @param ptr 四角信息对象智能指针
    */
    CArmAnnotationPtr(const CArmAnnotationPtr& ptr);


    /**
    * @brief 四角信息智能指针析构函数
    */
    ~CArmAnnotationPtr();


    /**
    * @brief 四角信息智能指针=重载函数
    */
    CArmAnnotationPtr& operator=(const CArmAnnotationPtr& ptr);


    /**
    * @brief 四角信息智能指针!=重载函数
    */
    bool operator!=(const CArmAnnotationPtr& ptr);


    /**
    * @brief 四角信息智能指针==重载函数
    */
    bool operator==(const CArmAnnotationPtr& ptr);

private:

    /// 四角信息对象
    CArmAnnotation* _Object;
};