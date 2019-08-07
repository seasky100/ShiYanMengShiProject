// CSetStationDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CSetStationDlg.h"
#include "afxdialogex.h"


// CSetStationDlg 对话框

IMPLEMENT_DYNAMIC(CSetStationDlg, CDialogEx)

CSetStationDlg::CSetStationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETSTATION_DIALOG, pParent)
	, m_byNo(0)
	, m_usLocation(0)
	, m_cstrName(_T(""))
	, m_bEdit(false)
	, m_byProcess(0)
	, m_cstrMESWP(_T(""))
	, m_bMES(FALSE)
{

}

CSetStationDlg::~CSetStationDlg()
{
}

void CSetStationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIONNO_EDIT, m_byNo);
	DDV_MinMaxInt(pDX, m_byNo, 1, 255);
	DDX_Text(pDX, IDC_STATIONLOCATION_EDIT, m_usLocation);
	DDV_MinMaxInt(pDX, m_usLocation, 1, 65535);
	DDX_Text(pDX, IDC_STATIONNAME_EDIT, m_cstrName);
	DDV_MaxChars(pDX, m_cstrName, 50);
	DDX_Control(pDX, IDC_STATIONPROCESS_COMBO, m_comboProcess);
	DDX_Text(pDX, IDC_MESWP_EDIT, m_cstrMESWP);
	DDX_Check(pDX, IDC_MES_CHECK, m_bMES);
}

void CSetStationDlg::InitComboBox()
{
	int nIndex = 0;
	m_comboProcess.InsertString(nIndex++, _T("NG修理"));
	m_comboProcess.InsertString(nIndex++, _T("箱体存放区"));
	m_comboProcess.InsertString(nIndex++, _T("箱体附件安装缓存"));
	m_comboProcess.InsertString(nIndex++, _T("箱体辅件安装"));
	m_comboProcess.InsertString(nIndex++, _T("水管安装"));
	m_comboProcess.InsertString(nIndex++, _T("下层模组吊装"));
	m_comboProcess.InsertString(nIndex++, _T("下层模组安装"));
	m_comboProcess.InsertString(nIndex++, _T("上层模组吊装"));
	m_comboProcess.InsertString(nIndex++, _T("上层模组安装"));
	m_comboProcess.InsertString(nIndex++, _T("线外BDU组装"));
	m_comboProcess.InsertString(nIndex++, _T("箱体BDU组装"));
	m_comboProcess.InsertString(nIndex++, _T("预留工站1(从控板刷写)"));
	m_comboProcess.InsertString(nIndex++, _T("预留工站2(未知)"));
	m_comboProcess.InsertString(nIndex++, _T("EOL测试"));
	m_comboProcess.InsertString(nIndex++, _T("电池包测试1"));
	m_comboProcess.InsertString(nIndex++, _T("箱体上盖安装缓存"));
	m_comboProcess.InsertString(nIndex++, _T("电池包测试2"));
	m_comboProcess.InsertString(nIndex++, _T("箱体上盖安装"));
	m_comboProcess.InsertString(nIndex++, _T("箱体气密测试"));
	m_comboProcess.InsertString(nIndex++, _T("成品包下料缓存"));
	m_comboProcess.InsertString(nIndex++, _T("成品包下料"));

	return;
}


BEGIN_MESSAGE_MAP(CSetStationDlg, CDialogEx)
	ON_EN_CHANGE(IDC_STATIONNAME_EDIT, &CSetStationDlg::OnEnChangeStationnameEdit)
	ON_EN_CHANGE(IDC_STATIONLOCATION_EDIT, &CSetStationDlg::OnEnChangeStationlocationEdit)
	ON_EN_CHANGE(IDC_STATIONNO_EDIT, &CSetStationDlg::OnEnChangeStationnoEdit)
	ON_CBN_SELCHANGE(IDC_STATIONPROCESS_COMBO, &CSetStationDlg::OnCbnSelchangeStationprocessCombo)
	ON_EN_CHANGE(IDC_MESWP_EDIT, &CSetStationDlg::OnEnChangeMeswpEdit)
	ON_BN_CLICKED(IDC_MES_CHECK, &CSetStationDlg::OnBnClickedMesCheck)
END_MESSAGE_MAP()


// CSetStationDlg 消息处理程序


BOOL CSetStationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitComboBox();

	m_comboProcess.SetCurSel(m_byProcess);

	if (m_bEdit)
	{
		UpdateData(false);

		SetWindowText(_T("编辑工位"));
		GetDlgItem(IDC_STATIONNO_EDIT)->EnableWindow(false);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CSetStationDlg::OnEnChangeStationnameEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}


void CSetStationDlg::OnEnChangeStationlocationEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}


void CSetStationDlg::OnEnChangeStationnoEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}


void CSetStationDlg::OnCbnSelchangeStationprocessCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_byProcess = m_comboProcess.GetCurSel();
}


void CSetStationDlg::OnEnChangeMeswpEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);
}


void CSetStationDlg::OnBnClickedMesCheck()
{
	// TODO: 在此添加控件通知处理程序代码
	m_bMES = !m_bMES;
	UpdateData(false);
}
