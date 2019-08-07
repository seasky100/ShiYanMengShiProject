#pragma once
#include "AGVSimular.h"
#include "AGVSimular.c"
#include <iostream>
#include <string>
#include <thread>
#include "../CommunicateDemo/CTCP.h"
#include "../CommunicateDemo/CTCP.cpp"
#include "../Thread/ThreadEx.h"
#include "../String/DebugPrint.h"
#include "../String/StringEx.h"
#include "../json/json.h"
#include "../json/json_value.cpp"
#include "../json/json_reader.cpp"
#include "../json/json_writer.cpp"

using namespace std;

HANDLE m_hShutdownEvent = NULL;	/*!< 关闭事件 */
thread *m_pThreadMsg = NULL;
thread *m_pThreadReceive = NULL;
SOCKET m_socket = INVALID_SOCKET;
int m_nReceive = 0;				/*!< 当前接收的字节数 */
HANDLE m_hProcess = NULL;
mutex m_lock;

void ProcessData(const unsigned char * pData, const unsigned int unSize)
{
	/*!
	 * 将接收到的数据传出
	 * 解决方案有3种
	 * 1、通过类继承重载ProcessData函数，在函数中实现对数据的处理
	 * 2、通过回调函数
	 * 3、通过线程消息，安全的线程消息是单字节传递，如果要一次性传递大量的数据有2种方法
	 * 3.1、创建内存空间，存放数据，传出数据指针，此方法容易存在内存泄漏
	 * 3.2、使用SendThreadMessage函数传递数据，如果数据处理速度过慢会影响数据的接收
	*/

	if (ThreadEx::GetThreadID(m_pThreadMsg) == 0)
	{
		return;
	}

	for (unsigned int unI = 0; unI < unSize; ++unI)
	{
		PostThreadMessage(ThreadEx::GetThreadID(m_pThreadMsg), TM_TCPCLIENT_RECEIVE, *(pData + unI), 1);
	}

	return;
}

void MessageThread()
{
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!GetMessage(&msg, NULL, 0, 0))
			{
				return;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == TM_TCPSERVER_ACCEPT)
			{
				m_socket = msg.wParam;
			}
			else if(msg.message == TM_TCPCLIENT_RECEIVE)
			{
				g_achReceive[m_nReceive] = msg.wParam;
				++m_nReceive;

				int nResult = ProcessData((unsigned char*)g_achReceive, m_nReceive, (unsigned char*)g_achPackage, MAX_PACKAGE_LEN);

				if (nResult > 0)
				{
					memset(g_achReceive, 0, MAX_RECEIVE_LEN);
					m_nReceive = 0;

					SetEvent(m_hProcess);
				}
				else if (nResult == -1)
				{
					memset(g_achReceive, 0, MAX_RECEIVE_LEN);
					m_nReceive = 0;
				}
			}
		}

		if (WaitForSingleObject(m_hShutdownEvent, 1) == WAIT_OBJECT_0)
		{
			break;
		}
	}

	return;
}

void ReceiveThread()
{
	char achDataBuffer[1500];		/*!< 数据接收区*/
	int nBufferLen = 0;				/*!< 接收区数据长度*/

	fd_set fds;

	while (m_socket != INVALID_SOCKET)
	{
		nBufferLen = 0;

		// 初始化接收区
		memset(achDataBuffer, 0, 1500);

		FD_ZERO(&fds);
		FD_SET(m_socket, &fds);

		switch (select((int)m_socket + 1, &fds, NULL, NULL, NULL))
		{
		case  SOCKET_ERROR:
		{
			if (errno == WSAEINTR)
			{
				continue;
			}

			shutdown(m_socket, SD_BOTH);
			m_socket = INVALID_SOCKET;

			break;
		}
		case 0:
		{
			continue;
		}
		default:
		{
			if (FD_ISSET(m_socket, &fds))
			{
				// 接收数据
				nBufferLen = recv(m_socket, achDataBuffer, 1500, 0);

				if (SOCKET_ERROR == nBufferLen)
				{
					shutdown(m_socket, SD_BOTH);
					m_socket = INVALID_SOCKET;

					return;
				} // if (SOCKET_ERROR == nBufferLen)

				if (nBufferLen > 0)
				{
					// 记录接收的数据
					stringstream ssHex; /*!< 16进制数据 */
					for (int nI = 0; nI < nBufferLen; ++nI)
					{
						ssHex << StringEx::string_format("%02X ", (*(achDataBuffer + nI) & 0xFF));
					}
					char* lpszStr = new char[nBufferLen + 1]; /*!< ASCII字符数据 */
					memset(lpszStr, 0, nBufferLen + 1);
					memcpy_s(lpszStr, nBufferLen, achDataBuffer, nBufferLen);

					DebugPrint::Printf("客户端接收数据成功!\n16进制:%s", ssHex.str().c_str());
					DebugPrint::Printf("ASCII字符串:%s", lpszStr);

					delete[] lpszStr;

					ProcessData((unsigned char*)achDataBuffer, nBufferLen);
				} // if (nBufferLen > 0)
				else
				{
					shutdown(m_socket, SD_BOTH);
					m_socket = INVALID_SOCKET;

					return;
				}
			} // if (FD_ISSET(m_socket, &fds))
			break;
		}
		}

		Sleep(1);
	}


	return;
}

