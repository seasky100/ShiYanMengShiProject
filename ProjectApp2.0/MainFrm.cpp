
// MainFrm.cpp: CMainFrame 类的实现
//

#include "pch.h"
#include "framework.h"
#include "ProjectApp.h"

#include "MainFrm.h"
#include "../Thread/ThreadEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_PAINT()
//	ON_WM_LBUTTONDBLCLK()
ON_WM_CLOSE()
ON_COMMAND(ID_USER_RELOGIN, &CMainFrame::OnUserRelogin)
ON_UPDATE_COMMAND_UI(ID_USER_ADD, &CMainFrame::OnUpdateUserAdd)
ON_UPDATE_COMMAND_UI(ID_USER_EDIT, &CMainFrame::OnUpdateUserEdit)
ON_UPDATE_COMMAND_UI(ID_USER_DELETE, &CMainFrame::OnUpdateUserDelete)
ON_COMMAND(ID_USER_ADD, &CMainFrame::OnUserAdd)
ON_COMMAND(ID_USER_EDIT, &CMainFrame::OnUserEdit)
ON_COMMAND(ID_USER_DELETE, &CMainFrame::OnUserDelete)
ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 状态行指示器
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_USER,		/*!< 用户状态 */
	ID_INDICATOR_TIME,		/*!< 时间状态 */
	ID_INDICATOR_SERVER,	/*!< 服务端状态 */
};

// CMainFrame 构造/析构

CMainFrame::CMainFrame() noexcept
{
	// TODO: 在此添加成员初始化代码
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_OFF_2007_BLUE);

	m_pJsonReader = m_JsonBuilder.newCharReader();
	m_hShutdownEvent = nullptr;
	m_strADO = "";
	m_strServerStatus = "未启动服务端";
	m_strUser = "无用户登录";
	m_nUserLevel = 0;
	m_pModeThread = nullptr;
	m_bDestroy = false;
}

