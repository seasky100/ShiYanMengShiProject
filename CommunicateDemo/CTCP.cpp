#include "pch.h"
#include "CTCP.h"
#include "../String/DebugPrint.h"
#include "../String/StringEx.h"

#ifdef _TCP_H

SocketClient::SocketClient()
	:m_socket(INVALID_SOCKET)
	, m_strLocalIP("")
	, m_nLocalPort(-1)
	, m_strSrvIP("")
	, m_nSrvPort(-1)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	
}

SocketClient::SocketClient(const char* strSrvIP, const char* strLocalIP, const int nSrvPort, const int nLocalPort)
	:m_socket(INVALID_SOCKET)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	m_strLocalIP = strLocalIP;
	m_nLocalPort = nLocalPort;
	m_strSrvIP = strSrvIP;
	m_nSrvPort = nSrvPort;
}

SocketClient::SocketClient(const wchar_t* wstrSrvIP, const wchar_t* wstrLocalIP, int nSrvPort, int nLocalPort)
	:m_socket(INVALID_SOCKET)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	m_strLocalIP = StringEx::wstring_to_string(wstrLocalIP);
	m_nLocalPort = nLocalPort;
	m_strSrvIP = StringEx::wstring_to_string(wstrSrvIP);
	m_nSrvPort = nSrvPort;
}

SocketClient::SocketClient(const string strLocalIP, string strSrvIP, int nLocalPort, int nSrvPort)
	:m_socket(INVALID_SOCKET)
	, m_strSockIP("")
	, m_usSockPort(0)
	, m_pReceiveThread(nullptr)
	, m_ulThreadID(0)
	, m_ullReceiveTime(0)
	, m_ullSendTime(0)
	, m_socketTmp(INVALID_SOCKET)
{
	m_strLocalIP = strLocalIP;
	m_nLocalPort = nLocalPort;
	m_strSrvIP = strSrvIP;
	m_nSrvPort = nSrvPort;
}

SocketClient::~SocketClient()
{
	Break();
}

bool SocketClient::ConnectPeer(const SOCKET socketClien, const unsigned long ulThreadID)
{
	// 无效SOCKET通道
	if (socketClien == INVALID_SOCKET)
	{
		return false;
	}

	std::string strClientIP = "";		/*!< 客户端IP */
	unsigned short usClientPort = 0;	/*!< 客户端端口 */

	// 无法获取客户端IP与端口
	if (TCP::GetPeerIPandPort(socketClien, strClientIP, usClientPort) == false)
	{
		return false;
	}

	// 无绑定的IP地址
	if (m_strLocalIP == "")
	{
		return false;
	}

	// IP或端口不符合
	if (m_strLocalIP != strClientIP || (m_nLocalPort != -1 && m_nLocalPort != usClientPort))
	{
		return false;
	}

	m_strSockIP = strClientIP;
	m_usSockPort = usClientPort;

	return Connect(socketClien, ulThreadID);
}

bool SocketClient::Connect(const SOCKET socketClien, const unsigned long ulThreadID)
{
	if (socketClien == INVALID_SOCKET)
	{
		return false;
	}

	// 通道不相同
	if (m_socket != INVALID_SOCKET)
	{
		if (m_socket != socketClien)
		{
			Break();
		}
	}

	EndReceiveThread();

	m_socket = socketClien;
	m_ulThreadID = ulThreadID;

	if (StartReceiveThread() == false)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		Break();

		return false;
	}

	std::string strSockIP = "", strPeerClient = "";	/*!< IP地址 */
	unsigned short usSockPort = 0, usPeerPort = 0;	/*!< 端口 */

	TCP::GetSockIPandPort(m_socket, strSockIP, usSockPort);
	TCP::GetPeerIPandPort(m_socket, strPeerClient, usPeerPort);

	DebugPrint::Printf("本地地址%s:%d连接远程地址%s:%d成功", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

	m_socketTmp = INVALID_SOCKET;

	m_ullReceiveTime = 0;
	m_ullSendTime = 0;

	return true;
}

