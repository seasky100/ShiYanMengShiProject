// ADOConn.cpp: implementation of the ADOConn class.
//
//////////////////////////////////////////////////////////////////////
#include "ADOConn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool ADOConn::m_bInstall = false;

ADOConn::ADOConn()
{
	m_bstrConnect = "";

	m_bstrUser = "";

	m_bstrPassword = "";

	m_pConnection = nullptr;

	m_pRecordset = nullptr;

	//m_bLock = false;

	if (m_bInstall == false)
	{
		CoInitialize(nullptr);

		m_bInstall = true;
	}
}

ADOConn::ADOConn(_bstr_t bstrConnect, _bstr_t bstrUser, _bstr_t bstrPassword)
{
	m_bstrConnect = bstrConnect;

	m_bstrUser = bstrUser;

	m_bstrPassword = bstrPassword;

	m_pConnection = nullptr;

	m_pRecordset = nullptr;

	//m_bLock = false;

	if (m_bInstall == false)
	{
		CoInitialize(nullptr);

		m_bInstall = true;
	}
}

ADOConn::ADOConn(const ADOConn &cAdo)
{
	m_bstrConnect = cAdo.m_bstrConnect;

	m_bstrUser = cAdo.m_bstrUser;

	m_bstrPassword = cAdo.m_bstrPassword;

	m_pConnection = nullptr;

	m_pRecordset = nullptr;

	if (m_bInstall == false)
	{
		CoInitialize(nullptr);

		m_bInstall = true;
	}
}

ADOConn::~ADOConn()
{
	//ExitConnect();

	if (m_pRecordset != nullptr)
	{
		m_pRecordset.Release();
		m_pRecordset = nullptr;
	}

	if (m_pConnection != nullptr)
	{
		m_pConnection.Release();
		m_pConnection = nullptr;
	}

	if (m_bInstall)
	{
		CoUninitialize();

		m_bInstall = false;
	}
}

bool ADOConn::OnInitADOConn()
{
	try
	{
		if (m_pConnection)
		{
			m_pConnection.Release();
			m_pConnection = nullptr;
		}

		if (m_bstrConnect == _bstr_t(""))
		{
			OutputDebugStringA("ADO连接串为空.\n");

			return false;
		}

       m_pConnection.CreateInstance("ADODB.Connection");

	   if (m_pConnection == nullptr)
	   {
		   return false;
	   }

	   m_pConnection->Open(m_bstrConnect, m_bstrUser, m_bstrPassword,adModeUnknown); // 连接数据库
	}
	catch(_com_error e)
	{
		OutputDebugString((LPCTSTR)e.Description());
		OutputDebugString(_bstr_t("\n"));

		if (m_pConnection)
		{
			m_pConnection.Release();
			m_pConnection = nullptr;
		}
	
		return false;
	}
	return true;
}

bool ADOConn::OnInitADOConn(_bstr_t bstrConnect, _bstr_t bstrUser, _bstr_t bstrPassword)
{
	m_bstrConnect = bstrConnect;

	m_bstrUser = bstrUser;

	m_bstrPassword = bstrPassword;

	return OnInitADOConn();
}

BOOL ADOConn::ExecuteSQL(_bstr_t bstrSQL,VARIANT& rs)
{
	try
	{
		if (m_pConnection == nullptr) 
		{
			if (OnInitADOConn() == false)
			{
				return false;
			}
		}

		m_pConnection->Execute(bstrSQL, &rs,adCmdText);

		return true;
	}
	catch(_com_error e)
	{
		OutputDebugString((LPCTSTR)e.Description());
		OutputDebugString(_bstr_t("\n"));

		return false;
	}
}

void ADOConn::ExitConnect()
{
	if (m_pRecordset != nullptr) 
	{
		m_pRecordset->Close();
		m_pRecordset.Release();
		m_pRecordset = nullptr;
	}

	if (m_pConnection != nullptr)
	{
		m_pConnection->Close();
		m_pConnection.Release();
		m_pConnection = nullptr;
	}

	return;
}

bool ADOConn::GetRecordSet(_bstr_t bstrSQL, _RecordsetPtr& result)
{
	try
	{
		if (m_pConnection == nullptr) 
		{
			if (OnInitADOConn() == false) 
			{
				return false;
			}
		}

		if (m_pRecordset != nullptr)
		{
			m_pRecordset->Close();
			m_pRecordset.Release();
			m_pRecordset = nullptr;
		}

		m_pRecordset.CreateInstance(__uuidof(Recordset));

		if (m_pRecordset == nullptr)
		{
			m_pRecordset = nullptr;
		}

        m_pRecordset->Open(bstrSQL,m_pConnection.GetInterfacePtr(),adOpenDynamic,adLockOptimistic,adCmdText);

		if (m_pRecordset == nullptr)
		{
			m_pRecordset.Release();
			m_pRecordset = nullptr;
			return false;
		}

		if (m_pRecordset->GetState() == adStateClosed)
		{
			m_pRecordset->Close();
			m_pRecordset.Release();
			m_pRecordset = nullptr;
			return false;
		}

		result = m_pRecordset;

		return true;
	}
	catch (_com_error e)
	{
		OutputDebugString((LPCTSTR)e.Description());
		OutputDebugString(_bstr_t("\n"));

		if (m_pRecordset)
		{
			m_pRecordset.Release();
			m_pRecordset = nullptr;
		}
		
	}

	return false;
}

void ADOConn::operator=(const ADOConn &cAdo)
{
	m_bstrConnect = cAdo.m_bstrUser;

	m_bstrUser = cAdo.m_bstrUser;

	m_bstrPassword = cAdo.m_bstrPassword;

	return;
}