/*!
 * @file CGraphicAGV.cpp
 * @brief CGraphicAGV类执行文件
 * @author FanKaiyu
 * @date 2018-10-15
 * @version 2.0
*/

#include "CGraphicAGV.h"
#include "../String/StringEx.h"

#ifdef _CGRAPHICAGV_H

bool CGraphicAGV::g_bInstall = false;
mutex CGraphicAGV::g_mutex;
unsigned int CGraphicAGV::g_unWidth = 0;
unsigned int CGraphicAGV::g_unHeight = 0;
FontFamily* CGraphicAGV::g_pFontFamily = nullptr;
Color CGraphicAGV::g_colorFont = Color::Black;
Image* CGraphicAGV::g_pImageAGV = nullptr;
Image* CGraphicAGV::g_pImageRun = nullptr;
Image* CGraphicAGV::g_pImageUnvol = nullptr;
Image* CGraphicAGV::g_pImageObs = nullptr;
Image* CGraphicAGV::g_pImageError = nullptr;
Image* CGraphicAGV::g_pImageCargo = nullptr;
Image* CGraphicAGV::g_pImageLoad = nullptr;
Image* CGraphicAGV::g_pImageUnload = nullptr;

CGraphicAGV::CGraphicAGV(const unsigned char byNo, const char* strParam)
	:m_byNo(byNo)
	, m_bRun(false)
	, m_bUnVol(false)
	, m_bError(false)
	, m_bObs(false)
	, m_bCargo(false)
	, m_byAction(_GRAPHICAGVACTSTA_NONE)
	, m_bShow(false)
	, m_strParam(strParam)
{

}

CGraphicAGV::CGraphicAGV(const unsigned char byNo, const wchar_t* wstrParam)
	:m_byNo(byNo)
	, m_bRun(false)
	, m_bUnVol(false)
	, m_bError(false)
	, m_bObs(false)
	, m_bCargo(false)
	, m_byAction(_GRAPHICAGVACTSTA_NONE)
	, m_bShow(false)
	, m_strParam(StringEx::wstring_to_string(wstrParam).c_str())
{
}

CGraphicAGV::CGraphicAGV(const unsigned char byNo, string strParam)
	:m_byNo(byNo)
	, m_bRun(false)
	, m_bUnVol(false)
	, m_bError(false)
	, m_bObs(false)
	, m_bCargo(false)
	, m_byAction(_GRAPHICAGVACTSTA_NONE)
	, m_bShow(false)
	, m_strParam(strParam.c_str())
{
}

CGraphicAGV::CGraphicAGV(const unsigned char byNo, const wstring wstrParam)
	:m_byNo(byNo)
	, m_bRun(false)
	, m_bUnVol(false)
	, m_bError(false)
	, m_bObs(false)
	, m_bCargo(false)
	, m_byAction(_GRAPHICAGVACTSTA_NONE)
	, m_bShow(false)
	, m_strParam(StringEx::wstring_to_string(wstrParam.c_str()).c_str())
{
}

CGraphicAGV::~CGraphicAGV()
{
	
}

bool CGraphicAGV::Install()
{
	if (g_bInstall == false)
	{
		g_unWidth = 40;
		g_unHeight = 40;
		g_colorFont = Color::Red;
		g_bInstall = true;
	}

	if (g_pFontFamily == nullptr)
	{
		g_pFontFamily = new FontFamily(L"微软雅黑");
	}

	if (g_pImageAGV == nullptr)
	{
		g_pImageAGV = Image::FromFile(L"../Image/AGV.png");
	}
	if (g_pImageRun == nullptr)
	{
		g_pImageRun = Image::FromFile(L"../Image/Run.png");
	}
	if (g_pImageUnvol == nullptr)
	{
		g_pImageUnvol = Image::FromFile(L"../Image/Unvol.png");
	}
	if (g_pImageObs == nullptr)
	{
		g_pImageObs = Image::FromFile(L"../Image/Obs.png");
	}
	if (g_pImageError == nullptr)
	{
		g_pImageError = Image::FromFile(L"../Image/Error.png");
	}
	if (g_pImageCargo == nullptr)
	{
		g_pImageCargo = Image::FromFile(L"../Image/Cargo.png");
	}
	if (g_pImageLoad == nullptr)
	{
		g_pImageLoad = Image::FromFile(L"../Image/Load.png");
	}
	if (g_pImageUnload == nullptr)
	{
		g_pImageUnload = Image::FromFile(L"../Image/Unload.png");
	}

	if (g_pFontFamily == nullptr
		|| g_pImageAGV == nullptr
		|| g_pImageRun == nullptr
		|| g_pImageUnvol == nullptr
		|| g_pImageObs == nullptr
		|| g_pImageError == nullptr
		|| g_pImageCargo == nullptr
		|| g_pImageLoad == nullptr
		|| g_pImageUnload == nullptr)
	{
		return false;
	}

	return true;
}