bool SocketClient::Connect(const unsigned long ulThreadID)
{
	if (m_socketTmp != INVALID_SOCKET)
	{
		return false;
	}

	// 初始化套接字
	WORD sockVersion = MAKEWORD(2, 2);

	WSADATA datawsa;

	// WSAStartup异步套接字启动函数
	if (WSAStartup(sockVersion, &datawsa) != 0)
	{
		WSACleanup();
		return false;
	}

	// 创建套接字
	m_socketTmp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 客户端SOCKET描述符

	if (m_socketTmp == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}

	SOCKADDR_IN addrLocal; // 本地地址
	addrLocal.sin_family = AF_INET;

	// 绑定端口
	if (m_nLocalPort != -1)
	{
		addrLocal.sin_port = htons(m_nLocalPort);
	}
	else
	{
		// 若port指定为0,则调用bind时，系统会为其指定一个可用的端口号
		addrLocal.sin_port = 0;
	}

	// 绑定IP
	if (m_strLocalIP == "")
	{
		addrLocal.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		inet_pton(AF_INET, m_strLocalIP.c_str(), &addrLocal.sin_addr.s_addr);
	}
	
	//绑定套接字 
	int retVal; // 返回值
	retVal = ::bind(m_socketTmp, (LPSOCKADDR)&addrLocal, sizeof(SOCKADDR_IN));

	if (SOCKET_ERROR == retVal)
	{
		//关闭套接字
		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		//释放套接字资源;
		WSACleanup();

		return false;
	}

	// 初始化sockaddr_in结构体
	sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(m_nSrvPort);
	// serAddr.sin_addr.S_un.S_addr = inet_addr(strIP.c_str());

	inet_pton(AF_INET, m_strSrvIP.c_str(), &serAddr.sin_addr.s_addr);

	// 连接服务器
	if (connect(m_socketTmp, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		WSACleanup();

		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		return false;
	}

	// 无法获取本地IP与端口
	if (TCP::GetPeerIPandPort(m_socketTmp, m_strSockIP, m_usSockPort) == false)
	{
		WSACleanup();

		shutdown(m_socketTmp, SD_BOTH);
		closesocket(m_socketTmp);
		m_socketTmp = INVALID_SOCKET;

		return false;
	}
	
	return Connect(m_socketTmp, ulThreadID);
}

bool SocketClient::Connect(const char* strSrvIP, const unsigned short usSrvPort, const unsigned long ulThreadID)
{
	m_strSrvIP = strSrvIP;
	m_nSrvPort = usSrvPort;

	return Connect(ulThreadID);
}

bool SocketClient::Connect(const wchar_t* wstrSrvIP, const unsigned short usSrvPort, const unsigned long ulThreadID)
{
	m_strSrvIP = StringEx::wstring_to_string(wstrSrvIP);
	m_nSrvPort = usSrvPort;

	return Connect(ulThreadID);
}

void SocketClient::Break()
{
	EndReceiveThread();

	m_ullReceiveTime = 0;
	m_ullSendTime = 0;

	return;
}

bool SocketClient::Send(const unsigned char * pData, const unsigned int unSize)
{
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	int nIndex = 0; /*!< 成功发送的字节数 */

	nIndex = send(m_socket, (char*)pData, unSize, 0);

	if (nIndex == SOCKET_ERROR)
	{
		DebugPrint::Printf("客户端%s:%d发送数据失败", m_strSockIP.c_str(), m_usSockPort);

		return false;
	}

	m_ullSendTime = GetTickCountEX();

	/*
	// 记录发送的数据
	stringstream ssHex; // 16进制数据
	for (int nI = 0; nI < nIndex; ++nI)
	{
		ssHex << StringEx::string_format("%02X ", (*(pData + nI) & 0xFF));
	}
	char* lpszStr = new char[nIndex + 1]; // ASCII字符数据
	memset(lpszStr, 0, nIndex + 1);
	memcpy_s(lpszStr, nIndex, pData, nIndex);

	DebugPrint::Printf("客户端%s:%d发送数据成功!\n16进制:%s", m_strSockIP.c_str(), m_usSockPort, ssHex.str().c_str());
	DebugPrint::Printf("ASCII字符串:%s", lpszStr);

	delete[] lpszStr;
	*/

	return true;
}

bool SocketClient::IsConnected()
{
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}

bool SocketClient::GetIPandPort(std::string & strIP, int & nPort)
{
	if (m_strSockIP != "")
	{
		strIP = m_strSockIP;
	}
	else
	{
		TCP::GetLocalIP(strIP, m_nLocalPort);
	}

	nPort = m_usSockPort;

	return IsConnected();
}

bool SocketClient::GetIPandPort(std::wstring & wstrIP, int & nPort)
{
	if (m_strSockIP != "")
	{
		wstrIP = StringEx::string_to_wstring(m_strSockIP);
	}
	else
	{
		TCP::GetLocalIP(wstrIP, m_nLocalPort);
	}

	nPort = m_usSockPort;

	return IsConnected();
}

void SocketClient::SetLocalIPandPort(const char * strIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}
	
	m_strLocalIP = strIP;
	m_nLocalPort = nPort;

	return;
}

