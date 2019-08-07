//#include "pch.h"
#include "CGraphicCaller.h"

#ifdef _CGRAPHICCALLER_H
#include "../String/StringEx.h"

bool CGraphicCaller::g_bInstall = false;
mutex CGraphicCaller::g_mutex;
unsigned int CGraphicCaller::g_unWidth = 0;
unsigned int CGraphicCaller::g_unHeight = 0;
FontFamily* CGraphicCaller::g_pFontFamily = nullptr;
Color CGraphicCaller::g_colorFont = Color::Black;
Image* CGraphicCaller::g_pImageCaller = nullptr;
Image* CGraphicCaller::g_pImageCallSignal = nullptr;
Image* CGraphicCaller::g_pImageCancelCallSignal = nullptr;
Image* CGraphicCaller::g_pImagePassSignal = nullptr;
Image* CGraphicCaller::g_pImageCancelPassSignal = nullptr;
Image* CGraphicCaller::g_pImageNGSignal = nullptr;
Image* CGraphicCaller::g_pImageCancelNGSignal = nullptr;
Image* CGraphicCaller::g_pImageScreamSignal = nullptr;
Image* CGraphicCaller::g_pImagePackOutbound = nullptr;

CGraphicCaller::CGraphicCaller(const unsigned char byNo, const char* strParam, const unsigned char byStation)
	: m_byNo(byNo)
	, m_byStation(byStation)
	, m_byCallSignal(0)
	, m_byCancelCallSignal(0)
	, m_byPassSignal(0)
	, m_byCancelPassSignal(0)
	, m_byNGSignal(0)
	, m_byCancelNGSignal(0)
	, m_byScreamSignal(0)
	, m_strParam(strParam)
	, m_bPackOutbound(false)
{
}

CGraphicCaller::CGraphicCaller(const unsigned char byNo, const wchar_t* wstrParam, const unsigned char byStation)
	: m_byNo(byNo)
	, m_byStation(byStation)
	, m_byCallSignal(0)
	, m_byCancelCallSignal(0)
	, m_byPassSignal(0)
	, m_byCancelPassSignal(0)
	, m_byNGSignal(0)
	, m_byCancelNGSignal(0)
	, m_byScreamSignal(0)
	, m_strParam(StringEx::wstring_to_string(wstrParam).c_str())
	, m_bPackOutbound(false)
{
}

CGraphicCaller::CGraphicCaller(const unsigned char byNo, const unsigned char byStation, string strParam)
	: m_byNo(byNo)
	, m_byStation(byStation)
	, m_byCallSignal(0)
	, m_byCancelCallSignal(0)
	, m_byPassSignal(0)
	, m_byCancelPassSignal(0)
	, m_byNGSignal(0)
	, m_byCancelNGSignal(0)
	, m_byScreamSignal(0)
	, m_strParam(strParam.c_str())
	, m_bPackOutbound(false)
{
}

CGraphicCaller::CGraphicCaller(const unsigned char byNo, const wstring wstrParam, const unsigned char byStation)
	: m_byNo(byNo)
	, m_byStation(byStation)
	, m_byCallSignal(0)
	, m_byCancelCallSignal(0)
	, m_byPassSignal(0)
	, m_byCancelPassSignal(0)
	, m_byNGSignal(0)
	, m_byCancelNGSignal(0)
	, m_byScreamSignal(0)
	, m_strParam(StringEx::wstring_to_string(wstrParam.c_str()).c_str())
	, m_bPackOutbound(false)
{
}

CGraphicCaller::~CGraphicCaller()
{
}

