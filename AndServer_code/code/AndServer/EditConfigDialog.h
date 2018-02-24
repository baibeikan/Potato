#pragma once


// CEditConfigDialog �Ի���

class CEditConfigDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEditConfigDialog)

public:
	CEditConfigDialog(IConfig* pConfig, vector<IScanUpdate>* pScanUpdate, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEditConfigDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_CONFIG };

public:
	CListBox m_addr;
	CIPAddressCtrl m_addrs;
	CIPAddressCtrl m_addre;
	CEdit m_scan_port;
	CEdit m_open_port;
	CEdit m_open_port_c;
	CEdit m_scan_time;
	CEdit m_task_time;
	CEdit m_task_count;

protected:
	IConfig* m_pConfig = NULL;
	vector<IScanUpdate>* m_pScanUpdate = NULL;

private:
	BOOL updataScanUpdate(int v);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnLbnSelchangeScaneUpdate();
	virtual BOOL OnInitDialog();
};
