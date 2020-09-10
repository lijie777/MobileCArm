
// MobileCArmTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "CArmServiceTool.h"
#include "CArmServiceToolDlg.h"
#include "afxdialogex.h"
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>
#include <CommonUtility.h>
#include <regex>
#include <ScanConfigHandler.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace log4cplus;
using namespace log4cplus::helpers;

Logger g_logger;

#define COLOR_FAIL (RGB(255, 0, 0))

void InitConsoleLogger()
{
#ifdef CONSOLE_LOG

    AllocConsole();//为当前的窗口程序申请一个Console窗口
    freopen("CONOUT$", "a+", stdout);

    PropertyConfigurator::doConfigure(TOWS(UCommonUtility::GetExeDir()) + std::wstring(L"\\Config\\log.cfg"));
#endif
    g_logger = Logger::getInstance(L"CArmTest");
}

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMobileCArmTestDlg 对话框

CMobileCArmTestDlg::CMobileCArmTestDlg(int argc, LPWSTR *argv, CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MOBILECARMTEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_nArgc = argc;
    m_ppArgv = argv;

    m_bAxisMovingX = false;
    m_bAxisMovingY = false;
    m_bAxisMovingZ = false;

    m_bAxisContMovingX = false;
    m_bAxisContMovingY = false;
    m_bAxisContMovingZ = false;

    m_bForceExit = false;
}

CMobileCArmTestDlg::~CMobileCArmTestDlg()
{
    if (m_ppArgv != nullptr)
    {
        LocalFree(m_ppArgv);
        m_ppArgv = nullptr;
    }

#ifdef CONSOLE_LOG
    FreeConsole();
#endif
}

void CMobileCArmTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_RICHEDIT_LOG, m_RichEditLog);
    DDX_Control(pDX, IDC_STATIC_DEV_STS, m_LabelStatus);
}

BEGIN_MESSAGE_MAP(CMobileCArmTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDOK, &CMobileCArmTestDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CMobileCArmTestDlg::OnBnClickedCancel)
    ON_BN_CLICKED(IDC_BUTTON_DEV_RST, &CMobileCArmTestDlg::OnBnClickedButtonDevRst)
    ON_BN_CLICKED(IDC_BUTTON_WARN_RST, &CMobileCArmTestDlg::OnBnClickedButtonWarnRst)
    ON_BN_CLICKED(IDC_CHECK_LASER_CROSS, &CMobileCArmTestDlg::OnBnClickedCheckLaserCross)
    ON_BN_CLICKED(IDC_BUTTON_GET_LSR_STS_CROSS, &CMobileCArmTestDlg::OnBnClickedButtonGetLsrStsCross)
    ON_BN_CLICKED(IDC_CHECK_LASER_VIEW_RANGE, &CMobileCArmTestDlg::OnBnClickedCheckLaserViewRange)
    ON_BN_CLICKED(IDC_BUTTON_GET_LSR_STS_VIEW, &CMobileCArmTestDlg::OnBnClickedButtonGetLsrStsView)
    ON_BN_CLICKED(IDC_BUTTON_SET_KV, &CMobileCArmTestDlg::OnBnClickedButtonSetKv)
    ON_BN_CLICKED(IDC_BUTTON_GET_KV, &CMobileCArmTestDlg::OnBnClickedButtonGetKv)
    ON_BN_CLICKED(IDC_BUTTON_SET_MA, &CMobileCArmTestDlg::OnBnClickedButtonSetMa)
    ON_BN_CLICKED(IDC_BUTTON_GET_MA, &CMobileCArmTestDlg::OnBnClickedButtonGetMa)
    ON_BN_CLICKED(IDC_BUTTON_SET_FPS, &CMobileCArmTestDlg::OnBnClickedButtonSetFps)
    ON_BN_CLICKED(IDC_BUTTON_SET_FRAME, &CMobileCArmTestDlg::OnBnClickedButtonSetFrame)
    ON_BN_CLICKED(IDC_BUTTON_SET_EXP, &CMobileCArmTestDlg::OnBnClickedButtonSetExp)
    ON_BN_CLICKED(IDC_BUTTON_SCAN_2D, &CMobileCArmTestDlg::OnBnClickedButtonScan2d)
    ON_BN_CLICKED(IDC_BUTTON_SCAN_3D, &CMobileCArmTestDlg::OnBnClickedButtonScan3d)
    ON_BN_CLICKED(IDC_BUTTON_SET_VER, &CMobileCArmTestDlg::OnBnClickedButtonSetVer)
    ON_BN_CLICKED(IDC_BUTTON_SET_HOR, &CMobileCArmTestDlg::OnBnClickedButtonSetHor)
    ON_BN_CLICKED(IDC_CHECK_ENABLE_DAP, &CMobileCArmTestDlg::OnBnClickedCheckEnableDap)
    ON_BN_CLICKED(IDC_BUTTON_READ_DAP, &CMobileCArmTestDlg::OnBnClickedButtonReadDap)
    ON_BN_CLICKED(IDC_BUTTON_READ_MB_TEMP, &CMobileCArmTestDlg::OnBnClickedButtonReadMbTemp)
    ON_BN_CLICKED(IDC_RADIO_X_AUTO, &CMobileCArmTestDlg::OnBnClickedRadioXAuto)
    ON_BN_CLICKED(IDC_RADIO_X_MANU, &CMobileCArmTestDlg::OnBnClickedRadioXManu)
    ON_BN_CLICKED(IDC_BUTTON_GET_POS_X, &CMobileCArmTestDlg::OnBnClickedButtonGetPosX)
    ON_BN_CLICKED(IDC_BUTTON_CONT_MOVE_X, &CMobileCArmTestDlg::OnBnClickedButtonContMoveX)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_X_TO, &CMobileCArmTestDlg::OnBnClickedButtonMoveXTo)
    ON_BN_CLICKED(IDC_RADIO_Z_AUTO, &CMobileCArmTestDlg::OnBnClickedRadioZAuto)
    ON_BN_CLICKED(IDC_RADIO_Z_MANU, &CMobileCArmTestDlg::OnBnClickedRadioZManu)
    ON_BN_CLICKED(IDC_BUTTON_GET_POS_Z, &CMobileCArmTestDlg::OnBnClickedButtonGetPosZ)
    ON_BN_CLICKED(IDC_BUTTON_CONT_MOVE_Z, &CMobileCArmTestDlg::OnBnClickedButtonContMoveZ)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_Z_TO, &CMobileCArmTestDlg::OnBnClickedButtonMoveZTo)
    ON_BN_CLICKED(IDC_RADIO_Y_AUTO, &CMobileCArmTestDlg::OnBnClickedRadioYAuto)
    ON_BN_CLICKED(IDC_RADIO_Y_MANU, &CMobileCArmTestDlg::OnBnClickedRadioYManu)
    ON_BN_CLICKED(IDC_BUTTON_GET_POS_Y, &CMobileCArmTestDlg::OnBnClickedButtonGetPosY)
    ON_BN_CLICKED(IDC_BUTTON_CONT_MOVE_Y, &CMobileCArmTestDlg::OnBnClickedButtonContMoveY)
    ON_BN_CLICKED(IDC_BUTTON_MOVE_Y_TO, &CMobileCArmTestDlg::OnBnClickedButtonMoveYTo)
    ON_BN_CLICKED(IDC_CHECK_DISABLE_TUBE_CTRL, &CMobileCArmTestDlg::OnBnClickedCheckDisableTubeCtrl)
    ON_BN_CLICKED(IDC_BUTTON_ALL_LSR_ON, &CMobileCArmTestDlg::OnBnClickedAllLaserSwitchOn)
    ON_BN_CLICKED(IDC_BUTTON_ALL_LSR_OFF, &CMobileCArmTestDlg::OnBnClickedAllLaserSwitchOff)
