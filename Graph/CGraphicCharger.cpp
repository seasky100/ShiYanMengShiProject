/*!
 * @file CGraphicCharger.cpp
 * @brief CGraphicCharger类执行文件
 * @author FanKaiyu
 * @date 2018-10-22
 * @version 2.0
*/

#include "CGraphicCharger.h"

#ifdef _CGRAPHICCHARGER_H
#include "../String/StringEx.h"

bool CGraphicCharger::g_bInstall = false;
mutex CGraphicCharger::g_mutex;
unsigned int CGraphicCharger::g_unWidth = 0;
unsigned int CGraphicCharger::g_unHeight = 0;
FontFamily* CGraphicCharger::g_pFontFamily = nullptr;
Color CGraphicCharger::g_colorFont = Color::Black;
Image* CGraphicCharger::g_pImageBK = nullptr;
Image* CGraphicCharger::g_pImagePower = nullptr;

CGraphicCharger::CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler, const char* strName, const char* strParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_byCtrler(byCtrler)
	, m_strName(strName)
	, m_strParam(strParam)
	, m_ullChargeTime(0)
	, m_bPower(false)
{
}

CGraphicCharger::CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler, const wchar_t* wstrName, const wchar_t* wstrParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_byCtrler(byCtrler)
	, m_strName(StringEx::wstring_to_string(wstrName).c_str())
	, m_strParam(StringEx::wstring_to_string(wstrParam).c_str())
	, m_ullChargeTime(0)
	, m_bPower(false)
{
}

CGraphicCharger::CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler, string strName, string strParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_byCtrler(byCtrler)
	, m_strName(strName.c_str())
	, m_strParam(strParam.c_str())
	, m_ullChargeTime(0)
	, m_bPower(false)
{
}

CGraphicCharger::CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler, const wstring wstrName, const wstring wstrParam)
	: m_byNo(byNo)
	, m_usLocation(usLocation)
	, m_byCtrler(byCtrler)
	, m_strName(StringEx::wstring_to_string(wstrName.c_str()).c_str())
	, m_strParam(StringEx::wstring_to_string(wstrParam.c_str()).c_str())
	, m_ullChargeTime(0)
	, m_bPower(false)
{
}

CGraphicCharger::~CGraphicCharger()
{
}

bool CGraphicCharger::Install()
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
		g_pImageBK = Image::FromFile(L"../Image/Charger.png");
	}
	if (g_pImagePower == nullptr)
	{
		g_pImagePower = Image::FromFile(L"../Image/Power.png");
	}

	if (g_pFontFamily == nullptr 
		|| g_pImageBK == nullptr 
		|| g_pImagePower == nullptr)
	{
		return false;
	}

	return true;
}

void CGraphicCharger::SetWidth(const unsigned int unWidth)
{
	if (g_unWidth == unWidth)
	{
		return;
	}

	g_unWidth = unWidth;

	CGraph::Refresh();

	return;
}

void CGraphicCharger::SetHeight(const unsigned int unHeight)
{
	if (g_unHeight == unHeight)
	{
		return;
	}

	g_unHeight = unHeight;

	CGraph::Refresh();

	return;
}

void CGraphicCharger::SetFontType(const char* strFontType)
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

void CGraphicCharger::SetFontType(const wchar_t* wstrFontType)
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

void CGraphicCharger::SetFontColor(const Color color)
{
	g_colorFont = color;

	CGraph::Refresh();

	return;
}

void CGraphicCharger::SetFontColr(const COLORREF color)
{
	g_colorFont.SetFromCOLORREF(color);

	CGraph::Refresh();

	return;
}

void CGraphicCharger::Uninstall()
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

	if (g_pImagePower)
	{
		delete g_pImagePower;
		g_pImagePower = nullptr;
	}
}

const unsigned int CGraphicCharger::GetWidth() { return g_unWidth; }

const unsigned int CGraphicCharger::GetHeight() { return g_unHeight; }

const float CGraphicCharger::GetReallyWidth() { return (float)g_unWidth* g_fScale; }

const float CGraphicCharger::GetReallyHeight() { return (float)g_unHeight* g_fScale; }

void CGraphicCharger::SetBKImage(const char* strBK)
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

void CGraphicCharger::SetBKImage(const wchar_t* wstrBK)
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

void CGraphicCharger::SetPowerImage(const char* strPower)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImagePower)
	{
		delete g_pImagePower;
	}

	g_pImagePower = Image::FromFile(StringEx::string_to_wstring(strPower).c_str());

	CGraph::Refresh();

	return;
}

void CGraphicCharger::SetPowerImage(const wchar_t* wstrPower)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImagePower)
	{
		delete g_pImagePower;
	}

	g_pImagePower = Image::FromFile(wstrPower);

	CGraph::Refresh();

	return;
}

