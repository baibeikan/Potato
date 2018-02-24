#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "INet.h"

// CEditDeviceDialog 对话框

class CEditDeviceDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEditDeviceDialog)

public:
	CEditDeviceDialog(BOOL isEdit, vector<IDeviceData>* pDevices, CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CEditDeviceDialog();

// 对话框数据
	enum { IDD = IDD_DIALOG_DEVICE };

public:
	CIPAddressCtrl m_addr;
	CEdit m_mac;
	CEdit m_port;
	CEdit m_area;
	CEdit m_exhibit;
	CComboBox m_type;
	CEdit m_code_state;
	CEdit m_code_state_return;
	CEdit m_code_on;
	CEdit m_code_off;
	CButton m_prev;
	CButton m_next;

protected:
	BOOL m_isEdit = FALSE;
	int m_index = 0;
	vector<IDeviceData>* m_pDevices = NULL;

private:
	void updataUI();
	BOOL updataData();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonPrev();
	afx_msg void OnBnClickedButtonNext();
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnCbnSelchangeComboType();
	virtual BOOL OnInitDialog();
};
