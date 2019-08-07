/*!
 * @file CAGV.h
 * @brief 描述AGV基本属性与功能的文件
 * @author FanKaiyu
 * @date 2019-04-04
 * @version 2.2
*/
#pragma once
#ifndef _AGV_H
#define _AGV_H

#include "../json/json.h"
#include "../CommunicateDemo/CTCP.h"
#include "../String/Buffer.h"
#include <list>

using namespace std;

/*!< AGV电量状态 */
enum _AGVPOWER
{
	_AGVPOWER_EMPTY = 0,		/*!< 未检测到电池 */
	_AGVPOWER_LOW = 20,			/*!< 低电量 */
	_AGVPOWER_NORMAL = 50,		/*!< 正常电量 */
	_AGVPOWER_HIGH = 90,		/*!< 高电量 */
	_AGVPOWER_FULL = 100,		/*!< 满电*/
};

/*!< AGV运行状态 */
enum _AGVSTATUS
{
	_AGVSTA_WAIT,		/*!< 待机状态.\
						上位机接收到此状态,认为AGV已经准备好并可以接收上位机的控制调度.\
						此时下位机可以接收并处理上位机发送的所有类型的报文.\
						*/
	_AGVSTA_RUN,		/*!< 运行状态.\
						上位机接收到此状态,认为AGV在移动或动作中.此时上位机除了会请求心跳状态外不做任何处理.\
						此时下位机除了处理上位机发出的心跳报文,其他报文均可以不处理.但移动或动作结束后必须将AGV状态恢复为待机状态.\
						*/
	_AGVSTA_STOP,		/*!< 停止状态.\
						上位机接收到此状态,认为AGV被人为停止.属于手动操作;此时上位机除了会请求心跳状态外不做任何处理.\
						此时下位机除了处理上位机发出的心跳报文,其他报文均可以不处理.用户可以手动控制AGV,但用户操作结束后必须将AGV状态恢复为待机状态.\
						*/
	_AGVSTA_SCREAM,		/*!< 急停状态.\
						上位机接收到此状态,认为AGV被紧急停止运行,属于手动操作.此时上位机会上报警告至用户,并且除了心跳状态外不做任务处理.\
						此时下位机除了处理上位机发出的心跳报文,其他报文均可以不处理.但用户操作结束后必须将AGV状态恢复为待机状态.
						*/
	_AGVSTA_FIND,		/*!< 寻磁/定位状态.\
						上位机接收到此状态,认为AGV脱离轨道.此时上位机除了会请求心跳状态外不做任何处理.\
						此时下位机除了处理上位机发出的心跳报文,其他报文均可以不处理.下位机应启动寻磁/定位机制,在一段时间内寻磁/定位失败后,应将AGV状态置为停止,并将异常状态置为脱磁/无法定位.\
						*/
	_AGVSTA_OBSDOWN,	/*!< 非接触式避障减速状态.\
						上位机接收到此状态,认为AGV触发非接触式避障并处于减速运行状态,此时上位机除了会请求心跳状态外不做任何处理.\
						此时下位机除了处理上位机发出的心跳报文,其他报文均可以不处理.\
						*/
	_AGVSTA_OBSSTOP,	/*!< 非接触式避障停止状态.\
						上位机接收到此状态,认为AGV触发非接触式避障并处于减速运行状态,此时上位机除了会请求心跳状态外不做任何处理.\
						此时下位机除了处理上位机发出的心跳报文,其他报文均可以不处理.\
						*/
	_AGVSTA_MOBSSTOP,	/*!< 接触式避障停止状态.\
						上位机接收到此状态,认为AGV触发接触式避障并处于停止状态,此时上位机上报异常至用户,并且除了会请求心跳状态外不做任何处理.\
						此时下位机除了处理上位机发出的心跳报文,其他报文均可以不处理.\
						*/
	_AGVSTA_TRAFFICSTOP,/*!< 交通管制停止状态.\
						上位机接收到此状态,认为AGV处于交通管制点等待上位机放行,此时上位机根据交通管制规则决定此状态的AGV是否允许通行.\
						此时下位机除了处理上位机发出的心跳报文以及交通管制控制报文,其他报文均可以不处理.\
						*/
	_AGVSTA_ARM,		/*!< 机械手反馈.\
						上位机接收到此状态,任务机械手已经对AGV操作完成.
						*/
	_AGVSTA_ALLSCREAM, /*!< 全线急停 */
	_AGVSTA_TRAFFICPASS,/*!< 交通管制放行状态 */
};