CMainFrame::~CMainFrame()
{
	delete m_pJsonReader;
	CloseHandle(m_hShutdownEvent);
	m_hShutdownEvent = nullptr;
	//m_ado.ExitConnect();
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("未能创建菜单栏\n");
		return -1;      // 未能创建
	}

	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);

	// 防止菜单栏在激活时获得焦点
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("未能创建工具栏\n");
		return -1;      // 未能创建
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// 允许用户定义的工具栏操作: 
	InitUserToolbars(nullptr, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("未能创建状态栏\n");
		return -1;      // 未能创建
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));

	// TODO: 如果您不希望工具栏和菜单栏可停靠，请删除这五行
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	JsonFileToADO("../File/ADO.json");

	InitADO();

	if (m_ado.OnInitADOConn() == false)
	{
		MessageBox(_T("启动失败!\n数据库连接失败"), _T("启动失败"), MB_ICONERROR);

		PostMessage(WM_CLOSE);
	}
	else
	{
		InitDialog();

		if (m_dlgUserLogin.DoModal() != IDOK)
		{
			PostMessage(WM_CLOSE);
		}
		else
		{
			if (m_hShutdownEvent)
			{
				ResetEvent(m_hShutdownEvent);
			}
			else
			{
				m_hShutdownEvent = CreateEvent(nullptr, true, false, nullptr);
			}

			m_pModeThread = new std::thread(&CMainFrame::ModeThread, this);

			if (m_pModeThread == nullptr)
			{
				PostMessage(WM_CLOSE);
			}
		}
	}

	m_wndView.JsonStringToADO(m_strADO.c_str());

	m_wndTabCtrl.Create(CMFCTabCtrl::STYLE_3D_ROUNDED_SCROLL, CRect(0, 0, 0, 0), this, 0, CMFCTabCtrl::LOCATION_TOP);

	if (!m_dlgTask.Create(IDD_TASK_DIALOG, &m_wndTabCtrl))
	{
		return -1;
	}
	if (!m_dlgAGVStatus.Create(IDD_AGVSTATUS_DIALOG, &m_wndTabCtrl))
	{
		return -1;
	}
	if (!m_dlgCallerStatus.Create(IDD_CALLERSTATUS_DIALOG, &m_wndTabCtrl))
	{
		return -1;
	}
	if (!m_dlgStationConfig.Create(IDD_STATIONCONFIG_DIALOG, &m_wndTabCtrl))
	{
		return -1;
	}

	// 创建一个视图以占用框架的工作区
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), &m_wndTabCtrl, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("未能创建视图窗口\n");
		return -1;
	}

	//InitDialog();

	m_wndTabCtrl.AddTab(&m_wndView, _T("主视图"), -1, FALSE);
	m_wndTabCtrl.AddTab(&m_dlgTask, _T("任务信息"), -1, FALSE);
	m_wndTabCtrl.AddTab(&m_dlgAGVStatus, _T("AGV信息"), -1, FALSE);
	m_wndTabCtrl.AddTab(&m_dlgCallerStatus, _T("呼叫器信息"), -1, FALSE);
	m_wndTabCtrl.AddTab(&m_dlgStationConfig, _T("工位信息"), -1, FALSE);

	// 启用 Visual Studio 2005 样式停靠窗口行为
	CDockingManager::SetDockingMode(DT_SMART);
	// 启用 Visual Studio 2005 样式停靠窗口自动隐藏行为
	EnableAutoHidePanes(CBRS_ALIGN_ANY);
	// 基于持久值设置视觉管理器和样式
	OnApplicationLook(theApp.m_nAppLook);

	// 启用工具栏和停靠窗口菜单替换
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// 启用快速(按住 Alt 拖动)工具栏自定义
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == nullptr)
	{
		// 加载用户定义的工具栏图像
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// 启用菜单个性化(最近使用的命令)
	// TODO: 定义您自己的基本命令，确保每个下拉菜单至少有一个基本命令。
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);

	// 启动定时器
	SetTimer(1, 1000, NULL);

	m_wndTabCtrl.SetActiveTab(0);

	SetLevel(m_dlgUserLogin.m_nLevel);

	switch (m_dlgUserLogin.m_nLevel)
	{
	case USER_NORMAL:
		m_strUser = StringEx::string_format("普通用户：%s 已登陆"
			, StringEx::CString_to_string(m_dlgUserLogin.m_cstrUserName).c_str());
		break;
	case USER_CONTROLER:
		m_strUser = StringEx::string_format("操作员：%s 已登陆"
			, StringEx::CString_to_string(m_dlgUserLogin.m_cstrUserName).c_str());
		break;
	case USER_ADMINISTER:
		m_strUser = StringEx::string_format("管理员：%s 已登陆"
			, StringEx::CString_to_string(m_dlgUserLogin.m_cstrUserName).c_str());
		break;
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		 | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 将焦点前移到视图窗口
	// m_dlgTask.SetFocus();

	/*
	if (m_wndView.GetSafeHwnd() == nullptr)
	{
		return;
	}

	m_wndView.SetFocus();
	*/
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 让视图第一次尝试该命令
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 否则，执行默认处理
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* 扫描菜单*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp,LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp,lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(nullptr, nullptr, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// 基类将执行真正的工作

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// 为所有用户工具栏启用自定义按钮
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i ++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != nullptr)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}

	return TRUE;
}

bool CMainFrame::UpdateAGVStatus(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;

	m_wndView.UpdateAGVStatus(JsonRoot);
	if (m_dlgAGVStatus.UpdateStatus(JsonRoot) == false)
	{
		return false;
	}

	return true;
}

bool CMainFrame::UpdateCallerStatus(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;

	m_wndView.UpdateCallerStatus(JsonRoot);
	if (m_dlgCallerStatus.UpdateStatus(JsonRoot) == false)
	{
		return false;
	}

	return true;
}

bool CMainFrame::UpdateServerStatus(Json::Value& JsonValue)
{
	//DebugPrint::Printf(JsonValue.toStyledString().c_str());

	Json::Value& JsonRoot = JsonValue;

	string strStatus = "";

	strStatus.append(Json_ReadString(JsonRoot["IP"]));
	strStatus.append(":");
	strStatus.append(Json_ReadString(JsonRoot["Port"]));
	strStatus.append(" ");

	if (Json_ReadBool(JsonRoot["Listen"]))
	{
		strStatus.append("已监听");
	}
	else
	{
		strStatus.append("未监听");
	}

	m_strServerStatus = strStatus;

	strStatus.clear();
	JsonRoot.clear();
	return true;
}

bool CMainFrame::UpdateTaskStatus(Json::Value& JsonValue)
{
	return m_dlgTask.UpdateTask(JsonValue);
}

