#include "../ProjectApp2.0/pch.h"
#include "ServiceFunction.h"

#ifdef _SERVICEFUNCTION_H

//#include "../ZIGBEECallDemo/CZigbeeControler.cpp"
//#include "../MESDemo/CMES.cpp"
//#include "../ADO/ADOConn.cpp"
#include "../String/SystemLog.h"
#include "../String/StringEx.h"

#include "../json/json.h"
//#include "../json/json_value.cpp"
//#include "../json/json_reader.cpp"
//#include "../json/json_writer.cpp"


ServiceFunction::ServiceFunction()
{
	m_pMes = nullptr;
	m_pZGBCtrler = nullptr;
	m_bScream = false;
	m_bKey = true;

	m_strMES = "";
	m_strADO = "";
	m_strZigbee = "";
	m_strStation = "";

	for (unsigned char i = 0; i < UCHAR_MAX; ++i)
	{
		m_bUpdateAGV[i] = false;
	}
}

ServiceFunction::~ServiceFunction()
{
	Release();

	ReleaseString();
}

bool ServiceFunction::AcceptClient(SOCKET socket)
{
	return BaseFunction::AcceptClient(socket);
}

void ServiceFunction::Function()
{
	BaseFunction::Function();

	RecordAGVStatus();

	ProcessZigbeePackage();

	RecordCallerStatus();

	if(m_bKey == false)
	{
		return;
	}

	ProcessCallSignal();

	if (m_bScream)
	{
		return;
	}

	ProcessAGVTask();

	return;
}

void ServiceFunction::ProcessAGVStatus(void* pUser)
{
	if (pUser == nullptr)
	{
		return;
	}

	CAGV* pAGV = (CAGV*)pUser;

	if (pAGV->GetStatus() == _AGVSTA_ARM)
	{
		ZGBCaller* pCaller = GetCaller(pAGV->GetCurRFID());

		if (pCaller)
		{
			if (pCaller->GetPassSignal() != SIGNAL_ON && m_pZGBCtrler)
			{
				// m_pZGBCtrler->SetSignal(pCaller->GetNo(), "Pass", SIGNAL_ON);
				m_pZGBCtrler->SetSignal(pCaller, PASS_SIGNAL, SIGNAL_ON);
			}
			else
			{
				pAGV->Reset();
			}
		}
	}
	else if (pAGV->GetStatus() != _AGVSTA_ALLSCREAM)
	{
		if(m_bScream || m_bKey == false)
		{
			pAGV->AllScream();
		}
	}
	else
	{
		if (m_bScream == false && m_bKey)
		{
			pAGV->Reset();
		}
	}

	return BaseFunction::ProcessAGVStatus(pUser);
}

bool ServiceFunction::InitMES()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strMES.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化MES失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return false;
	}
	m_mutexJson.unlock();

	string strProductLineNO = Json_ReadString(JsonRoot["ProductLineNO"]);
	if (m_pMes)
	{
		delete m_pMes;
		m_pMes = nullptr;
	}

	m_pMes = new CMES(strProductLineNO.c_str());

	strProductLineNO.clear();

	m_pMes->Install();

	JsonRoot.clear();

	return true;
}

void ServiceFunction::Pass(unsigned char byProcess, unsigned short usRFID)
{
	CStation* pStation = GetStation(usRFID);	/*!< 放行工位 */

	// 放行工位无效
	if (pStation == nullptr)
	{
		DebugPrint::Print("放行失败:\n无效的工位");
		return;
	}

	unsigned char byStation = pStation->GetNo();	/*!< 放行工位编号 */

	ZGBCaller* pCaller = GetCaller(usRFID);	/*!< 呼叫器 */

	// 无效的呼叫器
	if (pCaller == nullptr)
	{
		return;
	}

	// 取消放行信号
	if (pCaller->GetCancelPassSignal() == SIGNAL_ON)
	{
		return;
	}

	// 无放行信号
	if (pCaller->GetPassSignal() != SIGNAL_ON)
	{
		return;
	}

	// 未通过出站检测
	if (pCaller->IsPackOutBounded() == false)
	{
		bool bPackOutbound = false;

		if (pStation->EnbaleMES() == false)
		{
			bPackOutbound = true;
		}
		else
		{
			bPackOutbound = PackOutBound(pStation->GetMESWP().c_str());
		}

		if (pCaller->UpdatePackOutBound(bPackOutbound))
		{
			UpdateZGBCaller(pCaller);
		}
	}

	CAGV* pAGV = (CAGV*)m_aRFIDArr[usRFID].GetLocker();	/*!< 在RFID卡的AGV */
	CAGV* pAGVComing = IsAGVComing(pStation);

	// AGV在放行工位
	if (pAGV)
	{
		if (pAGVComing && pAGVComing != pAGV)
		{
			CancelTask(pAGVComing->GetNo(), _TASK_TYPE_PASS, "用户手动调度AGV至工位,取消放行任务");
		}

		return;
	}

	// AGV正在前往放行工位
	if (pAGVComing)
	{
		return;
	}

	// 是否存在从此工位走出的AGV
	pAGV = IsAGVRunning(pStation);

	if (pAGV)
	{
		return;
	}

	// 获取空闲AGV
	pAGV = GetFreeAGV(pStation);

	if (pAGV == nullptr)
	{
		return;
	}

	CStation* pStart = GetStation(pAGV->GetCurRFID());

	unsigned char byStart = 0;

	if (pStart)
	{
		byStart = pStart->GetNo();
	}

	// 创建放行任务
	CreateTask(byStart, byStation, pAGV->GetNo(), _TASK_TYPE_PASS);

	return;
}

void ServiceFunction::Call(unsigned char byProcess, unsigned short usRFID)
{
	if (byProcess == 0)
	{
		return;
	}

	ZGBCaller* pCallCaller = GetCaller(usRFID);	/*!< 呼叫呼叫器 */

	// 呼叫器呼叫器无效 
	if (pCallCaller == nullptr)
	{
		DebugPrint::Print("呼叫失败:\n无效的呼叫器");
		return;
	}

	// 无呼叫信号
	if (pCallCaller->GetCallSignal() != SIGNAL_ON)
	{
		return;
	}

	// 取消呼叫
	if (pCallCaller->GetCancelCallSignal() == SIGNAL_ON)
	{
		return;
	}

	CStation* pStation = GetStation(usRFID);	/*!< 呼叫工位 */

	// 呼叫工位无效
	if (pStation == nullptr)
	{
		DebugPrint::Print("呼叫失败:\n无效的工位");
		return;
	}

	unsigned char byStation = pStation->GetNo();	/*!< 呼叫工位编号 */

	CAGV* pAGV = (CAGV*)m_aRFIDArr[usRFID].GetLocker();	/*!< 在RFID卡的AGV */

	// AGV在呼叫工位
	if (pAGV)
	{
		return;
	}

	// 是否存在向此工位来的AGV
	pAGV = IsAGVComing(pStation);

	if (pAGV)
	{
		return;
	}

	CAGV* pAGVPre = (CAGV*)m_aRFIDArr[usRFID].GetPreformLocker();	/*!< 预计到达RFID卡的AGV */

	/*
	// AGV正在前往呼叫工位
	if (pAGV != nullptr)
	{
		return;
	}
	*/
	
	unsigned char byCallStation = pStation->GetNo();	/*!< 呼叫工位 */

	unsigned char byFrontProcess = byProcess;	/*!< 上一道工序 */
	list<ZGBCaller*> listCaller;	/*!< 上一道工序呼叫器列表 */

	while(true)
	{
		byFrontProcess -= 1;

		// 反向循环
		if (byFrontProcess == 0)
		{
			byFrontProcess = _MES_BoxUnload;
		}

		// 循环结束
		if (byProcess == byFrontProcess)
		{
			DebugPrint::Printf("呼叫失败:\n未找到上一工序的有效呼叫器,工序:%d", byProcess);
			break;
		}

		if (byProcess == _MES_BatteryBoxTest2 && (byFrontProcess == _MES_BoxCoverInstallBuffer || byFrontProcess == _MES_BatteryBoxTest1))
		{
			continue;
		}

		listCaller = GetCaller(byFrontProcess);

		if (listCaller.size() == 0)
		{
			if (byProcess == _MES_BoxCoverInstall && byFrontProcess == _MES_BatteryBoxTest1)
			{
				break;
			}

			continue;
		}

		for (list<ZGBCaller*>::iterator it = listCaller.begin(); it != listCaller.end(); ++it)
		{
			ZGBCaller* pCaller = *it;	/*!< 上一工序呼叫器 */

			unsigned char byPassStation = pCaller->GetStation();	/*!< 放行工位 */
			// 无效工位
			if (m_mapStations.find(byPassStation) == m_mapStations.end())
			{
				continue;
			}

			unsigned short usLocation = m_mapStations[byPassStation]->GetLocation();	/*!< 放行RFID */

			pAGV = (CAGV*)m_aRFIDArr[usLocation].GetLocker();	/*!< 放行工位AGV */

			// 无AGV
			if (pAGV == nullptr)
			{
				continue;
			}

			// 预计到达此工位的AGV与放行工位AGV相同
			if (pAGVPre != nullptr && pAGVPre != pAGV)
			{
				continue;
			}

			// 取消放行
			if (pCaller->GetCancelPassSignal() == SIGNAL_ON)
			{
				continue;
			}

			// 未放行
			if (pCaller->GetPassSignal() != SIGNAL_ON)
			{
				continue;
			}

			// 出站校验未通过
			if (pCaller->IsPackOutBounded() == false)
			{
				continue;
			}
		
			CreateTask(byPassStation, byCallStation, pAGV->GetNo(), _TASK_TYPE_CALL);

			return;
		}

		if (byProcess == _MES_BoxCoverInstall && byFrontProcess != _MES_BatteryBoxTest1)
		{
			continue;
		}

		break;
	}
	
	return;
}

void ServiceFunction::NG(unsigned char byProcess, unsigned short usRFID)
{
	ZGBCaller* pCaller = GetCaller(usRFID);	/*!< 呼叫器 */
	
	// 无效的呼叫器
	if (pCaller == nullptr)
	{
		DebugPrint::Print("NG失败:\n无效的呼叫器");
		return;
	}

	// 无NG信号
	if (pCaller->GetNGSignal() != SIGNAL_ON)
	{
		return;
	}

	// 取消NG信号
	if (pCaller->GetCancelNGSignal() == SIGNAL_ON)
	{
		if (m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pCaller);
		}

		return;
	}

	if (m_pZGBCtrler)
	{
		m_pZGBCtrler->Reset(pCaller);
	}

	return;

	CStation* pStation = GetStation(usRFID);	/*!< NG工位 */

	// 无效的工位
	if (pStation == nullptr)
	{
		DebugPrint::Print("NG失败:\n无效的工位");
		return;
	}

	unsigned char byStation = pStation->GetNo();	/*!< NG工位编号 */

	CAGV* pAGV = IsAGVComing(pStation);

	// AGV正在前往放行工位
	if (pAGV)
	{
		return;
	}

	pAGV = (CAGV*)m_aRFIDArr[usRFID].GetLocker();

	// 没有AGV在NG工位 或 NG工位AGV已经离开
	if (pAGV == nullptr)
	{
		pAGV = IsAGVRunning(pStation);

		if (pAGV)
		{
			return;
		}

		pAGV = GetFreeAGV(pStation);

		if (pAGV == nullptr)
		{
			return;
		}

		CreateTask(0, byStation, pAGV->GetNo(), _TASK_TYPE_PASS);

		return;
	}

	unsigned char byAGVNo = pAGV->GetNo();

	// 执行AGV
	// NG工位
	list<CStation*> listNGStations = GetStation((unsigned char)_MES_NG);

	for (list<CStation*>::iterator it = listNGStations.begin(); it != listNGStations.end(); ++it)
	{
		CStation* pNGStation = *it;								/*!< NG工位指针 */
		unsigned char byNGStation = pNGStation->GetNo();		/*!< NG工位编号 */

		CreateTask(byStation, byNGStation, pAGV->GetNo(), _TASK_TYPE_NG);

		break;
	}

	return;
}

void ServiceFunction::Scream()
{
	m_bScream = true;

	return;
}

