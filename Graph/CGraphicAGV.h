/*!
 * @file CGraphicAGV.h
 * @brief CGraphicAGV类头文件
 * @author FanKaiyu
 * @date 2018-10-15
 * @version 2.0
*/

#pragma once
#include "CGraphicWidget.h"

#ifndef _CGRAPHICAGV_H
#define _CGRAPHICAGV_H

#include <string>
#include <mutex>

using namespace std;

enum _GraphicAGVActionStatus
{
	_GRAPHICAGVACTSTA_NONE,
	_GRAPHICAGVACTSTA_LOAD,
	_GRAPHICAGVACTSTA_UNLOAD,
};

/*!
 * @class CGraphicAGV
 * @brief AGV图类
 *
 * 继承基层图类,绘制AGV图形以及根据AGV状态绘制AGV图形等操作
*/
class CGraphicAGV :
	public CGraphicWidget
{
public:
	CGraphicAGV(const unsigned char byNo,const char* strParam);
	CGraphicAGV(const unsigned char byNo, const wchar_t* wstrParam);
	CGraphicAGV(const unsigned char byNo, string strParam = "");
	CGraphicAGV(const unsigned char byNo, const wstring wstrParam);
	~CGraphicAGV();

protected:
	unsigned char m_byNo;				/*!< 编号 */
	bool m_bRun;						/*!< 移动标识 */
	bool m_bUnVol;						/*!< 电量不足标识 */
	bool m_bError;						/*!< 异常标识 */
	bool m_bObs;						/*!< 避障标识 */
	bool m_bCargo;						/*!< 载货状态 */
	unsigned char m_byAction;			/*!< 动作状态 0-无动作/完成 1-上料中 2-下料中 */
	string m_strParam;					/*!< 用以存放其他配置参数的JSON字符串 */

protected:
	bool m_bShow;						/*!< 显示标识符 */

protected:
	static bool g_bInstall;
	static mutex g_mutex;
	static unsigned int g_unWidth;		/*!< 宽 */
	static unsigned int g_unHeight;		/*!< 高 */
	static FontFamily *g_pFontFamily;	/*!< 字体 */
	static Color g_colorFont;
	static Image* g_pImageAGV;
	static Image* g_pImageRun;
	static Image* g_pImageUnvol;
	static Image* g_pImageObs;
	static Image* g_pImageError;
	static Image* g_pImageCargo;
	static Image* g_pImageLoad;
	static Image* g_pImageUnload;

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
	 * @brief 设置AGV贴图
	 * @param const char* AGV贴图路径
	*/
	static void SetAGVImage(const char* strAGV);

	static void SetAGVImage(const wchar_t* wstrAGV);

	/*!
	 * @brief 设置移动贴图
	 * @param const char* 移动贴图路径
	*/
	static void SetRunImage(const char* strRun);

	static void SetRunImage(const wchar_t* wstrRun);

	/*!
	 * @brief 设置电量不足贴图
	 * @param const char* 电量不足贴图路径
	*/
	static void SetUnVolImage(const char* strUnVol);

	static void SetUnVolImage(const wchar_t* wstrUnVol);

	/*!
	 * @brief 设置异常贴图
	 * @param const char* 异常贴图路径
	*/
	static void SetErrorImage(const char* strError);

	static void SetErrorImage(const wchar_t* wstrError);

	/*!
	 * @brief 设置避障贴图
	 * @param const char* 避障贴图路径
	*/
	static void SetObsImage(const char* strObs);

	static void SetObsImage(const wchar_t* wstrObs);

	/*!
	 * @brief 设置上料贴图
	 * @param const char* 上料贴图路径
	*/
	static void SetLoadImage(const char* strLoad);

	static void SetLoadImage(const wchar_t* wstrLoad);

	/*!
	 * @brief 设置下料贴图
	 * @param const char* 下料贴图路径
	*/
	static void SetUnloadImage(const char* strUnload);

	static void SetUnloadImage(const wchar_t* wstrUnload);

	/*!
	 * @brief 设置载货贴图
	 * @param const char* 载货贴图路径
	*/
	static void SetCargoImage(const char* strCargo);

	static void SetCargoImage(const wchar_t* wstrCargo);
public:
	/*!
	 * @brief 获取编号
	 * @return unsigned int 编号
	*/
	const unsigned char GetNo() const;

	/*!
	 * @brief 设置移动状态
	 * @param bool 移动标识 
	*/
	void SetRunStatus(const bool bRun);

	/*!
	 * @brief 设置电量不足状态
	 * @param bool 电量不足标识
	*/
	void SetUnVolStatus(const bool bUnVol);

	/*!
	 * @brief 设置异常状态
	 * @param bool 异常标识
	*/
	void SetErrorStatus(const bool bError);

	/*!
	 * @brief 设置避障状态
	 * @param bool 避障标识
	*/
	void SetObsStatus(const bool bObs);

	/*!
	 * @brief 设置载货状态
	 * @param bool 载货标识
	*/
	void SetCargoStatus(const bool bCargo);

	/*!
	 * @brief 设置动作状态
	 * @param unsigned char 动作状态
	*/
	void SetActionStatus(const unsigned char byAction);

	/*!
	 * @brief 是否显示
	 * @param bShow true为显示 false为隐藏
	*/
	void IsShow(const bool bShow);

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
	 * @brief 绘图
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

#endif //!_CGRAPHICAGV_H