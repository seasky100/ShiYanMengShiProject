
// ChildView.cpp: CChildView 类的实现
//

#include "pch.h"
#include "framework.h"
#include "ProjectApp.h"
#include "ChildView.h"
#include "MainFrm.h"
#include "CSetRFIDDlg.h"
#include "CSetStationDlg.h"
#include "CSetCallerDlg.h"
#include "CSetAGVDlg.h"
#include "../CommunicateDemo/CTCP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_bCtrl = false;
	m_bEdit = false;
	m_nUserLevel = 0;
	m_pJsonReader = m_JsonBuilder.newCharReader();
}

CChildView::~CChildView()
{
	delete m_pJsonReader;
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MBUTTONDBLCLK()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_SIZE()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOL_RFID, &CChildView::OnToolRfid)
	ON_COMMAND(ID_TOOL_STATION, &CChildView::OnToolStation)
	ON_COMMAND(ID_TOOL_CALLER, &CChildView::OnToolCaller)
	ON_COMMAND(ID_TOOL_TRACK_LINE, &CChildView::OnToolTrackLine)
	ON_COMMAND(ID_TOOL_TRACK_ARC, &CChildView::OnToolTrackArc)
	ON_UPDATE_COMMAND_UI(ID_TOOL_RFID, &CChildView::OnUpdateToolRfid)
	ON_UPDATE_COMMAND_UI(ID_TOOL_CHARGER, &CChildView::OnUpdateToolCharger)
	ON_UPDATE_COMMAND_UI(ID_TOOL_REST, &CChildView::OnUpdateToolRest)
	ON_UPDATE_COMMAND_UI(ID_TOOL_STATION, &CChildView::OnUpdateToolStation)
	ON_UPDATE_COMMAND_UI(ID_TOOL_CALLER, &CChildView::OnUpdateToolCaller)
	ON_UPDATE_COMMAND_UI(ID_TOOL_TRACK_LINE, &CChildView::OnUpdateToolTrackLine)
	ON_UPDATE_COMMAND_UI(ID_TOOL_TRACK_ARC, &CChildView::OnUpdateToolTrackArc)
	ON_COMMAND(ID_MODE_EDIT, &CChildView::OnModeEdit)
	ON_UPDATE_COMMAND_UI(ID_MODE_EDIT, &CChildView::OnUpdateModeEdit)
	ON_COMMAND(ID_MODE_RUN, &CChildView::OnModeRun)
	ON_UPDATE_COMMAND_UI(ID_MODE_RUN, &CChildView::OnUpdateModeRun)
	ON_COMMAND(ID_MAP_SAVE, &CChildView::OnMapSave)
	ON_UPDATE_COMMAND_UI(ID_MAP_SAVE, &CChildView::OnUpdateMapSave)
END_MESSAGE_MAP()



// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码
	m_map.Init();

	m_map.Draw(dc.GetSafeHdc());
	
	/*
	// 不要为绘制消息而调用 CWnd::OnPaint()
		//设置兼容区域，一般为当前窗口大小
	CRect RectCompatible;

	//建立兼容DC，即后面那块画布
	CDC Memdc;
	CBitmap Bitmap;

	//得到当前窗口的大小
	GetClientRect(&RectCompatible);
	Memdc.CreateCompatibleDC(&dc);

	//创建兼容位图
	Bitmap.CreateCompatibleBitmap(&dc, RectCompatible.Width(), RectCompatible.Height());

	//把位图选到兼容DC中，这样才可以开始画
	Memdc.SelectObject(Bitmap);
	Memdc.FillSolidRect(RectCompatible.left, RectCompatible.top, RectCompatible.Width(), RectCompatible.Height(), RGB(255, 255, 255));//用白色先刷新整个兼容DC，这相当于原来的刷新背景

	// m_map.Draw(Memdc.GetSafeHdc());

	//位图中图片加载到画布上
	//pDC->BitBlt(RectCompatible.left, RectCompatible.top, RectCompatible.Width(), RectCompatible.Height(), &Memdc, RectCompatible.left, RectCompatible.top, SRCCOPY);//最后把兼容DC拷贝到pDC
	(&dc)->BitBlt(RectCompatible.left, RectCompatible.top, RectCompatible.Width(), RectCompatible.Height(), &Memdc, RectCompatible.left, RectCompatible.top, SRCCOPY);//最后把兼容DC拷贝到pDC
																																									//实现前后交替，便不会闪动
	Memdc.DeleteDC();
	Bitmap.DeleteObject();
	*/

	return;
}

BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// return CView::OnEraseBkgnd(pDC);

	return TRUE;
}

void CChildView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值	

	CWnd::OnClose();
}

void CChildView::OnDestroy()
{
	CWnd::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}

void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnLButtonDblClk(nFlags, point);

	if (m_nUserLevel < 2)
	{
		return ;
	}

	string strJson;
	EditWidget(m_map.GetWidget(CPointToPointF(point)));
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnLButtonDown(nFlags, point);

	if (m_nUserLevel < 2)
	{
		return;
	}

	m_map.Select(CPointToPointF(point));
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnLButtonUp(nFlags, point);

	if (m_nUserLevel < 2)
	{
		return;
	}

	m_map.Confirm(CPointToPointF(point));
}

void CChildView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnMButtonDblClk(nFlags, point);

	m_map.Reset();
}

void CChildView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnMButtonDown(nFlags, point);

	m_map.SelectBackground(CPointToPointF(point));
}

void CChildView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnMButtonUp(nFlags, point);

	m_map.ConfirmBackground();
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnRButtonDown(nFlags, point);

	if (m_nUserLevel < 2)
	{
		return;
	}
}

void CChildView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	CWnd::OnRButtonDblClk(nFlags, point);

	if (m_nUserLevel < 2)
	{
		return;
	}
}

void CChildView::OnSize(UINT nType, int cx, int cy)
{
	SetFocus();

	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rcClient;
	GetClientRect(rcClient);

	m_map.OnSize(CRectToRectF(rcClient));
}

void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
{
	SetFocus();

	CWnd::OnRButtonUp(nFlags, point);

	if (m_nUserLevel < 2)
	{
		return;
	}

	m_map.Cancel();
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CWnd::OnMouseMove(nFlags, point);

	if (m_nUserLevel < 2)
	{
		return;
	}

	HCURSOR hCur = m_map.Drag(CPointToPointF(point));

	if (hCur)
	{
		SetCursor(hCur);
	}
	else
	{
		if (GetCursor() != LoadCursor(NULL, IDC_ARROW))
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
		}
	}
	
}

BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	SetFocus();

	if (zDelta > 0)
	{
		m_map.ZoomedIn();
	}
	else
	{
		m_map.ZoomedOut();
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CChildView::UpdateWidget(Json::Value& JsonValue, void* pMainFram)
{
	((CMainFrame*)pMainFram)->UpdateWidget(JsonValue);

	return;
}

bool CChildView::DeleteWidget(Json::Value& JsonValue)
{
	return m_map.DeleteWidget(JsonValue);
}

Json::Value CChildView::GetWidget(Json::Value& JsonValue)
{
	return m_map.GetWidget(JsonValue);
}

bool CChildView::EditWidget(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonConfig;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	// 充电器
	if (strcmp(strType.c_str(), "Charger") == 0)
	{
		
	}
	// 待机位
	else if (strcmp(strType.c_str(), "Rest") == 0)
	{
		

	}
	// 工位
	else if (strcmp(strType.c_str(), "Station") == 0)
	{
		CSetStationDlg dlg;
		dlg.m_bEdit = true;
		dlg.m_byNo = Json_ReadInt(JsonParam["No"]);
		dlg.m_usLocation = Json_ReadInt(JsonParam["Location"]);
		dlg.m_cstrName = StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str()));
		dlg.m_byProcess = Json_ReadInt(JsonParam["Param"]["Process"]);
		dlg.m_cstrMESWP = StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Param"]["MESWP"]).c_str()));
		dlg.m_bMES = Json_ReadInt(JsonParam["Param"]["MES"]);

		if (dlg.DoModal() == IDOK)
		{
			JsonParam["Param"]["MESWP"] = Json::Value(StringEx::AcsiiToUtf8(StringEx::CString_to_string(dlg.m_cstrMESWP).c_str()));
			JsonParam["Param"]["MES"] = Json::Value(dlg.m_bMES);
			JsonParam["Param"]["Process"] = Json::Value(dlg.m_byProcess);
			JsonParam["Location"] = Json::Value(dlg.m_usLocation);
			JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(StringEx::CString_to_string(dlg.m_cstrName).c_str()));
			JsonRoot["Param"] = JsonParam;

			if (m_map.EditWidget(JsonRoot) == false)
			{
				MessageBox(_T("编辑工位失败!"), _T("编辑图标失败"), MB_ICONERROR);

				return false;
			}
		}
		else
		{
			return false;
		}
	}
	// AGV
	else if (strcmp(strType.c_str(), "AGV") == 0)
	{
		CSetAGVDlg dlg;
		dlg.m_bEdit = true;
		dlg.m_byNo = Json_ReadInt(JsonParam["No"]);
		dlg.m_dwLocalIP = TCP::IPAddressToDWORD(Json_ReadString(JsonParam["Param"]["ClientIP"]).c_str());
		dlg.m_dwServerIP = TCP::IPAddressToDWORD(Json_ReadString(JsonParam["Param"]["ServerIP"]).c_str());
		dlg.m_nLocalPort = Json_ReadInt(JsonParam["Param"]["ClientPort"]);
		dlg.m_nServerPort = Json_ReadInt(JsonParam["Param"]["ServerPort"]);

		if (dlg.DoModal() == IDOK)
		{
			// 服务端
			if (dlg.m_nRadio == 0)
			{
				JsonConfig["ServerIP"] = Json::Value(TCP::DWORDToIPAddress(dlg.m_dwServerIP));
				JsonConfig["ServerPort"] = Json::Value(dlg.m_nServerPort);
				JsonConfig["ClientIP"] = Json::Value("");
			}
			// 客户端
			else
			{
				JsonConfig["ServerIP"] = Json::Value("");
				JsonConfig["ServerPort"] = Json::Value(-1);
				JsonConfig["ClientIP"] = Json::Value(TCP::DWORDToIPAddress(dlg.m_dwLocalIP));
			}

			JsonConfig["ClientPort"] = Json::Value(dlg.m_nLocalPort);

			JsonParam["Param"] = JsonConfig;
			JsonRoot["Param"] = JsonParam;

			if (m_map.EditWidget(JsonRoot) == false)
			{
				MessageBox(_T("编辑AGV失败!"), _T("编辑图标失败"), MB_ICONERROR);

				return false;
			}
		}
		else
		{
			return false;
		}
	}
	// 呼叫器
	else if (strcmp(strType.c_str(), "Caller") == 0)
	{
		CSetCallerDlg dlg;
		dlg.m_bEdit = true;
		dlg.m_byNo = Json_ReadInt(JsonParam["No"]);
		dlg.m_byStation = Json_ReadInt(JsonParam["Station"]);
		dlg.m_bUse = Json_ReadInt(JsonParam["Param"]["Use"]);
		dlg.m_cstrMAC = StringEx::string_to_CString(Json_ReadString(JsonParam["Param"]["MAC"]).c_str());

		if (dlg.DoModal() == IDOK)
		{
			JsonParam["Param"]["MAC"] = Json::Value(StringEx::CString_to_string(dlg.m_cstrMAC).c_str());
			JsonParam["Param"]["Use"] = Json::Value(dlg.m_bUse);
			JsonParam["Station"] = Json::Value(dlg.m_byStation);
			JsonRoot["Param"] = JsonParam;

			if (m_map.EditWidget(JsonRoot) == false)
			{
				MessageBox(_T("编辑呼叫器失败!"), _T("编辑图标失败"), MB_ICONERROR);

				return false;
			}

		}
		else
		{
			return false;
		}
	}

	strType.clear();
	JsonConfig.clear();
	JsonParam.clear();

	return true;
}

