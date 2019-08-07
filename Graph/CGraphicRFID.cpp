/*!
 * @file CGraphicMark.cpp
 * @brief CGraphicMark类执行文件
 * @author FanKaiyu
 * @date 2018-10-22
 * @version 2.0
*/

#include "CGraphicRFID.h"

#ifdef _CGRAPHICRFID_H
#include "../String/StringEx.h"

bool CGraphicRFID::g_bInstall = false;
mutex CGraphicRFID::g_mutex;
unsigned int CGraphicRFID::g_unWidth = 10;	/*!< 宽 */
unsigned int CGraphicRFID::g_unHeight = 10;	/*!< 高 */
FontFamily* CGraphicRFID::g_pFontFamily =nullptr;
Color CGraphicRFID::g_colorFont= Color::Black;

CGraphicRFID::CGraphicRFID(unsigned short usNo, PointF ptCenter)
	:m_usNo(usNo)
{
	SetCenterPoint(ptCenter);
}

CGraphicRFID::~CGraphicRFID()
{
}

bool CGraphicRFID::Install()
{
	if (g_bInstall == false)
	{
		g_unWidth = 10;
		g_unHeight = 10;
		g_colorFont = Color::Black;
		g_bInstall = true;
	}
	
	if (g_pFontFamily == nullptr)
	{
		g_pFontFamily = new FontFamily(L"微软雅黑");
	}

	if (g_pFontFamily == nullptr)
	{
		return false;
	}

	return true;
}

void CGraphicRFID::SetWidth(const unsigned int unWidth)
{
	if (g_unWidth == unWidth)
	{
		return;
	}

	g_unWidth = unWidth;

	CGraph::Refresh();

	return;
}

void CGraphicRFID::SetHeight(const unsigned int unHeight)
{
	if (g_unHeight == unHeight)
	{
		return;
	}

	g_unHeight = unHeight;

	CGraph::Refresh();

	return;
}

void CGraphicRFID::SetFontType(const char* strFontType)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}

	g_pFontFamily = new FontFamily(StringEx::string_to_wstring(strFontType).c_str());

	CGraph::Refresh();

	return;
}

void CGraphicRFID::SetFontType(const wchar_t* wstrFontType)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}

	g_pFontFamily = new FontFamily(wstrFontType);
	
	CGraph::Refresh();

	return;
}

void CGraphicRFID::SetFontColor(const Color color)
{
	g_colorFont = color;

	CGraph::Refresh();

	return;
}

void CGraphicRFID::SetFontColr(const COLORREF color)
{
	g_colorFont.SetFromCOLORREF(color);

	CGraph::Refresh();

	return;
}

void CGraphicRFID::Uninstall()
{
	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}
}

const unsigned int CGraphicRFID::GetWidth() { return g_unWidth; }

const unsigned int CGraphicRFID::GetHeight() { return g_unHeight; }

const float CGraphicRFID::GetReallyWidth() { return (float)g_unWidth* g_fScale; }

const float CGraphicRFID::GetReallyHeight() { return (float)g_unHeight* g_fScale; }

const unsigned short CGraphicRFID::GetNo() const
{
	return m_usNo;
}

const bool CGraphicRFID::IsInside(const PointF ptWinpoint)
{
	PointF ptWindow = VecToWin(m_ptCenter);
	REAL fWidth = GetReallyWidth();
	REAL fHeight = GetReallyHeight();
	RectF rect;
	rect.X = ptWindow.X - fWidth / 2.0f;
	rect.Y = ptWindow.Y - fHeight / 2.0f;
	rect.Width = fWidth;
	rect.Height = fHeight;

	return rect.Contains(ptWinpoint);
}

#ifdef _AFX
const bool CGraphicRFID::IsInside(const CPoint ptWinpoint)
{
	return IsInside(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

void CGraphicRFID::DrawImage(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	PointF ptWindow = VecToWin(m_ptCenter);
	float fWidth = GetReallyWidth()/*!< 实际宽 */, fHeight = GetReallyHeight() /*!< 实际高 */;
	RectF rectBK/*!< 背景贴图 */;

	rectBK.X = ptWindow.X - fWidth / 2;
	rectBK.Y = ptWindow.Y - fHeight / 2;
	rectBK.Width = fWidth;
	rectBK.Height = fHeight;

	/*!
	 * 绘制选中背景
	*/
	m_pBrush->SetColor(Color::Red);

	if (m_bSelect && g_bEdit)
	{
		graphic->FillRectangle(m_pBrush, rectBK);
	}

	/*!
	 * 绘制底圈
	*/

	// 设置为白色
	m_pBrush->SetColor(Color::White);

	graphic->FillEllipse(m_pBrush, rectBK);

	/*!
	 * 绘制外环
	*/
	m_pPen->SetColor(Color::Black);
	m_pPen->SetWidth(2.0f);

	graphic->DrawEllipse(m_pPen, rectBK);

	/*!
	 * 绘制编号
	*/
	Gdiplus::Font* pFont = new Font(g_pFontFamily, (g_unWidth / 4) * g_fScale, FontStyleRegular, UnitPoint);

	m_gsFormat.SetAlignment(StringAlignmentCenter);
	m_gsFormat.SetLineAlignment(StringAlignmentCenter);

	wstring wstrNo = StringEx::string_format(L"%d", m_usNo);

	m_pBrush->SetColor(g_colorFont);
	graphic->DrawString(wstrNo.c_str(), wstrNo.length(), pFont, rectBK, &m_gsFormat, m_pBrush);

	delete pFont;

	g_mutex.unlock();

	return;
}

#endif //!_CGRAPHICRFID_H