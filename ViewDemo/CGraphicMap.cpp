#include "CGraphicMap.h"

#ifdef _MAP_H
#include "../String/StringEx.h"
#include "../String/DebugPrint.h"
#include "../String/SystemLog.h"

bool CGraphicMap::g_bInstall = false;

CGraphicMap::CGraphicMap(void* pMainFram, UpdateWidget func)
	:m_pBK(nullptr)
	, m_bCreateNew(false)
	, m_pTmpCharger(nullptr)
	, m_pTmpRest(nullptr)
	, m_pTmpRFID(nullptr)
	, m_pTmpStation(nullptr)
	, m_pTmpTrack(nullptr)
	, m_pTmpCaller(nullptr)
	, m_bRefresh(true)
	//, m_bEdit(false)
	, m_bMulti(false)
	//, m_bSaveByADO(false)
	, m_strADO("")
	, m_pMainFram(pMainFram)
	, m_funcUpdateWidget(func)
	, m_bInstall(false)
	, m_unNewed(0)
	, m_pclBmp(nullptr)
{
	m_pJsonReader = m_JsonBuilder.newCharReader();
}

CGraphicMap::~CGraphicMap()
{
	if (m_pclBmp)
	{
		delete m_pclBmp;
		m_pclBmp = nullptr;
	}

	SaveMap();

	SaveBackgroundByADO();

	ReleaseTmpWidget();
	ReleaseWidget();

	delete m_pJsonReader;

	if (g_bInstall)
	{
		CGraphicAGV::Uninstall();
		CGraphicBackground::Uninstall();
		CGraphicCharger::Uninstall();
		CGraphicRest::Uninstall();
		CGraphicRFID::Uninstall();
		CGraphicStation::Uninstall();
		CGraphicCaller::Uninstall();

		g_bInstall = false;
	}

	//if (m_ado.IsLocked())
	//{
	//	m_ado.Release();
	//}

	m_ado.ExitConnect();
}

const bool CGraphicMap::CreateNewWidget(Json::Value& JsonValue)
{
	if (CGraph::EnableEdit() == false)
	{
		return false;
	}

	Cancel();

	if (m_bCreateNew)
	{
		ReleaseTmpWidget();

		m_bCreateNew = false;
	}

	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonCtrl;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	lock_guard<mutex> lock(m_mutex);

	// 充电器
	if (strcmp(strType.c_str(), "Charger") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapChargers.find(byNo) != m_mapChargers.end())
		{
			return false;
		}

		unsigned short usLocation = Json_ReadInt(JsonParam["Location"]);
		string strName = StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str());
		unsigned char byCtrler = Json_ReadInt(JsonParam["Ctrler"]);
		string strParam = JsonParam["Param"].toStyledString();

		m_pTmpCharger = new CGraphicCharger(byNo, usLocation, byCtrler, strName, strParam);
	}
	// 待机位
	else if (strcmp(strType.c_str(), "Rest") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapRests.find(byNo) != m_mapRests.end())
		{
			return false;
		}

		unsigned short usLocation = Json_ReadInt(JsonParam["Location"]);
		string strName = StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str());
		string strParam = JsonParam["Param"].toStyledString();

		m_pTmpRest = new CGraphicRest(byNo, usLocation, strName, strParam);
	}
	// RFID卡
	else if (strcmp(strType.c_str(), "RFID") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapRFIDs.find(byNo) != m_mapRFIDs.end())
		{
			return false;
		}

		m_pTmpRFID = new CGraphicRFID(byNo);
	}
	// 工位
	else if (strcmp(strType.c_str(), "Station") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapStations.find(byNo) != m_mapStations.end())
		{
			return false;
		}

		unsigned short usLocation = Json_ReadInt(JsonParam["Location"]);
		string strName = StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str());
		string strParam = JsonParam["Param"].toStyledString();

		m_pTmpStation = new CGraphicStation(byNo, usLocation, strName, strParam);
	}
	// 磁轨
	else if (strcmp(strType.c_str(), "Track") == 0)
	{
		bool bArc = Json_ReadBool(JsonParam["Arc"].asBool());

		m_pTmpTrack = new CGraphicTrack(bArc);
	}
	// 呼叫器
	else if (strcmp(strType.c_str(), "Caller") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapCallers.find(byNo) != m_mapCallers.end())
		{
			return false;
		}

		unsigned char byStation = Json_ReadInt(JsonParam["Station"]);
		string strParam = JsonParam["Param"].toStyledString();

		m_pTmpCaller = new CGraphicCaller(byNo, byStation, strParam);
	}

	m_bCreateNew = true;

	// AGV
	if (strcmp(strType.c_str(), "AGV") == 0)
	{
		m_bCreateNew = false;

		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapAGVs.find(byNo) != m_mapAGVs.end())
		{
			return false;
		}

		string strParam = JsonParam["Param"].toStyledString();

		m_mapAGVs[byNo] = new CGraphicAGV(byNo, strParam);

		JsonCtrl["Control"] = Json::Value("Add");
		JsonCtrl["Param"] = JsonRoot;
		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}

		++m_unNewed;
	}

	strType.clear();
	JsonParam.clear();

	return true;
}

const bool CGraphicMap::FindWidget(Json::Value& JsonValue)
{
	if (CGraph::EnableEdit() == false)
	{
		return false;
	}

	if (m_bCreateNew)
	{
		ReleaseTmpWidget();

		m_bCreateNew = false;
	}

	Cancel();

	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	// 充电器
	if (strcmp(strType.c_str(), "Charger") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapChargers.find(byNo) == m_mapChargers.end())
		{
			return false;
		}

		m_mapChargers[byNo]->Locate();
	}
	// 待机位
	else if (strcmp(strType.c_str(), "Rest") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapRests.find(byNo) == m_mapRests.end())
		{
			return false;
		}

		m_mapRests[byNo]->Locate();
	}
	// RFID卡
	else if (strcmp(strType.c_str(), "RFID") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapRFIDs.find(byNo) == m_mapRFIDs.end())
		{
			return false;
		}

		m_mapRFIDs[byNo]->Locate();
	}
	// 工位
	else if (strcmp(strType.c_str(), "Station") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapStations.find(byNo) == m_mapStations.end())
		{
			return false;
		}

		m_mapStations[byNo]->Locate();
	}
	// 呼叫器
	else if (strcmp(strType.c_str(), "Caller") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapCallers.find(byNo) == m_mapCallers.end())
		{
			return false;
		}

		m_mapCallers[byNo]->Locate();
	}

	strType.clear();
	JsonParam.clear();

	return true;
}

const bool CGraphicMap::EditWidget(Json::Value& JsonValue)
{
	if (CGraph::EnableEdit() == false)
	{
		return false;
	}

	if (m_bCreateNew)
	{
		ReleaseTmpWidget();

		m_bCreateNew = false;
	}

	Cancel();

	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonCtrl;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	JsonCtrl["Control"] = Json::Value("Edit");

	// 充电器
	if (strcmp(strType.c_str(), "Charger") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapChargers.find(byNo) == m_mapChargers.end())
		{
			return false;
		}

		unsigned short usLocation = m_mapChargers[byNo]->GetLocation();
		string strName = m_mapChargers[byNo]->GetName();
		unsigned char byCtrler = m_mapChargers[byNo]->GetCtrler();
		string strParam = m_mapChargers[byNo]->GetParam();

		m_mapChargers[byNo]->SetName(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str()).c_str());
		m_mapChargers[byNo]->SetLocation(Json_ReadInt(JsonParam["Location"]));
		m_mapChargers[byNo]->SetCtrler(Json_ReadInt(JsonParam["Ctrler"]));
		m_mapChargers[byNo]->SetParam(JsonParam["Param"].toStyledString().c_str());
	}
	// 待机位
	else if (strcmp(strType.c_str(), "Rest") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapRests.find(byNo) == m_mapRests.end())
		{
			return false;
		}

		unsigned short usLocation = m_mapStations[byNo]->GetLocation();
		string strName = m_mapStations[byNo]->GetName();
		string strParam = m_mapStations[byNo]->GetParam();

		m_mapRests[byNo]->SetLocation(Json_ReadInt(JsonParam["Location"]));
		m_mapRests[byNo]->SetName(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str()).c_str());
		m_mapRests[byNo]->SetParam(JsonParam["Param"].toStyledString().c_str());
	}
	// 工位
	else if (strcmp(strType.c_str(), "Station") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapStations.find(byNo) == m_mapStations.end())
		{
			return false;
		}

		unsigned short usLocation = m_mapStations[byNo]->GetLocation();
		string strName = m_mapStations[byNo]->GetName();
		string strParam = m_mapStations[byNo]->GetParam();

		m_mapStations[byNo]->SetLocation(Json_ReadInt(JsonParam["Location"]));
		m_mapStations[byNo]->SetName(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str()).c_str());
		m_mapStations[byNo]->SetParam(JsonParam["Param"].toStyledString().c_str());
	}
	// AGV
	else if (strcmp(strType.c_str(), "AGV") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapAGVs.find(byNo) == m_mapAGVs.end())
		{
			return false;
		}

		string strParam = m_mapAGVs[byNo]->GetParam();

		m_mapAGVs[byNo]->SetParam(JsonParam["Param"].toStyledString().c_str());
	}
	// 呼叫器
	else if (strcmp(strType.c_str(), "Caller") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapCallers.find(byNo) == m_mapCallers.end())
		{
			return false;
		}

		unsigned char byStation = m_mapCallers[byNo]->GetStation();
		string strParam = m_mapCallers[byNo]->GetParam();

		m_mapCallers[byNo]->BindStation(Json_ReadInt(JsonParam["Station"]));
		m_mapCallers[byNo]->SetParam(JsonParam["Param"].toStyledString().c_str());
	}

	JsonCtrl["Param"] = JsonRoot;
	if (m_funcUpdateWidget)
	{
		m_funcUpdateWidget(JsonCtrl, m_pMainFram);
	}

	++m_unNewed;

	JsonCtrl.clear();
	strType.clear();
	JsonParam.clear();

	return true;
}