bool CChildView::CreateWidget(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonConfig;

	string strType = Json_ReadString(JsonRoot["Type"]);

	// 充电器
	if (strcmp(strType.c_str(), "Charger") == 0)
	{

	}
	// 待机位
	else if (strcmp(strType.c_str(), "Rest") == 0)
	{


	}
	// 工位
	else if (strcmp(strType.c_str(), "Station") == 0)
	{
		OnToolStation();
	}
	// AGV
	else if (strcmp(strType.c_str(), "AGV") == 0)
	{
		CSetAGVDlg dlg;
		if (dlg.DoModal() == IDOK)
		{
			JsonParam["No"] = Json::Value(dlg.m_byNo);

			// 服务端
			if (dlg.m_nRadio == 0)
			{
				JsonConfig["ServerIP"] = Json::Value(TCP::DWORDToIPAddress(dlg.m_dwServerIP));
				JsonConfig["ServerPort"] = Json::Value(dlg.m_nServerPort);
				JsonConfig["ClientIP"] = Json::Value("");
			}
			// 客户端
			else
			{
				JsonConfig["ServerIP"] = Json::Value("");
				JsonConfig["ServerPort"] = Json::Value(-1);
				JsonConfig["ClientIP"] = Json::Value(TCP::DWORDToIPAddress(dlg.m_dwLocalIP));
			}

			JsonConfig["ClientPort"] = Json::Value(dlg.m_nLocalPort);

			JsonParam["Param"] = JsonConfig;
			JsonRoot["Param"] = JsonParam;

			if (m_map.CreateNewWidget(JsonRoot) == false)
			{
				MessageBox(_T("添加AGV失败!"), _T("添加图标失败"), MB_ICONERROR);

				return false;
			}

			JsonConfig.clear();
			JsonParam.clear();
		}
	}
	// 呼叫器
	else if (strcmp(strType.c_str(), "Caller") == 0)
	{
		OnToolCaller();
	}

	strType.clear();
	JsonConfig.clear();
	JsonParam.clear();

	return true;
}

void CChildView::UpdateAGVStatus(Json::Value& JsonValue)
{
	return m_map.UpdateAGVStatus(JsonValue);
}

void CChildView::UpdateCallerStatus(Json::Value& JsonValue)
{
	return m_map.UpdateCallerStatus(JsonValue);
}

void CChildView::UpdateChargerStatus(Json::Value& JsonValue)
{
	return m_map.UpdateChargerStatus(JsonValue);
}

void CChildView::JsonStringToADO(const char* strJson)
{
	m_strADO = strJson;

	return;
}

void CChildView::JsonStringToADO(const wchar_t* wstrJson)
{
	m_strADO = StringEx::wstring_to_string(wstrJson);

	return;
}

void CChildView::JsonFileToADO(const char* strFile)
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

void CChildView::JsonFileToADO(const wchar_t* wstrFile)
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

bool CChildView::SetEnableCaller(Json::Value& JsonValue, BOOL bEnable)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonConfig;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	// 呼叫器
	if (strcmp(strType.c_str(), "Caller") == 0)
	{
		JsonParam["Param"]["Use"] = Json::Value(bEnable);
		JsonRoot["Param"] = JsonParam;

		if (m_map.EditWidget(JsonRoot) == false)
		{
			MessageBox(_T("编辑呼叫器失败!"), _T("编辑图标失败"), MB_ICONERROR);

			return false;
		}
	}

	strType.clear();
	JsonConfig.clear();
	JsonParam.clear();

	return true;
}

