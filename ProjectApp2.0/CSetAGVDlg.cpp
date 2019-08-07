// CSetAGVDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CSetAGVDlg.h"
#include "afxdialogex.h"


// CSetAGVDlg 对话框

IMPLEMENT_DYNAMIC(CSetAGVDlg, CDialogEx)

CSetAGVDlg::CSetAGVDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETAGV_DIALOG, pParent)
	, m_byNo(0)
	, m_nRadio(0)
	, m_dwServerIP(0)
	, m_cstrServerPort(_T("-1"))
	, m_dwLocalIP(0)
	, m_cstrLocalPort(_T("-1"))
	, m_nServerPort(-1)
	, m_nLocalPort(-1)
	, m_bEdit(false)
{

}

CSetAGVDlg::~CSetAGVDlg()
{
}

void CSetAGVDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_AGVNO_EDIT, m_byNo);
	DDV_MinMaxInt(pDX, m_byNo, 1, 255);
	DDX_Radio(pDX, IDC_SERVER_RADIO, m_nRadio);
	DDX_IPAddress(pDX, IDC_AGVSERVER_IPADDRESS, m_dwServerIP);
	DDX_Text(pDX, IDC_AGVSERVERPORT_EDIT, m_cstrServerPort);
	DDV_MaxChars(pDX, m_cstrServerPort, 5);
	DDX_IPAddress(pDX, IDC_AGVCLIENT_IPADDRESS, m_dwLocalIP);
	DDX_Text(pDX, IDC_AGVCLIENTPORT_EDIT, m_cstrLocalPort);
}


BEGIN_MESSAGE_MAP(CSetAGVDlg, CDialogEx)
	ON_EN_CHANGE(IDC_AGVNO_EDIT, &CSetAGVDlg::OnEnChangeAgvnoEdit)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_AGVSERVER_IPADDRESS, &CSetAGVDlg::OnIpnFieldchangedAgvserverIpaddress)
	ON_EN_CHANGE(IDC_AGVSERVERPORT_EDIT, &CSetAGVDlg::OnEnChangeAgvserverportEdit)
	ON_NOTIFY(IPN_FIELDCHANGED, IDC_AGVCLIENT_IPADDRESS, &CSetAGVDlg::OnIpnFieldchangedAgvclientIpaddress)
	ON_EN_CHANGE(IDC_AGVCLIENTPORT_EDIT, &CSetAGVDlg::OnEnChangeAgvclientportEdit)
	ON_BN_CLICKED(IDC_SERVER_RADIO, &CSetAGVDlg::OnBnClickedServerRadio)
	ON_BN_CLICKED(IDC_CLIENT_RADIO, &CSetAGVDlg::OnBnClickedClientRadio)
END_MESSAGE_MAP()


// CSetAGVDlg 消息处理程序


void CSetAGVDlg::OnEnChangeAgvnoEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}


void CSetAGVDlg::OnIpnFieldchangedAgvserverIpaddress(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	UpdateData(true);
}


void CSetAGVDlg::OnEnChangeAgvserverportEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);

	if (m_cstrServerPort.IsEmpty())
	{
		m_nServerPort = -1;
		return;
	}

	if (m_cstrServerPort.GetAt(0) != _T('-'))
	{
		if ((m_cstrServerPort.GetAt(0) < _T('0') || m_cstrServerPort.GetAt(0) > _T('9')))
		{
			m_cstrLocalPort = _T("");
			UpdateData(false);

			MessageBox(_T("不允许填写除数字,'-'以外的字符"), _T("设置端口失败"), MB_ICONERROR);
			return;
		}
	}

	for (int i = 1; i < m_cstrServerPort.GetLength(); ++i)
	{
		if ((m_cstrServerPort.GetAt(i) < _T('0') || m_cstrServerPort.GetAt(i) > _T('9')))
		{
			m_cstrServerPort.Delete(i, 1);
			m_cstrServerPort = _T("");
			UpdateData(false);

			MessageBox(_T("不允许填写数字,'-'以外的字符"), _T("设置端口失败"), MB_ICONERROR);
			return;
		}
	}

	int nPort = _ttoi(m_cstrServerPort);

	if (nPort < 0 && nPort != -1)
	{
		m_cstrServerPort = _T("");
		UpdateData(false);

		MessageBox(_T("不允许填写小于-1的数"), _T("设置端口失败"), MB_ICONERROR);
		return;
	}

	m_nServerPort = nPort;

	return;
}


