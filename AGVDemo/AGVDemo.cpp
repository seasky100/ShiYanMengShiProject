// AGVDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include "CAGV.h"

using namespace std;

void SendThread(CAGV* pAGV)
{
	Json::CharReaderBuilder m_JsonBuilder;
	Json::CharReader* m_pJsonReader;
	JSONCPP_STRING m_JstrErrs;

	m_pJsonReader = m_JsonBuilder.newCharReader();

	while (true)
	{
		DataBuffer data = CAGV::GetProcessPackage();

		if (data.m_unSize == 0)
		{
			continue;
		}

		Json::Value JsonRoot;
		const char* pstr = (char*)data.m_pbyBuffer;

		if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
		{
			printf(m_JstrErrs.c_str());

			m_JstrErrs.clear();
			JsonRoot.clear();

			continue;
		}

		Json::Value JsonMsg = JsonRoot["Message"];

		int nNo = Json_ReadInt(JsonMsg["No"]);				/*!< AGV编号 */
		string strCmd = Json_ReadString(JsonMsg["Cmd"]);	/*!< AGV指令 */
		string strType = Json_ReadString(JsonMsg["Type"]);	/*!< AGV类型 */

		if (strcmp(strType.c_str(), "AGV") != 0)
		{
			strType.clear();
			strCmd.clear();
			JsonMsg.clear();
			JsonRoot.clear();

			continue;
		}

		// 心跳报文
		if (strcmp(strCmd.c_str(), "Heartbeat") == 0)
		{
			Json::Value JsonResult = JsonRoot["Result"];

			Json::Value JsonRFID = JsonResult["RFID"];
			unsigned short usCurRFID = Json_ReadInt(JsonRFID["Cur"]);					/*!< 当前RFID卡 */
			pAGV->UpdateCurRFID(usCurRFID);
			unsigned short usNextRFID = Json_ReadInt(JsonRFID["Next"]);					/*!< 下一RFID卡 */
			unsigned short usOldNextRFID = pAGV->UpdateNextRFID(usNextRFID);	/*!< 旧下一RFID卡 */
			unsigned short usEndRFID = Json_ReadInt(JsonRFID["End"]);				/*!< 行进终点RFID卡 */
			unsigned short usOldEndRFID = pAGV->UpdateEndRFID(usEndRFID);	/*!< 旧行进终点RFID卡 */
			pAGV->UpdateStatus(Json_ReadInt(JsonResult["Status"]));
			pAGV->UpdateSpeed(Json_ReadInt(JsonResult["Speed"]));
			pAGV->UpdatePower(Json_ReadInt(JsonResult["Power"]));
			pAGV->UpdateCargo(Json_ReadInt(JsonResult["Cargo"]));
			pAGV->UpdateLifter(Json_ReadInt(JsonResult["Lifter"]));
			pAGV->UpdateAction(Json_ReadInt(JsonResult["Action"]));
			pAGV->UpdateActStatus(Json_ReadInt(JsonResult["ActStatus"]));
			pAGV->UpdateError(Json_ReadInt(JsonResult["Error"]));

			JsonRFID.clear();
			JsonResult.clear();
		}

		//printf(JsonRoot.toStyledString().c_str());

		strType.clear();
		strCmd.clear();
		JsonMsg.clear();
		JsonRoot.clear();
	}
	
	return;
}

int main()
{
	char achPort[6];
	char achIP[IP4_ADDRESS_STRING_BUFFER_LENGTH];
	int nPort = 0;
	CAGV agv(1);
	char achUser[6];
	int nSelect = 0;

	// 初始化
	memset(achPort, 0, 6);
	memset(achIP, 0, IP4_ADDRESS_STRING_BUFFER_LENGTH);
	memset(achUser, 0, 6);

	// 获取本地IP
	//TCP::GetLocalIP(achIP);
	printf("请输入IP地址:");
	cin.get(achIP, IP4_ADDRESS_STRING_BUFFER_LENGTH);
	cin.get();
	printf("请输入端口:");
	
	// 用户输入端口
	cin.get(achPort,6);
	// 获取端口
	nPort = stoi(achPort);
	cin.get();
	
	while (agv.Connect(achIP, nPort) == false)
	{
		printf("连接AGV失败!是否重新连接?\n[1]是\n[2]否\n请选择:");
		// 用户选择
		cin.get(achUser, 2);
		nSelect = stoi(achUser);
		cin.get();

		if (nSelect == 1)
		{
			continue;
		}
		else
		{
			break;
		}
	}

	nSelect = 0;

	printf("连接AGV成功");

	thread* pThread = new std::thread(&SendThread, &agv);

	while (true)
	{
		printf("控制AGV动作:\n[1]移动\n[2]重新移动\n[3]交通管制通行\n[4]急停\n[5]复位\n[6]输出当前RFID卡号\n请选择:");

		// 用户选择
		cin.get(achUser, 2);
		nSelect = stoi(achUser);
		cin.get();

		if (nSelect == 1)
		{
			printf("请输入移动目标RFID卡编号:");

			cin.get(achUser, 6);

			agv.MoveTo(stoi(achUser));

			cin.get();
		}
		else if (nSelect == 2)
		{
			agv.RemoveTo();
		}
		else if (nSelect == 3)
		{
			break;
		}
		else if (nSelect == 4)
		{
			agv.Scream();
		}
		else if (nSelect == 5)
		{
			agv.Reset();
		}
		else if (nSelect == 6)
		{
			printf("当前RFID卡编号:%d\n",agv.GetCurRFID());
		}
		else
		{
			break;
		}
	}

	cin.get();

	agv.Break();

	printf("退出程序");
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
