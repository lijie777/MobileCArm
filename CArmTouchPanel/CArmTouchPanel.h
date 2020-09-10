#ifndef CARMTOUCHPANEL_H
#define CARMTOUCHPANEL_H

#include <QtWidgets/QWidget>
#include "ui_CArmTouchPanel.h"
#include "CArmData.h"
#include "CArmControl.h"
#include "CArmDataFlow.h"
#include "CArmOperationTab.h"
#include "IImageTextureImporter.h"
#include <BaseView.h>
#include "ViewToolCreator.h"
#include "CArmView.h"
#include <QList>
#include <QLine>
#include <functional>
#include <QDebug>
#include <QProgressBar>
#include <QGridLayout>
#include <QDataStream>
#include <QFile>
#include <fstream>
#include <string>
#include <QTimer>

#include "CArmScanSetting.h"
#include "../Common/Include/Config.h"
#include "TextureData.h"
#include "../Common/Include/CArmCommon.h"
#include "CArmManager.h"
#include "CArmHistoryWidget.h"
#include "CArmVideoAdjustWidget.h"


#pragma execution_character_set("utf-8")

class WorkStationComm;
class CArmCmdBase;

enum CARM_TYPE_ENUM
{
    CARM_TOUCH_PANEL = 0,
};
class CArmTouchPanel : public QWidget, public Ui::CArmTouchPanelClass
{
    Q_OBJECT

public:

    explicit CArmTouchPanel(QWidget *parent = 0);
    ~CArmTouchPanel();

signals:
    void createStartDialogSignal();

    void showStartDialogSignal();

    void closeStartDialogSignal();

    // 右屏登录信号
    void loginSignal();
    // 右屏登出信号
    void logoutSignal();
    // 右屏选择患者信息可以选择拍摄
    void scanReadySignal();

    void splashDialogShowSignal();

    void splashDialogCloseSignal();

    void startScaning(bool scanState);

    void sendImageData(CP_ReviewData*);

    // 计算曝光时间
    void flouroTimeSignal();
    // 报警灯信号
    void alarmLightSignal();

private slots:

    /**
    * @brief								曝光页面
    *
    * @return								无
    *   @retval							    无
    */
    void scanTabClicked_slot();

    /**
    * @brief								图像操作页面
    *
    * @return								无
    *   @retval							    无
    */
    void imageOperaterTabClicked_slot();

    /**
    * @brief								仿G臂操作页面
    *
    * @return								无
    *   @retval							    无
    */
    void GArmTabClicked_slot();

    /**
    * @brief								图像传输
    *
    * @return								无
    *   @retval							    无
    */
    void transferButtonClicked_slot();

    /**
    * @brief								对比度调节
    *
    * @param				value			当前值
    * @return								无
    *   @retval							    无
    */
    void contrastDataChanged_slot(int value);

    /**
    * @brief								亮度调节
    *
    * @param				value			当前值
    * @return								无
    *   @retval							    无
    */
    void brightnessDataChanged_slot(int value);

    /**
    * @brief								金属校准
    *
    * @param				value			当前值
    * @return								无
    *   @retval							    无
    */
    void metalAdjustingDataChanged_slot(CARM_METAL_GEAR_ENUM gearValue);

    /**
    * @brief								锐化调节
    *
    * @param				value			当前值
    * @return								无
    *   @retval							    无
    */
    void sharpenDataChanged_slot(int value);

    /**
    * @brief								降噪调节
    *
    * @param				value			当前值
    * @return								无
    *   @retval							    无
    */
    void denoiseDataChanged_slot(int value);

    /**
    * @brief								智能金属调节
    *
    * @return								无
    *   @retval							    无
    */
    void brainpowerMetalAdjusting_slot();

    /**
    * @brief								负片
    *
    * @return								无
    *   @retval							    无
    */
    void negativeFilm_slot();

    /**
    * @brief								旋转
    *
    * @param				status			按钮状态
    * @return								无
    *   @retval							    无
    */
    void rotate_slot(bool status);

    /**
    * @brief								设置旋转角度
    *
    * @param				angle			改变的角度
    * @return								无
    *   @retval							    无
    */
    void setRotateAngle_slot(int angle);

    /**
    *
    * @return								无
    *   @retval							    无
    */
    void flipHorizontal_slot();

    /**
    * @brief								垂直翻转
    *
    * @return								无
    *   @retval							    无
    */
    void flipVertical_slot();

    /**
    * @brief								重置图像
    *
    * @return								无
    *   @retval							    无
    */
    void resetImage_slot();