END_MESSAGE_MAP()

CString FormatString(LPCTSTR pstrFormat, ...)
{
    CString strResult;

    va_list args;
    va_start(args, pstrFormat);
    strResult.FormatV(pstrFormat, args);
    va_end(args);

    return strResult;
}

// CMobileCArmTestDlg 消息处理程序

BOOL CMobileCArmTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

    InitConsoleLogger();

    char **argv = new char *[m_nArgc];
    for (int i = 0; i < m_nArgc; i++)
    {
        std::string ss = TONS(m_ppArgv[i]);
        argv[i] = new char[ss.length() + 1];
        std::strcpy(argv[i], ss.c_str());
    }

    LocalFree(m_ppArgv);
    m_ppArgv = nullptr;

    bool bRes = m_DevComm.ArgsParse(m_nArgc, argv);
    for (int i = 0; i < m_nArgc; i++)
    {
        delete[] argv[i];
    }

    delete argv;

    if (!bRes)
    {
        m_bForceExit = true;
        SendMessage(WM_CLOSE);
        return FALSE;
    }

    m_DevComm.OnHBDisconnected = std::bind(&CMobileCArmTestDlg::OnHBDisconnected, this);
    m_DevComm.OnDevStatusChanged = std::bind(&CMobileCArmTestDlg::OnDevStatusChanged, this, std::placeholders::_1);
    m_DevComm.OnExpBtnChanged = std::bind(&CMobileCArmTestDlg::OnExpBtnChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_DevComm.OnPedalChanged = std::bind(&CMobileCArmTestDlg::OnPedalChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    m_DevComm.OnAxisStatusChanged = std::bind(&CMobileCArmTestDlg::OnAxisStatusChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_DevComm.OnCArmCollimatorInPlace = std::bind(&CMobileCArmTestDlg::OnCArmCollimatorInPlace, this, std::placeholders::_1, std::placeholders::_2);
    m_DevComm.OnCArmAxisManAutoChanged = std::bind(&CMobileCArmTestDlg::OnCArmAxisManAutoChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_DevComm.OnCArmLaserStatusChanged = std::bind(&CMobileCArmTestDlg::OnCArmLaserStatusChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_DevComm.OnCArmAxisPosChanged = std::bind(&CMobileCArmTestDlg::OnCArmAxisPosChanged, this, std::placeholders::_1, std::placeholders::_2);
    m_DevComm.OnCArmDevResetCompleted = std::bind(&CMobileCArmTestDlg::OnCArmDevResetCompleted, this, std::placeholders::_1);
    m_DevComm.OnNewImageReceived = std::bind(&CMobileCArmTestDlg::OnNewImageReceived, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

    if(!m_DevComm.StartHeartbeat() || !m_DevComm.StartLogging() || !m_DevComm.GetServicePorts() || !m_DevComm.StartServices())
    {
        m_bForceExit = true;
        SendMessage(WM_CLOSE);
        return FALSE;
    }

    InitGUI();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMobileCArmTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMobileCArmTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMobileCArmTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMobileCArmTestDlg::AppendMessage(const CString &strMsg, COLORREF clr, bool bBold)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

    CString strTime;
    strTime.Format(_T("%02d:%02d:%02d.%03d "), st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);

    m_RichEditLog.SetSel(-1, -1);

    CHARFORMAT cf;
    ZeroMemory(&cf, sizeof(CHARFORMAT));
    cf.cbSize = sizeof(CHARFORMAT);
    cf.dwMask = CFM_BOLD | CFM_COLOR;
    cf.dwEffects = bBold ? CFE_BOLD : 0;
    cf.crTextColor = clr;

    m_RichEditLog.SetSelectionCharFormat(cf);
    m_RichEditLog.ReplaceSel(strTime + strMsg + '\n');

    m_RichEditLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CMobileCArmTestDlg::InitGUI()
{
    CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_X_DIRECT);
    int nIdx = pBox->AddString(_T("向前")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionXAxisMove::Forward);
    pBox->AddString(_T("向后")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionXAxisMove::Backward);
    pBox->SetCurSel(0);

    pBox = (CComboBox*)GetDlgItem(IDC_COMBO_Y_DIRECT);
    nIdx = pBox->AddString(_T("顺时针")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionCArmMove::Clockwise);
    pBox->AddString(_T("逆时针")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionCArmMove::Anticlockwize);
    pBox->SetCurSel(0);

    pBox = (CComboBox*)GetDlgItem(IDC_COMBO_Z_DIRECT);
    nIdx = pBox->AddString(_T("向上")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionLiftColMove::Upward);
    pBox->AddString(_T("向下")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionLiftColMove::Downward);
    pBox->SetCurSel(0);

    pBox = (CComboBox*)GetDlgItem(IDC_COMBO_C_SCAN_DIRECT);
    nIdx = pBox->AddString(_T("顺时针")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionCArmMove::Clockwise);
    pBox->AddString(_T("逆时针")); pBox->SetItemData(nIdx, (DWORD_PTR)OptionCArmMove::Anticlockwize);
    pBox->SetCurSel(0);

    m_LabelStatus.SetText(_T("未连接"),COLOR_FAIL);

    SetDlgItemText(IDC_EDIT_KV, _T("110"));
    SetDlgItemText(IDC_EDIT_MA, _T("20"));
    SetDlgItemText(IDC_EDIT_FPS, _T("10"));
    SetDlgItemText(IDC_EDIT_FRAME_NUM, _T("1"));
    SetDlgItemText(IDC_EDIT_EXPOSURE, _T("10"));

    ((CButton*)GetDlgItem(IDC_RADIO_NORMAL))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_CHECK_XRAY))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_DAP))->SetCheck(TRUE);

    ((CButton*)GetDlgItem(IDC_RADIO_X_AUTO))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_RADIO_Y_AUTO))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_RADIO_Z_AUTO))->SetCheck(TRUE);

    SetDlgItemText(IDC_EDIT_POS_VER, _T("10"));
    SetDlgItemText(IDC_EDIT_POS_HOR, _T("10"));

    SetDlgItemText(IDC_EDIT_SPEED_X, _T("10"));
    SetDlgItemText(IDC_EDIT_DEST_POS_X, _T("100"));

    SetDlgItemText(IDC_EDIT_SPEED_Y, _T("10"));
    SetDlgItemText(IDC_EDIT_DEST_POS_Y, _T("100"));

    SetDlgItemText(IDC_EDIT_SPEED_Z, _T("10"));
    SetDlgItemText(IDC_EDIT_DEST_POS_Z, _T("100"));

    m_RichEditLog.SetBackgroundColor(FALSE, RGB(222, 222, 222));
}

