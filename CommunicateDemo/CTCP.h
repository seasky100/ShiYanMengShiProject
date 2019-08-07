/*!
 * @file TCP.h
 * @brief 基于SOCKET通信的TCP/IP通信
 * @author FanKaiyu
 * @date 2018-03-24
 * @version 2.1
*/

#pragma once
#ifndef _TCP_H
#define _TCP_H

#ifdef _WIN32
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <WinDNS.h>

#pragma comment(lib, "WS2_32")
#else
#include <netdb.h>
#endif
#include <sstream>
#include <string>
#include <thread>
#include <mutex>
#include <set>

#ifndef TM_TCPCLIENT_RECEIVE
#define TM_TCPCLIENT_RECEIVE WM_USER+1	/*!< 客户端数据接收数据的线程消息 */
#endif //!TM_TCPCLIENT_RECEIVE

#ifndef TM_TCPSERVER_ACCEPT
#define TM_TCPSERVER_ACCEPT WM_USER+2	/*!< 服务端接收客户端连接请求的线程消息*/
#endif //!TM_TCPSERVER_ACCEPT

#ifdef _WIN64
#ifndef ULL
#define ULL unsigned long long
#endif //!ULL
#ifndef GetTickCountEX
#define GetTickCountEX() GetTickCount64()
#endif //!GetTickCountEX()
#else
#ifndef ULL
#define ULL unsigned long
#endif //!ULL
#ifndef GetTickCountEX
#define GetTickCountEX() GetTickCount()
#endif//!GetTickCountEX()
#endif//_WIN64

namespace TCP
{
	/*!
	 * @brief 获取远程IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param char* IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, char* lpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//根据套接字获取地址信息
		if (::getpeername(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (inet_ntop(AF_INET, (void*)&addrClient.sin_addr, lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief 获取远程IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param wchar_t* IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, wchar_t* wlpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//根据套接字获取地址信息
		if (::getpeername(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (InetNtopW(AF_INET, (void*)&addrClient.sin_addr, wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief 获取远程IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param string& IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, std::string &strIP, unsigned short &usPort)
	{
		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetPeerIPandPort(socket, lpszIP, usPort) == false)
		{
			return false;
		}

		strIP = lpszIP;

		return true;
	}

	/*!
	 * @brief 获取远程IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param wstring& IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	 */
	static bool GetPeerIPandPort(const SOCKET socket, std::wstring &wstrIP, unsigned short &usPort)
	{
		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetPeerIPandPort(socket, wlpszIP, usPort) == false)
		{
			return false;
		}

		wstrIP = wlpszIP;

