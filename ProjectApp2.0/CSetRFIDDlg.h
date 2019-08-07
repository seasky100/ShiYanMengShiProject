#pragma once


// CSetRFIDDlg 对话框

class CSetRFIDDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetRFIDDlg)

public:
	CSetRFIDDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CSetRFIDDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETRFID_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// RFID卡编号
	int m_usNo;
	afx_msg void OnEnChangeRfidnoEdit();
};
