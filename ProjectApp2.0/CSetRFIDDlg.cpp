// CSetRFIDDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CSetRFIDDlg.h"
#include "afxdialogex.h"


// CSetRFIDDlg 对话框

IMPLEMENT_DYNAMIC(CSetRFIDDlg, CDialogEx)

CSetRFIDDlg::CSetRFIDDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETRFID_DIALOG, pParent)
	, m_usNo(0)
{

}

CSetRFIDDlg::~CSetRFIDDlg()
{
}

void CSetRFIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_RFIDNO_EDIT, m_usNo);
	DDV_MinMaxInt(pDX, m_usNo, 1, 65535);
}


BEGIN_MESSAGE_MAP(CSetRFIDDlg, CDialogEx)
	ON_EN_CHANGE(IDC_RFIDNO_EDIT, &CSetRFIDDlg::OnEnChangeRfidnoEdit)
END_MESSAGE_MAP()


// CSetRFIDDlg 消息处理程序


void CSetRFIDDlg::OnEnChangeRfidnoEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}