bool CGraphicCaller::Install()
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

	if (g_pImageCaller == nullptr)
	{
		g_pImageCaller = Image::FromFile(L"../Image/Caller.png");
	}
	if (g_pImageCallSignal == nullptr)
	{
		g_pImageCallSignal = Image::FromFile(L"../Image/CallSignal.png");
	}
	if (g_pImageCancelCallSignal == nullptr)
	{
		g_pImageCancelCallSignal = Image::FromFile(L"../Image/CancelCallSignal.png");
	}
	if (g_pImagePassSignal == nullptr)
	{
		g_pImagePassSignal = Image::FromFile(L"../Image/PassSignal.png");
	}
	if (g_pImageCancelPassSignal == nullptr)
	{
		g_pImageCancelPassSignal = Image::FromFile(L"../Image/CancelPassSignal.png");
	}
	if (g_pImageNGSignal == nullptr)
	{
		g_pImageNGSignal = Image::FromFile(L"../Image/NGSignal.png");
	}
	if (g_pImageCancelNGSignal == nullptr)
	{
		g_pImageCancelNGSignal = Image::FromFile(L"../Image/CancelNGSignal.png");
	}
	if (g_pImageScreamSignal == nullptr)
	{
		g_pImageScreamSignal = Image::FromFile(L"../Image/ScreamSignal.png");
	}
	if (g_pImagePackOutbound == nullptr)
	{
		g_pImagePackOutbound = Image::FromFile(L"../Image/PackOutbound.png");
	}

	if (g_pFontFamily == nullptr
		|| g_pImageCaller == nullptr
		|| g_pImageCallSignal == nullptr
		|| g_pImageCancelCallSignal == nullptr
		|| g_pImagePassSignal == nullptr
		|| g_pImageCancelPassSignal == nullptr
		|| g_pImageNGSignal == nullptr
		|| g_pImageCancelNGSignal == nullptr
		|| g_pImageScreamSignal == nullptr
		|| g_pImagePackOutbound == nullptr)
	{
		return false;
	}

	return true;
}

