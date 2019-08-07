// CSetCallerDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CSetCallerDlg.h"
#include "afxdialogex.h"


// CSetCallerDlg 对话框

IMPLEMENT_DYNAMIC(CSetCallerDlg, CDialogEx)

CSetCallerDlg::CSetCallerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETCALLER_DIALOG, pParent)
	, m_byNo(0)
	, m_byStation(0)
	, m_bEdit(false)
	, m_bUse(TRUE)
	, m_cstrMAC(_T(""))
{

}

CSetCallerDlg::~CSetCallerDlg()
{
}

void CSetCallerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_CALLERNO_EDIT, m_byNo);
	DDV_MinMaxInt(pDX, m_byNo, 1, 255);
	DDX_Text(pDX, IDC_CALLERSTATION_EDIT2, m_byStation);
	DDV_MinMaxInt(pDX, m_byStation, 0, 255);
	DDX_Check(pDX, IDC_USE_CHECK, m_bUse);
	DDX_Text(pDX, IDC_MACADDRESS_EDIT, m_cstrMAC);
	DDV_MaxChars(pDX, m_cstrMAC,4);
}


BEGIN_MESSAGE_MAP(CSetCallerDlg, CDialogEx)
	ON_EN_CHANGE(IDC_CALLERNO_EDIT, &CSetCallerDlg::OnEnChangeCallernoEdit)
	ON_EN_CHANGE(IDC_CALLERSTATION_EDIT2, &CSetCallerDlg::OnEnChangeCallerstationEdit2)
	ON_BN_CLICKED(IDC_USE_CHECK, &CSetCallerDlg::OnBnClickedUseCheck)
	ON_EN_CHANGE(IDC_MACADDRESS_EDIT, &CSetCallerDlg::OnEnChangeMacaddressEdit)
END_MESSAGE_MAP()


// CSetCallerDlg 消息处理程序


void CSetCallerDlg::OnEnChangeCallernoEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}


void CSetCallerDlg::OnEnChangeCallerstationEdit2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}


BOOL CSetCallerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	if (m_bEdit)
	{
		UpdateData(false);

		SetWindowText(_T("编辑呼叫器"));
		GetDlgItem(IDC_CALLERNO_EDIT)->EnableWindow(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSetCallerDlg::OnBnClickedUseCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bUse = !m_bUse;
	UpdateData(false);
}


void CSetCallerDlg::OnEnChangeMacaddressEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);

	for (size_t i = 0; i < m_cstrMAC.GetLength(); ++i)
	{
		if ((m_cstrMAC.GetAt(i) >= _T('0') && m_cstrMAC.GetAt(i) <= _T('9'))
			|| (m_cstrMAC.GetAt(i) >= _T('A') && m_cstrMAC.GetAt(i) <= _T('F'))
			|| (m_cstrMAC.GetAt(i) >= _T('a') && m_cstrMAC.GetAt(i) <= _T('f')))
		{
			continue;
		}

		MessageBox(_T("仅可以输入16进制数"), _T("输入"), MB_ICONERROR);
		m_cstrMAC.Delete(i);

		UpdateData(false);

		break;
	}

	return;
}