    /**
    * @brief								开始仿G臂测试
    *
    * @return								无
    *   @retval							    无
    */
    void GArmCheckStart_slot();

    /**
    * @brief								显示序列列表
    *
    * @param				state			按钮被点击下的状态
    * @return								无
    *   @retval							    无
    */
    void showHistoryList_slot(bool state);

    /**
    * @brief								显示从历史列表中切换的图像
    *
    * @param				index			切换的第几张图像
    * @return								无
    *   @retval							    无
    */
    void showCurrentIndexImage_slot(int index);

    /**
    * @brief 曝光开始时，界面提示窗口
    * @param state 曝光状态
    * @return void
    */
    void scanStateShoe_slot(bool state);

    /**
    * @brief 视频曝光结束，播放视频
    * @return void
    */
    void videoPlay_slot();

    /**
    * @brief 点击暂停或播放按钮，进行视频播放或暂停
    * @param state 视频是否播放或暂停
    * @return void
    */
    void playOrPauseVideo_slot(bool state);

    /**
    * @brief 点击后退按钮，后退一秒，进行视频后退
    * @return void
    */
    void videoRetreat_slot();

    /**
    * @brief 点击后退按钮，后退一秒，进行视频后退
    * @return void
    */
    void videoAdvance_slot();

    void CArmViewLoadImageData_slot(CP_ReviewData* reviewData);
    // 报警设置
    void expTimeUpdateSlot();

    // 曝光时间实时显示
    void expTimeDisplaySlot();

    /**
    * @brief 报警功能
    * @param cmdBase 通信信息
    * @return void
    */
    void alarmLight();

    /**
    * @brief 计算曝光时间
    * @param cmdBase 通信信息
    * @return void
    */
    void calcFlouroTime();

protected:
    void mousePressEvent(QMouseEvent *e)override;

private:
    void initSplashDilog();
    /**
    * @brief								设置窗口风格
    *
    * @return								无
    *   @retval							    无
    */
    void setWidgetStyle();

    /**
    * @brief								链接槽函数
    *
    * @return								无
    *   @retval							    无
    */
    void linkSlotFunc();

    /**
    * @brief								创建小C界面内部用的对象
    *
    * @return								无
    *   @retval							    无
    */
    void creatCArmWidgetObj();

    /**
    * @brief								注册回调接口
    *
    * @return								无
    *   @retval							    无
    */
    void registerCArmCallBack();

    /**
    * @brief								图像传输结果
    *
    * @param				state			表示结果true 表示成功 false表示失败
    * @return								无
    *   @retval							    无
    */
    void transferResulte(bool state);

    /**
    * @brief								仿G臂测试结果
    *
    * @param				state			表示结果true 表示成功 false表示失败
    * @return								无
    *   @retval							    无
    */
    void checkGArmResult(bool state);

    /**
    * @brief								曝光状态
    *
    * @param				state			表示结果true 表示开始 false表示结束
    * @return								无
    *   @retval							    无
    */
    void scanStatus(bool state);

    /**
    * @brief								显示当前页
    *
    * @param				currentTab		表示当前页枚举
    * @return								无
    *   @retval							    无
    */
    void setCurrentTab(CARM_TAB_ENUM currentTab);

    /**
    * @brief								扫描页信息
    *
    * @return								无
    *   @retval							    无
    */
    void scanTabInfo(void);

    /**
    * @brief								仿G臂页信息
    *
    * @return								无
    *   @retval							    无
    */
    void GArmTabInfo(void);

    /**
    * @brief								加载图像数据
    *
    * @param				newData		    数据结构
    * @return								无
    *   @retval							    无
    */
    void loadImageTexture(const NewImgReceived *newData);

    /**
    * @brief								将影像数据保存成二进制文件
    *
    * @param				newData		    数据结构
    * @return								无
    *   @retval							    无
    */
    bool saveBinaryFile(CP_ImageData imageData);

    /**
    * @brief								读取保存的二进制文件
    *
    * @param				newData		    数据结构
    * @return								无
    *   @retval							    无
    */
    bool readBinaryFile(NewImgReceived* imageData);

    /**
    * @brief								工作站发送数据接口
    *
    * @param				cmdBase		    数据基类
    * @return								无
    *   @retval							    无
    */
    void workStationData(const CArmCmdBase* cmdBase);

    /**
    * @brief								拍摄患者信息
    *
    * @param				dataInfo		拍摄患者信息数据结构
    * @return								无
    *   @retval							    无
    */
    void setDataInfoToUi(const CArmCmdNewPatientInfo *dataInfo);


    /**
    * @brief 样式设置
    * @param void
    * @return void
    */
    void setContorlInitData();