void ServiceFunction::CancelPass(unsigned char byProcess, unsigned short usRFID)
{
	// 放行工位指针
	CStation* pStation = GetStation(usRFID);

	if (pStation == nullptr)
	{
		return;
	}

	// 放行工位编号
	unsigned char byStation = pStation->GetNo();

	// 放行呼叫器指针
	ZGBCaller* pCaller = GetCaller(usRFID);

	// 放行呼叫器无效
	if (pCaller == nullptr)
	{
		return;
	}

	// 无取消放行信号
	if (pCaller->GetCancelPassSignal() != SIGNAL_ON)
	{
		return;
	}

	// 无放行信号
	if (pCaller->GetPassSignal() != SIGNAL_ON)
	{
		if (m_pZGBCtrler)
		{
			// 关闭取消放行信号
			m_pZGBCtrler->Reset(pCaller);
		}

		return;
	}

	/*!
	 * 1、即将来到此地 放行任务
	 * 2、已经离开此地 呼叫任务
	*/
	_variant_t var;				/*!< 字段值 */

	string strSQL = "";

	strSQL.append(StringEx::string_format(
		"update AGV_TASK_TABLE set [task_status]='%s',[task_update_time]=getdate(),[task_finish_time]=getdate(),[task_text]='%s' "\
		"where ([task_start]=%d or [task_end]=%d) and [task_type] in('%s','%s')"
		, _TASK_STATUS_CANCEL, "接收取消放行信号,取消放行"
		, byStation, byStation,_TASK_TYPE_PASS,_TASK_TYPE_CALL
	).c_str());

	if (m_ado.ExecuteSQL(strSQL.c_str(), var) == false)
	{
		return;
	}

	m_pZGBCtrler->Reset(pCaller);

	return;
}

void ServiceFunction::CancelCall(unsigned char byProcess, unsigned short usRFID)
{
	// 呼叫工位
	CStation* pStation = GetStation(usRFID);

	// 呼叫工位无效
	if (pStation == nullptr)
	{
		return;
	}

	// 呼叫工位编号
	unsigned char byStation = pStation->GetNo();

	// 呼叫呼叫器
	ZGBCaller* pCaller = GetCaller(usRFID);

	// 呼叫器无效
	if (pCaller == nullptr)
	{
		return;
	}

	// 无取消呼叫信号
	if (pCaller->GetCancelCallSignal() != SIGNAL_ON)
	{
		return;
	}

	// 无呼叫信号
	if (pCaller->GetCallSignal() != SIGNAL_ON)
	{
		if (m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pCaller);
		}

		return;
	}

	string strSQL = "";
	_variant_t var;				/*!< 字段值 */

	strSQL.append(StringEx::string_format(
		"update AGV_TASK_TABLE set [task_status]='%s',[task_update_time]=getdate(),[task_finish_time]=getdate(),[task_text]='%s' "\
		"where [task_end]=%d and [task_type]='%s'"
		, _TASK_STATUS_CANCEL, "接收取消呼叫信号,取消呼叫"
		, byStation, _TASK_TYPE_CALL
	).c_str());
	
	if (m_ado.ExecuteSQL(strSQL.c_str(), var) == false)
	{
		return;
	}

	m_pZGBCtrler->Reset(pCaller);

	return;
}

void ServiceFunction::CancelNG(unsigned char byProcess, unsigned short usRFID)
{
	// NG工位
	CStation* pStation = GetStation(usRFID);

	// NG工位无效
	if (pStation == nullptr)
	{
		return;
	}

	// NG工位编号
	unsigned char byStation = pStation->GetNo();

	// NG呼叫器
	ZGBCaller* pCaller = GetCaller(usRFID);

	// 呼叫器无效
	if (pCaller == nullptr)
	{
		return;
	}

	// 无取消NG信号
	if (pCaller->GetCancelNGSignal() != SIGNAL_ON)
	{
		return;
	}

	// 无NG信号
	if (pCaller->GetNGSignal() != SIGNAL_ON)
	{
		if (m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pCaller);
		}

		return;
	}

	if (m_pZGBCtrler)
	{
		m_pZGBCtrler->Reset(pCaller);
	}

	return;

	/*!
	 * 1、取消正在前往此工位的AGV任务
	 * 2、取消NG任务,并将货物带回
	*/
	_variant_t var;				/*!< 字段值 */

	string strSQL = "";

	strSQL.append(StringEx::string_format(
		"update AGV_TASK_TABLE set [task_status]='%s',[task_update_time]=getdate(),[task_finish_time]=getdate(),[task_text]='%s' "\
		"where ([task_start]=%d or [task_end]=%d) and [task_type] in('%s','%s')"
		, _TASK_STATUS_CANCEL, "接收取消NG信号,取消NG"
		, byStation, byStation, _TASK_TYPE_PASS, _TASK_TYPE_NG
	).c_str());

	if (m_ado.ExecuteSQL(strSQL.c_str(), var) == false)
	{
		return;
	}

	m_pZGBCtrler->Reset(pCaller);

	return;
}

void ServiceFunction::CancelScream()
{
	m_bScream = false;

	return;
}

CAGV* ServiceFunction::GetFreeAGV( CStation* pStation)
{
	if (pStation == nullptr)
	{
		return nullptr;
	}

	// 工位RFID卡
	unsigned short usRFID = pStation->GetLocation();

	// RFID卡指针
	RFID* pRFID = &m_aRFIDArr[usRFID];

	if (pRFID->GetLocker())
	{
		return (CAGV*)pRFID->GetLocker();
	}

	/*!
	 * 1、判断是否存在正在前往此工位的AGV
	*/

	if (IsAGVComing(pStation) != nullptr)
	{
		return nullptr;
	}

	// 工序
	unsigned char byProcess = pStation->GetProcess();

	/*!
	 * 2、机械臂后的工序不适于此工序
	*/
	if (byProcess == _MES_UpperModuleLifting || byProcess == _MES_LowerModuleLifting)
	{
		return nullptr;
	}

	// 电池包测试区1
	if (byProcess == _MES_BatteryBoxTest1)
	{
		return ApplyForBatteryBoxTest1(pStation);
	}
	// 电池包测试区2
	else if (byProcess == _MES_BatteryBoxTest2)
	{
		return ApplyForBatteryBoxTest2(pStation);
	}
	// 箱体上盖安装缓存区
	else if (byProcess == _MES_BoxCoverInstallBuffer)
	{
		return ApplyForBoxCoverInstallBuffer(pStation);
	}
	// NG维修站
	else if (byProcess == _MES_NG)
	{
		return ApplyForNG(pStation);
	}
	// 其他工站
	else
	{
		return ApplyForOther(pStation);
	}

	return nullptr;
}

bool ServiceFunction::CreateTask(const unsigned char byStartWP, const unsigned char byEndWP, unsigned char byExeAGV, const char* strType)
{
	string strSQL = "";

	strSQL.append(StringEx::string_format(
		"if exists(select [task_id] from AGV_TASK_TABLE where [task_id]=%d) DBCC CHECKIDENT(AGV_TASK_TABLE,reseed,0);"
		, INT_MAX
	).c_str());

	// 已经从工站走出但未到位
	// 已存在相同终点

	// 如果不存在相同任务,创建任务
	strSQL.append(StringEx::string_format(
		"if not exists(select * from AGV_TASK_TABLE where [task_end]=%d) "\
		"insert into AGV_TASK_TABLE([task_start],[task_end],[task_executer],[task_type]) values(%d,%d,%d,'%s');"
		, byEndWP, byStartWP, byEndWP, byExeAGV, strType
	).c_str());

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		DebugPrint::Printf("创建%s任务成功:AGV%d从工位:%d至%d", strType, byExeAGV, byStartWP, byEndWP);
		SystemLog::Recordf("创建%s任务成功:AGV%d从工位:%d至%d", strType, byExeAGV, byStartWP, byEndWP);

		return true;
	}

	return false;
}

int ServiceFunction::ControlAGV(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;

	unsigned char byNo = Json_ReadInt(JsonRoot["No"]);
	string strCmd = Json_ReadString(JsonRoot["Cmd"]);

	if (m_mapAGVs.find(byNo) == m_mapAGVs.end())
	{
		return -1;
	}

	DebugPrint::Printf("用户手动操作:AGV%d", byNo);
	SystemLog::Recordf("用户手动操作:AGV%d",byNo);

	if (strcmp(strCmd.c_str(), "Move") == 0)
	{
		unsigned char byStation = Json_ReadInt(JsonRoot["Target"]);

		if (m_mapStations.find(byStation) == m_mapStations.end())
		{
			return ERROR_END;
		}

		return m_mapAGVs[byNo]->MoveTo(m_mapStations[byStation]->GetLocation());
	}
	else if (strcmp(strCmd.c_str(), "TrafficPass") == 0)
	{
		return m_mapAGVs[byNo]->TrafficPass(m_mapAGVs[byNo]->GetCurRFID());
	}
	else if (strcmp(strCmd.c_str(), "Scream") == 0)
	{
		return m_mapAGVs[byNo]->Scream();
	}
	else if (strcmp(strCmd.c_str(), "Reset") == 0)
	{
		return m_mapAGVs[byNo]->Reset();
	}
	else if (strcmp(strCmd.c_str(), "LiftUp") == 0)
	{
		return m_mapAGVs[byNo]->LiftUp(m_mapAGVs[byNo]->GetCurRFID());
	}
	else if (strcmp(strCmd.c_str(), "LiftDown") == 0)
	{
		return m_mapAGVs[byNo]->LiftDown(m_mapAGVs[byNo]->GetCurRFID());
	}
	else if (strcmp(strCmd.c_str(), "ClearStatus") == 0)
	{
		unsigned short usCurRFID = 0, usEndRFID = 0;

		usCurRFID = m_mapAGVs[byNo]->GetCurRFID();
		usEndRFID = m_mapAGVs[byNo]->GetEndRFID();

		int nResult = m_mapAGVs[byNo]->InitAttribute();

		m_aRFIDArr[usCurRFID].Unlock(m_mapAGVs[byNo]);

		m_aRFIDArr[usEndRFID].PreformUnlock(m_mapAGVs[byNo]);

		if (nResult == ERROR_NONE)
		{
			UpdateAGV(m_mapAGVs[byNo]);
		}

		return nResult;
	}

	return -2;
}

bool ServiceFunction::DeleteTask(const int nNo)
{
	string strSQL = StringEx::string_format(
		"if not exists(select * from AGV_HISTASK_TABLE where [task_finish_time]=(select [task_finish_time] from AGV_TASK_TABLE where [task_id]=%d)) "\
		"insert into AGV_HISTASK_TABLE select * from AGV_TASK_TABLE where [task_id]=%d else delete AGV_TASK_TABLE where [task_id]=%d;"
		, nNo, nNo, nNo);

	// delete AGV_HISTASK_TABLE where [task_finish_time] < DATEADD(qq,-1,getdate())

	strSQL.append("delete AGV_HISTASK_TABLE where [task_finish_time] < DATEADD(qq,-1,getdate());");

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		if (m_mapStrUpdateTasks.find(nNo) != m_mapStrUpdateTasks.end())
		{
			delete m_mapStrUpdateTasks[nNo];
			m_mapStrUpdateTasks.erase(m_mapStrUpdateTasks.find(nNo));
		}

		return true;
	}

	return false;
}

