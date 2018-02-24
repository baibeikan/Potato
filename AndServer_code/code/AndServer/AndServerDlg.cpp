
// AndServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "AndServer.h"
#include "AndServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// CAndServerDlg �Ի���



CAndServerDlg::CAndServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CAndServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bInit = FALSE;

	HINSTANCE hInst = AfxGetInstanceHandle();
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/hd_32g.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/hd_32.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/picts_32g.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/picts_32.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/smicn_32g.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/smicn_32.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/help_32g.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	m_hicoType.Add((HICON)LoadImage(hInst, L"ico/help_32.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
}

CAndServerDlg::~CAndServerDlg()
{
	for (INT_PTR i = 0; i < m_hicoType.GetCount(); i++)
		DeleteObject(m_hicoType[i]);
}

void CAndServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_listClient);
	DDX_Control(pDX, IDC_EDIT, m_editLog);
	DDX_Control(pDX, IDC_EDIT_INFO, m_editInfo);
}

BEGIN_MESSAGE_MAP(CAndServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST, &CAndServerDlg::OnNMClickList)
	ON_NOTIFY(NM_RCLICK, IDC_LIST, &CAndServerDlg::OnNMRClickList)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST, &CAndServerDlg::OnNMDblclkList)
END_MESSAGE_MAP()

void CAndServerDlg::order(long v)
{
	POSITION pos = m_listClient.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		AfxMessageBox(L"��ѡ����Ҫ�������豸��");
		return;
	}

	while (pos)
	{
		int nItem = m_listClient.GetNextSelectedItem(pos);
		if ((v > 1 && (devices[nItem].port < 1024 || devices[nItem].type == _bstr_t("ͶӰ"))) ||
			(v <= 1 && devices[nItem].type == _bstr_t("����")))
		{
			CString str;
			str.Format(L"�豸(%s)��֧�ָò�����", (LPTSTR)devices[nItem].addr);
			AfxMessageBox(str);
			continue;
		}
		addTask(devices[nItem].addr, v);
	}
}

void CAndServerDlg::edit(long v)
{
	POSITION pos = m_listClient.GetFirstSelectedItemPosition();
	if (pos == NULL && (v == 0 || v == 2))
	{
		AfxMessageBox(L"��ѡ����Ҫ������豸��");
		return;
	}

	StopThread();
	m_editLog.SetWindowText(L"");
	m_editInfo.SetWindowText(L"");
	if (v == 0)
	{
		while (pos)
		{
			int nItem = m_listClient.GetNextSelectedItem(pos);
			clearDevice(devices[nItem].addr);
		}
	}
	else if (v == 1)
	{
		vector<IDeviceData> datas;
		datas.push_back(IDeviceData());
		CEditDeviceDialog dlg(FALSE, &datas);
		dlg.DoModal();
		if (datas[0].addr.length() > 0)
			addDevice(datas[0]);
	}
	else if (v == 2)
	{
		vector<IDeviceData> datas;
		while (pos)
		{
			int nItem = m_listClient.GetNextSelectedItem(pos);
			datas.push_back(devices[nItem]);
		}
		CEditDeviceDialog dlg(TRUE, &datas);
		dlg.DoModal();
		for (int i = 0; i < (int)datas.size(); i++)
			updateDevice(datas[i]);
	}
	RunThread();
}

void CAndServerDlg::editCodeBook()
{
	StopThread();
	loadCodeBook();
	CEditCodeBookDialog dlg(&codebooks);
	dlg.DoModal();
	clearCodeBook();
	for (int i = 0; i < (int)codebooks.size(); i++)
		addCodeBook(codebooks[i]);
	RunThread();
}

void CAndServerDlg::editConfig()
{
	StopThread();
	CEditConfigDialog dlg(&config, &scan_update);
	dlg.DoModal();
	updateConfig();
	AfxMessageBox(L"������������Ӧ�ø��£�");
	SendMessage(WM_CLOSE);
}

void CAndServerDlg::OnInitDevice()
{
	m_listClient.DeleteAllItems();
	for (int i = 0; i < (int)devices.size(); i++)
	{
		IDeviceData data = devices[i];
		int t = -1;
		if (data.type == _bstr_t("����"))
			t = 0;
		else if (data.type == _bstr_t("ͶӰ"))
			t = 1;
		else if (data.type == _bstr_t("����"))
			t = 2;
		else
			t = 3;
		m_listClient.InsertItem(i, (LPCTSTR)(LPTSTR)(data.exhibit), t * 2 + data.state);
	}
	m_listClient.UpdateWindow();

	CMenu* pSubMenu = GetMenu()->GetSubMenu(0);
	int c = pSubMenu->GetMenuItemCount();
	if (c > 2)
	{
		for (int i = c - 1; i >= 2; i--)
			pSubMenu->DeleteMenu(i, MF_BYPOSITION);
	}
	for (int i = 0; i < (int)codebooks.size(); i++)
	{
		if (i == 0)
			pSubMenu->AppendMenu(MF_SEPARATOR, 0, (LPCTSTR)NULL);
		pSubMenu->AppendMenu(MF_STRING | MF_BYPOSITION, ID_CODE_BOOK + i, codebooks[i].explain);
	}
}

