/*!
 * @file CGraph.h
 * @brief CGraph类头文件
 * @author FanKaiyu
 * @date 2018-10-16
 * @version 2.0
*/
#pragma once
//#include <afx.h>
#include <Windows.h>
#include <gdiplus.h>

using namespace Gdiplus;

#ifndef _CGRAPH_H
#define _CGRAPH_H

#ifdef _WIN64
#define ULL unsigned long long
#define GetTickCountEX() GetTickCount64()
#else
#define ULL unsigned long
#define GetTickCountEX() GetTickCount()
#endif

#define MAX_SCALE 10.0f
#define MIN_SCALE 0.4f
#define DEFALUT_SCALE 10.0f

static const PointF PointToPointF(const Point pt)
{
	return PointF((REAL)pt.X, (REAL)pt.Y);
}

static const Point PointFToPoint(const PointF pt)
{
	return Point((INT)pt.X, (INT)pt.Y);
}

static const RectF RectToRectF(const Rect rc)
{
	return RectF((REAL)rc.X, (REAL)rc.Y, (REAL)rc.Width, (REAL)rc.Height);
}

static const Rect RectFToRect(const RectF rc)
{
	return Rect((INT)rc.X, (INT)rc.Y, (INT)rc.Width, (INT)rc.Height);
}

#ifdef _AFX
static const Point CPointToPoint(const CPoint pt)
{
	return Point(pt.x, pt.y);
}

static const PointF CPointToPointF(const CPoint pt)
{
	return PointToPointF(CPointToPoint(pt));;
}

static const CPoint PointToCPoint(const Point pt)
{
	return CPoint(pt.X, pt.Y);
}

static const CPoint PointToCPoint(const PointF pt)
{
	return PointToCPoint(PointFToPoint(pt));;
}

static const Rect CRectToRect(const CRect rc)
{
	return Rect(rc.left, rc.top,rc.Width(), rc.Height());
}

static const RectF CRectToRectF(const CRect rc)
{
	return RectToRectF(CRectToRect(rc));
}

static const CRect RectToCRect(const Rect rc)
{
	return CRect(rc.X, rc.Y, rc.GetRight(), rc.GetBottom());
}

static const CRect RectToCRect(const RectF rc)
{
	return RectToCRect(RectFToRect(rc));;
}
#endif //_AFX

/*!
 * @class CGraph
 * @brief 基层图类
 *
 * 绘图的基本属性,用于其他绘图对象继承
 * 10个点代表10cm
 * 地图最大尺寸为500*500m
 * 即50000 * 50000
*/
class CGraph
{
public:
	CGraph();
	~CGraph();

protected:
	GdiplusStartupInput m_gdiplusInput;		/*!< GDI+初始化输入句柄 */
	GdiplusStartupOutput m_gdiplusOutput;	/*!< GDI+初始化输出句柄 */
	ULONG_PTR m_ulGdiplusToken;

protected:
	static PointF g_ptOffset;			/*!< 偏移量 */
	static float g_fScale;				/*!< 缩放比例 */
	static unsigned int g_unDistance;	/*!< 网格间距 */
	static Size g_sizeVector;			/*!< 矢量图尺寸 */
	static RectF g_rcClient;			/*!< 客户区尺寸 */
	static bool g_bFlash;				/*!< 闪烁标识 */
	static ULL g_ullFlashTime;			/*!< 闪烁开始时间 */
	static bool g_bRefresh;
	static bool g_bEdit;
	
public:
	/*!
	 * @brief 设置客户区尺寸
	 * @param rcClient 客户区尺寸
	*/
	static void SetClientRect(const RectF rcClient);
#ifdef _AFX
	static void SetClientRect(const CRect rcClient);
#endif //_AFX

	/*!
	 * @brief 获取客户区尺寸
	*/
	static const RectF GetClientRect();
#ifdef _AFX
	static const CRect GetClientRectC();
#endif
	

	/*!
	 * @brief 获取偏移量
	 * @return CPoint 偏移量
	*/
	static const PointF GetOffset();

	/*!
	 * @brief 设置偏移量
	 * @param ptOffset 偏移量
	*/
	static void SetOffset(const Point ptOffset);
	static void SetOffset(const PointF ptOffset);
#ifdef _AFX
	static void SetOffset(const CPoint ptOffset);
#endif //_AFX

	/*!
	 * @brief 获取缩放比例
	 * @return float fScale
	*/
	static const float GetScale();

	/*!
	 * @brief 设置缩放比例
	 * @param fScale 缩放比例
	*/
	static void SetScale(const float fScale);

	/*!
	 * @brief 将矢量坐标转换为窗口坐标
	 * @param ptVector 矢量坐标
	 * @return CPoint 窗口坐标
	*/
	static const Point VecToWin(const Point ptVector);
	static const PointF VecToWin(const PointF ptVector);
#ifdef _AFX
	static const CPoint VecToWin(const CPoint ptVector);
#endif //_AFX
	static const RectF VecToWin(const RectF rcVector);
	static const Rect VecToWin(const Rect rcVector);

	/*!
	 * @brief 将窗口坐标转换未矢量坐标
	 * @param ptWindow 窗口坐标
	 * @return CPoint 矢量坐标
	*/
	static const PointF WinToVec(const PointF ptWindow);
	static const Point WinToVec(const Point ptWindow);
#ifdef _AFX
	static const CPoint WinToVec(const CPoint ptWindow);
#endif //_AFX
	static const RectF WinToVec(const RectF rcWindow);
	static const Rect WinToVec(const Rect rcWindow);

	/*!
	 * @brief 矫正矢量坐标
	 * @param ptVector 矢量坐标
	 * @return CPoint 矫正后的矢量坐标
	*/
	static const Point Correct(const Point ptVector);
	static const PointF Correct(const PointF ptVector);
#ifdef _AFX
	static const CPoint Correct(const CPoint ptVector);
#endif //_AFX

	/*!
	 * @brief 定位
	 *
	 * 将地图中心移动至指定坐标点上
	 * @param CPoint 定位坐标
	*/
	static void Locate(const Point ptLocation);
	static void Locate(const PointF ptLocation);
#ifdef _AFX
	static void Locate(const CPoint ptLocation);
#endif //_AFX

	/*!
	 * @brief 刷新
	 * @param bool 为true刷新,否则不刷新
	*/
	static void Refresh(bool bFresh = true);

	static void SetEnableEdit(bool bEdit);

	static const bool EnableEdit();
};

#endif // !_CGRAPH_H