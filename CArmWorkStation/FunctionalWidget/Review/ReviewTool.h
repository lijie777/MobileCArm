#pragma once
#include "IReadTool.h"
#include <QAbstractButton>
#include "CArmVT.h"
#include "CArmBaseView.h"

/**
* @brief 移动C型臂阅片工具接口类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class CArmReadTool : public IReadTool
{
public:

    CArmReadTool(CArmBasePage* page) : m_page(page) { }
    ~CArmReadTool() { }

    /**
     * @brief 调用该函数，设置阅片工具的操作窗口对象
     *
     * @param view 窗口对象
     */
    virtual void setViewer(BaseView* view) { }


    /**
    * @brief 调用该函数，对显示窗口对象进行相关的工具处理
    */
    virtual void startWork() { }

protected:

    CArmBasePage* m_page;   //!< 阅片功能页
};



/**
* @brief 方位标记阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class DirTxMarkTool :public CArmReadTool
{
public:
    enum DirType
    {
        DirL,
        DirA,
        DirH,
        DirR,
        DirP,
        DirF
    };

public:

    /**
    * @brief 调用该函数，创建测量阅片工具对象
    *
    * @param type 测量工具类型
    * @param btn 测量工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(DirType type, QAbstractButton* btn = nullptr) { return new DirTxMarkTool(type, btn); }


    /**
    * @brief 调用该函数，启用测量工具
    */
    void startWork();



protected:

    /**
    * @brief 测量阅片工具类构造函数
    *
    * @param type 测量工具类型
    * @param btn 工具控制按键
    */
    DirTxMarkTool(DirType type, QAbstractButton* btn = nullptr) : CArmReadTool{ nullptr }, m_type{ type }, m_pCtrlButton{ btn } { }


    /**
    * @brief 测量阅片工具类析构函数
    */
    ~DirTxMarkTool() { }


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;

    /// 测量类型
    DirType m_type;
};





/**
* @brief 放大镜阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class MagnifyTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建放大镜阅片工具对象
    *
    * @param type 放大镜工具类型
    * @param btn 放大镜工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, QAbstractButton* btn = nullptr);


    /**
    * @brief 调用该函数，启用放大镜工具
    */
    void startWork();


protected:

    /**
    * @brief 放大镜阅片工具类构造函数
    *
    * @param type 放大镜工具类型
    * @param btn 工具控制按键
    */
    MagnifyTool(CArmBasePage* page, QAbstractButton* btn = nullptr) : CArmReadTool(page), m_pCtrlButton{btn} { }


    /**
    * @brief 放大镜阅片工具类析构函数
    */
    ~MagnifyTool() { }


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;
};



/**
* @brief 平移缩放阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class ZoomTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建平移缩放阅片工具对象
    *
    * @param btn 平移缩放工具控制按键对象
    * @param receiver 平移缩放导航数据接收工具
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, QAbstractButton* btn);


    /**
    * @brief 调用该函数，启用平移缩放工具
    */
    void startWork();

protected:

    /**
    * @brief 平移缩放阅片工具类构造函数
    *
    * @param btn 工具控制按键
    * @param receiver 平移缩放导航数据接收工具
    */
    ZoomTool(CArmBasePage* page, QAbstractButton* btn) :CArmReadTool(page), m_pCtrlButton(btn) { }

    /**
    * @brief 平移缩放阅片工具类析构函数
    */
    ~ZoomTool(){ }


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;
};


/**
* @brief 隐藏阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class HideTool :public CArmReadTool
{
public:


    /**
    * @brief 调用该函数，创建隐藏阅片工具对象
    *
    * @param en 隐藏标志
    * @return 阅片工具对象
    */
    static IReadToolPtr create();


    /**
    * @brief 调用该函数，启用隐藏工具
    */
    void startWork();


protected:

    /**
    * @brief 隐藏阅片工具类构造函数
    */
    HideTool(): CArmReadTool(nullptr) {  }


    /**
    * @brief 隐藏阅片工具类析构函数
    */
    ~HideTool(){ }


protected:

};


