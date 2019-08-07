// ZIGBEECallDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "../json/json.h"
#include "CZigbeeControler.h"
#include "../String/StringEx.h"
#include <map>

using namespace std;

int main()
{
	char achPort[6];
	CZigbeeControler zigbee(1);
	unsigned short usPort = 0;
	unsigned char byCount = 0;
	unsigned char byNo = 0;
	time_t cur;
	tm lt;
	map<unsigned char, ZGBCaller*> mapCallers;
	char ach[4];

	while (true)
	{
		// 初始化
		memset(achPort, 0, 6);

		printf("请输入串口号:");
		// 用户输入端口
		cin.get(achPort, 6);
		// 获取端口
		usPort = std::atoi(achPort);
		cin.get();

		printf("请输入呼叫器数量:");
		cin.get(achPort, 4);
		byCount = std::atoi(achPort);
		cin.get();

		if (zigbee.Open(usPort))
		{
			memset(ach, 0, 4);

			printf("连接Zigbee控制器%d成功!\n", 1);
			for (int i = 0; i < byCount; ++i)
			{
				printf("总数%d.当前%d.\n请输入呼叫器编号:",byCount,i+1);
				cin.get(achPort, 4);
				byNo = std::atoi(achPort);
				cin.get();
				printf("请输入呼叫器MAC地址:");
				cin.get(achPort,5);
				memcpy_s(ach, 4, achPort, 4);
				cin.get();

				mapCallers[byNo] = new ZGBCaller(byNo,0);
				mapCallers[byNo]->BindMACAddress(ach);

				zigbee.Heartbeat(mapCallers[byNo]);
			}

			break;
		}
		else
		{
			printf("连接Zigbee控制器%d失败!\n", 1);
		}
	}


	while (true)
	{
		DataBuffer package = ZGBCtrler::GetProcessPackage();

		if (package.m_unSize == 0)
		{
			continue;
		}

		// 记录发送的数据
		stringstream ssHex; // 16进制数据
		for (unsigned int unI = 0; unI < package.m_unSize; ++unI)
		{
			ssHex << StringEx::string_format("%02X ", (*(package.m_pbyBuffer + unI)) & 0xFF);
		}
		printf("16进制:%s\n", ssHex.str().c_str());

		unsigned char byIndex = 0;
		unsigned char byNo = *(package.m_pbyBuffer + byIndex++);				// AGV编号
		unsigned char byCmd = *(package.m_pbyBuffer + byIndex++);			// AGV指令

		time(&cur);
		localtime_s(&lt ,&cur);

		printf("\n%d-%d-%d %d:%d:%d\n", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday, lt.tm_hour, lt.tm_min, lt.tm_sec);//输出结果

		// 心跳报文
		switch (byCmd)
		{
		case FUNC_HEARTBEAT:
		{
			unsigned char bySignalCall = *(package.m_pbyBuffer + byIndex++);
			unsigned char bySignalCancelCall = *(package.m_pbyBuffer + byIndex++);
			unsigned char bySignalPass = *(package.m_pbyBuffer + byIndex++);
			unsigned char bySignalCancelPass = *(package.m_pbyBuffer + byIndex++);
			unsigned char bySignalNG = *(package.m_pbyBuffer + byIndex++);
			unsigned char bySignalCancelNG = *(package.m_pbyBuffer + byIndex++);
			unsigned char bySignalScream = *(package.m_pbyBuffer + byIndex++);

			printf("---呼叫器%d---\n呼叫信号:%d;\n取消呼叫信号:%d;\n放行信号:%d;\n取消放行信号:%d;\nNG信号:%d;\n取消NG信号:%d;\n全线急停信号:%d;\n------------"
				, byNo, bySignalCall, bySignalCancelCall, bySignalPass, bySignalCancelPass, bySignalNG, bySignalCancelNG, bySignalScream);

			zigbee.Reset(mapCallers[byNo]);
			break;
		}
		default:
		{
			for (map<unsigned char, ZGBCaller*>::iterator it = mapCallers.begin(); it != mapCallers.end(); it = mapCallers.erase(it))
			{
				delete it->second;
			}

			return -1;
		}
		}
	}

	for (map<unsigned char, ZGBCaller*>::iterator it = mapCallers.begin(); it != mapCallers.end(); it = mapCallers.erase(it))
	{
		delete it->second;
	}
	
	cin.get();

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