void CAndServerDlg::OnUpdateDeviceState(int index)
{
	IDeviceData data = devices[index];
	int t = -1;
	if (data.type == _bstr_t("����"))
		t = 0;
	else if (data.type == _bstr_t("ͶӰ"))
		t = 1;
	else if (data.type == _bstr_t("����"))
		t = 2;
	else
		t = 3;
	LVITEM item = { 0 };
	item.mask = LVIF_IMAGE;
	item.iItem = index;
	if (m_listClient.GetItem(&item))
	{
		item.iImage = t * 2 + devices[index].state;
		m_listClient.SetItem(&item);
	}
}

void CAndServerDlg::OnUninitDevice()
{
	m_listClient.UpdateWindow();
}

void CAndServerDlg::OnInitScan()
{
	EnableWindow(FALSE);
	m_editInfo.SetWindowText(L"");
	m_editInfo.SetSel(-1, -1);
	m_editInfo.ReplaceSel(CString(L"ɨ�����߼����...\r\n"));
	m_editInfo.ScrollWindow(0, 0);
}

void CAndServerDlg::OnUpdateScanInfo(int index)
{
	INetData data = scans[index];
	CString strLog;
	strLog.Format(L"IP��%s MAC��%02x%02x%02x%02x%02x%02x\r\n", CString(data.addr),
		data.mac[0], data.mac[1], data.mac[2], data.mac[3], data.mac[4], data.mac[5]);
	m_editInfo.SetSel(-1, -1);
	m_editInfo.ReplaceSel(strLog);
	m_editInfo.ScrollWindow(0, 0);
}

void CAndServerDlg::OnUninitScan()
{
	m_editInfo.SetSel(-1, -1);
	m_editInfo.ReplaceSel(CString(L"ɨ���������豸�б�...\r\n"));
	m_editInfo.ScrollWindow(0, 0);
	EnableWindow(TRUE);
}

void CAndServerDlg::OnInitTask()
{
	m_editLog.SetWindowText(L"");
	m_editLog.SetSel(-1, -1);
	m_editLog.ReplaceSel(CString(L"���������б�\r\n"));
	m_editLog.ScrollWindow(0, 0);
}

void CAndServerDlg::OnUpdateTaskInfo(ITaskData task, BYTE run)
{
	m_editLog.SetSel(-1, -1);
	
	CString strTask;
	if (task.task == 0)
	{
		strTask = CString(L"�ر�");
	}
	else if (task.task == 1)
	{
		strTask = CString(L"��");
	}
	else
	{
		for (int i = 0; i < (int)codebooks.size(); i++)
		{
			if (codebooks[i].code_id == (long)task.task)
			{
				strTask = (CString)(LPTSTR)(codebooks[i].explain);
				break;
			}
		}
	}
	CString strLog;
	strLog.Format(L"%s %s�豸(%s) (%d)...\r\n", 
		strTask, (CString)(LPTSTR)(task.type), (CString)(LPTSTR)(task.addr), task.runcount);
	m_editLog.ReplaceSel(strLog);

	if (run != 0x01)
	{
		CString strResult;
		if (run == 0x00)
			strResult = CString(L"�ɹ�");
		else if (run == 0x10)
			strResult = CString(L"�ɹ�������ִ֤�н��");
		else if (run == 0x20)
			strResult = CString(L"������Ч");
		else if (run == 0x40)
			strResult = CString(L"��ʱ");
		else if (run == 0x80)
			strResult = CString(L"����");
		strLog.Format(L"%s %s�豸(%s) %s\r\n", strTask, (CString)(LPTSTR)(task.type), (CString)(LPTSTR)(task.addr), strResult);
		m_editLog.ReplaceSel(strLog);
	}
	m_editLog.ScrollWindow(0, 0);
}

void CAndServerDlg::OnUninitTask()
{
	m_editLog.SetSel(-1, -1);
	m_editLog.ReplaceSel(CString(L"��ǰ�������\r\n"));
	m_editLog.ScrollWindow(0, 0);
}

// CAndServerDlg ��Ϣ�������

BOOL CAndServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, TRUE);			// ����Сͼ��
	
	ShowWindow(SW_MAXIMIZE);

	INT_PTR count = m_hicoType.GetCount();
	m_images.Create(32, 32, ILC_COLOR32, 0, count);
	for (INT_PTR i = 0; i < count; i++)
		m_images.Add(m_hicoType[i]);

	CRect rect;
	GetClientRect(&rect);
	CRect rcList = rect;
	rcList.bottom = rect.Height() * 3 / 4;
	m_listClient.MoveWindow(rcList);
	m_listClient.SetImageList(&m_images, LVSIL_NORMAL);
	CRect rcEdit = rect;
	rcEdit.top = rect.Height() * 3 / 4;
	rcEdit.right = rect.Width() / 2;
	m_editLog.MoveWindow(rcEdit);
	CRect rcEditInfo = rect;
	rcEditInfo.top = rect.Height() * 3 / 4;
	rcEditInfo.left = rect.Width() / 2;
	rcEditInfo.right = rect.Width();
	m_editInfo.MoveWindow(rcEditInfo);
	RunThread();

	m_bInit = TRUE;
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

BOOL CAndServerDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD id = LOWORD(wParam);
	switch (id)
	{
	case ID_OPEN:
	{
		order(1);
		break;
	}
	case ID_CLOSE:
	{
		order(0);
		break;
	}	
	case ID_ADD_DEVICE_LIST:
	{
		edit(1);
		break;
	}
	case ID_DEL_DEVICE_LIST:
	{
		edit(0);
		break;
	}
	case ID_EDIT_DEVICE_LIST:
	{
		edit(2);
		break;
	}
	case ID_UPDATE_FROM_ONLINE:
	{
		scanOnOff = true;
		break;
	}
	case ID_EDIT_CODE_BOOK:
	{
		editCodeBook();
		break;
	}
	case ID_EDIT_CONFIG:
	{
		editConfig();
		break;
	}
	default:
	{
		if (id >= ID_CODE_BOOK && id < ID_CODE_BOOK + (WORD)codebooks.size())
		{
			order(codebooks[LOWORD(wParam) - ID_CODE_BOOK].code_id);
		}
		break;
	}
	}

	return CDialogEx::OnCommand(wParam, lParam);
}


void CAndServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CAndServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CAndServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CAndServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	if (m_bInit)
	{
		m_listClient.MoveWindow(CRect(0, 0, cx, cy * 3 / 4));
		m_editLog.MoveWindow(CRect(0, cy * 3 / 4, cx / 2, cy));
		m_editInfo.MoveWindow(CRect(cx / 2, cy * 3 / 4, cx, cy));
	}
}


void CAndServerDlg::OnDestroy()
{
	StopThread();
	CDialogEx::OnDestroy();
}


void CAndServerDlg::OnNMClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem >= 0)
	{
		m_editInfo.SetWindowText(L"");
		m_editInfo.SetSel(-1, -1);
		m_editInfo.ReplaceSel(CString(L"��ַ\r��"));
		m_editInfo.ReplaceSel((LPCTSTR)(LPTSTR)devices[pNMItemActivate->iItem].addr);
		m_editInfo.ReplaceSel(CString(L"\r\n"));
		m_editInfo.ReplaceSel(CString(L"MAC\r��"));
		m_editInfo.ReplaceSel((LPCTSTR)(LPTSTR)devices[pNMItemActivate->iItem].mac);
		m_editInfo.ReplaceSel(CString(L"\r\n"));
		m_editInfo.ReplaceSel(CString(L"�˿�\r��"));
		m_editInfo.ReplaceSel((LPCTSTR)(LPTSTR)_bstr_t(devices[pNMItemActivate->iItem].port));
		m_editInfo.ReplaceSel(CString(L"\r\n"));
		m_editInfo.ReplaceSel(CString(L"չ��\r��"));
		m_editInfo.ReplaceSel((LPCTSTR)(LPTSTR)devices[pNMItemActivate->iItem].area);
		m_editInfo.ReplaceSel(CString(L"\r\n"));
		m_editInfo.ReplaceSel(CString(L"չ��\r��"));
		m_editInfo.ReplaceSel((LPCTSTR)(LPTSTR)devices[pNMItemActivate->iItem].exhibit);
		m_editInfo.ReplaceSel(CString(L"\r\n"));
		m_editInfo.ReplaceSel(CString(L"����\r��"));
		m_editInfo.ReplaceSel((LPCTSTR)(LPTSTR)devices[pNMItemActivate->iItem].type);
		m_editInfo.ReplaceSel(CString(L"\r\n"));
		m_editInfo.ScrollWindow(0, 0);
	}

	*pResult = 0;
}


void CAndServerDlg::OnNMRClickList(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (m_listClient.GetSelectedCount() == 0)
		return;

	CPoint pt;
	GetCursorPos(&pt);
	GetMenu()->GetSubMenu(0)->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);

	*pResult = 0;
}


void CAndServerDlg::OnNMDblclkList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pNMItemActivate->iItem >= 0)
	{
		edit(2);
	}

	*pResult = 0;
}