/**
* @brief 重置阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class RecoverTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建重置阅片工具对象
    *
    * @return 阅片工具对象
    */
    static IReadToolPtr create();


    /**
    * @brief 调用该函数，启用重置工具
    */
    void startWork();

protected:

    /**
    * @brief 重置阅片工具类构造函数
    */
    RecoverTool() : CArmReadTool(nullptr){ }

    /**
    * @brief 重置阅片工具类析构函数
    */
    ~RecoverTool(){ }

};



/**
* @brief 删除绘制阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class DeleteTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建删除绘制阅片工具对象
    *
    * @return 阅片工具对象
    */
    static IReadToolPtr create();


    /**
    * @brief 调用该函数，启用删除绘制工具
    */
    void startWork();

protected:

    /**
    * @brief 删除绘制阅片工具类构造函数
    */
    DeleteTool(): CArmReadTool(nullptr) { }


    /**
    * @brief 删除绘制阅片工具类析构函数
    */
    ~DeleteTool() { }
};


/**
* @brief 明亮度阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class BrightTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建明亮度阅片工具对象
    *
    * @param btn 明亮度工具控制按键对象
    */
    BrightTool(CArmBasePage* page, float value, QAbstractButton* btn)
        : CArmReadTool(page)
        , m_value(value)
        , m_pCtrlButton(btn)
    {}


    /**
    * @brief 调用该函数，启用明亮度工具
    */
    ~BrightTool() {}

    /**
    * @brief 调用该函数，创建明亮度阅片工具对象
    *
    * @param btn 明亮度工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page,float value, QAbstractButton* btn = nullptr);

    /**
    * @brief 调用该函数，启用明亮度工具
    */
    void startWork();

protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;

    /// 参数
    float m_value;
};

/**
* @brief 对比度度阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class ContrastTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建对比度阅片工具对象
    *
    * @param type 对比度工具类型
    * @param btn 对比度工具控制按键对象
    */
    ContrastTool(CArmBasePage* page, float value, QAbstractButton* btn = nullptr)
        : CArmReadTool(page)
        , m_value(value)
        , m_pCtrlButton(btn)
    {
    }

    /**
    * @brief 对比度阅片工具类析构函数
    */
    ~ContrastTool(){}

    /**
    * @brief 调用该函数，创建对比度阅片工具对象
    *
    * @param type 对比度工具类型
    * @param btn 对比度工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, float value, QAbstractButton* btn = nullptr);

    /**
    * @brief 调用该函数，启用对比度工具
    */
    void startWork() override;


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;

    float m_value;
};






/**
* @brief 亮度反转阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class NegativeTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建亮度反转阅片工具对象
    *
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page,bool en);

    /**
    * @brief 调用该函数，启用亮度反转工具
    */
    void startWork();


protected:

    /**
    * @brief 亮度反转阅片工具类构造函数
    */
    NegativeTool(CArmBasePage* page, bool en): CArmReadTool(page){}

    /**
    * @brief 亮度反转阅片工具类析构函数
    */
    ~NegativeTool(){}

private:

    bool m_param;   //!< 负片参数

};



/**
* @brief 旋转阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class RotateTool :public CArmReadTool
{
public:

    /**
    * @brief 旋转阅片工具类构造函数
    *
    * @param angle 旋转角度
    */
    RotateTool(CArmBasePage* page, int angle = 0) : CArmReadTool(page), m_angle(angle) {}

    /**
    * @brief 旋转阅片工具类析构函数
    */
    ~RotateTool(){}

    /**
    * @brief 调用该函数，创建旋转阅片工具对象
    *
    * @param m_rotateType 旋转类型
    * @param receiver 导航数据接收工具
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, int angle = 0);

    /**
    * @brief 调用该函数，启用旋转工具
    */
    void startWork();

protected:

    /// 旋转角度
    int m_angle;
};



