
// ChildView.h: CChildView 类的接口
//


#pragma once
#include "../ViewDemo/CGraphicMap.h"
#include "../json/json.h"

// CChildView 窗口

class CChildView : public CWnd
{
// 构造
public:
	CChildView();

// 特性
public:

// 操作
public:

// 重写
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CChildView();

	// 生成的消息映射函数
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);

protected:
	CGraphicMap m_map;
	bool m_bCtrl;
	bool m_bEdit;
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;
	string m_strADO;
	int m_nUserLevel;
protected:
	static void UpdateWidget(Json::Value& JsonValue, void* pMainFram);

public:
	bool DeleteWidget(Json::Value& JsonValue);
	Json::Value GetWidget(Json::Value& JsonValue);
	bool EditWidget(Json::Value& JsonValue);
	bool CreateWidget(Json::Value& JsonValue);
	void UpdateAGVStatus(Json::Value& JsonValue);
	void UpdateCallerStatus(Json::Value& JsonValue);
	void UpdateChargerStatus(Json::Value& JsonValue);
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
	bool SetEnableCaller(Json::Value& JsonValue, BOOL bEnable);
	bool SetEnableStation(Json::Value& JsonValue, BOOL bEnable);
	void SetLevel(int nLevel);
	void SetMode(bool bEdit);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnToolRfid();
	afx_msg void OnToolStation();
	afx_msg void OnToolCaller();
	afx_msg void OnToolTrackLine();
	afx_msg void OnToolTrackArc();
	afx_msg void OnUpdateToolRfid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolCharger(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolRest(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolStation(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolCaller(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolTrackLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToolTrackArc(CCmdUI* pCmdUI);
	afx_msg void OnModeEdit();
	afx_msg void OnUpdateModeEdit(CCmdUI* pCmdUI);
	afx_msg void OnModeRun();
	afx_msg void OnUpdateModeRun(CCmdUI* pCmdUI);
	virtual void PreSubclassWindow();
	afx_msg void OnMapSave();
	afx_msg void OnUpdateMapSave(CCmdUI* pCmdUI);
};