void SocketClient::SetLocalIPandPort(const wchar_t * wstrIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}

	m_strSockIP = StringEx::wstring_to_string(wstrIP);
	m_nLocalPort = nPort;

	return;
}

void SocketClient::SetServerIPandPort(const char* strIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}

	m_strSrvIP = strIP;
	m_nSrvPort = nPort;

	return;
}

void SocketClient::SetServerIPandPort(const wchar_t* wstrIP, int nPort)
{
	if (IsConnected())
	{
		Break();
	}

	m_strSrvIP = StringEx::wstring_to_string(wstrIP);
	m_nSrvPort = nPort;

	return;
}

bool SocketClient::StartReceiveThread()
{
	m_pReceiveThread = new std::thread(&SocketClient::ReceiveThread, this);

	if (m_pReceiveThread)
	{
		return true;
	}

	return false;
}

void SocketClient::EndReceiveThread()
{
	if (m_pReceiveThread)
	{
		if (m_pReceiveThread->joinable())
		{
			if (m_socket != INVALID_SOCKET)
			{
				// 关闭SOCKET套接字
				shutdown(m_socket, SD_BOTH);
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;

				DebugPrint::Printf("客户端%s:%d关闭数据接收线程", m_strSockIP.c_str(), m_usSockPort);
			}

			m_pReceiveThread->join();
		}

		delete m_pReceiveThread;
		m_pReceiveThread = nullptr;
	}
	
	return;
}

void SocketClient::ReceiveThread()
{
	char achDataBuffer[1500];		/*!< 数据接收区*/
	int nBufferLen = 0;				/*!< 接收区数据长度*/

	fd_set fds;

	DebugPrint::Printf("客户端%s:%d数据接收线程开启", m_strSockIP.c_str(), m_usSockPort);

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

			DebugPrint::Printf("客户端%s:%d通道异常关闭", m_strSockIP.c_str(), m_usSockPort);

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

					DebugPrint::Printf("客户端%s:%d数据接收异常，通道关闭", m_strSockIP.c_str(), m_usSockPort);

					break;
				} // if (SOCKET_ERROR == nBufferLen)

				if (nBufferLen > 0)
				{
					/*
					// 记录接收的数据
					stringstream ssHex; // 16进制数据
					for (int nI = 0; nI < nBufferLen; ++nI)
					{
						ssHex << StringEx::string_format("%02X ", (*(achDataBuffer + nI) & 0xFF));
					}
					char* lpszStr = new char[nBufferLen + 1]; // ASCII字符数据
					memset(lpszStr, 0, nBufferLen + 1);
					memcpy_s(lpszStr, nBufferLen, achDataBuffer, nBufferLen);

					DebugPrint::Printf("客户端%s:%d接收数据成功!\n16进制:%s",m_strSockIP.c_str(), m_usSockPort, ssHex.str().c_str());
					DebugPrint::Printf("ASCII字符串:%s", lpszStr);

					delete[] lpszStr;
					*/

					m_ullReceiveTime = GetTickCountEX();

					ProcessData((unsigned char*)achDataBuffer, nBufferLen);
				} // if (nBufferLen > 0)
				else
				{
					shutdown(m_socket, SD_BOTH);
					m_socket = INVALID_SOCKET;

					DebugPrint::Printf("客户端%s:%d数据接收异常，通道关闭", m_strSockIP.c_str(), m_usSockPort);

					break;
				}
			} // if (FD_ISSET(m_socket, &fds))
			break;
		}
		}

		Sleep(1);
	}

	DebugPrint::Printf("客户端%s:%d数据接收线程结束", m_strSockIP.c_str(), m_usSockPort);

	return;
}

