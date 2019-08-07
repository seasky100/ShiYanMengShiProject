#pragma once
#include "CGraph.h"

#ifndef _CGRAPHICWIDGET_H
#define _CGRAPHICWIDGET_H

class CGraphicWidget :
	public CGraph
{
public:
	CGraphicWidget();
	~CGraphicWidget();
protected:
	Graphics* m_pGraphics;
	StringFormat m_gsFormat;
	SolidBrush* m_pBrush;
	Pen* m_pPen;
	Bitmap* m_pclBmp;
	
protected:
	PointF m_ptCenter;		/*!< 中心坐标 矢量坐标 */
	PointF m_ptPrepoint;	/*!< 起始坐标 */
	PointF m_ptLastpoint;	/*!< 终止坐标 */
	PointF m_ptLocal;		/*!< 记录坐标 */
	bool m_bMove;			/*!< 移动标识 */
	bool m_bSelect;			/*!< 选中标识 */
	bool m_bNew;			/*!< 数据更新标识 */
	bool m_bRefresh;
	
public:
	/*!
	 * @brief 获取选中状态
	 * @return bool 选中返回true,未选中返回false
	*/
	virtual const bool IsSelected() const;
	virtual const bool IsSelected(PointF& pt) const;
#ifdef _AFX
	virtual const bool IsSelected(CPoint& pt) const;
#endif //_AFX

	virtual const bool IsNewed() const;
	virtual const bool IsMoveed() const;

	/*!
	 * @brief 选中
	 * @param ptWinpoint 窗口坐标
	*/
	virtual void Select(const PointF ptWinpoint);
#ifdef _AFX
	virtual void Select(const CPoint ptWinpoint);
#endif //_AFX

	/*!
	 * @brief 拖拽
	 * @param  ptWinpoint 窗口坐标
	 * @return HCURSOR 鼠标指针形状
	*/
	virtual const HCURSOR Drag(const PointF ptWinPoint);
#ifdef _AFX
	virtual const HCURSOR Drag(const CPoint ptWinPoint);
#endif //_AFX

	/*!
	 * @brief 确认
	*/
	virtual void Confirm();

	/*!
	 * @brief 取消
	*/
	virtual void Cancel();

	/*!
	 * @brief 获取中心坐标
	 * @return CPoint 中心坐标
	*/
	virtual const PointF GetCenterPoint() const;
#ifdef _AFX
	virtual const CPoint GetCenterPointC() const;
#endif //_AFX

	virtual const PointF GetLocalPoint() const;
#ifdef _AFX
	virtual const CPoint GetLocalPointC() const;
#endif //_AFX

	/*!
	 * @brief 设置中心坐标
	 * @param ptCenter 中心坐标
	*/
	virtual void SetCenterPoint(const PointF ptCenter);
#ifdef _AFX
	virtual void SetCenterPoint(const CPoint ptCenter);
#endif //_AFX

	void InitGraphic(HDC hdc);

	void Locate();

	void Draw(HDC hdc);
	void Draw(Graphics* graphic);

	virtual void DrawImage(Graphics* graphic) = 0;
};

#endif // !_CGRAPHICWIDGET_H