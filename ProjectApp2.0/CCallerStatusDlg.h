#pragma once
#include <list>

// CCallerStatusDlg 对话框

class CCallerStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CCallerStatusDlg)

public:
	CCallerStatusDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CCallerStatusDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CALLERSTATUS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listctrlCaller;
	bool m_bEdit;
	int m_nUserLevel;
	list<Json::Value> m_listUpdate;
	mutex m_mutex;

protected:
	enum LISTCTRLITEM
	{
		ITEM_NO,				// 编号
		ITEM_STATION,			// 绑定工位
		ITEM_USE,				// 是否启用
		ITEM_MAC,				// MAC地址
		ITEM_CALLSIGNAL,		// 呼叫信号
		ITEM_CANCELCALLSIGNAL,	// 取消呼叫信号
		ITEM_PASSSIGNAL,		// 放行信号
		ITEM_CANCELPASSSIGNAL,	// 取消放行信号
		ITEM_NGSIGNAL,			// NG信号
		ITEM_CANCELNGSIGNAL,	// 取消NG信号
		ITEM_SCREAM,			// 急停信号
		ITEM_PACKOUTBOUND,		// 出站校验
	};
public:
	void InitListCtrl();
	void SortList();
	void ListSize();
	void AddNewConfig(Json::Value& JsonValue);
	void EditConfig(Json::Value& JsonValue);
	bool UpdateStatus(Json::Value& JsonValue);
	void UpdateStatus();
	void DeleteConfig(int nNo);
	void SetEnableEdit(bool bEdit);
	void SetLevel(int nLevel);

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedEditButton();
	afx_msg void OnBnClickedDelButton();
	afx_msg void OnBnClickedEnableButton();
	afx_msg void OnBnClickedDisableButton();
	afx_msg void OnBnClickedAllenableButton();
	afx_msg void OnBnClickedAlldisableButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