bool ServiceFunction::FinishTask(const int nNo, const char* _Format, ...)
{
	std::string tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vsprintf_s((char*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	string strSQL = StringEx::string_format(
		"update AGV_TASK_TABLE set [task_status]='%s',[task_update_time]=getdate(),[task_finish_time]=getdate(),[task_text]='%s' where [task_id]=%d"
		, _TASK_STATUS_FINISH, tmp.c_str(), nNo);

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		DebugPrint::Printf("更新任务成功!更新任务%d状态至%s,并备注：%s", nNo, _TASK_STATUS_FINISH,tmp.c_str());

		return true;
	}

	return false;
}

bool ServiceFunction::FinishTask(const int nNo, const wchar_t* _Format, ...)
{
	std::wstring tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscwprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vswprintf_s((wchar_t*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	return 	FinishTask(nNo, StringEx::wstring_to_string(tmp).c_str());
}

bool ServiceFunction::CancelTask(const int nNo, const char* _Format, ...)
{
	std::string tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vsprintf_s((char*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	string strSQL = StringEx::string_format(
		"update AGV_TASK_TABLE set [task_status]='%s',[task_update_time]=getdate(),[task_finish_time]=getdate(),[task_text]='%s' where [task_id]=%d"
		, _TASK_STATUS_FINISH, tmp.c_str(), nNo);

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		DebugPrint::Printf("取消任务成功!取消任务%d,并备注：%s", nNo, tmp.c_str());

		return true;
	}

	return false;
}

bool ServiceFunction::CancelTask(const int nNo, const wchar_t* _Format, ...)
{
	std::wstring tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscwprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vswprintf_s((wchar_t*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	return 	CancelTask(nNo, StringEx::wstring_to_string(tmp).c_str());
}

bool ServiceFunction::CancelTask(const unsigned char byAGVNo, const char* strType, const char* _Format, ...)
{
	std::string tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vsprintf_s((char*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	string strSQL = StringEx::string_format(
		"update AGV_TASK_TABLE set [task_status]='%s',[task_update_time]=getdate(),[task_finish_time]=getdate(),[task_text]='%s' where [task_executer]=%d and [task_type]='%s'"
		, _TASK_STATUS_FINISH, tmp.c_str(), byAGVNo, strType);

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		DebugPrint::Printf("取消任务成功!取消AGV%d%s任务,并备注：%s", byAGVNo, strType,tmp.c_str());

		return true;
	}

	return false;
}

bool ServiceFunction::CancelTask(const unsigned char byAGVNo, const wchar_t* strType, const wchar_t* _Format, ...)
{
	std::wstring tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscwprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vswprintf_s((wchar_t*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	return CancelTask(byAGVNo, StringEx::wstring_to_string(strType).c_str(),StringEx::wstring_to_string(tmp).c_str());
}

bool ServiceFunction::UpdateTaskText(const int nNo, const char* _Format, ...)
{
	std::string tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vsprintf_s((char*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	string strSQL = "";

	strSQL.append(StringEx::string_format(
		"if not exists(select * from AGV_TASK_TABLE where [task_id]=%d and [task_text]='%s') "\
		"update AGV_TASK_TABLE set [task_text]='%s',[task_update_time]=getdate() where [task_id]=%d"
		, nNo, tmp.c_str(), tmp.c_str(), nNo));

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		return true;
	}

	return false;
}

bool ServiceFunction::UpdateTaskText(const int nNo, const wchar_t* _Format, ...)
{
	std::wstring tmp;

	va_list marker = NULL;
	va_start(marker, _Format);

	size_t num_of_chars = _vscwprintf(_Format, marker);

	if (num_of_chars > tmp.capacity())
	{
		tmp.resize(num_of_chars + 1);
	}

	vswprintf_s((wchar_t*)tmp.data(), tmp.capacity(), _Format, marker);

	va_end(marker);

	return UpdateTaskText(nNo, StringEx::wstring_to_string(tmp).c_str());
}

bool ServiceFunction::ExecuteTask(const int nTaskNo, const unsigned char byAGVNo, const char* strStatus)
{
	string strSQL = StringEx::string_format(
		"if not exists(select * from AGV_TASK_TABLE where [task_id]<>%d and [task_executer]=%d and [task_status]<>'%s') "\
		"update AGV_TASK_TABLE set [task_status]='%s',[task_update_time]=getdate() where [task_id]=%d"
		,nTaskNo, byAGVNo, _TASK_STATUS_NONE, strStatus, nTaskNo);

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		DebugPrint::Printf("更新任务成功!更新任务%d状态至%s", nTaskNo, strStatus);

		return true;
	}

	return false;
}

list<ZGBCaller*> ServiceFunction::GetCaller(const unsigned char byProcess)
{
	list<ZGBCaller*> listCallers;
	list<CStation*> listStations = GetStation(byProcess);

	for (list<CStation*>::iterator it = listStations.begin(); it != listStations.end(); ++it)
	{
		CStation* pStation = *it;

		ZGBCaller* pCaller = GetCallerS(pStation->GetNo());

		if (pCaller != nullptr && pCaller->EnableUse())
		{
			listCallers.push_back(pCaller);
		}
	}
	
	return listCallers;
}

ZGBCaller* ServiceFunction::GetCaller(const unsigned short usRFID)
{
	CStation* pStation = GetStation(usRFID);

	if (pStation == nullptr)
	{
		return nullptr;
	}

	ZGBCaller* pCaller = GetCallerS(pStation->GetNo());

	if (pCaller && pCaller->EnableUse())
	{
		return pCaller;
	}

	return nullptr; 
}

ZGBCaller* ServiceFunction::GetCallerS(const unsigned char byStation)
{
	for (map<unsigned char, ZGBCaller*>::iterator itCall = m_mapCallers.begin(); itCall != m_mapCallers.end(); ++itCall)
	{
		ZGBCaller* pCaller = itCall->second;

		if (pCaller->GetStation() == byStation)
		{
			if (pCaller->EnableUse())
			{
				return pCaller;
			}

			return nullptr;
		}
	}

	return nullptr;
}

list<CStation*> ServiceFunction::GetStation(const unsigned char byProcess)
{
	list<CStation*> listStations;

	for (map<unsigned char, CStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it)
	{
		CStation* pStation = it->second;

		if (pStation->GetProcess() == byProcess)
		{
			listStations.push_back(pStation);
		}
	}

	return listStations;
}

CStation* ServiceFunction::GetStation(const unsigned short usRFID)
{
	for (map<unsigned char, CStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it)
	{
		CStation* pStation = it->second;

		if (pStation->GetLocation() == usRFID)
		{
			return pStation;
		}
	}

	return nullptr;
}

void ServiceFunction::ProcessAGVTask()
{
	string strSql = StringEx::string_format("select * from AGV_TASK_TABLE order by [task_publish_time],[task_id]");

	_RecordsetPtr pRecordset;
	if (m_ado.GetRecordSet(strSql.c_str(), pRecordset) == false)
	{
		return;
	}

	if (pRecordset->adoEOF)
	{
		//m_ado.Release();
		return;
	}

	Json::Value JsonRoot;

	int nTaskNo = 0;			/*!< 任务号 */
	unsigned char byStart = 0;	/*!< 起始工位编号 */
	unsigned char byEnd = 0;	/*!< 终止工位编号 */
	string strStatus = "";		/*!< 任务状态 */
	unsigned char byAGVNo = 0;	/*!< 执行AGV编号 */
	string strType = "";		/*!< 任务类型 */
	string strPublish = "";		/*!< 任务发布时间 */
	string strUpdate = "";		/*!< 任务更新时间 */
	string strFinish = "";		/*!< 任务完成时间 */
	string strText = "";		/*!< 任务备注 */

	bool bUpdate = false;

	_variant_t var;				/*!< 字段值 */

	for(;!pRecordset->adoEOF; 
		pRecordset->MoveNext()
		, nTaskNo = 0
		, byStart = 0
		, byEnd = 0
		, strStatus.clear()
		, byAGVNo = 0
		, strType.clear()
		, strPublish.clear()
		, strUpdate.clear()
		, strFinish.clear()
		, strText.clear()
		, JsonRoot.clear()
		, bUpdate = false)
	{
		var = pRecordset->GetCollect(_bstr_t("task_id"));
		if (var.vt != VT_NULL)
		{
			nTaskNo = (int)var;
		}

		var = pRecordset->GetCollect(_bstr_t("task_start"));
		if (var.vt != VT_NULL)
		{
			byStart = (unsigned char)var;
		}

		var = pRecordset->GetCollect(_bstr_t("task_end"));
		if (var.vt != VT_NULL)
		{
			byEnd = (unsigned char)var;
		}

		var = pRecordset->GetCollect(_bstr_t("task_status"));
		if (var.vt != VT_NULL)
		{
			strStatus = (char*)_bstr_t(var);
		}

		var = pRecordset->GetCollect(_bstr_t("task_executer"));
		if (var.vt != VT_NULL)
		{
			byAGVNo = (unsigned char)var;
		}

		var = pRecordset->GetCollect(_bstr_t("task_type"));
		if (var.vt != VT_NULL)
		{
			strType = (char*)_bstr_t(var);
		}

		var = pRecordset->GetCollect(_bstr_t("task_publish_time"));
		if (var.vt != VT_NULL)
		{
			strPublish = (char*)_bstr_t(var);
		}

		var = pRecordset->GetCollect(_bstr_t("task_update_time"));
		if (var.vt != VT_NULL)
		{
			strUpdate = (char*)_bstr_t(var);
		}

		var = pRecordset->GetCollect(_bstr_t("task_finish_time"));
		if (var.vt != VT_NULL)
		{
			strFinish = (char*)_bstr_t(var);
		}

		var = pRecordset->GetCollect(_bstr_t("task_text"));
		if (var.vt != VT_NULL)
		{
			strText = (char*)_bstr_t(var);
		}

		// 更新任务状态
		if (m_pFunc)
		{	
			JsonRoot["No"] = Json::Value(nTaskNo);
			JsonRoot["Start"] = Json::Value(byStart);
			JsonRoot["End"] = Json::Value(byEnd);
			JsonRoot["Executer"] = Json::Value(byAGVNo);
			JsonRoot["Mission"] = Json::Value(StringEx::AcsiiToUtf8(strType.c_str()));
			JsonRoot["Status"] = Json::Value(StringEx::AcsiiToUtf8(strStatus.c_str()));
			JsonRoot["Text"] = Json::Value(StringEx::AcsiiToUtf8(strText.c_str()));
			JsonRoot["PublishTime"] = Json::Value(strPublish.c_str());
			JsonRoot["UpdateTime"] = Json::Value(strUpdate.c_str());
			JsonRoot["FinishTime"] = Json::Value(strFinish.c_str());
			JsonRoot["Type"] = Json::Value("Task");

			if (m_mapStrUpdateTasks.find(nTaskNo) == m_mapStrUpdateTasks.end())
			{
				g_mutex.lock();
				m_mapStrUpdateTasks[nTaskNo] = new Json::Value(JsonRoot);
				g_mutex.unlock();

				bUpdate = true;
			}
			else
			{
				if (*m_mapStrUpdateTasks[nTaskNo] != JsonRoot)
				{
					g_mutex.lock();
					m_mapStrUpdateTasks[nTaskNo]->clear();
					*m_mapStrUpdateTasks[nTaskNo] = JsonRoot;
					g_mutex.unlock();

					bUpdate = true;
				}
			}

			if (bUpdate && m_pFunc && m_pFunc(m_mapStrUpdateTasks[nTaskNo], m_pPtr) == false)
			{
				g_mutex.lock();
				m_mapStrUpdateTasks[nTaskNo]->clear();
				g_mutex.unlock();
			}
		}

		// 处理任务
		if (strcmp(strType.c_str(), _TASK_TYPE_CALL) == 0)
		{
			ProcessAGVCallTask(nTaskNo, byStart, byEnd, byAGVNo, strStatus.c_str());
		}
		else if (strcmp(strType.c_str(), _TASK_TYPE_NG) == 0)
		{
			ProcessAGVNGTask(nTaskNo, byStart, byEnd, byAGVNo, strStatus.c_str());
		}
		else if (strcmp(strType.c_str(), _TASK_TYPE_PASS) == 0)
		{
			ProcessAGVPassTask(nTaskNo, byStart, byEnd, byAGVNo, strStatus.c_str());
		}
		else if (strcmp(strType.c_str(), _TASK_TYPE_RETURN) == 0)
		{
			ProcessAGVReturnTask(nTaskNo, byStart, byEnd, byAGVNo, strStatus.c_str());
		}
		else if (strcmp(strType.c_str(), _TASK_TYPE_HANDFULL) == 0)
		{
			ProcessAGVHandFullTask(nTaskNo, byStart, byEnd, byAGVNo, strStatus.c_str());
		}
		else if (strcmp(strType.c_str(), _TASK_TYPE_HANDEMPTY) == 0)
		{
			ProcessAGVHandEmptyTask(nTaskNo, byStart, byEnd, byAGVNo, strStatus.c_str());
		}
	}

	//m_ado.Release();
	return;
}

void ServiceFunction::ProcessAGVCallTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus)
{
	ZGBCaller* pPassCalller = nullptr;	/*!< 呼叫呼叫器 */
	ZGBCaller* pCallCalller = nullptr;	/*!< 放行呼叫器 */

	// 起点或终点工位无效
	if (m_mapStations.find(byStart) == m_mapStations.end() || m_mapStations.find(byEnd) == m_mapStations.end())
	{
		return;
	}

	unsigned short usStartRFID = m_mapStations[byStart]->GetLocation();		/*!< 起始RFID卡 */
	unsigned short usEndRFID = m_mapStations[byEnd]->GetLocation();			/*!< 终止RFID卡 */

	pPassCalller = GetCallerS(byStart);
	pCallCalller = GetCallerS(byEnd);

	// 放行或呼叫呼叫器无效
	if (pPassCalller == nullptr || pCallCalller == nullptr)
	{
		return;
	}

	// 无效AGV
	if (m_mapAGVs.find(byAGVNo) == m_mapAGVs.end())
	{
		return;
	}

	unsigned short usAGVCurRFID = m_mapAGVs[byAGVNo]->GetCurRFID();			/*!< AGV当前RFID卡 */
	unsigned short usAGVEndRFID = m_mapAGVs[byAGVNo]->GetEndRFID();			/*!< AGV终止RFID卡 */
	unsigned char byAGVCurLifter = m_mapAGVs[byAGVNo]->GetLifter();			/*!< AGV当前举升状态 */
	unsigned char byAGVCurCargo = m_mapAGVs[byAGVNo]->GetCargo();			/*!< AGV当前载货状态 */
	unsigned short usAGVAction = m_mapAGVs[byAGVNo]->GetAction();			/*!< AGV当前动作 */
	unsigned char byAGVActStatus = m_mapAGVs[byAGVNo]->GetActionStatus();	/*!< AGV当前动作终状态 */

	// 任务未执行
	if (strcmp(strStatus, _TASK_STATUS_NONE) == 0)
	{
		// 判断任务执行条件
		// 未呼叫
		if (pCallCalller->GetCallSignal() == SIGNAL_OFF)
		{
			return;
		}

		// 未放行
		if (pPassCalller->GetPassSignal() == SIGNAL_OFF)
		{
			return;
		}

		// 未通过出站校验
		if (pPassCalller->IsPackOutBounded() == false)
		{
			return;
		}

		// AGV未处于起始RFID 
		if (usAGVCurRFID != usStartRFID)
		{
			return;
		}

		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_PRE);
	}
	else if (strcmp(strStatus, _TASK_STATUS_PRE) == 0)
	{
		// 未升起
		if (byAGVCurLifter != _AGVLIFTER_UP || usAGVAction != _AGVACT_LIFTUP)
		{
			if (m_mapAGVs[byAGVNo]->GetError() == _AGVERR_LIFTUP)
			{
				UpdateTaskText(nTaskNo, "异常:顶升失败,未能定位料车位置");

				return;
			}

			m_mapAGVs[byAGVNo]->LiftUp(usEndRFID);

			return;
		}

		UpdateTaskText(nTaskNo, "正常");

		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_EXE);
	}
	// 已执行
	else if (strcmp(strStatus, _TASK_STATUS_EXE) == 0)
	{	
		if (pPassCalller->GetPassSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pPassCalller);
		}

		/*
		// 无货料
		if (byAGVCurCargo == _AGVCARGO_EMPTY)
		{
			UpdateTaskText(nTaskNo, "异常:物料缺失!");

			return;
		}
		*/
		
		UpdateTaskText(nTaskNo, "正常");

		// 未抵达终点RFID卡
		if (usAGVCurRFID != usEndRFID)
		{
			// 移动终点不一致
			if (usAGVEndRFID != usEndRFID || m_mapAGVs[byAGVNo]->GetStatus() == _AGVSTA_WAIT)
			{
				// 移动至终点
				m_mapAGVs[byAGVNo]->MoveTo(usEndRFID);
			}
		}
		// 抵达终点RFID卡
		else
		{
			if (pPassCalller->GetPassSignal() == SIGNAL_ON && m_pZGBCtrler)
			{
				m_pZGBCtrler->Reset(pPassCalller);
			}

			if (pCallCalller->GetCallSignal() == SIGNAL_ON && m_pZGBCtrler)
			{
				m_pZGBCtrler->Reset(pCallCalller);
			}

			// 未下降
			if (byAGVCurLifter != _AGVLIFTER_DOWN || usAGVAction != _AGVACT_LIFTDOWN)
			{
				// 下降
				m_mapAGVs[byAGVNo]->LiftDown(usEndRFID);
				
				return;
			}

			// 进入下一阶段
			FinishTask(nTaskNo, "系统正常结束任务");
		}
	}
	// 已完成
	else if (strcmp(strStatus, _TASK_STATUS_FINISH) == 0)
	{
		// 关闭放行信号
		if (pPassCalller && pPassCalller->GetPassSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pPassCalller);
			return;
		}

		// 关闭呼叫信息
		if (pCallCalller && pCallCalller->GetCallSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pCallCalller);
			return;
		}

		// 删除任务
		DeleteTask(nTaskNo);
	}
	// 已取消
	else if (strcmp(strStatus, _TASK_STATUS_CANCEL) == 0)
	{
		// 取消放行
		if (pPassCalller && pPassCalller->GetCancelPassSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pPassCalller);
		
			return;
		}

		// 取消呼叫
		if (pCallCalller && pCallCalller->GetCancelPassSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pCallCalller);

			return;
		}

		//CreateTask(0, byStart, byAGVNo, _TASK_TYPE_RETURN);

		// 删除任务
		DeleteTask(nTaskNo);
	}

	return;
}

void ServiceFunction::ProcessAGVNGTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus)
{
	ZGBCaller* pNGCalller = nullptr;	/*!< NG呼叫器 */

	// 起点或终点工位无效
	if (m_mapStations.find(byStart) == m_mapStations.end() || m_mapStations.find(byEnd) == m_mapStations.end())
	{
		return;
	}

	unsigned short usStartRFID = m_mapStations[byStart]->GetLocation();		/*!< 起始RFID卡 */
	unsigned short usEndRFID = m_mapStations[byEnd]->GetLocation();			/*!< 终止RFID卡 */

	pNGCalller = GetCallerS(byStart);

	// 无效AGV
	if (m_mapAGVs.find(byAGVNo) == m_mapAGVs.end())
	{
		return;
	}

	unsigned short usAGVCurRFID = m_mapAGVs[byAGVNo]->GetCurRFID();			/*!< AGV当前RFID卡 */
	unsigned short usAGVEndRFID = m_mapAGVs[byAGVNo]->GetEndRFID();			/*!< AGV终止RFID卡 */
	unsigned char byAGVCurLifter = m_mapAGVs[byAGVNo]->GetLifter();			/*!< AGV当前举升状态 */
	unsigned char byAGVCurCargo = m_mapAGVs[byAGVNo]->GetCargo();			/*!< AGV当前载货状态 */
	unsigned short usAGVAction = m_mapAGVs[byAGVNo]->GetAction();			/*!< AGV当前动作 */
	unsigned char byAGVActStatus = m_mapAGVs[byAGVNo]->GetActionStatus();	/*!< AGV当前动作终状态 */

	// NG呼叫器无效
	if (pNGCalller == nullptr)
	{
		return;
	}

	// 任务未执行
	if (strcmp(strStatus, _TASK_STATUS_NONE) == 0)
	{
		// 判断任务执行条件
		// 未NG
		if (pNGCalller->GetNGSignal() == SIGNAL_OFF)
		{
			return;
		}

		// AGV未处于起始RFID
		if (usAGVCurRFID != usStartRFID)
		{
			return;
		}

		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_PRE);
	}
	else if (strcmp(strStatus, _TASK_STATUS_PRE) == 0)
	{
		// 未升起
		if (byAGVCurLifter != _AGVLIFTER_UP || usAGVAction != _AGVACT_LIFTUP)
		{
			if (m_mapAGVs[byAGVNo]->GetError() == _AGVERR_LIFTUP)
			{
				UpdateTaskText(nTaskNo, "异常:顶升失败,未能定位料车位置");

				return;
			}

			m_mapAGVs[byAGVNo]->LiftUp(usEndRFID);

			return;
		}

		UpdateTaskText(nTaskNo, "正常");

		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_EXE);
	}
	// 已执行
	else if (strcmp(strStatus, _TASK_STATUS_EXE) == 0)
	{
		if (pNGCalller->GetNGSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pNGCalller);
		}
		/*
		// 无货料
		if (byAGVCurCargo == _AGVCARGO_EMPTY)
		{
			UpdateTaskText(nTaskNo, "异常:物料缺失!");

			return;
		}
		*/

		UpdateTaskText(nTaskNo, "正常");

		// 未抵达终点RFID卡
		if (usAGVCurRFID != usEndRFID)
		{
			// 移动终点不一致
			if (usAGVEndRFID != usEndRFID || m_mapAGVs[byAGVNo]->GetStatus() == _AGVSTA_WAIT)
			{
				// 移动至终点
				m_mapAGVs[byAGVNo]->MoveTo(usEndRFID);
			}
		}
		// 抵达终点RFID卡
		else
		{
			if (pNGCalller->GetNGSignal() == SIGNAL_ON && m_pZGBCtrler)
			{
				m_pZGBCtrler->Reset(pNGCalller);
			}

			// 未下降
			if (byAGVCurLifter != _AGVLIFTER_DOWN || usAGVAction != _AGVACT_LIFTDOWN)
			{
				// 下降
				m_mapAGVs[byAGVNo]->LiftDown(usEndRFID);

				return;
			}

			// 进入下一阶段
			FinishTask(nTaskNo, "系统正常结束任务");
		}
	}
	// 已完成
	else if (strcmp(strStatus, _TASK_STATUS_FINISH) == 0)
	{
		// 关闭NG信号
		if (pNGCalller && pNGCalller->GetNGSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pNGCalller);
			return;
		}

		DeleteTask(nTaskNo);
	}
	// 已取消
	else if (strcmp(strStatus, _TASK_STATUS_CANCEL) == 0)
	{
		// 关闭NG信号
		if (pNGCalller && pNGCalller->GetNGSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pNGCalller);
			return;
		}

		//CreateTask(0, byStart, byAGVNo, _TASK_TYPE_RETURN);

		DeleteTask(nTaskNo);
	}

	return;
}

