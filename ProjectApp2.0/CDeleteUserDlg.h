#pragma once
#include <list>

// CDeleteUserDlg 对话框

class CDeleteUserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDeleteUserDlg)

public:
	CDeleteUserDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CDeleteUserDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DELUSER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

public:
	int m_nLevel;
	AES m_aes;
	ADOConn m_ado;
	string m_strADO;
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;
	list<string> m_listNormal;
	list<string> m_listControler;
	list<string> m_listAdminister;
	CComboBox m_comboLevel;
	CComboBox m_comboUserName;
	CString m_cstrUserName;
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
	void InitComboLevel();
	void InitComboUserName(int nLevel);

public:
	afx_msg void OnCbnSelchangeLevelCombo();
	afx_msg void OnCbnSelchangeUserCombo();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
