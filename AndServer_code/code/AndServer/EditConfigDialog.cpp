// CEditConfigDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "AndServer.h"
#include "EditConfigDialog.h"
#include "afxdialogex.h"


// CEditConfigDialog 对话框

IMPLEMENT_DYNAMIC(CEditConfigDialog, CDialogEx)

CEditConfigDialog::CEditConfigDialog(IConfig* pConfig, vector<IScanUpdate>* pScanUpdate, CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditConfigDialog::IDD, pParent), m_pConfig(pConfig), m_pScanUpdate(pScanUpdate)
{

}

CEditConfigDialog::~CEditConfigDialog()
{
}

void CEditConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SCAN_UPDATE, m_addr);
	DDX_Control(pDX, IDC_IPADDRESS1, m_addrs);
	DDX_Control(pDX, IDC_IPADDRESS2, m_addre);
	DDX_Control(pDX, IDC_EDIT_SCAN_PORT, m_scan_port);
	DDX_Control(pDX, IDC_EDIT_OPEN_PORT, m_open_port);
	DDX_Control(pDX, IDC_EDIT_OPEN_PORT_C, m_open_port_c);
	DDX_Control(pDX, IDC_EDIT_SCAN_TIME, m_scan_time);
	DDX_Control(pDX, IDC_EDIT_TASK_TIME, m_task_time);
	DDX_Control(pDX, IDC_EDIT_TASK_COUNT, m_task_count);
}


BEGIN_MESSAGE_MAP(CEditConfigDialog, CDialogEx)
	ON_BN_CLICKED(ID_BUTTON_ADD, &CEditConfigDialog::OnBnClickedButtonAdd)
	ON_BN_CLICKED(ID_BUTTON_EDIT, &CEditConfigDialog::OnBnClickedButtonEdit)
	ON_BN_CLICKED(ID_BUTTON_DEL, &CEditConfigDialog::OnBnClickedButtonDel)
	ON_BN_CLICKED(ID_BUTTON_UPDATE, &CEditConfigDialog::OnBnClickedButtonUpdate)
	ON_LBN_SELCHANGE(IDC_SCAN_UPDATE, &CEditConfigDialog::OnLbnSelchangeScaneUpdate)
END_MESSAGE_MAP()


BOOL CEditConfigDialog::updataScanUpdate(int v)
{
	int c = m_addr.GetCurSel();
	if (v == 0 || v == 2)
	{
		if (c == -1)
			return FALSE;
	}

	if (v == 0)
	{
		m_pScanUpdate->erase(m_pScanUpdate->begin() + c);
		m_addr.DeleteString(c);
		m_addrs.SetWindowText(L"");
		m_addre.SetWindowText(L"");
		return TRUE;
	}

	CString addrs;
	m_addrs.GetWindowText(addrs);
	CString addre;
	m_addre.GetWindowText(addre);
	CString strAddr = addrs + CString(L" - ") + addre;
	if (m_addrs.IsBlank())
	{
		AfxMessageBox(L"开始地址是必须要填写的！");
		return FALSE;
	}
	if (m_addre.IsBlank())
	{
		AfxMessageBox(L"结束地址是必须要填写的！");
		return FALSE;
	}
	int s = addrs.ReverseFind(L'.');
	int e = addre.ReverseFind(L'.');
	if (addrs.Left(s) != addre.Left(e))
	{
		AfxMessageBox(L"开始结束地址段不匹配！");
		return FALSE;
	}

	IScanUpdate su = {};
	su.addr_s = _bstr_t(addrs);
	su.addr_e = _bstr_t(addre);
	if (v == 1)
	{
		m_pScanUpdate->push_back(su);
		c = m_addr.AddString(strAddr);
		m_addr.SetCurSel(c);
	}
	else if (v == 2)
	{
		(*m_pScanUpdate)[c] = su;
		m_addr.DeleteString(c);
		m_addr.InsertString(c, strAddr);
		m_addr.SetCurSel(c);
	}
	AfxMessageBox(L"数据变更完成！");
	return TRUE;
}


// CEditConfigDialog 消息处理程序

void CEditConfigDialog::OnBnClickedButtonAdd()
{
	updataScanUpdate(1);
}


void CEditConfigDialog::OnBnClickedButtonEdit()
{
	updataScanUpdate(2);
}


void CEditConfigDialog::OnBnClickedButtonDel()
{
	updataScanUpdate(0);
}


void CEditConfigDialog::OnBnClickedButtonUpdate()
{
	CString str;
	m_scan_port.GetWindowText(str);
	m_pConfig->udp_scan_port = (UINT)_wtoi(str.GetBuffer());
	str.ReleaseBuffer();
	m_open_port.GetWindowText(str);
	m_pConfig->udp_open_port = (UINT)_wtoi(str.GetBuffer());
	str.ReleaseBuffer();
	m_open_port_c.GetWindowText(str);
	m_pConfig->udp_open_port_c = (UINT)_wtoi(str.GetBuffer());
	str.ReleaseBuffer();
	m_scan_time.GetWindowText(str);
	m_pConfig->scan_setp_time = (UINT)_wtoi(str.GetBuffer());
	str.ReleaseBuffer();
	m_task_time.GetWindowText(str);
	m_pConfig->task_setp_time = (UINT)_wtoi(str.GetBuffer());
	str.ReleaseBuffer();
	m_task_count.GetWindowText(str);
	m_pConfig->task_max_count = (int)_wtoi(str.GetBuffer());
	str.ReleaseBuffer();
}


void CEditConfigDialog::OnLbnSelchangeScaneUpdate()
{
	int c = m_addr.GetCurSel();
	if (c < 0)
		return;

	m_addrs.SetWindowText((*m_pScanUpdate)[c].addr_s);
	m_addre.SetWindowText((*m_pScanUpdate)[c].addr_e);
}


BOOL CEditConfigDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_scan_port.SetWindowText((CString)(LPWSTR)_bstr_t(m_pConfig->udp_scan_port));
	m_open_port.SetWindowText((CString)(LPWSTR)_bstr_t(m_pConfig->udp_open_port));
	m_open_port_c.SetWindowText((CString)(LPWSTR)_bstr_t(m_pConfig->udp_open_port_c));
	m_scan_time.SetWindowText((CString)(LPWSTR)_bstr_t(m_pConfig->scan_setp_time));
	m_task_time.SetWindowText((CString)(LPWSTR)_bstr_t(m_pConfig->task_setp_time));
	m_task_count.SetWindowText((CString)(LPWSTR)_bstr_t(m_pConfig->task_max_count));

	for (int i = 0; i < (int)m_pScanUpdate->size(); i++)
	{
		CString str(L"");
		str += (CString)(LPWSTR)_bstr_t((*m_pScanUpdate)[i].addr_s);
		str += (CString)(L" - ");
		str += (CString)(LPWSTR)_bstr_t((*m_pScanUpdate)[i].addr_e);
		m_addr.AddString(str);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}