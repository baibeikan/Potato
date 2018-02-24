// EditCodeBookDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "AndServer.h"
#include "EditCodeBookDialog.h"
#include "afxdialogex.h"


// CEditCodeBookDialog 对话框

IMPLEMENT_DYNAMIC(CEditCodeBookDialog, CDialogEx)

CEditCodeBookDialog::CEditCodeBookDialog(vector<ICodeBookData>* pCodeBooks, CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditCodeBookDialog::IDD, pParent), m_pCodeBooks(pCodeBooks)
{

}

CEditCodeBookDialog::~CEditCodeBookDialog()
{
}

void CEditCodeBookDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CODE_BOOK, m_code_book);
	DDX_Control(pDX, IDC_EDIT_CODE_SEND, m_code_send);
	DDX_Control(pDX, IDC_EDIT_CODE_RECV, m_code_recv);
	DDX_Control(pDX, IDC_EDIT_EXPLAIN, m_explain);
}


BEGIN_MESSAGE_MAP(CEditCodeBookDialog, CDialogEx)
	ON_BN_CLICKED(ID_BUTTON_ADD, &CEditCodeBookDialog::OnBnClickedButtonAdd)
	ON_BN_CLICKED(ID_BUTTON_EDIT, &CEditCodeBookDialog::OnBnClickedButtonEdit)
	ON_BN_CLICKED(ID_BUTTON_DEL, &CEditCodeBookDialog::OnBnClickedButtonDel)
	ON_LBN_SELCHANGE(IDC_CODE_BOOK, &CEditCodeBookDialog::OnLbnSelchangeCodeBook)
END_MESSAGE_MAP()


BOOL CEditCodeBookDialog::updataData(int v)
{
	int c = m_code_book.GetCurSel();
	if (v == 0 || v == 2)
	{
		if (c == -1)
			return FALSE;
	}

	if (v == 0)
	{
		m_pCodeBooks->erase(m_pCodeBooks->begin() + c);
		m_code_book.DeleteString(c);
		m_code_send.SetWindowText(L"");
		m_code_recv.SetWindowText(L"");
		m_explain.SetWindowText(L"");
		AfxMessageBox(L"数据变更完成！");
		return TRUE;
	}

	CString code_send;
	m_code_send.GetWindowText(code_send);
	CString code_recv;
	m_code_recv.GetWindowText(code_recv);
	CString explain;
	m_explain.GetWindowText(explain);
	if (code_send.GetLength() == 0)
	{
		AfxMessageBox(L"发送编码是必须要填写的！");
		return FALSE;
	}
	if (!IsByteString(code_send) || !IsByteString(code_recv))
	{
		AfxMessageBox(L"输入的编码不正确（有其他字符）！");
		return FALSE;
	}		
	if (code_send.GetLength() % 2 != 0 || code_recv.GetLength() % 2 != 0)
	{
		AfxMessageBox(L"输入的编码不正确（字符长度为奇数）！");
		return FALSE;
	}
	ICodeBookData data = {};
	data.code_send = _bstr_t(code_send);
	data.code_recv = _bstr_t(code_recv);
	data.explain = _bstr_t(explain);
	if (v == 1)
	{
		m_pCodeBooks->push_back(data);
		c = m_code_book.AddString(explain);
		m_code_book.SetCurSel(c);
	}
	else if (v == 2)
	{
		(*m_pCodeBooks)[c] = data;
		m_code_book.DeleteString(c);
		m_code_book.InsertString(c, explain);
		m_code_book.SetCurSel(c);
	}
	AfxMessageBox(L"数据变更完成！");
	return TRUE;
}


// CEditCodeBookDialog 消息处理程序


void CEditCodeBookDialog::OnBnClickedButtonAdd()
{
	updataData(1);
}


void CEditCodeBookDialog::OnBnClickedButtonEdit()
{
	updataData(2);
}


void CEditCodeBookDialog::OnBnClickedButtonDel()
{
	updataData(0);
}


void CEditCodeBookDialog::OnLbnSelchangeCodeBook()
{
	int c = m_code_book.GetCurSel();
	if (c < 0)
		return;

	ICodeBookData data = (*m_pCodeBooks)[c];
	m_code_send.SetWindowText(data.code_send);
	m_code_recv.SetWindowText(data.code_recv);
	m_explain.SetWindowText(data.explain);
}


BOOL CEditCodeBookDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	for (int i = 0; i < (int)m_pCodeBooks->size(); i++)
		m_code_book.AddString((*m_pCodeBooks)[i].explain);

	return TRUE;  // return TRUE unless you set the focus to a control
}
