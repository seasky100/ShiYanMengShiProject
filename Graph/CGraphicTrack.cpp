#include "CGraphicTrack.h"

#ifdef _CGRAPHICTRACK_H

float CGraphicTrack::g_fWidth = 5.0f;

CGraphicTrack::CGraphicTrack(const bool bArc,PointF ptStart ,PointF ptEnd)
	:m_bArc(bArc)
	, m_bShow(false)
	, m_ptDistance(ptStart - ptEnd)

{
	m_ptCenter = ptStart;

	if (m_ptDistance.Equals(PointF(0,0)) == false)
	{
		m_bShow = true;
	}
 }

#ifdef _AFX
CGraphicTrack::CGraphicTrack(const bool bArc, CPoint ptStart, CPoint ptEnd)
	:m_bArc(bArc)
	, m_bShow(false)
	, m_ptDistance(CPointToPointF(ptStart - ptStart))
{
	m_ptCenter = CPointToPointF(ptStart);

	if (m_ptDistance.Equals(PointF(0, 0)) == false)
	{
		m_bShow = true;
	}
}
#endif //_AFX

CGraphicTrack::~CGraphicTrack()
{

}

void CGraphicTrack::SetCenterPoint(const PointF ptStart)
{
	if (m_bShow == false)
	{
		m_ptCenter = ptStart;
		m_ptLocal = m_ptCenter;
		m_bShow = true;

		m_ptDistance = PointF(0, 0);

		m_bRefresh = true;
	}
	
	return;
}

#ifdef _AFX
void CGraphicTrack::SetCenterPoint(const CPoint ptCenter)
{
	return SetCenterPoint(CPointToPointF(ptCenter));
}
#endif //_AFX

void CGraphicTrack::SetEndPoint(const PointF ptEnd)
{
	if (m_bShow == false)
	{
		return;
	}

	m_ptDistance = m_ptCenter - ptEnd;

	m_bRefresh = true;

	return;
}

#ifdef _AFX
void CGraphicTrack::SetEndPoint(const CPoint ptCenter)
{
	return SetEndPoint(CPointToPointF(ptCenter));
}
#endif //_AFX

void CGraphicTrack::SetType(bool bArc)
{
	m_bArc = bArc;

	m_bRefresh = true;

	return;
}

const PointF CGraphicTrack::GetEndPoint() const
{
	return m_ptCenter - m_ptDistance;
}

const PointF CGraphicTrack::GetLocalEndPoint() const
{
	return m_ptLocal - m_ptDistance;
}

const PointF CGraphicTrack::GetDistancePoint() const
{
	return m_ptDistance;
}

const bool CGraphicTrack::GetType() const
{
	return m_bArc;
}