void CGraphicAGV::SetWidth(const unsigned int unWidth)
{
	if (g_unWidth == unWidth)
	{
		return;
	}

	g_unWidth = unWidth;

	CGraph::Refresh();

	return;
}

void CGraphicAGV::SetHeight(const unsigned int unHeight)
{
	if (g_unHeight == unHeight)
	{
		return;
	}

	g_unHeight = unHeight;

	CGraph::Refresh();

	return;
}

void CGraphicAGV::SetFontType(const char* strFontType)
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

void CGraphicAGV::SetFontType(const wchar_t* wstrFontType)
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

void CGraphicAGV::SetFontColor(const Color color)
{
	g_colorFont = color;

	CGraph::Refresh();

	return;
}

void CGraphicAGV::SetFontColr(const COLORREF color)
{
	g_colorFont.SetFromCOLORREF(color);

	CGraph::Refresh();

	return;
}

void CGraphicAGV::Uninstall()
{
	if (g_pFontFamily)
	{
		delete g_pFontFamily;
		g_pFontFamily = nullptr;
	}

	if (g_pImageAGV)
	{
		delete g_pImageAGV;
		g_pImageAGV = nullptr;
	}

	if (g_pImageRun)
	{
		delete g_pImageRun;
		g_pImageRun = nullptr;
	}

	if (g_pImageUnvol)
	{
		delete g_pImageUnvol;
		g_pImageUnvol = nullptr;
	}

	if (g_pImageObs)
	{
		delete g_pImageObs;
		g_pImageObs = nullptr;
	}

	if (g_pImageError)
	{
		delete g_pImageError;
		g_pImageError = nullptr;
	}

	if (g_pImageCargo)
	{
		delete g_pImageCargo;
		g_pImageCargo = nullptr;
	}

	if (g_pImageLoad)
	{
		delete g_pImageLoad;
		g_pImageLoad = nullptr;
	}

	if (g_pImageUnload)
	{
		delete g_pImageUnload;
		g_pImageUnload = nullptr;
	}
}

const unsigned int CGraphicAGV::GetWidth() { return g_unWidth; }

const unsigned int CGraphicAGV::GetHeight() { return g_unHeight; }

const float CGraphicAGV::GetReallyWidth() { return (float)g_unWidth* g_fScale; }

const float CGraphicAGV::GetReallyHeight() { return (float)g_unHeight* g_fScale; }

/*!
 * @brief 设置AGV贴图
 * @param const char* AGV贴图路径
*/
void CGraphicAGV::SetAGVImage(const char* strAGV)
{
	return SetAGVImage(StringEx::string_to_wstring(strAGV).c_str());
}

void CGraphicAGV::SetAGVImage(const wchar_t* wstrAGV)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageAGV)
	{
		delete g_pImageAGV;
	}

	g_pImageAGV = Image::FromFile(wstrAGV);

	CGraph::Refresh();

	return;
}

/*!
 * @brief 设置移动贴图
 * @param const char* 移动贴图路径
*/
void CGraphicAGV::SetRunImage(const char* strRun)
{
	return SetRunImage(StringEx::string_to_wstring(strRun).c_str());
}

void CGraphicAGV::SetRunImage(const wchar_t* wstrRun)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageRun)
	{
		delete g_pImageRun;
	}

	g_pImageRun = Image::FromFile(wstrRun);

	CGraph::Refresh();

	return;
}

/*!
 * @brief 设置电量不足贴图
 * @param const char* 电量不足贴图路径
*/
void CGraphicAGV::SetUnVolImage(const char* strUnVol)
{
	return SetUnVolImage(StringEx::string_to_wstring(strUnVol).c_str());
}