void CMobileCArmTestDlg::OnHBDisconnected()
{
    m_bForceExit = true;

    SendMessage(WM_CLOSE);
}

bool CMobileCArmTestDlg::SaveUIScanCfg()
{
    float fKV = 0.0f;
    if (!GetKV(fKV)) return false;

    float fMA = 0.0f;
    if (!GetMA(fMA)) return false;

    int nFPS = 0;
    if (!GetFPS(nFPS)) return false;

    int nFrameNum = 0;
    if (!GetFrameNum(nFrameNum)) return false;

    int nExp = 0;
    if (!GetExposure(nExp)) return false;

    CT_Range rng;
    GlobalMemMapping::GetScanConfigXML(rng);

    rng.fKV[0] = fKV;
    rng.fMA[0] = fMA;
    rng.nFPS = nFPS;
    rng.nFrameNum = nFrameNum;
    rng.nExposure[0] = nExp;
    rng.nBinningMode = 2;//正常使用时为1

    return GlobalMemMapping::SetScanConfigXML(rng);
}

bool CMobileCArmTestDlg::GetKV(float &fVal)
{
    CString strVal;
    GetDlgItemText(IDC_EDIT_KV, strVal);

    if (strVal.IsEmpty())
    {
        AppendMessage(_T("请输入管电压"), COLOR_FAIL);
        return false;
    }

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的管电压值无效"), COLOR_FAIL);
        return false;
    }

    fVal = _tstof(strVal);
    if (fVal <= 0)
    {
        AppendMessage(_T("输入的管电压值无效"), COLOR_FAIL);
        return false;
    }

    return true;
}

bool CMobileCArmTestDlg::GetMA(float &fVal)
{
    CString strVal;
    GetDlgItemText(IDC_EDIT_MA, strVal);

    if (strVal.IsEmpty())
    {
        AppendMessage(_T("请输入管电流"), COLOR_FAIL);
        return false;
    }

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的管电流值无效"), COLOR_FAIL);
        return false;
    }

    fVal = _tstof(strVal);
    if (fVal <= 0)
    {
        AppendMessage(_T("输入的管电流值无效"), COLOR_FAIL);
        return false;
    }

    return true;
}

bool CMobileCArmTestDlg::GetFPS(int &nVal)
{
    CString strVal;
    GetDlgItemText(IDC_EDIT_FPS, strVal);

    if (strVal.IsEmpty())
    {
        AppendMessage(_T("请输入帧率"), COLOR_FAIL);
        return false;
    }

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^([0-9]{1,})$")))
    {
        AppendMessage(_T("输入的帧率值无效"), COLOR_FAIL);
        return false;
    }

    nVal = _tstoi(strVal);
    if (nVal <= 0)
    {
        AppendMessage(_T("输入的帧率值无效"), COLOR_FAIL);
        return false;
    }

    return true;
}

bool CMobileCArmTestDlg::GetFrameNum(int &nVal)
{
    CString strVal;
    GetDlgItemText(IDC_EDIT_FRAME_NUM, strVal);

    if (strVal.IsEmpty())
    {
        AppendMessage(_T("请输入采集帧数"), COLOR_FAIL);
        return false;
    }

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^([0-9]{1,})$")))
    {
        AppendMessage(_T("输入的采集帧数无效"), COLOR_FAIL);
        return false;
    }

    nVal = _tstoi(strVal);
    if (nVal <= 0)
    {
        AppendMessage(_T("输入的采集帧数无效"), COLOR_FAIL);
        return false;
    }

    return true;
}

bool CMobileCArmTestDlg::GetExposure(int &nVal)
{
    CString strVal;
    GetDlgItemText(IDC_EDIT_EXPOSURE, strVal);

    if (strVal.IsEmpty())
    {
        AppendMessage(_T("请输入曝光时间"), COLOR_FAIL);
        return false;
    }

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^([0-9]{1,})$")))
    {
        AppendMessage(_T("输入的曝光时间无效"), COLOR_FAIL);
        return false;
    }

    nVal = _tstoi(strVal);
    if (nVal <= 0)
    {
        AppendMessage(_T("输入的曝光时间无效"), COLOR_FAIL);
        return false;
    }

    return true;
}

