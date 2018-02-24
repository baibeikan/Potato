
// AndServerDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"

#include "EditDeviceDialog.h"
#include "EditCodeBookDialog.h"
#include "EditConfigDialog.h"

#define ID_CODE_BOOK	40000

// CAndServerDlg �Ի���
class CAndServerDlg : public CDialogEx, public INet
{
// ����
public:
	CAndServerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CAndServerDlg();

// �Ի�������
	enum { IDD = IDD_ANDSERVER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

public:
	CListCtrl m_listClient;
	CEdit m_editLog;
	CEdit m_editInfo;
	CImageList m_images;

private:
	void order(long v);
	void edit(long v);
	void editCodeBook();
	void editConfig();

public:
	virtual void OnInitDevice();
	virtual void OnUpdateDeviceState(int index);
	virtual void OnUninitDevice();
	virtual void OnInitScan();
	virtual void OnUpdateScanInfo(int index);
	virtual void OnUninitScan();
	virtual void OnInitTask();
	virtual void OnUpdateTaskInfo(ITaskData task, BYTE run);
	virtual void OnUninitTask();

// ʵ��
protected:
	HICON m_hIcon = NULL;
	BOOL m_bInit = FALSE;
	CArray<HICON> m_hicoType;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult);
};