void CGraphicAGV::SetUnVolImage(const wchar_t* wstrUnVol)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageUnvol)
	{
		delete g_pImageUnvol;
	}

	g_pImageUnvol = Image::FromFile(wstrUnVol);

	CGraph::Refresh();

	return;
}

/*!
 * @brief 设置异常贴图
 * @param const char* 异常贴图路径
*/
void CGraphicAGV::SetErrorImage(const char* strError)
{
	return SetErrorImage(StringEx::string_to_wstring(strError).c_str());
}

void CGraphicAGV::SetErrorImage(const wchar_t* wstrError)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageError)
	{
		delete g_pImageError;
	}

	g_pImageError = Image::FromFile(wstrError);

	CGraph::Refresh();

	return;
}

/*!
 * @brief 设置避障贴图
 * @param const char* 避障贴图路径
*/
void CGraphicAGV::SetObsImage(const char* strObs)
{
	return SetObsImage(StringEx::string_to_wstring(strObs).c_str());
}

void CGraphicAGV::SetObsImage(const wchar_t* wstrObs)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageObs)
	{
		delete g_pImageObs;
	}

	g_pImageObs = Image::FromFile(wstrObs);

	CGraph::Refresh();

	return;
}

/*!
 * @brief 设置上料贴图
 * @param const char* 上料贴图路径
*/
void CGraphicAGV::SetLoadImage(const char* strLoad)
{
	return SetLoadImage(StringEx::string_to_wstring(strLoad).c_str());
}

void CGraphicAGV::SetLoadImage(const wchar_t* wstrLoad)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageLoad)
	{
		delete g_pImageLoad;
	}

	g_pImageLoad = Image::FromFile(wstrLoad);

	CGraph::Refresh();

	return;
}

/*!
 * @brief 设置下料贴图
 * @param const char* 下料贴图路径
*/
void CGraphicAGV::SetUnloadImage(const char* strUnload)
{
	return SetUnloadImage(StringEx::string_to_wstring(strUnload).c_str());
}

void CGraphicAGV::SetUnloadImage(const wchar_t* wstrUnload)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageUnload)
	{
		delete g_pImageUnload;
	}

	g_pImageUnload = Image::FromFile(wstrUnload);

	CGraph::Refresh();

	return;
}

/*!
 * @brief 设置载货贴图
 * @param const char* 载货贴图路径
*/
void CGraphicAGV::SetCargoImage(const char* strCargo)
{
	return SetCargoImage(StringEx::string_to_wstring(strCargo).c_str());
}

void CGraphicAGV::SetCargoImage(const wchar_t* wstrCargo)
{
	lock_guard<mutex> lock(g_mutex);

	if (g_pImageCargo)
	{
		delete g_pImageCargo;
	}

	g_pImageCargo = Image::FromFile(wstrCargo);

	CGraph::Refresh();

	return;
}

const unsigned char CGraphicAGV::GetNo() const
{
	return m_byNo;
}

void CGraphicAGV::SetRunStatus(const bool bRun)
{
	if (m_bRun != bRun)
	{
		m_bRun = bRun;
	}

	return;
}

void CGraphicAGV::SetUnVolStatus(const bool bUnVol)
{
	if (m_bUnVol != bUnVol)
	{
		m_bUnVol = bUnVol;
	}

	return;
}

void CGraphicAGV::SetErrorStatus(const bool bError)
{
	if (m_bError != bError)
	{
		m_bError = bError;
	}

	return;
}

void CGraphicAGV::SetObsStatus(const bool bObs)
{
	if (m_bObs != bObs)
	{
		m_bObs = bObs;
	}

	return;
}

void CGraphicAGV::SetCargoStatus(const bool bCargo)
{
	if (m_bCargo != bCargo)
	{
		m_bCargo = bCargo;
	}

	return;
}

void CGraphicAGV::SetActionStatus(const unsigned char byAction)
{
	if (m_byAction != byAction)
	{
		m_byAction = byAction;
	}

	return;
}

void CGraphicAGV::IsShow(const bool bShow)
{
	m_bShow = bShow;

	return;
}

const bool CGraphicAGV::IsInside(const PointF ptWinpoint)
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
const bool CGraphicAGV::IsInside(const CPoint ptWinpoint)
{
	return IsInside(CPointToPointF(ptWinpoint));
}
#endif //_AFX

