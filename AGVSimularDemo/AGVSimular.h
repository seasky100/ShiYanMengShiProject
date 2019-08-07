/*!
 * @file AGVSimular.c
 * @brief 下位机AGV数据通信文件
 * @author FanKaiyu
 * @date 2019-04-08
 * @version 1.3
 *
 * @update FanKaiyu 2019-04-10 1、增加变量g_usNextRFID;2、修改FeedbackHeartbeat函数,增加NextRFID回复
 * @update FanKaiyu 2019-04-15 1、增加状态_AGVSTA_ARM 2、修改ProcessData函数
 * @update FanKaiyu 2019-04-25 1、增加_AGVSTA_ALLSCREAM 2、增加 PACKAGE_HEAD、PACKAGE_TAIL、PACKAGE_TRANS 3、修改ProcessData、Translation、Retranslation、CreatePackage函数
*/
#include "CRC.c"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef _C_AGVSIMULAR
#define _C_AGVSIMULAR

/*! @brief AGV电量状态 */
enum _AGVPOWER
{
	_AGVPOWER_EMPTY = 0,		/*!< 未检测到电池 */
	_AGVPOWER_LOW = 20,			/*!< 低电量 */
	_AGVPOWER_NORMAL = 50,		/*!< 正常电量 */
	_AGVPOWER_HIGH = 90,		/*!< 高电量 */
	_AGVPOWER_FULL = 100,		/*!< 满电*/
};

/*! @brief AGV运行状态 */
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
	_AGVSTA_TRAFFIC,	/*!< 交通管制停止状态.\
						上位机接收到此状态,认为AGV处于交通管制点等待上位机放行,此时上位机根据交通管制规则决定此状态的AGV是否允许通行.\
						此时下位机除了处理上位机发出的心跳报文以及交通管制控制报文,其他报文均可以不处理.\
						*/
	_AGVSTA_ARM,		/*!< 机械手反馈.\
						上位机接收到此状态,任务机械手已经对AGV操作完成.
						*/
	_AGVSTA_ALLSCREAM,	/*!< 全线急停 */
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

#define PACKAGE_HEAD 0xBA
#define PACKAGE_TAIL 0xBE
#define PACKAGE_TRANS 0xB0

#define MAX_RECEIVE_LEN 1000
#define MAX_PACKAGE_LEN 1000
#define MAX_SEND_LEN 1000

#define CMD_HEARTBEAT "Heartbeat"
#define CMD_MOVE "Move"
#define CMD_REMOVE "Remove"
#define CMD_TRAFFIC_PASS "TrafficPass"
#define CMD_TRAFFIC_STOP "TrafficStop"
#define CMD_SHUTDOWN "Shutdown"
#define CMD_SCREAM "Scream"
#define CMD_REST "Reset"
#define CMD_RESTART "Restart"
#define CMD_LIFT_UP "LiftUp"
#define CMD_LIFT_DOWN "LiftDown"
#define CMD_STOP "Stop"
#define CMD_ACTION "Other"
#define CMD_SET_STATUS "Set"
#define CMD_PAUSE "Pause"
#define CMD_CONTINUE "Continue"
#define CMD_SPEED_CONTROL "SetSpeed"
#define CMD_ALLSCREAM "AllScream"

extern char g_achReceive[MAX_RECEIVE_LEN];	/*!< 数据接收缓存区:用以存放接收的数据 */
extern char g_achPackage[MAX_PACKAGE_LEN];	/*!< 报文数据体缓存区:用以存放解析出的JSON字符串 */
extern char g_achSend[MAX_SEND_LEN];		/*!< 数据发送缓存区:用以存放要发送给的数据 */

extern unsigned char g_byNo;				/*!< 编号 */
extern unsigned char g_byStatus;			/*!< 状态 */
extern char g_chSpeed;						/*!< 速度 */
extern unsigned char g_byPower;				/*!< 电量 */
extern unsigned short g_usCurRFID;			/*!< 行进当前RFID卡编号 */
extern unsigned short g_usNextRFID;			/*!< 行进下一RFID卡编号 */
extern unsigned short g_usEndRFID;			/*!< 终点RFID卡编号 */
extern unsigned char g_byCargo;				/*!< 载货状态 */
extern unsigned char g_byLifter;			/*!< 升降杆状态 0落下 1升起 */
extern char g_chError;						/*!< 异常信息 */
extern unsigned short g_usAction;			/*!< 执行的动作 0无动作 1移动 2重新移动 3交通管制 4升降杆上升 5升降杆下降 */
extern unsigned char g_byActStatus;			/*!< 动作状态  0未执行 1执行中 2执行完成 */

/*!
 * @brief 初始化
 *
 * 初始化AGV编号、状态
 * @param unsigned char AGV编号
*/
void InitAGV(const unsigned char byNo);

/*!
 * @brief 转义
 * @param const char* 需转义的数据
 * @param const int 需转义的数据长度
 * @param char* 转义后的数据
 * @param int& 转义后的数据长度
*/
void Translation(const char* pSourceData, int nSourceLen, char* pTransData, int* pTransLen);

/*!
 * @brief 反转义
 * @param const char* 需反转义的数据
 * @param const int 需反转义的数据长度
 * @param char*源数据
 * @param int& 源数据长度
*/
void Retranslation(const char* pTransData, int pTransLen, char* pSourceData, int* nSourceLen);

/*!
 * @brief 报文解析
 *
 * 将接收到的报文填充进数组中
 * @param const unsigned char* 存放接收数据的数组
 * @param unsigned int 数组长度
 * @param unsigned char* 存放解析出的报文的数组指针(Json字符串)
 * @param const unsigned int 报文数据长度
 * @return int 返回实际解析出的报文长度(Json字符串长度),返回0则未收到完整数据,返回-1则不存在有效报文
*/
int ProcessData(const unsigned char* pProcessData, const unsigned int MAX_LEN, unsigned char* pJson, const unsigned int MAX_JSON_LEN);

void CreatePackage(const char* lpszStr, const int nLen, char* pData, int* nSize);

/*!
 * @brief 反馈心跳报文
 *
 * 用以反馈心跳报文
 * @param const char* 来自上位机的报文(Json字符串)
 * @param const int 报文长度
 * @param char* 存放反馈报文的数组指针
 * @param const int 最大报文长度
 * @return int 实际报文长度
*/
int FeedbackHeartbeat(const char* pSource, const int pSourceLen, char* pPack, const int MAX_PACK_LEN);

/*!
 * @brief 反馈其他报文
 *
 * 用以返回其他需要AGV反馈的报文
 * @param const char* 来自上位机的报文(Json字符串)
 * @param const int 报文长度
 * @param char* 存放反馈报文的数组指针
 * @param const int 最大报文长度
 * @return int 实际报文长度
*/
int FeedbackOther(const char* pSource, const int pSourceLen, char* pPack, const int MAX_PACK_LEN);

#endif //!_C_AGVSIMULAR