void ServiceFunction::ProcessAGVPassTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus)
{
	ZGBCaller* pPassCalller = nullptr;	/*!< 放行呼叫器 */

	// 终点工位无效
	if (m_mapStations.find(byEnd) == m_mapStations.end())
	{
		return;
	}

	unsigned short usEndRFID = m_mapStations[byEnd]->GetLocation();			/*!< 终止RFID卡 */

	pPassCalller = GetCallerS(byEnd);

	unsigned short usAGVCurRFID = m_mapAGVs[byAGVNo]->GetCurRFID();			/*!< AGV当前RFID卡 */
	unsigned short usAGVEndRFID = m_mapAGVs[byAGVNo]->GetEndRFID();			/*!< AGV终止RFID卡 */
	unsigned char byAGVCurLifter = m_mapAGVs[byAGVNo]->GetLifter();			/*!< AGV当前举升状态 */
	unsigned char byAGVCurCargo = m_mapAGVs[byAGVNo]->GetCargo();			/*!< AGV当前载货状态 */
	unsigned short usAGVAction = m_mapAGVs[byAGVNo]->GetAction();			/*!< AGV当前动作 */
	unsigned char byAGVActStatus = m_mapAGVs[byAGVNo]->GetActionStatus();	/*!< AGV当前动作终状态 */

	// 任务未执行
	if (strcmp(strStatus, _TASK_STATUS_NONE) == 0)
	{
		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_PRE);
	}
	// 准备执行
	else if (strcmp(strStatus, _TASK_STATUS_PRE) == 0)
	{
		// 未下降
		if (byAGVCurLifter != _AGVLIFTER_DOWN || usAGVAction != _AGVACT_LIFTDOWN)
		{
			// 下降
			m_mapAGVs[byAGVNo]->LiftDown(usEndRFID);

			return;
		}

		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_EXE);
	}
	// 已执行
	else if (strcmp(strStatus, _TASK_STATUS_EXE) == 0)
	{
		// 未抵达终点RFID卡
		if (usAGVCurRFID != usEndRFID)
		{
			// 移动终点不一致
			if (usAGVEndRFID != usEndRFID || m_mapAGVs[byAGVNo]->GetStatus() == _AGVSTA_WAIT)
			{
				// 移动至终点
				m_mapAGVs[byAGVNo]->MoveTo(usEndRFID);
			}
		}
		// 抵达终点RFID卡
		else
		{
			// 进入下一阶段
			FinishTask(nTaskNo, "系统正常结束任务");
		}
	}
	// 已完成
	else if (strcmp(strStatus, _TASK_STATUS_FINISH) == 0)
	{
		DeleteTask(nTaskNo);
	}
	// 已取消
	else if (strcmp(strStatus, _TASK_STATUS_CANCEL) == 0)
	{
		if (m_mapAGVs[byAGVNo]->GetAction() != _AGVACT_NONE || m_mapAGVs[byAGVNo]->GetActionStatus() != _AGVACTSTA_NONE)
		{
			m_mapAGVs[byAGVNo]->Stop();
		}

		if (pPassCalller && pPassCalller->GetCancelPassSignal() == SIGNAL_ON && m_pZGBCtrler)
		{
			m_pZGBCtrler->Reset(pPassCalller);

			return;
		}
		
		DeleteTask(nTaskNo);
	}

	return;
}

