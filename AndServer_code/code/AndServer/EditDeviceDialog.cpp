// EditDeviceDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "AndServer.h"
#include "EditDeviceDialog.h"
#include "afxdialogex.h"


BOOL HasAddr(vector<IDeviceData> datas, CString addr)
{
	BOOL ret = FALSE;
	for (int i = 0; i < (int)datas.size(); i++)
	{
		if (addr == (CString)(LPTSTR)datas[i].addr)
		{
			ret = TRUE;
			break;
		}
	}
	return ret;
}

// CEditDeviceDialog 对话框

IMPLEMENT_DYNAMIC(CEditDeviceDialog, CDialogEx)

CEditDeviceDialog::CEditDeviceDialog(BOOL isEdit, vector<IDeviceData>* pDevices, CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditDeviceDialog::IDD, pParent), m_isEdit(isEdit), m_pDevices(pDevices)
{

}

CEditDeviceDialog::~CEditDeviceDialog()
{
}

void CEditDeviceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS, m_addr);
	DDX_Control(pDX, IDC_EDIT_MAC, m_mac);
	DDX_Control(pDX, IDC_EDIT_PORT, m_port);
	DDX_Control(pDX, IDC_EDIT_AREA, m_area);
	DDX_Control(pDX, IDC_EDIT_EXHIBIT, m_exhibit);
	DDX_Control(pDX, IDC_COMBO_TYPE, m_type);
	DDX_Control(pDX, IDC_EDIT_CODE_STATE, m_code_state);
	DDX_Control(pDX, IDC_EDIT_CODE_STATE_RETURN, m_code_state_return);
	DDX_Control(pDX, IDC_EDIT_CODE_ON, m_code_on);
	DDX_Control(pDX, IDC_EDIT_CODE_OFF, m_code_off);
	DDX_Control(pDX, ID_BUTTON_PREV, m_prev);
	DDX_Control(pDX, ID_BUTTON_NEXT, m_next);
}


BEGIN_MESSAGE_MAP(CEditDeviceDialog, CDialogEx)
	ON_BN_CLICKED(ID_BUTTON_PREV, &CEditDeviceDialog::OnBnClickedButtonPrev)
	ON_BN_CLICKED(ID_BUTTON_NEXT, &CEditDeviceDialog::OnBnClickedButtonNext)
	ON_BN_CLICKED(ID_BUTTON_OK, &CEditDeviceDialog::OnBnClickedButtonOk)
	ON_CBN_SELCHANGE(IDC_COMBO_TYPE, &CEditDeviceDialog::OnCbnSelchangeComboType)
END_MESSAGE_MAP()


void CEditDeviceDialog::updataUI()
{
	IDeviceData data = (*m_pDevices)[m_index];
	unsigned long ip = inet_addr(data.addr);
	BYTE* cip = (BYTE*)(&ip);
	m_addr.SetAddress(*cip, *(cip + 1), *(cip + 2), *(cip + 3));
	m_mac.SetWindowText(data.mac);
	CString strPort;
	strPort.Format(L"%d", data.port);
	m_port.SetWindowText(strPort);
	m_area.SetWindowText(data.area);
	m_exhibit.SetWindowText(data.exhibit);
	int type = 0;
	if (data.type == _bstr_t("电脑"))
		type = 0;
	else if (data.type == _bstr_t("投影"))
		type = 1;
	else if (data.type == _bstr_t("其它"))
		type = 2;
	m_type.SetCurSel(type);
	m_code_state.SetWindowText(data.code_state);
	m_code_state_return.SetWindowText(data.code_state_return);
	m_code_on.SetWindowText(data.code_on);
	m_code_off.SetWindowText(data.code_off);
}

