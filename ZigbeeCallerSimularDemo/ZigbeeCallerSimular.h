/*!
 * @file ZigbeeCallerSimular.c
 * @brief Zigbee呼叫器数据通信文件
 * @author FanKaiyu
 * @date 2019-04-16
 * @version 1.1
 *
 * @update FanKaiyu 2019-04-25 1、增加 PACKAGE_HEAD、PACKAGE_TAIL、PACKAGE_TRANS 2、修改ProcessData、Translation、Retranslation、CreatePackage函数
 * @update FanKaiyu 2019-05-04 修改报文:将原JSON报文变成16禁止报文.即报文改为"编号+功能码+参数"的组成方式.
*/

/*! 
 * @brief 报文解析
 * 
 * 1、心跳报文
 * 编号 0x01-0xFF
 * 功能码 0x1F
 * 参数 无
*/

#include "CRC.c"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifndef _C_ZIGBEECALLER
#define _C_ZIGBEECALLER

#define PACKAGE_HEAD 0xBA
#define PACKAGE_TAIL 0xBE
#define PACKAGE_TRANS 0xB0

#define MAX_RECEIVE_LEN 1000
#define MAX_PACKAGE_LEN 1000
#define MAX_SEND_LEN 1000

/*! @brief 功能码 */
enum FUNC_TYPE
{
	FUNC_HEARTBEAT = 0x1F,
	FUNC_RESET = 0x2F,
	FUNC_SETSIGNAL = 0x3F,
};

/*! @brief 呼叫器类型 */
enum SIGNAL_TYPE
{
	CALL_SIGNAL = 1,
	CANCELCALL_SIGNAL,
	PASS_SIGNAL,
	CANCELPASS_SIGNAL,
	NG_SIGNAL,
	CANCELNG_SIGNAL,
	SCREAM,
};

/*! @brief 呼叫器信号开关 */
enum CALL_SIGNAL
{
	SIGNAL_OFF,
	SIGNAL_ON,
};

extern char g_achReceive[MAX_RECEIVE_LEN];		/*!< 数据接收缓存区:用以存放接收的数据 */
extern char g_achPackage[MAX_PACKAGE_LEN];		/*!< 报文数据体缓存区:用以存放解析出的JSON字符串 */
extern char g_achSend[MAX_SEND_LEN];			/*!< 数据发送缓存区:用以存放要发送给的数据 */

extern unsigned char g_byNo;					/*!< 编号 */
extern unsigned char g_byCall;					/*!< 呼叫标识 */
extern unsigned char g_byCancelCall;			/*!< 取消呼叫标识 */
extern unsigned char g_byPass;					/*!< 放行标识 */
extern unsigned char g_byCancelPass;			/*!< 取消放行标识 */
extern unsigned char g_byNG;					/*!< NG标识 */
extern unsigned char g_byCancelNG;				/*!< 取消NG标识 */
extern unsigned char g_byScream;				/*!< 急停标识 */

/*!
 * @brief 初始化
 *
 * 初始化呼叫器编号、状态
 * @param unsigned char 呼叫器编号
*/
void InitCaller(unsigned char byNo);

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

#endif //!_C_ZIGBEECALLER

