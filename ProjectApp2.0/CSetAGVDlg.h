#pragma once


// CSetAGVDlg 对话框

class CSetAGVDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetAGVDlg)

public:
	CSetAGVDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetAGVDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETAGV_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// AGV编号
	int m_byNo;
	int m_nRadio;
	// 远程IP地址
	DWORD m_dwServerIP;
	// 远程端口
	CString m_cstrServerPort;
	// 本地IP地址
	DWORD m_dwLocalIP;
	// 本地端口	// 本地端口
	CString m_cstrLocalPort;
	int m_nServerPort;
	int m_nLocalPort;
	bool m_bEdit;
	afx_msg void OnEnChangeAgvnoEdit();
	afx_msg void OnIpnFieldchangedAgvserverIpaddress(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeAgvserverportEdit();
	afx_msg void OnIpnFieldchangedAgvclientIpaddress(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEnChangeAgvclientportEdit();
	afx_msg void OnBnClickedServerRadio();
	afx_msg void OnBnClickedClientRadio();
	virtual BOOL OnInitDialog();
};