    /**
    * @brief 获取传输图像信息
    * @param imageInfo 图像信息
    * @return IPCImgIdTransfer  图像中的imguid 患者ID 拍摄ID studyID
    */
    IPCImgIdTransfer getTransferInfo(CP_ImageData imageInfo);

    /*add begin by li*/
    void styleSetting(void);

    /**
    * @brief 影像数据更新
    * @param cmdBase 通信信息
    * @return void
    */
    void updateImage(const CArmCmdBase* cmdBase);

    /**
    * @brief 十字线激光灯状态发生变化
    * @param cmdBase 通信信息
    * @return void
    */
    void updateLaserLamp(const CArmCmdBase* cmdBase);

    /**
    * @brief 和工作站连接状态检查
    * @param cmdBase 通信信息
    * @return void
    */
    void netWorkStatus(const CArmCmdBase* cmdBase);



    /**
    * @brief 患者信息更新后界面刷新
    * @param cmdBase 通信信息
    * @return void
    */
    void updateScanInfo(const CArmCmdBase* cmdBase);

    /**
    * @brief 更新曝光时间
    * @param cmdBase 通信信息
    * @return void
    */
    void updateFluoroTime(const CArmCmdBase* cmdBase);
    /**
    * @brief 更新升降柱Z方向位置
    * @param cmdBase 通信信息
    * @return void
    */
    void updateCArmPos_Z(const CArmCmdBase* cmdBase);
    /**
    * @brief 更新前后轴X方向位置
    * @param cmdBase 通信信息
    * @return void
    */
    void updateCArmPos_X(const CArmCmdBase* cmdBase);
    /**
    * @brief 更新C型臂角度
    * @param cmdBase 通信信息
    * @return void
    */
    void updateCArmAngle(const CArmCmdBase* cmdBase);

    /**
    * @brief 初始化底部病人信息
    * @param void
    * @return void
    */
    void initBaseScanInfo(void);


    /**
    * @brief 扫描模式更新
    * @param scanMode 扫描模式
    * @return void
    */
    void baseScanModeUpdate(const ScanMode &scanMode);

    /**
    * @brief 扫描部位更新
    * @param scanMode 扫描部位
    * @return void
    */
    void baseScanPartUpdate(const ScanPart &scanPart);

    /**
    * @brief 扫描剂量更新
    * @param scanMode 扫描剂量
    * @return void
    */
    void baseScanDoseUpdate(const DoseType &doseType);

    /**
    * @brief 脉冲频率更新
    * @param scanMode 脉冲频率
    * @return void
    */
    void basePpsUpdate(int pps);

    /**
    * @brief 脉冲频率更新
    * @param kv 电压
    * @param ma 电流
    * @return void
    */
    void basekVmAUpdate(float kv, float ma);

    /*
    * @brief 界面数据变化汇总界面数据发给工作站
    * @return void
    */
    void collectImageData();

    /*
    * @brief 界面数据变化存入配置文件中
    * @return void
    */
    void imageDataChangedToXML();

    /*
    * @brief 读取当前患者的拍摄数据
    * @return void
    */
    void setCurrentPatientShootData();
    /**
    * @brief 传输数据给工作站
    * @param scanfig 扫描参数
    * @return void
    */
    void sendData2WorkStation(const CArmLastScanConfig& scanfig);

    /**
    * @brief 激光灯状态设置
    * @param lampStatus 激光灯状态 true:开启激光灯 false:关闭激光灯
    * @return void
    */
    void laserLampStatusSetting(bool lampStatus);

    /**
    * @brief 更新水平限束器的位置
    * @param cmdBase 通信信息
    * @return void
    */
    void updateCollimatorH(const CArmCmdBase* cmdBase);

    /**
    * @brief 更新垂直限束器的位置
    * @param cmdBase 通信信息
    * @return void
    */
    void updateCollimatorV(const CArmCmdBase* cmdBase);

    /**
    * @brief 扫描界面底部基础信息更新
    * @param scanConfig 扫描信息
    * @return void
    */
    void updateInfo(const CArmLastScanConfig& scanConfig);

    /**
    * @brief 视频拍摄显示
    * @param void
    * @return void
    */
    void videoImageShow(void);

    /*
    * @brief 二级手闸按下，开始放射线
    * @param cmdBase 通信信息
    * @return void
    */
    void exposeBtnStatus(const CArmCmdBase* cmdBase);
    /*
    * @brief 扫描结束
    * @param cmdBase 通信信息
    * @return void
    */
    void scanStatus(const CArmCmdBase* cmdBase);


