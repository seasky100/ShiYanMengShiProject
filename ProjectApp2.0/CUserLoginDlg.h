#pragma once

// CUserLoginDlg 对话框

class CUserLoginDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUserLoginDlg)

public:
	CUserLoginDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CUserLoginDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_USERLOGIN_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_cstrUserName;
	CString m_cstrPassword;
	CComboBox m_comboBox;
	int m_nLevel;
	ADOConn m_ado;
	string m_strADO;
	AES m_aes;
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;

public:
	/*!
	 * @brief 从JSON字符串中提取ADO配置信息
	 * @param const char* JSON字符串
	*/
	void JsonStringToADO(const char* strJson);
	void JsonStringToADO(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取ADO配置信息
	 * @param const char* JSON文件路径
	*/
	void JsonFileToADO(const char* strFile);
	void JsonFileToADO(const wchar_t* wstrFile);

	void InitADO();
	void InitComboBox();
	void InitUser();
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeLevelCombo();
	afx_msg void OnEnChangePasswordEdit();
	afx_msg void OnEnChangeUserEdit();
	afx_msg void OnBnClickedOk();
};