void CSetAGVDlg::OnIpnFieldchangedAgvclientIpaddress(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMIPADDRESS pIPAddr = reinterpret_cast<LPNMIPADDRESS>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;

	UpdateData(true);
}


void CSetAGVDlg::OnEnChangeAgvclientportEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码

	UpdateData(true);

	if (m_cstrLocalPort.IsEmpty())
	{
		m_nLocalPort = -1;
		return;
	}

	if (m_cstrLocalPort.GetAt(0) != _T('-'))
	{
		if ((m_cstrLocalPort.GetAt(0) < _T('0') || m_cstrLocalPort.GetAt(0) > _T('9')))
		{
			m_cstrLocalPort = _T("");
			UpdateData(false);

			MessageBox(_T("不允许填写除数字,'-'以外的字符"), _T("设置端口失败"), MB_ICONERROR);
			return;
		}
	}

	for (int i = 1; i < m_cstrLocalPort.GetLength(); ++i)
	{
		if ((m_cstrLocalPort.GetAt(i) < _T('0') || m_cstrLocalPort.GetAt(i) > _T('9')))
		{
			m_cstrLocalPort.Delete(i, 1);
			m_cstrLocalPort = _T("");
			UpdateData(false);

			MessageBox(_T("不允许填写数字,'-'以外的字符"), _T("设置端口失败"), MB_ICONERROR);
			return;
		}
	}

	int nPort = _ttoi(m_cstrLocalPort);

	if (nPort < 0 && nPort != -1)
	{
		m_cstrLocalPort = _T("");
		UpdateData(false);

		MessageBox(_T("不允许填写小于-1的数"), _T("设置端口失败"), MB_ICONERROR);
		return;
	}

	m_nLocalPort = nPort;

	return;
}


void CSetAGVDlg::OnBnClickedServerRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nRadio = 0;

	//m_dwLocalIP = 0;

	UpdateData(false);

	GetDlgItem(IDC_AGVCLIENT_IPADDRESS)->EnableWindow(false);
	GetDlgItem(IDC_AGVSERVER_IPADDRESS)->EnableWindow(true);
	GetDlgItem(IDC_AGVSERVERPORT_EDIT)->EnableWindow(true);
}


void CSetAGVDlg::OnBnClickedClientRadio()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nRadio = 1;

	//m_dwServerIP = 0;
	//m_cstrServerPort = _T("-1");

	UpdateData(false);

	GetDlgItem(IDC_AGVSERVER_IPADDRESS)->EnableWindow(false);
	GetDlgItem(IDC_AGVSERVERPORT_EDIT)->EnableWindow(false);
	GetDlgItem(IDC_AGVCLIENT_IPADDRESS)->EnableWindow(true);
}


BOOL CSetAGVDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	OnBnClickedClientRadio();

	if (m_bEdit)
	{
		if (m_dwServerIP == 0)
		{
			OnBnClickedClientRadio();
		}
		else
		{
			OnBnClickedServerRadio();
		}

		m_cstrLocalPort.Format(_T("%d"), m_nLocalPort);
		m_cstrServerPort.Format(_T("%d"), m_nServerPort);

		SetWindowText(_T("编辑AGV"));

		GetDlgItem(IDC_AGVNO_EDIT)->EnableWindow(false);

		UpdateData(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
