#pragma once


#include "IViewingTool.h"
#include "CArmCommon.h"
#define _CArmImgVT     0                    //!< 图像处理工具类型

#define _CArmBright_Origin      1.0     //!< 原始图像明亮度
#define _CArmBright_MinValue    0.0     //!< 明亮度最小值（最暗）
#define _CArmBright_MaxValue    4.0     //!< 明亮度最大值（最亮）
#define _CArmContrast_Origin    1.0     //!< 原始图像对比度
#define _CArmContrast_MinValue  0.0     //!< 图像对比度最小值（对比最弱）
#define _CArmContrast_MaxValue  3.0     //!< 图像对比度最大值（对比最强）
#define _CArmSharpen_Origin     1.0     //!< 原始图像锐化度
#define _CArmSharpen_MinValue   0.0     //!< 图像锐化度最小值（最模糊）
#define _CArmMaxValue_Contrast  3.0     //!< 图像锐化度最大值（最增强）


/**
* @brief 移动C型臂工作站通用功能函数
*/
class CArmVT : public IViewingTool
{
public:
    CArmVT(int type);
    ~CArmVT();

    /**
    * @brief 调用该函数，调节显示参数
    *
    * @param value 调节量
    */
    virtual void adjustValue(float value, std::string name) {}


    /**
    * @brief 调用该函数，设置显示参数
    *
    * @param value 显示参数
    */
    virtual void setValue(float value, std::string name) {};


    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    virtual void disposeViewer(BaseView* view) {};

};



/**
* @brief 移动C型臂工作站锐化工具
*/
class CArmSharpenVT : public CArmVT
{
public:

    CArmSharpenVT(const CArmSharpenVTObject& param) : CArmVT{ CArmVTEnum::VT_Sharpen }, m_param( param ) { }
    ~CArmSharpenVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);

protected:

    CArmSharpenVTObject m_param; //!< 锐化参数
};


/**
* @brief 移动C型臂工作站降噪工具
*/
class CArmDenoiseVT : public CArmVT
{
public:

    CArmDenoiseVT(const CArmDenoiseVTObject& param) : CArmVT{ CArmVTEnum::VT_Denoise }, m_param( param ) { }
    ~CArmDenoiseVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);

protected:

    CArmDenoiseVTObject m_param; //!< 降噪参数
};




/**
* @brief 移动C型臂工作站金属校正工具
*/
class CArmMetalCalibVT : public CArmVT
{
public:

    CArmMetalCalibVT(const CArmMetalCalibVTObject& param) : CArmVT{ CArmVTEnum::VT_MetalCalib }, m_param( param ) { }
    ~CArmMetalCalibVT() { }


    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);


protected:

    CArmMetalCalibVTObject m_param; //!< 金属校正参数
};


/**
* @brief 移动C型臂工作站明亮度调节工具
*/
class CArmBrightVT : public CArmVT
{
public:

    CArmBrightVT(float value) : CArmVT{ CArmVTEnum::VT_Bright }, m_param { value } { }
    ~CArmBrightVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);

protected:

    float m_param; //!< 明亮度参数
};


/**
* @brief 移动C型臂工作站对比度调节工具
*/
class CArmContrastVT : public CArmVT
{
public:

    CArmContrastVT(float value) : CArmVT{ CArmVTEnum::VT_Contrast }, m_param{ value } { }
    ~CArmContrastVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);

protected:

    float m_param; //!< 对比度参数
};



/**
* @brief 图像水平翻转工具
*/
class CArmFlipHVT : public CArmVT
{
public:

    CArmFlipHVT(bool en) : CArmVT{ CArmVTEnum::VT_FlipH }, m_param{ en } { }
    ~CArmFlipHVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);


protected:

    bool m_param; //!< 水平翻转参数
};


/**
* @brief 图像竖直翻转工具
*/
class CArmFlipVVT : public CArmVT
{
public:

    CArmFlipVVT(bool en) : CArmVT{ CArmVTEnum::VT_FlipV }, m_param{ en } { }
    ~CArmFlipVVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);


protected:

    bool m_param; //!< 竖直翻转参数
};


/**
* @brief 图像负片工具
*/
class CArmNegativeVT : public CArmVT
{
public:

    CArmNegativeVT(bool en) : CArmVT{ CArmVTEnum::VT_Negative }, m_param{ en } { }
    ~CArmNegativeVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);


protected:

    bool m_param; //!< 负片参数
};


/**
* @brief 图像旋转工具
*/
class CArmRotateVT : public CArmVT
{
public:

    CArmRotateVT(int en) : CArmVT{ CArmVTEnum::VT_Rotate }, m_param{ en } { }
    ~CArmRotateVT() { }

    /**
    * @brief 调用该函数，显示工具对窗口进行处理
    *
    * @param view 显示窗口对象
    */
    void disposeViewer(BaseView* view);


protected:

    int m_param; //!< 旋转参数
};


/**
* @brief 移动C型臂工作站通用功能函数
*/
class CArmImgVT
{
public:

    CArmImgVT(BaseView* view) : m_pViewer{ view } { }
    ~CArmImgVT()  { }

    /**
    * @brief 调用该函数，设置锐化参数
    * @param value 锐化参数
    */
    void setSharpenParam(CArmSharpenVTObject value);

    /**
    * @brief 调用该函数，设置降噪参数
    * @param value 降噪参数
    */
    void setDenoiseParam(CArmDenoiseVTObject value);

    /**
    * @brief 调用该函数，设置金属校正参数
    * @param value 金属校正参数
    */
    void setMetalCalibParam(CArmMetalCalibVTObject value);

    /**
    * @brief 调用该函数，设置明亮度参数
    * @param value 明亮度参数
    */
    void updateBright(int value);

    /**
    * @brief 调用该函数，设置对比度参数
    * @param value 对比度参数
    */
    void updateContrast(int value);

    /**
    * @brief 调用该函数，开启/关闭水平翻转
    * @param en 开启/关闭
    */
    void enableFlipH(bool en);

    /**
    * @brief 调用该函数，开启/关闭竖直翻转
    * @param en 开启/关闭
    */
    void enableFlipV(bool en);

    /**
    * @brief 调用该函数，开启/关闭负片
    * @param en 开启/关闭
    */
    void setNegative(bool en);

    /**
    * @brief 调用该函数，设置旋转参数
    * @param value 旋转参数
    */
    void setRotateParam(int value);

    CArmImgParam getImageParam();

protected:

    /**
    * @brief 调用该函数，进行图像处理
    */
    void processImage();

private:

    CArmImgParam m_param;       //!< 图像处理参数
    BaseView* m_pViewer;        //!< 处理窗口
};