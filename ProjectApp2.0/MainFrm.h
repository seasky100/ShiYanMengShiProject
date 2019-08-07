
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include "ChildView.h"
#include "../json/json.h"
#include "../ADO/ADOConn.h"
#include "../ProjectDemo/ServiceFunction.h"
#include "CAGVStatusDlg.h"
#include "CCallerStatusDlg.h"
#include "CStationConfigDlg.h"
#include "CTaskDlg.h"
#include "CUserLoginDlg.h"
#include "CAddUserDlg.h"
#include "CEditUserDlg.h"
#include "CDeleteUserDlg.h"

class CMainFrame : public CFrameWndEx
{
	
public:
	CMainFrame() noexcept;
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = nullptr, CCreateContext* pContext = nullptr);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;
	CMFCTabCtrl		m_wndTabCtrl;
	CChildView    m_wndView;
	CAGVStatusDlg m_dlgAGVStatus;
	CCallerStatusDlg m_dlgCallerStatus;
	CStationConfigDlg m_dlgStationConfig;
	CTaskDlg m_dlgTask;
	CUserLoginDlg m_dlgUserLogin;
	CAddUserDlg m_dlgAddUser;
	CEditUserDlg m_dlgEditUser;
	CDeleteUserDlg m_dlgDeleteUser;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

protected:
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;
	ADOConn m_ado;
	ServiceFunction m_func;
	HANDLE m_hShutdownEvent;
	string m_strADO;
	string m_strServerStatus;
	string m_strUser;
	int m_nUserLevel;
	thread* m_pModeThread;
	bool m_bDestroy;

public:
	bool UpdateAGVStatus(Json::Value& JsonValue);
	bool UpdateCallerStatus(Json::Value& JsonValue);
	bool UpdateServerStatus(Json::Value& JsonValue);
	bool UpdateTaskStatus(Json::Value& JsonValue);
	bool UpdateKeyStatus(Json::Value& JsonValue);
	void UpdateWidget(Json::Value& JsonValue);
	bool InitFunction();
	bool OpenEditMode();
	bool OpenRunMode();
	bool CreateWidget(Json::Value& JsonValue);
	bool EditWidget(Json::Value& JsonValue);
	bool DeleteWidget(Json::Value& JsonValue);
	bool CreateTask(const unsigned char byStart,const unsigned char byEnd,const unsigned char byAGV,const char* strType);
	bool CancelTask(int nTaskNo);
	bool SetEnableCaller(Json::Value& JsonValue, BOOL bEnable);
	bool SetEnableStation(Json::Value& JsonValue, BOOL bEnable);
	void SetLevel(int nLevel);
	void ControlAGV(Json::Value& JsonValue);
	static bool __stdcall UpdateStatus(Json::Value* JsonValue, void* pPtr);
	void ModeThread();

protected:
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

	Json::Value LoadAGV(const char* strJson);
	void LoadCaller(const char* strJson, Json::Value& json);
	Json::Value LoadRFID(const char* strJson);
	Json::Value LoadStation(const char* strJson);

	void InitDialog();

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	afx_msg void OnUserRelogin();
	afx_msg void OnUpdateUserAdd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserEdit(CCmdUI* pCmdUI);
	afx_msg void OnUpdateUserDelete(CCmdUI* pCmdUI);
	afx_msg void OnUserAdd();
	afx_msg void OnUserEdit();
	afx_msg void OnUserDelete();
	afx_msg void OnDestroy();
};