Json::Value CGraphicMap::GetWidget(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	Json::CharReaderBuilder JsonBuilder;
	Json::CharReader* pJsonReader = JsonBuilder.newCharReader();
	JSONCPP_STRING JstrErrs;

	string str = "";

	// 充电器
	if (strcmp(strType.c_str(), "Charger") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapChargers.find(byNo) == m_mapChargers.end())
		{
			return "";
		}

		JsonParam["Location"] = Json::Value(m_mapChargers[byNo]->GetLocation());
		JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(m_mapChargers[byNo]->GetName().c_str()));
		JsonParam["Ctrler"] = Json::Value(m_mapChargers[byNo]->GetCtrler());

		str.append(m_mapChargers[byNo]->GetParam().c_str());
		const char* pstr = str.c_str();

		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return "";
		}
	}
	// 待机位
	else if (strcmp(strType.c_str(), "Rest") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapRests.find(byNo) == m_mapRests.end())
		{
			return "";
		}

		JsonParam["Location"] = Json::Value(m_mapRests[byNo]->GetLocation());
		JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(m_mapRests[byNo]->GetName().c_str()));

		str.append(m_mapRests[byNo]->GetParam().c_str());
		const char* pstr = str.c_str();

		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return "";
		}
	}
	// 工位
	else if (strcmp(strType.c_str(), "Station") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapStations.find(byNo) == m_mapStations.end())
		{
			return "";
		}
		
		JsonParam["Location"] = Json::Value(m_mapStations[byNo]->GetLocation());
		JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(m_mapStations[byNo]->GetName().c_str()));

		str.append(m_mapStations[byNo]->GetParam().c_str());
		const char* pstr = str.c_str();

		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return "";
		}
	}
	// AGV
	else if (strcmp(strType.c_str(), "AGV") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapAGVs.find(byNo) == m_mapAGVs.end())
		{
			return "";
		}

		str.append(m_mapAGVs[byNo]->GetParam().c_str());
		const char* pstr = str.c_str();

		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return "";
		}
	}
	// 呼叫器
	else if (strcmp(strType.c_str(), "Caller") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapCallers.find(byNo) == m_mapCallers.end())
		{
			return "";
		}

		JsonParam["Station"] = Json::Value(m_mapCallers[byNo]->GetStation());

		str.append(m_mapCallers[byNo]->GetParam().c_str());
		const char* pstr = str.c_str();

		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return "";
		}
	}

	JsonRoot["Param"] = JsonParam;

	delete pJsonReader;

	str.clear();

	return JsonRoot;
}

Json::Value CGraphicMap::GetWidget(PointF ptWinpoint)
{
	if (CGraph::EnableEdit() == false)
	{
		return "";
	}

	if (m_bCreateNew)
	{
		ReleaseTmpWidget();

		m_bCreateNew = false;
	}

	//Cancel();

	Json::CharReaderBuilder JsonBuilder;
	Json::CharReader* pJsonReader = JsonBuilder.newCharReader();
	JSONCPP_STRING JstrErrs;

	Json::Value JsonRoot, JsonParam;

	string str = "";
	const char* pstr = nullptr;

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); ++it, str.clear())
	{
		if (it->second->IsSelected() && it->second->IsInside(ptWinpoint))
		{
			JsonRoot["Type"] = "Charger";

			JsonParam["No"] = Json::Value(it->second->GetNo());

			JsonParam["Location"] = Json::Value(it->second->GetLocation());
			JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(it->second->GetName().c_str()));
			JsonParam["Ctrler"] = Json::Value(it->second->GetCtrler());

			str.append(it->second->GetParam().c_str());
			pstr = str.c_str();

			if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
			{
				DebugPrint::Print(JstrErrs.c_str());
				DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

				JstrErrs.clear();
				return "";
			}

			JsonRoot["Param"] = JsonParam;

			delete pJsonReader;

			str.clear();

			return JsonRoot;
		}
	}

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); ++it, str.clear())
	{
		if (it->second->IsSelected() && it->second->IsInside(ptWinpoint))
		{
			JsonRoot["Type"] = "Rest";

			JsonParam["No"] = Json::Value(it->second->GetNo());

			JsonParam["Location"] = Json::Value(it->second->GetLocation());
			JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(it->second->GetName().c_str()));

			str.append(it->second->GetParam().c_str());
			pstr = str.c_str();

			if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
			{
				DebugPrint::Print(JstrErrs.c_str());
				DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

				JstrErrs.clear();
				return "";
			}

			JsonRoot["Param"] = JsonParam;

			delete pJsonReader;

			str.clear();

			return JsonRoot;
		}
	}

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it, str.clear())
	{
		if (it->second->IsSelected() && it->second->IsInside(ptWinpoint))
		{
			JsonRoot["Type"] = "Station";
			JsonParam["No"] = Json::Value(it->second->GetNo());
			JsonParam["Location"] = Json::Value(it->second->GetLocation());
			JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(it->second->GetName().c_str()));

			str.append(it->second->GetParam().c_str());
			pstr = str.c_str();

			if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
			{
				DebugPrint::Print(JstrErrs.c_str());
				DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

				JstrErrs.clear();
				return "";
			}

			JsonRoot["Param"] = JsonParam;

			delete pJsonReader;

			str.clear();

			return JsonRoot;
		}
	}

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it, str.clear())
	{
		if (it->second->IsSelected() && it->second->IsInside(ptWinpoint))
		{
			JsonRoot["Type"] = "Caller";

			JsonParam["No"] = Json::Value(it->second->GetNo());

			JsonParam["Station"] = Json::Value(it->second->GetStation());

			str.append(it->second->GetParam().c_str());
			pstr = str.c_str();

			if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
			{
				DebugPrint::Print(JstrErrs.c_str());
				DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

				JstrErrs.clear();
				return "";
			}

			JsonRoot["Param"] = JsonParam;

			delete pJsonReader;

			str.clear();

			return JsonRoot;
		}
	}

	delete pJsonReader;

	return "";
}

#ifdef _AFX
const Json::Value CGraphicMap::GetWidget(CPoint ptWinpoint)
{
	return GetWidget(PointF(ptWinpoint.x, ptWinpoint.y));
}

const wstring CGraphicMap::GetWidgetW(CPoint ptWinpoint)
{
	return GetWidgetW(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

const bool CGraphicMap::DeleteWidget(Json::Value& JsonValue)
{
	if (CGraph::EnableEdit() == false)
	{
		return false;
	}

	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonCtrl;

	string strType = Json_ReadString(JsonRoot["Type"]);
	JsonParam = JsonRoot["Param"];

	lock_guard<mutex> lock(m_mutex);

	JsonCtrl["Control"] = Json::Value("Delete");

	// 充电器
	if (strcmp(strType.c_str(), "Charger") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapChargers.find(byNo) != m_mapChargers.end())
		{
			delete m_mapChargers[byNo];
			m_mapChargers.erase(m_mapChargers.find(byNo));
		}

	}
	// 待机位
	else if (strcmp(strType.c_str(), "Rest") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapRests.find(byNo) != m_mapRests.end())
		{
			delete m_mapRests[byNo];
			m_mapRests.erase(m_mapRests.find(byNo));
		}
	}
	// 工位
	else if (strcmp(strType.c_str(), "Station") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapStations.find(byNo) != m_mapStations.end())
		{
			delete m_mapStations[byNo];
			m_mapStations.erase(m_mapStations.find(byNo));
		}
	}
	// AGV
	else if (strcmp(strType.c_str(), "AGV") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapAGVs.find(byNo) != m_mapAGVs.end())
		{
			delete m_mapAGVs[byNo];
			m_mapAGVs.erase(m_mapAGVs.find(byNo));
		}
	}
	// 呼叫器
	else if (strcmp(strType.c_str(), "Caller") == 0)
	{
		unsigned char byNo = Json_ReadInt(JsonParam["No"]);

		if (m_mapCallers.find(byNo) != m_mapCallers.end())
		{
			delete m_mapCallers[byNo];
			m_mapCallers.erase(m_mapCallers.find(byNo));
		}
	}

	JsonCtrl["Param"] = JsonRoot;
	if (m_funcUpdateWidget)
	{
		m_funcUpdateWidget(JsonCtrl, m_pMainFram);
	}

	++m_unNewed;

	strType.clear();
	JsonParam.clear();

	return true;
}

