#include "CGraphicWidget.h"

#ifdef _CGRAPHICWIDGET_H

CGraphicWidget::CGraphicWidget()
	:m_pGraphics(nullptr)
	, m_ptCenter(PointF(0, 0))
	, m_ptPrepoint(PointF(0, 0))
	, m_ptLastpoint(PointF(0, 0))
	, m_ptLocal(PointF(0, 0))
	, m_bMove(false)
	, m_bSelect(false)
	, m_pBrush(nullptr)
	, m_pPen(nullptr)
	, m_bNew(false)
	, m_pclBmp(nullptr)
	, m_bRefresh(true) 
{
	m_pBrush = new SolidBrush(Color::Black);
	m_pPen = new Pen(Color::Black);
}

CGraphicWidget::~CGraphicWidget()
{
	if (m_pclBmp)
	{
		delete m_pclBmp;
		m_pclBmp = nullptr;
	}

	if (m_pPen)
	{
		delete m_pPen;
		m_pPen = nullptr;
	}

	if (m_pBrush)
	{
		delete m_pBrush;
		m_pBrush = nullptr;
	}

	if (m_pGraphics == nullptr)
	{
		return;
	}

	HDC hdc = m_pGraphics->GetHDC();
	if (hdc != nullptr)
	{
		m_pGraphics->ReleaseHDC(hdc);
	}

	delete m_pGraphics;
	m_pGraphics = nullptr;
}

const bool CGraphicWidget::IsSelected() const
{
	return m_bSelect;
}

const bool CGraphicWidget::IsSelected(PointF& pt) const
{
	pt = m_ptPrepoint;

	return m_bSelect;
}

#ifdef _AFX
const bool CGraphicWidget::IsSelected(CPoint& pt) const
{
	pt = PointToCPoint(m_ptPrepoint);
	return m_bSelect;
}
#endif //_AFX

const bool CGraphicWidget::IsNewed() const
{
	return m_bNew;
}

const bool CGraphicWidget::IsMoveed() const
{
	return m_bMove;
}

void CGraphicWidget::Select(const PointF ptWinpoint)
{
	m_ptPrepoint = ptWinpoint;
	m_ptLastpoint = ptWinpoint;
	m_ptLocal = m_ptCenter;

	m_bMove = true;
	m_bSelect = true;

	m_bRefresh = true;

	return;
}

#ifdef _AFX
void CGraphicWidget::Select(const CPoint ptWinpoint)
{
	return Select(CPointToPointF(ptWinpoint));
}
#endif //_AFX

const HCURSOR CGraphicWidget::Drag(const PointF ptWinPoint)
{
	HCURSOR hCur = nullptr;

	m_ptLastpoint = ptWinPoint;

	if (m_bMove)
	{
		m_ptCenter.X = (m_ptLastpoint.X - m_ptPrepoint.X) / g_fScale + m_ptLocal.X;
		m_ptCenter.Y = -(m_ptLastpoint.Y - m_ptPrepoint.Y) / g_fScale + m_ptLocal.Y;

		if (m_ptCenter.X < -g_sizeVector.Width / 2.0f
			|| m_ptCenter.Y > g_sizeVector.Height / 2.0f
			|| m_ptCenter.X > g_sizeVector.Width / 2.0f
			|| m_ptCenter.Y < -g_sizeVector.Height / 2.0f)
		{
			m_ptCenter = m_ptLocal;
		}

		m_ptCenter = Correct(m_ptCenter);
		if (m_ptCenter.Equals(m_ptLocal) == false)
		{
			m_bNew = true;
			m_bRefresh = true;
		}
		else
		{
			m_bNew = false;
			m_bRefresh = false;
		}

		hCur = LoadCursor(NULL, IDC_HAND);
	}
	else
	{
		m_ptLocal = m_ptCenter;

		m_bNew = false;
		m_bRefresh = false;
	}

	return 	hCur;
}

#ifdef _AFX
const HCURSOR CGraphicWidget::Drag(const CPoint ptWinPoint)
{
	return Drag(CPointToPointF(ptWinPoint));
}
#endif //_AFX

void CGraphicWidget::Confirm()
{
	m_bMove = false;

	m_ptCenter = CGraph::Correct(m_ptCenter);

	m_ptLocal = m_ptCenter;

	m_bNew = false;

	return;
}

void CGraphicWidget::Cancel()
{
	m_ptCenter = m_ptLocal;
	m_bMove = false;
	m_bSelect = false;
	m_bNew = false;
	m_bRefresh = true;

	return;
}

const PointF CGraphicWidget::GetCenterPoint() const
{
	return m_ptCenter;
}
#ifdef _AFX
const CPoint CGraphicWidget::GetCenterPointC() const
{
	return PointToCPoint(m_ptCenter);
}
#endif //_AFX

const PointF CGraphicWidget::GetLocalPoint() const
{
	return m_ptLocal;
}
#ifdef _AFX
const CPoint CGraphicWidget::GetLocalPointC() const
{
	return PointToCPoint(m_ptLocal);
}
#endif //_AFX

void CGraphicWidget::SetCenterPoint(const PointF ptCenter)
{
	m_ptCenter = ptCenter;
	m_ptLocal = m_ptCenter;

	return;
}

#ifdef _AFX
void CGraphicWidget::SetCenterPoint(CPoint ptCenter)
{
	return SetCenterPoint(CPointToPointF(ptCenter));
}
#endif //_AFX

void CGraphicWidget::InitGraphic(HDC hdc)
{
	if (m_pGraphics == nullptr)
	{
		m_pGraphics = new Graphics(hdc);
	}
	else
	{
		HDC gHDC = m_pGraphics->GetHDC();

		if (gHDC != nullptr && gHDC != hdc)
		{
			m_pGraphics->ReleaseHDC(gHDC);
		}

		m_pGraphics->FromHDC(hdc);
	}

	return;
}

void CGraphicWidget::Locate()
{
	PointF ptWin = VecToWin(m_ptCenter);

	Select(ptWin);
	Confirm();

	CGraph::Locate(m_ptCenter);

	return;
}

void CGraphicWidget::Draw(HDC hdc)
{
	InitGraphic(hdc);

	Draw(m_pGraphics);

	return;
}

void CGraphicWidget::Draw(Graphics* graphic)
{
	DrawImage(graphic);

	/*
	m_bRefresh = true;

	Rect rGdi;
	graphic->GetVisibleClipBounds(&rGdi);

	if (m_bRefresh == false && g_bRefresh == false)
	{
		if (m_pclBmp == nullptr)
		{
			m_pclBmp = new Bitmap(rGdi.Width, rGdi.Height);
		}

		graphic->DrawImage(m_pclBmp, rGdi);

		return;
	}

	if (m_pclBmp)
	{
		delete m_pclBmp;
		m_pclBmp = nullptr;
	}

	m_pclBmp = new Bitmap(rGdi.Width, rGdi.Height);

	Graphics* grPtr = Graphics::FromImage(m_pclBmp);

	DrawImage(grPtr);

	delete grPtr;

	graphic->DrawImage(m_pclBmp, rGdi);

	m_bRefresh = false;
	*/

	return;
}

#endif // _CGRAPHICWIDGET_H