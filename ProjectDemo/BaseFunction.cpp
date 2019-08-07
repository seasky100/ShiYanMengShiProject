#include "../ProjectApp2.0/pch.h"
#include "BaseFunction.h"

#ifdef _BASEFUNCTION_H
//#include "../AGVDemo/CAGV.cpp"
#include "../String/DebugPrint.h"
#include "../String/SystemLog.h"
#include "../Thread/ThreadEx.h"
#include "../String/Buffer.h"
#include "../String/StringEx.h"

#include "../json/json.h"
//#include "../json/json_value.cpp"
//#include "../json/json_reader.cpp"
//#include "../json/json_writer.cpp"


mutex BaseFunction::g_mutex;

BaseFunction::BaseFunction()
{
	m_strAGVs = "";
	m_strServer = "";
	m_strTraffic = "";

	m_pThreadMain = nullptr;
	m_hShutdownEvent = nullptr;

	m_pFunc = nullptr;
	m_pPtr = nullptr;

	m_pStrUpdateServer = nullptr;

	m_pJsonReader = m_JsonBuilder.newCharReader();

	
	// 初始化
	for (unsigned short i = 0; i < USHORT_MAX; ++i)
	{
		m_aRFIDArr[i].SetNo(i);
	}

	for (unsigned short i = 0; i < USHORT_MAX; ++i)
	{
		m_ausExhange[i] = i;
	}

	for (unsigned char i = 0; i < UCHAR_MAX; ++i)
	{
		m_bUpdateAGV[i] = false;
	}
}

BaseFunction::~BaseFunction()
{
	m_pFunc = nullptr;

	Close();
	Release();

	delete m_pJsonReader;

	ReleaseString();
}

bool BaseFunction::Init(UpdateStatus func, void* ptr)
{
	m_pFunc = func;
	m_pPtr = ptr;

	InitServer();
	InitTraffic();
	InitExchangeRFID();
	if (InitAGVs() == false)
	{
		return false;
	}

	DebugPrint::Print("基础功能初始化");
	SystemLog::Record("基础功能初始化");

	return true;
}

bool BaseFunction::Open(UpdateStatus func, void* ptr)
{
	m_pFunc = func;
	m_pPtr = ptr;

	if (StartMainThread() == false)
	{
		Close();

		return false;
	}

	if (m_tcpServer.StartListen(UpdateServer,this) == false)
	{
		Close();

		return false;
	}

	UpdateServer(m_tcpServer);

	DebugPrint::Print("基础功能启动");
	SystemLog::Record("基础功能启动");

	return true;
}

void BaseFunction::Close()
{
	EndMainThread();

	if (m_tcpServer.IsListened())
	{
		m_tcpServer.CloseListen();

		UpdateServer(m_tcpServer);
	}

	for (map<unsigned char, CAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end(); ++it)
	{
		if (it->second->IsConnected())
		{
			it->second->Release();

			DebugPrint::Printf("中断AGV%d连接", it->second->GetNo());
			SystemLog::Recordf("中断AGV%d连接", it->second->GetNo());

			UpdateAGV(it->second);
		}
	}

	for (unsigned short i = 0; i < USHORT_MAX; ++i)
	{
		m_aRFIDArr[i].Release();
	}

	DebugPrint::Print("基础功能关闭");
	SystemLog::Record("基础功能关闭");

	return;
}

void BaseFunction::Release()
{
	m_pFunc = nullptr;
	m_pPtr = nullptr;

	Close();

	for (map<unsigned char, CAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end(); it = m_mapAGVs.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned short, Traffic*>::iterator it = m_mapTraffics.begin(); it != m_mapTraffics.end(); it = m_mapTraffics.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned short, Json::Value*>::iterator it = m_mapStrUpdateAGVs.begin(); it != m_mapStrUpdateAGVs.end(); ++it)
	{
		it->second->clear();
	}

	if (m_pStrUpdateServer)
	{
		m_pStrUpdateServer->clear();
	}

	for (unsigned short i = 0; i < USHORT_MAX; ++i)
	{
		m_ausExhange[i] = i;
	}

	CloseHandle(m_hShutdownEvent);
	m_hShutdownEvent = nullptr;

	m_strAGVs = "";
	m_strServer = "";
	m_strTraffic = "";

	DebugPrint::Print("基础功能资源释放");
	SystemLog::Record("基础功能资源释放");

	return;
}

void BaseFunction::ReleaseString()
{
	for (map<unsigned short, Json::Value*>::iterator it = m_mapStrUpdateAGVs.begin(); it != m_mapStrUpdateAGVs.end(); it = m_mapStrUpdateAGVs.erase(it))
	{
		delete it->second;
	}

	if (m_pStrUpdateServer)
	{
		delete m_pStrUpdateServer;
		m_pStrUpdateServer = nullptr;
	}
}

void BaseFunction::JsonStringToServer(const char * strJson)
{
	m_strServer = strJson;

	return;
}

void BaseFunction::JsonStringToServer(const wchar_t * wstrJson)
{
	m_strServer = StringEx::wstring_to_string(wstrJson);

	return;
}

void BaseFunction::JsonFileToServer(const char * strFile)
{
	ifstream fin;
	fin.open(strFile);
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strServer = ostring.str();

	return;
}