/**
* @brief 翻转阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class FlipTool :public CArmReadTool
{
public:

    /**
    * @brief 翻转阅片工具类构造函数
    *
    * @param m_mirrorType 翻转类型
    * @param receiver 导航数据接收工具对象
    */
    FlipTool(CArmBasePage* page,int type = 0,bool en = true) : CArmReadTool(page), m_mirrorType(type), m_param(en) { }
    ~FlipTool() { }

    /**
    * @brief 调用该函数，创建翻转阅片工具对象
    *
    * @param m_mirrorType 翻转类型
    * @param receiver 导航数据接收工具对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, int m_mirrorType = 0, bool en = true);

    /**
    * @brief 调用该函数，启用翻转工具
    */
    void startWork();

    /// 翻转类型
    enum MIRROR_TYPE
    {
        MT_HORIZONTAL,      //<! 水平翻转
        MT_VERTICAL         //<! 竖直翻转
    };


protected:



protected:

    int m_mirrorType; //!< 翻转类型
    bool m_param;     //!< 翻转参数
};


/**
* @brief 文本绘制阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class CommentTextTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建文本绘制阅片工具对象
    *
    * @param btn 文本绘制工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(QAbstractButton* btn = nullptr);


    /**
    * @brief 调用该函数，启用文本绘制工具
    */
    void startWork();

protected:

    /**
    * @brief 文本绘制阅片工具类构造函数
    *
    * @param btn 工具控制按键
    */
    CommentTextTool(QAbstractButton* btn = nullptr) : CArmReadTool(nullptr), m_pCtrlButton(btn) {}


    /**
    * @brief 文本绘制阅片工具类析构函数
    */
    ~CommentTextTool(){}


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;
};




/**
* @brief 结束操作阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class CloseTool : CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建结束操作阅片工具对象
    *
    * @return 阅片工具对象
    */
    static IReadToolPtr create();


    /**
    * @brief 调用该函数，启用结束操作工具
    */
    void startWork();

protected:


    /**
    * @brief 结束操作阅片工具类构造函数
    */
    CloseTool() : CArmReadTool(nullptr) {}


    /**
    * @brief 结束操作阅片工具类析构函数
    */
    ~CloseTool(){}

};



/**
* @brief 锐化阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class SharpenTool : public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建锐化阅片工具对象
    *
    * @param type 锐化工具类型
    * @param btn 锐化工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, int value, CArmSharpenEnum mode) { return new SharpenTool(page, value, mode); }


    /**
    * @brief 调用该函数，启用锐化工具
    */
    void startWork();


protected:

    /**
    * @brief 锐化阅片工具类构造函数
    *
    * @param type 锐化工具类型
    * @param btn 工具控制按键
    */
    SharpenTool(CArmBasePage* page, int value, CArmSharpenEnum mode) : CArmReadTool(page), m_value{ value }, m_mode{ mode } { }


    /**
    * @brief 锐化阅片工具类析构函数
    */
    ~SharpenTool() {}

private:

    int m_value;            //!< 锐化参数
    CArmSharpenEnum m_mode; //!< 锐化模式
};



/**
* @brief 降噪阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class DenoiseTool : public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建降噪阅片工具对象
    *
    * @param value 降噪参数
    * @param mode 降噪模式
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page,int value, CArmDenoiseEnum mode) { return new DenoiseTool(page, value, mode); }


    /**
    * @brief 调用该函数，启用降噪工具
    */
    void startWork();


protected:

    /**
    * @brief 降噪阅片工具类构造函数
    *
    * @param value 降噪参数
    * @param mode 降噪模式
    */
    DenoiseTool(CArmBasePage* page, int value, CArmDenoiseEnum mode) : CArmReadTool(page), m_value{ value }, m_mode{ mode } { }


    /**
    * @brief 降噪阅片工具类析构函数
    */
    ~DenoiseTool() {}

private:

    int m_value;            //!< 降噪参数
    CArmDenoiseEnum m_mode; //!< 降噪模式
};



/**
* @brief 金属校正阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class MetalCalibTool : public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建金属校正阅片工具对象
    *
    * @param value 金属校正参数
    * @param mode 金属校正模式
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, int value, CArmMetalCalibEnum mode) { return new MetalCalibTool(page, value, mode); }


    /**
    * @brief 调用该函数，启用金属校正工具
    */
    void startWork();


