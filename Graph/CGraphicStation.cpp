/*!
 * @file CGraphicStation.cpp
 * @brief CGraphicStation类执行文件
 * @author FanKaiyu
 * @date 2018-10-22
 * @version 2.0
*/

#include "CGraphicStation.h"

#ifdef _CGRAPHICSTATION_H
#include "../String/StringEx.h"

bool CGraphicStation::g_bInstall = false;
mutex CGraphicStation::g_mutex;
unsigned int CGraphicStation::g_unWidth = 0;
unsigned int CGraphicStation::g_unHeight = 0;
FontFamily* CGraphicStation::g_pFontFamily = nullptr;
Color CGraphicStation::g_colorFont = Color::Black;
Image* CGraphicStation::g_pImageBK = nullptr;


CGraphicStation::CGraphicStation(const unsigned char byNo, unsigned short usLocation, const char* strName,const char* strParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(strName)
	, m_strParam(strParam)
{
}

CGraphicStation::CGraphicStation(const unsigned char byNo, unsigned short usLocation, const wchar_t* wstrName,const wchar_t* wstrParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(StringEx::wstring_to_string(wstrName).c_str())
	, m_strParam(StringEx::wstring_to_string(wstrParam).c_str())
{
}

CGraphicStation::CGraphicStation(const unsigned char byNo, unsigned short usLocation, string strName,string strParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(strName.c_str())
	, m_strParam(strParam.c_str())
{
}

CGraphicStation::CGraphicStation(const unsigned char byNo, unsigned short usLocation, const wstring wstrName,const wstring wstrParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_strName(StringEx::wstring_to_string(wstrName.c_str()).c_str())
	, m_strParam(StringEx::wstring_to_string(wstrParam.c_str()).c_str())
{
}

CGraphicStation::~CGraphicStation()
{
}

bool CGraphicStation::Install()
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
		g_pFontFamily = new FontFamily(L"微软雅黑");
	}
	if (g_pImageBK == nullptr)
	{
		g_pImageBK = Image::FromFile(L"../Image/Station.png");
	}

	if (g_pFontFamily == nullptr || g_pImageBK == nullptr)
	{
		return false;
	}

	return true;
}

void CGraphicStation::SetWidth(const unsigned int unWidth)
{
	if (g_unWidth == unWidth)
	{
		return;
	}

	g_unWidth = unWidth;

	CGraph::Refresh();

	return;
}

void CGraphicStation::SetHeight(const unsigned int unHeight)
{
	if (g_unHeight == unHeight)
	{
		return;
	}

	g_unHeight = unHeight;

	CGraph::Refresh();

	return;
}

void CGraphicStation::SetFontType(const char* strFontType)
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

void CGraphicStation::SetFontType(const wchar_t* wstrFontType)
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

void CGraphicStation::SetFontColor(const Color color)
{
	g_colorFont = color;

	CGraph::Refresh();

	return;
}

void CGraphicStation::SetFontColr(const COLORREF color)
{
	g_colorFont.SetFromCOLORREF(color);

	CGraph::Refresh();

	return;
}

void CGraphicStation::Uninstall()
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

const unsigned int CGraphicStation::GetWidth() { return g_unWidth; }

const unsigned int CGraphicStation::GetHeight() { return g_unHeight; }

const float CGraphicStation::GetReallyWidth() { return (float)g_unWidth* g_fScale; }

const float CGraphicStation::GetReallyHeight() { return (float)g_unHeight* g_fScale; }

void CGraphicStation::SetBKImage(const char* strBK)
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

void CGraphicStation::SetBKImage(const wchar_t* wstrBK)
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

const unsigned char CGraphicStation::GetNo() const
{
	return m_byNo;
}

void CGraphicStation::SetName(const char* strName)
{
	m_strName = strName;

	return;
}

void CGraphicStation::SetName(const wchar_t* strName)
{
	m_strName = StringEx::wstring_to_string(strName);

	return;
}

const string CGraphicStation::GetName() const
{
	return m_strName;
}

const wstring CGraphicStation::GetNameW() const
{
	return StringEx::string_to_wstring(m_strName.c_str());
}

void CGraphicStation::SetLocation(const unsigned short usLocation)
{
	m_usLocation = usLocation;

	return;
}

const unsigned short CGraphicStation::GetLocation() const
{
	return m_usLocation;
}

void CGraphicStation::SetParam(const char* strParam)
{
	m_strParam = strParam;

	return;
}

void CGraphicStation::SetParam(const wchar_t* strParam)
{
	m_strParam = StringEx::wstring_to_string(strParam);

	return;
}

const string CGraphicStation::GetParam() const
{
	return m_strParam;
}

const wstring CGraphicStation::GetParamW() const
{
	return StringEx::string_to_wstring(m_strParam.c_str());
}

const bool CGraphicStation::IsInside(const PointF ptWinpoint)
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
const bool CGraphicStation::IsInside(const CPoint ptWinpoint)
{
	return IsInside(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

void CGraphicStation::DrawImage(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	PointF ptWindow = VecToWin(m_ptCenter);
	float fWidth = GetReallyWidth()/*!< 实际宽 */, fHeight = GetReallyHeight() /*!< 实际高 */;
	RectF rectBK/*!< 背景贴图 */, rectName /*!<名称贴图 */;

	rectBK.X = ptWindow.X - fWidth / 2;
	rectBK.Y = ptWindow.Y - fHeight / 2;
	rectBK.Width = fWidth;
	rectBK.Height = fHeight;

	rectName.X = rectBK.X;
	rectName.Y = rectBK.Y - rectBK.Height;
	rectName.Width = rectBK.Width;
	rectName.Height = rectBK.Height;

	/*!
	 * 绘制选中背景
	*/
	m_pBrush->SetColor(Color::Red);

	if (m_bSelect && g_bEdit)
	{
		graphic->FillRectangle(m_pBrush, rectBK);
	}

	/*!
	 * 绘制底图
	*/
	graphic->DrawImage(g_pImageBK, rectBK);

	/*!
	 * 绘制编号
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
		wstrText = StringEx::string_format(L"工位：%d", m_byNo);
	}

	m_pBrush->SetColor(g_colorFont);
	graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, rectName, &m_gsFormat, m_pBrush);

	delete pFont;

	g_mutex.unlock();

	return;
}

#endif //!_CGRAPHICSTATION