void BaseFunction::JsonFileToServer(const wchar_t * wstrFile)
{
	ifstream fin;
	fin.open(StringEx::wstring_to_string(wstrFile).c_str());
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strServer = ostring.str();

	return;
}

void BaseFunction::JsonStringToAGVs(const char * strJson)
{
	m_strAGVs = strJson;

	return;
}

void BaseFunction::JsonStringToAGVs(const wchar_t * wstrJson)
{
	m_strAGVs = StringEx::wstring_to_string(wstrJson);

	return;
}

void BaseFunction::JsonFileToAGVs(const char * strFile)
{
	ifstream fin;
	fin.open(strFile);
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strAGVs = ostring.str();

	return;
}

void BaseFunction::JsonFileToAGVs(const wchar_t * wstrFile)
{
	ifstream fin;
	fin.open(StringEx::wstring_to_string(wstrFile).c_str());
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strAGVs = ostring.str();

	return;
}

void BaseFunction::JsonStringToTraffics(const char * strJson)
{
	m_strTraffic = strJson;

	return;
}

void BaseFunction::JsonStringToTraffics(const wchar_t * wstrJson)
{
	m_strTraffic = StringEx::wstring_to_string(wstrJson);

	return;
}

void BaseFunction::JsonFileToTraffics(const char * strFile)
{
	ifstream fin;
	fin.open(strFile);
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strTraffic = ostring.str();

	return;
}

void BaseFunction::JsonFileToTraffics(const wchar_t * wstrFile)
{
	ifstream fin;
	fin.open(StringEx::wstring_to_string(wstrFile).c_str());
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strTraffic = ostring.str();

	return;
}

void BaseFunction::JsonStringToRFIDExchanges(const char* strJson)
{
	m_strExchange = strJson;

	return;
}

void BaseFunction::JsonStringToRFIDExchanges(const wchar_t* wstrJson)
{
	m_strExchange = StringEx::wstring_to_string(wstrJson);

	return;
}

void BaseFunction::JsonFileToRFIDExchanges(const char* strFile)
{
	ifstream fin;
	fin.open(strFile);
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strExchange = ostring.str();

	return;
}

void BaseFunction::JsonFileToRFIDExchanges(const wchar_t* wstrFile)
{
	ifstream fin;
	fin.open(StringEx::wstring_to_string(wstrFile).c_str());
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	m_strExchange = ostring.str();

	return;
}

void BaseFunction::Accept(SOCKET socket)
{
	if (AcceptClient(socket))
	{
		return;
	}

	shutdown(socket, SD_BOTH);
	closesocket(socket);

	return;
}

void BaseFunction::InitServer()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strServer.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化服务端失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return;
	}
	m_mutexJson.unlock();

	string strIP = Json_ReadString(JsonRoot["IP"]);
	int nPort = Json_ReadInt(JsonRoot["Port"]);

	m_tcpServer.Bind(nPort, strIP);

	strIP.clear();
	JsonRoot.clear();

	return;
}

void BaseFunction::InitTraffic()
{
	Json::Value JsonRoot;

	const char* pstr = m_strTraffic.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化交通管制失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return;
	}
	m_mutexJson.unlock();

	Json::Value ObjectArray;
	ObjectArray = JsonRoot["Traffic"];

	for (size_t k = 0; k < ObjectArray.size(); ++k)
	{
		int nCtrl = Json_ReadInt(ObjectArray[(int)k]["Control"]);

		if (m_mapTraffics.find(nCtrl) == m_mapTraffics.end())
		{
			m_mapTraffics[nCtrl] = new Traffic(nCtrl);
		}

		Json::Value ArrInt;
		ArrInt = ObjectArray[(int)k]["Stop"];

		for (size_t i = 0; i < ArrInt.size(); ++i)
		{
			int nRFID = Json_ReadInt(ArrInt[(int)i]);

			m_mapTraffics[nCtrl]->AddStopRFID(nRFID);
		}

		ArrInt.clear();

		ArrInt = ObjectArray[(int)k]["Compare"];

		for (size_t i = 0; i < ArrInt.size(); ++i)
		{
			int nArea = Json_ReadInt(ArrInt[(int)i]);

			m_mapTraffics[nCtrl]->AddCompareArea(nArea);
		}

		ArrInt.clear();
	}

	ObjectArray.clear();
	JsonRoot.clear();

	return;
}

