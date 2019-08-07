// CAddUserDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CAddUserDlg.h"
#include "afxdialogex.h"
#include <fstream>

// CAddUserDlg 对话框

IMPLEMENT_DYNAMIC(CAddUserDlg, CDialogEx)

CAddUserDlg::CAddUserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ADDUSER_DIALOG, pParent)
	, m_cstrUserName(_T(""))
	, m_cstrPassword(_T(""))
	, m_cstrComfirm(_T(""))
{
	m_nLevel = -1;
	m_strADO = "";

	m_pJsonReader = m_JsonBuilder.newCharReader();
}

CAddUserDlg::~CAddUserDlg()
{
	delete m_pJsonReader;
}

void CAddUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LEVEL_COMBO, m_comboLevel);
	DDX_Text(pDX, IDC_USER_EDIT, m_cstrUserName);
	DDV_MaxChars(pDX, m_cstrUserName, 20);
	DDX_Text(pDX, IDC_PASSWORD_EDIT, m_cstrPassword);
	DDV_MaxChars(pDX, m_cstrPassword, 20);
	DDX_Text(pDX, IDC_COMFIRM_EDIT, m_cstrComfirm);
	DDV_MaxChars(pDX, m_cstrComfirm, 20);
}


BEGIN_MESSAGE_MAP(CAddUserDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_LEVEL_COMBO, &CAddUserDlg::OnCbnSelchangeLevelCombo)
	ON_BN_CLICKED(IDOK, &CAddUserDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_USER_EDIT, &CAddUserDlg::OnEnChangeUserEdit)
	ON_EN_CHANGE(IDC_PASSWORD_EDIT, &CAddUserDlg::OnEnChangePasswordEdit)
	ON_EN_CHANGE(IDC_COMFIRM_EDIT, &CAddUserDlg::OnEnChangeComfirmEdit)
END_MESSAGE_MAP()


// CAddUserDlg 消息处理程序
void CAddUserDlg::JsonStringToADO(const char* strJson)
{
	m_strADO = strJson;

	return;
}

void CAddUserDlg::JsonStringToADO(const wchar_t* wstrJson)
{
	m_strADO = StringEx::wstring_to_string(wstrJson);

	return;
}

void CAddUserDlg::JsonFileToADO(const char* strFile)
{
	ifstream fin;
	fin.open(strFile);
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strADO = ostring.str();

	return;
}

void CAddUserDlg::JsonFileToADO(const wchar_t* wstrFile)
{
	ifstream fin;
	fin.open(StringEx::wstring_to_string(wstrFile).c_str());
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strADO = ostring.str();

	return;
}


void CAddUserDlg::InitADO()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strADO.c_str();

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化ADO失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	m_ado.m_bstrConnect = Json_ReadString(JsonRoot["Connect"]).c_str();
	m_ado.m_bstrUser = Json_ReadString(JsonRoot["User"]).c_str();
	m_ado.m_bstrPassword = Json_ReadString(JsonRoot["Password"]).c_str();
}

void CAddUserDlg::InitComboBox()
{
	m_comboLevel.InsertString(0, _T("普通用户"));
	m_comboLevel.InsertString(1, _T("操作员"));
	m_comboLevel.InsertString(2, _T("管理员"));

	m_comboLevel.SetCurSel(m_nLevel);
}

void CAddUserDlg::OnCbnSelchangeLevelCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nLevel = m_comboLevel.GetCurSel();
}


void CAddUserDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nLevel == -1)
	{
		MessageBox(_T("未选择用户级别"), _T("添加用户"), MB_ICONERROR);
		return;
	}

	if (m_cstrUserName == _T(""))
	{
		MessageBox(_T("用户名不能为空"), _T("添加用户"), MB_ICONERROR);
		return;
	}

	if (m_cstrPassword == _T("") || m_cstrComfirm == _T(""))
	{
		MessageBox(_T("密码不能为空"), _T("添加用户"), MB_ICONERROR);
		return;
	}

	if (m_cstrPassword != m_cstrComfirm)
	{
		MessageBox(_T("两次输入的密码不相同"), _T("添加用户"), MB_ICONERROR);
		return;
	}

	string strUser = StringEx::CString_to_string(m_cstrUserName);
	string strAesUser = m_aes.EncryptionAES(strUser);
	string strPassword = StringEx::CString_to_string(m_cstrPassword);
	string strAesPassword = m_aes.EncryptionAES(strPassword);

	string strSQL = StringEx::string_format(
		"if not exists("\
		"select [user_name] from SYSTEM_USER_TABLE where [user_name]='%s') "\
		"insert into SYSTEM_USER_TABLE([user_name],[user_password],[user_level]) values('%s','%s',%d)"
		, strAesUser.c_str(), strAesUser.c_str(),strAesPassword.c_str(), m_nLevel);

	_variant_t vr;
	if (m_ado.ExecuteSQL(strSQL.c_str(), vr) == false)
	{
		MessageBox(_T("数据库连接失败"), _T("添加用户"), MB_ICONERROR);
		return;
	}

	if ((int)vr < 1)
	{
		MessageBox(_T("已存在相同的用户名"), _T("添加用户"), MB_ICONERROR);
		return;
	}

	CDialogEx::OnOK();
}


void CAddUserDlg::OnEnChangeUserEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);

	for (size_t i = 0; i < m_cstrUserName.GetLength(); ++i)
	{
		if ((m_cstrUserName.GetAt(i) >= _T('0') && m_cstrUserName.GetAt(i) <= _T('9'))
			|| (m_cstrUserName.GetAt(i) >= _T('A') && m_cstrUserName.GetAt(i) <= _T('Z'))
			|| (m_cstrUserName.GetAt(i) >= _T('a') && m_cstrUserName.GetAt(i) <= _T('z')))
		{
			continue;
		}

		MessageBox(_T("仅可以使用数字以及字母"), _T("输入"), MB_ICONERROR);

		m_cstrUserName.Delete(i, 1);
		UpdateData(false);

		break;
	}

	return;
}


void CAddUserDlg::OnEnChangePasswordEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);

	for (size_t i = 0; i < m_cstrPassword.GetLength(); ++i)
	{
		if ((m_cstrPassword.GetAt(i) >= _T('0') && m_cstrPassword.GetAt(i) <= _T('9'))
			|| (m_cstrPassword.GetAt(i) >= _T('A') && m_cstrPassword.GetAt(i) <= _T('Z'))
			|| (m_cstrPassword.GetAt(i) >= _T('a') && m_cstrPassword.GetAt(i) <= _T('z')))
		{
			continue;
		}

		MessageBox(_T("仅可以使用数字以及字母"), _T("输入"), MB_ICONERROR);

		m_cstrPassword.Delete(i, 1);
		UpdateData(false);

		break;
	}

	return;
}


void CAddUserDlg::OnEnChangeComfirmEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);

	for (size_t i = 0; i < m_cstrComfirm.GetLength(); ++i)
	{
		if ((m_cstrComfirm.GetAt(i) >= _T('0') && m_cstrComfirm.GetAt(i) <= _T('9'))
			|| (m_cstrComfirm.GetAt(i) >= _T('A') && m_cstrComfirm.GetAt(i) <= _T('Z'))
			|| (m_cstrComfirm.GetAt(i) >= _T('a') && m_cstrComfirm.GetAt(i) <= _T('z')))
		{
			continue;
		}

		MessageBox(_T("仅可以使用数字以及字母"), _T("输入"), MB_ICONERROR);

		m_cstrComfirm.Delete(i, 1);
		UpdateData(false);

		break;
	}

	return;
}


BOOL CAddUserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_nLevel = -1;
	m_cstrUserName = _T("");
	m_cstrPassword = _T("");
	m_cstrComfirm = _T("");

	InitComboBox();
	InitADO();

	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