bool CMainFrame::UpdateKeyStatus(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;

	if (Json_ReadBool(JsonRoot["Signal"].asBool()) == true)
	{
		return true;
	}

	m_func.Close();
	m_func.Release();

	MessageBox(_T("对不起,您已失去本产品的使用权限."), _T("权限到期"), MB_ICONERROR);

	PostMessage(WM_CLOSE);

	return true;
}

void CMainFrame::UpdateWidget(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam;

	JsonParam = JsonRoot["Param"];
	string strControl = Json_ReadString(JsonRoot["Control"]);
	string strType = Json_ReadString(JsonParam["Type"]);

	if (strcmp(strControl.c_str(), "Delete") == 0)
	{	
		int nNo = Json_ReadInt(JsonParam["Param"]["No"]);

		if (strcmp(strType.c_str(), "Station") == 0)
		{
			m_dlgStationConfig.DeleteConfig(nNo);

			m_dlgTask.m_dlgCreateTask.DeleteStation(nNo);
			m_dlgAGVStatus.DeleteStation(nNo);
		}
		else if (strcmp(strType.c_str(), "Rest") == 0)
		{
		}
		else if (strcmp(strType.c_str(), "Caller") == 0)
		{
			m_dlgCallerStatus.DeleteConfig(nNo);
		}
		else if (strcmp(strType.c_str(), "Charger") == 0)
		{
		}
		else if (strcmp(strType.c_str(), "AGV") == 0)
		{
			m_dlgAGVStatus.DeleteConfig(nNo);

			m_dlgTask.m_dlgCreateTask.DeleteAGV(nNo);
		}		
	}
	else if (strcmp(strControl.c_str(), "Edit") == 0)
	{
		if (strcmp(strType.c_str(), "Station") == 0)
		{
			m_dlgStationConfig.EditConfig(JsonParam.toStyledString().c_str());
			
			// 为创建任务对话框更改工位信息
			m_dlgTask.m_dlgCreateTask.AddStation(Json_ReadInt(JsonParam["Param"]["No"]),
				StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Param"]["Name"]).c_str()).c_str());

			// 为AGV状态对话框更改工位信息
			m_dlgAGVStatus.AddStation(Json_ReadInt(JsonParam["Param"]["No"]),
				StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Param"]["Name"]).c_str()).c_str());
		}
		else if (strcmp(strType.c_str(), "Rest") == 0)
		{
			
		}
		else if (strcmp(strType.c_str(), "Caller") == 0)
		{
			m_dlgCallerStatus.EditConfig(JsonParam);
		}
		else if (strcmp(strType.c_str(), "Charger") == 0)
		{
		
		}
		else if (strcmp(strType.c_str(), "AGV") == 0)
		{
			m_dlgAGVStatus.EditConfig(JsonParam);
		}
	}
	else if (strcmp(strControl.c_str(), "Add") == 0)
	{
		if (strcmp(strType.c_str(), "Station") == 0)
		{
			m_dlgStationConfig.AddNewConfig(JsonParam.toStyledString().c_str());

			// 为创建任务对话框添加工位信息
			m_dlgTask.m_dlgCreateTask.AddStation(Json_ReadInt(JsonParam["Param"]["No"]),
				StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Param"]["Name"]).c_str()).c_str());

			// 为AGV状态对话框添加工位信息
			m_dlgAGVStatus.AddStation(Json_ReadInt(JsonParam["Param"]["No"]),
				StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Param"]["Name"]).c_str()).c_str());
		}
		else if (strcmp(strType.c_str(), "Rest") == 0)
		{
			int nNo = Json_ReadInt(JsonParam["No"]);
		}
		else if (strcmp(strType.c_str(), "Caller") == 0)
		{
			m_dlgCallerStatus.AddNewConfig(JsonParam);
		}
		else if (strcmp(strType.c_str(), "Charger") == 0)
		{
			int nNo = Json_ReadInt(JsonParam["No"]);
		}
		else if (strcmp(strType.c_str(), "AGV") == 0)
		{
			m_dlgAGVStatus.AddNewConfig(JsonParam);

			m_dlgTask.m_dlgCreateTask.AddAGV(Json_ReadInt(JsonParam["Param"]["No"]));
		}
	}

	strType.clear();
	strControl.clear();
	JsonRoot.clear();

	return;
}

bool CMainFrame::InitFunction()
{
	m_func.JsonStringToADO(m_strADO.c_str());
	m_func.JsonFileToMES("../File/MES.json");
	m_func.JsonFileToTraffics("../File/Traffics.json");
	m_func.JsonFileToServer("../File/Server.json");
	m_func.JsonFileToRFIDExchanges("../File/ExchangeRFID.json");

	ifstream fin;
	fin.open("../File/Map.json");
	if (!fin)
	{
		//TCHAR("error");
		return false;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	string strJson = ostring.str();

	Json::Value JsonRoot;

	const char* pstr = strJson.c_str();

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return false;
	}

	Json::Value JsonAGV,JsonRFID,JsonStation,JsonZigbee;

	JsonAGV = LoadAGV(JsonRoot["AGV"].toStyledString().c_str());
	JsonRFID = LoadRFID(JsonRoot["RFID"].toStyledString().c_str());
	JsonStation = LoadStation(JsonRoot["Station"].toStyledString().c_str());

	m_func.JsonStringToAGVs(JsonAGV.toStyledString().c_str());
	m_func.JsonStringToStations(JsonStation.toStyledString().c_str());

	ostring.str("");

	fin.open("../File/Zigbee.json");
	if (!fin)
	{
		//TCHAR("error");
		return false;
	}
	ostring << fin.rdbuf();
	fin.close();

	strJson= ostring.str();

	pstr = strJson.c_str();

	if (m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonZigbee, &m_JstrErrs))
	{
		LoadCaller(JsonRoot["Caller"].toStyledString().c_str(), JsonZigbee);

		m_func.JsonStringToZigbee(JsonZigbee.toStyledString().c_str());
	}

	return m_func.Init(UpdateStatus, this);
}

bool CMainFrame::OpenEditMode()
{
	//m_wndToolBar.Invalidate();

	PostThreadMessage(ThreadEx::GetThreadID(m_pModeThread), WM_USER + 1, 0, 1);

	return true;
}

bool CMainFrame::OpenRunMode()
{
	PostThreadMessage(ThreadEx::GetThreadID(m_pModeThread), WM_USER + 1, 1, 0);

	return true;
}

bool CMainFrame::CreateWidget(Json::Value& JsonValue)
{
	return m_wndView.CreateWidget(JsonValue);
}

bool CMainFrame::EditWidget(Json::Value& JsonValue)
{
	return m_wndView.EditWidget(m_wndView.GetWidget(JsonValue));
}

bool CMainFrame::DeleteWidget(Json::Value& JsonValue)
{
	return m_wndView.DeleteWidget(JsonValue);
}

bool CMainFrame::CreateTask(const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGV, const char* strType)
{
	return m_func.CreateTask(byStart, byEnd, byAGV, strType);
}

bool CMainFrame::CancelTask(int nTaskNo)
{
	return m_func.FinishTask(nTaskNo, "用户手动取消任务");
}

bool CMainFrame::SetEnableCaller(Json::Value& JsonValue, BOOL bEnable)
{
	return m_wndView.SetEnableCaller(m_wndView.GetWidget(JsonValue), bEnable);
}

bool CMainFrame::SetEnableStation(Json::Value& JsonValue, BOOL bEnable)
{
	return m_wndView.SetEnableStation(m_wndView.GetWidget(JsonValue), bEnable);
}

void CMainFrame::SetLevel(int nLevel)
{
	m_nUserLevel = nLevel;
	m_dlgAGVStatus.SetLevel(m_nUserLevel);
	m_dlgCallerStatus.SetLevel(m_nUserLevel);
	m_dlgStationConfig.SetLevel(m_nUserLevel);
	m_dlgTask.SetLevel(m_nUserLevel);
	m_wndView.SetLevel(m_nUserLevel);

	return;
}

void CMainFrame::ControlAGV(Json::Value& JsonValue)
{
	int nResult = m_func.ControlAGV(JsonValue);

	switch(nResult)
	{
	case -1:
		MessageBox(_T("Error:The AGV is null"),_T("操作AGV"),MB_ICONERROR);
		break;
	case ERROR_NONE:
		MessageBox(_T("操作成功"), _T("操作AGV"), MB_ICONERROR);
		break;
	case ERROR_END:
		MessageBox(_T("Error:The end point is null"), _T("操作AGV"), MB_ICONERROR);
		break;
	case ERROR_CONNECT:
		MessageBox(_T("Error:AGV is not connected"), _T("操作AGV"), MB_ICONERROR);
		break;
	case ERROR_BATTERY:
		MessageBox(_T("Error:AGV battery is empty"), _T("操作AGV"), MB_ICONERROR);
		break;
	case ERROR_STATUS:
		MessageBox(_T("Error:The AGV is not ready"), _T("操作AGV"), MB_ICONERROR);
		break;
	case ERROR_INTERNET:
		MessageBox(_T("Error:The AGV is connected"), _T("操作AGV"), MB_ICONERROR);
		break;
	default:
		MessageBox(_T("Error:Unknow"), _T("操作AGV"), MB_ICONERROR);
		break;
	}
}

bool __stdcall CMainFrame::UpdateStatus(Json::Value* JsonValue, void* pPtr)
{
	if (pPtr == nullptr || ((CMainFrame*)pPtr)->m_bDestroy || ((CMainFrame*)pPtr)->m_hWnd == NULL)
	{
		return false;
	}

	if (JsonValue == nullptr)
	{
		return false;
	}

	if (JsonValue->empty())
	{
		return false;
	}

	BaseFunction::g_mutex.lock();
	Json::Value JsonRoot(*JsonValue);
	BaseFunction::g_mutex.unlock();

	string strType = Json_ReadString(JsonRoot["Type"]);

	if (strcmp(strType.c_str(),"Caller")==0)
	{
		return ((CMainFrame*)pPtr)->UpdateCallerStatus(JsonRoot);
	}
	else if (strcmp(strType.c_str(), "AGV") == 0)
	{
		return ((CMainFrame*)pPtr)->UpdateAGVStatus(JsonRoot);
	}
	else if (strcmp(strType.c_str(), "Server") == 0)
	{
		return ((CMainFrame*)pPtr)->UpdateServerStatus(JsonRoot);
	}
	else if (strcmp(strType.c_str(), "Task") == 0)
	{
		return ((CMainFrame*)pPtr)->UpdateTaskStatus(JsonRoot);
	}
	else if (strcmp(strType.c_str(), "Key") == 0)
	{
		return ((CMainFrame*)pPtr)->UpdateKeyStatus(JsonRoot);
	}

	return false;
}

void CMainFrame::ModeThread()
{
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
			{
				return;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (WaitForSingleObject(m_hShutdownEvent, 1) == WAIT_OBJECT_0)
			{
				m_func.Release();
				m_func.ReleaseString();

				break;
			}

			if (msg.message == WM_USER+1)
			{
				// 运行
				if (msg.wParam == 1)
				{
					if (InitFunction() == false)
					{
						m_dlgAGVStatus.OpenEditMode();
						m_dlgTask.SetEnableControl(false);
						m_dlgCallerStatus.SetEnableEdit(true);
						m_dlgStationConfig.SetEnableEdit(true);

						m_wndView.SetMode(true);
						MessageBox(_T("模式切换失败!\n原因：缺少必要的运行参数."), _T("模式"),MB_ICONERROR);
						continue;
					}

					if (m_func.Open(UpdateStatus, this) == false)
					{
						m_func.Release();

						m_dlgAGVStatus.OpenEditMode();
						m_dlgTask.SetEnableControl(false);
						m_dlgCallerStatus.SetEnableEdit(true);
						m_dlgStationConfig.SetEnableEdit(true);

						m_wndView.SetMode(true);

						MessageBox(_T("模式切换失败!\n原因：启动功能失败."), _T("模式"), MB_ICONERROR);
						continue;
					}

					if (m_bDestroy)
					{
						continue;
					}

					m_dlgAGVStatus.OpenRunMode();
					m_dlgTask.SetEnableControl(true);
					m_dlgCallerStatus.SetEnableEdit(false);
					m_dlgStationConfig.SetEnableEdit(false);

					//m_wndToolBar.Invalidate();
					m_wndView.SetMode(false);
				}
				// 编辑
				else if (msg.lParam == 1)
				{
					m_strServerStatus = "未启动服务端";

					m_func.Release();

					if (m_bDestroy)
					{
						m_func.ReleaseString();
						break;
					}

					m_dlgAGVStatus.OpenEditMode();
					m_dlgTask.SetEnableControl(false);
					m_dlgCallerStatus.SetEnableEdit(true);
					m_dlgStationConfig.SetEnableEdit(true);

					m_wndView.SetMode(true);
				}
				else
				{
					m_func.Release();
					m_func.ReleaseString();

					break;
				}
			}
		}
	}

	return;
}

void CMainFrame::JsonStringToADO(const char* strJson)
{
	m_strADO = strJson;

	return;
}

void CMainFrame::JsonStringToADO(const wchar_t* wstrJson)
{
	m_strADO = StringEx::wstring_to_string(wstrJson);

	return;
}

void CMainFrame::JsonFileToADO(const char* strFile)
{
	ifstream fin;
	fin.open(strFile);
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strADO = ostring.str();

	return;
}

void CMainFrame::JsonFileToADO(const wchar_t* wstrFile)
{
	ifstream fin;
	fin.open(StringEx::wstring_to_string(wstrFile).c_str());
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strADO = ostring.str();

	return;
}

void CMainFrame::InitADO()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strADO.c_str();

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化ADO失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	m_ado.m_bstrConnect = Json_ReadString(JsonRoot["Connect"]).c_str();
	//m_ado.m_bstrUser = Json_ReadString(JsonRoot["User"]).c_str();
	//m_ado.m_bstrPassword = Json_ReadString(JsonRoot["Password"]).c_str();
	m_ado.m_bstrUser = _T("HarbinBrain");
	m_ado.m_bstrPassword = _T("Harbin045158973970Brain");

	m_ado.m_bstrConnect += (_T(";User ID=") + m_ado.m_bstrUser + _T(";Password=") + m_ado.m_bstrPassword + _T(";"));

	JsonRoot["User"] = Json::Value("HarbinBrain");
	JsonRoot["Password"] = Json::Value("Harbin045158973970Brain");
	JsonRoot["Connect"] = Json::Value((char*)m_ado.m_bstrConnect);

	m_strADO = JsonRoot.toStyledString().c_str();

	JsonRoot.clear();

	return;
}

Json::Value CMainFrame::LoadAGV(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取AGV图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return Json::Value();
	}

	Json::Value JsonValue,JsonArr;
	string strSQL = "";

	_RecordsetPtr pRecordset = nullptr;
	_variant_t var;

	for (size_t k = 0; k < JsonRoot.size(); ++k, JsonArr.clear(), strSQL = "", pRecordset = nullptr)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		unsigned short usCurRFID = 0,usEndRFID = 0;
		unsigned char byStatus = 0;

		JsonArr["No"] = JsonRoot[(int)k]["No"];

		JsonArr["Param"]["ClientIP"] = JsonRoot[(int)k]["Param"]["ClientIP"];
		JsonArr["Param"]["ClientPort"] = JsonRoot[(int)k]["Param"]["ClientPort"];
		JsonArr["Param"]["ServerIP"] = JsonRoot[(int)k]["Param"]["ServerIP"];
		JsonArr["Param"]["ServerPort"] = JsonRoot[(int)k]["Param"]["ServerPort"];

		strSQL.append(StringEx::string_format(
			"select [agv_cur_rfid],[agv_end_rfid],[agv_status] from AGV_CONFIG_TABLE where [agv_id]=%d"
			, byNo));

		if (m_ado.GetRecordSet(strSQL.c_str(), pRecordset))
		{
			if (!pRecordset->adoEOF)
			{
				var = pRecordset->GetCollect(_bstr_t("agv_cur_rfid"));
				if (var.vt != VT_NULL)
				{
					usCurRFID = (unsigned short)var;
				}

				var = pRecordset->GetCollect(_bstr_t("agv_end_rfid"));
				if (var.vt != VT_NULL)
				{
					usEndRFID = (unsigned short)var;
				}

				var = pRecordset->GetCollect(_bstr_t("agv_status"));
				if (var.vt != VT_NULL)
				{
					byStatus = (unsigned char)var;
				}
			}

			//m_ado.Release();
		}

		JsonArr["RFID"]["Cur"] = Json::Value(usCurRFID);
		JsonArr["RFID"]["End"] = Json::Value(usEndRFID);
		JsonArr["Status"] = Json::Value(byStatus);

		JsonValue["AGV"].append(JsonArr);
	}

	return JsonValue;
}

