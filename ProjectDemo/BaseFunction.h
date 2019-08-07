/*!
 * @file BaseFunction.h
 * @brief 实现调度系统基础功能的文件
 * @author FanKaiyu
 * @date 2019-04-09
 * @version 2.2
*/
#pragma once
#ifndef _BASEFUNCTION_H
#define _BASEFUNCTION_H

#include "../AGVDemo/CAGV.h"
//#include "../AGVDemo/CAGV.cpp"
#include "CRFID.h"
#include "CTraffiic.h"
#include <map>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

typedef bool(__stdcall* UpdateStatus)(Json::Value*, void*);

/*!
 * @class BeseFunction
 * @brief 实现调度系统基本功能的类
*/
class BaseFunction
{
public:
	BaseFunction();
	~BaseFunction();

protected:
	TCPServer m_tcpServer;								/*!< 服务端 */
	map<unsigned char, CAGV*> m_mapAGVs;				/*!< AGV数组 */
	map<unsigned short, Traffic*> m_mapTraffics;		/*!< 交通管制数组 */

	RFID m_aRFIDArr[USHORT_MAX];						/*!< RFID卡数组 */
	unsigned short m_ausExhange[USHORT_MAX];			/*!< 转换RFID卡编号的数组 */
	bool m_bUpdateAGV[UCHAR_MAX];						/*!< 更新AGV数据的数组 */

protected:
	string m_strAGVs;									/*!< 用以储存AGV配置信息的JSON字符串 {"AGV":[{"No":1,"ServerIP":"","ServerPort":0,"ClientIP":"","ClientPort":-1,"CurRFID":0},{...}]} */
	string m_strServer;									/*!< 用以储存服务端配置信息的JSON字符串 {"IP":"","Port":0} */
	string m_strTraffic;								/*!< 用以储存交通管制配置信息的JSON字符串 {"Traffic":[{"Control":1,"Stop":[2,3,4,5],"Compare":[6,7,8,9]},{...}]} */
	string m_strExchange;

protected:
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;

	mutex m_mutexJson;
	HANDLE m_hShutdownEvent;							/*!< 关闭事件 */

	thread* m_pThreadMain;								/*!< 主线程 */

	UpdateStatus m_pFunc;
	void* m_pPtr;

protected:
	Json::Value*m_pStrUpdateServer;							/*!< 用以存放服务端界面更新的JSON字符串 {"IP":"","Port":"","Listen":true}*/
	map<unsigned short, Json::Value*> m_mapStrUpdateAGVs;	/*!< 用以存放AGV界面更新的JSON字符串 {"No":1,"Connect":true,"RFID":{"Cur":0,"Next":0,"End":0},"Message":{"IP":"","Port":"","Status":{"Value":"","Color":""},"Speed":"","Power":{"Value":"","Color":""},"Cargo":{"Value":"","Color":""},"Lifter":"","Error":{"Value":"","Color":""},"Action":"",},"View":{"Obs":false,"Run":false,"Unvol":true,"Cargo":false,"Error":false,"Action":0}}*/

public:
	static mutex g_mutex;

public:
	/*!
	 * @brief 初始化程序
	*/
	virtual bool Init(UpdateStatus func, void* ptr);

	/*!
	 * @brief 启动程序
	 * @param unsigned long 消息线程ID
	 * @return bool 启动成功返回true,否则返回false
	*/
	virtual bool Open(UpdateStatus func, void* ptr);

	/*!
	 * @brief 关闭程序
	*/
	virtual void Close();

	/*!
	 * @brief 释放程序内存
	*/
	virtual void Release();

	virtual void ReleaseString();

	/*!
	 * @brief 从JSON字符串中提取服务端配置信息
	 * @param const char* JSON字符串
	*/
	void JsonStringToServer(const char* strJson);