const bool CGraphicTrack::IsInside(const PointF ptWinpoint)
{
	PointF ptStartWin, ptEndWin;
	ptStartWin = VecToWin(m_ptCenter);
	ptEndWin = VecToWin(GetEndPoint());

	float fW = g_fWidth * g_fScale; /*!< ¼ì²â°ë¾¶:¿í */
	float fH = g_fWidth * g_fScale; /*!< ¼ì²â°ë¾¶:¸ß */

	if (m_bArc)
	{
		RectF rcfTmp;
		rcfTmp.Width = fabsf(ptStartWin.X - ptEndWin.X) * 2.0f;
		rcfTmp.Height = fabsf(ptStartWin.Y - ptEndWin.Y) * 2.0f;

		float fLr = (rcfTmp.Width >= rcfTmp.Height ? rcfTmp.Width : rcfTmp.Height) / 2.0f; /*!< ³¤°ëÖá*/
		float fSr = (rcfTmp.Width <= rcfTmp.Height ? rcfTmp.Width : rcfTmp.Height) / 2.0f; /*!< ¶Ì°ëÖá */

		PointF ptMid(0,0); /*!< ÖÐÐÄ×ø±ê */

		float x = 0;
		float y = 0;
		float a = 0; /*!< ½Ç¶ÈÖµ */

		 // 1
		if (ptEndWin.X < ptStartWin.X && ptEndWin.Y < ptStartWin.Y)
		{
			rcfTmp.X = ptEndWin.X;
			rcfTmp.Y = ptEndWin.Y - rcfTmp.Height / 2.0f;

			ptMid.X = rcfTmp.X + rcfTmp.Width / 2.0f;
			ptMid.Y = rcfTmp.Y + rcfTmp.Height / 2.0f;

			for (float i = 0; i <= 90; i += 0.1f)
			{
				a = tanf(AngleToRadianF(i));

				// ÍÖÔ²½¹µãÔÚXÖá x^2/a^2+y^2/b^2=1£¬(a>b>0)£»
				if (ptMid.X + fSr < rcfTmp.X + rcfTmp.Width)
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / ((fSr*fSr) + ((fLr*fLr) * (a * a))));
				}
				// ÍÖÔ²½¹µãÔÚYÖá y^2/a^2+x^2/b^2=1£¬(a>b>0)£»
				else
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / (((fSr*fSr) * (a * a)) + (fLr*fLr)));
				}

				y = a * x;

				x = ptMid.X - x;
				y = ptMid.Y + y;

				RectF rcf(x - fW / 2.0f, y - fH / 2.0f, fW, fH);

				if (rcf.Contains(ptWinpoint))
				{
					return true;
				}
			}

		}
		// 2
		else if (ptEndWin.X > ptStartWin.X && ptEndWin.Y < ptStartWin.Y)
		{
			rcfTmp.X = ptStartWin.X - rcfTmp.Width / 2.0f;
			rcfTmp.Y = ptEndWin.Y - rcfTmp.Height / 2.0f;

			ptMid.X = rcfTmp.X + rcfTmp.Width / 2.0f;
			ptMid.Y = rcfTmp.Y + rcfTmp.Height / 2.0f;

			for (float i = 0; i <= 90; i += 0.1f)
			{
				a = tanf(AngleToRadianF(i));

				// ÍÖÔ²½¹µãÔÚXÖá x^2/a^2+y^2/b^2=1£¬(a>b>0)£»
				if (ptMid.X + fSr < rcfTmp.X + rcfTmp.Width)
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / ((fSr*fSr) + ((fLr*fLr) * (a * a))));
				}
				// ÍÖÔ²½¹µãÔÚYÖá y^2/a^2+x^2/b^2=1£¬(a>b>0)£»
				else
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / (((fSr*fSr) * (a * a)) + (fLr*fLr)));
				}

				y = a * x;

				x = ptMid.X + x;
				y = ptMid.Y + y;

				RectF rcf(x - fW / 2.0f, y - fH / 2.0f, fW, fH);

				if (rcf.Contains(ptWinpoint))
				{
					return true;
				}
			}

		}
		// 3
		else if (ptEndWin.X < ptStartWin.X && ptEndWin.Y > ptStartWin.Y)
		{
			rcfTmp.X = ptEndWin.X;
			rcfTmp.Y = ptStartWin.Y;

			ptMid.X = rcfTmp.X + rcfTmp.Width / 2.0f;
			ptMid.Y = rcfTmp.Y + rcfTmp.Height / 2.0f;

			for (float i = 0; i <= 90; i += 0.1f)
			{
				a = tanf(AngleToRadianF(i));

				// ÍÖÔ²½¹µãÔÚXÖá x^2/a^2+y^2/b^2=1£¬(a>b>0)£»
				if (ptMid.X + fSr < rcfTmp.X + rcfTmp.Width)
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / ((fSr*fSr) + ((fLr*fLr) * (a * a))));
				}
				// ÍÖÔ²½¹µãÔÚYÖá y^2/a^2+x^2/b^2=1£¬(a>b>0)£»
				else
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / (((fSr*fSr) * (a * a)) + (fLr*fLr)));
				}

				y = a * x;

				x = ptMid.X - x;
				y = ptMid.Y - y;

				RectF rcf(x - fW / 2.0f, y - fH / 2.0f, fW, fH);

				if (rcf.Contains(ptWinpoint))
				{
					return true;
				}
			}

		}
		// 4
		else if (ptEndWin.X > ptStartWin.X && ptEndWin.Y > ptStartWin.Y)
		{
			rcfTmp.X = ptStartWin.X - rcfTmp.Width / 2.0f;
			rcfTmp.Y = ptStartWin.Y;

			ptMid.X = rcfTmp.X + rcfTmp.Width / 2.0f;
			ptMid.Y = rcfTmp.Y + rcfTmp.Height / 2.0f;

			for (float i = 0; i <= 90; i += 0.1f)
			{
				a = tanf(AngleToRadianF(i));

				// ÍÖÔ²½¹µãÔÚXÖá x^2/a^2+y^2/b^2=1£¬(a>b>0)£»
				if (ptMid.X + fSr < rcfTmp.X + rcfTmp.Width)
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / ((fSr*fSr) + ((fLr*fLr) * (a * a))));
				}
				// ÍÖÔ²½¹µãÔÚYÖá y^2/a^2+x^2/b^2=1£¬(a>b>0)£»
				else
				{
					x = sqrtf((fLr*fLr) * (fSr*fSr) / (((fSr*fSr) * (a * a)) + (fLr*fLr)));
				}

				y = a * x;

				x = ptMid.X + x;
				y = ptMid.Y - y;

				RectF rcf(x - fW / 2.0f, y - fH / 2.0f, fW, fH);

				if (rcf.Contains(ptWinpoint))
				{
					return true;
				}
			}

		}
		else
		{
			x = ptStartWin.X < ptEndWin.X ? ptStartWin.X : ptEndWin.X;
			y = ptStartWin.Y < ptEndWin.Y ? ptStartWin.Y : ptEndWin.Y;

			x -= fW / 2.0f;
			y -= fH / 2.0f;

			float w = fabsf(ptStartWin.X - ptEndWin.Y) > 0 ? fabsf(ptStartWin.X - ptEndWin.Y) : fW;
			float h = fabsf(ptStartWin.Y - ptEndWin.Y) > 0 ? fabsf(ptStartWin.Y - ptEndWin.Y) : fH;

			RectF rcf(x, y, w, h);

			if (rcf.Contains(ptWinpoint))
			{
				return true;
			}

		}
	}
	else
	{
		float fx = 0.0f, fy = 0.0f;
		fx = ptStartWin.X - ptEndWin.X;
		fy = ptStartWin.Y - ptEndWin.Y;

		// Ö±Ïß
		if (fx == 0 || fy == 0)
		{
			float x = ptStartWin.X < ptEndWin.X ? ptStartWin.X : ptEndWin.X;
			float y = ptStartWin.Y < ptEndWin.Y ? ptStartWin.Y : ptEndWin.Y;

			x -= fW / 2.0f;
			y -= fH / 2.0f;

			float w = fabsf(ptStartWin.X - ptEndWin.X) > 0 ? fabsf(ptStartWin.X - ptEndWin.X) : fW;
			float h = fabsf(ptStartWin.Y - ptEndWin.Y) > 0 ? fabsf(ptStartWin.Y - ptEndWin.Y) : fH;

			RectF rcf(x, y, w, h);

			if (rcf.Contains(ptWinpoint))
			{
				return true;
			}
		}
		// Ð±Ïß
		else
		{
			float s = fabsf(fy / fx);

			float x = 0.0f, y = 0.0f;
			for (int i = 0; i < fabsf(fx); i += (int)(fW / 2.0f))
			{
				if (fx > 0)
				{
					x = ptStartWin.X - i;
				}
				else
				{
					x = ptStartWin.X + i;
				}

				if (fy > 0)
				{
					y = ptStartWin.Y - (float)i * s;
				}
				else
				{
					y = ptStartWin.Y + (float)i * s;
				}

				RectF rcf(x - fW / 2.0f, y - fH / 2.0f, fW, fH);

				if (rcf.Contains(ptWinpoint))
				{
					return true;
				}
			}
		}
	}

	return false;
}