bool BaseFunction::InitAGVs()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strAGVs.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化AGV失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return false;
	}
	m_mutexJson.unlock();
	
	Json::Value ObjectArray, Param;
	ObjectArray = JsonRoot["AGV"];

	if (ObjectArray.size() == 0)
	{
		return false;
	}

	for (size_t k = 0; k < ObjectArray.size(); ++k, Param.clear())
	{
		int nNo = Json_ReadInt(ObjectArray[(int)k]["No"]);
		int nCurRFID = Json_ReadInt(ObjectArray[(int)k]["RFID"]["Cur"]);
		int nEndRFID = Json_ReadInt(ObjectArray[(int)k]["RFID"]["End"]);
		int nStatus = Json_ReadInt(ObjectArray[(int)k]["Status"]);
		Param = ObjectArray[(int)k]["Param"];
		string strSrvIP = Json_ReadString(Param["ServerIP"]);		/*!< 服务端IP:当程序为服务端,AGV为客户端时值为空;当AGV为服务端,程序为客户端时值为AGV的IP地址 */
		int nSrvPort = Json_ReadInt(Param["ServerPort"]);			/*!< 服务端端口:当程序为服务端,AGV为客户端时值为-1;当AGV为服务端,程序为客户端时值为AGV的端口 */
		string strClientIP = Json_ReadString(Param["ClientIP"]);	/*!< 客户端IP:当程序为服务端,AGV为客户端时值为AGV的IP地址;当AGV为服务端,程序为客户端时值为空 */
		int nClientPort = Json_ReadInt(Param["ClientPort"]);		/*!< 客户端端口:当程序为服务端,AGV为客户端时值为AGV的端口,值为-1时则为随机端口;当AGV为服务端,程序为客户端时值为程序指定端口,值为-1时则为随机端口 */

		if (m_mapAGVs.find(nNo) == m_mapAGVs.end())
		{
			m_mapAGVs[nNo] = new CAGV(nNo, UpdateAGV,this, strClientIP,strSrvIP, nClientPort,nSrvPort);
		}

		unsigned short usOldRFID = m_mapAGVs[nNo]->UpdateCurRFID(nCurRFID);
		m_mapAGVs[nNo]->UpdateEndRFID(nEndRFID);
		m_mapAGVs[nNo]->UpdateStatus(nStatus);

		// 解锁旧RFID
		m_aRFIDArr[usOldRFID].Unlock(m_mapAGVs[nNo]);

		// 锁定新RFID
		m_aRFIDArr[nCurRFID].Lock(m_mapAGVs[nNo]);

		UpdateAGV(m_mapAGVs[nNo]);

		strSrvIP.clear();
		strClientIP.clear();
	}

	ObjectArray.clear();
	JsonRoot.clear();

	return true;
}

bool BaseFunction::InitExchangeRFID()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strExchange.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化RFID转换表失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return false;
	}
	m_mutexJson.unlock();

	Json::Value ObjectArray, Source;
	ObjectArray = JsonRoot["Exchange"];

	if (ObjectArray.size() == 0)
	{
		return false;
	}

	for (size_t k = 0; k < ObjectArray.size(); ++k, Source.clear())
	{
		Source = ObjectArray[(int)k]["Source"];
		int nExchange = Json_ReadInt(ObjectArray[(int)k]["Exchange"]);
		int nSource = 0;

		for (size_t s = 0; s < Source.size(); ++s, nSource = 0)
		{
			nSource = Json_ReadInt(Source[(int)s]);
			m_ausExhange[nSource] = nExchange;
		}
	}

	return false;
}

void BaseFunction::ProcessAGVPackage()
{
	DataBuffer package = CAGV::GetProcessPackage();

	if (package.m_unSize == 0)
	{
		return;
	}

	Json::Value JsonRoot;

	const char* pstr = (char*)package.m_pbyBuffer;

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("处理AGV报文失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return;
	}
	m_mutexJson.unlock();

	Json::Value JsonMsg = JsonRoot["Message"];

	int nNo = Json_ReadInt(JsonMsg["No"]);				/*!< AGV编号 */
	string strCmd = Json_ReadString(JsonMsg["Cmd"]);	/*!< AGV指令 */
	string strType = Json_ReadString(JsonMsg["Type"]);	/*!< AGV类型 */

	if (strcmp(strType.c_str(), "AGV") != 0)
	{
		strType.clear();
		strType.clear();
		JsonMsg.clear();

		return;
	}

	if (m_mapAGVs.find(nNo) == m_mapAGVs.end())
	{
		strType.clear();
		strType.clear();
		JsonMsg.clear();

		return;
	}

	// 心跳报文
	if (strcmp(strCmd.c_str(), "Heartbeat") == 0)
	{
		Json::Value JsonResult = JsonRoot["Result"];

		Json::Value JsonRFID = JsonResult["RFID"];

		unsigned short usCurRFID = Json_ReadInt(JsonRFID["Cur"]);					/*!< 当前RFID卡 */
		usCurRFID = ExchangeRFID(usCurRFID);

		if (usCurRFID != 0)
		{
			unsigned short usOldRFID = m_mapAGVs[nNo]->UpdateCurRFID(usCurRFID);	/*!< 上一RFID卡*/

			m_aRFIDArr[usOldRFID].Unlock(m_mapAGVs[nNo]);

			m_aRFIDArr[usCurRFID].Lock(m_mapAGVs[nNo]);
		}

		unsigned short usNextRFID = Json_ReadInt(JsonRFID["Next"]);					/*!< 下一RFID卡 */
		unsigned short usOldNextRFID = m_mapAGVs[nNo]->UpdateNextRFID(usNextRFID);	/*!< 旧下一RFID卡 */

		m_aRFIDArr[usOldNextRFID].PreformUnlock(m_mapAGVs[nNo]);

		m_aRFIDArr[usNextRFID].PreformLock(m_mapAGVs[nNo]);

		unsigned short usEndRFID = Json_ReadInt(JsonRFID["End"]);				/*!< 行进终点RFID卡 */
		unsigned short usOldEndRFID = m_mapAGVs[nNo]->UpdateEndRFID(usEndRFID);	/*!< 旧行进终点RFID卡 */

		m_aRFIDArr[usOldEndRFID].PreformUnlock(m_mapAGVs[nNo]);

		m_aRFIDArr[usEndRFID].PreformLock(m_mapAGVs[nNo]);

		m_mapAGVs[nNo]->UpdateStatus(Json_ReadInt(JsonResult["Status"]));
		m_mapAGVs[nNo]->UpdateSpeed(Json_ReadInt(JsonResult["Speed"]));
		m_mapAGVs[nNo]->UpdatePower(Json_ReadInt(JsonResult["Power"]));
		m_mapAGVs[nNo]->UpdateCargo(Json_ReadInt(JsonResult["Cargo"]));
		m_mapAGVs[nNo]->UpdateLifter(Json_ReadInt(JsonResult["Lifter"]));
		m_mapAGVs[nNo]->UpdateAction(Json_ReadInt(JsonResult["Action"]));
		m_mapAGVs[nNo]->UpdateActStatus(Json_ReadInt(JsonResult["ActStatus"]));
		m_mapAGVs[nNo]->UpdateError(Json_ReadInt(JsonResult["Error"]));

		UpdateAGV(m_mapAGVs[nNo]);

		ProcessAGVStatus(m_mapAGVs[nNo]);

		JsonRFID.clear();
		JsonResult.clear();
	}
	// 其他报文
	else
	{

		strType.clear();
		strType.clear();
		JsonMsg.clear();

		return;
	}

	strType.clear();
	strType.clear();
	JsonMsg.clear();

	return;
}

