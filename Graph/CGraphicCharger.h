/*!
 * @file CGraphicCharger.h
 * @brief CGraphicCharger类头文件
 * @author FanKaiyu
 * @date 2018-10-22
 * @version 2.0
*/

#pragma once
#include "CGraphicWidget.h"
#include <mutex>

using namespace std;

#ifndef _CGRAPHICCHARGER_H
#define _CGRAPHICCHARGER_H

/*!
 * @class CGraphicCharger
 * @brief 充电器图类
 *
 * 继承基层图类,绘制充电器图形以及拖拽等操作
*/
class CGraphicCharger :
	public CGraphicWidget
{
public:
	CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler,const char* strName,const char* strParam);
	CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler, const wchar_t* wstrName,const wchar_t* wstrParam);
	CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler, string strName = "",string strParam = "");
	CGraphicCharger(const unsigned char byNo, const unsigned short usLocation, const unsigned char byCtrler, const wstring wstrName,const wstring wstrParam = L"");
	~CGraphicCharger();

protected:
	unsigned char m_byNo;			/*!< 编号 */
	unsigned short m_usLocation;	/*!< 坐标 */
	unsigned char m_byCtrler;		/*!< 控制器 */
	string m_strName;				/*!< 名称 */
	string m_strParam;				/*!< 用以存放其他配置参数的JSON字符串 */

protected:
	bool m_bPower;					/*!< 通电状态 */
	ULL m_ullChargeTime;			/*!< 充电开始时间 */

protected:
	static bool g_bInstall;
	static mutex g_mutex;
	static unsigned int g_unWidth;	/*!< 宽 */
	static unsigned int g_unHeight;	/*!< 高 */
	static FontFamily* g_pFontFamily;	/*!< 字体 */
	static Color g_colorFont;
	static Image* g_pImageBK;
	static Image* g_pImagePower;

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

	/*!
	 * @brief 设置充电贴图
	 * @param const char* 充电贴图路径
	*/
	static void SetPowerImage(const char* strPower);

	static void SetPowerImage(const wchar_t* wstrPower);
public:
	/*!
	 * @brief 获取编号
	 * @return unsigned char 编号
	*/
	const unsigned char GetNo() const;

	/*!
	 * @brief 设置名称
	 * @param const char* 名称
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
	 * @brief 设置充电状态
	 * @param bPower true为充电,false为断电
	*/
	void SetPower(const bool bPower, const ULL ullChargeTime);

	/*!
	 * @brief 设置控制器
	 * @param unsigned char 控制器编号
	*/
	void SetCtrler(const unsigned char byCtrler);

	/*!
	 * @brief 获取控制器
	 * @return unsigned char 控制器编号
	*/
	const unsigned char GetCtrler() const;

	/*!
	 * @brief 设置地标卡
	 * @param unsigned short 地标卡
	*/
	void SetLocation(const unsigned short usLocation);

	/*!
	 * @brief 获取地标卡
	 * @return unsigned short 地标卡
	*/
	const unsigned short GetLocation() const;

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
};

#endif //!_CGRAPHICCHARGER_H