BOOL CEditDeviceDialog::updataData()
{
	CString addr;
	m_addr.GetWindowText(addr);
	CString mac;
	m_mac.GetWindowText(mac);
	CString port;
	m_port.GetWindowText(port);
	CString area;
	m_area.GetWindowText(area);
	CString exhibit;
	m_exhibit.GetWindowText(exhibit);
	CString type;
	m_type.GetWindowText(type);
	CString code_state;
	m_code_state.GetWindowText(code_state);
	CString code_state_return;
	m_code_state_return.GetWindowText(code_state_return);
	CString code_on;
	m_code_on.GetWindowText(code_on);
	CString code_off;
	m_code_off.GetWindowText(code_off);
	if (m_addr.IsBlank())
	{
		AfxMessageBox(L"地址是必须要填写的！");
		return FALSE;
	}
	if (HasAddr(*m_pDevices, addr) && !m_isEdit)
	{
		AfxMessageBox(L"地址是已存在！");
		return FALSE;
	}
	if (type == CString(L"电脑"))
	{
		if (mac.GetLength() == 0)
		{
			AfxMessageBox(L"MAC是必须要填写的！");
			return FALSE;
		}
		if (mac.GetLength() != 12 || !IsByteString(mac))
		{
			AfxMessageBox(L"输入的MAC不正确！");
			return FALSE;
		}
	}
	if (type == CString(L"投影"))
	{
		if (port.GetLength() == 0)
		{
			AfxMessageBox(L"端口是必须要填写的！");
			return FALSE;
		}
		if (_ttol((LPCTSTR)port) < 1024)
		{
			AfxMessageBox(L"端口是必须大于等于1024！");
			return FALSE;
		}
		if (code_state.GetLength() == 0 || code_state_return.GetLength() == 0 || 
			code_on.GetLength() == 0 || code_off.GetLength() == 0)
		{
			AfxMessageBox(L"编码是必须要填写的！");
			return FALSE;
		}
		if (!IsByteString(code_state) || !IsByteString(code_state_return) ||
			!IsByteString(code_on) || !IsByteString(code_off))
		{
			AfxMessageBox(L"输入的编码不正确（有其他字符）！");
			return FALSE;
		}
		if (code_state.GetLength() % 2 != 0 || code_state_return.GetLength() % 2 != 0 ||
			code_on.GetLength() % 2 != 0 || code_off.GetLength() % 2 != 0)
		{
			AfxMessageBox(L"输入的编码不正确（字符长度为奇数）！");
			return FALSE;
		}
	}
	if (type == CString(L"其它"))
	{
		if (port.GetLength() == 0)
		{
			AfxMessageBox(L"端口是必须要填写的！");
			return FALSE;
		}
		if (_ttol((LPCTSTR)port) < 1024)
		{
			AfxMessageBox(L"端口是必须大于等于1024！");
			return FALSE;
		}
	}
	IDeviceData data;
	data.addr = _bstr_t(addr);
	data.mac = _bstr_t(mac);
	data.port = atoi(_bstr_t(port));
	data.area = _bstr_t(area);
	data.exhibit = _bstr_t(exhibit);
	data.type = _bstr_t(type);
	data.code_state = _bstr_t(code_state);
	data.code_state_return = _bstr_t(code_state_return);
	data.code_on = _bstr_t(code_on);
	data.code_off = _bstr_t(code_off);
	if (m_isEdit)
		(*m_pDevices)[m_index] = data;
	else
		(*m_pDevices)[0] = data;
	AfxMessageBox(L"数据变更成功！");
	return TRUE;
}

// CEditDeviceDialog 消息处理程序


void CEditDeviceDialog::OnBnClickedButtonPrev()
{
	if (m_index <= 0)
		return;
	m_index--;
	updataUI();
	OnCbnSelchangeComboType();
}


void CEditDeviceDialog::OnBnClickedButtonNext()
{
	if (m_index >= (int)m_pDevices->size() - 1)
		return;
	m_index++;
	updataUI();
	OnCbnSelchangeComboType();
}


void CEditDeviceDialog::OnBnClickedButtonOk()
{
	if (!updataData())
		return;
	if (!m_isEdit)
		OnOK();
}


void CEditDeviceDialog::OnCbnSelchangeComboType()
{
	if (m_isEdit)
	{
		m_prev.EnableWindow((m_index == 0) ? FALSE : TRUE);
		m_next.EnableWindow((m_index == (int)m_pDevices->size() - 1) ? FALSE : TRUE);
	}
	else
	{
		m_prev.EnableWindow(FALSE);
		m_next.EnableWindow(FALSE);
	}
	CString type;
	m_type.GetWindowText(type);
	BOOL isProj = (type == CString(L"投影"));
	m_mac.EnableWindow(!isProj);
	m_code_state.EnableWindow(isProj);
	m_code_state_return.EnableWindow(isProj);
	m_code_on.EnableWindow(isProj);
	m_code_off.EnableWindow(isProj);
	if (!isProj)
	{
		m_code_state.SetWindowText(NULL);
		m_code_state_return.SetWindowText(NULL);
		m_code_on.SetWindowText(NULL);
		m_code_off.SetWindowText(NULL);
	}
}


BOOL CEditDeviceDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	updataUI();
	SetWindowText(!m_isEdit ? L"新增设备" : L"编辑设备");
	m_addr.EnableWindow(!m_isEdit);
	OnCbnSelchangeComboType();

	return TRUE;  // return TRUE unless you set the focus to a control
}