void CGraphicAGV::DrawImage(Graphics* graphic)
{
	if (m_bShow == false || g_bEdit)
	{
		return;
	}

	if (g_mutex.try_lock() == false)
	{
		return;
	}

	PointF ptWindow = VecToWin(m_ptCenter);	/*!< 中心坐标 */

	/*
	 * 货料贴图
	 *
	 * 底图
	 * 状态贴图
	 * 底图
	 *
	 * 文字贴图
	*/

	float fWidth = GetReallyWidth()/*!< 实际宽 */, fHeight = GetReallyHeight() /*!< 实际高 */;
	RectF rectBK/*!< 背景贴图 */, rectStatus /*!< 状态贴图 */, rectText /*!< 文本贴图 */, rectOther /*!< 其他贴图 */;

	rectBK.X = ptWindow.X - fWidth / 2.0f;
	rectBK.Y = ptWindow.Y - fHeight / 2.0f;
	rectBK.Width = fWidth;
	rectBK.Height = fHeight;

	rectStatus.X = rectBK.X + fWidth / 4.0f;
	rectStatus.Y = rectBK.Y + fHeight / 4.0f;
	rectStatus.Width = rectBK.Width / 2.0f;
	rectStatus.Height = rectBK.Height / 2.0f;

	rectText.X = rectBK.X;
	rectText.Y = rectBK.GetBottom();
	rectText.Width = rectBK.Width;
	rectText.Height = rectBK.Height;

	rectOther.X = rectBK.X + fWidth / 4.0f;
	rectOther.Y = rectBK.Y - rectBK.Height / 2.0f;
	rectOther.Width = rectBK.Width / 2.0f;
	rectOther.Height = rectBK.Height / 2.0f;

	/*!
	 * 绘制AGV贴图
	*/
	graphic->DrawImage(g_pImageAGV, rectBK);

	/*!
	 * 绘制AGV状态
	*/
	Image* pImage = nullptr;
	bool bFlash = false;

	// 移动
	if (m_bRun)
	{
		pImage = g_pImageRun;

		bFlash = true;
	}

	// 避障
	if (m_bObs)
	{
		pImage = g_pImageObs;

		bFlash = true;
	}

	// 动作
	switch (m_byAction)
	{
	case _GRAPHICAGVACTSTA_LOAD:
		pImage = g_pImageLoad;

		bFlash = true;
		break;
	case _GRAPHICAGVACTSTA_UNLOAD:
		pImage = g_pImageUnload;

		bFlash = true;
		break;
	}

	// 电量不足
	if (m_bUnVol)
	{
		pImage = g_pImageUnvol;

		bFlash = true;
	}

	// 异常
	if (m_bError)
	{
		pImage = g_pImageError;

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
			if (pImage == g_pImageUnvol)
			{
				rectStatus.Y += rectBK.Height / 8;
				rectStatus.Height = rectBK.Height / 4;
			}

			graphic->DrawImage(pImage, rectStatus);
		}
	}

	// 载货
	if (m_byAction == _GRAPHICAGVACTSTA_NONE && m_bCargo)
	{
		graphic->DrawImage(g_pImageCargo, rectOther);
	}

	/*!
	 * 绘制AGV型号
	*/
	m_gsFormat.SetAlignment(StringAlignmentCenter);
	m_gsFormat.SetLineAlignment(StringAlignmentNear);

	Font* pFont = new Font(g_pFontFamily, (g_unWidth / 10) * g_fScale, FontStyleRegular, Gdiplus::Unit::UnitPoint);

	m_pBrush->SetColor(g_colorFont);

	wstring strText = StringEx::string_format(L"AGV：%d", m_byNo);
	graphic->DrawString(strText.c_str(), strText.length(), pFont, rectText, &m_gsFormat, m_pBrush);

	delete pFont;

	g_mutex.unlock();

	return;
}

void CGraphicAGV::SetParam(const char* strParam)
{
	m_strParam = strParam;

	return;
}

void CGraphicAGV::SetParam(const wchar_t* strParam)
{
	m_strParam = StringEx::wstring_to_string(strParam);

	return;
}

const string CGraphicAGV::GetParam() const
{
	return m_strParam;
}

const wstring CGraphicAGV::GetParamW() const
{
	return StringEx::string_to_wstring(m_strParam.c_str());
}

#endif //!_CGRAPHICAGV_H