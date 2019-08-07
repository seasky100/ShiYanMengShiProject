/*!
 * @file CGraphicBackground.h
 * @brief CGraphicBackground类头文件
 * @author FanKaiyu
 * @date 2018-10-15
 * @version 2.0
*/

#pragma once
#include "CGraphicWidget.h"
#include <mutex>

using namespace std;

#ifndef _CGRAPHICBACKGROUND_H
#define _CGRAPHICBACKGROUND_H

/*!
 * @class CGraph
 * @brief 背景图类
 *
 * 继承基层图类,绘制背景图、辅助线以及拖拽、缩放等操作
*/
class CGraphicBackground :
	public CGraphicWidget
{
public:
	CGraphicBackground(Color colorLine = Color::Black,float fWidth = 2.0f);
	CGraphicBackground(const COLORREF colorLine, const float fWidth);
	~CGraphicBackground();

protected:
	Image *m_pImage;		/*!< 背景图 */
	float m_fWidth;			/*!< 辅助线宽度 */

protected:
	static bool g_bInstall;
	static mutex g_mutex;
	static Color g_gcrFont;
	static FontFamily* g_pFontFamily;	/*!< 字体 */

public:
	static bool Install();

	static void SetFontType(const char* strFontType);

	static void SetFontType(const wchar_t* wstrFontType);

	static void Uninstall();
public:
	/*!
	 * @brief 设置辅助线
	 * @param colorLine 辅助线颜色
	 * @param fWidth 辅助线宽度
	*/
	void SetLine(const Color colorLine, const float fWidth);
	void SetLine(const COLORREF colorLine, const float fWidth);

	/*!
	 * @brief 获取辅助线
	 * @param colorLine 返回辅助线颜色
	 * @param fWidth 返回辅助线宽度
	*/
	void GetLine(Color &colorLine, float &fWidth);
	void GetLine(COLORREF& colorLine, float& fWidth);

	/*!
	 * @brief 设置背景图
	 * @param strPath 背景图路径
	*/
	void SetImage(const char* strPath);
	void SetImage(const wchar_t* strPath);

	/*!
	 * @brief 缩放
	 *
	 * 默认放大
	 * @param bool 缩小标识
	*/
	void Scale(bool bShrink = false);

	/*!
	 * @brief 复位
	*/
	void Reset();

	/*!
	 * @brief 拖拽
	 * @param  ptWinpoint 窗口坐标
	 * @return HCURSOR 鼠标指针形状
	*/
	const HCURSOR Drag(const PointF ptWinPoint);
#ifdef _AFX
	const HCURSOR Drag(const CPoint ptWinPoint);
#endif //_AFX

public:
	void DrawImage(Graphics* graphic);

	/*!
	 * @brief 绘制背景图
	 * @param pDC 视图区句柄
	*/
	void DrawBackground(HDC hdc);
	void DrawBackground(Graphics* graphic);

	/*!
	 * @brief 绘制辅助线
	 * @param pDC 视图区句柄
	*/
	void DrawLine(HDC hdc);
	void DrawLine(Graphics* graphic);

	/*!
	 * @brief 绘制坐标
	 * @param pDC 视图区句柄
	*/
	void DrawPoint(HDC hdc);
	void DrawPoint(Graphics* graphic);
};

#endif // !_CGRAPHICBACKGROUND_H