const HCURSOR CGraphicMap::Drag(const PointF ptWinPoint)
{
	HCURSOR hCur = nullptr;
	HCURSOR hResultCur = nullptr;

	if (m_pBK)
	{
		hResultCur = m_pBK->Drag(ptWinPoint);

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}

	PointF pt = CGraph::Correct(CGraph::WinToVec(ptWinPoint));

	if (m_bCreateNew)
	{
		hResultCur = DragTmpWidget(pt);

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}
	else
	{
		hResultCur = DragWidget(ptWinPoint);

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}

	if (hCur)
	{
		m_bRefresh = true;
	}

	return 	hCur;
}

#ifdef _AFX
const HCURSOR CGraphicMap::Drag(const CPoint ptWinPoint)
{
	return Drag(PointF(ptWinPoint.x, ptWinPoint.y));
}
#endif //_AFX

void CGraphicMap::Draw(HDC hdc)
{	
	Graphics gr(hdc);

	Rect rGdi;
	gr.GetVisibleClipBounds(&rGdi);

	if (m_bRefresh == false)
	{
		if (m_pclBmp == nullptr)
		{
			m_pclBmp = new Bitmap(rGdi.Width, rGdi.Height);
		}

		gr.DrawImage(m_pclBmp, rGdi);

		return;
	}

	if (m_pclBmp)
	{
		delete m_pclBmp;
		m_pclBmp = nullptr;	
	}

	m_pclBmp = new Bitmap(rGdi.Width, rGdi.Height);

	Graphics* grPtr = Graphics::FromImage(m_pclBmp);

	if (m_mutex.try_lock() == false)
	{
		return;
	}

	DrawWidget(grPtr);

	DrawTmpWidget(grPtr);

	m_mutex.unlock();

	CGraph::Refresh(false);

	m_bRefresh = false;

	delete grPtr;

	gr.DrawImage(m_pclBmp, rGdi);

	return;
}

void CGraphicMap::OpenEditMode()
{
	for (map<unsigned char, CGraphicAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end(); ++it)
	{
		it->second->IsShow(false);
	}

	CGraph::SetEnableEdit(true);

	m_bRefresh = true;

	return;
}

void CGraphicMap::CloseEditMode()
{
	Cancel();

	ReleaseTmpWidget();

	SaveMap();

	CGraph::SetEnableEdit(false);

	m_bRefresh = true;

	return;
}

void CGraphicMap::Cancel()
{
	m_bRefresh = true;

	if (m_pBK)
	{
		m_pBK->Cancel();
	}

	if (CGraph::EnableEdit() == false)
	{
		return;
	}

	if (m_bCreateNew)
	{
		ReleaseTmpWidget();
		m_bCreateNew = false;
	}
	else
	{
		for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end(); ++it)
		{
			if ((*it)->IsSelected())
			{
				(*it)->Cancel();
			}
		}

		for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end(); ++it)
		{
			if (it->second->IsSelected())
			{
				it->second->Cancel();
			}
		}

		for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it)
		{
			if (it->second->IsSelected())
			{
				it->second->Cancel();
			}
		}

		for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); ++it)
		{
			if (it->second->IsSelected())
			{
				it->second->Cancel();
			}
		}

		for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); ++it)
		{
			if (it->second->IsSelected())
			{
				it->second->Cancel();
			}
		}

		for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it)
		{
			if (it->second->IsSelected())
			{
				it->second->Cancel();
			}
		}
	}

	return;
}

bool CGraphicMap::Init(void* pMainFram, UpdateWidget func)
{
	if (g_bInstall == false)
	{
		if (CGraphicAGV::Install() == false
			|| CGraphicBackground::Install() == false
			|| CGraphicCharger::Install() == false
			|| CGraphicRest::Install() == false
			|| CGraphicRFID::Install() == false
			|| CGraphicStation::Install() == false
			|| CGraphicCaller::Install() == false
			)
		{
			g_bInstall = false;
		}
		else
		{
			g_bInstall = true;
		}
	}

	if (m_bInstall)
	{
		return CGraph::EnableEdit();
	}

	if (m_pMainFram != pMainFram)
	{
		m_pMainFram = pMainFram;
	}

	if (m_funcUpdateWidget != func)
	{
		m_funcUpdateWidget = func;
	}

	m_pBK = new CGraphicBackground();

	InitADO();
	LoadMap();

	//m_bSaveByADO = m_ado.OnInitADOConn();

	LoadBackgroundByADO();

	// 未编辑过界面
	if (m_mapAGVs.size() == 0 || m_mapRFIDs.size() == 0 || m_listTracks.size() == 0 || m_mapStations.size() == 0 || m_mapCallers.size() == 0)
	{
		CGraph::SetEnableEdit(true);
	}

	m_bInstall = true;

	return CGraph::EnableEdit();
}

void CGraphicMap::Select(const PointF ptWinpoint)
{
	if (CGraph::EnableEdit() == false)
	{
		return;
	}

	if (m_bCreateNew)
	{
		SelectTmpWidget(ptWinpoint);
	}
	else
	{
		SelectWidget(ptWinpoint);
	}

	m_bRefresh = true;

	return;
}

#ifdef _AFX
void CGraphicMap::Select(const CPoint ptWinpoint)
{
	return Select(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

void CGraphicMap::SelectBackground(const PointF ptWinpoint)
{
	if (m_pBK)
	{
		m_pBK->Select(ptWinpoint);

		m_bRefresh = true;
	}

	return;
}

#ifdef _AFX
void CGraphicMap::SelectBackground(const CPoint ptWinpoint)
{
	return SelectBackground(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

void CGraphicMap::Confirm(const PointF ptWinpoint)
{
	if (CGraph::EnableEdit() == false)
	{
		return;
	}

	if (m_bCreateNew)
	{
		ConfirmTmpWidget();

		m_bCreateNew = false;
	}
	else
	{
		ConfirmWidget(ptWinpoint);
	}

	m_bRefresh = true;

	return;
}

#ifdef _AFX
void CGraphicMap::Confirm(const CPoint ptWinpoint)
{
	return Confirm(PointF(ptWinpoint.x, ptWinpoint.y));
}
#endif //_AFX

void CGraphicMap::ConfirmBackground()
{
	if (m_pBK)
	{
		m_pBK->Confirm();

		m_bRefresh = true;
	}

	return;
}

void CGraphicMap::DeleteWidget()
{
	if (CGraph::EnableEdit() == false)
	{
		return;
	}

	Json::Value JsonRoot,JsonParam;

	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end();)
	{
		if ((*it)->IsSelected())
		{
			delete (*it);
			it = m_listTracks.erase(it);

			++m_unNewed;
		}
		else
		{
			++it;
		}
	}

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end();)
	{
		if (it->second->IsSelected())
		{
			delete it->second;
			it = m_mapRFIDs.erase(it);

			++m_unNewed;
		}
		else
		{
			++it;
		}
	}

	JsonParam["Type"] = Json::Value("Station");

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); JsonRoot.clear())
	{
		if (it->second->IsSelected())
		{
			JsonParam["Param"]["No"] = Json::Value(it->second->GetNo());
			JsonRoot["Param"] = JsonParam;
			JsonRoot["Control"] = Json::Value("Delete");
			if (m_funcUpdateWidget)
			{
				m_funcUpdateWidget(JsonRoot, m_pMainFram);
			}

			delete it->second;
			it = m_mapStations.erase(it);

			++m_unNewed;
		}
		else
		{
			++it;
		}
	}

	JsonParam["Type"] = Json::Value("Rest");
	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); JsonRoot.clear())
	{
		if (it->second->IsSelected())
		{
			JsonParam["Param"]["No"] = Json::Value(it->second->GetNo());
			JsonRoot["Param"] = JsonParam;
			JsonRoot["Control"] = Json::Value("Delete");
			if (m_funcUpdateWidget)
			{		
				m_funcUpdateWidget(JsonRoot, m_pMainFram);
			}

			delete it->second;
			it = m_mapRests.erase(it);

			++m_unNewed;
		}
		else
		{
			++it;
		}
	}

	JsonParam["Type"] = Json::Value("Charger");
	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); JsonRoot.clear())
	{
		if (it->second->IsSelected())
		{
			JsonParam["Param"]["No"] = Json::Value(it->second->GetNo());
			JsonRoot["Param"] = JsonParam;
			JsonRoot["Control"] = Json::Value("Delete");
			if (m_funcUpdateWidget)
			{
				m_funcUpdateWidget(JsonRoot, m_pMainFram);
			}

			delete it->second;
			it = m_mapChargers.erase(it);

			++m_unNewed;
		}
		else
		{
			++it;
		}
	}

	JsonParam["Type"] = Json::Value("Caller");
	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); JsonRoot.clear())
	{
		if (it->second->IsSelected())
		{
			JsonParam["Param"]["No"] = Json::Value(it->second->GetNo());
			JsonRoot["Param"] = JsonParam;
			JsonRoot["Control"] = Json::Value("Delete");
			if (m_funcUpdateWidget)
			{
				m_funcUpdateWidget(JsonRoot, m_pMainFram);
			}

			delete it->second;
			it = m_mapCallers.erase(it);

			++m_unNewed;
		}
		else
		{
			++it;
		}
	}

	m_bRefresh = true;

	return;
}

