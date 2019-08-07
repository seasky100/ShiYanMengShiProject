// CCreateTaskDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CCreateTaskDlg.h"
#include "afxdialogex.h"


// CCreateTaskDlg 对话框

IMPLEMENT_DYNAMIC(CCreateTaskDlg, CDialogEx)

CCreateTaskDlg::CCreateTaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CREATETASK_DIALOG, pParent)
{
	m_strType = "";
	m_byStart = 0;
	m_byEnd = 0;
	m_byAGV = 0;
}

CCreateTaskDlg::~CCreateTaskDlg()
{
}

void CCreateTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_START_COMBO, m_comboStart);
	DDX_Control(pDX, IDC_END_COMBO, m_comboEnd);
	DDX_Control(pDX, IDC_EXECUTER_COMBO, m_comboAGV);
	DDX_Control(pDX, IDC_TYPE_COMBO, m_comboType);
}

void CCreateTaskDlg::AddStation(const unsigned char nNo, const char* strName)
{
	m_mapStation[nNo].clear();
	m_mapStation[nNo] = StringEx::string_format("%d：%s", nNo, strName);

	return;
}

void CCreateTaskDlg::DeleteStation(const unsigned char nNo)
{
	if (m_mapStation.find(nNo) == m_mapStation.end())
	{
		return;
	}

	m_mapStation.erase(m_mapStation.find(nNo));
	
	return;
}

void CCreateTaskDlg::AddAGV(const unsigned char nNo)
{
	if (m_setAGV.find(nNo) != m_setAGV.end())
	{
		return;
	}

	m_setAGV.insert(nNo);

	return;
}

void CCreateTaskDlg::DeleteAGV(const unsigned char nNo)
{
	if (m_setAGV.find(nNo) == m_setAGV.end())
	{
		return;
	}

	m_setAGV.erase(m_setAGV.find(nNo));

	return;
}

void CCreateTaskDlg::InitComboBox()
{
	m_comboType.InsertString(0, _T("空载移动"));
	m_comboType.InsertString(1, _T("满载移动"));

	m_comboType.SetCurSel(-1);

	return;
}

void CCreateTaskDlg::InitComboStation()
{
	m_comboStart.ResetContent();
	m_comboEnd.ResetContent();

	m_comboStart.AddString(_T("0：任意位置"));

	for (map<unsigned char, string>::iterator it = m_mapStation.begin(); it != m_mapStation.end(); ++it)
	{
		m_comboStart.AddString(StringEx::string_to_CString(it->second.c_str()));
		m_comboEnd.AddString(StringEx::string_to_CString(it->second.c_str()));
	}

	m_comboStart.SetCurSel(-1);
	m_comboEnd.SetCurSel(-1);

	return;
}

void CCreateTaskDlg::InitComboAGV()
{
	m_comboAGV.ResetContent();

	CString itemText = _T("");

	for (set<unsigned char>::iterator it = m_setAGV.begin(); it != m_setAGV.end(); ++it, itemText = _T(""))
	{
		itemText.Format(_T("%d"), *it);
		m_comboAGV.AddString(itemText);
	}

	m_comboAGV.SetCurSel(-1);

	return;
}


BEGIN_MESSAGE_MAP(CCreateTaskDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_START_COMBO, &CCreateTaskDlg::OnCbnSelchangeStartCombo)
	ON_CBN_SELCHANGE(IDC_END_COMBO, &CCreateTaskDlg::OnCbnSelchangeEndCombo)
	ON_CBN_SELCHANGE(IDC_EXECUTER_COMBO, &CCreateTaskDlg::OnCbnSelchangeExecuterCombo)
	ON_CBN_SELCHANGE(IDC_TYPE_COMBO, &CCreateTaskDlg::OnCbnSelchangeTypeCombo)
	ON_BN_CLICKED(IDOK, &CCreateTaskDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCreateTaskDlg 消息处理程序


void CCreateTaskDlg::OnCbnSelchangeStartCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString itemText = _T("");

	m_comboStart.GetLBText(m_comboStart.GetCurSel(), itemText);

	m_byStart = 0;

	string _str = StringEx::CString_to_string(itemText);

	for (map<unsigned char, string>::iterator it = m_mapStation.begin(); it != m_mapStation.end(); ++it)
	{
		if (strcmp(_str.c_str(), it->second.c_str()) == 0)
		{
			m_byStart = it->first;
			break;
		}
	}

	return;
}


void CCreateTaskDlg::OnCbnSelchangeEndCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString itemText = _T("");

	m_comboEnd.GetLBText(m_comboEnd.GetCurSel(), itemText);

	string _str = StringEx::CString_to_string(itemText);

	for (map<unsigned char, string>::iterator it = m_mapStation.begin(); it != m_mapStation.end(); ++it)
	{
		if (strcmp(_str.c_str(), it->second.c_str()) == 0)
		{
			m_byEnd = it->first;
			break;
		}
	}

	return;
}


void CCreateTaskDlg::OnCbnSelchangeExecuterCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString itemText = _T("");

	m_comboAGV.GetLBText(m_comboAGV.GetCurSel(), itemText);

	m_byAGV = _ttoi(itemText);
}


void CCreateTaskDlg::OnCbnSelchangeTypeCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	CString itemText = _T("");

	m_comboType.GetLBText(m_comboType.GetCurSel(), itemText);

	m_strType.clear();
	m_strType.append("手动");
	m_strType.append(StringEx::CString_to_string(itemText));

	return;
}


void CCreateTaskDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_comboStart.GetCurSel() == -1)
	{
		MessageBox(_T("请选择任务起点"),_T("创建任务"),MB_ICONSTOP);

		return;
	}

	if (m_comboEnd.GetCurSel() == -1)
	{
		MessageBox(_T("请选择任务终点"), _T("创建任务"), MB_ICONSTOP);

		return;
	}

	if (m_comboAGV.GetCurSel() == -1)
	{
		MessageBox(_T("请选择执行任务的AGV"), _T("创建任务"), MB_ICONSTOP);

		return;
	}

	if (m_comboType.GetCurSel() == -1)
	{
		MessageBox(_T("请选择任务类型"), _T("创建任务"), MB_ICONSTOP);

		return;
	}

	if (m_byStart == 0 && strcmp(m_strType.c_str(), "手动满载移动") == 0)
	{
		MessageBox(_T("无法创建任意起点的满载移动任务"), _T("创建任务"), MB_ICONSTOP);

		return;
	}

	if (m_byStart == m_byEnd)
	{
		MessageBox(_T("无法创建任意起点与终点相同的任务"), _T("创建任务"), MB_ICONSTOP);

		return;
	}

	CDialogEx::OnOK();
}


BOOL CCreateTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitComboBox();
	InitComboAGV();
	InitComboStation();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