void CMobileCArmTestDlg::AllLaserSwitch(bool bOn, const CString &strAct)
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(strAct + _T("全部激光灯"));

    ErrorCode res = m_DevComm.LaserSwitch(OptionLaserSwitch::All, bOn);

    if (IS_OK(res))
        AppendMessage(strAct + _T("全部激光灯成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("全部激光灯失败，错误码：%04X"), (int)res);

        AppendMessage(strAct + strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::LaserStatusQueryThd(OptionLaserSwitch eLaser)
{
    bool bOn = true;
    float fOffDelay = 0;
    ErrorCode res;

    int ctrlID = (eLaser == OptionLaserSwitch::CrossCurve) ? IDC_STATIC_OFF_DELAY_CROSS : IDC_STATIC_OFF_DELAY_VIEW;

    while (bOn)
    {
        fOffDelay = 0;

        res = m_DevComm.GetLaserStatus(eLaser, bOn, fOffDelay);

        if (IS_OK(res))
        {
            if(bOn)
                SetDlgItemText(ctrlID, FormatString(_T("剩余关闭延时：%.2fs"), fOffDelay));
            else
                SetDlgItemText(ctrlID, _T("剩余关闭延时：---"));
        }
        else
        {
            CString strMsg;
            strMsg.Format(_T("获取十字线激光灯状态失败，错误码：%04X"), (int)res);

            AppendMessage(strMsg, COLOR_FAIL);

            SetDlgItemText(ctrlID, _T("剩余关闭延时：---"));
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

#pragma region 设备进程发来的消息处理函数

//设备状态报告函数
void CMobileCArmTestDlg::OnDevStatusChanged(ErrorCode err)
{
    if(IS_ERR(err))
        m_LabelStatus.SetText(_T("未连接"), COLOR_FAIL);
    else
        m_LabelStatus.SetText(_T("已连接"), RGB(0, 188, 0));
}

//曝光手闸按下状态变化处理函数
void CMobileCArmTestDlg::OnExpBtnChanged(CArmExpBtnDef eBtnStep, bool bPressed)
{
    CString str;
    str.Format(_T("%s曝光手闸%s"), eBtnStep == CArmExpBtnDef::Level1 ? _T("一级") : _T("二级"), bPressed ? _T("按下") : _T("松开"));

    AppendMessage(str);
}

//脚踏板踏下状态变化处理函数
void CMobileCArmTestDlg::OnPedalChanged(CArmPedalDef eBtnStep, CArmExpBtnDef level, bool bPressed)
{
    CString str;
    str.Format(_T("%s踏板%s"), eBtnStep == CArmPedalDef::Left ? _T("左脚") : _T("右脚"), level == CArmExpBtnDef::Level1 ? _T("一级") : _T("二级"), bPressed ? _T("踏下") : _T("松开"));

    AppendMessage(str);
}

//各轴启停变化处理函数
void CMobileCArmTestDlg::OnAxisStatusChanged(CArmAxisDef eAxis, CArmAxisMotionDef eStatus)
{
    static std::map<CArmAxisDef, CString> axisMap =
    {
        { CArmAxisDef::CArm, _T("C型臂")},
        { CArmAxisDef::ForBackward, _T("前后轴") },
        { CArmAxisDef::LiftingCol, _T("升降柱") }
    };

    static std::map<CArmAxisMotionDef, CString> motionStatus =
    {
        { CArmAxisMotionDef::Moving, _T("开始运动") },
        { CArmAxisMotionDef::Stopped, _T("停止运动") }
    };

    CString str;
    str.Format(_T("%s%s"), axisMap[eAxis], motionStatus[eStatus]);

    AppendMessage(str);

    if (eAxis == CArmAxisDef::CArm)
    {
        if (m_bAxisContMovingY) SetDlgItemText(IDC_BUTTON_CONT_MOVE_Y, eStatus == CArmAxisMotionDef::Moving ? _T("停止") : _T("连续运动"));
        else if (m_bAxisMovingY) SetDlgItemText(IDC_BUTTON_MOVE_Y_TO, eStatus == CArmAxisMotionDef::Moving ? _T("停止") : _T("运动到"));

        if (eStatus == CArmAxisMotionDef::Stopped)
        {
            m_bAxisContMovingY = false;
            m_bAxisMovingY = false;
        }
    }
    else if (eAxis == CArmAxisDef::LiftingCol)
    {
        if (m_bAxisContMovingZ) SetDlgItemText(IDC_BUTTON_CONT_MOVE_Z, eStatus == CArmAxisMotionDef::Moving ? _T("停止") : _T("连续运动"));
        if (m_bAxisMovingZ) SetDlgItemText(IDC_BUTTON_MOVE_Z_TO, eStatus == CArmAxisMotionDef::Moving ? _T("停止") : _T("运动到"));

        if (eStatus == CArmAxisMotionDef::Stopped)
        {
            m_bAxisContMovingZ = false;
            m_bAxisMovingZ = false;
        }
    }
    else if (eAxis == CArmAxisDef::ForBackward)
    {
        if (m_bAxisContMovingX) SetDlgItemText(IDC_BUTTON_CONT_MOVE_X, eStatus == CArmAxisMotionDef::Moving ? _T("停止") : _T("连续运动"));
        if (m_bAxisMovingX) SetDlgItemText(IDC_BUTTON_MOVE_X_TO, eStatus == CArmAxisMotionDef::Moving ? _T("停止") : _T("运动到"));

        if (eStatus == CArmAxisMotionDef::Stopped)
        {
            m_bAxisContMovingX = false;
            m_bAxisMovingX = false;
        }
    }
}

//限束器运动到位消息处理函数
void CMobileCArmTestDlg::OnCArmCollimatorInPlace(OptionCollimator coll, float fPos)
{
    static std::map<OptionCollimator, CString> collName =
    {
        { OptionCollimator::Hor, _T("水平限束器") },
        { OptionCollimator::Ver, _T("垂直限束器") }
    };

    CString strMsg, strPos;
    strPos.Format(_T("%.2f"), fPos);
    strMsg.Format(_T("%s运动到位,当前位置：%smm"), collName[coll], strPos);

    AppendMessage(strMsg);

    SetDlgItemText(coll == OptionCollimator::Hor ? IDC_EDIT_POS_HOR : IDC_EDIT_POS_VER, strPos);
}

//各轴自动手动状态变化处理函数
void CMobileCArmTestDlg::OnCArmAxisManAutoChanged(CArmAxisDef eAxis, CArmAxisCtrlDef eStatus)
{
    static std::map<CArmAxisDef, CString> axisMap =
    {
        { CArmAxisDef::CArm, _T("C型臂") },
        { CArmAxisDef::ForBackward, _T("前后轴") },
        { CArmAxisDef::LiftingCol, _T("升降柱") }
    };

    static std::map<CArmAxisDef, std::pair<int, int>> axisCtrl =
    {
        { CArmAxisDef::CArm, { IDC_RADIO_Y_AUTO, IDC_RADIO_Y_MANU} },
        { CArmAxisDef::ForBackward,{ IDC_RADIO_X_AUTO, IDC_RADIO_X_MANU } },
        { CArmAxisDef::LiftingCol,{ IDC_RADIO_Z_AUTO, IDC_RADIO_Z_MANU } }
    };

    static std::map<CArmAxisCtrlDef, CString> ctrlStatus =
    {
        { CArmAxisCtrlDef::Auto, _T("自动模式") },
        { CArmAxisCtrlDef::Manu, _T("手动模式") }
    };

    CString str;
    str.Format(_T("%s改为%s"), axisMap[eAxis], ctrlStatus[eStatus]);

    AppendMessage(str);

    ((CButton*)GetDlgItem(axisCtrl[eAxis].first))->SetCheck(eStatus == CArmAxisCtrlDef::Auto);
    ((CButton*)GetDlgItem(axisCtrl[eAxis].second))->SetCheck(eStatus != CArmAxisCtrlDef::Auto);
}

//激光灯开关状态变化处理函数
void CMobileCArmTestDlg::OnCArmLaserStatusChanged(OptionLaserSwitch eLaser, bool bOn)
{
    static std::map<OptionLaserSwitch, CString> laserMap =
    {
        { OptionLaserSwitch::All, _T("全部") },
        { OptionLaserSwitch::CrossCurve, _T("十字线") },
        { OptionLaserSwitch::ViewRange, _T("视野范围") }
    };

    static std::map<OptionLaserSwitch, int> laserIDMap =
    {
        { OptionLaserSwitch::All, IDC_CHECK_ALL_LASER },
        { OptionLaserSwitch::CrossCurve, IDC_CHECK_LASER_CROSS },
        { OptionLaserSwitch::ViewRange, IDC_CHECK_LASER_VIEW_RANGE }
    };

    CString str;
    str.Format(_T("%s激光灯%s"), laserMap[eLaser], bOn ? _T("打开") : _T("关闭"));

    AppendMessage(str);

    ((CButton*)GetDlgItem(laserIDMap[eLaser]))->SetCheck(bOn);

    (eLaser == OptionLaserSwitch::CrossCurve) ? (m_bLaserCrossCurveOn = bOn) : (m_bLaserViewRangeOn = bOn);

    if (bOn)
    {
        std::thread thd(&CMobileCArmTestDlg::LaserStatusQueryThd, this, eLaser);
        thd.detach();
    }
}

//各轴位置变化时发出的实时位置信息处理函数
void CMobileCArmTestDlg::OnCArmAxisPosChanged(CArmAxisDef eAxis, float fPos)
{
    static std::map<CArmAxisDef, CString> axisMap =
    {
        { CArmAxisDef::CArm, _T("C型臂") },
        { CArmAxisDef::ForBackward, _T("前后轴") },
        { CArmAxisDef::LiftingCol, _T("升降柱") }
    };

    static std::map<CArmAxisDef, int> axisIDMap =
    {
        { CArmAxisDef::CArm, IDC_STATIC_CURR_POS_Y },
        { CArmAxisDef::ForBackward, IDC_STATIC_CURR_POS_X },
        { CArmAxisDef::LiftingCol, IDC_STATIC_CURR_POS_Z }
    };

    CString strMsg, strPos;

    if (eAxis == CArmAxisDef::CArm)
    {
        strPos.Format(_T("当前角度: %.2f°"), fPos);
        strMsg.Format(_T("%s角度变化: %s°"), axisMap[eAxis], strPos);
    }
    else if(eAxis == CArmAxisDef::ForBackward)
    {
        strPos.Format(_T("当前位置: %.2fmm"), fPos);
        strMsg.Format(_T("%s位置变化: %smm"), axisMap[eAxis], strPos);
    }
    else
    {
        strPos.Format(_T("当前高度: %.2fmm"), fPos);
        strMsg.Format(_T("%s高度变化: %smm"), axisMap[eAxis], strPos);
    }

    AppendMessage(strMsg);

    SetDlgItemText(axisIDMap[eAxis], strPos);
}

//设备复位完成后的回调函数
void CMobileCArmTestDlg::OnCArmDevResetCompleted(ErrorCode res)
{
    CString strMsg;
    strMsg.Format(_T("设备复位完成，复位结果：%04X"), (int)res);

    AppendMessage(strMsg, IS_OK(res) ? 0 : COLOR_FAIL);
}

//收到新影像
void CMobileCArmTestDlg::OnNewImageReceived(int nIdx, int nWidth, int nHeight, unsigned short*pImageData)
{
    AppendMessage(FormatString(_T("收到影像 - %d  宽度-%d  高度-%d"), nIdx + 1, nWidth, nHeight));

    if (pImageData != nullptr)
    {
        delete[] pImageData;
        pImageData = nullptr;
    }
}

#pragma endregion

#pragma region 对话框消息处理函数

void CMobileCArmTestDlg::OnBnClickedOk()
{
    
    //CDialogEx::OnOK();
}

void CMobileCArmTestDlg::OnBnClickedCancel()
{
    //主动退出则弹窗询问是否确定要退出
    if (!m_bForceExit && MessageBox(_T("确定要退出？"), _T("提示"), MB_YESNO) != IDYES) return;

    //主动退出要通知Laucher进程
    if (!m_bForceExit) m_DevComm.ExitApp();

    //m_DevComm.Uninitialize();

    CDialogEx::OnCancel();
}

void CMobileCArmTestDlg::OnBnClickedButtonDevRst()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("开始复位设备"));
    ErrorCode res = m_DevComm.ResetDevice();

    if (IS_OK(res))
    {
        AppendMessage(_T("设备复位成功"));

        m_bAxisMovingX = false;
        m_bAxisMovingY = false;
        m_bAxisMovingZ = false;

        m_bAxisContMovingX = false;
        m_bAxisContMovingY = false;
        m_bAxisContMovingZ = false;

        SetDlgItemText(IDC_BUTTON_CONT_MOVE_X, _T("连续运动"));
        SetDlgItemText(IDC_BUTTON_CONT_MOVE_Y, _T("连续运动"));
        SetDlgItemText(IDC_BUTTON_CONT_MOVE_Z, _T("连续运动"));

        SetDlgItemText(IDC_BUTTON_MOVE_X_TO, _T("运动到"));
        SetDlgItemText(IDC_BUTTON_MOVE_Y_TO, _T("运动到"));
        SetDlgItemText(IDC_BUTTON_MOVE_Z_TO, _T("运动到"));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("设备复位失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonWarnRst()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("开始设备报警复位"));
    ErrorCode res = m_DevComm.ResetWarn();

    if (IS_OK(res))
        AppendMessage(_T("设备报警复位成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设备报警复位失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedCheckLaserCross()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    bool bChecked = ((CButton*)GetDlgItem(IDC_CHECK_LASER_CROSS))->GetCheck();

    CString strAct = bChecked ? _T("打开") : _T("关闭");

    AppendMessage(strAct + _T("十字线激光灯"));

    ErrorCode res = m_DevComm.LaserSwitch(OptionLaserSwitch::CrossCurve, bChecked);

    if (IS_OK(res))
        AppendMessage(strAct + _T("十字线激光灯成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("十字线激光灯失败，错误码：%04X"), (int)res);

        AppendMessage(strAct + strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonGetLsrStsCross()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取十字线激光灯状态"));

    bool bOn = false;
    float fOffDelay = 0;

    ErrorCode res = m_DevComm.GetLaserStatus(OptionLaserSwitch::CrossCurve, bOn, fOffDelay);

    if (IS_OK(res))
    {
        AppendMessage(_T("获取十字线激光灯状态成功"));

        ((CButton*)GetDlgItem(IDC_CHECK_LASER_CROSS))->SetCheck(bOn);
        SetDlgItemText(IDC_STATIC_OFF_DELAY_CROSS, FormatString(_T("剩余关闭延时：%.2fs"), fOffDelay));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取十字线激光灯状态失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedCheckLaserViewRange()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    bool bChecked = ((CButton*)GetDlgItem(IDC_CHECK_LASER_VIEW_RANGE))->GetCheck();

    CString strAct = bChecked ? _T("打开") : _T("关闭");

    AppendMessage(strAct + _T("视野范围激光灯"));

    ErrorCode res = m_DevComm.LaserSwitch(OptionLaserSwitch::ViewRange, bChecked);

    if (IS_OK(res))
        AppendMessage(strAct + _T("视野范围激光灯成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("视野范围激光灯失败，错误码：%04X"), (int)res);

        AppendMessage(strAct + strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonGetLsrStsView()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取视野范围激光灯状态"));

    bool bOn = false;
    float fOffDelay = 0;

    ErrorCode res = m_DevComm.GetLaserStatus(OptionLaserSwitch::ViewRange, bOn, fOffDelay);

    if (IS_OK(res))
    {
        AppendMessage(_T("获取视野范围激光灯状态成功"));

        ((CButton*)GetDlgItem(IDC_CHECK_LASER_VIEW_RANGE))->SetCheck(bOn);
        SetDlgItemText(IDC_STATIC_OFF_DELAY_VIEW, FormatString(_T("剩余关闭延时：%.2fs"), fOffDelay));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取视野范围激光灯状态失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonSetKv()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    float fVal = 0;
    if (!GetKV(fVal)) return;

    AppendMessage(_T("设置管电压"));

    ErrorCode res = m_DevComm.SetKV(fVal);

    if (IS_OK(res))
        AppendMessage(_T("设置管电压成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置管电压失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonGetKv()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取当前管电压"));

    float fVal = 0;
    ErrorCode res = m_DevComm.GetKV(fVal);

    if (IS_OK(res))
    {
        AppendMessage(_T("获取当前管电压成功"));
        SetDlgItemText(IDC_EDIT_KV, FormatString(_T("%.2f"), fVal));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取当前管电压失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonSetMa()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    float fVal = 0;
    if (!GetMA(fVal)) return;

    AppendMessage(_T("设置管电流"));

    ErrorCode res = m_DevComm.SetMA(fVal);

    if (IS_OK(res))
        AppendMessage(_T("设置管电流成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置管电流失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonGetMa()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取当前管电流"));

    float fVal = 0;
    ErrorCode res = m_DevComm.GetMA(fVal);

    if (IS_OK(res))
    {
        AppendMessage(_T("获取当前管电流成功"));
        SetDlgItemText(IDC_EDIT_MA, FormatString(_T("%.2f"), fVal));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取当前管电流失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonSetFps()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    int nVal;
    if (!GetFPS(nVal)) return;

    AppendMessage(_T("设置帧率"));

    ErrorCode res = m_DevComm.SetFPS(nVal);

    if (IS_OK(res))
        AppendMessage(_T("设置帧率成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置帧率失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonSetFrame()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    int nFramNum = 0;
    if (!GetFrameNum(nFramNum)) return;

    AppendMessage(_T("设置采集帧数"));

    ErrorCode res = m_DevComm.SetFrameNum(nFramNum);

    if (IS_OK(res))
        AppendMessage(_T("设置采集帧数成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置采集帧数失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonSetExp()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    int nExp = 0;
    if (!GetExposure(nExp)) return;

    AppendMessage(_T("设置曝光时间"));

    ErrorCode res = m_DevComm.SetExposure(nExp);

    if (IS_OK(res))
        AppendMessage(_T("设置曝光时间成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置曝光时间失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonScan2d()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    DevWorkMode workMode = ((CButton*)GetDlgItem(IDC_RADIO_NORMAL))->GetCheck() ? DevWorkMode::Normal : DevWorkMode::Debug;
    if (workMode == DevWorkMode::Normal && !SaveUIScanCfg()) return;

    AppendMessage(_T("开始2D扫描"));

    ErrorCode res = m_DevComm.StartScan2D(workMode, ((CButton*)GetDlgItem(IDC_CHECK_XRAY))->GetCheck());

    if (IS_OK(res))
        AppendMessage(_T("开始2D扫描成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("开始2D扫描失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonScan3d()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    DevWorkMode workMode = ((CButton*)GetDlgItem(IDC_RADIO_NORMAL))->GetCheck() ? DevWorkMode::Normal : DevWorkMode::Debug;
    if (workMode == DevWorkMode::Normal && !SaveUIScanCfg()) return;

    AppendMessage(_T("开始3D扫描"));

    CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_C_SCAN_DIRECT);
    OptionCArmMove moveDirect = (OptionCArmMove)pBox->GetItemData(pBox->GetCurSel());

    ErrorCode res = m_DevComm.StartScan3D(workMode, moveDirect, ((CButton*)GetDlgItem(IDC_CHECK_XRAY))->GetCheck());

    if (IS_OK(res))
        AppendMessage(_T("开始3D扫描成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("开始3D扫描失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonSetVer()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    CString strVal;
    GetDlgItemText(IDC_EDIT_POS_VER, strVal);

    if (strVal.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的垂直限束器位置无效"), COLOR_FAIL);
        return;
    }

    float fVal = _tstof(strVal);
    if (fVal <= 0)
    {
        AppendMessage(_T("输入的垂直限束器位置无效"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置垂直限束器位置"));

    ErrorCode res = m_DevComm.SetCollimator(OptionCollimator::Ver,10, fVal);

    if (IS_OK(res))
        AppendMessage(_T("设置垂直限束器位置成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置垂直限束器位置失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonSetHor()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    CString strVal;
    GetDlgItemText(IDC_EDIT_POS_HOR, strVal);

    if (strVal.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的水平限束器位置无效"), COLOR_FAIL);
        return;
    }

    float fVal = _tstof(strVal);
    if (fVal <= 0)
    {
        AppendMessage(_T("输入的水平限束器位置无效"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置水平限束器位置"));

    ErrorCode res = m_DevComm.SetCollimator(OptionCollimator::Hor, 10,fVal);

    if (IS_OK(res))
        AppendMessage(_T("设置水平限束器位置成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置水平限束器位置失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedCheckEnableDap()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    bool bEnable = ((CButton*)GetDlgItem(IDC_CHECK_ENABLE_DAP))->GetCheck();
    CString strAct = bEnable ? _T("启用") : _T("禁用");

    AppendMessage(strAct + _T("DAP"));

    ErrorCode res = m_DevComm.EnableDAP(bEnable);

    if (IS_OK(res))
        AppendMessage(strAct + _T("DAP成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("DAP失败，错误码：%04X"), (int)res);

        AppendMessage(strAct + strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonReadDap()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取DAP读数"));

    DAPValue fVal{};
    ErrorCode res = m_DevComm.GetDAPValue(fVal);

    if (IS_OK(res))
    {
        AppendMessage(FormatString(_T("获取DAP读数成功:%.2f"), fVal.DAP));
        SetDlgItemText(IDC_STATIC_DAP_VALUE, FormatString(_T("DAP读数：%.2f"), fVal.DAP));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取DAP读数失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonReadMbTemp()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取主控板温度"));

    float fVal = 0;
    ErrorCode res = m_DevComm.GetMainboardTemp(fVal);

    if (IS_OK(res))
    {
        AppendMessage(FormatString(_T("获取主控板温度成功:%.2f"), fVal));
        SetDlgItemText(IDC_STATIC_MB_TEMP, FormatString(_T("主控板温度：%.2f"), fVal));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取主控板温度失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedRadioXAuto()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置前后轴为自动模式"));

    float fVal = 0;
    ErrorCode res = m_DevComm.SetManuAxis(CArmAxisDef::ForBackward, CArmAxisCtrlDef::Auto);

    if (IS_OK(res))
        AppendMessage(FormatString(_T("设置前后轴为自动模式成功:%.2f"), fVal));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置前后轴为自动模式失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedRadioXManu()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置前后轴为手动模式"));

    float fVal = 0;
    ErrorCode res = m_DevComm.SetManuAxis(CArmAxisDef::ForBackward, CArmAxisCtrlDef::Manu);

    if (IS_OK(res))
        AppendMessage(FormatString(_T("设置前后轴为手动模式成功:%.2f"), fVal));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置前后轴为手动模式失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonGetPosX()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取前后轴的位置"));

    float fVal = 0;
    ErrorCode res = m_DevComm.GetPosAxis(CArmAxisDef::ForBackward, fVal);

    if (IS_OK(res))
    {
        AppendMessage(FormatString(_T("获取前后轴位置成功: %.2fmm"), fVal));
        SetDlgItemText(IDC_STATIC_CURR_POS_X, FormatString(_T("当前位置: %.2fmm"), fVal));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取前后轴位置失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonContMoveX()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    if (m_bAxisMovingX) return;

    if (m_bAxisContMovingX)
    {
        ErrorCode res = m_DevComm.MoveAxisX(OptionXAxisMove::Stop, 0);

        if (IS_OK(res))
        {
            AppendMessage(_T("停止前后轴连续运动成功"));
            SetDlgItemText(IDC_BUTTON_CONT_MOVE_X, _T("正在停止"));
        }
        else
        {
            CString strMsg;
            strMsg.Format(_T("停止前后轴连续运动失败，错误码：%04X"), (int)res);

            AppendMessage(strMsg);
        }

        return;
    }

    CString strVal;
    GetDlgItemText(IDC_EDIT_SPEED_X, strVal);

    if (strVal.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的前后轴运动速度无效"), COLOR_FAIL);
        return;
    }

    float fVal = _tstof(strVal);
    if (fVal <= 0)
    {
        AppendMessage(_T("输入的前后轴运动速度无效"), COLOR_FAIL);
        return;
    }

    CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_X_DIRECT);
    int nSel = pBox->GetCurSel();
    if (nSel < 0)
    {
        AppendMessage(_T("未选择前后轴运动方向"), COLOR_FAIL);
        return;
    }

    OptionXAxisMove moveMode = (OptionXAxisMove)pBox->GetItemData(nSel);

    AppendMessage(_T("启动前后轴连续运动"));

    ErrorCode res = m_DevComm.MoveAxisX(moveMode, fVal);

    if (IS_OK(res))
    {
        AppendMessage(_T("启动前后轴连续运动成功"));

        m_bAxisContMovingX = true;
        SetDlgItemText(IDC_BUTTON_CONT_MOVE_X, _T("启动中"));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("启动前后轴连续运动失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonMoveXTo()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    if (m_bAxisContMovingX) return;

    if (m_bAxisMovingX)
    {
        ErrorCode res = m_DevComm.MoveAxisX(OptionXAxisMove::Stop, 0);

        if (IS_OK(res))
        {
            AppendMessage(_T("停止前后轴定点移动成功"));
            SetDlgItemText(IDC_BUTTON_MOVE_X_TO, _T("正在停止"));
        }
        else
        {
            CString strMsg;
            strMsg.Format(_T("停止前后轴定点移动失败，错误码：%04X"), (int)res);

            AppendMessage(strMsg);
        }

        return;
    }

    CString strSpeed;
    GetDlgItemText(IDC_EDIT_SPEED_X, strSpeed);

    if (strSpeed.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strSpeed, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的前后轴运动速度无效"), COLOR_FAIL);
        return;
    }

    float fSpeed = _tstof(strSpeed);
    if (fSpeed <= 0)
    {
        AppendMessage(_T("输入的前后轴运动速度无效"), COLOR_FAIL);
        return;
    }

    CString strPos;
    GetDlgItemText(IDC_EDIT_DEST_POS_X, strPos);

    if (strPos.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strPos, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的前后轴目标位置无效"), COLOR_FAIL);
        return;
    }

    float fPos = _tstof(strPos);
    if (fPos <= 0)
    {
        AppendMessage(_T("输入的前后轴目标位置无效"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("移动前后轴到目标位置"));

    ErrorCode res = m_DevComm.SetPosAxis(CArmAxisDef::ForBackward, fSpeed, fPos);

    if (IS_OK(res))
    {
        m_bAxisMovingX = true;

        AppendMessage(_T("移动前后轴到目标位置成功"));
        SetDlgItemText(IDC_BUTTON_MOVE_X_TO, _T("启动中"));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("移动前后轴到目标位置失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedRadioZAuto()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置升降柱为自动模式"));

    float fVal = 0;
    ErrorCode res = m_DevComm.SetManuAxis(CArmAxisDef::LiftingCol, CArmAxisCtrlDef::Auto);

    if (IS_OK(res))
        AppendMessage(FormatString(_T("设置升降柱为自动模式成功:%.2f"), fVal));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置升降柱为自动模式失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedRadioZManu()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置升降柱手动模式"));

    float fVal = 0;
    ErrorCode res = m_DevComm.SetManuAxis(CArmAxisDef::LiftingCol, CArmAxisCtrlDef::Manu);

    if (IS_OK(res))
        AppendMessage(FormatString(_T("设置升降柱为手动模式成功:%.2f"), fVal));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置升降柱为手动模式失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonGetPosZ()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取升降柱的高度"));

    float fVal = 0;
    ErrorCode res = m_DevComm.GetPosAxis(CArmAxisDef::LiftingCol, fVal);

    if (IS_OK(res))
    {
        AppendMessage(FormatString(_T("获取升降柱高度成功: %.2fmm"), fVal));
        SetDlgItemText(IDC_STATIC_CURR_POS_Z, FormatString(_T("当前高度: %.2fmm"), fVal));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("获取升降柱高度失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonContMoveZ()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    if (m_bAxisMovingZ) return;

    if (m_bAxisContMovingZ)
    {
        ErrorCode res = m_DevComm.MoveAxisZ(OptionLiftColMove::Stop, 0);

        if (IS_OK(res))
        {
            AppendMessage(_T("停止升降柱连续升降成功"));
            SetDlgItemText(IDC_BUTTON_CONT_MOVE_Z, _T("正在停止"));
        }
        else
        {
            CString strMsg;
            strMsg.Format(_T("停止升降柱连续升降失败，错误码：%04X"), (int)res);

            AppendMessage(strMsg, COLOR_FAIL);
        }

        return;
    }

    CString strVal;
    GetDlgItemText(IDC_EDIT_SPEED_Z, strVal);

    if (strVal.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的升降柱升降速度无效"), COLOR_FAIL);
        return;
    }

    float fVal = _tstof(strVal);
    if (fVal <= 0)
    {
        AppendMessage(_T("输入的升降柱升降速度无效"), COLOR_FAIL);
        return;
    }

    CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_Z_DIRECT);
    int nSel = pBox->GetCurSel();
    if (nSel < 0)
    {
        AppendMessage(_T("未选择升降柱升降方向"), COLOR_FAIL);
        return;
    }

    OptionLiftColMove m = (OptionLiftColMove)pBox->GetItemData(nSel);

    AppendMessage(_T("启动升降柱连续升降"));

    ErrorCode res = m_DevComm.MoveAxisZ(m, fVal);

    if (IS_OK(res))
    {
        m_bAxisContMovingZ = true;

        AppendMessage(_T("启动升降柱连续升降成功"));
        SetDlgItemText(IDC_BUTTON_CONT_MOVE_Z, _T("启动中"));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("启动升降柱连续升降失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonMoveZTo()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    if (m_bAxisContMovingZ) return;

    if (m_bAxisMovingZ)
    {
        ErrorCode res = m_DevComm.MoveAxisZ(OptionLiftColMove::Stop, 0);

        if (IS_OK(res))
        {
            AppendMessage(_T("停止升降柱定点升降成功"));
            SetDlgItemText(IDC_BUTTON_MOVE_Z_TO, _T("正在停止"));
        }
        else
        {
            CString strMsg;
            strMsg.Format(_T("停止升降柱定点升降失败，错误码：%04X"), (int)res);

            AppendMessage(strMsg);
        }

        return;
    }

    CString strSpeed;
    GetDlgItemText(IDC_EDIT_SPEED_Z, strSpeed);

    if (strSpeed.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strSpeed, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的升降柱升降速度无效"), COLOR_FAIL);
        return;
    }

    float fSpeed = _tstof(strSpeed);
    if (fSpeed <= 0)
    {
        AppendMessage(_T("输入的升降柱升降速度无效"), COLOR_FAIL);
        return;
    }

    CString strPos;
    GetDlgItemText(IDC_EDIT_DEST_POS_Z, strPos);

    if (strPos.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strPos, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的升降柱目标高度无效"), COLOR_FAIL);
        return;
    }

    float fPos = _tstof(strPos);
    if (fPos <= 0)
    {
        AppendMessage(_T("输入的升降柱目标高度无效"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("移动升降柱到目标高度"));

    ErrorCode res = m_DevComm.SetPosAxis(CArmAxisDef::LiftingCol, fSpeed, fPos);

    if (IS_OK(res))
    {
        m_bAxisMovingZ = true;

        AppendMessage(_T("移动升降柱到目标高度成功"));
        SetDlgItemText(IDC_BUTTON_MOVE_Z_TO, _T("启动中"));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("移动升降柱到目标高度失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedRadioYAuto()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置C型臂为自动模式"));

    float fVal = 0;
    ErrorCode res = m_DevComm.SetManuAxis(CArmAxisDef::CArm, CArmAxisCtrlDef::Auto);

    if (IS_OK(res))
        AppendMessage(FormatString(_T("设置C型臂为自动模式成功:%.2f"), fVal));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置C型臂为自动模式失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedRadioYManu()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("设置C型臂为手动模式"));

    float fVal = 0;
    ErrorCode res = m_DevComm.SetManuAxis(CArmAxisDef::CArm, CArmAxisCtrlDef::Manu);

    if (IS_OK(res))
        AppendMessage(FormatString(_T("设置C型臂为手动模式成功:%.2f"), fVal));
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置C型臂为手动模式失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonGetPosY()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("获取C型臂的当前角度"));

    float fVal = 0;
    ErrorCode res = m_DevComm.GetPosAxis(CArmAxisDef::CArm, fVal);

    if (IS_OK(res))
    {
        AppendMessage(FormatString(_T("获取C型臂当前角度成功: %.2f°"), fVal));
        SetDlgItemText(IDC_STATIC_CURR_POS_Y, FormatString(_T("当前角度: %.2f°"), fVal));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("设置C型臂当前角度失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonContMoveY()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    if (m_bAxisMovingY) return;

    if (m_bAxisContMovingY)
    {
        ErrorCode res = m_DevComm.MoveAxisY(OptionCArmMove::Stop, 0);

        if (IS_OK(res))
        {
            AppendMessage(_T("停止C型臂连续旋转成功"));
            SetDlgItemText(IDC_BUTTON_CONT_MOVE_Y, _T("正在停止"));
        }
        else
        {
            CString strMsg;
            strMsg.Format(_T("停止C型臂连续旋转失败，错误码：%04X"), (int)res);

            AppendMessage(strMsg, COLOR_FAIL);
        }

        return;
    }

    CString strVal;
    GetDlgItemText(IDC_EDIT_SPEED_Y, strVal);

    if (strVal.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strVal, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的C型臂旋转速度无效"), COLOR_FAIL);
        return;
    }

    float fVal = _tstof(strVal);
    if (fVal <= 0)
    {
        AppendMessage(_T("输入的C型臂旋转速度无效"), COLOR_FAIL);
        return;
    }

    CComboBox *pBox = (CComboBox*)GetDlgItem(IDC_COMBO_Y_DIRECT);
    int nSel = pBox->GetCurSel();
    if (nSel < 0)
    {
        AppendMessage(_T("未选择C型臂旋转方向"), COLOR_FAIL);
        return;
    }

    OptionCArmMove modeMove = (OptionCArmMove)pBox->GetItemData(nSel);

    AppendMessage(_T("启动C型臂连续旋转"));

    ErrorCode res = m_DevComm.MoveAxisY(modeMove, fVal);

    if (IS_OK(res))
    {
        m_bAxisContMovingY = true;

        AppendMessage(_T("启动C型臂连续旋转成功"));
        SetDlgItemText(IDC_BUTTON_CONT_MOVE_Y, _T("启动中"));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("启动C型臂连续旋转失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedButtonMoveYTo()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    if (m_bAxisContMovingY) return;

    if (m_bAxisMovingY)
    {
        ErrorCode res = m_DevComm.MoveAxisY(OptionCArmMove::Stop, 0);

        if (IS_OK(res))
        {
            AppendMessage(_T("停止C型臂连续旋转成功"));
            SetDlgItemText(IDC_BUTTON_MOVE_Y_TO, _T("正在停止"));
        }
        else
        {
            CString strMsg;
            strMsg.Format(_T("停止C型臂连续旋转失败，错误码：%04X"), (int)res);

            AppendMessage(strMsg, COLOR_FAIL);
        }

        return;
    }

    CString strSpeed;
    GetDlgItemText(IDC_EDIT_SPEED_Y, strSpeed);

    if (strSpeed.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strSpeed, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的C型臂旋转速度无效"), COLOR_FAIL);
        return;
    }

    float fSpeed = _tstof(strSpeed);
    if (fSpeed <= 0)
    {
        AppendMessage(_T("输入的C型臂旋转速度无效"), COLOR_FAIL);
        return;
    }

    CString strPos;
    GetDlgItemText(IDC_EDIT_DEST_POS_Y, strPos);

    if (strPos.IsEmpty()) return;

    if (!std::regex_match((LPCTSTR)strPos, std::wregex(L"^[+-]?([0-9]*\.?[0-9]+|[0-9]+\.?[0-9]*)([eE][+-]?[0-9]+)?$")))
    {
        AppendMessage(_T("输入的C型臂目标角度无效"), COLOR_FAIL);
        return;
    }

    float fPos = _tstof(strPos);
    if (fPos <= 0)
    {
        AppendMessage(_T("输入的C型臂目标角度无效"), COLOR_FAIL);
        return;
    }

    AppendMessage(_T("转动C型臂到目标角度"));

    ErrorCode res = m_DevComm.SetPosAxis(CArmAxisDef::CArm, fSpeed, fPos);

    if (IS_OK(res))
    {
        m_bAxisMovingY = true;

        AppendMessage(_T("转动C型臂到目标角度成功"));
        SetDlgItemText(IDC_BUTTON_MOVE_Y_TO, _T("启动中"));
    }
    else
    {
        CString strMsg;
        strMsg.Format(_T("转动C型臂到目标角度失败，错误码：%04X"), (int)res);

        AppendMessage(strMsg, COLOR_FAIL);
    }
}

BOOL CMobileCArmTestDlg::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_ESCAPE)
            return TRUE;
    }

    return CDialogEx::PreTranslateMessage(pMsg);
}

void CMobileCArmTestDlg::OnBnClickedCheckDisableTubeCtrl()
{
    if (!m_DevComm.IsDevProcConnected())
    {
        AppendMessage(_T("设备未连接"), COLOR_FAIL);
        return;
    }

    bool bEnable = ((CButton*)GetDlgItem(IDC_CHECK_DISABLE_TUBE_CTRL))->GetCheck();
    CString strAct = bEnable ? _T("暂停") : _T("恢复");

    AppendMessage(strAct + _T("球管控制"));

    ErrorCode res = m_DevComm.EnableTubeCtrl(!bEnable);

    if (IS_OK(res))
        AppendMessage(strAct + _T("球管控制成功"));
    else
    {
        CString strMsg;
        strMsg.Format(_T("球管控制失败，错误码：%04X"), (int)res);

        AppendMessage(strAct + strMsg, COLOR_FAIL);
    }
}

void CMobileCArmTestDlg::OnBnClickedAllLaserSwitchOn()
{
    AllLaserSwitch(true, _T("打开"));
}

void CMobileCArmTestDlg::OnBnClickedAllLaserSwitchOff()
{
    AllLaserSwitch(false, _T("关闭"));
}

#pragma endregion

