#ifndef CARMDATAFLOW_H_
#define CARMDATAFLOW_H_
/**
* @file CArmDataFlow.h
* @brief 小C触屏与工作站数据流文件
* @author 路旺明
*/
#include <functional>
#include "../Common/Include/Config.h"
#include "../Common/Include/CArmScCmdDef.h"

/**
* @brief 获取系统预设值
* 
*/
class CArmInitData
{
private:
    CArmInitData();

public:

    /**
    * @brief								创建对象
    *
    * @return		CArmInitData			类型
    *   @retval							    返回自己的对象
    */
    static CArmInitData* getInstance();

    /**
    * @brief								提供使用者销毁对象
    *
    * @return		            			无
    *   @retval							    无
    */
    void destroy();

    /**
    * @brief								工作站给触屏设置预设值
    *
    * @param				type			标记该view的类型
    * @param				parent			默认父类
    * @return								无
    *   @retval							    无
    */
    void setSystemDefaultData(CArmConfig *configData);

    ///将曝光参数发给工作站
    std::function<void()> sendScanData;

    CArmConfig*            m_pCArmConfig = nullptr;         //!< 开机时获取到的扫描信息

private:

    ///对象指针
    static  CArmInitData*           m_initData;
    
};

/**
* @brief 仿G臂测试
*
*/
class GArmCheck
{
private:
    /**
    * @brief								 仿G臂构造接口
    *
    * @return						        无
    *   @retval							    无
    */
    GArmCheck();

public:
    /**
    * @brief								 获取类的操作对象
    *		
    * @return				GArmCheck		对象类型
    *   @retval							    返回类的操作对象指针
    */
    static GArmCheck* getInstance();

    /**
    * @brief                                 提供给使用对象销毁类对象
    *
    * @return						        无
    *   @retval							    无
    */
    void destroy();

    /**
    * @brief								工作站推送的患者信息
    *
    * @param				type			标记该view的类型
    * @param				parent			默认父类
    * @return								无
    *   @retval							    无
    */
    void setPatientInfo();

    /**
    * @brief								触屏上选择的仿G臂测试角度
    *
    * @param				type			标记该view的类型
    * @param				parent			默认父类
    * @return								无
    *   @retval							    无
    */
    void setCheckData();

    /**
    * @brief								工作站发送测试
    *
    * @param				type			标记该view的类型
    * @param				parent			默认父类
    * @return								无
    *   @retval							    无
    */
    void setCheckResult(bool result);

    ///将患者信息发送给界面
    std::function<void()> patienInfo;

    ///仿G臂需要测试角度给工作站
    std::function<void()> needCheckData;

    ///测试结果给触摸屏
    std::function<void(bool)> checkResult;

private:

    ///仿G臂对象指针
    static GArmCheck*   m_gArmCheck;
};

/**
* @brief 触屏界面数据发生改变
*
*/
class CArmDataChanged
{
private:
    /**
    * @brief								触屏界面发生变化构造函数
    *
    * @return						        无
    *   @retval							    无
    */
    CArmDataChanged();
public:
    /**
    * @brief								 获取类的操作对象
    *
    * @return		CArmDataChanged		    对象类型
    *   @retval							    返回类的操作对象指针
    */
    static CArmDataChanged* getInstance();
    
    /**
    * @brief                                提供给使用对象销毁类对象
    *
    * @return						        无
    *   @retval							    无
    */
    void destroy();

    /**
    * @brief								触摸屏曝光参数发生改变
    *
    * @param				state			表示被按钮按下的状态
    * @return								无
    *   @retval							    无
    */
    void setScanDataChanged();

    /**
    * @brief								激光等状态发生变化
    *
    * @param				state			表示激光灯是否被开启或关闭
    * @return								无
    *   @retval							    无
    */
    void setLaserLampStatus(bool status);

    /**
    * @brief								曝光状态
    *
    * @param				state			表示是否开始曝光
    * @return								无
    *   @retval							    无
    */
    void setScanStatus(bool status);

    /**
    * @brief								C型臂那个方位被按下
    *
    * @param				state			表示是否开始曝光
    * @return								无
    *   @retval							    无
    */
    void setCArmButtonInfo();

    /**
    * @brief								工作站发送C型臂实时的位置数据
    *
    * @param				state			表示是否开始曝光
    * @return								无
    *   @retval							    无
    */
    void setCArmData();

    /**
    * @brief								工作站发送曝光完成的影像数据
    *
    * @param				state			表示是否开始曝光
    * @return								无
    *   @retval							    无
    */
    void setImageData(NewImgReceived imageData);

    /**
    * @brief								影像后处理的实时数据
    *
    * @param				state			表示是否开始曝光
    * @return								无
    *   @retval							    无
    */
    void setImageDisposeData();


    ///激光灯状态
    std::function<void(bool)>   laserLampStatus;

    ///曝光状态
    std::function<void(bool)>   scanStatus;

    std::function<void(NewImgReceived)>   loadImageData;

    

private:
    ///小C界面数据发生变化
    static CArmDataChanged*     m_CArmData;

};


/**
* @brief 进行影像传输
*
*/
class CArmTransferSignal
{
private:
    /**
    * @brief								进行影像传输构造函数
    *
    * @return						        无
    *   @retval							    无
    */
    CArmTransferSignal();

public:
    /**
    * @brief								获取类的操作对象
    *
    * @return		CArmTransferSignal		对象类型
    *   @retval							    返回类的操作对象指针
    */
    static CArmTransferSignal* getInstance();

    void destroy();

    /**
    * @brief								触屏按钮被按下
    *
    * @param				state			表示被按钮按下的状态
    * @return								无
    *   @retval							    无
    */
    void setTransferState(bool state);

    /**
    * @brief								传输结果
    *
    * @param				state			表示传输是否成功
    * @return								无
    *   @retval							    无
    */
    void setTransferResult(bool state);

    ///发送传输按钮被按的状态
    std::function<void(bool)> transferPress;

    ///发送传输结果
    std::function<void(bool)> transferResult;

private:
    static CArmTransferSignal*       m_transferObj;
};


#endif // !CARMDATAFLOW_H_