protected:

    /**
    * @brief 金属校正阅片工具类构造函数
    *
    * @param value 金属校正参数
    * @param mode 金属校正模式
    */
    MetalCalibTool(CArmBasePage* page, int value, CArmMetalCalibEnum mode) :CArmReadTool(page), m_value{ value }, m_mode{ mode } { }


    /**
    * @brief 金属校正阅片工具类析构函数
    */
    ~MetalCalibTool() {}

private:

    int m_value;                //!< 金属校正参数
    CArmMetalCalibEnum m_mode;  //!< 金属校正模式
};



/**
* @brief 箭头标记阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class ArrowMarkTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建测量阅片工具对象
    *
    * @param type 测量工具类型
    * @param btn 测量工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(QAbstractButton* btn = nullptr) { return new ArrowMarkTool(btn); }


    /**
    * @brief 调用该函数，启用测量工具
    */
    void startWork();



protected:

    /**
    * @brief 测量阅片工具类构造函数
    *
    * @param type 测量工具类型
    * @param btn 工具控制按键
    */
    ArrowMarkTool(QAbstractButton* btn = nullptr) :CArmReadTool(nullptr), m_pCtrlButton{ btn } { }


    /**
    * @brief 测量阅片工具类析构函数
    */
    ~ArrowMarkTool() { }


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;

};



/**
* @brief 直线测量阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class LineMeasureTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建测量阅片工具对象
    *
    * @param type 测量工具类型
    * @param btn 测量工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(QAbstractButton* btn = nullptr) { return new LineMeasureTool(btn); }


    /**
    * @brief 调用该函数，启用测量工具
    */
    void startWork();



protected:

    /**
    * @brief 测量阅片工具类构造函数
    *
    * @param type 测量工具类型
    * @param btn 工具控制按键
    */
    LineMeasureTool(QAbstractButton* btn = nullptr) :CArmReadTool(nullptr), m_pCtrlButton{ btn } { }


    /**
    * @brief 测量阅片工具类析构函数
    */
    ~LineMeasureTool() { }


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;

};



/**
* @brief 角度测量阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class AngleMeasureTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建测量阅片工具对象
    *
    * @param type 测量工具类型
    * @param btn 测量工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(QAbstractButton* btn = nullptr) { return new AngleMeasureTool(btn); }


    /**
    * @brief 调用该函数，启用测量工具
    */
    void startWork();



protected:

    /**
    * @brief 测量阅片工具类构造函数
    *
    * @param type 测量工具类型
    * @param btn 工具控制按键
    */
    AngleMeasureTool(QAbstractButton* btn = nullptr) : CArmReadTool(nullptr), m_pCtrlButton{ btn } { }


    /**
    * @brief 测量阅片工具类析构函数
    */
    ~AngleMeasureTool() { }


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;

};




/**
* @brief 图像校准阅片工具类
* 该类主要对显示窗口对象进行相关的工具处理
*/
class ImgCalibTool :public CArmReadTool
{
public:

    /**
    * @brief 调用该函数，创建测量阅片工具对象
    *
    * @param type 测量工具类型
    * @param btn 测量工具控制按键对象
    * @return 阅片工具对象
    */
    static IReadToolPtr create(CArmBasePage* page, QAbstractButton* btn = nullptr) { return new ImgCalibTool(page, btn); }


    /**
    * @brief 调用该函数，启用测量工具
    */
    void startWork();



protected:

    /**
    * @brief 测量阅片工具类构造函数
    *
    * @param type 测量工具类型
    * @param btn 工具控制按键
    */
    ImgCalibTool(CArmBasePage* page, QAbstractButton* btn = nullptr) : CArmReadTool(page), m_pCtrlButton{ btn } { }


    /**
    * @brief 测量阅片工具类析构函数
    */
    ~ImgCalibTool() { }


protected:

    /// 工具控制按键对象
    QAbstractButton* m_pCtrlButton;

};





