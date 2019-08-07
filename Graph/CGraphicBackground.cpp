/*!
 * @file CGraphicBackground.cpp
 * @brief CGraphicBackground类执行文件
 * @author FanKaiyu
 * @date 2018-10-15
 * @version 2.0
*/

#include "CGraphicBackground.h"

#ifdef _CGRAPHICBACKGROUND_H
#include "../String/StringEx.h"

bool CGraphicBackground::g_bInstall = false;
mutex CGraphicBackground::g_mutex;
Color CGraphicBackground::g_gcrFont = Color::Black;
FontFamily* CGraphicBackground::g_pFontFamily = nullptr;

CGraphicBackground::CGraphicBackground(Color colorLine, float fWidth)
{
	m_fWidth = fWidth;
	
	m_pImage = new Image(L"../Image/Background.jpg");
	m_pPen = new Pen(colorLine, m_fWidth);
}

CGraphicBackground::CGraphicBackground(const COLORREF colorLine, const float fWidth)
{
	m_fWidth = fWidth;

	m_pImage = new Image(L"../Image/Background.jpg");
	Color color;
	color.SetFromCOLORREF(colorLine);
	m_pPen = new Pen(color, m_fWidth);
}

CGraphicBackground::~CGraphicBackground()
{
	if (m_pImage)
	{
		delete m_pImage;
		m_pImage = nullptr;
	}
}

bool CGraphicBackground::Install()
{
	if (g_bInstall == false)
	{
		g_gcrFont = Color::Red;
		g_bInstall = true;
	}

	if (g_pFontFamily == nullptr)
	{
		g_pFontFamily = new FontFamily(L"微软雅黑");
	}
	g_gcrFont = Color::Red;

	if (g_pFontFamily == nullptr)
	{
		return false;
	}

	return true;
}

void CGraphicBackground::SetFontType(const char* strFontType)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}

	g_pFontFamily = new FontFamily(StringEx::string_to_wstring(strFontType).c_str());

	return;
}

void CGraphicBackground::SetFontType(const wchar_t* wstrFontType)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}

	g_pFontFamily = new FontFamily(wstrFontType);
	return;
}

void CGraphicBackground::Uninstall()
{
	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}
}

void CGraphicBackground::SetLine(const Color colorLine, const float fWidth)
{
	lock_guard<mutex> lock(g_mutex);

	m_fWidth = fWidth;

	if (m_pPen)
	{
		delete m_pPen;
	}

	m_pPen = new Pen(colorLine, m_fWidth);

	return;
}

void CGraphicBackground::SetLine(const COLORREF colorLine, const float fWidth)
{
	lock_guard<mutex> lock(g_mutex);

	m_fWidth = fWidth;

	if (m_pPen)
	{
		delete m_pPen;
	}

	Color color;
	color.SetFromCOLORREF(colorLine);

	m_pPen = new Pen(color, m_fWidth);

	return;
}

void CGraphicBackground::GetLine(Color & colorLine, float & fWidth)
{
	if (m_pPen)
	{
		m_pPen->GetColor(&colorLine);
	}
	else
	{
		colorLine = Color::White;
	}
	
	fWidth = m_fWidth;

	return;
}

void CGraphicBackground::GetLine(COLORREF& colorLine, float& fWidth)
{
	if (m_pPen)
	{
		Color color;
		m_pPen->GetColor(&color);

		colorLine = color.ToCOLORREF();
	}
	else
	{
		colorLine = RGB(255,255,255);
	}
		
	fWidth = m_fWidth;

	return;
}

void CGraphicBackground::SetImage(const char* strPath)
{
	lock_guard<mutex> lock(g_mutex);

	if (m_pImage)
	{
		delete m_pImage;
	}

	m_pImage = Image::FromFile(StringEx::string_to_wstring(strPath).c_str());

	return;
}

void CGraphicBackground::SetImage(const wchar_t* strPath)
{
	lock_guard<mutex> lock(g_mutex);

	if (m_pImage)
	{
		delete m_pImage;
	}

	m_pImage = Image::FromFile(strPath);

	return;
}

void CGraphicBackground::Scale(bool bShrink)
{
	if (bShrink)
	{
		if (g_fScale >= MIN_SCALE)
		{
			g_fScale -= 0.1f;

			Refresh();
		}
	}
	else
	{
		if (g_fScale <= MAX_SCALE)
		{
			g_fScale += 0.1f;

			Refresh();
		}
	}

	if (g_fScale == 0)
	{
		if (bShrink)
		{
			g_fScale -= 0.1f;

			Refresh();
		}
		else
		{
			g_fScale += 0.1f;

			Refresh();
		}
	}

	return;
}

void CGraphicBackground::Reset()
{
	CGraph::SetScale(DEFALUT_SCALE);
	CGraph::SetOffset(PointF(0, 0));

	return;
}