void ServiceFunction::ProcessAGVReturnTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus)
{
	// 终点工位无效
	if (m_mapStations.find(byEnd) == m_mapStations.end())
	{
		return;
	}

	unsigned short usEndRFID = m_mapStations[byEnd]->GetLocation();			/*!< 终止RFID卡 */

	// 无效AGV
	if (m_mapAGVs.find(byAGVNo) == m_mapAGVs.end())
	{
		return;
	}

	unsigned short usAGVCurRFID = m_mapAGVs[byAGVNo]->GetCurRFID();			/*!< AGV当前RFID卡 */
	unsigned short usAGVEndRFID = m_mapAGVs[byAGVNo]->GetEndRFID();			/*!< AGV终止RFID卡 */
	unsigned char byAGVCurLifter = m_mapAGVs[byAGVNo]->GetLifter();			/*!< AGV当前举升状态 */
	unsigned char byAGVCurCargo = m_mapAGVs[byAGVNo]->GetCargo();			/*!< AGV当前载货状态 */
	unsigned short usAGVAction = m_mapAGVs[byAGVNo]->GetAction();			/*!< AGV当前动作 */
	unsigned char byAGVActStatus = m_mapAGVs[byAGVNo]->GetActionStatus();	/*!< AGV当前动作终状态 */

	// 任务未执行
	if (strcmp(strStatus, _TASK_STATUS_NONE) == 0)
	{
		if (usAGVCurRFID == usEndRFID)
		{
			// 进入下一阶段
			ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_EXE);
		}
		else
		{
			// 进入下一阶段
			ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_PRE);
		}
	}
	// 准备执行
	else if (strcmp(strStatus, _TASK_STATUS_PRE) == 0)
	{
		// 未升起
		if (byAGVCurLifter != _AGVLIFTER_UP || usAGVAction != _AGVACT_LIFTUP)
		{
			if (m_mapAGVs[byAGVNo]->GetError() == _AGVERR_LIFTUP)
			{
				UpdateTaskText(nTaskNo, "异常:顶升失败,未能定位料车位置");

				return;
			}

			// 升起
			m_mapAGVs[byAGVNo]->LiftUp(usEndRFID);

			return;
		}

		UpdateTaskText(nTaskNo, "正常");

		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_EXE);
	}
	// 正在执行
	else if (strcmp(strStatus, _TASK_STATUS_EXE) == 0)
	{
		/*
		// 无货料
		if (byAGVCurCargo == _AGVCARGO_EMPTY)
		{
			UpdateTaskText(nTaskNo, "异常:物料缺失!");

			return;
		}
		*/

		UpdateTaskText(nTaskNo, "正常");

		// 未抵达终点RFID卡
		if (usAGVCurRFID != usEndRFID)
		{
			// 移动终点不一致
			if (usAGVEndRFID != usEndRFID || m_mapAGVs[byAGVNo]->GetStatus() == _AGVSTA_WAIT)
			{
				// 移动至终点
				m_mapAGVs[byAGVNo]->MoveTo(usEndRFID);
			}
		}
		// 抵达终点RFID卡
		else
		{
			// 未下降
			if (byAGVCurLifter != _AGVLIFTER_DOWN || usAGVAction != _AGVACT_LIFTDOWN)
			{
				// 下降
				m_mapAGVs[byAGVNo]->LiftDown(usEndRFID);

				return;
			}

			// 进入下一阶段
			FinishTask(nTaskNo, "系统正常结束任务");
		}
	}
	// 已完成
	else if (strcmp(strStatus, _TASK_STATUS_FINISH) == 0)
	{
		DeleteTask(nTaskNo);
	}
	// 已取消
	else if (strcmp(strStatus, _TASK_STATUS_CANCEL) == 0)
	{
		DeleteTask(nTaskNo);
	}

	return;
}

void ServiceFunction::ProcessAGVHandFullTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus)
{
	// 起点或终点工位无效
	if (m_mapStations.find(byStart) == m_mapStations.end() || m_mapStations.find(byEnd) == m_mapStations.end())
	{
		return;
	}

	unsigned short usStartRFID = m_mapStations[byStart]->GetLocation();		/*!< 起始RFID卡 */
	unsigned short usEndRFID = m_mapStations[byEnd]->GetLocation();			/*!< 终止RFID卡 */

	// 无效AGV
	if (m_mapAGVs.find(byAGVNo) == m_mapAGVs.end())
	{
		return;
	}

	unsigned short usAGVCurRFID = m_mapAGVs[byAGVNo]->GetCurRFID();			/*!< AGV当前RFID卡 */
	unsigned short usAGVEndRFID = m_mapAGVs[byAGVNo]->GetEndRFID();			/*!< AGV终止RFID卡 */
	unsigned char byAGVCurLifter = m_mapAGVs[byAGVNo]->GetLifter();			/*!< AGV当前举升状态 */
	unsigned char byAGVCurCargo = m_mapAGVs[byAGVNo]->GetCargo();			/*!< AGV当前载货状态 */
	unsigned short usAGVAction = m_mapAGVs[byAGVNo]->GetAction();			/*!< AGV当前动作 */
	unsigned char byAGVActStatus = m_mapAGVs[byAGVNo]->GetActionStatus();	/*!< AGV当前动作终状态 */

	// 任务未执行
	if (strcmp(strStatus, _TASK_STATUS_NONE) == 0)
	{
		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_PRE);
	}
	// 准备执行
	else if (strcmp(strStatus, _TASK_STATUS_PRE) == 0)
	{
		// 未抵达起点
		if (usAGVCurRFID != usStartRFID)
		{
			// 未落下
			if (byAGVCurLifter != _AGVLIFTER_DOWN || usAGVAction != _AGVACT_LIFTDOWN)
			{
				// 落下
				m_mapAGVs[byAGVNo]->LiftDown(usAGVCurRFID);

				return;
			}

			if (usAGVEndRFID != usStartRFID || m_mapAGVs[byAGVNo]->GetStatus() == _AGVSTA_WAIT)
			{
				m_mapAGVs[byAGVNo]->MoveTo(usStartRFID);
			}

			return;
		}

		// 未升起
		if (byAGVCurLifter != _AGVLIFTER_UP || usAGVAction != _AGVACT_LIFTUP)
		{
			if (m_mapAGVs[byAGVNo]->GetError() == _AGVERR_LIFTUP)
			{
				UpdateTaskText(nTaskNo, "异常:顶升失败,未能定位料车位置");

				return;
			}

			// 升起
			m_mapAGVs[byAGVNo]->LiftUp(usEndRFID);

			return;
		}

		UpdateTaskText(nTaskNo, "正常");

		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo, _TASK_STATUS_EXE);
	}
	// 正在执行
	else if (strcmp(strStatus, _TASK_STATUS_EXE) == 0)
	{
		// 未抵达终点RFID卡
		if (usAGVCurRFID != usEndRFID)
		{
			// 移动终点不一致
			if (usAGVEndRFID != usEndRFID || m_mapAGVs[byAGVNo]->GetStatus() == _AGVSTA_WAIT)
			{
				// 移动至终点
				m_mapAGVs[byAGVNo]->MoveTo(usEndRFID);
			}

			return;
		}

		// 抵达终点RFID卡
		// 未下降
		if (byAGVCurLifter != _AGVLIFTER_DOWN || usAGVAction != _AGVACT_LIFTDOWN)
		{
			// 下降
			m_mapAGVs[byAGVNo]->LiftDown(usEndRFID);

			return;
		}

		// 进入下一阶段
		FinishTask(nTaskNo, "系统正常结束任务");
	}
	// 已完成
	else if (strcmp(strStatus, _TASK_STATUS_FINISH) == 0)
	{
		DeleteTask(nTaskNo);
	}
	// 已取消
	else if (strcmp(strStatus, _TASK_STATUS_CANCEL) == 0)
	{
		DeleteTask(nTaskNo);
	}

	return;
}

void ServiceFunction::ProcessAGVHandEmptyTask(const int nTaskNo, const unsigned char byStart, const unsigned char byEnd, const unsigned char byAGVNo, const char* strStatus)
{
	// 终点工位无效
	if (m_mapStations.find(byEnd) == m_mapStations.end())
	{
		return;
	}

	unsigned short usEndRFID = m_mapStations[byEnd]->GetLocation();			/*!< 终止RFID卡 */

	// 无效AGV
	if (m_mapAGVs.find(byAGVNo) == m_mapAGVs.end())
	{
		return;
	}

	unsigned short usAGVCurRFID = m_mapAGVs[byAGVNo]->GetCurRFID();			/*!< AGV当前RFID卡 */
	unsigned short usAGVEndRFID = m_mapAGVs[byAGVNo]->GetEndRFID();			/*!< AGV终止RFID卡 */
	unsigned char byAGVCurLifter = m_mapAGVs[byAGVNo]->GetLifter();			/*!< AGV当前举升状态 */
	unsigned char byAGVCurCargo = m_mapAGVs[byAGVNo]->GetCargo();			/*!< AGV当前载货状态 */
	unsigned short usAGVAction = m_mapAGVs[byAGVNo]->GetAction();			/*!< AGV当前动作 */
	unsigned char byAGVActStatus = m_mapAGVs[byAGVNo]->GetActionStatus();	/*!< AGV当前动作终状态 */

	// 任务未执行
	if (strcmp(strStatus, _TASK_STATUS_NONE) == 0)
	{
		// 进入下一阶段
		ExecuteTask(nTaskNo, byAGVNo,_TASK_STATUS_PRE);
	}
	// 准备执行
	else if (strcmp(strStatus, _TASK_STATUS_PRE) == 0)
	{
		// 未下降
		if (byAGVCurLifter != _AGVLIFTER_DOWN || usAGVAction != _AGVACT_LIFTDOWN)
		{
			// 下降
			m_mapAGVs[byAGVNo]->LiftDown(usEndRFID);

			return;
		}

		ExecuteTask(nTaskNo, byAGVNo,_TASK_STATUS_EXE);
	}
	// 正在执行
	else if (strcmp(strStatus, _TASK_STATUS_EXE) == 0)
	{
		// 未抵达终点RFID卡
		if (usAGVCurRFID != usEndRFID)
		{
			// 移动终点不一致
			if (usAGVEndRFID != usEndRFID || m_mapAGVs[byAGVNo]->GetStatus() == _AGVSTA_WAIT)
			{
				// 移动至终点
				m_mapAGVs[byAGVNo]->MoveTo(usEndRFID);
			}
		}
		// 抵达终点RFID卡
		else
		{
			// 进入下一阶段
			FinishTask(nTaskNo, "系统正常结束任务");
		}
	}
	// 已完成
	else if (strcmp(strStatus, _TASK_STATUS_FINISH) == 0)
	{
		DeleteTask(nTaskNo);
	}
	// 已取消
	else if (strcmp(strStatus, _TASK_STATUS_CANCEL) == 0)
	{
		DeleteTask(nTaskNo);
	}

	return;
}