		return true;
	}

	/*!
	 * @brief 获取本地IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param char* IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	*/
	static bool GetSockIPandPort(const SOCKET socket, char* lpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//根据套接字获取地址信息
		if (::getsockname(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (inet_ntop(AF_INET, (void*)&addrClient.sin_addr, lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief 获取本地IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param wchar_t* IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	*/
	static bool GetSockIPandPort(const SOCKET socket, wchar_t* wlpszIP, unsigned short &usPort)
	{
		if (socket == INVALID_SOCKET)
		{
			return false;
		}

		sockaddr_in addrClient;
		int addrClientlen = sizeof(addrClient);

		//根据套接字获取地址信息
		if (::getsockname(socket, (SOCKADDR*)&addrClient, &addrClientlen) != 0)
		{
			return false;
		}

		if (InetNtopW(AF_INET, (void*)&addrClient.sin_addr, wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return false;
		}

		usPort = htons(addrClient.sin_port);

		return true;
	}

	/*!
	 * @brief 获取本地IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param string& IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	*/
	static bool GetSockIPandPort(const SOCKET socket, std::string &strIP, unsigned short &usPort)
	{
		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetSockIPandPort(socket, lpszIP, usPort) == false)
		{
			return false;
		}

		strIP = lpszIP;

		return true;
	}

	/*!
	 * @brief 获取本地IP地址和端口号
	 * @param const SOCKET socket通道描述符
	 * @param wstring& IP地址
	 * @param unsigned short& 端口
	 * @return bool 获取成功返回true,否则返回false
	*/
	static bool GetSockIPandPort(const SOCKET socket, std::wstring &wstrIP, unsigned short &usPort)
	{
		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		if (GetSockIPandPort(socket, wlpszIP, usPort) == false)
		{
			return false;
		}

		wstrIP = wlpszIP;

		return true;
	}

	/*!
	 * @brief 获取计算机本地IP
	 * @param char* 本地IP地址字符串
	 * @param int 制定端口号 值为-1则不指定
	 * @return bool 成功返回true,失败返回false
	*/
	static bool GetLocalIP(char* lpszIP, int nPort = -1)
	{
		//1.初始化wsa  
		WSADATA wsaData;
		int ret = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0)
		{
			return false;
		}

		//2.获取主机名  
		char hostname[256];
		ret = ::gethostname(hostname, sizeof(hostname));
		if (ret == SOCKET_ERROR)
		{
			return false;
		}

		//3.获取主机ip  
		struct addrinfo hints;
		struct addrinfo *res, *cur;
		struct sockaddr_in *addr;
		// char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;		/* Allow IPv4 */
		hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
		hints.ai_protocol = 0;			/* Any protocol */
		hints.ai_socktype = SOCK_STREAM;

		char lpszPort[6];
		if (nPort > -1)
		{
			sprintf_s(lpszPort, 6, "%d", nPort);
			getaddrinfo(hostname, lpszPort, &hints, &res);
		}
		else
		{
			getaddrinfo(hostname, NULL, &hints, &res);
		}

		if (ret == -1)
		{
			perror("getaddrinfo");
			return false;
		}

		for (cur = res; cur != NULL; cur = cur->ai_next)
		{
			addr = (struct sockaddr_in *)cur->ai_addr;

			sprintf_s(lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, "%d.%d.%d.%d",
				(*addr).sin_addr.S_un.S_un_b.s_b1,
				(*addr).sin_addr.S_un.S_un_b.s_b2,
				(*addr).sin_addr.S_un.S_un_b.s_b3,
				(*addr).sin_addr.S_un.S_un_b.s_b4);
		}

		freeaddrinfo(res);

		WSACleanup();

		return true;
	}

	/*!
	 * @brief 获取计算机本地IP
	 * @param char* 本地IP地址字符串
	 * @param int 指定端口号 值为-1则不指定
	 * @return bool 成功返回true,失败返回false
	*/
	static bool GetLocalIP(wchar_t* wlpszIP, int nPort = -1)
	{
		//1.初始化wsa  
		WSADATA wsaData;
		int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (ret != 0)
		{
			return false;
		}

		//2.获取主机名  
		char hostname[256];
		ret = gethostname(hostname, sizeof(hostname));
		if (ret == SOCKET_ERROR)
		{
			return false;
		}

		//3.获取主机ip  
		struct addrinfo hints;
		struct addrinfo *res, *cur;
		struct sockaddr_in *addr;
		// char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET;		/* Allow IPv4 */
		hints.ai_flags = AI_PASSIVE;	/* For wildcard IP address */
		hints.ai_protocol = 0;			/* Any protocol */
		hints.ai_socktype = SOCK_STREAM;

		char lpszPort[6];
		if (nPort > -1)
		{
			sprintf_s(lpszPort, 6, "%d", nPort);
			getaddrinfo(hostname, lpszPort, &hints, &res);
		}
		else
		{
			getaddrinfo(hostname, NULL, &hints, &res);
		}

		if (ret == -1)
		{
			perror("getaddrinfo");
			return false;
		}

		for (cur = res; cur != NULL; cur = cur->ai_next)
		{
			addr = (struct sockaddr_in *)cur->ai_addr;

			swprintf_s(wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH, L"%d.%d.%d.%d",
				(*addr).sin_addr.S_un.S_un_b.s_b1,
				(*addr).sin_addr.S_un.S_un_b.s_b2,
				(*addr).sin_addr.S_un.S_un_b.s_b3,
				(*addr).sin_addr.S_un.S_un_b.s_b4);
		}

		freeaddrinfo(res);

		WSACleanup();

		return true;
	}

	/*!
	 * @brief 获取计算机本地IP
	 * @param string& 本地IP地址字符串
	 * @param int 指定端口号 值为-1则不指定
	 * @return bool 成功返回true,失败返回false
	*/
	static bool GetLocalIP(std::string& strIP, int nPort = -1)
	{
		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (GetLocalIP(lpszIP, nPort) == false)
		{
			return false;
		}

		strIP = lpszIP;

		return true;
	}

	/*!
	 * @brief 获取计算机本地IP
	 * @param wstring& 本地IP地址字符串
	 * @param int 指定端口号 值为-1则不指定
	 * @return bool 成功返回true,失败返回false
	*/
	static bool GetLocalIP(std::wstring& wstrIP, int nPort = -1)
	{
		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (GetLocalIP(wlpszIP, nPort) == false)
		{
			return false;
		}

		wstrIP = wlpszIP;

		return true;
	}

	static const std::string DWORDToIPAddress(const DWORD dwIP)
	{
		SOCKADDR_IN addr;
		addr.sin_addr.s_addr = htonl(dwIP);

		char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (inet_ntop(AF_INET, (void*)& addr.sin_addr, lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return "";
		}

		return lpszIP;
	}

	static const std::wstring DWORDToIPAddressW(const DWORD dwIP)
	{
		SOCKADDR_IN addr;
		addr.sin_addr.s_addr = htonl(dwIP);

		wchar_t wlpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
		if (InetNtopW(AF_INET, (void*)& addr.sin_addr, wlpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH) == nullptr)
		{
			perror("fail to convert");
			return L"";
		}

		return wlpszIP;
	}

	static const DWORD IPAddressToDWORD(const char* strIP)
	{
		SOCKADDR_IN addr;
		if (strcmp(strIP, "") == 0)
		{
			inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr.s_addr);
		}
		else
		{
			inet_pton(AF_INET, strIP, &addr.sin_addr.s_addr);
		}

		return htonl(addr.sin_addr.s_addr);
	}

	static const DWORD IPAddressToDWORD(const wchar_t* wstrIP)
	{
		SOCKADDR_IN addr;
		if (wcscmp(wstrIP, L"") == 0)
		{
			InetPtonW(AF_INET,L"0.0.0.0", &addr.sin_addr.s_addr);
		}
		else
		{
			InetPtonW(AF_INET, wstrIP, &addr.sin_addr.s_addr);
		}

		return htonl(addr.sin_addr.s_addr);
	}
};

typedef void(__stdcall* UpdateServer)(void*, void*,SOCKET);

/*!
 * @class SocketClient
 * @brief 基于SOCKET通信的TCP客户端对象
*/
typedef class SocketClient
{
public:
	SocketClient();
	SocketClient(const char* strSrvIP, const char* strLocalIP,int nSrvPort = -1, int nLocalPort = -1);
	SocketClient(const wchar_t* wstrSrvIP, const wchar_t* wstrLocalIP, int nSrvPort = -1, int nLocalPort = -1);

	SocketClient(const std::string strLocalIP, std::string strSrvIP = "", int nLocalPort = -1, int nSrvPort = -1);
	~SocketClient();

protected:
	SOCKET m_socket;					/*!< 客户端绑定SOCKET通道 */
	std::string m_strLocalIP;			/*!< 客户端绑定IP地址 值为空时绑定随机IP;通道建立成功前设定的本地IP地址 */
	int m_nLocalPort;					/*!< 客户端绑定端口 值为-1时绑定随机端口;通道建立成功前设定的本地端口 */
	std::string m_strSrvIP;				/*!< 服务端连接IP地址 值为空时绑定随机IP;通道建立成功前设定的远程IP地址 */
	int m_nSrvPort;						/*!< 服务端连接端口 值为-1时绑定随机端口;通道建立成功前设定的远程端口 */
	std::string m_strSockIP;			/*!< 客户端本地IP地址 值为空时绑定随机IP;通道建立成功后返回的IP地址 */
	unsigned short m_usSockPort;		/*!< 客户端本地端口 值为-1时绑定随机端口;通道建立成功后返回的端口 */
	unsigned long m_ulThreadID;			/*!< 线程ID */
	ULL m_ullReceiveTime;				/*!< 数据接收的时间（系统运行时间） */
	ULL m_ullSendTime;					/*!< 数据发送的时间（系统运行时间） */
	SOCKET m_socketTmp;

private:
	std::thread* m_pReceiveThread;		/*!< 数据接收线程指针 */

public:
	/*!
	 * @brief 连接客户端
	 * @param const SOCKET 由服务器Accept接收的客户端SOCKET通道
	 * @param const unsigned long 线程ID
	 * @return bool 连接成功返回true,否则返回false
	*/
	bool ConnectPeer(const SOCKET socketClien, const unsigned long ulThreadID = 0);

	/*!
	 * @brief 连接服务端
	 * @param const char* 服务端IP地址
	 * @param const unsigned short 服务端端口
	 * @param const unsigned long 线程ID
	 * @return bool 连接成功返回true,否则返回false
	*/
	bool Connect(const unsigned long ulThreadID = 0);

	bool Connect(const char* strSrvIP, const unsigned short usSrvPort,const unsigned long ulThreadID = 0);
	bool Connect(const wchar_t* wstrSrvIP, const unsigned short usSrvPort,const unsigned long ulThreadID = 0);

	/*!
	 * @brief 中断连接
	*/
	void Break();

	/*!
	 * @brief 发送数据
	 * @param const unsigned char* 数据块指针
	 * @param const unsigned int 数据块大小
	 * @return bool 发送成功返回true,否则返回false
	*/
	bool Send(const unsigned char* pData,const unsigned int unSize);

	/*!
	 * @brief 是否连接
	 * @return bool 已连接返回true,否则返回false
	*/
	bool IsConnected();

	/*!
	 * @brief 获取IP地址以及端口
	 * @param string& 客户端IP地址
	 * @param int& 客户端端口
	 * @return bool 已连接返回true,未连接返回false
	*/
	bool GetIPandPort(std::string& strIP,int& nPort);

	/*!
	 * @brief 获取IP地址以及端口
	 * @param wstring& 客户端IP地址
	 * @param int& 客户端端口
	 * @return bool 已连接返回true,未连接返回false
	*/
	bool GetIPandPort(std::wstring& wstrIP, int& nPort);

	/*!
	 * @brief 设置IP地址以及端口
	 * @param const char* 客户端IP地址
	 * @param const int 客户端端口
	*/
	void SetLocalIPandPort(const char* strIP, int nPort = -1);

	/*!
	 * @brief 设置IP地址以及端口
	 * @param const wchar_t* 客户端IP地址
	 * @param const int 客户端端口
	*/
	void SetLocalIPandPort(const wchar_t* wstrIP, int nPort = -1);

	void SetServerIPandPort(const char* strIP, int nPort = -1);
	void SetServerIPandPort(const wchar_t* wstrIP, int nPort = -1);
protected:
	/*!
	 * @brief 连接客户端
	 * @param const SOCKET SOCKET通道
	 * @param const unsigned long 线程ID
	 * @return bool 连接成功返回true,否则返回false
	*/
	bool Connect(const SOCKET socketClien, const unsigned long ulThreadID = 0);

	/*!
	 * @brief 数据接收线程启动
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartReceiveThread();

	/*!
	 * @brief 数据接收线程终止
	*/
	void EndReceiveThread();

	/*!
	 * @brief 数据接收线程
	*/
	void ReceiveThread();

	/*!
	 * @brief 数据处理
	 * @param const unsigned char* 待处理的数据
	 * @param const unsigned int 待处理的数据大小
	*/
	virtual void ProcessData(const unsigned char* pData,const unsigned int unSize) = 0;
} TCPClient;

/*!
 * @class SockerServer
 * @brief 基于SOCKET通信的TCP服务端对象
*/
typedef class SocketServer
{
public:
	SocketServer();
	SocketServer(const unsigned short usPort, const char* strIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	SocketServer(const unsigned short usPort, const wchar_t* wstrIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	SocketServer(const unsigned short usPort, std::string strIP = "", UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	SocketServer(const unsigned short usPort, std::wstring wstrIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr);
	~SocketServer();

protected:
	SOCKET m_socket;					/*!< 服务端SOCKET通道 */
	std::string m_strIP;				/*!< 服务端绑定IP地址 */
	unsigned short m_usPort;			/*!< 服务端绑定端口 */
	std::thread *m_pAcceptThread;		/*!< 数据接收线程指针 */
	unsigned long m_ulThreadID;			/*!< 线程ID */
	UpdateServer m_funcPrt;
	void* m_funcParamPrt;

public:
	/*!
	 * @brief 启动服务端监听
	 * @param unsigned long 线程ID
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartListen(UpdateServer funcPrt = nullptr, void* ptr = nullptr,const unsigned long ulThreadID = 0);

	/*!
	 * @brief 启动服务端监听
	 * @param const unsigned short 服务端端口
	 * @param const char* 服务端绑定IP
	 * @param unsigned long 线程ID
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartListen(const unsigned short usPort, const char* strIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief 启动服务端监听
	 * @param const unsigned short 服务端端口
	 * @param const wchar_t* 服务端绑定IP
	 * @param unsigned long 线程ID
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartListen(const unsigned short usPort, const wchar_t* wstrIP, UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief 启动服务端监听
	 * @param const unsigned short 服务端端口
	 * @param string 服务端绑定IP
	 * @param unsigned long 线程ID
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartListen(const unsigned short usPort, std::string strIP = "", UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief 启动服务端监听
	 * @param const unsigned short 服务端端口
	 * @param wstring 服务端绑定IP
	 * @param unsigned long 线程ID
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartListen(const unsigned short usPort, std::wstring wstrIP = L"", UpdateServer funcPrt = nullptr, void* ptr = nullptr, const unsigned long ulThreadID = 0);

	/*!
	 * @brief 关闭服务端监听
	*/
	void CloseListen();

	/*!
	 * @brief 是否已监听
	 * @return bool 已监听返回true,否则返回false
	*/
	bool IsListened();

	/*!
	 * @brief 绑定IP和端口
	 * @param unsigned short 端口
	 * @param const char* IP地址
	*/
	void Bind(unsigned short usPort,const char* strIP);

	/*!
	 * @brief 绑定IP和端口
	 * @param unsigned short 端口
	 * @param const wchar_t* IP地址
	*/
	void Bind(unsigned short usPort, const wchar_t* wstrIP);

	/*!
	 * @brief 绑定IP和端口
	 * @param unsigned short 端口
	 * @param string IP地址
	*/
	void Bind(unsigned short usPort, std::string strIP = "");

	/*!
	 * @brief 绑定IP和端口
	 * @param unsigned short 端口
	 * @param wstring IP地址
	*/
	void Bind(unsigned short usPort, std::wstring wstrIP);

	/*!
	 * @brief 获取IP地址和端口
	 * @param char* 服务端IP地址
	 * @param unsigned short& 服务端端口
	*/
	void GetIPandPort(char* strIP,unsigned short& usPort) const;

	/*!
	 * @brief 获取IP地址和端口
	 * @param wchar_t* 服务端IP地址
	 * @param unsigned short& 服务端端口
	*/
	void GetIPandPort(wchar_t* wstrIP, unsigned short& usPort) const;

	/*!
	 * @brief 获取IP地址和端口
	 * @param string& 服务端IP地址
	 * @param unsigned short& 服务端端口
	*/
	void GetIPandPort(std::string& strIP, unsigned short& usPort) const;

	/*!
	 * @brief 获取IP地址和端口
	 * @param wstring& 服务端IP地址
	 * @param unsigned short& 服务端端口
	*/
	void GetIPandPort(std::wstring& wstrIP, unsigned short& usPort) const;

protected:
	/*!
	 * @brief 启动客户端接收线程
	 * @return bool 启动成功返回true,否则返回false
	*/
	bool StartAcceptThread();

	/*!
	 * @brief 关闭客户端接收线程
	*/
	void EndAcceptThread();

	/*!
	 * @brief 客户端接收线程
	*/
	void AcceptThread();

	/*!
	 * @brief 客户端接收
	 * @param const SOCKET 客户端SOCKET通道
	*/
	void AcceptClient(const SOCKET socket);

} TCPServer;

#endif //!_TCP_H