const HCURSOR CGraphicBackground::Drag(const PointF ptWinPoint)
{
	HCURSOR hCur = nullptr;

	m_ptLastpoint = ptWinPoint;

	if (m_bMove)
	{
		g_ptOffset = m_ptLastpoint - m_ptPrepoint + m_ptLocal;
	
		PointF ptLeftTop = WinToVec(PointF(g_rcClient.X, g_rcClient.Y));
		PointF ptRightBottom = WinToVec(PointF(g_rcClient.GetRight(), g_rcClient.GetBottom()));

		if (ptLeftTop.X < -g_sizeVector.Width / 2.0f
			|| ptLeftTop.Y > g_sizeVector.Height / 2.0f
			|| ptRightBottom.X > g_sizeVector.Width / 2.0f
			|| ptRightBottom.Y < -g_sizeVector.Height / 2.0f)
		{
			g_ptOffset = m_ptLocal;
		}

		m_ptCenter = g_ptOffset;

		hCur = LoadCursor(NULL, IDC_HAND);
	}
	else
	{
		m_ptLocal = g_ptOffset;
	}

	return hCur;
}

#ifdef _AFX
const HCURSOR CGraphicBackground::Drag(const CPoint ptWinPoint)
{
	return Drag(CPointToPointF(ptWinPoint));
}
#endif //_AFX

void CGraphicBackground::DrawImage(Graphics* graphic)
{
	DrawBackground(graphic);

	if (g_bEdit)
	{
		DrawLine(graphic);

		//DrawPoint(graphic);
	}

	return;
}

void CGraphicBackground::DrawBackground(HDC hdc)
{
	InitGraphic(hdc);

	DrawBackground(m_pGraphics);

	return;
}

void CGraphicBackground::DrawBackground(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	graphic->DrawImage(m_pImage, g_rcClient);

	g_mutex.unlock();

	return;
}

void CGraphicBackground::DrawLine(HDC hdc)
{
	InitGraphic(hdc);

	DrawLine(m_pGraphics);

	return;
}

void CGraphicBackground::DrawLine(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	m_pBrush->SetColor(Color::Red);

	Rect rectBK;

	// 设置实线
	m_pPen->SetDashStyle(DashStyleSolid);
	// 设置宽度
	m_pPen->SetWidth(m_fWidth);

	PointF ptStart = PointF(0, 0);	/*!< 起点坐标 GDI+ */
	PointF ptEnd = PointF(0, 0);	/*!< 终点坐标 GDI+ */

	/*!
	 * 绘制实线
	*/

	// 从左向右绘制实线(竖线)
	for (int x = -g_sizeVector.Width / 2; x <= g_sizeVector.Width / 2; x += g_unDistance * 10)
	{
		ptStart = VecToWin(PointF(x, g_sizeVector.Height / 2));
		ptEnd = VecToWin(PointF(x, -g_sizeVector.Height / 2));

		ptStart.Y = ptStart.Y < g_rcClient.Y ? g_rcClient.Y : ptStart.Y;
		ptEnd.Y = ptEnd.Y > g_rcClient.GetBottom() ? g_rcClient.GetBottom() - 1 : ptEnd.Y;

		if (g_rcClient.Contains(ptStart) && g_rcClient.Contains(ptEnd))
		{
			graphic->DrawLine(m_pPen, ptStart, ptEnd);
		}
	}

	// 由上至下绘制实线(横线)
	for (int y = g_sizeVector.Height / 2; y >= -g_sizeVector.Height / 2; y -= g_unDistance * 10)
	{
		ptStart = VecToWin(PointF(-g_sizeVector.Width / 2, y));
		ptEnd = VecToWin(PointF(g_sizeVector.Width / 2, y));

		ptStart.X = ptStart.X < g_rcClient.X ? g_rcClient.X : ptStart.X;
		ptEnd.X = ptEnd.X > g_rcClient.GetRight() ? g_rcClient.GetRight() - 1 : ptEnd.X;

		if (g_rcClient.Contains(ptStart) && g_rcClient.Contains(ptEnd))
		{
			graphic->DrawLine(m_pPen, ptStart, ptEnd);
		}
	}

	/*!
	 * 绘制虚线
	*/
	if (g_fScale < 1)
	{
		g_mutex.unlock();

		return;
	}

	// 设置实线
	m_pPen->SetDashStyle(DashStyleDot);
	// 设置宽度
	m_pPen->SetWidth(1.0f);

	int nCount = 0;

	// 从左向右绘制虚线(竖线)
	for (int x = -g_sizeVector.Width / 2; x <= g_sizeVector.Width / 2; x += g_unDistance, ++nCount)
	{
		if (nCount % 10 == 0)
		{
			continue;
		}

		ptStart = VecToWin(PointF(x, g_sizeVector.Height / 2));
		ptEnd = VecToWin(PointF(x, -g_sizeVector.Height / 2));

		ptStart.Y = ptStart.Y < g_rcClient.Y ? g_rcClient.Y : ptStart.Y;
		ptEnd.Y = ptEnd.Y > g_rcClient.GetBottom() ? g_rcClient.GetBottom() - 1 : ptEnd.Y;

		if (g_rcClient.Contains(ptStart) && g_rcClient.Contains(ptEnd))
		{
			graphic->DrawLine(m_pPen, ptStart, ptEnd);
		}
	}

	nCount = 0;

	// 由上至下绘制虚线(横线)
	for (int y = g_sizeVector.Height / 2; y >= -g_sizeVector.Height / 2; y -= g_unDistance, ++nCount)
	{
		if (nCount % 10 == 0)
		{
			continue;
		}
		ptStart = VecToWin(PointF(-g_sizeVector.Width / 2, y));
		ptEnd = VecToWin(PointF(g_sizeVector.Width / 2, y));

		ptStart.X = ptStart.X < g_rcClient.X ? g_rcClient.X : ptStart.X;
		ptEnd.X = ptEnd.X > g_rcClient.GetRight() ? g_rcClient.GetRight() - 1 : ptEnd.X;

		if (g_rcClient.Contains(ptStart) && g_rcClient.Contains(ptEnd))
		{
			graphic->DrawLine(m_pPen, ptStart, ptEnd);
		}
	}

	g_mutex.unlock();
	return;
}

