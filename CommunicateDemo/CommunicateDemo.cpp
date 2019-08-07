// CommunicateDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "CTCP.h"

int main()
{
	std::cout << "请输入服务端IP:";
	char lpszIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
	memset(lpszIP, 0, IP4_ADDRESS_STRING_BUFFER_LENGTH);
	std::cin.get(lpszIP, IP4_ADDRESS_STRING_BUFFER_LENGTH);
	std::cin.get();
	std::cin.clear();
	std::cin.sync();

	std::cout << "请输入服务端端口:";
	char lpszPort[6];
	memset(lpszPort, 0, 6);
	std::cin.get(lpszPort, 6);
	int nPort = std::stoi(lpszPort);
	std::cin.get();
	std::cin.clear();
	std::cin.sync();

	std::cout << "正在连接服务端，请稍后..." << std::endl;

	TCPClient tcpClient;
	if (tcpClient.Connect(lpszIP, nPort))
	{
		std::string strClientIP = "";
		int nClientPort = -1;
		tcpClient.GetIPandPort(strClientIP, nClientPort);
		std::cout << "连接成功!客户端IP:" << strClientIP <<":"<< nClientPort <<std::endl;
	}
	else
	{
		std::cout << "连接失败!" << std::endl;
	}

	tcpClient.Break();

	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
