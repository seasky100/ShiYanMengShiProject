#pragma once
#include <map>
#include <set>
#include <list>

// CAGVStatusDlg 对话框

class CAGVStatusDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAGVStatusDlg)

public:
	CAGVStatusDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CAGVStatusDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AGVSTATUS_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	// AGV列表控件
	CListCtrl m_listctrlAGVConfig;
	CListCtrl m_listctrlAGVStatus;
	bool m_bEdit;
	int m_nUserLevel;	/*!< 用户权限等级 */
	map<unsigned char, string> m_mapStation;
	set<unsigned char> m_setAGV;
	CComboBox m_comboNo;
	CComboBox m_comboCtrl;
	CComboBox m_comboTarget;
	list<Json::Value> m_listUpdate;
	mutex m_mutex;

protected:
	enum LISTCTRLITEM_CONFIG
	{
		CITEM_NO,		// AGV编号
		CITEM_LOCALIP,	// AGV本地IP地址
		CITEM_LOCALPORT,	// AGV本地端口
		CITEM_SERVERIP,	// AGV远程IP地址
		CITEM_SERVERPORT,// AGV远程端口
	};

	enum LISTCTRLITEM_RUN
	{
		RITEM_NO,		// 编号
		RITEM_IP,		// IP地址
		RITEM_PORT,		// 端口
		RITEM_LINK,		// 连接状态
		RITEM_CURRFID,	// 当前坐标
		RITEM_ENDRFID,	// 终点坐标
		RITEM_STATUS,	// 运行状态
		RITEM_SPEED,	// 速度
		RITEM_POWER,	// 电量
		RITEM_CARGO,	// 载货状态
		RITEM_LIFTER,	// 升降杆状态
		RITEM_ACTION,	// 动作信息
		RITEM_ERROR,	// 异常信息
	};
public:
	void InitListCtrl();
	void SortConfigList();
	void SortStatusList();
	void ListSize();
	void OpenEditMode();
	void OpenRunMode();
	void AddNewConfig(Json::Value& JsonValue);
	void AddNewStatus(int nNo);
	void EditConfig(Json::Value& JsonValue);
	bool UpdateStatus(Json::Value& JsonValue);
	void UpdateStatus();
	void DeleteConfig(int nNo);
	void DeleteStatus(int nNo);
	void SetLevel(int nLevel);
	void InitComboCtrl();
	void AddStation(const unsigned char, const char*);
	void DeleteStation(const unsigned char);
	void AddAGV(const unsigned char);
	void DeleteAGV(const unsigned char);
	void InitComboStation();
	void InitComboAGV();

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedAddagvButton();
	afx_msg void OnBnClickedEditagvButton();
	afx_msg void OnBnClickedDelagvButton();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnCbnSelchangeAgvctrlCombo();
	afx_msg void OnBnClickedExeButton();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedClearButton();
};
