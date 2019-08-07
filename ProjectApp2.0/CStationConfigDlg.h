#pragma once


// CStationConfigDlg 对话框

class CStationConfigDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStationConfigDlg)

public:
	CStationConfigDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CStationConfigDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_STATIONCONFIG_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listctrlStationConfig;
	bool m_bEdit;
	int m_nUserLevel;

protected:
	enum LISTCTRLITEM
	{
		ITEM_NO,				// 编号
		ITEM_LOCATION,			// 绑定工位
		ITEM_NAME,				// 名称
		ITEM_PROCESS,			// 工序
		ITEM_MESWP,				// MES工站编号
		ITEM_MES,				// 是否启用MES检测
	};
public:
	void InitListCtrl();
	void SortList();
	void ListSize();
	void AddNewConfig(const char* strJson);
	void EditConfig(const char* strJson);
	void DeleteConfig(int nNo);
	void SetEnableEdit(bool bEdit);
	void SetLevel(int nLevel);
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedEditButton();
	afx_msg void OnBnClickedDelButton();
	afx_msg void OnBnClickedEnableButton();
	afx_msg void OnBnClickedDisableButton();
	afx_msg void OnBnClickedAllenableButton();
	afx_msg void OnBnClickedAlldisableButton();
};
