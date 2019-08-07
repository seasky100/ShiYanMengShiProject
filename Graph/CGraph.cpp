/*!
 * @file CGraph.cpp
 * @brief CGraph类执行文件
 * @author FanKaiyu
 * @date 2018-10-16
 * @version 2.0
*/

#include "CGraph.h"

#ifdef _CGRAPH_H

bool CGraph::g_bFlash = false;
ULL CGraph::g_ullFlashTime = 0;
PointF CGraph::g_ptOffset = PointF(0,0);
float CGraph::g_fScale = DEFALUT_SCALE;
unsigned int CGraph::g_unDistance = 10;
Size CGraph::g_sizeVector = Size(4800, 4800);
RectF CGraph::g_rcClient = RectF(0,0,0,0);
bool CGraph::g_bRefresh = true;
bool CGraph::g_bEdit = false;

CGraph::CGraph()
{
	// 初始化GDI+
	GdiplusStartup(&m_ulGdiplusToken, &m_gdiplusInput, &m_gdiplusOutput);
}


CGraph::~CGraph()
{
	// 关闭GDI+
	GdiplusShutdown(m_ulGdiplusToken);
}

#ifdef _AFX
void CGraph::SetClientRect(const CRect rcClient)
{
	g_rcClient = CRectToRectF(rcClient);

	return;
}
#endif //_AFX

const RectF CGraph::GetClientRect()
{
	return g_rcClient;
}

#ifdef _AFX
const CRect CGraph::GetClientRectC()
{
	return RectToCRect(g_rcClient);;
}
#endif //_AFX

void CGraph::SetClientRect(const RectF rcClient)
{
	if (g_rcClient.Equals(rcClient) == false)
	{
		g_rcClient = rcClient;

		Refresh();
	}
	
	return;
}

const PointF CGraph::GetOffset()
{
	return g_ptOffset;
}

void CGraph::SetOffset(const Point ptOffset)
{
	return SetOffset(PointToPointF(ptOffset));
}

void CGraph::SetOffset(const PointF ptOffset)
{
	if (g_ptOffset.Equals(ptOffset) == false)
	{
		g_ptOffset = ptOffset;

		Refresh();
	}

	return;
}

#ifdef _AFX
void CGraph::SetOffset(const CPoint ptOffset)
{
	return SetOffset(CPointToPoint(ptOffset));
}
#endif //_AFX

const float CGraph::GetScale()
{
	return g_fScale;
}

void CGraph::SetScale(float fScale)
{
	if (g_fScale != fScale)
	{
		g_fScale = fScale;

		Refresh();
	}

	return;
}

const PointF CGraph::VecToWin(const PointF ptVector)
{
	REAL fX = ptVector.X * g_fScale + g_rcClient.GetRight() / 2.0f + g_ptOffset.X;
	REAL fY = -ptVector.Y * g_fScale + g_rcClient.GetBottom() / 2.0f + g_ptOffset.Y;

	return PointF(fX, fY);
}

const Point CGraph::VecToWin(const Point ptVector)
{
	return PointFToPoint(VecToWin(PointToPointF(ptVector)));
}

#ifdef _AFX
const CPoint CGraph::VecToWin(const CPoint ptVector)
{
	return PointToCPoint(VecToWin(CPointToPoint(ptVector)));
}
#endif //_AFX

const RectF CGraph::VecToWin(const RectF rcVector)
{
	RectF rcWin;
	rcWin.X = rcVector.X * g_fScale + g_rcClient.GetRight() / 2.0f + g_ptOffset.X;
	rcWin.Y = -rcVector.Y * g_fScale + g_rcClient.GetBottom() / 2.0f + g_ptOffset.Y;

	rcWin.Width = rcVector.Width * g_fScale;
	rcWin.Height = rcVector.Height * g_fScale;

	return rcWin;
}

const Rect CGraph::VecToWin(const Rect rcVector)
{
	return RectFToRect(VecToWin(RectToRectF(rcVector)));
}