void ServiceFunction::ProcessCallSignal()
{
	for (map<unsigned char, ZGBCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it)
	{
		ZGBCaller* pCaller = it->second;

		unsigned char byStaion = pCaller->GetStation();

		if(m_mapStations.find(byStaion) == m_mapStations.end())
		{
			continue;
		}

		unsigned char byProcess = m_mapStations[byStaion]->GetProcess();
		unsigned short usRFID = m_mapStations[byStaion]->GetLocation();

		if (pCaller->EnableUse() == false)
		{
			continue;
		}

		if (pCaller->GetCancelPassSignal() == SIGNAL_ON)
		{
			CancelPass(byProcess, usRFID);
		}
		else if (pCaller->GetPassSignal() == SIGNAL_ON)
		{
			Pass(byProcess, usRFID);
		}

		if (pCaller->GetCancelNGSignal() == SIGNAL_ON)
		{
			CancelNG(byProcess, usRFID);
		}
		else if (pCaller->GetNGSignal() == SIGNAL_ON)
		{
			NG(byProcess, usRFID);
		}

		if (pCaller->GetCancelCallSignal() == SIGNAL_ON)
		{
			CancelCall(byProcess, usRFID);
		}
		else if (pCaller->GetCallSignal() == SIGNAL_ON)
		{
			Call(byProcess, usRFID);
		}
	}

	return;
}

void ServiceFunction::ProcessZigbeePackage()
{
	DataBuffer package = ZGBCtrler::GetProcessPackage();

	if (package.m_unSize == 0)
	{
		return;
	}

	/*
	Json::Value JsonRoot;

	JSONCPP_STRING errs;
	const char* pstr = (char*)package.m_pbyBuffer;

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("处理Zigbee报文失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return;
	}
	m_mutexJson.unlock();

	Json::Value JsonMsg = JsonRoot["Message"];

	int nNo = Json_ReadInt(JsonMsg["No"]);				// AGV编号
	string strCmd = Json_ReadString(JsonMsg["Cmd"]);	// AGV指令
	string strType = Json_ReadString(JsonMsg["Type"]);	// AGV类型

	if (strcmp(strType.c_str(), "Caller") != 0)
	{
		return;
	}

	if (m_mapCallers.find(nNo) == m_mapCallers.end())
	{
		return;
	}

	// 心跳报文
	if (strcmp(strCmd.c_str(), "Heartbeat") == 0)
	{
		Json::Value JsonResult = JsonRoot["Result"];

		int nSignalCall = Json_ReadInt(JsonResult["Call"]);
		int nSignalCancelCall = Json_ReadInt(JsonResult["CancelCall"]);
		int nSignalPass = Json_ReadInt(JsonResult["Pass"]);
		int nSignalCancelPass = Json_ReadInt(JsonResult["CancelPass"]);
		int nSignalNG = Json_ReadInt(JsonResult["NG"]);
		int nSignalCancelNG = Json_ReadInt(JsonResult["CancelNG"]);
		int nSignalScream = Json_ReadInt(JsonResult["Scream"]);

		if (m_mapCallers[nNo]->UpdateCallSignal(nSignalCall)
			|| m_mapCallers[nNo]->UpdateCancelCallSignal(nSignalCancelCall)
			|| m_mapCallers[nNo]->UpdatePassSignal(nSignalPass)
			|| m_mapCallers[nNo]->UpdateCancelPassSignal(nSignalCancelPass)
			|| m_mapCallers[nNo]->UpdateNGSignal(nSignalNG)
			|| m_mapCallers[nNo]->UpdateCancelNGSignal(nSignalCancelNG))
		{
			UpdateZGBCaller(m_mapCallers[nNo]);
		}

		if (m_mapCallers[nNo]->UpdateScreamSignal(nSignalScream))
		{
			switch (nSignalScream)
			{
			case SIGNAL_ON:
				Scream();
				break;
			case SIGNAL_OFF:
				CancelScream();
				break;
			}	
		}
		
	}
	// 其他报文
	else
	{
		return;
	}
	*/

	unsigned char byIndex = 0;
	unsigned char byNo = *(package.m_pbyBuffer + byIndex++);			// AGV编号
	unsigned char byCmd = *(package.m_pbyBuffer + byIndex++);			// AGV指令

	if (m_mapCallers.find(byNo) == m_mapCallers.end())
	{
		return;
	}

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

		if (m_mapCallers[byNo]->UpdateCallSignal(bySignalCall)
			|| m_mapCallers[byNo]->UpdateCancelCallSignal(bySignalCancelCall)
			|| m_mapCallers[byNo]->UpdatePassSignal(bySignalPass)
			|| m_mapCallers[byNo]->UpdateCancelPassSignal(bySignalCancelPass)
			|| m_mapCallers[byNo]->UpdateNGSignal(bySignalNG)
			|| m_mapCallers[byNo]->UpdateCancelNGSignal(bySignalCancelNG))
		{
			UpdateZGBCaller(m_mapCallers[byNo]);
		}

		if (m_mapCallers[byNo]->UpdateScreamSignal(bySignalScream))
		{
			switch (bySignalScream)
			{
			case SIGNAL_ON:
				Scream();
				break;
			case SIGNAL_OFF:
				CancelScream();
				break;
			}
		}

		break;
	}
	default:
	{
		return;
	}
	}

	return;
}


CAGV* ServiceFunction::ApplyForBatteryBoxTest1(CStation* pStation)
{
	unsigned char byProcess = pStation->GetProcess();

	bool bContinue = true;
	CAGV* pAGV = nullptr;

	// 从下2道工序抓取
	for (int i = 0; i < 3; ++i,++byProcess, bContinue = true, pAGV = nullptr)
	{
		pAGV = CatchAGVFromProcess(pStation, byProcess, bContinue);

		if (pAGV)
		{
			return pAGV;
		}

		if (bContinue == false)
		{
			break;
		}
	}

	if (bContinue == false)
	{
		return nullptr;
	}
	
	return CatchAGVFromOther(pStation);
}

CAGV* ServiceFunction::ApplyForBatteryBoxTest2(CStation* pStation)
{
	bool bContinue = true;
	CAGV* pAGV = nullptr;

	unsigned char byProcess = pStation->GetProcess();
	unsigned char byEnd = pStation->GetProcess() + 1;

	if (byEnd > _MES_BoxUnload)
	{
		byEnd = _MES_BoxStorageArea;
	}

	for (;; --byProcess, bContinue = true, pAGV = nullptr)
	{
		if (byProcess == _MES_NG)
		{
			byProcess = _MES_BoxUnload;
		}

		pAGV = CatchAGVFromProcess(pStation, byProcess, bContinue);

		if (pAGV)
		{
			return pAGV;
		}

		if (bContinue == false)
		{
			break;
		}

		if (byProcess == byEnd)
		{
			break;
		}
	}

	return nullptr;
}

CAGV* ServiceFunction::ApplyForBoxCoverInstallBuffer(CStation* pStation)
{
	bool bContinue = true;
	CAGV* pAGV = nullptr;

	unsigned char byProcess = pStation->GetProcess() - 1;

	pAGV = CatchAGVFromProcess(pStation, byProcess, bContinue);

	if (pAGV)
	{
		return pAGV;
	}

	if (bContinue == false)
	{
		return nullptr;
	}

	byProcess = pStation->GetProcess() + 1;

	pAGV = CatchAGVFromProcess(pStation, byProcess, bContinue);

	if (pAGV)
	{
		return pAGV;
	}

	if (bContinue == false)
	{
		return nullptr;
	}

	byProcess = pStation->GetProcess() - 2;

	for (;; --byProcess, bContinue = true, pAGV = nullptr)
	{
		if (byProcess == _MES_NG)
		{
			byProcess = _MES_BoxUnload;
		}

		if (byProcess == pStation->GetProcess())
		{
			break;
		}

		pAGV = CatchAGVFromProcess(pStation, byProcess, bContinue);

		if (pAGV)
		{
			return pAGV;
		}

		if (bContinue == false)
		{
			break;
		}
	}

	return nullptr;
}

CAGV* ServiceFunction::ApplyForEolTest(CStation* pStation)
{
	CAGV* pAGV = CatchAGVFromOther(pStation);

	if (pAGV)
	{
		return pAGV;
	}

	unsigned char byProcess = pStation->GetProcess()+1;
	bool bContinue = true;

	// 从下2道工序抓取
	for (int i = 0; i < 3; ++i, ++byProcess, bContinue = true, pAGV = nullptr)
	{
		pAGV = CatchAGVFromProcess(pStation, byProcess, bContinue);

		if (pAGV)
		{
			return pAGV;
		}

		if (bContinue == false)
		{
			break;
		}
	}

	if (bContinue == false)
	{
		return nullptr;
	}

	return nullptr;
}

CAGV* ServiceFunction::ApplyForNG(CStation* pStation)
{
	return CatchAGVFromNG(pStation);
}

CAGV* ServiceFunction::ApplyForOther(CStation* pStation)
{
	return CatchAGVFromOther(pStation);
}

CAGV* ServiceFunction::CatchAGVFromProcess(CStation* pStation, const unsigned char byProcess, bool& bContinue)
{
	/*
	 * 1、是否存在可用AGV
	 * 1.1在工位上
			1.1.1 是否存在对向行驶的AGV任务
	 * 1.2不在工位上
	*/
	bContinue = true;

	// 包含此工序的工位列表
	list<CStation*> listStation = GetStation(byProcess);

	for (list<CStation*>::iterator it = listStation.begin(); it != listStation.end(); ++it)
	{
		// 工位相同
		if (*it == pStation)
		{
			continue;
		}

		// 工位RFID
		unsigned short usRFID = (*it)->GetLocation();

		// RFID卡指针
		RFID* pRFID = &m_aRFIDArr[usRFID];

		// 工位上存在AGV
		if (pRFID->GetLocker())
		{
			bContinue = false;

			// 机械臂工序存在AGV
			if (byProcess == _MES_UpperModuleLifting || byProcess == _MES_LowerModuleLifting)
			{
				bContinue = false;

				return nullptr;
			}


			ZGBCaller* pCaller = GetCaller(usRFID);

			if (pCaller && pCaller->GetPassSignal() != SIGNAL_OFF)
			{
				continue;
			}

			// AGV指针
			CAGV* pAGV = (CAGV*)pRFID->GetLocker();

			if (pAGV->IsConnected() == false)
			{
				continue;
			}

			// AGV有任务
			if (DoesAGVHaveATask(pAGV->GetNo()) > 0)
			{
				continue;
			}

			return pAGV;
		}

		// 是否存在AGV即将到达工位
		CAGV* pAGV = IsAGVComing(pStation);

		if (pAGV)
		{
			bContinue = false;

			// AGV前往机械臂工序
			if (byProcess == _MES_UpperModuleLifting || byProcess == _MES_LowerModuleLifting)
			{
				return nullptr;
			}		
		}
	}

	return nullptr;
}

CAGV* ServiceFunction::CatchAGVFromNG(CStation* pStation)
{
	return nullptr;
}

CAGV* ServiceFunction::CatchAGVFromOther(CStation* pStation)
{
	unsigned char byProcess = pStation->GetProcess();
	unsigned char byNextProcess = byProcess - 1;

	bool bContinue = true;
	CAGV* pAGV = nullptr;

	for (;; --byNextProcess, bContinue = true, pAGV = nullptr)
	{
		if (byNextProcess == _MES_NG)
		{
			byNextProcess = _MES_BoxUnload;
		}

		// 在充电测试区1之后的工站抓取AGV
		if (byProcess >= _MES_BatteryBoxTest1)
		{
			// 抓到充电池测试区1为止
			if (byNextProcess < _MES_BatteryBoxTest1)
			{
				break;
			}
		}
		// 在机械手2之后的工站抓取AGV
		else if (byProcess > _MES_UpperModuleLifting)
		{
			// 抓到机械手2工站为止
			if (byNextProcess == _MES_UpperModuleLifting)
			{
				break;
			}
		}
		// 在机械手1之前的工站抓取AGV
		else if (byProcess < _MES_LowerModuleLifting)
		{
			// 抓到充电池测试区1为止
			// EOL测试之前
			if (byNextProcess == _MES_BatteryBoxTest1 - 1)
			{
				break;
			}
		}
		else if (byNextProcess == byProcess)
		{
			break;
		}

		pAGV = CatchAGVFromProcess(pStation, byNextProcess, bContinue);

		if (pAGV)
		{
			return pAGV;
		}

		if (bContinue == false)
		{
			break;
		}
	}

	return nullptr;
}

