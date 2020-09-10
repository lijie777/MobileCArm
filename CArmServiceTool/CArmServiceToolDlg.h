
// MobileCArmTestDlg.h : 头文件
//

#pragma once

#include "CArmDeviceComm.h"
#include "afxcmn.h"
#include "Label.h"
#include "afxwin.h"

// CMobileCArmTestDlg 对话框
class CMobileCArmTestDlg : public CDialogEx
{
// 构造
public:
	CMobileCArmTestDlg(int argc, LPWSTR *argv, CWnd* pParent = NULL);	// 标准构造函数

    ~CMobileCArmTestDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MOBILECARMTEST_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

    /// 命令行参数个数
    int m_nArgc;

    /// 命令行参数
    LPWSTR *m_ppArgv;

    /// 强制退出标志
    bool m_bForceExit;

    UCArmDeviceComm m_DevComm;

    void AppendMessage(const CString &strMsg, COLORREF clr = 0, bool bBold = false);

    /**
     * @brief 初始化界面显示
     * @return 
     */
    void InitGUI();

    /**
     * @brief 心跳连接中断后的处理
     * @return 
     */
    void OnHBDisconnected();

    /**
     * @brief 将界面上的扫描设置保存到共享内存
     * @return 
     */
    bool SaveUIScanCfg();

    /**
     * @brief 获取界面输入的管电压并检查
     * @param fVal 获取管电压值
     * @return 是否获取到合格管电压
     */
    bool GetKV(float &fVal);

    /**
    * @brief 获取界面输入的管电流并检查
    * @param fVal 获取管电流值
    * @return 是否获取到合格管电流
    */
    bool GetMA(float &fVal);

    /**
    * @brief 获取界面输入的帧率并检查
    * @param fVal 获取帧率
    * @return 是否获取到合格帧率
    */
    bool GetFPS(int &fVal);

    /**
    * @brief 获取界面输入的帧数并检查
    * @param fVal 获取帧数
    * @return 是否获取到合格帧数
    */
    bool GetFrameNum(int &fVal);

    /**
    * @brief 获取界面输入的曝光时间并检查
    * @param fVal 获取曝光时间
    * @return 是否获取到合格曝光时间
    */
    bool GetExposure(int &fVal);

    /**
     * @brief 全部激光灯开关控制
     * @param bOn 开关
     * @param strAct 动作
     * @return 
     */
    void AllLaserSwitch(bool bOn, const CString &strAct);

    /**
     * @brief 查询激光灯状态的线程函数
     * @param eLaser 激光灯
     * @return 
     */
    void LaserStatusQueryThd(OptionLaserSwitch eLaser);

#pragma region 设备进程发来的消息处理函数

    /**
     * @brief 设备状态报告函数
     * @param err 设备的当前状态
     * @return 
     */
    void OnDevStatusChanged(ErrorCode err);

    /**
     * @brief 曝光手闸按下状态变化处理函数
     * @param eBtnStep 曝光手闸级别
     * @param bPressed 是否按下
     * @return 
     */
    void OnExpBtnChanged(CArmExpBtnDef eBtnStep, bool bPressed);

    /**
     * @brief 脚踏板踏下状态变化处理函数
     * @param eBtnStep 左右脚踏
     * @param bPressed 是否踏下
     * @return 
     */
    void OnPedalChanged(CArmPedalDef eBtnStep, CArmExpBtnDef level, bool bPressed);

    /**
     * @brief 各轴启停变化处理函数
     * @param eAxis 轴
     * @param eStatus 运动状态
     * @return 
     */
    void OnAxisStatusChanged(CArmAxisDef eAxis, CArmAxisMotionDef eStatus);

    /**
     * @brief 限束器运动到位消息处理函数
     * @param coll 限束器
     * @param fPos 当前位置
     * @return 
     */
    void OnCArmCollimatorInPlace(OptionCollimator coll, float fPos);

    /**
     * @brief 各轴自动手动状态变化处理函数
     * @param eAxis 轴
     * @param eStatus 自动/手动
     * @return 
     */
    void OnCArmAxisManAutoChanged(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus);