const PointF CGraph::WinToVec(const PointF ptWindow)
{
	REAL fX0 = g_rcClient.GetRight() / 2.0f;	/*!< 客户区中心X坐标 */
	REAL fY0 = g_rcClient.GetBottom() / 2.0f;	/*!< 客户区中心Y坐标 */

	REAL fX = ptWindow.X - fX0;
	REAL fY = fY0 - ptWindow.Y;

	fX = (fX - g_ptOffset.X) / g_fScale;
	fY = (fY + g_ptOffset.Y) / g_fScale;

	return PointF(fX, fY);
}

const Point CGraph::WinToVec(const Point ptWindow)
{
	return PointFToPoint(WinToVec(PointToPointF(ptWindow)));
}

#ifdef _AFX
const CPoint CGraph::WinToVec(const CPoint ptWindow)
{
	return PointToCPoint(WinToVec(CPointToPoint(ptWindow)));
}
#endif //_AFXs

const RectF CGraph::WinToVec(const RectF rcWindow)
{
	RectF rc;
	REAL fX0 = g_rcClient.GetRight() / 2.0f;	/*!< 客户区中心X坐标 */
	REAL fY0 = g_rcClient.GetBottom() / 2.0f;	/*!< 客户区中心Y坐标 */

	REAL fX = rcWindow.X - fX0;
	REAL fY = fY0 - rcWindow.Y;

	fX = (fX - g_ptOffset.X) / g_fScale;
	fY = (fY + g_ptOffset.Y) / g_fScale;

	rc.Width = rcWindow.Width / g_fScale;
	rc.Height = rcWindow.Width / g_fScale;

	return rc;
}

const Rect CGraph::WinToVec(const Rect rcWindow)
{
	return RectFToRect(WinToVec(RectToRectF(rcWindow)));
}

const Point CGraph::Correct(Point ptVector)
{
	int nDis = g_unDistance;
	int nDx = abs(ptVector.X) % nDis; /*!< 距离最近方格的X距离 */
	int nDy = abs(ptVector.Y) % nDis; /*!< 距离最近方格的Y距离*/

	if (nDx > nDis / 2)
	{
		if (ptVector.X > 0)
		{
			ptVector.X += nDis;
		}
		else
		{
			ptVector.X -= nDis;
		}
	}

	if (nDy > nDis / 2)
	{
		if (ptVector.Y > 0)
		{
			ptVector.Y += nDis;
		}
		else
		{
			ptVector.Y -= nDis;
		}
	}

	if (ptVector.X > 0)
	{
		ptVector.X -= nDx;
	}
	else
	{
		ptVector.X += nDx;
	}

	if (ptVector.Y > 0)
	{
		ptVector.Y -= nDy;
	}
	else
	{
		ptVector.Y += nDy;
	}

	return ptVector;
}

const PointF CGraph::Correct(const PointF ptVector)
{
	return PointToPointF(Correct(PointFToPoint(ptVector)));
}

#ifdef _AFX
const CPoint CGraph::Correct(const CPoint ptVector)
{
	return PointToCPoint(CPointToPoint(ptVector));;
}
#endif //_AFX

void CGraph::Locate(Point ptLocation)
{
	return Locate(PointToPointF(ptLocation));
}

void CGraph::Locate(PointF ptLocation)
{
	PointF ptWin = VecToWin(ptLocation);

	PointF ptDis = PointF(g_rcClient.GetRight() / 2.0f, g_rcClient.GetBottom() / 2.0f) - ptWin;
	SetOffset(ptDis + g_ptOffset);

	return;
}

#ifdef _AFX
void CGraph::Locate(CPoint ptLocation)
{
	return Locate(CPointToPointF(ptLocation));
}
#endif //_AFX

void CGraph::Refresh(bool bFresh)
{
	g_bRefresh = bFresh;
	
	return;
}

void CGraph::SetEnableEdit(bool bEdit)
{
	g_bEdit = bEdit;

	return;
}

const bool CGraph::EnableEdit()
{
	return g_bEdit;
}

#endif // !_CGRAPH_H