void CGraphicMap::Refresh()
{
	m_bRefresh = true;

	return;
}

void CGraphicMap::MultiSelect()
{
	m_bMulti = true;

	return;
}

void CGraphicMap::SignalSelect()
{
	m_bMulti = false;

	return;
}

void CGraphicMap::SelectAll()
{
	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end(); ++it)
	{
		if ((*it)->IsSelected())
		{
			continue;
		}

		(*it)->Select(CGraph::VecToWin((*it)->GetCenterPoint()));
		(*it)->Confirm();
	}

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			continue;
		}

		it->second->Select(CGraph::VecToWin(it->second->GetCenterPoint()));
		it->second->Confirm();
	}

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			continue;
		}

		it->second->Select(CGraph::VecToWin(it->second->GetCenterPoint()));
		it->second->Confirm();
	}

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			continue;
		}

		it->second->Select(CGraph::VecToWin(it->second->GetCenterPoint()));
		it->second->Confirm();
	}

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			continue;
		}

		it->second->Select(CGraph::VecToWin(it->second->GetCenterPoint()));
		it->second->Confirm();
	}

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			continue;
		}

		it->second->Select(CGraph::VecToWin(it->second->GetCenterPoint()));
		it->second->Confirm();
	}

	m_bRefresh = true;

	return;
}

void CGraphicMap::UpdateChargerStatus(Json::Value& JsonValue)
{
	if (CGraph::EnableEdit())
	{
		return;
	}

	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonView;

	unsigned char byNo = Json_ReadInt(JsonRoot["No"]);

	if (m_mapChargers.find(byNo) == m_mapChargers.end())
	{
		return;
	}

	// VIEW
	JsonView = JsonRoot["View"];

	m_mapChargers[byNo]->SetPower(Json_ReadBool(JsonView["Power"].asBool()), Json_ReadULL(JsonView["ChargeTime"]));

	JsonView.clear();

	return;
}

void CGraphicMap::ReleaseTmpWidget()
{
	lock_guard<mutex> lock(m_mutex);

	if (m_pTmpCharger)
	{
		delete m_pTmpCharger;
		m_pTmpCharger = nullptr;
	}

	if (m_pTmpRest)
	{
		delete m_pTmpRest;
		m_pTmpRest = nullptr;
	}

	if (m_pTmpRFID)
	{
		delete m_pTmpRFID;
		m_pTmpRFID = nullptr;
	}

	if (m_pTmpStation)
	{
		delete m_pTmpStation;
		m_pTmpStation = nullptr;
	}

	if (m_pTmpTrack)
	{
		delete m_pTmpTrack;
		m_pTmpTrack = nullptr;
	}

	if (m_pTmpCaller)
	{
		delete m_pTmpCaller;
		m_pTmpCaller = nullptr;
	}

	return;
}

void CGraphicMap::UpdateAGVStatus(Json::Value& JsonValue)
{
	if (CGraph::EnableEdit())
	{
		return;
	}

	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonRFID, JsonView;

	unsigned char byNo = Json_ReadInt(JsonRoot["No"]);

	if (m_mapAGVs.find(byNo) == m_mapAGVs.end())
	{
		return;
	}

	bool bConnected = Json_ReadBool(JsonRoot["Connect"].asBool());

	m_mapAGVs[byNo]->IsShow(false);

	if (bConnected == false)
	{
		return;
	}

	JsonRFID = JsonRoot["RFID"];
	
	// RFID
	unsigned short usCurRFUD = Json_ReadInt(JsonRFID["Cur"]);
	unsigned short usOldRFID = Json_ReadInt(JsonRFID["Old"]);

	if (m_mapRFIDs.find(usOldRFID) != m_mapRFIDs.end())
	{
		ReleaseRFID(m_mapAGVs[byNo], usOldRFID);
	}

	if (m_mapRFIDs.find(usCurRFUD) == m_mapRFIDs.end())
	{
		m_mapAGVs[byNo]->IsShow(false);
		return;
	}

	BindRFID(m_mapAGVs[byNo], usCurRFUD);

	// VIEW
	JsonView = JsonRoot["View"];

	m_mapAGVs[byNo]->SetRunStatus(Json_ReadBool(JsonView["Run"].asBool()));
	m_mapAGVs[byNo]->SetObsStatus(Json_ReadBool(JsonView["Obs"].asBool()));
	m_mapAGVs[byNo]->SetUnVolStatus(Json_ReadBool(JsonView["Unvol"].asBool()));
	m_mapAGVs[byNo]->SetCargoStatus(Json_ReadBool(JsonView["Cargo"].asBool()));
	m_mapAGVs[byNo]->SetErrorStatus(Json_ReadBool(JsonView["Error"].asBool()));
	m_mapAGVs[byNo]->SetActionStatus(Json_ReadInt(JsonView["Action"]));

	JsonRFID.clear();
	JsonView.clear();

	return;
}

void CGraphicMap::UpdateCallerStatus(Json::Value& JsonValue)
{
	if (CGraph::EnableEdit())
	{
		return;
	}

	Json::Value& JsonRoot = JsonValue;

	unsigned char byNo = Json_ReadInt(JsonRoot["No"]);

	if (m_mapCallers.find(byNo) == m_mapCallers.end())
	{
		return;
	}

	m_mapCallers[byNo]->SetCallSignal(Json_ReadInt(JsonRoot["Signal"]["Call"]));
	m_mapCallers[byNo]->SetCancelCallSignal(Json_ReadInt(JsonRoot["Signal"]["CancelCall"]));
	m_mapCallers[byNo]->SetPassSignal(Json_ReadInt(JsonRoot["Signal"]["Pass"]));
	m_mapCallers[byNo]->SetCancelPassSignal(Json_ReadInt(JsonRoot["Signal"]["CancelPass"]));
	m_mapCallers[byNo]->SetNGSignal(Json_ReadInt(JsonRoot["Signal"]["NG"]));
	m_mapCallers[byNo]->SetCancelNGSignal(Json_ReadInt(JsonRoot["Signal"]["CancelNG"]));
	m_mapCallers[byNo]->SetScreamSignal(Json_ReadInt(JsonRoot["Signal"]["Scream"]));
	m_mapCallers[byNo]->SetPackOutboundSignal(JsonRoot["PackOutbound"].asBool());

	return;
}

void CGraphicMap::JsonStringToADO(const char* strJson)
{
	m_strADO = strJson;

	return;
}

void CGraphicMap::JsonStringToADO(const wchar_t* wstrJson)
{
	m_strADO = StringEx::wstring_to_string(wstrJson);

	return;
}

void CGraphicMap::JsonFileToADO(const char* strFile)
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

void CGraphicMap::JsonFileToADO(const wchar_t* wstrFile)
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

void CGraphicMap::SaveMap()
{
	if (m_unNewed == 0)
	{
		return;
	}

	if (SystemLog::IsFileExisted("../File/Map.json") == false)
	{
		SystemLog::CreateFileEx("../File/Map.json");
	}

	lock_guard<mutex> lock(m_mutex);

	Json::Value JsonRoot;
	Json::Value JsonAGVArr, JsonRFIDArr, JsonStationArr, JsonTrackArr, JsonCallerArr, JsonChargerArr, JsonRestArr;

	SaveAGVToJsonString(JsonAGVArr);
	SaveRFIDToJsonString(JsonRFIDArr);
	SaveStationToJsonString(JsonStationArr);
	SaveTrackToJsonString(JsonTrackArr);
	SaveCallerToJsonString(JsonCallerArr);
	SaveChargerToJsonString(JsonChargerArr);
	SaveRestToJsonString(JsonRestArr);

	JsonRoot["AGV"] = JsonAGVArr;
	JsonRoot["RFID"] = JsonRFIDArr;
	JsonRoot["Station"] = JsonStationArr;
	JsonRoot["Track"] = JsonTrackArr;
	JsonRoot["Caller"] = JsonCallerArr;
	JsonRoot["Charger"] = JsonChargerArr;
	JsonRoot["Rest"] = JsonRestArr;

	// 生成Json文件储存
	ofstream fout("../File/Map.json");
	if (fout)
	{
		string strContext;
		strContext = JsonRoot.toStyledString();
		fout << strContext;
		fout.close();
	}

	m_unNewed = 0;

	return;
}