/*! @brief AGV动作信息码 */
enum _AGVACTION
{
	_AGVACT_NONE,		/*!< 无/停止动作.
						未执行状态:AGV收到此动作,若AGV正在执行其他动作,则停止动作的执行.
						执行状态:不处理.
						执行完成状态:不处理.
						*/
	_AGVACT_MOVE,		/*!< 移动动作
						未执行状态:AGV收到此动作,则AGV开始移动至终止地标卡.若AGV当前坐标与终止坐标相同,则不用移动.将动作状态置为执行完成.否则将动作状态置为执行中.
						执行状态:若AGV当前坐标与终止坐标相同,则不用移动,将动作状态置为执行完成.
						执行完成状态:不处理.
						*/
	_AGVACT_REMOVE,		/*!< 重新移动动作
						未执行状态:AGV收到此动作,则AGV重新移动至终止坐标.若AGV因当前坐标与终止坐标相同不能移动时,则AGV可将当前坐标重置后重新移动.将动作状态置为执行中.
						执行状态:若AGV当前坐标与终止坐标相同,则不用移动.将动作状态置为执行完成.
						执行完成状态:不处理.
						*/
	_AGVACT_TRAFFIC,	/*!< 交通管制动作
						未执行状态:AGV收到此动作,则AGV在终止地标卡等待交通管制.
						执行状态:不处理.
						执行完成状态:AGV收到此动作,则AGV可以离开终止地标卡.
						*/
	_AGVACT_LIFTUP,		/*!< 升降杆上升动作.
						未执行状态:对移载式AGV而言是辊筒执行上料动作;
								对于潜入式或牵引式AGV而言是升降杆执行上升动作;
								对叉车式AGV而言是货叉执行升起动作;
								对机械手AGV而言是机械手执行抓取动作,当机械手AGV在同一个点需要完成多个不同的动作时,通过增加动作码执行相应的动作;
								执行动作前AGV应判断是否处于可执行此动作的指定位置上.若不在,则应将当前地标卡置为0并上报异常.
								若开始执行动作,则将动作状态置为执行中.
						执行状态:动作完成后将动作状态置为执行完成.拥有货料检测功能的AGV应在确认货料已存放至AGV上后再将动作状态置为执行完成.
						执行完成状态:不处理.
						*/
	_AGVACT_LIFTDOWN,	/*!< 升降杆下降
						未执行状态:对移载式AGV而言是辊筒执行下料动作;
								对于潜入式或牵引式AGV而言是升降杆执行下降动作;
								对叉车式AGV而言是货叉执行下降动作;
								对机械手AGV而言是机械手执行下放动作,当机械手AGV在同一个点需要完成多个不同的动作时,通过增加动作码执行相应的动作;
								执行动作前AGV应判断是否处于可执行此动作的指定位置上.若不在,则应将当前地标卡置为0并上报异常.
								若开始执行动作,则将动作状态置为执行中.
						执行状态:动作完成后将动作状态置为执行完成.拥有货料检测功能的AGV应在确认货料已离开AGV后再将动作状态置为执行完成.
						执行完成状态:不处理.
						*/
};

/*! @brief AGV动作状态信息码 */
enum _AGVACTIONSTATUS
{
	_AGVACTSTA_NONE,	/*!< 动作未执行状态 */
	_AGVACTSTA_EXE,		/*!< 动作执行中状态 */
	_AGVACTSTA_FINISH,	/*!< 动作执行完成状态 */
	_AGVACTSTA_PAUSE,	/*!< 动作执行暂停状态 */
};

