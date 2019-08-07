#pragma once


// CSetStationDlg 对话框

class CSetStationDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetStationDlg)

public:
	CSetStationDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetStationDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETSTATION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 工位编号
	int m_byNo;
	// 工位坐标
	int m_usLocation;
	// 工位名称	// 工位名称
	CString m_cstrName;
	// 工序列表
	CComboBox m_comboProcess;
	// 编辑标识
	bool m_bEdit;
	// 工序
	int m_byProcess;

public:
	void InitComboBox();
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeStationnameEdit();
	afx_msg void OnEnChangeStationlocationEdit();
	afx_msg void OnEnChangeStationnoEdit();
	afx_msg void OnCbnSelchangeStationprocessCombo();
	CString m_cstrMESWP;
	BOOL m_bMES;
	afx_msg void OnEnChangeMeswpEdit();
	afx_msg void OnBnClickedMesCheck();
};
