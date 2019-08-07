#pragma once
#include "../Graph/CGraphicWidget.h"

#ifndef _CGRAPHICCALLER_H
#define _CGRAPHICCALLER_H
#include <string>
#include <mutex>

using namespace std;

class CGraphicCaller :
	public CGraphicWidget
{
public:
	CGraphicCaller(const unsigned char byNo, const char* strParam,const unsigned char byStation = 0);
	CGraphicCaller(const unsigned char byNo, const wchar_t* wstrParam, const unsigned char byStation = 0);
	CGraphicCaller(const unsigned char byNo, const unsigned char byStation = 0,string strParam="");
	CGraphicCaller(const unsigned char byNo, const wstring wstrParam, const unsigned char byStation = 0);
	~CGraphicCaller();

protected:
	unsigned char m_byNo;					/*!< 编号 */
	unsigned char m_byStation;				/*!< 绑定工位编号 */
	unsigned char m_byCallSignal;			/*!< 呼叫信号 */
	unsigned char m_byCancelCallSignal;		/*!< 取消呼叫信号 */
	unsigned char m_byPassSignal;			/*!< 放行信号 */
	unsigned char m_byCancelPassSignal;		/*!< 取消放行信号 */
	unsigned char m_byNGSignal;				/*!< NG信号 */
	unsigned char m_byCancelNGSignal;		/*!< 取消NG信号 */
	unsigned char m_byScreamSignal;			/*!< 急停信号 */
	bool m_bPackOutbound;					/*!< 出站校验 */
	string m_strParam;						/*!< 用以存放其他配置参数的JSON字符串 */

protected:
	static bool g_bInstall;
	static mutex g_mutex;
	static unsigned int g_unWidth;		/*!< 宽 */
	static unsigned int g_unHeight;		/*!< 高 */
	static FontFamily* g_pFontFamily;	/*!< 字体 */
	static Color g_colorFont;
	static Image* g_pImageCaller;
	static Image* g_pImageCallSignal;
	static Image* g_pImageCancelCallSignal;
	static Image* g_pImagePassSignal;
	static Image* g_pImageCancelPassSignal;
	static Image* g_pImageNGSignal;
	static Image* g_pImageCancelNGSignal;
	static Image* g_pImageScreamSignal;
	static Image* g_pImagePackOutbound;

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
	 * @brief 设置呼叫器贴图
	 * @param const char* 呼叫器贴图路径
	*/
	static void SetCallerImage(const char* strImagePath);

	static void SetCallerImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置呼叫信号贴图
	 * @param const char* 呼叫信号贴图路径
	*/
	static void SetCallSignalImage(const char* strImagePath);

	static void SetCallSignalImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置取消呼叫信号贴图
	 * @param const char* 取消呼叫信号贴图路径
	*/
	static void SetCancelCallImage(const char* strImagePath);

	static void SetCancelCallImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置放行信号贴图
	 * @param const char* 放行信号贴图路径
	*/
	static void SetPassSignalImage(const char* strImagePath);

	static void SetPassSignalImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置取消放行信号贴图
	 * @param const char* 取消放行信号贴图路径
	*/
	static void SetCancelPassSignalImage(const char* strImagePath);

	static void SetCancelPassSignalImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置NG信号贴图
	 * @param const char* NG信号贴图路径
	*/
	static void SetNGSignalImage(const char* strImagePath);

	static void SetNGSignalImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置取消NG信号贴图
	 * @param const char* 取消NG信号贴图路径
	*/
	static void SetCancelNGSignalImage(const char* strImagePath);

	static void SetCancelNGSignalImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置急停信号贴图
	 * @param const char* 急停信号贴图路径
	*/
	static void SetScreamSignalImage(const char* strImagePath);

	static void SetScreamSignalImage(const wchar_t* wstrImagePath);

	/*!
	 * @brief 设置出站校验贴图
	 * @param const char* 急停信号贴图路径
	*/
	static void SetPackOutboundImage(const char* strImagePath);

	static void SetPackOutboundImage(const wchar_t* wstrImagePath);

public:
	/*!
	 * @brief 获取编号
	 * @return unsigned char 编号
	*/
	const unsigned char GetNo() const;

	/*!
	 * @brief 获取工位编号
	 * @return unsigned char 工位编号
	*/
	const unsigned char GetStation() const;

	/*!
	 * @brief 绑定工位
	 * @param unsigned char 工位编号
	*/
	void BindStation(const unsigned char byStation);

	/*!
	 * @brief 设置呼叫信号
	 * @param unsigned char 信号开关
	*/
	void SetCallSignal(const unsigned char bySignal);

	/*!
	 * @brief 设置取消呼叫信号
	 * @param unsigned char 信号开关
	*/
	void SetCancelCallSignal(const unsigned char bySignal);

	/*!
	 * @brief 设置放行信号
	 * @param unsigned char 信号开关
	*/
	void SetPassSignal(const unsigned char bySignal);

	/*!
	 * @brief 设置取消放行信号
	 * @param unsigned char 信号开关
	*/
	void SetCancelPassSignal(const unsigned char bySignal);

	/*!
	 * @brief 设置NG信号
	 * @param unsigned char 信号开关
	*/
	void SetNGSignal(const unsigned char bySignal);

	/*!
	 * @brief 设置取消NG信号
	 * @param unsigned char 信号开关
	*/
	void SetCancelNGSignal(const unsigned char bySignal);

	/*!
	 * @brief 设置急停信号
	 * @param unsigned char 信号开关
	*/
	void SetScreamSignal(const unsigned char bySignal);

	/*!
	 * @brief 设置出站校验
	 * @param bool 出站开关
	*/
	void SetPackOutboundSignal(const bool bOutbound);

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

#endif //!_CGRAPHICCALLER_H