/*! @brief AGV异常信息码*/
enum _AGVERROR
{
	_AGVERR_NONE,		/*!< 无/正常
						说明:AGV运行正常.
						上位机:不处理.
						下位机:不处理.
						*/
	_AGVERR_MISS,		/*!< 脱磁/无法定位
						说明:提醒用户将AGV恢复至轨道上,防止因脱磁造成的运行流程卡顿.
						上位机:上报异常至用户.
						下位机:变更状态为停止.
						*/
	_AGVERR_PATH,		/*!< 路径异常
						说明:因上位机发送的移动指令错误（即没有对应的移动路径）引起的.
						上位机:上报异常至用户.
						下位机:变更状态为停止.
						*/
	_AGVERR_LOCATE,		/*!< 坐标异常
						说明:因AGV未处于上位机动作的指定位置引起的.上位机应发送重新移动或移动动作,使AGV重新到达指定位置,保证执行动作的安全性.
						上位机:上报异常至用户.
						下位机:将当前做坐标重置.
						*/
	_AGVERR_DIRECTION,	/*!< 定向异常
						说明:因缺少相对朝向引起的异常。
						上位机:上报异常至用户.
						下位机:变更状态为停止.
						*/
	_AGVERR_LIFTUP,		/*!< 顶升异常 */
	_AGVERR_ACT = -2,	/*!< 动作异常
						说明:由PC上位机检测出因执行某一动作长时间未完成引起的.
						上位机:上报异常至用户.
						下位机:不处理.
						*/
	_AGVERR_COMM,		/*!< 通信异常
						说明:由PC上位机检测出因无法发送报文或长时间未接收报文引起的.
						上位机:上报异常至用户.
						下位机:不处理.
						*/
};

/*! @brief 升降杆状态 */
enum _AGVLIFTER
{
	_AGVLIFTER_DOWN,	/*!< 落下 */
	_AGVLIFTER_UP,		/*!< 升起 */
};

/*! @brief 载货状态 */
enum _AGVCARGO
{
	_AGVCARGO_EMPTY = 0,	/*!< 空载 */
	_AGVCARGO_FULL = 255,	/*!< 满载 */
};

/*!< @brief 操作错误码 */
enum _CTRLERROR
{
	ERROR_NONE,			/*!< 操作成功 */
	ERROR_END,			/*!< 终点错误 */
	ERROR_CONNECT,		/*!< 连接错误 */
	ERROR_BATTERY,		/*!< 电池错误 */
	ERROR_STATUS,		/*!< 状态错误 */
	ERROR_INTERNET,		/*!< 网络错误 */
};

typedef void(__stdcall* UpdateAGV)(void*, void*);