const HCURSOR CGraphicTrack::Drag(const PointF ptWinPoint)
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

		PointF ptEnd = GetEndPoint();

		if (ptEnd.X < -g_sizeVector.Width / 2.0f
			|| ptEnd.Y > g_sizeVector.Height / 2.0f
			|| ptEnd.X > g_sizeVector.Width / 2.0f
			|| ptEnd.Y < -g_sizeVector.Height / 2.0f)
		{
			m_ptCenter = m_ptLocal;
		}

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
const HCURSOR CGraphicTrack::Drag(const CPoint ptWinPoint)
{
	return Drag(CPointToPointF(ptWinPoint));
}
#endif //_AFX


void CGraphicTrack::DrawImage(Graphics* graphic)
{
	PointF ptStartWin = VecToWin(m_ptCenter);
	PointF ptEndWin = VecToWin(GetEndPoint());
	float fTmpWidth = (g_fWidth / 1.5f) * g_fScale;

	if (m_pPen == nullptr)
	{
		m_pPen = new Pen(Color::Yellow,g_fWidth * g_fScale); 
	}
	else
	{
		m_pPen->SetColor(Color::Yellow);
		m_pPen->SetWidth(g_fWidth * g_fScale);
		m_pPen->SetDashStyle(DashStyle::DashStyleSolid);
	}

	if (m_bArc)
	{
		RectF rcfTmp;
		rcfTmp.Width = fabsf(ptStartWin.X - ptEndWin.X) * 2.0f;
		rcfTmp.Height = fabsf(ptStartWin.Y - ptEndWin.Y) * 2.0f;

		/*   
		*	+x
		* +y   
		*/

		// 1
		if (ptEndWin.X < ptStartWin.X && ptEndWin.Y < ptStartWin.Y)
		{
			rcfTmp.X = ptStartWin.X - rcfTmp.Width / 2.0f;
			rcfTmp.Y = ptEndWin.Y - rcfTmp.Height / 2.0f;

			if (m_bSelect && g_bEdit)
			{
				m_pPen->SetColor(Color::Red);
				graphic->DrawArc(m_pPen, rcfTmp, 180, -90);
			}
			else
			{
				graphic->DrawArc(m_pPen, rcfTmp, 180, -90);
				m_pPen->SetWidth(fTmpWidth);
				m_pPen->SetColor(Color::Black);
				m_pPen->SetDashStyle(DashStyle::DashStyleDash);
				graphic->DrawArc(m_pPen, rcfTmp, 180, -90);
			}

		}
		// 2
		else if (ptEndWin.X > ptStartWin.X && ptEndWin.Y < ptStartWin.Y)
		{
			rcfTmp.X = ptStartWin.X - rcfTmp.Width / 2.0f;
			rcfTmp.Y = ptEndWin.Y - rcfTmp.Height / 2.0f;

			if (m_bSelect && g_bEdit)
			{
				m_pPen->SetColor(Color::Red);
				graphic->DrawArc(m_pPen, rcfTmp, 0, 90);
			}
			else
			{
				graphic->DrawArc(m_pPen, rcfTmp, 0, 90);
				m_pPen->SetWidth(fTmpWidth);
				m_pPen->SetColor(Color::Black);
				m_pPen->SetDashStyle(DashStyle::DashStyleDash);
				graphic->DrawArc(m_pPen, rcfTmp, 0, 90);
			}
		}
		// 3
		else if (ptEndWin.X < ptStartWin.X && ptEndWin.Y > ptStartWin.Y)
		{
			rcfTmp.X = ptEndWin.X;
			rcfTmp.Y = ptStartWin.Y;

			if (m_bSelect && g_bEdit)
			{
				m_pPen->SetColor(Color::Red);
				graphic->DrawArc(m_pPen, rcfTmp, 180, 90);
			}
			else
			{
				graphic->DrawArc(m_pPen, rcfTmp, 180, 90);
				m_pPen->SetWidth(fTmpWidth);
				m_pPen->SetColor(Color::Black);
				m_pPen->SetDashStyle(DashStyle::DashStyleDash);
				graphic->DrawArc(m_pPen, rcfTmp, 180, 90);
			}
		}
		// 4
		else if (ptEndWin.X > ptStartWin.X && ptEndWin.Y > ptStartWin.Y)
		{
			rcfTmp.X = ptStartWin.X - rcfTmp.Width / 2.0f;
			rcfTmp.Y = ptStartWin.Y;

			if (m_bSelect && g_bEdit)
			{
				m_pPen->SetColor(Color::Red);

				graphic->DrawArc(m_pPen, rcfTmp, 0, -90);
			}
			else
			{
				graphic->DrawArc(m_pPen, rcfTmp, 0, -90);
				m_pPen->SetWidth(fTmpWidth);
				m_pPen->SetColor(Color::Black);
				m_pPen->SetDashStyle(DashStyle::DashStyleDash);
				graphic->DrawArc(m_pPen, rcfTmp, 0, -90);
			}
		}
		else
		{
			if (m_bSelect && g_bEdit)
			{
				m_pPen->SetColor(Color::Red);

				graphic->DrawLine(m_pPen, ptStartWin.X, ptStartWin.Y, ptEndWin.X, ptEndWin.Y);
			}
			else
			{
				graphic->DrawLine(m_pPen, ptStartWin.X, ptStartWin.Y, ptEndWin.X, ptEndWin.Y);
				m_pPen->SetWidth(fTmpWidth);
				m_pPen->SetColor(Color::Black);
				m_pPen->SetDashStyle(DashStyle::DashStyleDash);
				graphic->DrawLine(m_pPen, ptStartWin.X, ptStartWin.Y, ptEndWin.X, ptEndWin.Y);
			}
		}
	}
	else
	{
		if (m_bSelect && g_bEdit)
		{
			m_pPen->SetColor(Color::Red);

			graphic->DrawLine(m_pPen, ptStartWin.X, ptStartWin.Y, ptEndWin.X, ptEndWin.Y);
		}
		else
		{
			graphic->DrawLine(m_pPen, ptStartWin.X, ptStartWin.Y, ptEndWin.X, ptEndWin.Y);
			m_pPen->SetWidth(fTmpWidth);
			m_pPen->SetColor(Color::Black);
			m_pPen->SetDashStyle(DashStyle::DashStyleDash);
			graphic->DrawLine(m_pPen, ptStartWin.X, ptStartWin.Y, ptEndWin.X, ptEndWin.Y);
		}
	}

	return;
}

void CGraphicTrack::SetWidth(const float fWidth)
{
	g_fWidth = fWidth;

	CGraph::Refresh();

	return;
}

const float CGraphicTrack::GetWidth() 
{
	return g_fWidth;
}

const double CGraphicTrack::RadianToAngle(const double dbRadian)
{
	double PI = 3.14159265;
	return dbRadian * 180,0 / PI;
}


const double CGraphicTrack::AngleToRadian(const double dbAngle)
{
	double PI = 3.14159265;
	return dbAngle * PI / 180,0;
}

const float CGraphicTrack::RadianToAngleF(const float fRadian)
{
	float PI = 3.14159265f;
	return fRadian * 180.0f / PI;
}


const float CGraphicTrack::AngleToRadianF(const float fAngle)
{
	float PI = 3.14159265f;
	return fAngle * PI / 180.0f;
}

#endif //!_CGRAPHICTRACK_H