bool CChildView::SetEnableStation(Json::Value& JsonValue, BOOL bEnable)
{
	Json::Value JsonRoot = JsonValue;
	Json::Value JsonParam, JsonConfig;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	// 工位
	if (strcmp(strType.c_str(), "Station") == 0)
	{
		JsonParam["Param"]["MES"] = Json::Value(bEnable);
		JsonRoot["Param"] = JsonParam;

		if (m_map.EditWidget(JsonRoot) == false)
		{
			MessageBox(_T("编辑工位失败!"), _T("编辑图标失败"), MB_ICONERROR);

			return false;
		}
	}

	strType.clear();
	JsonConfig.clear();
	JsonParam.clear();

	return true;
}

void CChildView::SetLevel(int nLevel)
{
	m_nUserLevel = nLevel;

	return;
}

void CChildView::SetMode(bool bEdit)
{
	if (bEdit)
	{
		m_map.OpenEditMode();
		m_bEdit = true;
	}
	else
	{
		m_bEdit = false;
	}

	return;
}

BOOL CChildView::PreTranslateMessage(MSG* pMsg)
{
	if (m_nUserLevel < 2)
	{
		return CWnd::PreTranslateMessage(pMsg);
	}

	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN)
	{
		switch (pMsg->wParam)
		{
		case VK_CONTROL:
			m_map.MultiSelect();
			m_bCtrl = true;
			break;
		default:
			break;
		}
	}
	else if (pMsg->message == WM_KEYUP)
	{
		switch (pMsg->wParam)
		{
		case VK_CONTROL:
			m_map.SignalSelect();
			m_bCtrl = false;
			break;
		case 'A':
			m_map.SelectAll();
			break;
		case 'S':
			m_map.SaveMap();
			break;
		case VK_ESCAPE:
			m_map.Cancel();
			break;
		case VK_DELETE:
			m_map.DeleteWidget();
			break;
		default:
			break;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_bEdit == false)
	{
		m_map.Refresh();
	}

	CRect rcClient;
	GetClientRect(rcClient);

	InvalidateRect(rcClient);
	UpdateWindow();

	CWnd::OnTimer(nIDEvent);
}

int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_map.JsonStringToADO(m_strADO.c_str());
	m_bEdit = m_map.Init(AfxGetApp()->m_pMainWnd, &UpdateWidget);

	SetTimer(1, (unsigned int)(30.0f / 1000.0f), NULL);
	//SetTimer(1, 10, NULL);

	if (m_bEdit)
	{
		OnModeEdit();
	}
	else
	{
		OnModeRun();
	}

	SetFocus();

	return 0;
}

void CChildView::OnToolRfid()
{
	// TODO: 在此添加命令处理程序代码
	CSetRFIDDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		Json::Value JsonRoot,JsonParam;
		JsonParam["No"] = Json::Value(dlg.m_usNo);
		JsonRoot["Type"] = Json::Value("RFID");
		JsonRoot["Param"] = JsonParam;

		if (m_map.CreateNewWidget(JsonRoot) == false)
		{
			MessageBox(_T("添加RFID卡失败!"), _T("添加图标失败"), MB_ICONERROR);
		}

		JsonParam.clear();
		JsonRoot.clear();
	}

	SetFocus();

	return;
}

void CChildView::OnToolStation()
{
	// TODO: 在此添加命令处理程序代码
	CSetStationDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		Json::Value JsonRoot, JsonParam,JsonConfig;
		JsonConfig["MESWP"] = Json::Value(StringEx::AcsiiToUtf8(StringEx::CString_to_string(dlg.m_cstrMESWP).c_str()));
		JsonConfig["MES"] = Json::Value(dlg.m_bMES);
		JsonConfig["Process"] = Json::Value(dlg.m_byProcess);
		JsonParam["No"] = Json::Value(dlg.m_byNo);
		JsonParam["Location"] = Json::Value(dlg.m_usLocation);
		JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(StringEx::CString_to_string(dlg.m_cstrName).c_str()));
		JsonRoot["Type"] = Json::Value("Station");
		JsonParam["Param"] = JsonConfig;
		JsonRoot["Param"] = JsonParam;

		if (m_map.CreateNewWidget(JsonRoot) == false)
		{
			MessageBox(_T("添加工位失败!"), _T("添加图标失败"), MB_ICONERROR);
		}

		JsonConfig.clear();
		JsonParam.clear();
		JsonRoot.clear();
	}

	SetFocus();

	return;
}