void CGraphicMap::LoadMap()
{
	ifstream fin;
	fin.open("../File/Map.json");
	if (!fin)
	{
		//TCHAR("error");
		return;
	}
	ostringstream ostring;
	ostring << fin.rdbuf();
	fin.close();

	string strJsonMap = ostring.str();

	/*
	 {
	 "AGV":[{"No":0,"Param":{}},...],
	 "RFID":[{"No":0,"CenterX":0,"CenterY":0},...],
	 "Track":[{"Arc":true,"CenterX":0,"CenterY":0,"DistanceX":0,"DistanceY":0},...]
	 "Station":[{"No":0,"Location":0,"Name":"","CenterX":0,"CenterY":0,"Param":{"Process":0}},...]
	 "Caller":[{"No":0,"Station":0,"CenterX":0,"CenterY":0,"Param":{}},...]
	 "Charger":[{"No":0,"Name":"","Location":0,"Controler":0,"CenterX":0,"CenterY":0,"Param":0},...]
	 "Rest":[{"No":0,"Name":"","Location":0,"CenterX":0,"CenterY":0,"Param":0},...]
	 }
	*/
	Json::Value JsonRoot;

	const char* pstr = strJsonMap.c_str();

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	JsonStringToAGV(JsonRoot["AGV"].toStyledString().c_str());
	JsonStringToRFID(JsonRoot["RFID"].toStyledString().c_str());
	JsonStringToStation(JsonRoot["Station"].toStyledString().c_str());
	JsonStringToTrack(JsonRoot["Track"].toStyledString().c_str());
	JsonStringToCaller(JsonRoot["Caller"].toStyledString().c_str());
	JsonStringToCharger(JsonRoot["Charger"].toStyledString().c_str());
	JsonStringToRest(JsonRoot["Rest"].toStyledString().c_str());

	return;
}

void CGraphicMap::BindRFID(CGraphicAGV* pAGV, const unsigned short usRFID)
{
	if (pAGV == nullptr)
	{
		return;
	}

	if (m_mapRFIDs.find(usRFID) == m_mapRFIDs.end())
	{
		pAGV->IsShow(false);

		return;
	}

	pAGV->IsShow(true);

	PointF pt = m_mapRFIDs[usRFID]->GetCenterPoint();

	pAGV->SetCenterPoint(pt);

	return;
}

void CGraphicMap::ReleaseRFID(CGraphicAGV* pAGV, const unsigned short usRFID)
{
	if (pAGV == nullptr)
	{
		return;
	}

	if (m_mapRFIDs.find(usRFID) == m_mapRFIDs.end())
	{
		return;
	}

	return;
}

void CGraphicMap::DrawWidget(Graphics* grPtr)
{
	if (m_pBK)
	{
		m_pBK->Draw(grPtr);
	}

	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end(); ++it)
	{
		(*it)->Draw(grPtr);
	}

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end(); ++it)
	{
		it->second->Draw(grPtr);
	}

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it)
	{
		it->second->Draw(grPtr);
	}

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); ++it)
	{
		it->second->Draw(grPtr);
	}

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); ++it)
	{
		it->second->Draw(grPtr);
	}

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it)
	{
		it->second->Draw(grPtr);
	}

	for (map<unsigned char, CGraphicAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end(); ++it)
	{
		it->second->Draw(grPtr);
	}

	return;
}

void CGraphicMap::DrawTmpWidget(Graphics* grPtr)
{
	if (m_pTmpCharger)
	{
		m_pTmpCharger->Draw(grPtr);
	}

	if (m_pTmpRest)
	{
		m_pTmpRest->Draw(grPtr);
	}

	if (m_pTmpStation)
	{
		m_pTmpStation->Draw(grPtr);
	}

	if (m_pTmpCaller)
	{
		m_pTmpCaller->Draw(grPtr);
	}

	if (m_pTmpRFID)
	{
		m_pTmpRFID->Draw(grPtr);
	}

	if (m_pTmpTrack)
	{
		m_pTmpTrack->Draw(grPtr);
	}

	return;
}

void CGraphicMap::InitADO()
{
	Json::Value JsonRoot;

	const char* pstr = (char*)m_strADO.c_str();

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("初始化ADO失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	m_ado.m_bstrConnect = Json_ReadString(JsonRoot["Connect"]).c_str();
	m_ado.m_bstrUser = Json_ReadString(JsonRoot["User"]).c_str();
	m_ado.m_bstrPassword = Json_ReadString(JsonRoot["Password"]).c_str();

	JsonRoot.clear();

	return;
}

void CGraphicMap::LoadBackgroundByADO()
{
	string strSQL = "select * from AGV_SYSTEM_CONFIG where [system_name] in ('scale','offset_x','offset_y')";

	_RecordsetPtr pRecordset = nullptr;
	if (m_ado.GetRecordSet(strSQL.c_str(), pRecordset) == false)
	{
		return;
	}

	if (pRecordset->adoEOF)
	{
		//m_ado.Release();

		return;
	}

	int nOffsetX = 0;
	int nOffsetY = 0;
	float fScale = 0.0f;
	string strName = "";

	_variant_t var;				/*!< 字段值 */

	for (; !pRecordset->adoEOF; pRecordset->MoveNext(), strName.clear())
	{
		var = pRecordset->GetCollect(_bstr_t("system_name"));
		if (var.vt != VT_NULL)
		{
			strName.append((char*)_bstr_t(var));
		}

		var = pRecordset->GetCollect(_bstr_t("system_value"));

		if (strcmp(strName.c_str(), "scale") == 0)
		{
			if (var.vt != VT_NULL)
			{
				fScale = atof((char*)_bstr_t(var));
			}
		}
		else if (strcmp(strName.c_str(), "offset_x") == 0)
		{
			if (var.vt != VT_NULL)
			{
				nOffsetX = atoi((char*)_bstr_t(var));
			}
		}
		else if (strcmp(strName.c_str(), "offset_y") == 0)
		{
			if (var.vt != VT_NULL)
			{
				nOffsetY = atoi((char*)_bstr_t(var));
			}
		}
	}

	//m_ado.Release();

	CGraph::SetOffset(PointToPointF(Point(nOffsetX, nOffsetY)));
	CGraph::SetScale(fScale);

	return;
}

void CGraphicMap::SaveBackgroundByADO()
{
	string strSQL = "";

	float fScale = CGraph::GetScale();
	Point pt = PointFToPoint(CGraph::GetOffset());

	strSQL.append(StringEx::string_format(
		"if not exists("\
		"select [system_name] from AGV_SYSTEM_CONFIG where [system_name]='scale'"\
		")"\
		" insert into AGV_SYSTEM_CONFIG([system_name],[system_value]) values('scale','%f')"\
		" else "\
		"update AGV_SYSTEM_CONFIG set [system_value]='%f' where [system_name]='scale';"
		, fScale, fScale)
	);

	strSQL.append(StringEx::string_format(
		"if not exists("\
		"select [system_name] from AGV_SYSTEM_CONFIG where [system_name]='offset_x'"\
		")"\
		" insert into AGV_SYSTEM_CONFIG([system_name],[system_value]) values('offset_x','%d')"\
		" else "\
		"update AGV_SYSTEM_CONFIG set [system_value]='%d' where [system_name]='offset_x';"
		, pt.X, pt.X)
	);

	strSQL.append(StringEx::string_format(
		"if not exists("\
		"select [system_name] from AGV_SYSTEM_CONFIG where [system_name]='offset_y'"\
		")"\
		" insert into AGV_SYSTEM_CONFIG([system_name],[system_value]) values('offset_y','%d')"\
		" else "\
		"update AGV_SYSTEM_CONFIG set [system_value]='%d' where [system_name]='offset_y';"
		, pt.Y, pt.Y)
	);

	ULL ullStart = GetTickCountEX();

	_variant_t rsv;
	if (m_ado.ExecuteSQL(strSQL.c_str(), rsv) && rsv.vt != VT_NULL && (int)rsv > 0)
	{
		DebugPrint::Printf("执行%s\n消耗时间;%dms", strSQL.c_str(), GetTickCountEX() - ullStart);
	}

	return;
}

void CGraphicMap::JsonStringToAGV(const char* strJson)
{
	/* [{"No":0,"Param":{}},...] */
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取AGV图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		string strParam = JsonRoot[(int)k]["Param"].toStyledString();

		if (m_mapAGVs.find(byNo) == m_mapAGVs.end())
		{
			m_mapAGVs[byNo] = new CGraphicAGV(byNo, strParam);
		}
		else
		{
			m_mapAGVs[byNo]->SetParam(strParam.c_str());
		}

		Json::Value JsonCtrl,JsonParam;
		JsonParam["Type"] = Json::Value("AGV");
		JsonParam["Param"] = JsonRoot[(int)k];
		JsonCtrl["Control"] = Json::Value("Add");
		JsonCtrl["Param"] = JsonParam;

		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}
	}

	return;
}

void CGraphicMap::JsonStringToRFID(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取RFID图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		unsigned short usNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		Point ptCenter;

		ptCenter.X = Json_ReadInt(JsonRoot[(int)k]["CenterX"]);
		ptCenter.Y = Json_ReadInt(JsonRoot[(int)k]["CenterY"]);

		if (m_mapRFIDs.find(usNo) == m_mapRFIDs.end())
		{
			m_mapRFIDs[usNo] = new CGraphicRFID(usNo, PointToPointF(ptCenter));
		}
		else
		{
			m_mapRFIDs[usNo]->SetCenterPoint(PointToPointF(ptCenter));
		}
	}

	return;
}