void CMainFrame::LoadCaller(const char* strJson, Json::Value& json)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取呼叫器图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	Json::Value JsonArr;

	string strSQL = "";

	_RecordsetPtr pRecordset = nullptr;
	_variant_t var;

	for (size_t k = 0; k < JsonRoot.size(); ++k, strSQL = "", pRecordset = nullptr)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		bool bPackOutbound = false;
		unsigned char bySignalCall = 0, bySignalCancelCall = 0, bySignalPass = 0, bySignalCancelPass = 0, bySignalNG = 0, bySignalCancelNG = 0;

		JsonArr["No"] = JsonRoot[(int)k]["No"];
		JsonArr["Station"] = JsonRoot[(int)k]["Station"];
		JsonArr["Use"] = JsonRoot[(int)k]["Param"]["Use"];
		JsonArr["MAC"] = JsonRoot[(int)k]["Param"]["MAC"];

		strSQL.append(StringEx::string_format(
			"select [caller_packoutbound],[caller_signal_call],[caller_signal_cancelcall],[caller_signal_pass],[caller_signal_cancelpass],[caller_signal_ng],[caller_signal_cancelng] "\
			"from CALLER_CONFIG_TABLE where [caller_id]=%d"
			, byNo));

		if (m_ado.GetRecordSet(strSQL.c_str(), pRecordset))
		{
			if (!pRecordset->adoEOF)
			{
				var = pRecordset->GetCollect(_bstr_t("caller_packoutbound"));
				if (var.vt != VT_NULL)
				{
					bPackOutbound = (int)var;
				}
				var = pRecordset->GetCollect(_bstr_t("caller_signal_call"));
				if (var.vt != VT_NULL)
				{
					bySignalCall = (unsigned char)var;
				}
				var = pRecordset->GetCollect(_bstr_t("caller_signal_cancelcall"));
				if (var.vt != VT_NULL)
				{
					bySignalCancelCall = (unsigned char)var;
				}
				var = pRecordset->GetCollect(_bstr_t("caller_signal_pass"));
				if (var.vt != VT_NULL)
				{
					bySignalPass = (unsigned char)var;
				}
				var = pRecordset->GetCollect(_bstr_t("caller_signal_cancelpass"));
				if (var.vt != VT_NULL)
				{
					bySignalCancelPass = (unsigned char)var;
				}
				var = pRecordset->GetCollect(_bstr_t("caller_signal_ng"));
				if (var.vt != VT_NULL)
				{
					bySignalNG = (unsigned char)var;
				}
				var = pRecordset->GetCollect(_bstr_t("caller_signal_cancelng"));
				if (var.vt != VT_NULL)
				{
					bySignalCancelNG = (unsigned char)var;
				}
			}

			//m_ado.Release();
		}

		JsonArr["PackOutBound"] = Json::Value(bPackOutbound);
		JsonArr["Signal"]["Call"] = Json::Value(bySignalCall);
		JsonArr["Signal"]["CancelCall"] = Json::Value(bySignalCancelCall);
		JsonArr["Signal"]["Pass"] = Json::Value(bySignalPass);
		JsonArr["Signal"]["CancelPass"] = Json::Value(bySignalCancelPass);
		JsonArr["Signal"]["NG"] = Json::Value(bySignalNG);
		JsonArr["Signal"]["CancelNG"] = Json::Value(bySignalCancelNG);

		json["Caller"].append(JsonArr);
	}

	return;
}