void BaseFunction::TrafficControl()
{
	for (map<unsigned short, Traffic*>::iterator it = m_mapTraffics.begin(); it != m_mapTraffics.end(); ++it)
	{
		unsigned short usCtrlRFID = it->second->GetControlRFID();	/*!< 管制RFID卡 */

		TrafficControl(m_aRFIDArr[usCtrlRFID].GetLocker());
	}

	return;
}

void BaseFunction::TrafficControl(void * pUser)
{
	// 指针无效
	if (pUser == nullptr)
	{
		return;
	}

	CAGV* pAGV = (CAGV*)pUser;	/*!< AGV指针 */

	// AGV不处于交通管制状态
	if (pAGV->GetStatus() != _AGVSTA_TRAFFICSTOP)
	{
		return;
	}

	unsigned short usCurRFID = pAGV->GetCurRFID();	/*!< AGV当前RFID卡 */

	if (m_mapTraffics.find(usCurRFID) == m_mapTraffics.end())
	{
		DebugPrint::Printf("AGV%d交通管制控制失败:\n不存在有效的交通管制坐标", pAGV->GetNo());
		return;
	}

	/*
	 * 禁行区域检测
	*/
	set<unsigned short> setStop = m_mapTraffics[usCurRFID]->GetStopArray();	/*!< 禁行RFID组 */

	for (set<unsigned short>::iterator it = setStop.begin(); it != setStop.end(); ++it)
	{
		unsigned short usStopRFID = *it;	/*!< 禁行RFID卡 */

		CAGV* pStopAGV = (CAGV*)m_aRFIDArr[usStopRFID].GetLocker();	/*!< 禁行RFID卡上的行进AGV指针 */

		// 禁行RFID卡上存在AGV
		if (pStopAGV != nullptr && pStopAGV != pAGV)
		{
			DebugPrint::Printf("AGV%d禁止放行:\n禁行区域存在行进AGV%d", pAGV->GetNo(), pStopAGV->GetNo());
			return;
		}
	}

	setStop.clear();

	/*
	 * 比对区域检测
	*/
	set<unsigned short> setCompare = m_mapTraffics[usCurRFID]->GetCompareArray();

	for (set<unsigned short>::iterator it = setCompare.begin(); it != setCompare.end(); ++it)
	{
		unsigned short usCompareArea = *it;

		if (m_mapTraffics.find(usCompareArea) == m_mapTraffics.end())
		{
			continue;
		}

		/*
		 * 禁行区域检测
		*/
		setStop = m_mapTraffics[usCompareArea]->GetStopArray();	/*!< 禁行RFID组 */

		for (set<unsigned short>::iterator itStop = setStop.begin(); itStop != setStop.end(); ++itStop)
		{
			unsigned short usStopRFID = *itStop;	/*!< 禁行RFID卡 */

			CAGV* pStopAGV = (CAGV*)m_aRFIDArr[usStopRFID].GetLocker();	/*!< 禁行RFID卡上的行进AGV指针 */

			// 禁行RFID卡上存在AGV
			if (pStopAGV != nullptr && pStopAGV != pAGV)
			{
				DebugPrint::Printf("AGV%d禁止放行:\n禁行区域存在行进AGV%d", pAGV->GetNo(), pStopAGV->GetNo());
				return;
			}
		}

		CAGV* pAGVCmp = (CAGV*)m_aRFIDArr[usCompareArea].GetLocker();

		if (pAGVCmp == nullptr)
		{
			continue;
		}

		unsigned char byStatus = pAGVCmp->GetStatus();

		if(byStatus != _AGVSTA_TRAFFICSTOP && byStatus != _AGVSTA_TRAFFICPASS && byStatus != _AGVSTA_RUN)
		{
			continue;
		}

		long long nResul = m_aRFIDArr[usCurRFID].Compare(m_aRFIDArr[usCompareArea]);	/*!< 比对时间差值 */

		if (nResul > 0)
		{
			DebugPrint::Printf("AGV%d禁止放行:\n非最先到达该区域的AGV", pAGV->GetNo());
			return;
		}
	}

	pAGV->TrafficPass(usCurRFID);

	return;
}