void CGraphicMap::JsonStringToStation(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取工位图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		unsigned short usLocation = Json_ReadInt(JsonRoot[(int)k]["Location"]);
		string strName = StringEx::Utf8ToAcsii(Json_ReadString(JsonRoot[(int)k]["Name"]).c_str());
		string strParam = JsonRoot[(int)k]["Param"].toStyledString();
		Point ptCenter;

		ptCenter.X = Json_ReadInt(JsonRoot[(int)k]["CenterX"]);
		ptCenter.Y = Json_ReadInt(JsonRoot[(int)k]["CenterY"]);

		if (m_mapStations.find(byNo) == m_mapStations.end())
		{
			m_mapStations[byNo] = new CGraphicStation(byNo, usLocation, strName, strParam);
		}
		else
		{
			m_mapStations[byNo]->SetLocation(usLocation);
			m_mapStations[byNo]->SetName(strName.c_str());
			m_mapStations[byNo]->SetParam(strParam.c_str());
		}

		m_mapStations[byNo]->SetCenterPoint(PointToPointF(ptCenter));

		Json::Value JsonCtrl, JsonParam;
		JsonParam["Type"] = Json::Value("Station");
		JsonParam["Param"] = JsonRoot[(int)k];
		JsonCtrl["Control"] = Json::Value("Add");
		JsonCtrl["Param"] = JsonParam;

		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}
	}

	return;
}

void CGraphicMap::JsonStringToTrack(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取磁轨图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		bool Arc = Json_ReadBool(JsonRoot[(int)k]["Arc"].asBool());
		Point ptCenter,ptDistance;

		ptCenter.X = Json_ReadInt(JsonRoot[(int)k]["CenterX"]);
		ptCenter.Y = Json_ReadInt(JsonRoot[(int)k]["CenterY"]);
		ptDistance.X = Json_ReadInt(JsonRoot[(int)k]["DistanceX"]);
		ptDistance.Y = Json_ReadInt(JsonRoot[(int)k]["DistanceY"]);

		m_listTracks.push_back(new CGraphicTrack(Arc, PointToPointF(ptCenter), PointToPointF(ptCenter - ptDistance)));
	}

	return;
}

void CGraphicMap::JsonStringToCaller(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取呼叫器图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		unsigned char byStation = Json_ReadInt(JsonRoot[(int)k]["Station"]);
		string strParam = JsonRoot[(int)k]["Param"].toStyledString();
		Point ptCenter;

		ptCenter.X = Json_ReadInt(JsonRoot[(int)k]["CenterX"]);
		ptCenter.Y = Json_ReadInt(JsonRoot[(int)k]["CenterY"]);

		if (m_mapCallers.find(byNo) == m_mapCallers.end())
		{
			m_mapCallers[byNo] = new CGraphicCaller(byNo, byStation, strParam);
		}
		else
		{
			m_mapCallers[byNo]->BindStation(byStation);
			m_mapCallers[byNo]->SetParam(strParam.c_str());
		}

		m_mapCallers[byNo]->SetCenterPoint(PointToPointF(ptCenter));

		Json::Value JsonCtrl, JsonParam;
		JsonParam["Type"] = Json::Value("Caller");
		JsonParam["Param"] = JsonRoot[(int)k];
		JsonCtrl["Control"] = Json::Value("Add");
		JsonCtrl["Param"] = JsonParam;

		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}
	}

	return;
}

void CGraphicMap::ReleaseWidget()
{
	lock_guard<mutex> lock(m_mutex);

	if (m_pBK)
	{
		delete m_pBK;
		m_pBK = nullptr;
	}

	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end(); it = m_listTracks.erase(it))
	{
		delete *it;
	}

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end(); it = m_mapRFIDs.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); it = m_mapStations.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); it = m_mapRests.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); it = m_mapChargers.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, CGraphicAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end(); it = m_mapAGVs.erase(it))
	{
		delete it->second;
	}

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); it = m_mapCallers.erase(it))
	{
		delete it->second;
	}

	return;
}

const HCURSOR CGraphicMap::DragTmpWidget(const PointF ptWinPoint)
{
	if (CGraph::EnableEdit() == false)
	{
		return nullptr;
	}

	HCURSOR hCur = nullptr;

	if (m_pTmpCharger)
	{
		m_pTmpCharger->SetCenterPoint(ptWinPoint);

		hCur = LoadCursor(NULL, IDC_HAND);
	}

	if (m_pTmpRest)
	{
		m_pTmpRest->SetCenterPoint(ptWinPoint);

		hCur = LoadCursor(NULL, IDC_HAND);
	}

	if (m_pTmpRFID)
	{
		m_pTmpRFID->SetCenterPoint(ptWinPoint);

		hCur = LoadCursor(NULL, IDC_HAND);
	}

	if (m_pTmpStation)
	{
		m_pTmpStation->SetCenterPoint(ptWinPoint);

		hCur = LoadCursor(NULL, IDC_HAND);
	}

	if (m_pTmpTrack)
	{
		m_pTmpTrack->SetEndPoint(ptWinPoint);

		hCur = LoadCursor(NULL, IDC_HAND);
	}

	if (m_pTmpCaller)
	{
		m_pTmpCaller->SetCenterPoint(ptWinPoint);

		hCur = LoadCursor(NULL, IDC_HAND);
	}

	return hCur;
}

const HCURSOR CGraphicMap::DragWidget(const PointF ptWinPoint)
{
	if (CGraph::EnableEdit() == false)
	{
		return nullptr;
	}

	HCURSOR hCur = nullptr;
	HCURSOR hResultCur = nullptr;

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			hResultCur = it->second->Drag(ptWinPoint);
		}

		if(hResultCur)
		{
			hCur = hResultCur;
		}
	}

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			hResultCur = it->second->Drag(ptWinPoint);
		}

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			hResultCur = it->second->Drag(ptWinPoint);
		}

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			hResultCur = it->second->Drag(ptWinPoint);
		}

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			hResultCur = it->second->Drag(ptWinPoint);
		}

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}

	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end(); ++it)
	{
		if ((*it)->IsSelected())
		{
			hResultCur = (*it)->Drag(ptWinPoint);
		}

		if (hResultCur)
		{
			hCur = hResultCur;
		}
	}

	return hCur;
}

void CGraphicMap::ConfirmTmpWidget()
{
	lock_guard<mutex> lock(m_mutex);

	Json::Value JsonCtrl, JsonRoot, JsonParam;

	JsonCtrl["Control"] = Json::Value("Add");

	Json::CharReaderBuilder JsonBuilder;
	Json::CharReader* pJsonReader = JsonBuilder.newCharReader();
	JSONCPP_STRING JstrErrs;

	string str = "";

	const char* pstr = nullptr;

	if (m_pTmpCharger)
	{
		m_mapChargers[m_pTmpCharger->GetNo()] = m_pTmpCharger;

		++m_unNewed;

		JsonParam["No"] = Json::Value(m_pTmpCharger->GetNo());
		JsonParam["Location"] = Json::Value(m_pTmpCharger->GetLocation());
		JsonParam["Ctrl"] = Json::Value(m_pTmpCharger->GetCtrler());
		JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(m_pTmpCharger->GetName().c_str()));
		str.append(m_pTmpCharger->GetParam().c_str());
		pstr = str.c_str();
		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return;
		}
		str.clear();

		JsonRoot["Type"] = Json::Value("Charger");
		JsonRoot["Param"] = JsonParam;
		JsonCtrl["Param"] = JsonRoot;
		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}

		//delete m_pTmpCharger;
		m_pTmpCharger = nullptr;
	}

	if (m_pTmpRest)
	{
		m_mapRests[m_pTmpRest->GetNo()] = m_pTmpRest;

		++m_unNewed;

		JsonParam["No"] = Json::Value(m_pTmpRest->GetNo());
		JsonParam["Location"] = Json::Value(m_pTmpRest->GetLocation());
		JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(m_pTmpRest->GetName().c_str()));

		str.append(m_pTmpRest->GetParam().c_str());
		pstr = str.c_str();
		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return ;
		}
		str.clear();

		JsonRoot["Type"] = Json::Value("Rest");
		JsonRoot["Param"] = JsonParam;
		JsonCtrl["Param"] = JsonRoot;
		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}

		//delete m_pTmpRest;
		m_pTmpRest = nullptr;
	}

	if (m_pTmpRFID)
	{
		m_mapRFIDs[m_pTmpRFID->GetNo()] = m_pTmpRFID;

		++m_unNewed;

		//delete m_pTmpRFID;
		m_pTmpRFID = nullptr;
	}

	if (m_pTmpStation)
	{
		m_mapStations[m_pTmpStation->GetNo()] = m_pTmpStation;

		++m_unNewed;

		JsonParam["No"] = Json::Value(m_pTmpStation->GetNo());
		JsonParam["Location"] = Json::Value(m_pTmpStation->GetLocation());
		JsonParam["Name"] = Json::Value(StringEx::AcsiiToUtf8(m_pTmpStation->GetName().c_str()));
		str.append(m_pTmpStation->GetParam().c_str());
		pstr = str.c_str();
		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return;
		}
		str.clear();

		JsonRoot["Type"] = Json::Value("Station");
		JsonRoot["Param"] = JsonParam;
		JsonCtrl["Param"] = JsonRoot;
		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}

		//delete m_pTmpStation;
		m_pTmpStation = nullptr;
	}

	if (m_pTmpCaller)
	{
		m_mapCallers[m_pTmpCaller->GetNo()] = m_pTmpCaller;

		++m_unNewed;

		JsonParam["No"] = Json::Value(m_pTmpCaller->GetNo());
		JsonParam["Station"] = Json::Value(m_pTmpCaller->GetStation());

		str.append(m_pTmpCaller->GetParam().c_str());
		pstr = str.c_str();
		if (!pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonParam["Param"], &JstrErrs))
		{
			DebugPrint::Print(JstrErrs.c_str());
			DebugPrint::Print("查找图标失败:\n非JSON类型字符串");

			JstrErrs.clear();
			return;
		}
		str.clear();

		JsonRoot["Type"] = Json::Value("Caller");
		JsonRoot["Param"] = JsonParam;
		JsonCtrl["Param"] = JsonRoot;
		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}

		//delete m_pTmpCaller;
		m_pTmpCaller = nullptr;
	}

	if (m_pTmpTrack)
	{
		PointF pt = m_pTmpTrack->GetDistancePoint();
		if (pt.Equals(PointF(0,0)))
		{
			delete m_pTmpTrack;
			m_pTmpTrack = nullptr;

			return;
		}

		m_listTracks.push_back(m_pTmpTrack);

		++m_unNewed;

		//delete m_pTmpTrack;
		m_pTmpTrack = nullptr;
	}

	delete pJsonReader;

	return;
}