void CGraphicBackground::DrawPoint(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	PointF ptVector = PointF(0, 0);	/*!< 矢量坐标 */
	wstring wstrText = L"";

	Gdiplus::Font* pFont = nullptr;

	pFont = new Font(g_pFontFamily, 10.2f, FontStyle::FontStyleRegular, Gdiplus::Unit::UnitPoint);

	m_gsFormat.SetAlignment(StringAlignmentNear);
	m_gsFormat.SetLineAlignment(StringAlignmentNear);

	m_pBrush->SetColor(g_gcrFont);

	graphic->SetTextRenderingHint(TextRenderingHint::TextRenderingHintSystemDefault);

	/*!
	 * 绘制实线坐标
	*/
	float fWidth = 50.0f;
	float fHeight = 50.0f;
	RectF rect;
	PointF ptLeft;
	rect.Width = fWidth;
	rect.Height = fHeight;

	// 绘制X坐标(实线)
	for (int x = g_rcClient.X; x <= g_rcClient.GetRight(); ++x)
	{
		ptLeft.Y = g_rcClient.Y;
		ptLeft.X = x + m_fWidth;
		
		ptVector = WinToVec(PointF(x, g_rcClient.Y));

		if (abs((INT)ptVector.X) % (g_unDistance * 10) == 0 && abs(ptVector.X) < g_sizeVector.Width / 2.0f)
		{
			wstrText = StringEx::string_format(L"%d", (INT)ptVector.X);

			graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, ptLeft, &m_gsFormat, m_pBrush);
		}
	}

	// 绘制Y坐标(实线)
	for (int y = g_rcClient.X; y <= g_rcClient.GetBottom(); ++y)
	{
		ptLeft.Y = y + m_fWidth;
		ptLeft.X = g_rcClient.X;

		ptVector = WinToVec(PointF(g_rcClient.X, y));

		if (abs((INT)ptVector.Y) % (g_unDistance * 10) == 0 && abs(ptVector.Y) < g_sizeVector.Height / 2.0f)
		{
			wstrText = StringEx::string_format(L"%d", (INT)ptVector.Y);

			graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, ptLeft, &m_gsFormat, m_pBrush);
		}
	}

	/*!
	 * 绘制虚线坐标
	*/

	if (g_fScale < 1)
	{
		delete pFont;

		g_mutex.unlock();
		return;
	}

	// 绘制X坐标(虚线)
	for (int x = g_rcClient.X; x <= g_rcClient.GetRight(); ++x)
	{
		ptVector = WinToVec(PointF(x, g_rcClient.Y));

		// 跳过实线
		if (abs((INT)ptVector.X) % (g_unDistance * 10) == 0)
		{
			continue;
		}

		ptLeft.Y = g_rcClient.Y;
		ptLeft.X = x + m_fWidth;

		if (abs((INT)ptVector.X) % g_unDistance == 0 && abs(ptVector.X) < g_sizeVector.Width / 2.0f)
		{
			wstrText = StringEx::string_format(L"%d", (INT)ptVector.X);

			graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, ptLeft, &m_gsFormat, m_pBrush);
		}
	}

	// 绘制Y坐标(虚线)
	for (int y = g_rcClient.Y; y <= g_rcClient.GetBottom(); ++y)
	{
		ptVector = WinToVec(PointF(g_rcClient.X, y));

		// 跳过实线
		if (abs((INT)ptVector.Y) % (g_unDistance * 10) == 0 && abs(ptVector.Y) < g_sizeVector.Height / 2)
		{
			continue;
		}

		ptLeft.Y = y + m_fWidth;
		ptLeft.X = g_rcClient.X;

		if (abs((INT)ptVector.Y) % g_unDistance == 0)
		{
			wstrText = StringEx::string_format(L"%d", (INT)ptVector.Y);

			graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, ptLeft, &m_gsFormat, m_pBrush);
		}
	}

	delete pFont;

	g_mutex.unlock();
	return;
}

void CGraphicBackground::DrawPoint(HDC hdc)
{
	InitGraphic(hdc);

	DrawPoint(m_pGraphics);

	return;
}

#endif // !_CGRAPHICBACKGROUND_H