void BaseFunction::ObsControl(void * pUser)
{
	// 指针无效
	if (pUser == nullptr)
	{
		return;
	}

	CAGV* pAGV = (CAGV*)pUser;								/*!< AGV指针 */

	unsigned short usCurRFID = pAGV->GetCurRFID();			/*!< 当前行进RFID卡 */
	unsigned short usNextRFID = pAGV->GetNextRFID();		/*!< 行进下一RFID卡 */
	char chSpeed = pAGV->GetSpeed();						/*!< 当前速度 */
	unsigned short usAction = pAGV->GetAction();			/*!< 动作信息 */
	unsigned char byActStatus = pAGV->GetActionStatus();	/*!< 动作状态 */
	unsigned char byStatus = pAGV->GetStatus();				/*!< 状态信息 */

	// AGV不在运行或待机的状态
	if (byStatus != _AGVSTA_RUN && byStatus != _AGVSTA_WAIT)
	{
		return;
	}

	if (usCurRFID != 0)
	{
		CAGV* pLockerAGV = (CAGV*)m_aRFIDArr[usCurRFID].GetLocker();

		if (pLockerAGV != nullptr && pLockerAGV != pAGV)
		{
			char chLockerSpeed = pLockerAGV->GetSpeed();			/*!< 前方AGV的速度 */
			unsigned char byLockerStatus = pLockerAGV->GetStatus();	/*!< 前方AGV的状态*/

			if (byLockerStatus == _AGVSTA_RUN || byLockerStatus == _AGVSTA_OBSDOWN)
			{
				// 速度过快则减速
				if (chSpeed > chLockerSpeed)
				{
					pAGV->SpeedControl(chLockerSpeed);
				}
				else
				{
					// 移动暂停则继续
					if ((usAction == _AGVACT_MOVE || usAction == _AGVACT_REMOVE) && byActStatus == _AGVACTSTA_PAUSE)
					{
						pAGV->Continue();
					}
				}
			}
			else
			{
				if (chSpeed > 0 && (usAction == _AGVACT_MOVE || usAction == _AGVACT_REMOVE))
				{
					pAGV->Pause();
				}
			}
		}
	}
	
	if (usNextRFID != 0)
	{
		CAGV* pLockerAGV = (CAGV*)m_aRFIDArr[usNextRFID].GetLocker();

		if (pLockerAGV != nullptr && pLockerAGV != pAGV)
		{
			char chLockerSpeed = pLockerAGV->GetSpeed();
			unsigned char byLockerStatus = pLockerAGV->GetStatus();

			if (byLockerStatus == _AGVSTA_RUN || byLockerStatus == _AGVSTA_OBSDOWN)
			{
				if ((usAction == _AGVACT_MOVE || usAction == _AGVACT_REMOVE) && byActStatus == _AGVACTSTA_PAUSE)
				{
					pAGV->Continue();
				}
			}
			else
			{
				if (chSpeed > 0 && (usAction == _AGVACT_MOVE || usAction == _AGVACT_REMOVE))
				{
					pAGV->Pause();
				}
			}
		}
	}
	
	return;
}

bool BaseFunction::AcceptAGV(SOCKET socket)
{
	// 连接AGV
	for (map<unsigned char, CAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end(); ++it)
	{
		if (it->second->Connect(socket))
		{
			DebugPrint::Printf("已连接AGV%d", it->second->GetNo());
			SystemLog::Recordf("已连接AGV%d", it->second->GetNo());

			it->second->UpdateError(_AGVERR_NONE);

			UpdateAGV(it->second);

			return true;
		}
	}

	return false;
}

void BaseFunction::ConnectAGV()
{
	for (map<unsigned char, CAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end(); ++it)
	{
		it->second->StartConnectThread();
	}

	return;
}

bool BaseFunction::AcceptClient(SOCKET socket)
{
	if (AcceptAGV(socket))
	{
		return true;
	}

	return false;
}

void BaseFunction::ProcessAGVStatus(void* pUser)
{
	if (pUser == nullptr)
	{
		return;
	}

	CAGV* pAGV = (CAGV*)pUser;

	if (pAGV->GetStatus() == _AGVSTA_TRAFFICSTOP)
	{
		TrafficControl(pAGV);
	}
	else
	{
		ObsControl(pAGV);
	}
}

const unsigned short BaseFunction::ExchangeRFID(const unsigned short usRFID)
{
	return m_ausExhange[usRFID];
}

void BaseFunction::MainThread()
{
	ULL ullTime = 0;
	while (true)
	{
		Function();
		
		if (WaitForSingleObject(m_hShutdownEvent, 100) == WAIT_OBJECT_0)
		{
			break;
		}
	}

	return;
}

bool BaseFunction::StartMainThread()
{
	EndMainThread();

	if (m_hShutdownEvent)
	{
		ResetEvent(m_hShutdownEvent);
	}
	else
	{
		m_hShutdownEvent = CreateEvent(nullptr, true, false, nullptr);
	}

	m_pThreadMain = new std::thread(&BaseFunction::MainThread, this);

	if (m_pThreadMain)
	{
		return true;
	}

	return false;
}

