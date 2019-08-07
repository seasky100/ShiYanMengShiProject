/*!
 * @file ServiceFunction.h
 * @brief 实现调度系统业务功能的文件
 * @author FanKaiyu
 * @date 2019-04-15
 * @version 1.0
 *
 * update 2019-05-12 FanKaiyu 取消第2机械手往返2次机制
*/
#pragma once

#ifndef _SERVICEFUNCTION_H
#define _SERVICEFUNCTION_H

#include "BaseFunction.h"
#include "../MESDemo/CMES.h"
#include "../ZIGBEECallDemo/CZigbeeControler.h"
#include "../ZIGBEECallDemo/CZigbeeCaller.h"
#include "../ADO/ADOConn.h"
#include "CStation.h"
#include <map>
#include <list>

using namespace std;

/*!< @brief 加工工序 */
enum _MES_PROCESS
{
	_MES_NG,								/*!< NG工位 */
	_MES_BoxStorageArea = 1,				/*!< 箱体存放区 */
	_MES_BoxAccessorierInstallBuffer,		/*!< 箱体辅件安装缓存 */
	_MES_BoxAccessoriesInstall,				/*!< 箱体辅件安装:WP-001#1*/
	_MES_WaterPipesInstall,					/*!< 水管安装:WP-002#1 */
	_MES_LowerModuleLifting,				/*!< 下层模组吊装,第1机械臂:WP-003#1 */
	_MES_LowerModuleInstall,				/*!< 下层模组安装:WP-004#1 */
	_MES_UpperModuleLifting,				/*!< 上层模组吊装,第2机械臂:WP-005#1 */
	_MES_UpperModuleInstall,				/*!< 上层模组安装,往返2次:WP-006#1 */
	_MES_OfflineBDUAssembly,				/*!< 线外BDU组装:WP-007#1 */
	_MES_BoxBDUAssembly,					/*!< 箱体BDU组装:WP-008#1 */
	_MES_TheAuxiliaryControlPanelSwipes,	/*!< 预留工站1(从控板刷写) */
	_MES_Unknown2,							/*!< 预留工站2(未知) */
	_MES_EOLTest,							/*!< EOL测试:WP-009#1 */
	_MES_BatteryBoxTest1,					/*!< 电池包测试1:WP-010#1 */
	_MES_BoxCoverInstallBuffer,				/*!< 箱体上盖安装缓存 */
	_MES_BatteryBoxTest2,					/*!< 电池包测试2:WP-010#1 */
	_MES_BoxCoverInstall,					/*!< 箱体上盖安装:WP-011#1 */
	_MES_BoxAirTightTest,					/*!< 箱体气密测试:WP-012#1 */
	_MES_BoxUnloadBuffer,					/*!< 成品包下料缓存 */
	_MES_BoxUnload,							/*!< 成品包下料 */
};

#define _TASK_STATUS_NONE		"未执行"
#define _TASK_STATUS_PRE		"准备阶段"
#define _TASK_STATUS_EXE		"正在执行"
#define _TASK_STATUS_FINISH		"已完成"
#define _TASK_STATUS_CANCEL		"已取消"

#define _TASK_STATUS_NONE_W		L"未执行"
#define _TASK_STATUS_PRE_W		L"准备阶段"
#define _TASK_STATUS_EXE_W		L"正在执行"
#define _TASK_STATUS_FINISH_W	L"已完成"
#define _TASK_STATUS_CANCEL_W	L"已取消"

#define _TASK_TYPE_CALL			"呼叫"
#define _TASK_TYPE_PASS			"放行"
#define _TASK_TYPE_NG			"NG"
#define _TASK_TYPE_RETURN		"返回"
#define _TASK_TYPE_HANDFULL		"手动满载移动"
#define _TASK_TYPE_HANDEMPTY	"手动空载移动"

#ifndef TM_UPDATE_KEY
#define TM_UPDATE_KEY WM_USER+7		/*!< 更新权限信息的线程消息 */
#endif //!TM_UPDATE_KEY

