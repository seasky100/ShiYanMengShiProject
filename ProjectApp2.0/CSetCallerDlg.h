#pragma once


// CSetCallerDlg 对话框

class CSetCallerDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetCallerDlg)

public:
	CSetCallerDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetCallerDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETCALLER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// 呼叫器编号
	int m_byNo;
	// 呼叫器绑定工位
	int m_byStation;
	// 编辑标识
	bool m_bEdit;
	afx_msg void OnEnChangeCallernoEdit();
	afx_msg void OnEnChangeCallerstationEdit2();
	virtual BOOL OnInitDialog();
	BOOL m_bUse;
	afx_msg void OnBnClickedUseCheck();
	CString m_cstrMAC;
	afx_msg void OnEnChangeMacaddressEdit();
};
