
// MFCApplication1Dlg.h : 头文件
//

#pragma once
#include "shockwaveflash1.h"
#include "GameSystem.h"

#define WM_FLASH_STOP (WM_USER+100)
#define WM_FLASH_PLAY (WM_USER+101)

// CMFCApplication1Dlg 对话框
class CMFCApplication1Dlg : public CDialogEx, GameSystem
{
// 构造
public:
	CMFCApplication1Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCAPPLICATION1_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

private:
	void OnInital();

	void OnUpdate(float deltaTime);

	void OnRelease();

	BOOL PreTranslateMessage(MSG* pMsg);

	void SwitchGameStatus(GAME_STATUS eGameStatus);

	void EnterIdle() override;

	void LeaveIdle() override;

	virtual HWND GetWindowHandle() override;


private:
	static DWORD MainThread(LPVOID lParam);

private:
	//GameSystem* m_pGameSystem;
	//GAME_STATUS m_eGameStatus;

	HANDLE m_hThread;
	bool m_bIsMainThreadRun;
	int m_nUpdate;
	int m_nRender;

// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CShockwaveflash1 m_Shockwaveflash;
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};
