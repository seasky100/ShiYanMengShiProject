/*!
 * @file CGraphicMark.h
 * @brief CGraphicMark类头文件
 * @author FanKaiyu
 * @date 2018-10-22
 * @version 2.0
*/

#pragma once
#include "CGraphicWidget.h"

#ifndef _CGRAPHICRFID_H
#define _CGRAPHICRFID_H

#include <mutex>

using namespace std;

/*!
 * @class CGraphicMark
 * @brief 地标卡类
 *
 * 继承基层图类,绘制地标卡图形以及拖拽等操作
*/
class CGraphicRFID :
	public CGraphicWidget
{
public:
	CGraphicRFID(unsigned short usNo,PointF ptCenter = PointF(0,0));
	~CGraphicRFID();

protected:
	unsigned short m_usNo;	/*!< 编号 */

protected:
	static bool g_bInstall;
	static mutex g_mutex;
	static unsigned int g_unWidth;	/*!< 宽 */
	static unsigned int g_unHeight;	/*!< 高 */
	static FontFamily* g_pFontFamily;	/*!< 字体 */
	static Color g_colorFont;

public:
	static bool Install();

	static void SetWidth(const unsigned int unWidth);

	static void SetHeight(const unsigned int unHeight);

	static void SetFontType(const char* strFontType);

	static void SetFontType(const wchar_t* wstrFontType);

	static void SetFontColor(const Color color);

	static void SetFontColr(const COLORREF color);

	static void Uninstall();

	static const unsigned int GetWidth();

	static const unsigned int GetHeight();

	static const float GetReallyWidth();

	static const float GetReallyHeight();

public:
	/*!
	 * @brief 获取编号
	 * @return unsigned short 编号
	*/
	const unsigned short GetNo() const;

	/*!
	 * @brief 窗口坐标是否在区域内
	 * @param ptWinpoint 窗口坐标
	 * @return 在区域内返回true,不在返回false
	*/
	const bool IsInside(const PointF ptWinpoint);
#ifdef _AFX
	const bool IsInside(const CPoint ptWinpoint);
#endif //_AFX

	/*!
	 * @brief 绘制
	 * @param pDC 窗口句柄
	*/
	void DrawImage(Graphics* graphic);
};

#endif //!_CGRAPHICRFID_H