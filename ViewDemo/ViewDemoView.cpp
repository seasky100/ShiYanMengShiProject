
// ViewDemoView.cpp: CViewDemoView 类的实现
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "ViewDemo.h"
#endif

#include "ViewDemoDoc.h"
#include "ViewDemoView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewDemoView

IMPLEMENT_DYNCREATE(CViewDemoView, CView)

BEGIN_MESSAGE_MAP(CViewDemoView, CView)
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
//	ON_WM_MOUSEHWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CViewDemoView 构造/析构

CViewDemoView::CViewDemoView() noexcept
{
	// TODO: 在此处添加构造代码
	m_bCtrl = false;
}

CViewDemoView::~CViewDemoView()
{
}

BOOL CViewDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CViewDemoView 绘图

void CViewDemoView::OnDraw(CDC* pDC)
{
	CViewDemoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	//设置兼容区域，一般为当前窗口大小
	CRect RectCompatible;

	//建立兼容DC，即后面那块画布
	CDC Memdc;
	CBitmap Bitmap;

	//得到当前窗口的大小
	GetClientRect(&RectCompatible);
	Memdc.CreateCompatibleDC(pDC);

	//创建兼容位图
	Bitmap.CreateCompatibleBitmap(pDC, RectCompatible.Width(), RectCompatible.Height());

	//把位图选到兼容DC中，这样才可以开始画
	Memdc.SelectObject(Bitmap);
	Memdc.FillSolidRect(RectCompatible.left, RectCompatible.top, RectCompatible.Width(), RectCompatible.Height(), RGB(255, 255, 255));//用白色先刷新整个兼容DC，这相当于原来的刷新背景

	m_map.Draw(Memdc.GetSafeHdc());

	//位图中图片加载到画布上
	//pDC->BitBlt(RectCompatible.left, RectCompatible.top, RectCompatible.Width(), RectCompatible.Height(), &Memdc, RectCompatible.left, RectCompatible.top, SRCCOPY);//最后把兼容DC拷贝到pDC
	pDC->BitBlt(RectCompatible.left, RectCompatible.top, RectCompatible.Width(), RectCompatible.Height(), &Memdc, RectCompatible.left, RectCompatible.top, SRCCOPY);//最后把兼容DC拷贝到pDC
																																									//实现前后交替，便不会闪动
	Memdc.DeleteDC();
	Bitmap.DeleteObject();

	return;
}

void CViewDemoView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	//OnContextMenu(this, point);
}

void CViewDemoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CViewDemoView 诊断

#ifdef _DEBUG
void CViewDemoView::AssertValid() const
{
	CView::AssertValid();
}

void CViewDemoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CViewDemoDoc* CViewDemoView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CViewDemoDoc)));
	return (CViewDemoDoc*)m_pDocument;
}
#endif //_DEBUG 


// CViewDemoView 消息处理程序


BOOL CViewDemoView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// return CView::OnEraseBkgnd(pDC);

	return TRUE;
}


void CViewDemoView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnClose();
}


void CViewDemoView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
}


void CViewDemoView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnLButtonDblClk(nFlags, point);
}


void CViewDemoView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_map.Select(CPointToPointF(point));

	CView::OnLButtonDown(nFlags, point);
}


void CViewDemoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_map.Confirm();

	CView::OnLButtonUp(nFlags, point);
}


void CViewDemoView::OnMButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_map.Reset();

	CView::OnMButtonDblClk(nFlags, point);
}


void CViewDemoView::OnMButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_map.SelectBackground(CPointToPointF(point));

	CView::OnMButtonDown(nFlags, point);
}


void CViewDemoView::OnMButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_map.ConfirmBackground();

	CView::OnMButtonUp(nFlags, point);
}


void CViewDemoView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CView::OnRButtonDown(nFlags, point);
}


void CViewDemoView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_map.Cancel();

	CView::OnRButtonDblClk(nFlags, point);
}


void CViewDemoView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	CRect rcClient;
	GetClientRect(rcClient);

	m_map.OnSize(CRectToRectF(rcClient));
}


//void CViewDemoView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
//{
//	// 此功能要求 Windows Vista 或更高版本。
//	// _WIN32_WINNT 符号必须 >= 0x0600。
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CView::OnMouseHWheel(nFlags, zDelta, pt);
//}


void CViewDemoView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CView::OnMouseMove(nFlags, point);

	m_map.Drag(CPointToPointF(point));
}


BOOL CViewDemoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (zDelta > 0)
	{
		m_map.ZoomedIn();
	}
	else
	{
		m_map.ZoomedOut();
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


void CViewDemoView::UpdateWidget(const char* strJson, void* pMainFram)
{
	Json::CharReaderBuilder JsonBuilder;
	Json::CharReader* pJsonReader = JsonBuilder.newCharReader();

	Json::Value JsonRoot;
	JSONCPP_STRING JstrErrs;

	const char* pstr = strJson;

	if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &JstrErrs))
	{
		DebugPrint::Print(JstrErrs.c_str());
		DebugPrint::Print("更新图标失败:\n非JSON类型字符串");

		JstrErrs.clear();
		return;
	}

	string strControl = Json_ReadString(JsonRoot["Control"]);

	if (strcmp(strControl.c_str(), "Delete") == 0)
	{
		string strType = Json_ReadString(JsonRoot["Type"]);

		if (strcmp(strType.c_str(), "Station") == 0)
		{
			int nNo = Json_ReadInt(JsonRoot["No"]);

			// ((CMainFrame*)pMainFram)->
		}
		else if (strcmp(strType.c_str(), "Rest") == 0)
		{
			int nNo = Json_ReadInt(JsonRoot["No"]);
		}
		else if (strcmp(strType.c_str(), "Caller") == 0)
		{
			int nNo = Json_ReadInt(JsonRoot["No"]);
		}
		else if(strcmp(strType.c_str(), "Charger") == 0)
		{
			int nNo = Json_ReadInt(JsonRoot["No"]);
		}
	}

	strControl.clear();
	JsonRoot.clear();

	delete pJsonReader;
	return;
}

void CViewDemoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: 在此添加专用代码和/或调用基类
	m_map.JsonFileToADO("../File/ADO.json");
	m_map.Init(AfxGetApp()->m_pMainWnd, &UpdateWidget);

	SetTimer(1, 30.0f / 1000.0f, NULL);
}


BOOL CViewDemoView::PreTranslateMessage(MSG* pMsg)
{
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
			if (m_bCtrl)
			{
				m_map.SelectAll();
			}
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

	return CView::PreTranslateMessage(pMsg);
}


void CViewDemoView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	m_map.Refresh();

	CRect rcClient;
	GetClientRect(rcClient);

	InvalidateRect(rcClient);
	UpdateWindow();

	CView::OnTimer(nIDEvent);
}