const unsigned char CGraphicCharger::GetNo() const
{
	return m_byNo;
}

void CGraphicCharger::SetName(const char* strName)
{
	m_strName = strName;

	return;
}

void CGraphicCharger::SetName(const wchar_t* strName)
{
	m_strName = StringEx::wstring_to_string(strName);

	return;
}

const string CGraphicCharger::GetName() const
{
	return m_strName;
}
const wstring CGraphicCharger::GetNameW() const
{
	return StringEx::string_to_wstring(m_strName.c_str());
}

void CGraphicCharger::SetPower(const bool bPower, const ULL ullChargeTime)
{
	m_bPower = bPower;

	if (m_bPower == false)
	{
		m_ullChargeTime = 0;
	}
	else
	{
		m_ullChargeTime = ullChargeTime;
	}

	return;
}

void CGraphicCharger::SetCtrler(const unsigned char byCtrler)
{
	m_byCtrler = byCtrler;

	return;
}

const unsigned char CGraphicCharger::GetCtrler() const
{
	return m_byCtrler;
}

void CGraphicCharger::SetLocation(const unsigned short usLocation)
{
	m_usLocation = usLocation;

	return;
}

const unsigned short CGraphicCharger::GetLocation() const
{
	return m_usLocation;
}

const bool CGraphicCharger::IsInside(const PointF ptWinpoint)
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
const bool CGraphicCharger::IsInside(const CPoint ptWinpoint)
{
	return IsInside(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

void CGraphicCharger::DrawImage(Graphics* graphic)
{
	if (g_mutex.try_lock() == false)
	{
		return;
	}

	PointF ptWindow = VecToWin(m_ptCenter);
	float fWidth = GetReallyWidth()/*!< 实际宽 */, fHeight = GetReallyHeight() /*!< 实际高 */;
	RectF rectBK/*!< 背景贴图 */, rectStatus /*!< 状态贴图 */, rectName /*!<名称贴图 */, rectTime /*!< 时间贴图 */;

	rectBK.X = ptWindow.X - fWidth / 2;
	rectBK.Y = ptWindow.Y - fHeight / 2;
	rectBK.Width = fWidth;
	rectBK.Height = fHeight;

	rectStatus.X = rectBK.X + fWidth / 4;
	rectStatus.Y = rectBK.Y + fHeight / 4;
	rectStatus.Width = rectBK.Width / 2;
	rectStatus.Height = rectBK.Height / 2;

	rectTime.X = rectBK.X;
	rectTime.Y = rectBK.GetBottom();
	rectTime.Width = rectBK.Width;
	rectTime.Height = rectBK.Height;

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
	 * 绘制底圈
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
		wstrText = StringEx::string_format(L"充电器：%d", m_byNo);
	}

	m_pBrush->SetColor(g_colorFont);
	graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, rectName, &m_gsFormat, m_pBrush);

	if (g_bEdit)
	{
		g_mutex.unlock();

		return;
	}

	/*!
	 * 绘制充电标识符
	*/
	if (m_bPower)
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
			graphic->DrawImage(g_pImagePower, rectStatus);
		}
	}

	wstrText.clear();

	unsigned int unHour = 0;	/*!< 持续小时 */
	unsigned int unMin = 0;		/*!< 持续分钟 */
	unsigned int unSec = 0;		/*!< 持续秒 */
	unsigned int unMinSec = 0;	/*!< 持续毫秒 */

	ULL ullDurativeTime = 0;
	ullDurativeTime = GetTickCountEX() - m_ullChargeTime;

	unHour = ullDurativeTime / 3600000L;
	unMin = (ullDurativeTime % 3600000L) / 60000L;
	unSec = (ullDurativeTime % 60000L) / 1000L;
	unMinSec = ullDurativeTime % 1000L;

	wstrText = StringEx::string_format(L"%dh:%dm:%ds", unHour, unMin, unSec);

	m_gsFormat.SetAlignment(StringAlignmentCenter);
	m_gsFormat.SetLineAlignment(StringAlignmentNear);

	graphic->DrawString(wstrText.c_str(), wstrText.length(), pFont, rectTime, &m_gsFormat, m_pBrush);

	delete pFont;

	return;
}

void CGraphicCharger::SetParam(const char* strParam)
{
	m_strParam = strParam;

	return;
}

void CGraphicCharger::SetParam(const wchar_t* strParam)
{
	m_strParam = StringEx::wstring_to_string(strParam);

	return;
}

const string CGraphicCharger::GetParam() const
{
	return m_strParam;
}

const wstring CGraphicCharger::GetParamW() const
{
	return StringEx::string_to_wstring(m_strParam.c_str());
}
#endif //!_CGRAPHICCHARGER_H