/*!
 * @class ServiceFunction
 * @brief 实现调度系统业务功能的类
*/
class ServiceFunction :
	public BaseFunction
{
public:
	ServiceFunction();
	~ServiceFunction();

protected:
	CMES* m_pMes;									/*!< MES指针 */
	ADOConn m_ado;									/*!< 数据库ADO连接*/
	ZGBCtrler* m_pZGBCtrler;						/*!< Zigbee控制器指针 */
	map<unsigned char, CStation*> m_mapStations;	/*!< 工位组 */
	map<unsigned char, ZGBCaller*> m_mapCallers;	/*!< Zigbee呼叫器组 */
	bool m_bScream;									/*!< 全线急停标识 */
	bool m_bKey;									/*!< 权限标识 */
	
protected:
	string m_strMES;		/*!< 用以存放MES配置信息的JSON字符串:{"ProductLineNO":"PACK-Line-1","WorkPositionNo":{0:"",1:"WP-001#1",2:"WP-002#1",3:"WP-003#1",4:"WP-004#1",5:"WP-005#1",6:"WP-006#1",7:"WP-007#1",8:"WP-008#1",9:"WP-009#1",10:"WP-010#1",11:"WP-011#1",12:"WP-012#1",13:""}} */
	string m_strADO;		/*!< 用以储存ADO配置信息的JSON字符串:{"Connect":"","User":"HarbinBrain","Password":"0451HarbinBrain58973970"} */
	string m_strZigbee;		/*!< 用以储存ZIGBEE配置信息的JSON字符串:{"Control":{"No":1,"Port":0,"Baud":115200,"Parity":"N","DataBits":8,"StopBits":1},"Caller":[{"No":1,"Station":0,"PackOutBound":true},...]} */
	string m_strStation;	/*!< 用以储存工位配置信息的JSON字符串:{"Station":[{"No":1,"Name":"","Location":0,"Process":0},{},...]} */

protected:
	map<unsigned char, Json::Value*> m_mapStrUpdateCallers;	/*!< 用以存放呼叫器界面更新的JSON字符串 {"No":1,"PackOutBound":true,"Call":0,"CancelCall":0,"Pass":0,"CancelPass":0,"NG":0,"CancelNG":0,"Scream":0} */
	map<unsigned char, Json::Value*> m_mapStrUpdateTasks;	/*!< 用以存放任务界面更新的JSON字符串 */
	bool m_bUpdateCaller[UCHAR_MAX];

protected:
	bool AcceptClient(SOCKET socket);

	void Function();

	void ProcessAGVStatus(void* pUser);

	/*!
	 * @brief 初始化MES
	*/
	bool InitMES();

	/*!
	 * @brief 初始化ADO
	*/
	void InitADO();

	/*!
	 * @brief 初始化工位
	*/
	bool InitStations();

	/*!
	 * @brief 初始化ZIGBEE
	*/
	bool InitZigbee();
public:
	/*!
	 * @brief 初始化功能
	*/
	bool Init(UpdateStatus func, void* ptr);

	/*!
	 * @brief 从JSON字符串中提取MES配置信息
	 * @param const char* JSON字符串
	*/
	void JsonStringToMES(const char* strJson);
	void JsonStringToMES(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取MES配置信息
	 * @param const char* JSON文件路径
	*/
	void JsonFileToMES(const char* strFile);
	void JsonFileToMES(const wchar_t* wstrFile);

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

	/*!
	 * @brief 从JSON字符串中提取工位配置信息
	 * @param const char* JSON 字符串
	*/
	void JsonStringToStations(const char* strJson);
	void JsonStringToStations(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取工位信息
	 * @param const char* JSON文件路径
	*/
	void JsonFileToStations(const char* strFile);
	void JsonFileToStations(const wchar_t* wstrFile);

	/*!
	 * @brief 从JSON字符串中提取Zigbee配置信息
	 * @param const char* JSON字符串
	*/
	void JsonStringToZigbee(const char* strJson);
	void JsonStringToZigbee(const wchar_t* wstrJson);

	/*!
	 * @brief 从JSON文件中提取Zigbee配置信息
	 * @param const char* JSON文件路径
	*/
	void JsonFileToZigbee(const char* strFile);
	void JsonFileToZigbee(const wchar_t* wstrFile);

	/*!
	 * @brief 开启功能
	 * @param unsigned long 消息线程ID
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool Open(UpdateStatus func, void* ptr);

	/*!
	 * @brief 关闭功能
	*/
	void Close();

	/*!
	 * @brief 释放内存
	*/
	void Release();

	void ReleaseString();

	/*!
	 * @brief 权限设置
	 * @param bool 权限
	 * @return bool 设置成功返回true,否则返回false
	*/
	bool SetKey(bool bKey);

	/*!
	 * @brief 完成任务
	 * @param int 任务号
	 * @param const char* 任务备注
	 * @return bool 删除成功返回true,否则返回false
	*/
	bool FinishTask(const int nNo, const char* _Format, ...);
	bool FinishTask(const int nNo, const wchar_t* _Format, ...);
	bool CancelTask(const int nNo, const char* _Format, ...);
	bool CancelTask(const int nNo, const wchar_t* _Format, ...);
	bool CancelTask(const unsigned char byAGVNo, const char* strType, const char* _Format, ...);
	bool CancelTask(const unsigned char byAGVNo, const wchar_t* strType, const wchar_t* _Format, ...);

	/*!
	 * @brief 创建任务
	 * @param unsigned char 任务起始工站编号
	 * @param unsigend char 任务终止工站编号
	 * @param unsigned char 执行任务AGV编号
	 * @return bool 创建成功返回true
	*/
	bool CreateTask(const unsigned char byStartWP, const unsigned char byEndWP, unsigned char byExeAGV, const char* strType);

	/*!
	 * @brief 用户控制AGV
	 * @param const char* JSON字符串
	 * @return int 返回操作结果的码值
	*/
	int ControlAGV(Json::Value& JsonValue);

protected:
	/*!
	 * @brief PACK出库校验
	 * @param const char* 工站编号
	 * @return bool 校验成功返回true,否则返回false
	*/
	bool PackOutBound(const char* strWorkPosition);

	/*!
	 * @brief 放行
	 * @param unsigned char 工序步骤
	 * @param unsigned short 坐标
	 * @param const char* 工站编号
	*/
	void Pass(unsigned char byProcess, unsigned short usRFID);

	/*!
	 * @brief 呼叫
	 * @param unsigned char 工序步骤
	 * @param unsigned short 坐标
	*/
	void Call(unsigned char byProcess, unsigned short usRFID);

	/*! 
	 * @brief NG
	 * @param unsigned char 工序步骤
	 * @param unsigned short 坐标
	*/
	void NG(unsigned char byProcess, unsigned short usRFID);

	/*!
	 * @brief 全站急停
	*/
	void Scream();

	/*!
	 * @brief 取消放行
	 * @param unsigned char 工序步骤
	 * @param unsigned short 坐标
	*/
	void CancelPass(unsigned char byProcess, unsigned short usRFID);

	/*!
	 * @brief 取消呼叫
	 * @param unsigned char 工序步骤
	 * @param unsigned short 坐标
	*/
	void CancelCall(unsigned char byProcess, unsigned short usRFID);

	/*!
	 * @brief 取消NG
	 * @param unsigned char 工序步骤
	 * @param unsigned short 坐标
	*/
	void CancelNG(unsigned char byProcess, unsigned short usRFID);

	/*!
	 * @brief 取消急停
	*/
	void CancelScream();

	/*!
	 * @brief 获取空闲AGV
	 * @param unsigned char 工序步骤
	 * @param unsigned char 工位
	*/
	CAGV* GetFreeAGV(CStation* pStation);

	/*!
	 * @brief 删除任务
	 * @param int 任务号
	 * @param const char* 任务备注
	 * @return bool 删除成功返回true,否则返回false
	*/
	bool DeleteTask(const int nNo);

	/*!
	 * @brief 更新任务备注
	*/
	bool UpdateTaskText(const int nNo, const char* _Format, ...);
	bool UpdateTaskText(const int nNo, const wchar_t* _Format, ...);

	/*!
	 * @brief 执行任务
	*/
	bool ExecuteTask(const int nTaskNo, const unsigned char byAGVNo,const char* strStatus);

	/*!
	 * @brief 获取呼叫器
	 * @param unsigned char 工序
	 * @return list<ZGBCaller*> 与此工序绑定的呼叫器列表
	*/
	list<ZGBCaller*> GetCaller(const unsigned char byProcess);

	/*!
	 * @brief 获取呼叫器
	 * @param unsigned short RFID卡
	 * @return ZGBCaller* 与此RFID卡绑定的呼叫器
	*/		
	ZGBCaller* GetCaller(const unsigned short usRFID);

	/*!
	 * @brief 获取呼叫器
	 * @param unsigned char 工位编号
	 * @return ZGBCaller* 与此RFID卡绑定的呼叫器
	*/
	ZGBCaller* GetCallerS(const unsigned char byStation);

	/*!
	 * @brief 获取工站
	 * @param unsigned char 工序
	 * @return list<CStation*> 与此工序绑定的工站列表
	*/
	list<CStation*> GetStation(const unsigned char byProcess);

	/*!
	 * @brief 获取工位
	 * @param unsigned short RFID卡
	 * @return CStation* 与此RFID卡绑定的工站
	*/
	CStation* GetStation(const unsigned short usRFID);

	/*!
	 * @brief AGV任务处理
	*/
	void ProcessAGVTask();

	/*!
	 * @brief 处理呼叫任务
	 * @param int 任务编号
	 * @param unsigned char 任务起始工位
	 * @param unsigned char 任务终止工位
	 * @param unsigned char 执行任务AGV编号
	 * @param unsigned char 任务状态
	*/
	void ProcessAGVCallTask(const int nTaskNo,const unsigned char byStart,const unsigned char byEnd,const unsigned char byAGVNo,const char* strStatus);

	/*!
	 * @brief 处理NG任务
	 * @param int 任务编号
	 * @param unsigned char 任务起始工位
	 * @param unsigned char 任务终止工位
	 * @param unsigned char 执行任务AGV编号
	 * @param unsigned char 任务状态
	*/
	void ProcessAGVNGTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus);

	/*!
	 * @brief 处理放行任务
	 * @param int 任务编号
	 * @param unsigned char 任务起始工位
	 * @param unsigned char 任务终止工位
	 * @param unsigned char 执行任务AGV编号
	 * @param unsigned char 任务状态
	*/
	void ProcessAGVPassTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus);

	/*!
	 * @brief 处理返回任务
	 * @param int 任务编号
	 * @param unsigned char 任务起始工位
	 * @param unsigned char 任务终止工位
	 * @param unsigned char 执行任务AGV编号
	 * @param unsigned char 任务状态
	*/
	void ProcessAGVReturnTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus);

	/*!
	 * @brief 处理手动满载任务
	 * @param int 任务编号
	 * @param unsigned char 任务起始工位
	 * @param unsigned char 任务终止工位
	 * @param unsigned char 执行任务AGV编号
	 * @param unsigned char 任务状态
	*/
	void ProcessAGVHandFullTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus);

	/*!
	 * @brief 处理手动任务
	 * @param int 任务编号
	 * @param unsigned char 任务起始工位
	 * @param unsigned char 任务终止工位
	 * @param unsigned char 执行任务AGV编号
	 * @param unsigned char 任务状态
	*/
	void ProcessAGVHandEmptyTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus);

	/*!
	 * @brief 呼叫器信号处理
	*/
	void ProcessCallSignal();

	/*!
	 * @brief 处理Zigbee读取的报文
	*/
	void ProcessZigbeePackage();

	CAGV* ApplyForBatteryBoxTest1(CStation* pStation);
	CAGV* ApplyForBatteryBoxTest2(CStation* pStation);
	CAGV* ApplyForBoxCoverInstallBuffer(CStation* pStation);
	CAGV* ApplyForEolTest(CStation* pStation);
	CAGV* ApplyForNG(CStation* pStation);
	CAGV* ApplyForOther(CStation* pStation);

	/*!
	 * @brief 从工序上获取空闲AGV
	 * @param CStation* 需求AGV的工位指针
	 * @param unsigned char 检索的工序
	 * @param bool& 当没有可用AGV时，此标志位的值代表是否有AGV即将到达工序
	 * @return CAGV* 返回可用的AGV
	*/
	CAGV* CatchAGVFromProcess(CStation* pStation,const unsigned char byProcess,bool& bContinue);
	CAGV* CatchAGVFromNG(CStation* pStation);
	CAGV* CatchAGVFromOther(CStation* pStation);

	int DoesAGVHaveATask(unsigned char byNo);
	CAGV* IsAGVComing(CStation *pStation);
	CAGV* IsAGVRunning(CStation* pStation);

protected:
	/*!
	 * @brief 更新呼叫器状态信息
	*/
	void UpdateZGBCaller(ZGBCaller* pCaller);

	void RecordAGVStatus();
	void RecordCallerStatus();
};

#endif //!_SERVICEFUNCTION_H