    /**
     * @brief 激光灯开关状态变化处理函数
     * @param eLaser 激光灯选项
     * @param bOn 是否打开
     * @return 
     */
    void OnCArmLaserStatusChanged(OptionLaserSwitch eLaser, bool bOn);

    /**
     * @brief 各轴位置变化时发出的实时位置信息处理函数
     * @param eAxis 轴
     * @param fPos 当前位置
     * @return 
     */
    void OnCArmAxisPosChanged(CArmAxisDef eAxis, float fPos);

    /**
     * @brief 设备复位完成后的回调函数
     * @param res 是否复位成功
     * @return 
     */
    void OnCArmDevResetCompleted(ErrorCode res);

    /**
     * @brief 新影像接收函数
     * @param nIdx 影像索引
     * @param nWidth 影像宽度
     * @param nHeight 影像高度
     * @param pImageData 影像像素数据
     * @return 
     */
    void OnNewImageReceived(int nIdx, int nWidth, int nHeight, unsigned short*pImageData);

#pragma endregion

public:
    afx_msg void OnBnClickedOk();
    afx_msg void OnBnClickedCancel();
    afx_msg void OnBnClickedButtonDevRst();
    afx_msg void OnBnClickedButtonWarnRst();
    afx_msg void OnBnClickedAllLaserSwitchOn();
    afx_msg void OnBnClickedAllLaserSwitchOff();
    afx_msg void OnBnClickedCheckLaserCross();
    afx_msg void OnBnClickedButtonGetLsrStsCross();
    afx_msg void OnBnClickedCheckLaserViewRange();
    afx_msg void OnBnClickedButtonGetLsrStsView();
    afx_msg void OnBnClickedButtonSetKv();
    afx_msg void OnBnClickedButtonGetKv();
    afx_msg void OnBnClickedButtonSetMa();
    afx_msg void OnBnClickedButtonGetMa();
    afx_msg void OnBnClickedButtonSetFps();
    afx_msg void OnBnClickedButtonSetFrame();
    afx_msg void OnBnClickedButtonSetExp();
    afx_msg void OnBnClickedButtonScan2d();
    afx_msg void OnBnClickedButtonScan3d();
    afx_msg void OnBnClickedButtonSetVer();
    afx_msg void OnBnClickedButtonSetHor();
    afx_msg void OnBnClickedCheckEnableDap();
    afx_msg void OnBnClickedButtonReadDap();
    afx_msg void OnBnClickedButtonReadMbTemp();
    afx_msg void OnBnClickedRadioXAuto();
    afx_msg void OnBnClickedRadioXManu();
    afx_msg void OnBnClickedButtonGetPosX();
    afx_msg void OnBnClickedButtonContMoveX();
    afx_msg void OnBnClickedButtonMoveXTo();
    afx_msg void OnBnClickedRadioZAuto();
    afx_msg void OnBnClickedRadioZManu();
    afx_msg void OnBnClickedButtonGetPosZ();
    afx_msg void OnBnClickedButtonContMoveZ();
    afx_msg void OnBnClickedButtonMoveZTo();
    afx_msg void OnBnClickedRadioYAuto();
    afx_msg void OnBnClickedRadioYManu();
    afx_msg void OnBnClickedButtonGetPosY();
    afx_msg void OnBnClickedButtonContMoveY();
    afx_msg void OnBnClickedButtonMoveYTo();
    afx_msg void OnBnClickedCheckDisableTubeCtrl();

    virtual BOOL PreTranslateMessage(MSG* pMsg);

    CRichEditCtrl m_RichEditLog;
    CLabel m_LabelStatus;

    bool m_bAxisMovingX;
    bool m_bAxisMovingY;
    bool m_bAxisMovingZ;

    bool m_bAxisContMovingX;
    bool m_bAxisContMovingY;
    bool m_bAxisContMovingZ;

    bool m_bLaserCrossCurveOn;
    bool m_bLaserViewRangeOn;
};
