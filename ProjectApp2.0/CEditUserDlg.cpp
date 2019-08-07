// CEditUserDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CEditUserDlg.h"
#include "afxdialogex.h"
#include <fstream>

// CEditUserDlg 对话框

IMPLEMENT_DYNAMIC(CEditUserDlg, CDialogEx)

CEditUserDlg::CEditUserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDITUSER_DIALOG, pParent)
	, m_cstrUserName(_T(""))
	, m_cstrOldPassword(_T(""))
	, m_cstrNewPassword(_T(""))
	, m_cstrComfirm(_T(""))
{
	m_nLevel = -1;
	m_strADO = "";

	m_pJsonReader = m_JsonBuilder.newCharReader();
}

CEditUserDlg::~CEditUserDlg()
{
	delete m_pJsonReader;
}

void CEditUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LEVEL_COMBO, m_comboLevel);
	DDX_Text(pDX, IDC_USER_EDIT, m_cstrUserName);
	DDV_MaxChars(pDX, m_cstrUserName, 20);
	DDX_Text(pDX, IDC_OLDPASSWORD_EDIT, m_cstrOldPassword);
	DDV_MaxChars(pDX, m_cstrOldPassword, 20);
	DDX_Text(pDX, IDC_NEWPASSWORD_EDIT, m_cstrNewPassword);
	DDV_MaxChars(pDX, m_cstrNewPassword, 20);
	DDX_Text(pDX, IDC_COMFIRM_EDIT, m_cstrComfirm);
	DDV_MaxChars(pDX, m_cstrComfirm, 20);
}


BEGIN_MESSAGE_MAP(CEditUserDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_LEVEL_COMBO, &CEditUserDlg::OnCbnSelchangeLevelCombo)
	ON_EN_CHANGE(IDC_USER_EDIT, &CEditUserDlg::OnEnChangeUserEdit)
	ON_EN_CHANGE(IDC_OLDPASSWORD_EDIT, &CEditUserDlg::OnEnChangeOldpasswordEdit)
	ON_EN_CHANGE(IDC_NEWPASSWORD_EDIT, &CEditUserDlg::OnEnChangeNewpasswordEdit)
	ON_EN_CHANGE(IDC_COMFIRM_EDIT, &CEditUserDlg::OnEnChangeComfirmEdit)
	ON_BN_CLICKED(IDOK, &CEditUserDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CEditUserDlg 消息处理程序
void CEditUserDlg::JsonStringToADO(const char* strJson)
{
	m_strADO = strJson;

	return;
}

void CEditUserDlg::JsonStringToADO(const wchar_t* wstrJson)
{
	m_strADO = StringEx::wstring_to_string(wstrJson);

	return;
}

void CEditUserDlg::JsonFileToADO(const char* strFile)
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

void CEditUserDlg::JsonFileToADO(const wchar_t* wstrFile)
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


void CEditUserDlg::InitADO()
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

void CEditUserDlg::InitComboBox()
{
	m_comboLevel.InsertString(0, _T("普通用户"));
	m_comboLevel.InsertString(1, _T("操作员"));
	m_comboLevel.InsertString(2, _T("管理员"));

	m_comboLevel.SetCurSel(m_nLevel);
}

void CEditUserDlg::OnCbnSelchangeLevelCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nLevel = m_comboLevel.GetCurSel();
}


void CEditUserDlg::OnEnChangeUserEdit()
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


void CEditUserDlg::OnEnChangeOldpasswordEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);

	for (size_t i = 0; i < m_cstrOldPassword.GetLength(); ++i)
	{
		if ((m_cstrOldPassword.GetAt(i) >= _T('0') && m_cstrOldPassword.GetAt(i) <= _T('9'))
			|| (m_cstrOldPassword.GetAt(i) >= _T('A') && m_cstrOldPassword.GetAt(i) <= _T('Z'))
			|| (m_cstrOldPassword.GetAt(i) >= _T('a') && m_cstrOldPassword.GetAt(i) <= _T('z')))
		{
			continue;
		}

		MessageBox(_T("仅可以使用数字以及字母"), _T("输入"), MB_ICONERROR);

		m_cstrOldPassword.Delete(i, 1);
		UpdateData(false);

		break;
	}

	return;
}