Json::Value CMainFrame::LoadRFID(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取RFID图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return Json::Value();
	}

	Json::Value JsonValue, JsonArr;

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);

		JsonValue["RFID"].append(byNo);
	}

	return JsonValue;
}

Json::Value CMainFrame::LoadStation(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取工位图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return Json::Value();
	}

	Json::Value JsonValue, JsonArr;

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		JsonArr["No"] = JsonRoot[(int)k]["No"];
		JsonArr["Location"] = JsonRoot[(int)k]["Location"];
		JsonArr["Name"] = JsonRoot[(int)k]["Name"];
		JsonArr["Param"]["Process"] = JsonRoot[(int)k]["Param"]["Process"];
		JsonArr["Param"]["MESWP"] = JsonRoot[(int)k]["Param"]["MESWP"];
		JsonArr["Param"]["MES"] = JsonRoot[(int)k]["Param"]["MES"];

		JsonValue["Station"].append(JsonArr);
	}

	return JsonValue;
}

void CMainFrame::InitDialog()
{
	m_dlgUserLogin.JsonStringToADO(m_strADO.c_str());
	m_dlgAddUser.JsonStringToADO(m_strADO.c_str());
	m_dlgEditUser.JsonStringToADO(m_strADO.c_str());
	m_dlgDeleteUser.JsonStringToADO(m_strADO.c_str());

	return;
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	stringstream ss;
	system_clock::time_point tpNow = system_clock::now();
	auto t = system_clock::to_time_t(tpNow);
	// 获取日期
	tm tmTime;
	localtime_s(&tmTime, &t);
	// 合成文本
	ss << put_time(&tmTime, "%Y-%m-%d %X");

	m_wndStatusBar.SetPaneText(4, StringEx::string_to_CString(m_strUser.c_str()));
	m_wndStatusBar.SetPaneText(5, StringEx::string_to_CString(ss.str().c_str()));
	m_wndStatusBar.SetPaneText(6, StringEx::string_to_CString(m_strServerStatus.c_str()));

	CFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWndEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_wndTabCtrl.GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rc,rcStaus,rcMenu,rcToolBar;
	m_wndMenuBar.GetClientRect(&rcMenu);
	m_wndToolBar.GetClientRect(&rcToolBar);
	m_wndStatusBar.GetClientRect(&rcStaus);

	GetClientRect(rc);

	if (m_wndMenuBar.IsDocked())
	{
		rc.top += rcMenu.Height();
	}

	if (m_wndToolBar.IsDocked())
	{
		rc.top += 8;

		rc.top += rcToolBar.Height();
	}

	rc.bottom -= rcStaus.Height();

	m_wndTabCtrl.MoveWindow(rc);
	m_wndTabCtrl.GetClientRect(&rc);

	rc.top += m_wndTabCtrl.GetTabsHeight();

	m_wndView.MoveWindow(&rc);
	m_dlgTask.MoveWindow(&rc);
	m_dlgAGVStatus.MoveWindow(&rc);
	m_dlgCallerStatus.MoveWindow(&rc);
	m_dlgStationConfig.MoveWindow(&rc);
}