void BaseFunction::EndMainThread()
{
	if (m_pThreadMain == nullptr)
	{
		return;
	}

	if (m_pThreadMain->joinable())
	{
		SetEvent(m_hShutdownEvent);

		m_pThreadMain->join();
	}

	delete m_pThreadMain;
	m_pThreadMain = nullptr;

	return;
}

void BaseFunction::Function()
{
	ConnectAGV();

	ProcessAGVPackage();

	//TrafficControl();
}

void BaseFunction::UpdateAGV(void* pAGV, void* ptr)
{
	return ((BaseFunction*)ptr)->UpdateAGV((CAGV*)pAGV);
}

void BaseFunction::UpdateServer(void* pServer, void* ptr, SOCKET socket)
{
	if (socket != INVALID_SOCKET)
	{
		((BaseFunction*)ptr)->AcceptClient(socket);

		return;
	}

	return ((BaseFunction*)ptr)->UpdateServer(*(TCPServer*)pServer);
}

void BaseFunction::UpdateAGV(CAGV * pAGV)
{
	if (m_pFunc == nullptr)
	{
		return;
	}

	if (pAGV == nullptr)
	{
		return;
	}
	/*
	{
		"No":1,
		"Connect":true,
		"Status":0,
		"RFID":
		{
			"Old":0,
			"Cur":0,
			"Next":0,
			"End":0
		},
		"Message":
		{
			"IP":"",
			"Port":"",
			"Status":
			{
				"Value":"",
				"Color":""
			},
			"Speed":"",
			"Power":
			{
				"Value":"",
				"Color":""
			},
			"Cargo":
			{
				"Value":"",
				"Color":""
			},
			"Lifter":"",
			"Error":
			{
				"Value":"",
				"Color":""
			},
			"Action":"",
		},
		"View":
		{
			"Obs":false,
			"Run":false,
			"Unvol":true,
			"Cargo":false,
			"Error":false,
			"Action":0
		}
	}
	*/
	string strMsg = "";							/*!< 信息 */
	string strColor = "";						/*!< 颜色 */

	string strIP = "";	/*!< IP地址 */
	int nPort = -1;		/*!< 端口*/
	pAGV->GetIPandPort(strIP, nPort);
	unsigned char byNo = pAGV->GetNo();						/*!< 编号 */
	unsigned char byStatus = pAGV->GetStatus();				/*!< 运行状态 */
	unsigned char byPower = pAGV->GetPower();				/*!< 电量 */
	unsigned char byCargo = pAGV->GetCargo();				/*!< 载货状态 */
	unsigned char byLifter = pAGV->GetLifter();				/*!< 升降杆状态 */
	char chError = pAGV->GetError();						/*!< 异常信息 */
	unsigned short usAction = pAGV->GetAction();			/*!< 动作信息 */
	unsigned char byActStatus = pAGV->GetActionStatus();	/*!< 动作状态 */
	bool bObs = false;										/*!< 避障标识 */
	bool bStop = true;										/*!< 停止标识 */
	bool Unvol = false;										/*!< 低电量标识 */
	bool bCargo = true;										/*!< 载货标识 */
	bool bNone = false;										/*!< 无异常标识 */
	char chLoad = 0;										/*!< 上/下料状态 0无动作 1上料中 2下料中 */
		
	Json::Value JsonRoot, JsonRFID,JsonMsg, JsonView, JsonStatus, JsonPower, JsonCargo, JsonError;

	JsonRoot["Type"] = Json::Value("AGV");
	JsonRoot["No"] = Json::Value(byNo);
	JsonRoot["Connect"] = Json::Value(pAGV->IsConnected());
	JsonRoot["Status"] = Json::Value(pAGV->GetStatus());

	JsonRFID["Old"] = Json::Value(pAGV->GetOldRFID());
	JsonRFID["Cur"] = Json::Value(pAGV->GetCurRFID());
	JsonRFID["Next"] = Json::Value(pAGV->GetNextRFID());
	JsonRFID["End"] = Json::Value(pAGV->GetEndRFID());

	JsonMsg["IP"] = Json::Value(strIP.c_str());
	JsonMsg["Port"] = Json::Value(StringEx::string_format("%d",nPort).c_str());

	strMsg = "";
	strColor = "";
	switch (byStatus)
	{
	case _AGVSTA_WAIT:
	{
		strMsg = "待机";
		strColor = "White";
		break;
	}
	case _AGVSTA_RUN:
	{
		strMsg = "运行";
		strColor = "Green";
		bStop = false;
		break;
	}
	case _AGVSTA_STOP:
	{
		strMsg = "停止";
		strColor = "Yellow";
		break;
	}
	case _AGVSTA_SCREAM:
	{
		strMsg = "急停";
		strColor = "Red";
		break;
	}
	case _AGVSTA_FIND:
	{
		strMsg = "寻磁";
		strColor = "Yellow";
		break;
	}
	case _AGVSTA_OBSDOWN:
	{
		strMsg = "避障减速";
		strColor = "Green";
		bStop = false;
		bObs = true;
		break;
	}
	case _AGVSTA_OBSSTOP:
	{
		strMsg = "避障停止";
		strColor = "Yellow";
		bObs = true;
		break;
	}
	case _AGVSTA_MOBSSTOP:
	{
		strMsg = "碰撞停止";
		strColor = "Red";
		bObs = true;
		break;
	}
	case _AGVSTA_TRAFFICSTOP:
	{
		strMsg = "交通管制停止";
		strColor = "Yellow";
		break;
	}
	case _AGVSTA_ARM:
	{
		strMsg = "机械臂放行";
		strColor = "Green";
		break;
	}
	case _AGVSTA_ALLSCREAM:
	{
		strMsg = "全线急停";
		strColor = "Red";
		break;
	}
	case _AGVSTA_TRAFFICPASS:
	{
		strMsg = "交通管制放行";
		strColor = "Green";
		break;
	}
	}
	
	JsonStatus["Value"] = Json::Value(StringEx::AcsiiToUtf8(strMsg.c_str()));
	JsonStatus["Color"] = Json::Value(strColor.c_str());

	JsonMsg["Speed"] = Json::Value(StringEx::string_format("%d%%",pAGV->GetSpeed()).c_str());

	strColor = "";
	if (byPower == _AGVPOWER_EMPTY)
	{
		strColor = "Red";
		Unvol = true;
	}
	else if (byPower <= _AGVPOWER_LOW)
	{
		strColor = "Red";
		Unvol = true;
	}
	else if (byPower <= _AGVPOWER_NORMAL)
	{
		strColor = "White";
	}
	else if (byPower <= _AGVPOWER_HIGH)
	{
		strColor = "Green";
	}
	else if (byPower <= _AGVPOWER_FULL)
	{
		strColor = "Green";
	}
	JsonPower["Value"] = Json::Value(StringEx::string_format("%d%%",byPower).c_str());
	JsonPower["Color"] = Json::Value(strColor.c_str());

	strMsg = "";
	strColor = "";
	switch (byCargo)
	{
	case _AGVCARGO_EMPTY:
	{
		strMsg = "空载";
		strColor = "Green";
		bCargo = false;
		break;
	}
	case _AGVCARGO_FULL:
	{
		strMsg = "满载";
		strColor = "Red";
		break;
	}
	default:
	{
		strMsg = StringEx::string_format("%d件", byCargo);
		break;
	}
	}
	JsonCargo["Value"] = Json::Value(StringEx::AcsiiToUtf8(strMsg.c_str()));
	JsonCargo["Color"] = Json::Value(strColor.c_str());

	strMsg = "";	
	switch (byLifter)
	{
	case _AGVLIFTER_UP:
	{
		strMsg = "升起";
		break;
	}
	case _AGVLIFTER_DOWN:
	{
		strMsg = "落下";
		break;
	}
	}
	JsonMsg["Lifter"] = Json::Value(StringEx::AcsiiToUtf8(strMsg.c_str()));

	strMsg = "";
	strColor = "Red";
	switch (chError)
	{
	case _AGVERR_NONE:
	{
		strMsg = "正常";
		strColor = "Green";
		bNone = true;
		break;
	}
	case _AGVERR_MISS:
	{
		strMsg = "脱磁";
		break;
	}
	case _AGVERR_PATH:
	{
		strMsg = "路径异常";
		break;
	}
	case _AGVERR_LOCATE:
	{
		strMsg = "坐标异常";
		break;
	}
	case _AGVERR_DIRECTION:
	{
		strMsg = "定向异常";
		break;
	}
	case _AGVERR_ACT:
	{
		strMsg = "动作异常";
		break;
	}
	case _AGVERR_COMM:
	{
		strMsg = "通信异常";
		break;
	}
	case _AGVERR_LIFTUP:
		strMsg = "顶升异常";
		break;
	}
	JsonError["Value"] = Json::Value(StringEx::AcsiiToUtf8(strMsg.c_str()));
	JsonError["Color"] = Json::Value(strColor.c_str());

	strMsg = "";
	switch (usAction)
	{
	case _AGVACT_NONE:
	{
		strMsg = "无动作";

		break;
	}
	case _AGVACT_MOVE:
	{
		switch (byActStatus)
		{
		case _AGVACTSTA_NONE:
		{
			strMsg = "移动未执行";
			break;
		}
		case _AGVACTSTA_EXE:
		{
			strMsg = "移动中";
			break;
		}
		case _AGVACTSTA_FINISH:
		{
			strMsg = "移动完成";
			break;
		}
		case _AGVACTSTA_PAUSE:
		{
			strMsg = "移动暂停";
			break;
		}
		}

		break;
	}
	case _AGVACT_REMOVE:
	{
		switch (byActStatus)
		{
		case _AGVACTSTA_NONE:
		{
			strMsg = "重新移动未执行";
			break;
		}
		case _AGVACTSTA_EXE:
		{
			strMsg = "重新移动中";
			break;
		}
		case _AGVACTSTA_FINISH:
		{
			strMsg = "重新移动完成";
			break;
		}
		case _AGVACTSTA_PAUSE:
		{
			strMsg = "重新移动暂停";
			break;
		}
		}

		break;
	}
	case _AGVACT_LIFTUP:
	{
		switch (byActStatus)
		{
		case _AGVACTSTA_NONE:
		{
			strMsg = "升起升降杆未执行";
			chLoad = 1;
			break;
		}
		case _AGVACTSTA_EXE:
		{
			strMsg = "升降杆上升中";
			chLoad = 1;
			break;
		}
		case _AGVACTSTA_FINISH:
		{
			strMsg = "升降杆已升起";
			break;
		}
		case _AGVACTSTA_PAUSE:
		{
			strMsg = "升降杆上升暂停";
			chLoad = 1;
			break;
		}
		}

		break;
	}
	case _AGVACT_LIFTDOWN:
	{
		switch (byActStatus)
		{
		case _AGVACTSTA_NONE:
		{
			strMsg = "落下升降杆未执行";
			chLoad = 2;
			break;
		}
		case _AGVACTSTA_EXE:
		{
			strMsg = "升降杆下降中";
			chLoad = 2;
			break;
		}
		case _AGVACTSTA_FINISH:
		{
			strMsg = "升降杆已落下";
			break;
		}
		case _AGVACTSTA_PAUSE:
		{
			strMsg = "升降杆下降暂停";
			chLoad = 2;
			break;
		}
		}

		break;
	}
	}
	JsonMsg["Action"] = Json::Value(StringEx::AcsiiToUtf8(strMsg.c_str()));

	JsonView["Obs"] = Json::Value(bObs);
	JsonView["Run"] = Json::Value(!bStop);
	JsonView["Unvol"] = Json::Value(Unvol);
	JsonView["Cargo"] = Json::Value(bCargo);
	JsonView["Error"] = Json::Value(!bNone);
	JsonView["Action"] = Json::Value(chLoad);

	JsonMsg["Status"] = JsonStatus;
	JsonMsg["Power"] = JsonPower;
	JsonMsg["Cargo"] = JsonCargo;
	JsonMsg["Error"] = JsonError;

	JsonRoot["RFID"] = JsonRFID;
	JsonRoot["Message"] = JsonMsg;
	JsonRoot["View"] = JsonView;

	bool bUpdate = false;
	if (m_mapStrUpdateAGVs.find(byNo) == m_mapStrUpdateAGVs.end())
	{
		g_mutex.lock();
		m_mapStrUpdateAGVs[byNo] = new Json::Value(JsonRoot);
		g_mutex.unlock();

		bUpdate = true;
	}
	else
	{
		if (JsonRoot != *m_mapStrUpdateAGVs[byNo])
		{
			if (JsonRoot["RFID"] != (*m_mapStrUpdateAGVs[byNo])["RFID"]
				|| JsonRoot["Status"] != (*m_mapStrUpdateAGVs[byNo])["Status"])
			{
				m_bUpdateAGV[byNo - 1] = true;
			}

			g_mutex.lock();
			m_mapStrUpdateAGVs[byNo]->clear();
			*m_mapStrUpdateAGVs[byNo] = JsonRoot;
			g_mutex.unlock();

			bUpdate = true;
		}
	}
	
	if (bUpdate == false)
	{
		JsonRoot.clear();
		JsonRFID.clear();
		JsonMsg.clear();
		JsonView.clear();
		JsonStatus.clear();
		JsonPower.clear();
		JsonCargo.clear();
		JsonError.clear();
		strMsg.clear();
		strColor.clear();
		strIP.clear();

		return;
	}

	if (m_pFunc && m_pFunc(m_mapStrUpdateAGVs[byNo], m_pPtr) == false)
	{
		g_mutex.lock();
		m_mapStrUpdateAGVs[byNo]->clear();
		g_mutex.unlock();
	}

	JsonRoot.clear();
	JsonRFID.clear();
	JsonMsg.clear();
	JsonView.clear();
	JsonStatus.clear();
	JsonPower.clear();
	JsonCargo.clear();
	JsonError.clear();
	strMsg.clear();
	strColor.clear();
	strIP.clear();

	return;
}

