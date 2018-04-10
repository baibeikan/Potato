
// MFCApplication1Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFCApplication1.h"
#include "MFCApplication1Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// CMFCApplication1Dlg 对话框



CMFCApplication1Dlg::CMFCApplication1Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCAPPLICATION1_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCApplication1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOCKWAVEFLASH1, m_Shockwaveflash);
}

void CMFCApplication1Dlg::OnInital()
{
	TCHAR szTempDir[MAX_PATH] = {};
	GetCurrentDirectory(MAX_PATH, szTempDir);

	CRect clientRect;
	GetClientRect(clientRect);

	TCHAR szTempPath[MAX_PATH] = {};
	_stprintf_s(szTempPath, _T("%s\\resource\\%s"), szTempDir, _T("SkeletalMovement.swf"));
	m_Shockwaveflash.MoveWindow(0, 0, 1, 1);
	m_Shockwaveflash.LoadMovie(0, szTempPath);
	//m_Shockwaveflash.Play();
	m_Shockwaveflash.Stop();
	//m_Shockwaveflash.ShowWindow(SW_HIDE);
	m_nUpdate = 0;
	m_nRender = 0;

	__super::Inital(m_hWnd);

	DWORD dwThreadID = 0;
	m_hThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)MainThread, this, 0, &dwThreadID);

	TCHAR szTempBuffer[512] = {};

	_stprintf_s(szTempBuffer, _T("线程创建ID=0x%x 0x%x"), dwThreadID, (DWORD)m_hThread);
	OutputDebugString(szTempBuffer);

	//SetTimer(0, int(1000.0f / 72 + 0.5f), NULL);
}

void CMFCApplication1Dlg::OnUpdate(float deltaTime)
{
	__super::Update(deltaTime);
	__super::Render();
}

void CMFCApplication1Dlg::OnRelease()
{
	m_bIsMainThreadRun = false;

	DWORD dwExitCode = STILL_ACTIVE;
	do 
	{
		GetExitCodeThread(m_hThread, &dwExitCode);
	} while (dwExitCode == STILL_ACTIVE);
}

BOOL CMFCApplication1Dlg::PreTranslateMessage(MSG * pMsg)
{
	//if (pMsg->message == WM_KEYDOWN)
	//{
	//	if (pMsg->wParam == 'A')
	//	{
	//		SwitchGameStatus(GAME_STATUS((m_eGameStatus + 1) % GAME_STATUS_MAX));
	//	}
	//}

	BOOL bRet = __super::PreTranslateMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
	if (bRet)
	{
		return TRUE;
	}

	switch (pMsg->message)
	{
	case WM_FLASH_STOP:
		m_Shockwaveflash.MoveWindow(0, 0, 1, 1);
		m_Shockwaveflash.Stop();
		break;
	case WM_FLASH_PLAY:
		if (1)
		{
			CRect clientRect;
			GetClientRect(clientRect);
			m_Shockwaveflash.MoveWindow(clientRect);
			m_Shockwaveflash.GotoFrame(1);
			m_Shockwaveflash.Play();
		}
		break;
	default:
		break;
	}

	return __super::PreTranslateMessage(pMsg);
}

void CMFCApplication1Dlg::SwitchGameStatus(GAME_STATUS eGameStatus)
{
	//switch (m_eGameStatus)
	//{
	//case GAME_IDLE:
	//	m_Shockwaveflash.Stop();
	//	m_Shockwaveflash.MoveWindow(0, 0, 0, 0);
	//	m_Shockwaveflash.BringWindowToTop();
	//	break;
	//case GAME_RUN:
	//	break;
	//}

	//m_eGameStatus = eGameStatus;
	//switch (m_eGameStatus)
	//{
	//case GAME_IDLE:
	//	if(1)
	//	{
	//		CRect clientRect;
	//		GetClientRect(clientRect);

	//		m_Shockwaveflash.MoveWindow(clientRect);
	//		m_Shockwaveflash.GotoFrame(1);
	//		m_Shockwaveflash.Play();
	//	}
	//	break;
	//case GAME_RUN:
	//	break;
	//}
}

void CMFCApplication1Dlg::EnterIdle()
{
	PostMessage(WM_FLASH_PLAY);
}

void CMFCApplication1Dlg::LeaveIdle()
{
	//m_Shockwaveflash.GotoFrame(1);
	//m_Shockwaveflash.Stop();
	//m_Shockwaveflash.MoveWindow(0, 0, 1, 1);
	PostMessage(WM_FLASH_STOP);
}

HWND CMFCApplication1Dlg::GetWindowHandle()
{
	return m_hWnd;
}

DWORD CMFCApplication1Dlg::MainThread(LPVOID lParam)
{
	CMFCApplication1Dlg* lpDlg = (CMFCApplication1Dlg*)lParam;
	if (lpDlg == NULL)
		return -1;

	lpDlg->m_bIsMainThreadRun = true;

	LARGE_INTEGER PerformanceFrequency;
	QueryPerformanceFrequency(&PerformanceFrequency);

	LARGE_INTEGER performanceCounter, tempPerformanceCounter;
	QueryPerformanceCounter(&performanceCounter);

	float fps = 1 / 60.0f;
	float deltaTime = 0.0f;

	while (lpDlg->m_bIsMainThreadRun)
	{
		tempPerformanceCounter = performanceCounter;
		QueryPerformanceCounter(&performanceCounter);

		float tempDeltaTime = (float)(performanceCounter.QuadPart - tempPerformanceCounter.QuadPart) / PerformanceFrequency.QuadPart;

		deltaTime += tempDeltaTime;
		if (deltaTime >= fps)
		{
			tempDeltaTime = fps;
			lpDlg->OnUpdate(tempDeltaTime);
			deltaTime -= fps;
		}
	}
	return 0;
}

BEGIN_MESSAGE_MAP(CMFCApplication1Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMFCApplication1Dlg 消息处理程序

BOOL CMFCApplication1Dlg::OnInitDialog()
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

	MoveWindow(0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));

	OnInital();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCApplication1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMFCApplication1Dlg::OnPaint()
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
HCURSOR CMFCApplication1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCApplication1Dlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (nIDEvent == 0)
	{
		//OnUpdate();
	}
	CDialogEx::OnTimer(nIDEvent);
}


void CMFCApplication1Dlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	OnRelease();
	// TODO: 在此处添加消息处理程序代码
}
