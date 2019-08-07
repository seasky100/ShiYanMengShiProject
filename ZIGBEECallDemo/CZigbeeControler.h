/*!
 * @file CZigbeeControler.h
 * @brief 实现与Zigbee通信的相关功能
 * @author FanKaiyu
 * @date 2019-04-02
 * @version 1.0
*/

#pragma once
#ifndef _ZGBCTRLER_H
#define _ZGBCTRLER_H

#include "../CommunicateDemo/CSerialPort.h"
#include "../String/Buffer.h"
#include "../json/json.h"
#include "CZigbeeCaller.h"
#include <thread>
#include <map>
#include <list>

using namespace std;

/*! @brief 功能码 */
enum FUNC_TYPE
{
	FUNC_HEARTBEAT = 0x1F,
	FUNC_RESET = 0x2F,
	FUNC_SETSIGNAL = 0x3F,
};

/*!
 * @class CZigbeeControler
 * @brief Zigbee通信控制器类
*/
typedef class CZigbeeControler
	:protected CSerialPort
{
public:
	CZigbeeControler(unsigned char byNo);
	CZigbeeControler(unsigned char byNo, unsigned short usPort, unsigned int unBaud, char chParity, unsigned char byDataBits, unsigned char byStopBits);
	~CZigbeeControler();
protected:
	unsigned char m_byNo;						/*!< 控制器编号 */

protected:
	thread* m_pThreadSend;						/*!< 数据发送线程 */
	DataBuffer m_bufferRead;					/*!< 缓存区 用以储存未处理的数据 */
	HANDLE m_hShutdownEvent;					/*!< 关闭事件 */
	list<DataBuffer> m_listSingle;				/*!< 单次发送的数据列表 */
	list<DataBuffer> m_listWhile;				/*!< 循环发送的数据列表 */
	mutex m_mutexSend;							/*!< 发送数据互斥锁 */
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;

protected:
	static list<DataBuffer> m_listProcess;		/*!< 待处理的数据报文列表 */
	static mutex m_mutexProcess;				/*!< 待处理数据互斥锁 */

public:
	/*!
	 * @brief 心跳
	 *
	 * 获取呼叫器当前实时状态信息
	 * 呼叫器接收到指令后需反馈至程序
	 * {"Message"={},Result={"Call":0,"CancelCall":0,"Pass":0,"CancelPass":0,"NG":0,"CancelNG":0,"Scream"：0}}
	 * @param unsigned char 呼叫器编号
	*/
	void Heartbeat(ZGBCaller* pCaller);
	void Heartbeat(const unsigned char byNo, char achMAC[4]);

	/*!
	 * @brief 重置
	 *
	 * 重置呼叫器信号量
	 * 无需呼叫器回复
	 * @param unsigned char 呼叫器编号
	*/
	void Reset(ZGBCaller* pCaller);
	void Reset(const unsigned char byNo, char achMAC[4]);

	/*!
	 * @brief 设置信号
	 * @param unsigned char 呼叫器编号
	 * @param const char* 信号类型
	 * @param unsigned char 信号开关
	*/
	void SetSignal(ZGBCaller* pCaller, const unsigned char bySignal, const unsigned char bySwitch);
	void SetSignal(const unsigned char byNo, char achMAC[4], const unsigned char bySignal, const unsigned char bySwitch);
	//void SetSignal(const unsigned char byNo,const char* strSignal,const unsigned char bySwitch);
	//void SetSignal(const unsigned char byNo, const wchar_t* strSignal, const unsigned char bySwitch);

public:
	/*!
	 * @brief 开启控制器
	 * @param unsigned short 串口号
	 * @param unsigned int 波特率
	 * @param char 奇偶校验位
	 * @param unsigned char 数据位
	 * @param unsigned char 停止位
	 * @param unsinged long 线程ID
	 * @return bool 开启成功返回true,否则返回false
	*/
	bool Open();
	bool Open(unsigned short usPort, unsigned int unBaud = CBR_115200, char chParity = NONE_PARITY, unsigned char byDataBits = 8, unsigned char byStopBits = 1);

	/*!
	 * @brief 关闭控制器
	*/
	void Close();

	/*!
	 * @brief 控制器是否已开启
	 * @return bool 开启返回true,否则返回false
	*/
	bool IsOpened();

	/*!
	 * @brief 获取待处理数据包
	 * @return DataBuffer 获取待处理数据包
	*/
	static DataBuffer GetProcessPackage();
protected:
	/*!
	 * @brief 从串口读取数据
	 * @param char* 数据块指针
	 * @param int 数据长度
	*/
	void ReadChar(const char* data, int len);

	/*!
	 * @brief 解析数据包
	 * @param const unsigned char* 数据包指针
	 * @param const int 数据包长度
	*/
	void ProcessPackage(const unsigned char* pPack,const int nPackLen);

	/*!
	 * @brief 发送报文
	 * @param char* Json字符串
	*/
	void SendPackage( const char* lpszStr, char achMAC[4]);

	/*!
	 * @brief 发送报文
	 * @param char* 数据块指针
	 * @param int 数据块长度
	*/
	void SendPackage( const char* pData, const int nLen, char achMAC[4]);

	/*!
	 * @brief 转义
	 * @param const char* 需转义的数据
	 * @param const int 需转义的数据长度
	 * @param char* 转义后的数据
	 * @param int& 转义后的数据长度
	*/
	void Translation(const char* pSourceData,int nSourceLen,char* pTransData,int& pTransLen);

	/*!
	 * @brief 反转义
	 * @param const char* 需反转义的数据
	 * @param const int 需反转义的数据长度
	 * @param char*源数据
	 * @param int& 源数据长度
	*/
	void Retranslation(const char* pTransData, int pTransLen, char* pSourceData, int& nSourceLen);

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
	 * @brief 合成报文
	 * @param const char* 数据体
	 * @param const int 数据体长度
	 * @return DataBuffer 合成后的完整报文
	*/
	DataBuffer CreatePackage(const char* pData,const int nLen, char achMAC[4]);

	/*!
	 * @brief 插入待接收回复的数据列表
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	void InsertIntoWhileList(const char* pData, const int nLen, char achMAC[4]);

	/*!
	 * @brief 插入数据发送列表
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	void InsertIntoSingleList(const char* pData, const int nLen, char achMAC[4]);

	/*!
	 * @brief 从待接收回复的数据列表移除
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	void RemoveFromWhileList(const char* pData, const int nLen, char achMAC[4]);

	/*!
	 * @brief 插入待处理数据包
	 * @param const char* 数据体指针
	 * @param const int 数据体长度
	*/
	static void InsertIntoProcessList(const char* pData, const int nLen);
} ZGBCtrler;

#endif //!_ZGBCTRLER_H