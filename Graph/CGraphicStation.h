/*!
 * @file CGraphicStation.h
 * @brief CGraphicStation类头文件
 * @author FanKaiyu
 * @date 2018-10-22
 * @version 2.0
*/

#pragma once
#include "CGraphicWidget.h"
#include <mutex>

using namespace std;

#ifndef _CGRAPHICSTATION_H
#define _CGRAPHICSTATION_H

/*!
 * @class CGraphicStation
 * @brief 工位图类
 *
 * 继承基层图类,绘制工图以及拖拽等操作
*/
class CGraphicStation :
	public CGraphicWidget
{
public:
	CGraphicStation(const unsigned char byNo, unsigned short usLocation,const char* strName,const char* strParam);
	CGraphicStation(const unsigned char byNo, unsigned short usLocation, const wchar_t* wstrName,const wchar_t* wstrParam);
	CGraphicStation(const unsigned char byNo, unsigned short usLocation, string strName = "",string strParam = "");
	CGraphicStation(const unsigned char byNo, unsigned short usLocation, const wstring wstrName,const wstring wstrParam = L"");
	~CGraphicStation();

protected:
	unsigned char m_byNo;			/*!< 编号 */
	string m_strName;				/*!< 名称 */
	unsigned short m_usLocation;	/*!< RFID卡 */
	string m_strParam;				/*!< 用以存放其他配置参数的JSON字符串 */

protected:
	static bool g_bInstall;
	static mutex g_mutex;
	static unsigned int g_unWidth;	/*!< 宽 */
	static unsigned int g_unHeight;	/*!< 高 */
	static FontFamily* g_pFontFamily;	/*!< 字体 */
	static Color g_colorFont;
	static Image* g_pImageBK;

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

	/*!
	 * @brief 设置背景贴图
	 * @param const char* 背景贴图路径
	*/
	static void SetBKImage(const char* strBK);

	static void SetBKImage(const wchar_t* wstrBK);

public:
	/*!
	 * @brief 获取编号
	 * @return unsigned char 编号
	*/
	const unsigned char GetNo() const;

	/*!
	 * @brief 设置名称
	 * @param CString 名称
	*/
	void SetName(const char* strName);
	void SetName(const wchar_t* strName);

	/*!
	 * @brief 获取名称
	 * @return CString 名称
	*/
	const string GetName() const;
	const wstring GetNameW() const;

	/*!
	 * @brief 设置地标卡
	 * @param unsigned shor 地标卡
	*/
	void SetLocation(const unsigned short usLocation);

	/*!
	 * @brief 获取地标卡
	 * @return unsigned short 地标卡
	*/
	const unsigned short GetLocation() const;

	/*!
	 * @brief 设置其他参数
	 * @param CString 名称
	*/
	void SetParam(const char* strParam);
	void SetParam(const wchar_t* strParam);

	/*!
	 * @brief 获取其他参数
	 * @return CString 名称
	*/
	const string GetParam() const;
	const wstring GetParamW() const;

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

#endif //!_CGRAPHICSTATION_H