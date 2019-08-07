// CDeleteUserDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CDeleteUserDlg.h"
#include "afxdialogex.h"
#include <fstream>


// CDeleteUserDlg 对话框

IMPLEMENT_DYNAMIC(CDeleteUserDlg, CDialogEx)

CDeleteUserDlg::CDeleteUserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DELUSER_DIALOG, pParent)
{
	m_nLevel = -1;
	m_strADO = "";
	m_cstrUserName = _T("");

	m_pJsonReader = m_JsonBuilder.newCharReader();
}

CDeleteUserDlg::~CDeleteUserDlg()
{
	delete m_pJsonReader;
}

void CDeleteUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LEVEL_COMBO, m_comboLevel);
	DDX_Control(pDX, IDC_USER_COMBO, m_comboUserName);
}


BEGIN_MESSAGE_MAP(CDeleteUserDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_LEVEL_COMBO, &CDeleteUserDlg::OnCbnSelchangeLevelCombo)
	ON_CBN_SELCHANGE(IDC_USER_COMBO, &CDeleteUserDlg::OnCbnSelchangeUserCombo)
	ON_BN_CLICKED(IDOK, &CDeleteUserDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDeleteUserDlg 消息处理程序
void CDeleteUserDlg::JsonStringToADO(const char* strJson)
{
	m_strADO = strJson;

	return;
}

void CDeleteUserDlg::JsonStringToADO(const wchar_t* wstrJson)
{
	m_strADO = StringEx::wstring_to_string(wstrJson);

	return;
}

void CDeleteUserDlg::JsonFileToADO(const char* strFile)
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

void CDeleteUserDlg::JsonFileToADO(const wchar_t* wstrFile)
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


void CDeleteUserDlg::InitADO()
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

	string strName = "",strAesName = "";
	strName = "brain";
	strAesName = m_aes.EncryptionAES(strName);

	string strSQL = StringEx::string_format("select * from SYSTEM_USER_TABLE where [user_name]<>'%s'", strAesName.c_str());

	_RecordsetPtr pRecordset = nullptr;
	if (m_ado.GetRecordSet(strSQL.c_str(), pRecordset) == false)
	{
		return;
	}
	
	int nLevel = -1;

	_variant_t var;
	for (; !pRecordset->adoEOF; pRecordset->MoveNext(), nLevel = -1, strName .clear())
	{
		var = pRecordset->GetCollect(_bstr_t("user_level"));
		if (var.vt != VT_NULL)
		{
			nLevel = (int)var;
		}

		var = pRecordset->GetCollect(_bstr_t("user_name"));
		if (var.vt != VT_NULL)
		{
			strAesName = (char*)_bstr_t(var);
		}

		strName = m_aes.DecryptionAES(strAesName);

		switch (nLevel)
		{
		case USER_NORMAL:
			m_listNormal.push_back(strName);
			break;
		case USER_CONTROLER:
			m_listControler.push_back(strName);
			break;
		case USER_ADMINISTER:
			m_listAdminister.push_back(strName);
			break;
		}
	}

	//m_ado.Release();

	return;
}

void CDeleteUserDlg::InitComboLevel()
{
	m_comboLevel.InsertString(0, _T("普通用户"));
	m_comboLevel.InsertString(1, _T("操作员"));
	m_comboLevel.InsertString(2, _T("管理员"));

	m_comboLevel.SetCurSel(m_nLevel);

}

void CDeleteUserDlg::InitComboUserName(int nLevel)
{
	m_comboUserName.ResetContent();

	list<string>::iterator it;

	switch (nLevel)
	{
	case USER_NORMAL:
		for (it = m_listNormal.begin(); it != m_listNormal.end(); ++it)
		{
			m_comboUserName.AddString(StringEx::string_to_CString(*it));
		}
		break;
	case USER_CONTROLER:
		for (it = m_listControler.begin(); it != m_listControler.end(); ++it)
		{
			m_comboUserName.AddString(StringEx::string_to_CString(*it));
		}
		break;
	case USER_ADMINISTER:
		for (it = m_listAdminister.begin(); it != m_listAdminister.end(); ++it)
		{
			m_comboUserName.AddString(StringEx::string_to_CString(*it));
		}
		break;
	}

	m_comboUserName.SetCurSel(-1);

	return;
}

void CDeleteUserDlg::OnCbnSelchangeLevelCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_nLevel = m_comboLevel.GetCurSel();
	InitComboUserName(m_nLevel);
}


void CDeleteUserDlg::OnCbnSelchangeUserCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	m_comboUserName.GetLBText(m_comboUserName.GetCurSel(), m_cstrUserName);
}


void CDeleteUserDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	string strUser = StringEx::CString_to_string(m_cstrUserName);
	string strAesUser = m_aes.EncryptionAES(strUser);

	string strSQL = StringEx::string_format(
		"delete SYSTEM_USER_TABLE where [user_name]='%s' and [user_level]=%d"
		, strAesUser.c_str(), m_nLevel);

	_variant_t vr;
	if (m_ado.ExecuteSQL(strSQL.c_str(), vr) == false)
	{
		MessageBox(_T("数据库连接失败"), _T("注销用户"), MB_ICONERROR);
		return;
	}

	list<string>::iterator it;
	int nIndex = -1;

	switch (m_nLevel)
	{
	case USER_NORMAL:
		for (it = m_listNormal.begin(); it != m_listNormal.end(); ++it)
		{
			if (*it == strUser)
			{
				m_listNormal.erase(it);
				break;
			}
			
		}

		nIndex = m_comboUserName.FindString(0, m_cstrUserName);
		if (nIndex != -1)
		{
			m_comboUserName.DeleteString(nIndex);
			m_comboUserName.SetCurSel(-1);
		}

		break;
	case USER_CONTROLER:
		for (it = m_listControler.begin(); it != m_listControler.end(); ++it)
		{
			if (*it == strUser)
			{
				m_listControler.erase(it);
				break;
			}

		}

		nIndex = m_comboUserName.FindString(0, m_cstrUserName);
		if (nIndex != -1)
		{
			m_comboUserName.DeleteString(nIndex);
			m_comboUserName.SetCurSel(-1);
		}

		break;
	case USER_ADMINISTER:
		for (it = m_listAdminister.begin(); it != m_listAdminister.end(); ++it)
		{
			if (*it == strUser)
			{
				m_listAdminister.erase(it);
				break;
			}

		}

		nIndex = m_comboUserName.FindString(0, m_cstrUserName);
		if (nIndex != -1)
		{
			m_comboUserName.DeleteString(nIndex);
			m_comboUserName.SetCurSel(-1);
		}

		break;
	}

	//CDialogEx::OnOK();
}


BOOL CDeleteUserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	m_listNormal.clear();
	m_listControler.clear();
	m_listAdminister.clear();
	m_nLevel = -1;
	m_cstrUserName = _T("");

	InitADO();
	InitComboLevel();
	InitComboUserName(m_nLevel);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}