void CGraphicCaller::SetWidth(const unsigned int unWidth)
{
	if (g_unWidth == unWidth)
	{
		return;
	}

	g_unWidth = unWidth;

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetHeight(const unsigned int unHeight)
{
	if (g_unHeight == unHeight)
	{
		return;
	}

	g_unHeight = unHeight;

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetFontType(const char* strFontType)
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

void CGraphicCaller::SetFontType(const wchar_t* wstrFontType)
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

void CGraphicCaller::SetFontColor(const Color color)
{
	g_colorFont = color;

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetFontColr(const COLORREF color)
{
	g_colorFont.SetFromCOLORREF(color);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::Uninstall()
{
	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}

	if (g_pImageCaller)
	{
		delete g_pImageCaller;
		g_pImageCaller = nullptr;
	}

	if (g_pImageCallSignal)
	{
		delete g_pImageCallSignal;
		g_pImageCallSignal = nullptr;
	}

	if (g_pImageCancelCallSignal)
	{
		delete g_pImageCancelCallSignal;
		g_pImageCancelCallSignal = nullptr;
	}

	if (g_pImagePassSignal)
	{
		delete g_pImagePassSignal;
		g_pImagePassSignal = nullptr;
	}

	if (g_pImageCancelPassSignal)
	{
		delete g_pImageCancelPassSignal;
		g_pImageCancelPassSignal = nullptr;
	}

	if (g_pImageNGSignal)
	{
		delete g_pImageNGSignal;
		g_pImageNGSignal = nullptr;
	}

	if (g_pImageCancelNGSignal)
	{
		delete g_pImageCancelNGSignal;
		g_pImageCancelNGSignal = nullptr;
	}

	if (g_pImageScreamSignal)
	{
		delete g_pImageScreamSignal;
		g_pImageScreamSignal = nullptr;
	}
}

const unsigned int CGraphicCaller::GetWidth() { return g_unWidth; }

const unsigned int CGraphicCaller::GetHeight() { return g_unHeight; }

const float CGraphicCaller::GetReallyWidth() { return (float)g_unWidth* g_fScale; }

const float CGraphicCaller::GetReallyHeight() { return (float)g_unHeight* g_fScale; }

void CGraphicCaller::SetCallerImage(const char* strImagePath)
{	
	return SetCallerImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetCallerImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageCaller)
	{
		delete g_pImageCaller;
	}

	g_pImageCaller = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetCallSignalImage(const char* strImagePath)
{
	return SetCallSignalImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetCallSignalImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageCallSignal)
	{
		delete g_pImageCallSignal;
	}

	g_pImageCallSignal = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetCancelCallImage(const char* strImagePath)
{
	return SetCancelCallImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetCancelCallImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageCancelCallSignal)
	{
		delete g_pImageCancelCallSignal;
	}

	g_pImageCancelCallSignal = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetPassSignalImage(const char* strImagePath)
{
	return SetPassSignalImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetPassSignalImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImagePassSignal)
	{
		delete g_pImagePassSignal;
	}

	g_pImagePassSignal = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetCancelPassSignalImage(const char* strImagePath)
{
	return SetCancelPassSignalImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetCancelPassSignalImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageCancelPassSignal)
	{
		delete g_pImageCancelPassSignal;
	}

	g_pImageCancelPassSignal = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetNGSignalImage(const char* strImagePath)
{
	return SetNGSignalImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetNGSignalImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageNGSignal)
	{
		delete g_pImageNGSignal;
	}

	g_pImageNGSignal = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetCancelNGSignalImage(const char* strImagePath)
{
	return SetCancelNGSignalImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetCancelNGSignalImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageCancelNGSignal)
	{
		delete g_pImageCancelNGSignal;
	}

	g_pImageCancelNGSignal = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetScreamSignalImage(const char* strImagePath)
{
	return SetScreamSignalImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetScreamSignalImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageScreamSignal)
	{
		delete g_pImageScreamSignal;
	}

	g_pImageScreamSignal = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

void CGraphicCaller::SetPackOutboundImage(const char* strImagePath)
{
	return SetPackOutboundImage(StringEx::string_to_wstring(strImagePath).c_str());
}

void CGraphicCaller::SetPackOutboundImage(const wchar_t* wstrImagePath)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImagePackOutbound)
	{
		delete g_pImagePackOutbound;
	}

	g_pImagePackOutbound = Image::FromFile(wstrImagePath);

	CGraph::Refresh();

	return;
}

const unsigned char CGraphicCaller::GetNo() const
{
	return m_byNo;
}

const unsigned char CGraphicCaller::GetStation() const
{
	return m_byStation;
}

void CGraphicCaller::BindStation(const unsigned char byStation)
{
	if (m_byStation == byStation)
	{
		return;
	}

	m_byStation = byStation;

	return;
}

void CGraphicCaller::SetCallSignal(const unsigned char bySignal)
{
	if (m_byCallSignal == bySignal)
	{
		return;
	}

	m_byCallSignal = bySignal;

	return;
}

void CGraphicCaller::SetCancelCallSignal(const unsigned char bySignal)
{
	if (m_byCancelCallSignal == bySignal)
	{
		return;
	}

	m_byCancelCallSignal = bySignal;

	return;
}

void CGraphicCaller::SetPassSignal(const unsigned char bySignal)
{
	if (m_byPassSignal == bySignal)
	{
		return;
	}

	m_byPassSignal = bySignal;

	return;
}

void CGraphicCaller::SetCancelPassSignal(const unsigned char bySignal)
{
	if (m_byCancelPassSignal == bySignal)
	{
		return;
	}

	m_byCancelPassSignal = bySignal;

	return;
}

void CGraphicCaller::SetNGSignal(const unsigned char bySignal)
{
	if (m_byNGSignal == bySignal)
	{
		return;
	}

	m_byNGSignal = bySignal;

	return;
}

void CGraphicCaller::SetCancelNGSignal(const unsigned char bySignal)
{
	if (m_byCancelNGSignal == bySignal)
	{
		return;
	}

	m_byCancelNGSignal = bySignal;

	return;
}

void CGraphicCaller::SetScreamSignal(const unsigned char bySignal)
{
	if (m_byScreamSignal == bySignal)
	{
		return;
	}

	m_byScreamSignal = bySignal;

	return;
}

void CGraphicCaller::SetPackOutboundSignal(const bool bOutbound)
{
	if (m_bPackOutbound == bOutbound)
	{
		return;
	}

	m_bPackOutbound = bOutbound;

	return;
}

const bool CGraphicCaller::IsInside(const PointF ptWinpoint)
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
const bool CGraphicCaller::IsInside(const CPoint ptWinpoint)
{
	return IsInside(CPointToPointF(ptWinpoint));
}
#endif //_AFX

void CGraphicCaller::DrawImage(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	PointF ptWindow = VecToWin(m_ptCenter);	/*!< ÖÐÐÄ×ø±ê */

	/*
	 * »õÁÏÌùÍ¼
	 *
	 * µ×Í¼
	 * ×´Ì¬ÌùÍ¼
	 * µ×Í¼
	 *
	 * ÎÄ×ÖÌùÍ¼
	*/

	float fWidth = GetReallyWidth()/*!< Êµ¼Ê¿í */, fHeight = GetReallyHeight() /*!< Êµ¼Ê¸ß */;
	RectF rectBK/*!< ±³¾°ÌùÍ¼ */, rectText /*!< ÎÄ±¾ÌùÍ¼ */;

	rectBK.X = ptWindow.X - fWidth / 2.0f;
	rectBK.Y = ptWindow.Y - fHeight / 2.0f;
	rectBK.Width = fWidth;
	rectBK.Height = fHeight;

	rectText.X = rectBK.X;
	rectText.Y = rectBK.GetBottom();
	rectText.Width = rectBK.Width;
	rectText.Height = rectBK.Height;

	m_pBrush->SetColor(Color::Red);

	if (m_bSelect && g_bEdit)
	{
		graphic->FillRectangle(m_pBrush, rectBK);
	}

	/*!
	 * »æÖÆºô½ÐÆ÷ÌùÍ¼
	*/
	graphic->DrawImage(g_pImageCaller, rectBK);

	/*!
	 * »æÖÆºô½ÐÆ÷ÐÍºÅ
	*/
	m_gsFormat.SetAlignment(StringAlignmentCenter);
	m_gsFormat.SetLineAlignment(StringAlignmentNear);

	Gdiplus::Font* pFont = new Gdiplus::Font(g_pFontFamily, (g_unWidth / 10) * g_fScale, FontStyleRegular, Gdiplus::Unit::UnitPoint);

	m_pBrush->SetColor(Color::Red);

	wstring strText = StringEx::string_format(L"ºô½ÐÆ÷£º%d", m_byNo);
	graphic->DrawString(strText.c_str(), strText.length(), pFont, rectText, &m_gsFormat, m_pBrush);

	delete pFont;

	if (g_bEdit)
	{
		g_mutex.unlock();

		return;
	}

	Rect rectStatus /*!< ×´Ì¬ÌùÍ¼ */, rectOther /*!< ÆäËûÌùÍ¼ */;

	rectStatus.X = rectBK.X + fWidth / 4.0f;
	rectStatus.Y = rectBK.Y + fHeight / 4.0f;
	rectStatus.Width = rectBK.Width / 2.0f;
	rectStatus.Height = rectBK.Height / 2.0f;

	rectOther.X = rectBK.X + fWidth / 4.0f;
	rectOther.Y = rectBK.Y - rectBK.Height / 2.0f;
	rectOther.Width = rectBK.Width / 2.0f;
	rectOther.Height = rectBK.Height / 2.0f;

	/*!
	 * »æÖÆºô½ÐÆ÷×´Ì¬
	*/
	Image* pImage = nullptr;
	bool bFlash = false;

	// ºô½Ð
	if (m_byCallSignal != 0)
	{
		pImage = g_pImageCallSignal;

		bFlash = true;
	}

	// È¡Ïûºô½Ð
	if (m_byCancelCallSignal != 0)
	{
		pImage = g_pImageCancelCallSignal;

		bFlash = true;
	}

	// ·ÅÐÐ
	if (m_byPassSignal != 0)
	{
		pImage = g_pImagePassSignal;

		bFlash = true;
	}

	// È¡Ïû·ÅÐÐ
	if (m_byCancelPassSignal != 0)
	{
		pImage = g_pImageCancelPassSignal;

		bFlash = true;
	}

	// NG
	if (m_byNGSignal != 0)
	{
		pImage = g_pImageNGSignal;

		bFlash = true;
	}

	// È¡ÏûNG
	if (m_byCancelNGSignal != 0)
	{
		pImage = g_pImageCancelNGSignal;

		bFlash = true;
	}

	// ¼±Í£
	if (m_byScreamSignal != 0)
	{
		pImage = g_pImageScreamSignal;

		bFlash = true;
	}

	if (bFlash)
	{
		if (g_ullFlashTime == 0)
		{
			g_bFlash = true;
			g_ullFlashTime = GetTickCountEX();
		}
		else if (g_ullFlashTime > 0 && GetTickCountEX() - g_ullFlashTime > 1000)
		{
			g_bFlash = !g_bFlash;
			g_ullFlashTime = GetTickCountEX();
		}

		if (g_bFlash)
		{
			graphic->DrawImage(pImage, rectStatus);
		}
	}

	if (m_bPackOutbound)
	{
		graphic->DrawImage(g_pImagePackOutbound, rectOther);
	}

	g_mutex.unlock();

	return;
}

void CGraphicCaller::SetParam(const char* strParam)
{
	m_strParam = strParam;

	return;
}

void CGraphicCaller::SetParam(const wchar_t* strParam)
{
	m_strParam = StringEx::wstring_to_string(strParam);

	return;
}

const string CGraphicCaller::GetParam() const
{
	return m_strParam;
}

const wstring CGraphicCaller::GetParamW() const
{
	return StringEx::string_to_wstring(m_strParam.c_str());
}
#endif //_CGRAPHICCALLER_H