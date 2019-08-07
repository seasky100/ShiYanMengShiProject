#pragma once
#ifndef _MAP_H
#define _MAP_H

#include "../Graph/CGraphicAGV.h"
#include "../Graph/CGraphicBackground.h"
#include "../Graph/CGraphicCharger.h"
#include "../Graph/CGraphicRest.h"
#include "../Graph/CGraphicRFID.h"
#include "../Graph/CGraphicStation.h"
#include "../Graph/CGraphicTrack.h"
#include "CGraphicCaller.h"
#include "../json/json.h"
#include "../ADO/ADOConn.h"
#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

typedef void(__stdcall* UpdateWidget)(Json::Value&,void*);

class CGraphicMap
{
public:
	CGraphicMap(void *pMainFram = nullptr, UpdateWidget func = nullptr);
	~CGraphicMap();

protected:
	static bool g_bInstall;	/*!< 初始化标识 */

protected:
	mutex m_mutex;

protected:
	CGraphicBackground* m_pBK;							/*!< 背景图指针 */
	map<unsigned char, CGraphicAGV*> m_mapAGVs;			/*!< AGV图标指针 */
	map<unsigned char, CGraphicCharger*> m_mapChargers;	/*!< 充电器图标指针 */
	map<unsigned char, CGraphicRest*> m_mapRests;		/*!< 待机位图标指针 */
	map<unsigned short, CGraphicRFID*> m_mapRFIDs;		/*!< RFID卡图标指针 */
	map<unsigned char, CGraphicStation*> m_mapStations;	/*!< 工位图标指针 */
	map<unsigned char, CGraphicCaller*> m_mapCallers;	/*!< 呼叫器图标指针 */
	list<CGraphicTrack*> m_listTracks;					/*!< 磁轨图标指针 */

protected:
	CGraphicCharger* m_pTmpCharger;		/*!< 临时充电器指针 */
	CGraphicRest* m_pTmpRest;			/*!< 临时待机位指针 */
	CGraphicRFID* m_pTmpRFID;			/*!< 临时RFID卡指针 */
	CGraphicStation* m_pTmpStation;		/*!< 临时工位指针 */
	CGraphicTrack* m_pTmpTrack;			/*!< 临时磁轨指针 */
	CGraphicCaller* m_pTmpCaller;		/*!< 临时呼叫器指针 */

protected:
	bool m_bCreateNew;					/*!< 新建图标标识 */
	bool m_bRefresh;					/*!< 刷新标识 */
	//bool m_bEdit;						/*!< 编辑标识 */
	bool m_bMulti;						/*!< 多选标识 */
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;			
	ADOConn m_ado;						/*!< ADO数据库连接串对象 */
	//bool m_bSaveByADO;				/*!< 通过ADO数据库储存参数的标识 */
	string m_strADO;					/*!< 用以储存ADO配置信息的JSON字符串:{"Connect":"","User":"HarbinBrain","Password":"0451HarbinBrain58973970"} */
	void* m_pMainFram;					/*!< 主框架指针 */
	UpdateWidget m_funcUpdateWidget;	/*!< 通知界面图标参数更新的回调函数指针 */
	bool m_bInstall;					/*!< 初始化标识 */
	unsigned int m_unNewed;				/*!< 空间新增标识 */
	Bitmap *m_pclBmp;

public:
	/*!
	 * @brief 创建新图标
	 * @param const char* 记录图标参数的JSON字符串{"Type":"","Param":{"No":0,"Param":{}}}
	 * @return bool 创建成功返回true,否则返回false
	*/
	const bool CreateNewWidget(Json::Value& JsonValue);

	/*!
	 * @brief 查找图标
	 * @param const char* 记录图标参数的JSON字符串{"Type":"","Param":{}}
	 * @return bool 创建成功返回true,否则返回false
	*/
	const bool FindWidget(Json::Value& JsonValue);

	/*!
	 * @brief 编辑图标
	 * @param const char* 记录图标参数的JSON字符串{"Type":"","Param":{}}
	 * @return bool 创建成功返回true,否则返回false
	*/
	const bool EditWidget(Json::Value& JsonValue);

	/*!
	 * @breif 获取图标详细参数
	 * @param const char* 记录图标参数的JSON字符串{"Type":"","Param":{}}
	 * @return bool 创建成功返回true,否则返回false
	*/
	Json::Value GetWidget(Json::Value& JsonValue);
	Json::Value GetWidget(PointF ptWinpoint);
#ifdef _AFX
	Json::Value GetWidget(CPoint ptWinpoint);
#endif //_AFX

	/*!
	 * @brief 删除图标
	 * @param const char* 记录图标参数的JSON字符串
	*/
	const bool DeleteWidget(Json::Value& JsonValue);

	/*!
	 * @brief 拖拽图标
	*/
	const HCURSOR Drag(const PointF ptWinPoint);
#ifdef _AFX
	const HCURSOR Drag(const CPoint ptWinPoint);
#endif //_AFX

	/*!
	 * @brief 绘制
	 * @param pDC 窗口句柄
	*/
	void Draw(HDC hdc);

	/*!
	 * @brief 开启编辑模式
	*/
	void OpenEditMode();