SocketServer::SocketServer()
{
	m_socket = INVALID_SOCKET;
	m_strIP = "";
	m_usPort = 0;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = nullptr;
	m_funcParamPrt = nullptr;
}

SocketServer::SocketServer(const unsigned short usPort, const char * strIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = strIP;
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::SocketServer(const unsigned short usPort, const wchar_t * wstrIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = StringEx::wstring_to_string(wstrIP);
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::SocketServer(const unsigned short usPort, std::string strIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = strIP;
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::SocketServer(const unsigned short usPort, std::wstring wstrIP, UpdateServer funcPrt, void* ptr)
{
	m_socket = INVALID_SOCKET;
	m_strIP = StringEx::wstring_to_string(wstrIP);
	m_usPort = usPort;
	m_pAcceptThread = nullptr;
	m_ulThreadID = 0;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;
}

SocketServer::~SocketServer()
{
	CloseListen();
}

bool SocketServer::StartListen(UpdateServer funcPrt, void* ptr,const unsigned long ulThreadID)
{
	CloseListen();

	WSADATA wsd;

	SOCKADDR_IN addrServ; /*!< 服务器地址*/

	int retVal; /*!< 返回值*/

	// 初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
	{
		return false;
	}

	//创建套接字  
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == m_socket)
	{
		//释放套接字资源
		WSACleanup();

		return false;
	}

	//服务端套接字地址   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(m_usPort);
	if (m_strIP == "")
	{
		addrServ.sin_addr.s_addr = INADDR_ANY;
	}
	else
	{
		// addrServ.sin_addr.s_addr = inet_addr(m_strIP.c_str());

		inet_pton(AF_INET, m_strIP.c_str(), &addrServ.sin_addr.s_addr);
	}

	//绑定套接字  
	retVal = ::bind(m_socket, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));

	if (SOCKET_ERROR == retVal)
	{
		//关闭套接字
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		//释放套接字资源;
		WSACleanup();

		return false;
	}

	m_ulThreadID = ulThreadID;
	m_funcPrt = funcPrt;
	m_funcParamPrt = ptr;

	StartAcceptThread();

	return true;
}

bool SocketServer::StartListen(const unsigned short usPort,  const char * strIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	m_usPort = usPort;
	m_strIP = strIP;

	return StartListen(funcPrt, ptr, ulThreadID);
}

bool SocketServer::StartListen(const unsigned short usPort,  const wchar_t * wstrIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	m_usPort = usPort;
	m_socket = INVALID_SOCKET;

	m_strIP = StringEx::wstring_to_string(wstrIP);

	return StartListen(funcPrt, ptr, ulThreadID);
}

bool SocketServer::StartListen(const unsigned short usPort, std::string strIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	return StartListen(usPort, strIP.c_str(), funcPrt, ptr, ulThreadID);
}

bool SocketServer::StartListen(const unsigned short usPort, std::wstring wstrIP, UpdateServer funcPrt, void* ptr, const unsigned long ulThreadID)
{
	return StartListen(usPort, wstrIP.c_str(), funcPrt, ptr,ulThreadID);
}

void SocketServer::CloseListen()
{
	m_funcPrt = nullptr;
	m_funcParamPrt = nullptr;

	EndAcceptThread();

	return;
}

bool SocketServer::IsListened()
{
	if (m_socket == INVALID_SOCKET)
	{
		return false;
	}

	if (m_pAcceptThread == nullptr)
	{
		shutdown(m_socket, SD_BOTH);
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;

		return false;
	}

	if (m_pAcceptThread->joinable())
	{
		return true;
	}

	shutdown(m_socket, SD_BOTH);
	closesocket(m_socket);
	m_socket = INVALID_SOCKET;

	return false;
}

void SocketServer::Bind(unsigned short usPort, const char * strIP)
{
	if (IsListened())
	{
		CloseListen();
	}

	m_usPort = usPort;
	m_strIP = strIP;

	return;
}

void SocketServer::Bind(unsigned short usPort, const wchar_t * wstrIP)
{
	if (IsListened())
	{
		CloseListen();
	}

	m_usPort = usPort;
	m_strIP = StringEx::wstring_to_string(wstrIP);

	return;
}

void SocketServer::Bind(unsigned short usPort, std::string strIP)
{
	Bind(usPort, strIP.c_str());

	return;
}

void SocketServer::Bind(unsigned short usPort, std::wstring wstrIP)
{
	Bind(usPort, wstrIP.c_str());

	return;
}

void SocketServer::GetIPandPort(char * strIP, unsigned short & usPort) const
{
	memset(strIP, 0, IP4_ADDRESS_STRING_BUFFER_LENGTH);

	if (m_strIP == "")
	{
		if (m_socket != INVALID_SOCKET)
		{
			TCP::GetLocalIP(strIP, m_usPort);
		}
		else
		{
			TCP::GetLocalIP(strIP, -1);
		}
	}
	else
	{
		memcpy_s(strIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, m_strIP.c_str(), m_strIP.length());
	}

	usPort = m_usPort;

	return;
}

void SocketServer::GetIPandPort(wchar_t * wstrIP, unsigned short & usPort) const
{
	wmemset(wstrIP, 0, IP4_ADDRESS_STRING_BUFFER_LENGTH);

	if (m_strIP == "")
	{
		if (m_socket != INVALID_SOCKET)
		{
			TCP::GetLocalIP(wstrIP, m_usPort);
		}
		else
		{
			TCP::GetLocalIP(wstrIP, -1);
		}
	}
	else
	{
		wmemcpy_s(wstrIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, StringEx::string_to_wstring(m_strIP).c_str(), StringEx::string_to_wstring(m_strIP).length());
	}

	usPort = m_usPort;

	return;
}

void SocketServer::GetIPandPort(std::string & strIP, unsigned short & usPort) const
{
	char strLocalIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

	if (m_strIP == "")
	{
		if (m_socket != INVALID_SOCKET)
		{
			TCP::GetLocalIP(strLocalIP, m_usPort);
		}
		else
		{
			TCP::GetLocalIP(strLocalIP, -1);
		}
	}
	else
	{
		strIP = m_strIP;
	}

	if (strcmp(strLocalIP, "") == 0)
	{
		strIP = m_strIP;
	}
	else
	{
		strIP = strLocalIP;
	}

	usPort = m_usPort;

	return;
}

void SocketServer::GetIPandPort(std::wstring & wstrIP, unsigned short & usPort) const
{
	if (m_socket != INVALID_SOCKET)
	{
		TCP::GetSockIPandPort(m_socket, wstrIP, usPort);

		return;
	}

	if (m_strIP == "")
	{
		char strLocalIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (TCP::GetLocalIP(strLocalIP, -1))
		{
			wstrIP = StringEx::string_to_wstring(strLocalIP);
		}
		else
		{
			wstrIP = StringEx::string_to_wstring(m_strIP);
		}
	}
	else
	{
		wstrIP = StringEx::string_to_wstring(m_strIP);
	}

	usPort = m_usPort;

	return;
}

bool SocketServer::StartAcceptThread()
{
	EndAcceptThread();

	m_pAcceptThread = new std::thread(&SocketServer::AcceptThread, this);

	if (m_pAcceptThread)
	{
		return true;
	}

	return false;
}

void SocketServer::EndAcceptThread()
{
	if (m_pAcceptThread == nullptr)
	{
		return;
	}

	if (m_pAcceptThread->joinable())
	{
		if (m_socket != INVALID_SOCKET)
		{
			std::string strSockIP = "";	/*!< IP地址 */
			unsigned short usSockPort = 0;	/*!< 端口 */
			TCP::GetSockIPandPort(m_socket, strSockIP, usSockPort);
			DebugPrint::Printf("服务端:%s:%d通道关闭", strSockIP.c_str(), usSockPort);

			// 关闭SOCKET套接字
			shutdown(m_socket, SD_BOTH);
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}

		m_pAcceptThread->join();
	}

	delete m_pAcceptThread;
	m_pAcceptThread = nullptr;

	return;
}

void SocketServer::AcceptThread()
{
	std::string strSockIP = "";	/*!< IP地址 */
	unsigned short usSockPort = 0;	/*!< 端口 */
	TCP::GetSockIPandPort(m_socket, strSockIP, usSockPort);
	DebugPrint::Printf("服务端:%s:%d监听线程开启", strSockIP.c_str(), usSockPort);

	while (true)
	{
		//开始监听   
		int retVal = listen(m_socket, 1); /*!< 返回值 */

		if (SOCKET_ERROR == retVal)
		{
			// 监听异常

			if (m_socket != INVALID_SOCKET)
			{
				//关闭套接字
				closesocket(m_socket);
				m_socket = INVALID_SOCKET;

				DebugPrint::Printf("服务端:%s:%d监听异常，通道关闭", strSockIP.c_str(), usSockPort);

				if (m_funcPrt)
				{
					m_funcPrt(this, m_funcParamPrt, m_socket);
				}
			}
			
			//释放套接字资源;
			WSACleanup();

			return;
		}

		//接受客户端请求  
		sockaddr_in addrClient; /*!< 客户端地址*/

		int addrClientlen = sizeof(addrClient); /*!< 客户端长度*/

		SOCKET sClient = accept(m_socket, (sockaddr FAR*)&addrClient, &addrClientlen); /*!< 客户端SOCKET描述符*/

		if (INVALID_SOCKET == sClient)
		{
			//关闭套接字
			closesocket(m_socket);
			m_socket = INVALID_SOCKET;

			//释放套接字资源;
			WSACleanup();

			DebugPrint::Printf("服务端:%s:%d接收客户端异常，通道关闭", strSockIP.c_str(), usSockPort);

			if (m_funcPrt)
			{
				m_funcPrt(this, m_funcParamPrt, m_socket);
			}

			return;
		}
		else
		{
			AcceptClient(sClient);
		}

		Sleep(1);
	}

	DebugPrint::Printf("服务端:%s:%d监听线程结束", strSockIP.c_str(), usSockPort);

	return;
}

void SocketServer::AcceptClient(const SOCKET socket)
{
	std::string strSockIP = "",strPeerClient = "";	/*!< IP地址 */
	unsigned short usSockPort = 0,usPeerPort = 0;	/*!< 端口 */
	TCP::GetSockIPandPort(socket, strSockIP, usSockPort);
	TCP::GetPeerIPandPort(socket, strPeerClient, usPeerPort);
	DebugPrint::Printf("服务端%s:%d连接客户端%s:%d成功", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

	if (m_funcPrt == nullptr)
	{
		// 关闭SOCKET套接字
		shutdown(socket, SD_BOTH);
		closesocket(socket);

		DebugPrint::Printf("服务端%s:%d连接客户端%s:%d中断", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

		return;
	}

	m_funcPrt(this, m_funcParamPrt, socket);

	if (m_ulThreadID == 0)
	{
		// 关闭SOCKET套接字
		shutdown(socket, SD_BOTH);
		closesocket(socket);

		DebugPrint::Printf("服务端%s:%d连接客户端%s:%d中断", strSockIP.c_str(), usSockPort, strPeerClient.c_str(), usPeerPort);

		return;
	}

	PostThreadMessage(m_ulThreadID, TM_TCPSERVER_ACCEPT, socket, 0);

	return;
}

#endif //_TCP_H