void CGraphicMap::ConfirmWidget(const PointF ptWinpoint)
{
	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end(); ++it)
	{
		if ((*it)->IsSelected())
		{
			if ((*it)->IsNewed())
			{
				++m_unNewed;
			}

			if (m_bMulti && (*it)->IsMoveed() == false && (*it)->IsInside(ptWinpoint))
			{
				(*it)->Cancel();
			}

			(*it)->Confirm();
		}
	}

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			if (it->second->IsNewed())
			{
				++m_unNewed;
			}

			if (m_bMulti && it->second->IsMoveed() == false && it->second->IsInside(ptWinpoint))
			{
				it->second->Cancel();
			}

			it->second->Confirm();
		}
	}

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			if (it->second->IsNewed())
			{
				++m_unNewed;
			}

			if (m_bMulti && it->second->IsMoveed() == false && it->second->IsInside(ptWinpoint))
			{
				it->second->Cancel();
			}

			it->second->Confirm();
		}
	}

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			if (it->second->IsNewed())
			{
				++m_unNewed;
			}

			if (m_bMulti && it->second->IsMoveed() == false && it->second->IsInside(ptWinpoint))
			{
				it->second->Cancel();
			}

			it->second->Confirm();
		}
	}

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			if (it->second->IsNewed())
			{
				++m_unNewed;
			}

			if (m_bMulti && it->second->IsMoveed() == false && it->second->IsInside(ptWinpoint))
			{
				it->second->Cancel();
			}

			it->second->Confirm();
		}
	}

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it)
	{
		if (it->second->IsSelected())
		{
			if (it->second->IsNewed())
			{
				++m_unNewed;
			}

			if (m_bMulti && it->second->IsMoveed() == false && it->second->IsInside(ptWinpoint))
			{
				it->second->Cancel();
			}

			it->second->Confirm();	
		}
	}

	return;
}

void CGraphicMap::SelectTmpWidget(const PointF ptWinpoint)
{
	PointF pt = CGraph::Correct(CGraph::WinToVec(ptWinpoint));

	if (m_pTmpTrack)
	{
		m_pTmpTrack->SetCenterPoint(pt);
	}

	return;
}

void CGraphicMap::SelectWidget(const PointF ptWinpoint)
{
	/*!
	* 分为单选与多选
	*/
	if (!m_bMulti)
	{
		// 取消全部选中
		Cancel();
	}

	// 满足该坐标的所有控件
	list<CGraphicWidget*> listWidget;
	bool bSelected = false;

	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end(); ++it, bSelected = false)
	{
		bSelected = (*it)->IsSelected();
		if ((*it)->IsInside(ptWinpoint))
		{
			listWidget.push_front((*it));
		}
		// 多选 且 已被选中
		else if (m_bMulti && bSelected)
		{
			// 保持选中
			(*it)->Select(ptWinpoint);
		}
	}

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end(); ++it, bSelected = false)
	{
		bSelected = it->second->IsSelected();
		if (it->second->IsInside(ptWinpoint))
		{
			listWidget.push_front(it->second);
		}
		// 多选 且 已被选中
		else if (m_bMulti && bSelected)
		{
			// 保持选中
			it->second->Select(ptWinpoint);
		}
	}

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end(); ++it, bSelected = false)
	{
		bSelected = it->second->IsSelected();
		if (it->second->IsInside(ptWinpoint))
		{
			listWidget.push_front(it->second);
		}
		// 多选 且 已被选中
		else if (m_bMulti && bSelected)
		{
			// 保持选中
			it->second->Select(ptWinpoint);
		}
	}

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end(); ++it, bSelected = false)
	{
		bSelected = it->second->IsSelected();
		if (it->second->IsInside(ptWinpoint))
		{
			listWidget.push_front(it->second);
		}
		// 多选 且 已被选中
		else if (m_bMulti && bSelected)
		{
			// 保持选中
			it->second->Select(ptWinpoint);
		}
	}

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end(); ++it, bSelected = false)
	{
		bSelected = it->second->IsSelected();
		if (it->second->IsInside(ptWinpoint))
		{
			listWidget.push_front(it->second);
		}
		// 多选 且 已被选中
		else if (m_bMulti && bSelected)
		{
			// 保持选中
			it->second->Select(ptWinpoint);
		}
	}

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end(); ++it, bSelected = false)
	{
		bSelected = it->second->IsSelected();
		if (it->second->IsInside(ptWinpoint))
		{
			listWidget.push_front(it->second);
		}
		// 多选 且 已被选中
		else if (m_bMulti && bSelected)
		{
			// 保持选中
			it->second->Select(ptWinpoint);
		}
	}

	if (listWidget.size() > 0)
	{
		listWidget.front()->Select(ptWinpoint);
	}
	
	return;
}

void CGraphicMap::ZoomedIn()
{
	if (m_pBK)
	{
		m_pBK->Scale();
	}

	m_bRefresh = true;

	return;
}

void CGraphicMap::ZoomedOut()
{
	if (m_pBK)
	{
		m_pBK->Scale(true);
	}

	m_bRefresh = true;

	return;
}

void CGraphicMap::Reset()
{
	if (m_pBK)
	{
		m_pBK->Reset();
	}

	m_bRefresh = true;

	return;
}

void CGraphicMap::OnSize(RectF rcClient)
{
	CGraph::SetClientRect(rcClient);

	m_bRefresh = true;

	return;
}

#ifdef _AFX
void CGraphicMap::OnSize(CRect rcClient)
{
	CGraph::SetClientRect(rcClient);

	m_bRefresh = true;

	return;
}
#endif //_AFX

void CGraphicMap::JsonStringToCharger(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取充电器图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		unsigned short usLocation = Json_ReadInt(JsonRoot[(int)k]["Location"]);
		unsigned char byCtrler = Json_ReadInt(JsonRoot[(int)k]["Controler"]);
		string strName = StringEx::Utf8ToAcsii(Json_ReadString(JsonRoot[(int)k]["Name"]).c_str());
		string strParam = JsonRoot[(int)k]["Param"].toStyledString();
		Point ptCenter;

		ptCenter.X = Json_ReadInt(JsonRoot[(int)k]["CenterX"]);
		ptCenter.Y = Json_ReadInt(JsonRoot[(int)k]["CenterY"]);

		if (m_mapChargers.find(byNo) == m_mapChargers.end())
		{
			m_mapChargers[byNo] = new CGraphicCharger(byNo, usLocation, byCtrler, strName, strParam);
		}
		else
		{
			m_mapChargers[byNo]->SetCtrler(byCtrler);
			m_mapChargers[byNo]->SetLocation(usLocation);
			m_mapChargers[byNo]->SetName(strName.c_str());
			m_mapChargers[byNo]->SetParam(strParam.c_str());
		}

		m_mapRests[byNo]->SetCenterPoint(PointToPointF(ptCenter));

		Json::Value JsonCtrl, JsonParam;
		JsonParam["Type"] = Json::Value("Charger");
		JsonParam["Param"] = JsonRoot[(int)k];
		JsonCtrl["Control"] = Json::Value("Add");
		JsonCtrl["Param"] = JsonParam;

		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}
	}

	return;
}