	/*!
	 * @brief 从JSON字符串中提取服务端配置信息
	 * @param const wchar_t* JSON字符串
	*/
	void JsonStringToServer(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取服务端配置信息
	 * @param const char* JSON文件路径
	*/
	void JsonFileToServer(const char* strFile);

	/*!
	 * @brief 从JSON文件中提取服务端配置信息
	 * @param const wchar_t* JSON文件路径
	*/
	void JsonFileToServer(const wchar_t* wstrFile);

	/*!
	 * @brief 从JSON字符串中提取AGV配置信息
	 * @param const char* JSON字符串
	*/
	void JsonStringToAGVs(const char* strJson);

	/*!
	 * @brief 从JSON字符串中提取AGV配置信息
	 * @param const wchar_t* JSON字符串
	*/
	void JsonStringToAGVs(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取AGV配置信息
	 * @param const char* JSON文件路径
	*/
	void JsonFileToAGVs(const char* strFile);

	/*!
	 * @brief 从JSON文件中提取AGV配置信息
	 * @param const wchar_t* JSON文件路径
	*/
	void JsonFileToAGVs(const wchar_t* wstrFile);

	/*!
	 * @brief 从JSON字符串中提取交通管制卡配置
	 * @param const char* JSON字符串
	*/
	void JsonStringToTraffics(const char* strJson);

	/*!
	 * @brief 从JSON字符串中提取交通管制卡配置
	 * @param const wchar_t* JSON字符串
	*/
	void JsonStringToTraffics(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取交通管制卡配置
	 * @param const char* JSON文件路径
	*/
	void JsonFileToTraffics(const char* strFile);

	/*!
	 * @brief 从JSON文件中提取交通管制卡配置
	 * @param const wchar_t* JSON文件路径
	*/
	void JsonFileToTraffics(const wchar_t* wstrFile);

	/*!
	 * @brief 从JSON字符串中提取RFID转换配置
	 * @param const char* JSON字符串
	*/
	void JsonStringToRFIDExchanges(const char* strJson);

	/*!
	 * @brief 从JSON字符串中提取RFID转换配置
	 * @param const wchar_t* JSON字符串
	*/
	void JsonStringToRFIDExchanges(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取RFID转换配置
	 * @param const char* JSON文件路径
	*/
	void JsonFileToRFIDExchanges(const char* strFile);

	/*!
	 * @brief 从JSON文件中提取RFID转换配置
	 * @param const wchar_t* JSON文件路径
	*/
	void JsonFileToRFIDExchanges(const wchar_t* wstrFile);

protected:
	/*!
	 * @brief 客户端接入
	 * @param SOCKET 客户端SOCKET通道
	*/
	void Accept(SOCKET socket);

	/*!
	 * @brief 初始化服务端
	*/
	void InitServer();

	/*!
	 * @brief 初始化RFID
	*/
	void InitTraffic();

	/*!
	 * @brief 初始化AGV
	*/
	bool InitAGVs();

	bool InitExchangeRFID();
	
	/*!
	 * @brief 处理AGV接收的报文
	*/
	void ProcessAGVPackage();

	/*!
	 * @brief 交通管制控制
	*/
	void TrafficControl();

	/*!
	 * @brief 交通管制控制
	 * @param void* 接受控制的对象指针
	*/
	void TrafficControl(void* pUser);

	/*!
	 * @brief 避障控制
	 * @param void* 接受控制的对象指针
	*/
	void ObsControl(void* pUser);

	/*!
	 * @brief 接收AGV连接请求
	 * @bool 连接成功返回true,否则返回false
	*/
	bool AcceptAGV(SOCKET socket);
	void ConnectAGV();

	/*!
	 * @brief 接收客户端连接请求
	 *
	 * 可重载
	 * @bool 连接成功返回true,否则返回false
	*/
	virtual bool AcceptClient(SOCKET socket);

	/*!
	 * @brief AGV状态处理
	 * @param void* 接受控制的对象指针
	*/
	virtual void ProcessAGVStatus(void* pUser);

	const unsigned short ExchangeRFID(const unsigned short usRFID);

protected:
	/*!
	 * @brief 主线程
	 *
	 * 用于实现程序功能
	*/
	void MainThread();

	/*!
	 * @brief 启动主线程
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartMainThread();

	/*!
	 * @brief 关闭主线程
	*/
	void EndMainThread();

	/*!
	 * @brief 程序功能
	 *
	 * 可重载
	*/
	virtual void Function();

protected:
	static void UpdateAGV(void* pAGV,void* ptr);
	static void UpdateServer(void* pServer, void* ptr,SOCKET socket);

	/*!
	 * @brief 更新AGV状态信息
	*/
	void UpdateAGV(CAGV* pAGV);

	/*!
	 * @brief 更新服务端状态信息
	*/
	void UpdateServer(TCPServer& server);
};

#endif //!_BASEFUNCTION_H