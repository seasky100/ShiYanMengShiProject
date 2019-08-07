#pragma once
#include "CGraphicWidget.h"

#ifndef _CGRAPHICTRACK_H
#define _CGRAPHICTRACK_H

#include <math.h>

class CGraphicTrack :
	public CGraphicWidget
{
public:
	CGraphicTrack(const bool bArc, PointF ptStart = PointF(0, 0), PointF ptEnd = PointF(0, 0));
#ifdef _AFX
	CGraphicTrack(const bool bArc, CPoint ptStart, CPoint ptEnd);
#endif //_AFX
	~CGraphicTrack();

protected:
	bool m_bArc;			/*!< 弧线标识 */
	PointF m_ptDistance;	/*!< 终点与起点的偏移量*/
	bool m_bShow;			/*!< 显示标识 */

protected:
	static float g_fWidth;	/*!< 宽 */

public:
	/*!
	 * @brief 设置起点坐标
	 * @param ptStart 起点坐标(矢量坐标)
	*/
	void SetCenterPoint(const PointF ptStart);
#ifdef _AFX
	void SetCenterPoint(const CPoint ptCenter);
#endif //_AFX

	/*!
	 * @brief 设置终点坐标
	 * @param ptEnd 终点坐标(矢量坐标)
	*/
	void SetEndPoint(const PointF ptEnd);
#ifdef _AFX
	void SetEndPoint(const CPoint ptCenter);
#endif //_AFX

	/*!
	 * @brief 设置轨道类型
	 * @param bArc 弧线为true,直线为false
	*/
	void SetType(const bool bArc);

	/*!
	 * @brief 获取终点坐标(矢量坐标)
	 * @return CPoint 终点坐标
	*/
	const PointF GetEndPoint() const;
	const PointF GetLocalEndPoint() const;
	const PointF GetDistancePoint() const;

	/*!
	 * @brief 获取轨道类型
	 * @return bool true为弧形,false为直线
	*/
	const bool GetType() const;

	/*!
	 * @brief 窗口坐标是否在区域内
	 * @param ptWinpoint 窗口坐标
	 * @return 在区域内返回true,不在返回false
	*/
	const bool IsInside(const PointF ptWinpoint);

	/*!
	 * @brief 拖拽
	 * @param  ptWinpoint 窗口坐标
	 * @return HCURSOR 鼠标指针形状
	*/
	const HCURSOR Drag(const PointF ptWinPoint);
#ifdef _AFX
	const HCURSOR Drag(const CPoint ptWinPoint);
#endif //_AFX

	/*!
	 * @brief 绘制
	 * @param pDC 窗口句柄
	*/
	void DrawImage(Graphics* graphic);

	/*!
	 * @brief 设置轨道宽度
	 * @param fWidth 宽度
	*/
	static void SetWidth(const float fWidth);

	/*!
	 * @brief 获取轨道宽度
	 * @return float 宽度
	*/
	static const float GetWidth();

	/*!
	 * @brief 弧度转角度
	 * @param dbRadian 弧度值
	 * @return double 角度值
	*/
	static const double RadianToAngle(const double dbRadian);

	/*!
	 * @brief 角度转弧度
	 * @param dbAngle 角度值
	 * @return double 弧度值
	*/
	static const double AngleToRadian(const double dbAngle);

	/*!
	 * @brief 弧度转角度
	 * @param dbRadian 弧度值
	 * @return double 角度值
	*/
	static const float RadianToAngleF(const float dbRadian);

	/*!
	 * @brief 角度转弧度
	 * @param dbAngle 角度值
	 * @return double 弧度值
	*/
	static const float AngleToRadianF(const float dbAngle);
};

#endif //!_CGRAPHICTRACK_H