void BaseFunction::UpdateServer(TCPServer & server)
{
	if (m_pFunc == nullptr)
	{
		return;
	}

	string strIP = "";			/*!< IP地址 */
	unsigned short usPort = 0;	/*!< 端口 */
	server.GetIPandPort(strIP, usPort);

	Json::Value JsonRoot;
	JsonRoot["Type"] = Json::Value("Server");
	JsonRoot["IP"] = Json::Value(strIP.c_str());
	JsonRoot["Port"] = Json::Value(StringEx::string_format("%d", usPort).c_str());
	JsonRoot["Listen"] = Json::Value(server.IsListened());

	bool bUpdate = false;
	if (m_pStrUpdateServer == nullptr)
	{
		g_mutex.lock();
		m_pStrUpdateServer = new Json::Value(JsonRoot);
		g_mutex.unlock();

		bUpdate = true;
	}
	else
	{
		if (JsonRoot != *m_pStrUpdateServer)
		{
			g_mutex.lock();
			m_pStrUpdateServer->clear();
			*m_pStrUpdateServer = JsonRoot;
			g_mutex.unlock();

			bUpdate = true;
		}
	}

	if (bUpdate == false)
	{
		strIP.clear();
		JsonRoot.clear();

		return;
	}

	if (m_pFunc && m_pFunc(m_pStrUpdateServer, m_pPtr) == false)
	{
		g_mutex.lock();
		m_pStrUpdateServer->clear();
		g_mutex.unlock();
	}

	strIP.clear();
	JsonRoot.clear();

	return;
}

#endif //_BASEFUNCTION_H