    void updateDoseUsedByDAP(const CArmCmdBase* cmdBase);


public slots:
    /**
    * @brief C型臂升降柱上升/下降（长按）
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void CArmUpDown(OptionLiftColMove opt, float speed);

    /**
    * @brief C型臂前后轴向前/向后（长按）
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void CArmFrontBack(OptionXAxisMove opt, float speed);

    /**
    * @brief C型臂角度调节（顺时针/逆时针）（长按）
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void CArmRotate(OptionCArmMove opt, float speed);

    /**
    * @brief C型臂升降柱上升/下降（非长按）
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void CArmUpDown(float pos, float speed);

    /**
    * @brief C型臂前后轴向前/向后（非长按）
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void CArmFrontBack(float pos, float speed);

    /**
    * @brief C型臂角度调节（顺时针/逆时针）（非长按）
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void CArmRotate(float pos, float speed);

    /**
    * @brief 限束器调节(长按)
    * @param option 移动位置
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void collimatorMotion(OptionCollimatorMoveDirection option, CArmDeviceMoveTypeDef pos, float speed);

    /**
    * @brief 限束器水平调节（非长按）
    * @param opt 移动方向
    * @param speed 移动速度
    * @return void
    */
    void collimatorMotion(CArmDeviceMoveTypeDef opt, float pos, float speed);

    /**
    * @brief 启动界面显示
    * @param void
    * @return void
    */
    void splashDilogShow(void);
    /**
    * @brief 启动界面关闭
    * @param void
    * @return void
    */
    void splashDilogClose();

    /**
    * @brief 右屏登录
    * @param void
    * @return void
    */
    void loginSlot(void);

    /**
    * @brief 右屏登出
    * @param void
    * @return void
    */
    void logoutSlot(void);

    /**
    * @brief 右屏选择患者信息后才可以拍片
    * @param void
    * @return void
    */
    void scanReadySlot(void);

private:

    ///风格字符串结构体
    CARM_STYLE_STRUCT      m_styleStruct;

    ///当前页提示
    QLabel*                m_pCurrentTabLabel;

    ///扫描状态弹窗
    CArmDialog*            m_pScanDialog;

    ///仿G臂状态弹窗
    CArmDialog*            m_pGArmCheckDialog;

    CArmRotateDialog*      m_pRotateDialog;

    CArmHistoryWidget*     m_pHistoryWidget;

    ///图像处理页面
    CArmImageTab*          m_pImageOperationTab;

    ///纹理加载工具
    IImageTextureImporter*  m_pImageTextureImporter;

    ///图像View
    CArmView*               m_pCArmImageView;

    ///TCP客户端
    WorkStationComm*        m_workStationComm;

    ///扫描页面
    CArmScanSetting*       m_pScanSettingWidget;

    ///图像存储二进制文件路径
    std::string            m_imageFilePthStr;

    ///当前患者数据结构
    CP_PatientData          m_CurrPatientInfo;

    ///拍摄完成后的信息
    CP_InstanceData         m_InstanceInfo;

    CP_ImageData            m_CurrentImageInfo;

    ///当前拍摄数据结构
    CArmLastScanConfig          *m_CurrentShootData;

    int                         m_iRecordMetalGear;

    std::map<ScanMode, CP_ImageData> m_ScanModeDataMap;


    std::multimap<CArmLastScanConfig *, CP_ImageData> m_PatientImageMap;


    std::vector<CP_ImageData>     m_ImageInfoVec;

    std::vector<CP_ReviewData*>     m_VideoImageInfoVec;

    std::vector<CP_ReviewData*>     m_MppImageInfoVec;

    std::vector<CP_ReviewData*>     m_PointImageInfoVec;

    QDialog*                m_splashDialog;
    QLabel*                 m_splashLabel;

    int                     m_iCountTime;

    QTimer*                 m_pTimeClock;


    mutable bool            m_bNetConnect{ false };  //!< 工作站是否连接
    mutable bool            m_bLogin{ false };       //!< 工作站是否登录

    ///记录曝光的是否开始
    bool                    m_bScaningStatus;

    IImage2DTextureImportorComponentPtr m_pTextureImportor;
    QTime                   m_alarmTime;     //!< 警告使用的时间
    QTimer                  m_alarmTimer;    //!< 报警复位定时器

    QTime                   *m_expTime;  //!< 已经曝光的时间
    QTimer                  *m_timer;    //!< 时间显示定时器

    mutable bool            m_bScanInfoFlag{ false };  //!< true:可以发送扫描信息 false:不可以发送扫描信息

};

#endif // CARMTOUCHPANEL_H