void CEditUserDlg::OnEnChangeNewpasswordEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateData(true);

	for (size_t i = 0; i < m_cstrNewPassword.GetLength(); ++i)
	{
		if ((m_cstrNewPassword.GetAt(i) >= _T('0') && m_cstrNewPassword.GetAt(i) <= _T('9'))
			|| (m_cstrNewPassword.GetAt(i) >= _T('A') && m_cstrNewPassword.GetAt(i) <= _T('Z'))
			|| (m_cstrNewPassword.GetAt(i) >= _T('a') && m_cstrNewPassword.GetAt(i) <= _T('z')))
		{
			continue;
		}

		MessageBox(_T("仅可以使用数字以及字母"), _T("输入"), MB_ICONERROR);

		m_cstrNewPassword.Delete(i, 1);
		UpdateData(false);

		break;
	}

	return;
}


void CEditUserDlg::OnEnChangeComfirmEdit()
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


void CEditUserDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_nLevel == -1)
	{
		MessageBox(_T("未选择用户级别"), _T("重置密码"), MB_ICONERROR);
		return;
	}

	if (m_cstrUserName == _T(""))
	{
		MessageBox(_T("用户名不能为空"), _T("重置密码"), MB_ICONERROR);
		return;
	}

	if (m_cstrNewPassword == _T("") || m_cstrComfirm == _T("") || m_cstrNewPassword == _T(""))
	{
		MessageBox(_T("密码不能为空"), _T("重置密码"), MB_ICONERROR);
		return;
	}

	if (m_cstrNewPassword == m_cstrOldPassword)
	{
		MessageBox(_T("新密码不能与旧密码相同"), _T("重置密码"), MB_ICONERROR);
		return;
	}

	if (m_cstrNewPassword != m_cstrComfirm)
	{
		MessageBox(_T("两次输入的密码不相同"), _T("重置密码"), MB_ICONERROR);
		return;
	}

	string strUser = StringEx::CString_to_string(m_cstrUserName);
	string strAesUser = m_aes.EncryptionAES(strUser);
	string strOldPassword = StringEx::CString_to_string(m_cstrOldPassword);
	string strAesOldPassword = m_aes.EncryptionAES(strOldPassword);
	string strNewPassword = StringEx::CString_to_string(m_cstrNewPassword);
	string strAesNewPassword = m_aes.EncryptionAES(strNewPassword);

	string strSQL = StringEx::string_format(
		"if exists("\
		"select [user_name] from SYSTEM_USER_TABLE where [user_name]='%s' and [user_password]='%s' and [user_level]=%d) "\
		"update SYSTEM_USER_TABLE set [user_password]='%s' where [user_name]='%s' and [user_level]=%d"
		, strAesUser.c_str(), strAesOldPassword.c_str(), m_nLevel,strAesNewPassword.c_str(), strAesUser.c_str(),m_nLevel);

	_variant_t vr;
	if (m_ado.ExecuteSQL(strSQL.c_str(), vr) == false)
	{
		MessageBox(_T("数据库连接失败"), _T("重置密码"), MB_ICONERROR);
		return;
	}

	if ((int)vr < 1)
	{
		MessageBox(_T("用户名或密码不正确"), _T("重置密码"), MB_ICONERROR);
		return;
	}

	CDialogEx::OnOK();
}


BOOL CEditUserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_nLevel = -1;
	m_cstrUserName = _T("");
	m_cstrNewPassword = _T("");
	m_cstrComfirm = _T("");
	m_cstrOldPassword = _T("");

	InitComboBox();
	InitADO();

	UpdateData(false);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