int ServiceFunction::DoesAGVHaveATask(unsigned char byNo)
{
	string strSQL = "";
	_RecordsetPtr pRecordset = nullptr;

	strSQL = StringEx::string_format("select [task_id] from AGV_TASK_TABLE where [task_executer]=%d", byNo);

	if (m_ado.GetRecordSet(strSQL.c_str(), pRecordset) == false)
	{
		return -1;
	}

	if (pRecordset->adoEOF)
	{
		//ado.Release();

		return 0;
	}

	_variant_t var;				/*!< 字段值 */

	var = pRecordset->GetCollect(_bstr_t("task_id"));

	if (var.vt != VT_NULL)
	{
		return (int)var;
	}

	//ado.Release();

	return -1;
}

CAGV* ServiceFunction::IsAGVComing(CStation* pStation)
{
	if (pStation == nullptr)
	{
		return nullptr;
	}

	// 工位RFID
	unsigned short usRFID = pStation->GetLocation();

	CAGV* pAGV = (CAGV*)m_aRFIDArr[usRFID].GetPreformLocker();

	if (pAGV)
	{
		return pAGV;
	}

	string strSQL = StringEx::string_format("select [task_executer] from AGV_TASK_TABLE where [task_end]=%d", pStation->GetNo());

	_RecordsetPtr pRecordset = nullptr;
	if (m_ado.GetRecordSet(strSQL.c_str(), pRecordset) == false)
	{
		return nullptr;
	}

	if (pRecordset->adoEOF)
	{
		//pAdo->Release();
		return nullptr;
	}

	unsigned char byNo = 0;

	_variant_t var;				/*!< 字段值 */

	var = pRecordset->GetCollect(_bstr_t("task_executer"));
	if (var.vt != VT_NULL)
	{
		byNo = (unsigned char)var;
	}

	//pAdo->Release();

	if (m_mapAGVs.find(byNo) == m_mapAGVs.end())
	{
		return nullptr;
	}

	return m_mapAGVs[byNo];
}

CAGV* ServiceFunction::IsAGVRunning(CStation* pStation)
{
	string strSQL = StringEx::string_format(
		"select [task_executer] from AGV_TASK_TABLE where [task_start]=%d", pStation->GetNo());

	_RecordsetPtr pRecordset = nullptr;
	if (m_ado.GetRecordSet(strSQL.c_str(), pRecordset) == false)
	{
		return nullptr;
	}

	if (pRecordset->adoEOF)
	{
		//ado.Release();

		return nullptr;
	}

	unsigned char byNo = 0;

	_variant_t var;				/*!< 字段值 */

	var = pRecordset->GetCollect(_bstr_t("task_executer"));
	if (var.vt != VT_NULL)
	{
		byNo = (unsigned char)var;
	}

	//ado.Release();

	if (m_mapAGVs.find(byNo) == m_mapAGVs.end())
	{
		return nullptr;
	}

	return  m_mapAGVs[byNo];
}

void ServiceFunction::UpdateZGBCaller(ZGBCaller* pCaller)
{
	if (m_pFunc == nullptr)
	{
		return;
	}

	if (pCaller == nullptr)
	{
		return;
	}

	unsigned char byNo = pCaller->GetNo();						/*!< 编号 */

	Json::Value JsonRoot;
	int nSignal = 0;

	JsonRoot["Type"] = Json::Value("Caller");
	JsonRoot["No"] = Json::Value(byNo);
	JsonRoot["PackOutbound"] = Json::Value(pCaller->IsPackOutBounded());

	nSignal = pCaller->GetCallSignal();
	JsonRoot["Signal"]["Call"] = Json::Value(nSignal);

	nSignal = pCaller->GetCancelCallSignal();
	JsonRoot["Signal"]["CancelCall"] = Json::Value(nSignal);

	nSignal = pCaller->GetPassSignal();
	JsonRoot["Signal"]["Pass"] = Json::Value(nSignal);

	nSignal = pCaller->GetCancelPassSignal();
	JsonRoot["Signal"]["CancelPass"] = Json::Value(nSignal);

	nSignal = pCaller->GetNGSignal();
	JsonRoot["Signal"]["NG"] = Json::Value(nSignal);

	nSignal = pCaller->GetCancelNGSignal();
	JsonRoot["Signal"]["CancelNG"] = Json::Value(nSignal);

	nSignal = pCaller->GetScreamSignal();
	JsonRoot["Signal"]["Scream"] = Json::Value(nSignal);

	bool bUpdate = false;
	if (m_mapStrUpdateCallers.find(byNo) == m_mapStrUpdateCallers.end())
	{
		g_mutex.lock();
		m_mapStrUpdateCallers[byNo] = new Json::Value(JsonRoot);
		g_mutex.unlock();

		bUpdate = true;
	}
	else
	{
		if (*m_mapStrUpdateCallers[byNo] != JsonRoot)
		{
			g_mutex.lock();
			m_mapStrUpdateCallers[byNo]->clear();
			*m_mapStrUpdateCallers[byNo]= JsonRoot;
			g_mutex.unlock();

			bUpdate = true;
		}
	}

	if (bUpdate == false)
	{
		JsonRoot.clear();
		return;
	}

	m_bUpdateCaller[byNo-1] = true;

	if (m_pFunc && m_pFunc(m_mapStrUpdateCallers[byNo], m_pPtr) == false)
	{
		g_mutex.lock();
		m_mapStrUpdateCallers[byNo]->clear();
		g_mutex.unlock();
	}

	JsonRoot.clear();

	return;
}

void ServiceFunction::RecordAGVStatus()
{
	for (unsigned char i = 0; i < UCHAR_MAX; ++i)
	{
		if (m_bUpdateAGV[i] == false)
		{
			continue;
		}

		if (m_mapStrUpdateAGVs.find(i + 1) == m_mapStrUpdateAGVs.end())
		{
			continue;
		}

		g_mutex.lock();
		Json::Value JsonRoot(*m_mapStrUpdateAGVs[i + 1]);
		g_mutex.unlock();

		unsigned char byNo = Json_ReadInt(JsonRoot["No"]);
		unsigned short usCurRFID = Json_ReadInt(JsonRoot["RFID"]["Cur"]);
		unsigned short usEndRFID = Json_ReadInt(JsonRoot["RFID"]["End"]);
		unsigned char byStatus = Json_ReadInt(JsonRoot["Status"]);

		string strSQL = "";
		strSQL = StringEx::string_format(
			"if not exists (select [agv_id] from AGV_CONFIG_TABLE where [agv_id]=%d) "\
			"insert into AGV_CONFIG_TABLE([agv_id],[agv_cur_rfid],[agv_end_rfid],[agv_status]) values(%d,%d,%d,%d) "\
			"else "\
			"update AGV_CONFIG_TABLE set [agv_cur_rfid]=%d,[agv_end_rfid]=%d,[agv_status]=%d where [agv_id]=%d"
			, byNo, byNo, usCurRFID, usEndRFID, byStatus, usCurRFID, usEndRFID, byStatus, byNo);

		ULL ullTime = GetTickCountEX();

		_variant_t var;
		if (m_ado.ExecuteSQL(strSQL.c_str(), var) == false)
		{
			continue;
		}

		DebugPrint::Printf("执行SQL语句:\n%s\n耗时：%dms", strSQL.c_str(), GetTickCountEX() - ullTime);

		m_bUpdateAGV[i] = false;
	}

	return;
}

void ServiceFunction::RecordCallerStatus()
{
	for (unsigned char i = 0; i < UCHAR_MAX; ++i)
	{
		if (m_bUpdateCaller[i] == false)
		{
			continue;
		}

		if (m_mapStrUpdateCallers.find(i + 1) == m_mapStrUpdateCallers.end())
		{
			continue;
		}
		g_mutex.lock();
		Json::Value JsonRoot(*m_mapStrUpdateCallers[i + 1]);
		g_mutex.unlock();

		unsigned char byNo = Json_ReadInt(JsonRoot["No"]);
		bool bPackOutbound = Json_ReadBool(JsonRoot["PackOutbound"]);
		unsigned char bySignaleCall = Json_ReadInt(JsonRoot["Signal"]["Call"]);
		unsigned char bySignaleCancelCall = Json_ReadInt(JsonRoot["Signal"]["CancelCall"]);
		unsigned char bySignalePass = Json_ReadInt(JsonRoot["Signal"]["Pass"]);
		unsigned char bySignaleCancelPass = Json_ReadInt(JsonRoot["Signal"]["CancelPass"]);
		unsigned char bySignaleNG = Json_ReadInt(JsonRoot["Signal"]["NG"]);
		unsigned char bySignaleCancelNG = Json_ReadInt(JsonRoot["Signal"]["CancelNG"]);
		unsigned char bySignaleScream = Json_ReadInt(JsonRoot["Signal"]["Scaream"]);

		string strSQL = "";
		strSQL = StringEx::string_format(
			"if not exists (select [caller_id] from CALLER_CONFIG_TABLE where [caller_id]=%d) "\
			"insert into CALLER_CONFIG_TABLE([caller_id],[caller_packoutbound],[caller_signal_call],"\
			"[caller_signal_cancelcall],[caller_signal_pass],[caller_signal_cancelpass],"\
			"[caller_signal_ng],[caller_signal_cancelng]) "\
			"values(%d,%d,%d,%d,%d,%d,%d,%d) "\
			"else "\
			"update CALLER_CONFIG_TABLE set [caller_packoutbound]=%d,[caller_signal_call]=%d,"\
			"[caller_signal_cancelcall]=%d,"\
			"[caller_signal_pass]=%d,[caller_signal_cancelpass]=%d,[caller_signal_ng]=%d,"\
			"[caller_signal_cancelng]=%d where [caller_id]=%d"
			, byNo, byNo, bPackOutbound, bySignaleCall, bySignaleCancelCall, bySignalePass, bySignaleCancelPass, bySignaleNG, bySignaleCancelNG,
			bPackOutbound, bySignaleCall, bySignaleCancelCall, bySignalePass, bySignaleCancelPass, bySignaleNG, bySignaleCancelNG, byNo);

		ULL ullTime = GetTickCountEX();

		_variant_t var;
		if (m_ado.ExecuteSQL(strSQL.c_str(), var) == false)
		{
			continue;
		}

		DebugPrint::Printf("执行SQL语句:\n%s\n耗时：%dms", strSQL.c_str(), GetTickCountEX() - ullTime);

		m_bUpdateCaller[i] = false;
	}

	return;
}

bool ServiceFunction::Open(UpdateStatus func, void* ptr)
{
	if (m_ado.OnInitADOConn() == false)
	{
		DebugPrint::Print("启动业务功能失败:\n连接数据库失败!");
		SystemLog::Record("启动业务功能失败:\n连接数据库失败!");
		return false;
	}

	if (m_bKey == false)
	{
		DebugPrint::Print("缺少使用权限!");
		SystemLog::Record("缺少使用权限!");
	}

	if (BaseFunction::Open(func, ptr) == false)
	{
		return false;
	}

	if (m_pZGBCtrler == nullptr || m_pZGBCtrler->Open() == false)
	{
		DebugPrint::Print("启动业务功能失败:\n连接Zigbee控制器失败!");
		SystemLog::Record("启动业务功能失败:\n连接Zigbee控制器失败!");
		return false;
	}

	for (map<unsigned char, ZGBCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it)
	{
		if (it->second->EnableUse() == false)
		{
			continue;
		}

		m_pZGBCtrler->Heartbeat(it->second);

		UpdateZGBCaller(it->second);
	}

	DebugPrint::Print("业务功能启动");
	SystemLog::Record("业务功能启动");

	return true;
}

