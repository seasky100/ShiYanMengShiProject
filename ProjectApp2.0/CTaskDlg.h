#pragma once
#include "CCreateTaskdlg.h"
#include <list>

// CTaskDlg 对话框

class CTaskDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTaskDlg)

public:
	CTaskDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CTaskDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TASK_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_listctrlTask;
	bool m_bEdit;
	CCreateTaskDlg m_dlgCreateTask;
	int m_nUserLevel;
	list<Json::Value> m_listUpdate;
	mutex m_mutex;

protected:
	enum LISTCTRLITEM
	{
		ITEM_NO,				// 编号
		ITEM_TYPE,				// 类型
		ITEM_START,				// 起始工位
		ITEM_END,				// 终止工位
		ITEM_EXECUTER,			// 执行AGV编号
		ITEM_STATUS,			// 状态
		ITEM_PUBLISH,			// 发布时间
		ITEM_UPDATE,			// 更新时间
		ITEM_FINISH,			// 完成时间
		ITEM_TEXT,				// 备注
	};
public:
	void InitListCtrl();
	void ListSize();
	bool UpdateTask(Json::Value& JsonValue);
	void UpdateStatus();
	void SetEnableControl(bool bEdit);
	void SetLevel(int nLevel);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCreateButton();
	afx_msg void OnBnClickedCancelButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
