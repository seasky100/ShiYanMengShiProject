#pragma once


// CEditUserDlg 对话框

class CEditUserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEditUserDlg)

public:
	CEditUserDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEditUserDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDITUSER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboLevel;
	CString m_cstrUserName;
	CString m_cstrOldPassword;
	CString m_cstrNewPassword;
	CString m_cstrComfirm;
	int m_nLevel;
	AES m_aes;
	ADOConn m_ado;
	string m_strADO;
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

public:
	afx_msg void OnCbnSelchangeLevelCombo();
	afx_msg void OnEnChangeUserEdit();
	afx_msg void OnEnChangeOldpasswordEdit();
	afx_msg void OnEnChangeNewpasswordEdit();
	afx_msg void OnEnChangeComfirmEdit();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