int main()
{
	InitAGV(1);

	printf("请输入端口:");

	char achPort[6];	/*!< 用户输入端口 */

	// 初始化 
	memset(achPort, 0, 6);

	cin.get(achPort, 6);
	
	int nPort = stoi(achPort);

	TCPServer server(nPort);	/*!< 服务端*/

	if (m_hShutdownEvent)
	{
		ResetEvent(m_hShutdownEvent);
	}
	else
	{
		m_hShutdownEvent = CreateEvent(nullptr, true, false, nullptr);
	}

	printf("启动消息线程\n");
	m_pThreadMsg = new thread(&MessageThread);

	printf("启动服务端监听\n");
	server.StartListen(ThreadEx::GetThreadID(m_pThreadMsg));

	printf("等待客户端连接......\n");

	while (m_socket == INVALID_SOCKET);

	printf("客户端连接成功\n");

	if (m_hProcess)
	{
		ResetEvent(m_hProcess);
	}
	else
	{
		m_hProcess = CreateEvent(nullptr, true, false, nullptr);
	}

	printf("启动数据接收线程\n");
	m_pThreadReceive = new thread(&ReceiveThread);

	while (true)
	{
		if (WaitForSingleObject(m_hProcess, INFINITE) == WAIT_OBJECT_0)
		{
			ResetEvent(m_hProcess);

			Json::CharReaderBuilder builder;
			Json::CharReader* JsonReader(builder.newCharReader());
			Json::Value JsonRoot;

			JSONCPP_STRING errs;
			const char* pstr = (char*)g_achPackage;

			if (!JsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &errs))
			{
				break;
			}

			string strAction = "";
			strAction = Json_ReadString(JsonRoot["Cmd"]);

			if (strcmp(strAction.c_str(), "Heartbeat") == 0)
			{
				printf("接收心跳报文\n");
				int nSendLen = FeedbackHeartbeat(g_achPackage, strlen(g_achPackage), g_achSend, MAX_SEND_LEN);
				send(m_socket, g_achSend, nSendLen,0);

				// 记录发送的数据
				stringstream ssHex; /*!< 16进制数据 */
				for (int nI = 0; nI < nSendLen; ++nI)
				{
					ssHex << StringEx::string_format("%02X ", (*(g_achSend + nI) & 0xFF));
				}
				char* lpszStr = new char[nSendLen + 1]; /*!< ASCII字符数据 */
				memset(lpszStr, 0, nSendLen + 1);
				memcpy_s(lpszStr, nSendLen, g_achSend, nSendLen);

				DebugPrint::Printf("客户端发送数据成功!\n16进制:%s", ssHex.str().c_str());
				DebugPrint::Printf("ASCII字符串:%s", lpszStr);

				delete[] lpszStr;
			}
			else
			{
				printf("接收报文动作%s\n", strAction.c_str());
				int nSendLen = FeedbackOther(g_achPackage, strlen(g_achPackage), g_achSend, MAX_SEND_LEN);
				send(m_socket, g_achSend, nSendLen, 0);

				// 记录发送的数据
				stringstream ssHex; /*!< 16进制数据 */
				for (int nI = 0; nI < nSendLen; ++nI)
				{
					ssHex << StringEx::string_format("%02X ", (*(g_achSend + nI) & 0xFF));
				}
				char* lpszStr = new char[nSendLen + 1]; /*!< ASCII字符数据 */
				memset(lpszStr, 0, nSendLen + 1);
				memcpy_s(lpszStr, nSendLen, g_achSend, nSendLen);

				DebugPrint::Printf("客户端发送数据成功!\n16进制:%s", ssHex.str().c_str());
				DebugPrint::Printf("ASCII字符串:%s", lpszStr);

				delete[] lpszStr;
			}
		}
	}

	// 关闭数据接收线程
	printf("关闭数据接收线程\n");
	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	m_pThreadReceive->join();

	delete m_pThreadReceive;
	m_pThreadReceive = NULL;

	CloseHandle(m_hProcess);
	m_hProcess = NULL;

	// 关闭消息线程	
	printf("关闭消息线程\n");
	SetEvent(m_hShutdownEvent);

	server.CloseListen();

	m_pThreadMsg->join();

	CloseHandle(m_hShutdownEvent);
	m_hShutdownEvent = NULL;

	delete m_pThreadMsg;
	m_pThreadMsg = NULL;

	printf("程序退出\n");

	return 0;
}