void CChildView::OnToolCaller()
{
	// TODO: 在此添加命令处理程序代码
	CSetCallerDlg dlg;
	if (dlg.DoModal() == IDOK)
	{
		Json::Value JsonRoot, JsonParam, JsonConfig;
		JsonConfig["MAC"] = Json::Value(StringEx::CString_to_string(dlg.m_cstrMAC).c_str());
		JsonConfig["Use"] = Json::Value(dlg.m_bUse);
		JsonParam["No"] = Json::Value(dlg.m_byNo);
		JsonParam["Station"] = Json::Value(dlg.m_byStation);
		JsonRoot["Type"] = Json::Value("Caller");
		JsonParam["Param"] = JsonConfig;
		JsonRoot["Param"] = JsonParam;

		if (m_map.CreateNewWidget(JsonRoot) == false)
		{
			MessageBox(_T("添加呼叫器失败!"), _T("添加图标失败"), MB_ICONERROR);
		}

		JsonParam.clear();
		JsonRoot.clear();
	}

	SetFocus();

	return;
}

void CChildView::OnToolTrackLine()
{
	// TODO: 在此添加命令处理程序代码
	Json::Value JsonRoot, JsonParam;
	JsonParam["Arc"] = Json::Value(false);
	JsonRoot["Type"] = Json::Value("Track");
	JsonRoot["Param"] = JsonParam;

	if (m_map.CreateNewWidget(JsonRoot) == false)
	{
		MessageBox(_T("添加磁轨失败!"), _T("添加图标失败"), MB_ICONERROR);
	}

	SetFocus();

	return;
}

void CChildView::OnToolTrackArc()
{
	// TODO: 在此添加命令处理程序代码
	Json::Value JsonRoot, JsonParam;
	JsonParam["Arc"] = Json::Value(true);
	JsonRoot["Type"] = Json::Value("Track");
	JsonRoot["Param"] = JsonParam;

	if (m_map.CreateNewWidget(JsonRoot) == false)
	{
		MessageBox(_T("添加磁轨失败!"), _T("添加图标失败"), MB_ICONERROR);
	}

	SetFocus();

	return;
}

void CChildView::OnUpdateToolRfid(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}

void CChildView::OnUpdateToolCharger(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}

void CChildView::OnUpdateToolRest(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}


void CChildView::OnUpdateToolStation(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}


void CChildView::OnUpdateToolCaller(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}


void CChildView::OnUpdateToolTrackLine(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}


void CChildView::OnUpdateToolTrackArc(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}


void CChildView::OnModeEdit()
{
	// TODO: 在此添加命令处理程序代码
	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->OpenEditMode() == false)
	{
		return;
	}
}


void CChildView::OnUpdateModeEdit(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 0 && !m_bEdit);
}


void CChildView::OnModeRun()
{
	m_map.CloseEditMode();

	// TODO: 在此添加命令处理程序代码
	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->OpenRunMode() == false)
	{
		//m_map.OpenEditMode();

		return;
	}

	return;
}


void CChildView::OnUpdateModeRun(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 0 && m_bEdit);
}


void CChildView::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类

	CWnd::PreSubclassWindow();
}


void CChildView::OnMapSave()
{
	// TODO: 在此添加命令处理程序代码
	if (m_bEdit == false)
	{
		return;
	}

	m_map.SaveMap();
}


void CChildView::OnUpdateMapSave(CCmdUI* pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(m_nUserLevel > 1 && m_bEdit);
}
