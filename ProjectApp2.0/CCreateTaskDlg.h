#pragma once
#include <set>

// CCreateTaskDlg 对话框

class CCreateTaskDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCreateTaskDlg)

public:
	CCreateTaskDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCreateTaskDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CREATETASK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboStart;
	CComboBox m_comboEnd;
	CComboBox m_comboAGV;
	CComboBox m_comboType;

public:
	map<unsigned char, string> m_mapStation;
	set<unsigned char> m_setAGV;
	string m_strType;
	unsigned char m_byStart;
	unsigned char m_byEnd;
	unsigned char m_byAGV;

public:
	void AddStation(const unsigned char,const char*);
	void DeleteStation(const unsigned char);
	void AddAGV(const unsigned char);
	void DeleteAGV(const unsigned char);
	void InitComboStation();
	void InitComboAGV();
	void InitComboBox();
public:
	afx_msg void OnCbnSelchangeStartCombo();
	afx_msg void OnCbnSelchangeEndCombo();
	afx_msg void OnCbnSelchangeExecuterCombo();
	afx_msg void OnCbnSelchangeTypeCombo();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