void CMainFrame::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 在此处添加消息处理程序代码
					   // 不为绘图消息调用 CFrameWndEx::OnPaint()

	if (m_wndTabCtrl.GetSafeHwnd() == false)
	{
		return;
	}

	CWnd* m_wnd = m_wndTabCtrl.GetActiveWnd();//得到活动窗体的句柄
	m_wnd->RedrawWindow();//使窗体重画
}


//void CMainFrame::OnLButtonDblClk(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CFrameWndEx::OnLButtonDblClk(nFlags, point);
//}


void CMainFrame::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值}
	m_bDestroy = true;

	if (m_pModeThread)
	{
		if (m_pModeThread->joinable())
		{
			PostThreadMessage(ThreadEx::GetThreadID(m_pModeThread), WM_USER + 1, 0, 0);
			SetEvent(m_hShutdownEvent);

			m_pModeThread->join();
		}

		delete m_pModeThread;
		m_pModeThread = nullptr;
	}

	CFrameWndEx::OnClose();
}


void CMainFrame::OnUserRelogin()
{
	// TODO: 在此添加命令处理程序代码
	if (m_dlgUserLogin.DoModal() != IDOK)
	{
		return;
	}

	SetLevel(m_dlgUserLogin.m_nLevel);

	switch (m_dlgUserLogin.m_nLevel)
	{
	case USER_NORMAL:
		m_strUser = StringEx::string_format("普通用户：%s 已登陆"
			, StringEx::CString_to_string(m_dlgUserLogin.m_cstrUserName).c_str());
		break;
	case USER_CONTROLER:
		m_strUser = StringEx::string_format("操作员：%s 已登陆"
			, StringEx::CString_to_string(m_dlgUserLogin.m_cstrUserName).c_str());
		break;
	case USER_ADMINISTER:
		m_strUser = StringEx::string_format("管理员：%s 已登陆"
			, StringEx::CString_to_string(m_dlgUserLogin.m_cstrUserName).c_str());
		break;
	}

	return;
}


void CMainFrame::OnUpdateUserAdd(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1);
}


void CMainFrame::OnUpdateUserEdit(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1);
}


void CMainFrame::OnUpdateUserDelete(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1);
}


void CMainFrame::OnUserAdd()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgAddUser.DoModal();
}


void CMainFrame::OnUserEdit()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgEditUser.DoModal();
}


void CMainFrame::OnUserDelete()
{
	// TODO: 在此添加命令处理程序代码
	m_dlgDeleteUser.DoModal();
}


void CMainFrame::OnDestroy()
{
	// TODO: 在此处添加消息处理程序代码
	m_ado.ExitConnect();

	CFrameWndEx::OnDestroy();
}
