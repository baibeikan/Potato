#pragma once
#include "afxwin.h"


// CEditCodeBookDialog �Ի���

class CEditCodeBookDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEditCodeBookDialog)

public:
	CEditCodeBookDialog(vector<ICodeBookData>* pCodeBooks, CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CEditCodeBookDialog();

// �Ի�������
	enum { IDD = IDD_DIALOG_CODE_BOOK };

public:
	CListBox m_code_book;
	CEdit m_code_send;
	CEdit m_code_recv;
	CEdit m_explain;

protected:
	vector<ICodeBookData>* m_pCodeBooks = NULL;

private:
	BOOL updataData(int v);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonEdit();
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnLbnSelchangeCodeBook();
	virtual BOOL OnInitDialog();
};
