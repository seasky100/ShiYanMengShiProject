// pch.h: 这是预编译标头文件。
// 下方列出的文件仅编译一次，提高了将来生成的生成性能。
// 这还将影响 IntelliSense 性能，包括代码完成和许多代码浏览功能。
// 但是，如果此处列出的文件中的任何一个在生成之间有更新，它们全部都将被重新编译。
// 请勿在此处添加要频繁更新的文件，这将使得性能优势无效。

#ifndef PCH_H
#define PCH_H

// 添加要在此处预编译的标头
#include "framework.h"

#include "../ADO/ADOConn.h"
#include "../Data/AES.h"
#include "../Data/CRC.h"
#include "../json/json.h"
#include "../String/Buffer.h"
#include "../String/DebugPrint.h"
#include "../String/StringEx.h"
#include "../String/SystemLog.h"
#include "../Thread/ThreadEx.h"
#include "../XML/Xml.h"
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>
#include <afxcontrolbars.h>

#define TM_TCPCLIENT_RECEIVE WM_USER+1	/*!< 客户端接收数据的线程消息 */
#define TM_TCPSERVER_ACCEPT WM_USER+2	/*!< 服务端接收客户端连接请求的线程消息*/
#define TM_COMM_RXCHAR	WM_USER+3		/*!< 串口接收数据的线程消息 */
#define TM_UPDATE_AGV WM_USER+4			/*!< 更新AGV信息的线程消息 */
#define TM_UPDATE_SERVER WM_USER+5		/*!< 更新服务端信息的线程消息 */
#define TM_UPDATE_CALLER WM_USER+6		/*!< 更新呼叫器信息的线程消息 */
#define TM_UPDATE_TASK WM_USER+7		/*!< 更新任务信息的线程消息 */
#define TM_UPDATE_KEY WM_USER+8			/*!< 更新权限信息的线程消息 */
#define TM_EXIT WM_USER+9				/*!< 线程退出消息 */

#ifdef _WIN64
#define ULL unsigned long long
#define GetTickCountEX() GetTickCount64()
#else
#define ULL unsigned long
#define GetTickCountEX() GetTickCount()
#endif

enum USERLEVEL
{
	USER_NORMAL,
	USER_CONTROLER,
	USER_ADMINISTER,
};

#endif //PCH_H