void ServiceFunction::Close()
{
	BaseFunction::Close();

	//if (m_ado.IsLocked())
	//{
	//	m_ado.Release();
	//}

	//m_ado.ExitConnect();

	if (m_pMes)
	{
		if (m_pMes->IsOpen())
		{
			m_pMes->Close();
		}
	}

	if (m_pZGBCtrler)
	{
		if(m_pZGBCtrler->IsOpened())
		{
			m_pZGBCtrler->Close();
		}
	}

	DebugPrint::Print("业务功能关闭");
	SystemLog::Record("业务功能关闭");

	return;
}

void ServiceFunction::Release()
{
	m_pFunc = nullptr;

	Close();

	BaseFunction::Release();

	if (m_pMes)
	{
		delete m_pMes;
		m_pMes = nullptr;
	}

	if (m_pZGBCtrler)
	{
		delete m_pZGBCtrler;
		m_pZGBCtrler = nullptr;
	}

	for (map<unsigned char, CStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); it = m_mapStations.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, ZGBCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); it = m_mapCallers.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, Json::Value*>::iterator it = m_mapStrUpdateCallers.begin(); it != m_mapStrUpdateCallers.end(); ++it)
	{
		it->second->clear();
	}

	m_strMES = "";
	m_strADO = "";
	m_strZigbee = "";
	m_strStation = "";

	m_ado.ExitConnect();

	DebugPrint::Print("业务功能资源释放");
	SystemLog::Record("业务功能资源释放");

	return;
}

void ServiceFunction::ReleaseString()
{
	for (map<unsigned char, Json::Value*>::iterator it = m_mapStrUpdateCallers.begin(); it != m_mapStrUpdateCallers.end(); it = m_mapStrUpdateCallers.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, Json::Value*>::iterator it = m_mapStrUpdateTasks.begin(); it != m_mapStrUpdateTasks.end(); it = m_mapStrUpdateTasks.erase(it))
	{
		delete it->second;
	}

	return; BaseFunction::ReleaseString();
}

bool ServiceFunction::SetKey(bool bKey)
{
	if (m_bKey == bKey)
	{
		return false;
	}

	m_bKey = bKey;

	string strSql = StringEx::string_format(
		"if not exists(select * from AGV_SYSTEM_CONFIG where [system_name]='Key'and [system_value]='%d') "\
		"update AGV_SYSTEM_CONFIG set [system_value]='%d' where [system_name]='Key'"
		, m_bKey, m_bKey);

	if (m_bKey)
	{
		DebugPrint::Print("用户失去使用权限");
		SystemLog::Record("用户失去使用权限");
	}

	return true;
}

bool ServiceFunction::PackOutBound(const char* strWorkPosition)
{
	// 无需PACK出站校验
	if (strcmp(strWorkPosition, "") == 0)
	{
		// 更新放行状态等待工站呼叫
		return true;
	}

	// 未连接MES
	if (m_pMes == nullptr)
	{
		return false;
	}

	if (m_pMes->IsOpen())
	{
		m_pMes->Close();
	}

	if (m_pMes->Connect(strWorkPosition) == false)
	{
		return false;
	}

	if (m_pMes->PackOutBound(strWorkPosition) == false)
	{
		return false;
	}

	m_pMes->Close();
	
	return true;
}

void ServiceFunction::InitADO()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strADO.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化ADO失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return;
	}
	m_mutexJson.unlock();

	m_ado.m_bstrConnect = Json_ReadString(JsonRoot["Connect"]).c_str();
	m_ado.m_bstrUser = Json_ReadString(JsonRoot["User"]).c_str();
	m_ado.m_bstrPassword = Json_ReadString(JsonRoot["Password"]).c_str();

	JsonRoot.clear();

	return;
}

bool ServiceFunction::InitStations()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strStation.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化工位失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return false;
	}
	m_mutexJson.unlock();

	Json::Value ObjectArray,Param;
	ObjectArray = JsonRoot["Station"];

	if (ObjectArray.size() == 0)
	{
		return false;
	}

	for (size_t k = 0; k < ObjectArray.size(); ++k, Param.clear())
	{
		int nNo = Json_ReadInt(ObjectArray[(int)k]["No"]);
		string strName = StringEx::Utf8ToAcsii(Json_ReadString(ObjectArray[(int)k]["Name"]).c_str());	
		int nLocation = Json_ReadInt(ObjectArray[(int)k]["Location"]);
		Param = ObjectArray[(int)k]["Param"];
		int nProcess = Json_ReadInt(Param["Process"]);
		bool bMES = Json_ReadBool(Param["MES"].asBool());
		string strMESWP = StringEx::Utf8ToAcsii(Json_ReadString(Param["MESWP"]).c_str());

		if (m_mapStations.find(nNo) == m_mapStations.end())
		{
			m_mapStations[nNo] = new CStation(nNo, nLocation, nProcess, strName.c_str(), strMESWP.c_str(), bMES);
		}
		else
		{
			m_mapStations[nNo]->BindLocaion(nLocation);
			m_mapStations[nNo]->BindProcess(nProcess);
			m_mapStations[nNo]->SetName(strName.c_str());
			m_mapStations[nNo]->SetMESWP(strMESWP.c_str());
			m_mapStations[nNo]->SetEnableMES(bMES);
		}
	}

	ObjectArray.clear();
	JsonRoot.clear();

	return true;
}

bool ServiceFunction::InitZigbee()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strZigbee.c_str();

	m_mutexJson.lock();
	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化ZIGBEE失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		m_mutexJson.unlock();
		return false;
	}
	m_mutexJson.unlock();

	Json::Value Object;
	Object = JsonRoot["Control"];

	int nNo = Json_ReadInt(Object["No"]);
	int nPort = Json_ReadInt(Object["Port"]);
	int nBaud = Json_ReadInt(Object["Baud"]);
	char chParity = Json_ReadString(Object["Parity"]).at(0);
	int nDataBits = Json_ReadInt(Object["DataBits"]);
	int nStopBits = Json_ReadInt(Object["StopBits"]);

	if (m_pZGBCtrler)
	{
		if (m_pZGBCtrler->IsOpened())
		{
			m_pZGBCtrler->Close();
		}

		delete m_pZGBCtrler;
		m_pZGBCtrler = nullptr;
	}

	m_pZGBCtrler = new ZGBCtrler(nNo, nPort, nBaud, chParity, nDataBits, nStopBits);

	nNo = 0;

	Json::Value ObjectArray;
	ObjectArray = JsonRoot["Caller"];

	if (ObjectArray.size() == 0)
	{
		return false;
	}

	for (size_t k = 0; k < ObjectArray.size(); ++k, nNo = 0)
	{
		nNo = Json_ReadInt(ObjectArray[(int)k]["No"]);
		int nStation = Json_ReadInt(ObjectArray[(int)k]["Station"]);
		bool bUse = Json_ReadBool(ObjectArray[(int)k]["Use"].asBool());
		bool bPackOutBound = Json_ReadBool(ObjectArray[(int)k]["PackOutBound"].asBool());
		string strMAC = Json_ReadString(ObjectArray[(int)k]["MAC"]);
		unsigned char bySignalCall = Json_ReadInt(ObjectArray[(int)k]["Signal"]["Call"]);
		unsigned char bySignalCancelCall = Json_ReadInt(ObjectArray[(int)k]["Signal"]["CancelCall"]);
		unsigned char bySignalPass = Json_ReadInt(ObjectArray[(int)k]["Signal"]["Pass"]);
		unsigned char bySignalCancelPass = Json_ReadInt(ObjectArray[(int)k]["Signal"]["CancelPass"]);
		unsigned char bySignalNG = Json_ReadInt(ObjectArray[(int)k]["Signal"]["NG"]);
		unsigned char bySignalCancelNG = Json_ReadInt(ObjectArray[(int)k]["Signal"]["CancelNG"]);

		if (m_mapCallers.find(nNo) == m_mapCallers.end())
		{
			m_mapCallers[nNo] = new ZGBCaller(nNo, nStation, bUse);
		}
		else
		{
			m_mapCallers[nNo]->BindStation(nStation);
			m_mapCallers[nNo]->SetEnableUse(bUse);
		}

		char achMAC[4];
		memcpy_s(achMAC, 4, strMAC.c_str(), 4);

		m_mapCallers[nNo]->BindMACAddress(achMAC);
		m_mapCallers[nNo]->UpdatePackOutBound(bPackOutBound);
		m_mapCallers[nNo]->UpdateCallSignal(bySignalCall);
		m_mapCallers[nNo]->UpdateCancelCallSignal(bySignalCancelCall);
		m_mapCallers[nNo]->UpdatePassSignal(bySignalPass);
		m_mapCallers[nNo]->UpdateCancelPassSignal(bySignalCancelPass);
		m_mapCallers[nNo]->UpdateNGSignal(bySignalNG);
		m_mapCallers[nNo]->UpdateCancelNGSignal(bySignalCancelNG);

		UpdateZGBCaller(m_mapCallers[nNo]);
	}

	ObjectArray.clear();
	JsonRoot.clear();

	return true;
}

bool ServiceFunction::Init(UpdateStatus func, void* ptr)
{
	if (BaseFunction::Init(func, ptr) == false)
	{
		return false;
	}

	InitADO();

	if (InitMES() == false
		|| InitStations() == false
		|| InitZigbee() == false
		)
	{
		return false;
	}

	DebugPrint::Print("业务功能初始化");
	SystemLog::Record("业务功能初始化");

	return true;
}

void ServiceFunction::JsonStringToMES(const char* strJson)
{
	m_strMES = strJson;

	return;
}

void ServiceFunction::JsonStringToMES(const wchar_t* wstrJson)
{
	m_strMES = StringEx::wstring_to_string(wstrJson);

	return;
}

void ServiceFunction::JsonFileToMES(const char* strFile)
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

	m_strMES = ostring.str();

	return;
}

void ServiceFunction::JsonFileToMES(const wchar_t* wstrFile)
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

	m_strMES = ostring.str();

	return;
}

void ServiceFunction::JsonStringToADO(const char* strJson)
{
	m_strADO = strJson;

	return;
}

void ServiceFunction::JsonStringToADO(const wchar_t* wstrJson)
{
	m_strADO = StringEx::wstring_to_string(wstrJson);

	return;
}

void ServiceFunction::JsonFileToADO(const char* strFile)
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

	m_strADO = ostring.str();

	return;
}

void ServiceFunction::JsonFileToADO(const wchar_t* wstrFile)
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

	m_strADO = ostring.str();

	return;
}

void ServiceFunction::JsonStringToStations(const char* strJson)
{
	m_strStation = strJson;

	return;
}

void ServiceFunction::JsonStringToStations(const wchar_t* wstrJson)
{
	m_strStation = StringEx::wstring_to_string(wstrJson);

	return;
}

void ServiceFunction::JsonFileToStations(const char* strFile)
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

	m_strStation = ostring.str();

	return;
}

void ServiceFunction::JsonFileToStations(const wchar_t* wstrFile)
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

	m_strStation = ostring.str();

	return;
}

void ServiceFunction::JsonStringToZigbee(const char* strJson)
{
	m_strZigbee = strJson;

	return;
}

void ServiceFunction::JsonStringToZigbee(const wchar_t* wstrJson)
{
	m_strZigbee = StringEx::wstring_to_string(wstrJson);

	return;
}

void ServiceFunction::JsonFileToZigbee(const char* strFile)
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

	m_strZigbee = ostring.str();

	return;
}

void ServiceFunction::JsonFileToZigbee(const wchar_t* wstrFile)
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

	m_strZigbee = ostring.str();

	return;
}

#endif //_SERVICEFUNCTION_H


/*
int main()
{
	ServiceFunction m_service;

	m_service.JsonFileToADO("../File/ADO.json");
	m_service.JsonFileToAGVs("../File/AGVs.json");
	m_service.JsonFileToMES("../File/MES.json");
	m_service.JsonFileToRFIDs("../File/RFIDs.json");
	m_service.JsonFileToServer("../File/Server.json");
	m_service.JsonFileToStations("../File/Stations.json");
	m_service.JsonFileToTraffics("../File/Traffics.json");
	m_service.JsonFileToZigbee("../File/Zigbee.json");

	m_service.Init();

	if (m_service.Open())
	{
		cout << "程序启动成功";

		cin.get();
	}
	else
	{
		cout << "程序启动失败";
	}

	m_service.Close();

	m_service.Release();

	cout << "程序启动退出";

	cin.get();
}
*/