/*!
 * @class CAGV
 * @brief 基于SOCKET通信,采用RFID卡定位的AGV类
*/
class CAGV :
	public TCPClient
{
public:
	CAGV();
	CAGV(unsigned char byNo, UpdateAGV funcPrt = nullptr, void* ptr = nullptr);
	CAGV(unsigned char byNo, UpdateAGV funcPrt, void* ptr,const char* strSrvIP, const char* strLocalIP, int nSrvPort = -1, int nLocalPort = -1);
	CAGV(unsigned char byNo, UpdateAGV funcPrt, void* ptr, const wchar_t* wstrSrvIP, const wchar_t* wstrLocalIP, int nSrvPort = -1, int nLocalPort = -1);
	CAGV(unsigned char byNo, UpdateAGV funcPrt, void* ptr, const string strLocalIP, string strSrvIP = "", int nLocalPort = -1, int nSrvPort = -1);
	~CAGV();

protected:
	unsigned char m_byNo;					/*!< 编号 */

protected:
	unsigned char m_byStatus;				/*!< 状态 */
	char m_chSpeed;							/*!< 速度 0 - 100前进速度 -0 - -100后退速度 */
	unsigned char m_byPower;				/*!< 电量 0%-100% 0%没电,1%-20%电压不足,20%-90%正常电,90%-100%满电 */
	unsigned short m_usCurRFID;				/*!< 行进当前RFID */
	unsigned short m_usOldRFID;				/*!< 行进上一RFID */
	unsigned short m_usNextRFID;			/*!< 行进下一RFID*/
	unsigned short m_usEndRFID;				/*!< 移动终点RFID */
	unsigned char m_byCargo;				/*!< 载货状态 0-255 0空载 255满载 */
	unsigned char m_byLifter;				/*!< 升降杆状态 0落下 1升起 */
	char m_chError;							/*!< 异常信息 */
	unsigned short m_usAction;				/*!< 执行的动作 0无动作 1移动 2重新移动 3交通管制 4升降杆上升 5升降杆下降 */
	unsigned char m_byActStatus;			/*!< 动作状态  0未执行 1执行中 2执行完成 3暂停执行*/

protected:
	thread *m_pThreadSend;					/*!< 数据发送线程 */
	DataBuffer m_bufferReceive;				/*!< 缓存区 用以储存未处理的数据 */
	HANDLE m_hShutdownEvent;				/*!< 关闭事件 */
	list<DataBuffer> m_listSingle;			/*!< 单次发送的数据列表 */
	list<DataBuffer> m_listWhile;			/*!< 循环发送的数据列表 */
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;
	void* m_pFunc;
	UpdateAGV m_funcUpdateAGV;
	thread* m_pThreadConnect;				/*!< 连接线程 */
	HANDLE m_hDestroyEvent;					/*!< 关闭事件 */
	std::mutex m_mutexLock;					/*!< 数据发送互斥锁 */

protected:
	static list<DataBuffer> m_listProcess;	/*!< 待处理的数据报文列表 */
	static mutex m_mutexProcess;			/*!< 待处理数据互斥锁 */

public:
	/*!
	 * @brief 移动
	 * 
	 * 控制AGV移动至指定的RFID卡
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	 * @param unsigend short 移动终点RFID卡编号
	*/
	int MoveTo(const unsigned short usRFID);

	/*!
	 * @brief 重新移动
	 * 
	 * 控制AGV重新移动至已到达的RFID卡
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	*/
	int RemoveTo();

	/*!
	 * @brief 交通管制通行
	 *
	 * 控制AGV自指定的RFID卡上通行
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	 * @param unsigned short 通行RFID卡编号
	*/
	int TrafficPass(const unsigned short usRFID);

	/*!
	 * @brief 交通管制停止
	 *
	 * 控制AGV至指定的RFID卡上时停止
	 * AGV接收到指令后需反馈至程序
	 * {"Message={},Result="Sucess/Failed"}
	 * @param unsigned short 停止RFID卡编号
	*/
	int TrafficStop(const unsigned short usRFID);

	/*!
	 * @brief 关机
	 *
	 * 控制AGV关闭电源
	 * AGV接收到指令后无需反馈至程序
	*/
	int Shutdown();

	/*!
	 * @brief 急停
	 *
	 * 控制AGV急停
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	*/
	int Scream();

	/*!
	 * @brief 复位
	 *
	 * 控制AGV复位(恢复急停等状态,使AGV重新接收程序控制)
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	*/
	int Reset();

	/*!
	 * @brief 重置
	*/
	int Restart();

	/*!
	 * @brief 升降杆上升
	 *
	 * 控制AGV升降杆上升
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	 * @param unsigend short 执行动作的RFID卡编号
	*/
	int LiftUp(const unsigned short usRFID);

	/*!
	 * @brief 升降杆下降
	 *
	 * 控制AGV升降杆下降
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	 * @param unsigend short 执行动作的RFID卡编号
	*/
	int LiftDown(const unsigned short usRFID);

	/*!
	 * @brief 停止
	 *
	 * 控制AGV停止当前执行的动作(包含移动等动作在内)
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	*/
	int Stop();

	/*!
	 * @brief 动作控制
	 *
	 * 控制AGV执行其他未在程序内设定的动作
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	 * @param unsigned short 执行动作的起始RFID卡编号
	 * @param unsigend short 执行动作的结束RDIF卡编号
	 * @param unsigend short 执行的动作
	*/
	int Action(const unsigned short usStartRFID,const unsigned short usEndRFID,const unsigned short usAction);

	/*!
	 * @brief 状态设置
	 *
	 * 设置AGV的当前RFID卡编号、运行状态、速度
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	 * @param unsigned short 当前RFID卡编号
	 * @param unsigned char 当前状态
	 * @param char 当前速度
	*/
	int StatusSet(const unsigned short usRFID, const unsigned char byStatus,const char chSpeed);

	/*!
	 * @brief 暂停
	 *
	 * 暂停AGV当前正在执行的动作
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	*/
	int Pause();

	/*!
	 * @brief 继续
	 *
	 * 继续AGV正在执行的动作
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	*/
	int Continue();

	/*!
	 * @brief 调整速度
	 *
	 * 调整AGV当前速度
	 * AGV接收到指令后无需反馈至程序
	 * 程序通过AGV反馈的状态、执行的动作、移动的终点RFID等判断指令是否被AGV执行
	 * @param char 当前最大速度%
	*/
	int SpeedControl(char chSpeed);

	/*!
	 * @brief 全线急停
	*/
	int AllScream();

public:
	/*!
	 * @brief 获取当前状态
	 * @return unsigned char 当前状态
	*/
	const unsigned char GetStatus() const;

	/*!
	 * @brief 获取行进当前RFID卡编号
	 * @return unsigned short 行进当前RFID卡编号
	*/
	const unsigned short GetCurRFID() const;

	/*!
	 * @brief 获取行进上一RFID卡编号
	 * @return unsigned short 行进上一RFID卡编号
	*/
	const unsigned short GetOldRFID() const;

	/*!
	 * @brief 获取行进下一RFID卡编号
	 * @return unsigend short 行进下一RFID卡编号
	*/
	const unsigned short GetNextRFID() const;

	/*!
	 * @brief 获取行进终点RFID卡编号
	 * @return unsigned short 行进终点RFID卡编号
	*/
	const unsigned short GetEndRFID() const;

	/*!
	 * @brief 获取当前速度
	 * @return char 当前速度
	*/
	const char GetSpeed() const;

	/*!
	 * @brief 获取当前电量
	 * @return unsigned char 当前电量
	*/
	const unsigned char GetPower() const;

	/*!
	 * @brief 获取当前载货状态
	 * @return unsigned char 当前载货状态
	*/
	const unsigned char GetCargo() const;

	/*!
	 * @brief 获取当前升降杆状态
	 * @return unsigned char 当前升降杆状态
	*/
	const unsigned char GetLifter() const;

	/*!
	 * @brief 获取当前异常
	 * @return char 当前异常
	*/
	const char GetError() const;

	/*!
	 * @brief 获取当前动作
	 * @return unsigned short 当前动作
	*/
	const unsigned short GetAction() const;

	/*!
	 * @breif 获取当前动作状态
	 * @return unsigned char 当前动作状态
	*/
	const unsigned char GetActionStatus() const;

public:
	/*!
	 * @brief 更新行进当前RFID卡编号
	 * @param const unsigned short 新RFID卡编号
	 * @return const unsigned short 旧RFID卡编号
	*/
	const unsigned short UpdateCurRFID(const unsigned short usRFID);

	/*!
	 * @brief 更新行进下一RFID卡编号
	 * @param const unsigned short 新RFID卡编号
	 * @return const unsigned short 旧RFID卡编号
	*/
	const unsigned short UpdateNextRFID(const unsigned short usRFID);

	/*!
	 * @brief 更新行进终点RFID卡编号
	 * @param const unsigned short 新RFID卡编号
	 * @return const unsigned short 旧RFID卡编号
	*/
	const unsigned short UpdateEndRFID(const unsigned short usRFID);

	/*!
	 * @brief 更新状态
	 * @param const unsigend char 新状态
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateStatus(const unsigned char byStatus);

	/*!
	 * @brief 更新速度
	 * @param const char 新速度
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateSpeed(const char chSpeed);

	/*!
	 * @brief 更新电量
	 * @param unsigned char 新电量
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdatePower(const unsigned char byPower);

	/*!
	 * @brief 更新载货状态
	 * @param unsigned char 新载货状态
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateCargo(const unsigned char byCargo);

	/*!
	 * @brief 更新升降杆状态
	 * @param unsigned char 新升降杆状态
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateLifter(const unsigned char byLifter); 

	/*!
	 * @brief 更新异常信息
	 * @param char 新异常信息
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateError(const char chError);

	/*!
	 * @brief 更新动作
	 * @param unsigned short 新动作
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateAction(const unsigned short usAction);

	/*!
	 * @brief 更新动作状态
	 * @param unsigned short 新动作状态
	 * @return bool 更新返回true,否则返回false
	*/
	const bool UpdateActStatus(const unsigned char byActStatus);

public:
	/*!
	 * @brief 连接客户端
	 * @param const SOCKET 由服务器Accept接收的客户端SOCKET通道
	 * @return bool 连接成功返回true,否则返回false
	*/
	bool Connect(const SOCKET socketClient);

	/*!
	 * @brief 连接服务端
	 * @param const char* 服务端IP地址
	 * @param const unsigned short 服务端端口
	 * @return bool 连接成功返回true,否则返回false
	*/
	bool Connect(const char* strSrvIP, const unsigned short usSrvPort);

	/*!
	 * @brief 连接服务端
	 * @param const wchar_t* 服务端IP地址
	 * @param const unsigned short 服务端端口
	 * @return bool 连接成功返回true,否则返回false
	*/
	bool Connect(const wchar_t* wstrSrvIP, const unsigned short usSrvPort);

	/*!
	 * @brief 中断连接
	*/
	void Break();

	void Release();

	/*!
	 * @brief 是否连接
	 * @return bool 已连接返回true,否则返回false
	*/
	bool IsConnected();

	/*!
	 * @brief 获取待处理数据包
	 * @return DataBuffer 获取待处理数据包
	*/
	static DataBuffer GetProcessPackage();

	/*!
	 * @brief 获取AGV编号
	 * @return unsigned char AGV编号
	*/
	unsigned char GetNo() const;

	/*!
	 * @brief 初始化属性状态
	*/
	int InitAttribute();

	bool StartConnectThread();

protected:
	/*! 
	 * @brief 心跳
	 *
	 * 获取AGV当前实时状态信息
	 * AGV接收到指令后需反馈至程序
	 * {"Message"={},Result={"Status"="","Speed"="","Power"="","CurRFID"="","EndRFID"="","Cargo"="","Lifter"="","Error"="","Action"="","ActStatus"=""}}
	*/
	void Heartbeat();

	/*!
	 * @brief 启动数据发送线程
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartSendThread();

	/*!
	 * @brief 关闭数据发送线程
	*/
	void EndSendThread();

	/*!
	 * @brief 数据发送线程
	*/
	void SendThread();

	/*!
	 * @brief 数据处理
	 * @param unsigned char 接收的数据指针
	 * @param unsigned int 接收的数据长度
	*/
	void ProcessData(const unsigned char * pData, const unsigned int unSize);

	/*!
	 * @brief 解析数据包
	 * @param const unsigned char* 数据包指针
	 * @param const int 数据包长度
	*/
	void ProcessPackage(const unsigned char* pPack, const int nPackLen);

	/*!
	 * @brief 合成报文
	 * @param char* Json字符串
	 * @return DataBuffer 合成后的数据
	*/
	DataBuffer CreatePackage(const char* lpszStr);

	/*!
	 * @brief 合成报文
	 * @param char* 数据块指针
	 * @param int 数据块长度
	 * @return DataBuffer 合成后的数据
	*/
	DataBuffer CreatePackage(const char* lpszStr,const int nLen);

	/*!
	 * @brief 发送报文
	 * @param char* Json字符串
	*/
	void SendPackage(const char* lpszStr);
	
	/*!
	 * @brief 发送报文
	 * @param char* 数据块指针
	 * @param int 数据块长度
	*/
	void SendPackage(const char* pData,const int nLen);

	/*!
	 * @brief 转义
	 * @param const char* 需转义的数据
	 * @param const int 需转义的数据长度
	 * @param char* 转义后的数据
	 * @param int& 转义后的数据长度
	*/
	void Translation(const char* pSourceData, int nSourceLen, char* pTransData, int& pTransLen);

	/*!
	 * @brief 反转义
	 * @param const char* 需反转义的数据
	 * @param const int 需反转义的数据长度
	 * @param char*源数据
	 * @param int& 源数据长度
	*/
	void Retranslation(const char* pTransData, int pTransLen, char* pSourceData, int& nSourceLen);

	/*!
	 * @brief 插入待接收回复的数据列表
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	void InsertIntoWhileList(const char* pData, const int nLen);
	void InsertIntoWhileList(const char* string);

	/*!
	 * @brief 插入数据发送列表
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	void InsertIntoSingleList(const char* pData, const int nLen);

	/*!
	 * @brief 从待接收回复的数据列表移除
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	void RemoveFromWhileList(const char* pData, const int nLen);

	/*!
	 * @brief 插入待处理数据包
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	static void InsertIntoProcessList(const char* pData, const int nLen);

	void ConnectThread();

	void EndConnectThread();
};

#endif //!_AGV_H
