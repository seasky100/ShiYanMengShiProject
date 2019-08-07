#include "CGraphicRest.h"

#ifdef _CGRAPHICREST_H
#include "../String/StringEx.h"

bool CGraphicRest::g_bInstall = false;
mutex CGraphicRest::g_mutex;
unsigned int CGraphicRest::g_unWidth = 0;
unsigned int CGraphicRest::g_unHeight = 0;
FontFamily* CGraphicRest::g_pFontFamily = nullptr;
Color CGraphicRest::g_colorFont = Color::Black;
Image* CGraphicRest::g_pImageBK = nullptr;

CGraphicRest::CGraphicRest(const unsigned char byNo, const unsigned short usLocation, const char* strName, const char* strParam)
	:m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(strName)
	, m_strParam(strParam)
{
}

CGraphicRest::CGraphicRest(const unsigned char byNo, const unsigned short usLocation, const wchar_t* wstrName, const wchar_t* wstrParam)
	:m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(StringEx::wstring_to_string(wstrName))
	, m_strParam(StringEx::wstring_to_string(wstrParam).c_str())
{
}

CGraphicRest::CGraphicRest(const unsigned char byNo, const unsigned short usLocation, string strName, string strParam)
	:m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(strName.c_str())
	, m_strParam(strParam.c_str())
{
}

CGraphicRest::CGraphicRest(const unsigned char byNo, const unsigned short usLocation, const wstring wstrName, const wstring wstrParam)
	:m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(StringEx::wstring_to_string(wstrName.c_str()))
	, m_strParam(StringEx::wstring_to_string(wstrParam.c_str()).c_str())
{
}

CGraphicRest::~CGraphicRest()
{

}

bool CGraphicRest::Install()
{
	if (g_bInstall == false)
	{
		g_unWidth = 20;
		g_unHeight = 20;
		g_colorFont = Color::Red;
		g_bInstall = true;
	}

	if (g_pFontFamily == nullptr)
	{
		g_pFontFamily = new FontFamily(L"Î¢ÈíÑÅºÚ");
	}

	if (g_pImageBK == nullptr)
	{
		g_pImageBK = Image::FromFile(L"../Image/Rest.png");
	}

	if (g_pFontFamily == nullptr || g_pImageBK == nullptr)
	{
		return false;
	}

	return true;
}

void CGraphicRest::SetWidth(const unsigned int unWidth)
{
	if (g_unWidth == unWidth)
	{
		return;
	}

	g_unWidth = unWidth;

	CGraph::Refresh();

	return;
}

void CGraphicRest::SetHeight(const unsigned int unHeight)
{
	if (g_unHeight == unHeight)
	{
		return;
	}

	g_unHeight = unHeight;

	CGraph::Refresh();

	return;
}

void CGraphicRest::SetFontType(const char* strFontType)
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

void CGraphicRest::SetFontType(const wchar_t* wstrFontType)
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

void CGraphicRest::SetFontColor(const Color color)
{
	g_colorFont = color;

	CGraph::Refresh();

	return;
}

void CGraphicRest::SetFontColr(const COLORREF color)
{
	g_colorFont.SetFromCOLORREF(color);

	CGraph::Refresh();

	return;
}

void CGraphicRest::Uninstall()
{
	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}

	if (g_pImageBK)
	{
		delete g_pImageBK;
		g_pImageBK = nullptr;
	}
}

const unsigned int CGraphicRest::GetWidth() { return g_unWidth; }

const unsigned int CGraphicRest::GetHeight() { return g_unHeight; }

const float CGraphicRest::GetReallyWidth() { return (float)g_unWidth* g_fScale; }

const float CGraphicRest::GetReallyHeight() { return (float)g_unHeight* g_fScale; }

void CGraphicRest::SetBKImage(const char* strBK)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageBK)
	{
		delete g_pImageBK;
	}

	g_pImageBK = Image::FromFile(StringEx::string_to_wstring(strBK).c_str());

	CGraph::Refresh();

	return;
}

void CGraphicRest::SetBKImage(const wchar_t* wstrBK)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageBK)
	{
		delete g_pImageBK;
	}

	g_pImageBK = Image::FromFile(wstrBK);

	CGraph::Refresh();

	return;
}

const unsigned char CGraphicRest::GetNo() const{ return m_byNo; }

void CGraphicRest::SetLocation(const unsigned short usLocation) { m_usLocation = usLocation; } 

const unsigned short CGraphicRest::GetLocation() const { return m_usLocation; }

void CGraphicRest::SetName(const char* strName) { m_strName = strName; }

void CGraphicRest::SetName(const wchar_t* wstrName) { m_strName = StringEx::wstring_to_string(wstrName); }

const string CGraphicRest::GetName() const { return m_strName; }

const wstring CGraphicRest::GetNameW() const { return StringEx::string_to_wstring(m_strName); }

const bool CGraphicRest::IsInside(const PointF ptWinpoint)
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
const bool CGraphicRest::IsInside(const CPoint ptWinpoint)
{
	return IsInside(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

void CGraphicRest::DrawImage(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	PointF ptWindow = VecToWin(m_ptCenter);
	float fWidth = GetReallyWidth()/*!< Êµ¼Ê¿í */, fHeight = GetReallyHeight() /*!< Êµ¼Ê¸ß */;
	RectF rectBK/*!< ±³¾°ÌùÍ¼ */, rectName /*!<Ãû³ÆÌùÍ¼ */;

	rectBK.X = ptWindow.X - fWidth / 2;
	rectBK.Y = ptWindow.Y - fHeight / 2;
	rectBK.Width = fWidth;
	rectBK.Height = fHeight;

	rectName.X = rectBK.X;
	rectName.Y = rectBK.Y - rectBK.Height;
	rectName.Width = rectBK.Width;
	rectName.Height = rectBK.Height;

	/*!
	 * »æÖÆÑ¡ÖÐ±³¾°
	*/
	m_pBrush->SetColor(Color::Red);

	if (m_bSelect && g_bEdit)
	{
		graphic->FillRectangle(m_pBrush, rectBK);
	}

	/*!
	 * »æÖÆµ×È¦
	*/
	graphic->DrawImage(g_pImageBK, rectBK);

	/*!
	 * »æÖÆ±àºÅ
	*/
	m_gsFormat.SetAlignment(StringAlignmentCenter);
	m_gsFormat.SetLineAlignment(StringAlignmentFar);

	Gdiplus::Font* pFont = new Font(g_pFontFamily, (g_unWidth / 10) * g_fScale, FontStyleRegular, UnitPoint);

	wstring wstrText = L"";
	if (m_strName != "")
	{
		wstrText = StringEx::string_to_wstring(m_strName);
	}
	else
	{
		wstrText = StringEx::string_format(L"´ý»úÎ»£º%d", m_byNo);
	}

	m_pBrush->SetColor(g_colorFont);
	graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, rectName, &m_gsFormat, m_pBrush);

	delete pFont;

	g_mutex.unlock();

	return;
}

void CGraphicRest::SetParam(const char* strParam)
{
	m_strParam = strParam;

	return;
}

void CGraphicRest::SetParam(const wchar_t* strParam)
{
	m_strParam = StringEx::wstring_to_string(strParam);

	return;
}

const string CGraphicRest::GetParam() const
{
	return m_strParam;
}

const wstring CGraphicRest::GetParamW() const
{
	return StringEx::string_to_wstring(m_strParam.c_str());
}

#endif //!_CGRAPHICREST_H