void CGraphicMap::JsonStringToRest(const char* strJson)
{
	Json::Value JsonRoot;

	const char* pstr = strJson;

	if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &m_JstrErrs))
	{
		DebugPrint::Print(m_JstrErrs.c_str());
		DebugPrint::Print("读取待机位图标失败:\n非JSON类型字符串");

		m_JstrErrs.clear();
		return;
	}

	for (size_t k = 0; k < JsonRoot.size(); ++k)
	{
		unsigned char byNo = Json_ReadInt(JsonRoot[(int)k]["No"]);
		unsigned short usLocation = Json_ReadInt(JsonRoot[(int)k]["Location"]);
		string strName = StringEx::Utf8ToAcsii(Json_ReadString(JsonRoot[(int)k]["Name"]).c_str());
		string strParam = JsonRoot[(int)k]["Param"].toStyledString();
		Point ptCenter;

		ptCenter.X = Json_ReadInt(JsonRoot[(int)k]["CenterX"]);
		ptCenter.Y = Json_ReadInt(JsonRoot[(int)k]["CenterY"]);

		if (m_mapRests.find(byNo) == m_mapRests.end())
		{
			m_mapRests[byNo] = new CGraphicRest(byNo, usLocation, strName, strParam);
		}
		else
		{
			m_mapRests[byNo]->SetLocation(usLocation);
			m_mapRests[byNo]->SetName(strName.c_str());
			m_mapRests[byNo]->SetParam(strParam.c_str());
		}

		m_mapRests[byNo]->SetCenterPoint(PointToPointF(ptCenter));

		Json::Value JsonCtrl, JsonParam;
		JsonParam["Type"] = Json::Value("Rest");
		JsonParam["Param"] = JsonRoot[(int)k];
		JsonCtrl["Control"] = Json::Value("Add");
		JsonCtrl["Param"] = JsonParam;

		if (m_funcUpdateWidget)
		{
			m_funcUpdateWidget(JsonCtrl, m_pMainFram);
		}
	}

	return;
}

void CGraphicMap::SaveAGVToJsonString(Json::Value& json)
{
	Json::Value arr;
	string str = "";
	const char* pstr = nullptr;

	for (map<unsigned char, CGraphicAGV*>::iterator it = m_mapAGVs.begin(); it != m_mapAGVs.end();
		++it, arr.clear(), str.clear(), pstr = nullptr)
	{
		arr["No"] = Json::Value(it->second->GetNo());

		str.append(it->second->GetParam().c_str());
		pstr = str.c_str();

		if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &arr["Param"], &m_JstrErrs))
		{
			DebugPrint::Print(m_JstrErrs.c_str());
			DebugPrint::Print("读取AGV图标失败:\n非JSON类型字符串");

			m_JstrErrs.clear();
			return;
		}

		json.append(arr);
	}

	return;
}

void CGraphicMap::SaveRFIDToJsonString(Json::Value& json)
{
	Json::Value arr;
	Point ptCenter = Point(0, 0);

	for (map<unsigned short, CGraphicRFID*>::iterator it = m_mapRFIDs.begin(); it != m_mapRFIDs.end();
		++it, ptCenter = Point(0, 0), arr.clear())
	{
		arr["No"] = Json::Value(it->second->GetNo());
		ptCenter = PointFToPoint(it->second->GetCenterPoint());

		arr["CenterX"] = Json::Value(ptCenter.X);
		arr["CenterY"] = Json::Value(ptCenter.Y);

		json.append(arr);
	}

	return;
}

void CGraphicMap::SaveStationToJsonString(Json::Value& json)
{
	Json::Value arr;
	Point ptCenter = Point(0, 0);
	string str = "";
	const char* pstr = nullptr;

	for (map<unsigned char, CGraphicStation*>::iterator it = m_mapStations.begin(); it != m_mapStations.end();
		++it, ptCenter = Point(0, 0), arr.clear(), str.clear(), pstr = nullptr)
	{
		arr["No"] = Json::Value(it->second->GetNo());
		ptCenter = PointFToPoint(it->second->GetCenterPoint());

		arr["CenterX"] = Json::Value(ptCenter.X);
		arr["CenterY"] = Json::Value(ptCenter.Y);

		arr["Location"] = Json::Value(it->second->GetLocation());
		arr["Name"] = Json::Value(StringEx::AcsiiToUtf8(it->second->GetName().c_str()));

		str.append(it->second->GetParam().c_str());
		pstr = str.c_str();

		if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &arr["Param"], &m_JstrErrs))
		{
			DebugPrint::Print(m_JstrErrs.c_str());
			DebugPrint::Print("读取工位图标失败:\n非JSON类型字符串");

			m_JstrErrs.clear();
			return;
		}


		json.append(arr);
	}

	return;
}

void CGraphicMap::SaveTrackToJsonString(Json::Value& json)
{
	Json::Value arr;
	Point ptCenter = Point(0, 0);
	Point ptDistance = Point(0, 0);

	for (list<CGraphicTrack*>::iterator it = m_listTracks.begin(); it != m_listTracks.end();
		++it, ptCenter = Point(0,0), ptDistance = Point(0,0), arr.clear())
	{
		arr["Arc"] = Json::Value((*it)->GetType());

		ptCenter = PointFToPoint((*it)->GetCenterPoint());

		arr["CenterX"] = Json::Value(ptCenter.X);
		arr["CenterY"] = Json::Value(ptCenter.Y);

		ptDistance = PointFToPoint((*it)->GetDistancePoint());

		arr["DistanceX"] = Json::Value(ptDistance.X);
		arr["DistanceY"] = Json::Value(ptDistance.Y);

		json.append(arr);
	}

	return;
}

void CGraphicMap::SaveCallerToJsonString(Json::Value& json)
{
	Json::Value arr;
	Point ptCenter = Point(0, 0);
	string str = "";
	const char* pstr = nullptr;

	for (map<unsigned char, CGraphicCaller*>::iterator it = m_mapCallers.begin(); it != m_mapCallers.end();
		++it, ptCenter = Point(0, 0), arr.clear(), str.clear(), pstr = nullptr)
	{
		arr["No"] = Json::Value(it->second->GetNo());
		ptCenter = PointFToPoint(it->second->GetCenterPoint());

		arr["CenterX"] = Json::Value(ptCenter.X);
		arr["CenterY"] = Json::Value(ptCenter.Y);

		arr["Station"] = Json::Value(it->second->GetStation());

		str.append(it->second->GetParam().c_str());
		pstr = str.c_str();

		if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &arr["Param"], &m_JstrErrs))
		{
			DebugPrint::Print(m_JstrErrs.c_str());
			DebugPrint::Print("读取呼叫器图标失败:\n非JSON类型字符串");

			m_JstrErrs.clear();
			return;
		}


		json.append(arr);
	}

	return;
}

void CGraphicMap::SaveChargerToJsonString(Json::Value& json)
{
	Json::Value arr;
	Point ptCenter = Point(0, 0);
	string str = "";
	const char* pstr = nullptr;

	for (map<unsigned char, CGraphicCharger*>::iterator it = m_mapChargers.begin(); it != m_mapChargers.end();
		++it, ptCenter = Point(0, 0), arr.clear(), str.clear(), pstr = nullptr)
	{
		arr["No"] = Json::Value(it->second->GetNo());
		ptCenter = PointFToPoint(it->second->GetCenterPoint());

		arr["CenterX"] = Json::Value(ptCenter.X);
		arr["CenterY"] = Json::Value(ptCenter.Y);

		arr["Location"] = Json::Value(it->second->GetLocation());
		arr["Name"] = Json::Value(StringEx::AcsiiToUtf8(it->second->GetName().c_str()));
		arr["Control"] = Json::Value(it->second->GetCtrler());

		str.append(it->second->GetParam().c_str());
		pstr = str.c_str();

		if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &arr["Param"], &m_JstrErrs))
		{
			DebugPrint::Print(m_JstrErrs.c_str());
			DebugPrint::Print("读取工位图标失败:\n非JSON类型字符串");

			m_JstrErrs.clear();
			return;
		}


		json.append(arr);
	}

	return;
}

void CGraphicMap::SaveRestToJsonString(Json::Value& json)
{
	Json::Value arr;
	Point ptCenter = Point(0, 0);
	string str = "";
	const char* pstr = nullptr;

	for (map<unsigned char, CGraphicRest*>::iterator it = m_mapRests.begin(); it != m_mapRests.end();
		++it, ptCenter = Point(0, 0), arr.clear(), str.clear(), pstr = nullptr)
	{
		arr["No"] = Json::Value(it->second->GetNo());
		ptCenter = PointFToPoint(it->second->GetCenterPoint());

		arr["CenterX"] = Json::Value(ptCenter.X);
		arr["CenterY"] = Json::Value(ptCenter.Y);

		arr["Location"] = Json::Value(it->second->GetLocation());
		arr["Name"] = Json::Value(StringEx::AcsiiToUtf8(it->second->GetName().c_str()));

		str.append(it->second->GetParam().c_str());
		pstr = str.c_str();

		if (!m_pJsonReader->parse(pstr, pstr + strlen(pstr), &arr["Param"], &m_JstrErrs))
		{
			DebugPrint::Print(m_JstrErrs.c_str());
			DebugPrint::Print("读取工位图标失败:\n非JSON类型字符串");

			m_JstrErrs.clear();
			return;
		}


		json.append(arr);
	}

	return;
}

#endif //_MAP_H