	/*!
	 * @brief 关闭编辑模式
	*/
	void CloseEditMode();

	/*!
	 * @brief 缩小
	*/
	void ZoomedIn();

	/*!
	 * @brief 放大
	*/
	void ZoomedOut();

	/*!
	 * @brief 复位
	*/
	void Reset();

	/*!
	 * @brief 尺寸变更
	*/
	void OnSize(RectF rcClient);
#ifdef _AFX
	void OnSize(CRect rcClient);
#endif //_AFX

	/*!
	 * @brief 取消
	*/
	void Cancel();

	/*!
	 * @brief 初始化
	 * @return bool 未初始化返回true,否则返回false
	*/
	bool Init(void* pMainFram = nullptr, UpdateWidget func = nullptr);

	/*!
	 * @brief 选择图标
	*/
	void Select(const PointF ptWinpoint);
#ifdef _AFX
	void Select(const CPoint ptWinpoint);
#endif //_AFX

	/*!
	 * @brief 选择背景
	*/
	void SelectBackground(const PointF ptWinpoint);
#ifdef _AFX
	void SelectBackground(const CPoint ptWinpoint);
#endif //_AFX

	/*!
	 * @brief 确认
	*/
	void Confirm(const PointF ptWinpoint);
#ifdef _AFX
	void Confirm(const CPoint ptWinpoint);
#endif //_AFX

	void ConfirmBackground();

	/*!
	 * @brief 删除图标
	*/
	void DeleteWidget();

	/*!
	 * @brief 刷新
	*/
	void Refresh();

	/*!
	 * @brief 多选
	*/
	void MultiSelect();

	/*!
	 * @brief 单选
	*/
	void SignalSelect();

	/*!
	 * @brief 选择全部图标
	*/
	void SelectAll();

	/*!
	 * @brief 更新充电器状态
	 * @param const char* 记录充电器状态信息的JSON字符串
	*/
	void UpdateChargerStatus(Json::Value& JsonValue);

	/*!
	 * @brief 更新AGV状态信息
	 * @param const char* 记录AGV状态信息的JSON字符串
	*/
	void UpdateAGVStatus(Json::Value& JsonValue);

	/*!
	 * @brief 更新Caller状态信息
	 * @param const char* 记录AGV状态信息的JSON字符串
	*/
	void UpdateCallerStatus(Json::Value& JsonValue);

	/*!
	 * @brief 从JSON字符串中提取ADO配置信息
	 * @param const char* JSON字符串
	*/
	void JsonStringToADO(const char* strJson);
	void JsonStringToADO(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取ADO配置信息
	 * @param const char* JSON文件路径
	*/
	void JsonFileToADO(const char* strFile);
	void JsonFileToADO(const wchar_t* wstrFile);

	void SaveMap();
	void LoadMap();

protected:
	/*!
	 * @brief 释放临时图标
	*/
	void ReleaseTmpWidget();

	/*!
	 * @brief 释放图标
	*/
	void ReleaseWidget();

	/*!
	 * @brief 拖拽临时图标
	*/
	const HCURSOR DragTmpWidget(const PointF ptWinPoint);

	/*!
	 * @brief 拖拽图标
	*/
	const HCURSOR DragWidget(const PointF ptWinPoint);

	/*!
	 * @brief 确认临时图标
	*/
	void ConfirmTmpWidget();

	/*!
	 * @brief 确认图标
	*/
	void ConfirmWidget(const PointF ptWinpoint);

	/*!
	 * @brief 选择临时图标
	*/
	void SelectTmpWidget(const PointF ptWinpoint);

	/*!
	 * @brief 选择图标
	*/
	void SelectWidget(const PointF ptWinpoint);

	/*!
	 * @brief 绑定RFID卡
	*/
	void BindRFID(CGraphicAGV* pAGV, const unsigned short usRFID);

	/*!
	 * @brief 释放RFID
	*/
	void ReleaseRFID(CGraphicAGV* pAGV, const unsigned short usRFID);

	void DrawWidget(Graphics* grPtr);

	void DrawTmpWidget(Graphics* grPtr);

	/*!
	 * @brief 初始化ADO
	*/
	void InitADO();

	/*!
	 * @brief 通过数据库读取背景
	*/
	void LoadBackgroundByADO();

	/*!
	 * @brief 通过数据库储存背景
	*/
	void SaveBackgroundByADO();

	void JsonStringToAGV(const char* strJson);
	void JsonStringToRFID(const char* strJson);
	void JsonStringToStation(const char* strJson);
	void JsonStringToTrack(const char* strJson);
	void JsonStringToCaller(const char* strJson);
	void JsonStringToCharger(const char* strJson);
	void JsonStringToRest(const char* strJson);

	void SaveAGVToJsonString(Json::Value& json);
	void SaveRFIDToJsonString(Json::Value& json);
	void SaveStationToJsonString(Json::Value& json);
	void SaveTrackToJsonString(Json::Value& json);
	void SaveCallerToJsonString(Json::Value& json);
	void SaveChargerToJsonString(Json::Value& json);
	void SaveRestToJsonString(Json::Value& json);
};

#endif //!_MAP_H
