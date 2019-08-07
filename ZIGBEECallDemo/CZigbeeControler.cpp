//#include "pch.h"
#include "CZigbeeControler.h"

#ifdef _ZGBCTRLER_H

#include "../CommunicateDemo/CSerialPort.cpp"
#include "../Data/CRC.h"
#include "../Thread/ThreadEx.h"
#include "../String/StringEx.h"
//#include "../json/json_writer.cpp"
//#include "../json/json_value.cpp"
//#include "../json/json_reader.cpp"

list<DataBuffer> CZigbeeControler::m_listProcess;
mutex CZigbeeControler::m_mutexProcess;

CZigbeeControler::CZigbeeControler(unsigned char byNo)
	:m_byNo(byNo)
	, m_pThreadSend(nullptr)
	, m_hShutdownEvent(nullptr)
{
	m_pJsonReader = m_JsonBuilder.newCharReader();
}

CZigbeeControler::CZigbeeControler(unsigned char byNo, unsigned short usPort, unsigned int unBaud, char chParity, unsigned char byDataBits, unsigned char byStopBits)
	:CSerialPort(usPort, unBaud, chParity, byDataBits, byStopBits)
	,m_byNo(byNo)
	, m_pThreadSend(nullptr)
	, m_hShutdownEvent(nullptr)
{
	m_pJsonReader = m_JsonBuilder.newCharReader();
}


CZigbeeControler::~CZigbeeControler()
{
	Close();

	delete m_pJsonReader;
}

void CZigbeeControler::Heartbeat(ZGBCaller* pCaller)
{
	char ach[4];
	pCaller->GetMACAddress(ach);

	Heartbeat(pCaller->GetNo(), ach);

	return;
}

void CZigbeeControler::Heartbeat(const unsigned char byNo, char achMAC[4])
{
	/*
	// 声明Json变量，这个作为根
	Json::Value JsonRoot;
	JsonRoot["No"] = Json::Value(byNo);
	JsonRoot["Type"] = Json::Value("Caller");
	JsonRoot["Cmd"] = Json::Value("Heartbeat");

	string strJsonString = JsonRoot.toStyledString(); //  JSON字符串
	*/

	unsigned char abyPack[2];
	abyPack[0] = byNo;
	abyPack[1] = FUNC_HEARTBEAT;

	// 发送报文
	InsertIntoWhileList((char*)abyPack, sizeof(abyPack), achMAC);

	return;
}

void CZigbeeControler::Reset(ZGBCaller* pCaller)
{
	char ach[4];
	pCaller->GetMACAddress(ach);

	Reset(pCaller->GetNo(), ach);

	return;
}

void CZigbeeControler::Reset(const unsigned char byNo, char achMAC[4])
{
	/*
	// 声明Json变量，这个作为根
	Json::Value JsonRoot;
	JsonRoot["No"] = Json::Value(byNo);
	JsonRoot["Type"] = Json::Value("Caller");
	JsonRoot["Cmd"] = Json::Value("Reset");

	string strJsonString = JsonRoot.toStyledString(); //  JSON字符串
	*/

	unsigned char abyPack[2];
	abyPack[0] = byNo;
	abyPack[1] = FUNC_RESET;

	// 发送报文
	InsertIntoSingleList((char*)abyPack, sizeof(abyPack), achMAC);

	return;
}

void CZigbeeControler::SetSignal(ZGBCaller* pCaller, const unsigned char bySignal, const unsigned char bySwitch)
{
	char ach[4];
	pCaller->GetMACAddress(ach);

	SetSignal(pCaller->GetNo(), ach, bySignal, bySwitch);

	return;
}

void CZigbeeControler::SetSignal(const unsigned char byNo, char achMAC[4],const unsigned char bySignal, const unsigned char bySwitch)
{
	unsigned char abyPack[4];
	abyPack[0] = byNo;
	abyPack[1] = FUNC_SETSIGNAL;
	abyPack[2] = bySignal;
	abyPack[3] = bySwitch;

	// 发送报文
	InsertIntoSingleList((char*)abyPack, sizeof(abyPack), achMAC);

	return;
}

/*
void CZigbeeControler::SetSignal(const unsigned char byNo, const char* strSignal, const unsigned char bySwitch)
{
	// 声明Json变量，这个作为根
	Json::Value JsonRoot;
	JsonRoot["No"] = Json::Value(byNo);
	JsonRoot["Type"] = Json::Value("Caller");
	JsonRoot["Cmd"] = Json::Value("SetSignal");
	JsonRoot["Signal"] = Json::Value(strSignal);
	JsonRoot["Switch"] = Json::Value(bySwitch);

	string strJsonString = JsonRoot.toStyledString();  // JSON字符串

	// 发送报文
	InsertIntoSingleList(strJsonString.c_str(), strJsonString.length());

	return;
}
*/

/*
void CZigbeeControler::SetSignal(const unsigned char byNo, const wchar_t* strSignal, const unsigned char bySwitch)
{
	return SetSignal(byNo, StringEx::wstring_to_string(strSignal).c_str(), bySwitch);
}
*/

bool CZigbeeControler::Open()
{
	EndSendThread();

	if (OpenSerialPort())
	{
		StartSendThread();

		return true;
	}

	return false;
}

bool CZigbeeControler::Open(unsigned short usPort, unsigned int unBaud, char chParity, unsigned char byDataBits, unsigned char byStopBits)
{
	EndSendThread();

	if(OpenSerialPort(usPort, unBaud, chParity, byDataBits, byStopBits))
	{
		StartSendThread();

		return true;
	}

	return false;
}

void CZigbeeControler::Close()
{
	EndSendThread();

	ClosePort();

	return;
}

bool CZigbeeControler::IsOpened()
{
	return CSerialPort::IsOpened();
}

DataBuffer CZigbeeControler::GetProcessPackage()
{
	if (m_listProcess.size() == 0)
	{
		return DataBuffer();
	}

	lock_guard<mutex> lock(m_mutexProcess);

	DataBuffer buffer = m_listProcess.front();
	m_listProcess.pop_front();

	return buffer;
}

bool CZigbeeControler::StartSendThread()
{
	EndSendThread();

	if (m_hShutdownEvent)
	{
		ResetEvent(m_hShutdownEvent);
	}
	else
	{
		m_hShutdownEvent = CreateEvent(nullptr, true, false, nullptr);
	}

	m_pThreadSend = new std::thread(&CZigbeeControler::SendThread, this);

	if (m_pThreadSend)
	{
		return true;
	}

	return false;
}

void CZigbeeControler::EndSendThread()
{
	if (m_pThreadSend == nullptr)
	{
		return;
	}

	if (m_pThreadSend->joinable())
	{
		SetEvent(m_hShutdownEvent);

		m_pThreadSend->join();
	}

	delete m_pThreadSend;
	m_pThreadSend = nullptr;

	return;
}

void CZigbeeControler::SendThread()
{
	list<DataBuffer> listSend;

	while (true)
	{
		if (WaitForSingleObject(m_hShutdownEvent, 1) == WAIT_OBJECT_0)
		{
			break;
		}

		if (m_mutexSend.try_lock() == false)
		{
			if (WaitForSingleObject(m_hShutdownEvent, 1) == WAIT_OBJECT_0)
			{
				break;
			}

			continue;
		}

		listSend = m_listWhile;

		if (m_listSingle.size() > 0)
		{
			listSend.splice(listSend.begin(), m_listSingle);
			m_listSingle.size();
		}
		
		m_mutexSend.unlock();

		for (list<DataBuffer>::iterator it = listSend.begin(); it != listSend.end(); it = listSend.erase(it))
		{
			WriteToPort((char*)((*it).m_pbyBuffer), (*it).m_unSize);

			if (WaitForSingleObject(m_hShutdownEvent, 50) == WAIT_OBJECT_0)
			{
				break;
			}
		}
	}

	listSend.clear();
	m_listSingle.clear();
	m_listWhile.clear();

	return;
}

void CZigbeeControler::ReadChar(const char* data, int len)
{
	m_bufferRead.Add((unsigned char*)data, len);
	DataBuffer& bufferProcess = m_bufferRead;

	char* pLeftData = (char*)bufferProcess.m_pbyBuffer;	/*!< 剩余数据 */
	int nLeftDataLen = bufferProcess.m_unSize;			/*!< 剩余数据长度 */

	char* pHead = nullptr;			/*!< 报文头地址 */
	char* pTail = nullptr;			/*!< 报文尾地址 */
	int nBody = 0;					/*!< 数据体长度 */
	char* pSourceData = nullptr;	/*!< 源数据 */
	int nSourceLen = 0;				/*!< 源数据长度 */
	unsigned short usPackLen = 0;	/*!< 报文长度 */
	unsigned short usRealLen = 0;	/*!< 实际长度 */
	unsigned short usPackCheck = 0;	/*!< 校验值 */
	unsigned short usRealCheck = 0;	/*!< 实际校验值 */

	for (;; pHead = nullptr, pTail = nullptr, nBody = 0, pSourceData = nullptr, nSourceLen = 0, usPackLen = 0, usRealLen = 0, usPackCheck = 0, usRealCheck = 0)
	{
		// 查找报文头
		pHead = (char*)memchr(pLeftData, 0xBA, nLeftDataLen);

		// 剩余数据中无报文头
		if (pHead == nullptr)
		{
			bufferProcess.Release();
			break;
		}

		// 查找报文尾
		pTail = (char*)memchr(pHead + 1, 0xBE, bufferProcess.m_unSize - (pHead - (char*)bufferProcess.m_pbyBuffer) - 1);

		// 剩余数据中无报文尾
		if (pTail == nullptr)
		{
			// 保留报文头后的数据
			bufferProcess = DataBuffer((unsigned char*)pHead, bufferProcess.m_unSize - (pHead - (char*)bufferProcess.m_pbyBuffer));

			break;
		}

		nBody = pTail - pHead - 1;
		pSourceData = new char[nBody];
		nSourceLen = 0;

		if (pSourceData == nullptr)
		{
			continue;
		}

		if (nBody < 2)
		{
			delete[] pSourceData;
			continue;
		}

		// 初始化
		memset(pSourceData, 0, nBody);
		// 反转义
		Retranslation(pHead + 1, nBody, pSourceData, nSourceLen);

		unsigned char byLenHi = pSourceData[0];
		unsigned char byLenLo = pSourceData[1];

		usPackLen = (byLenHi << 8) | byLenLo;

		usRealLen = nSourceLen - 4;

		// 数据长度不相同
		if (usPackLen != usRealLen)
		{
			/*
			stringstream ssHex; // 16进制数据

			for (unsigned int unI = 0; unI < nBody; ++unI)
			{
				ssHex << StringEx::string_format("%02X ", (*(pHead + 1 + unI)) & 0xFF);
			}
			DebugPrint::Printf("(源数据)16进制:%s\n", ssHex.str().c_str());

			ssHex.str("");

			for (unsigned int unI = 0; unI < nSourceLen; ++unI)
			{
				ssHex << StringEx::string_format("%02X ", (*(pSourceData + unI)) & 0xFF);
			}
			DebugPrint::Printf("(转义后)16进制:%s\n", ssHex.str().c_str());

			DebugPrint::Printf("Zigbee控制器%d数据处理错误:\n报文长度:%d;\n实际接收长度:%d;", m_byNo, usPackLen, usRealLen);
			*/

			// 释放源数据
			delete[] pSourceData;
			
			pLeftData = pTail + 1;
			nLeftDataLen = bufferProcess.m_unSize - (pTail + 1 - (char*)bufferProcess.m_pbyBuffer);

			// 无剩余数据
			if (nLeftDataLen <= 0)
			{
				bufferProcess.Release();
				break;
			}

			continue;
		}

		unsigned char byCheckHi = *(pSourceData + 2 + usPackLen);
		unsigned char byCheckLo = *(pSourceData + 3 + usPackLen);

		usPackCheck = (byCheckHi << 8) | byCheckLo;

		usRealCheck = CRC::CRC16((unsigned char*)pSourceData, usRealLen + 2);	/*!< 实际校验值 */

		if (usPackCheck != usRealCheck)
		{
			DebugPrint::Printf("Zigbee控制器%d数据处理错误:\n报文校验值:%04X;\n实际校验值:%04X;", m_byNo, usPackCheck, usRealCheck);

			// 释放源数据
			delete[] pSourceData;

			pLeftData = pTail + 1;
			nLeftDataLen = bufferProcess.m_unSize - (pTail + 1 - (char*)bufferProcess.m_pbyBuffer);

			// 无剩余数据
			if (nLeftDataLen <= 0)
			{
				bufferProcess.Release();
				break;
			}

			continue;
		}

		/*
		// 记录接收的报文
		stringstream ssHex; // 16进制数据
		for (unsigned int unI = 0; unI < usPackLen; ++unI)
		{
			ssHex << StringEx::string_format("%02X ", (*(pSourceData + 2 + unI) & 0xFF));
		}
		char* lpszStr = new char[usPackLen + 1];	// ASCII字符数据
		// 初始化
		memset(lpszStr, 0, usPackLen + 1);
		memcpy_s(lpszStr, usPackLen, pSourceData + 2, usPackLen);

		DebugPrint::Printf("Zigbee控制器%d接收报文成功!\n16进制:%s", m_byNo, ssHex.str().c_str());
		DebugPrint::Printf("ASCII字符串:%s", lpszStr);

		// 释放字符串 
		delete[] lpszStr;
		*/

		ProcessPackage((unsigned char*)(pSourceData + 2), usPackLen);

		// 释放源数据
		delete[] pSourceData;

		pLeftData = pTail + 1;
		nLeftDataLen = bufferProcess.m_unSize - (pTail + 1 - (char*)bufferProcess.m_pbyBuffer);

		// 无剩余数据
		if (nLeftDataLen <= 0)
		{
			bufferProcess.Release();
			break;
		}
	}
		
	return;
}

void CZigbeeControler::ProcessPackage(const unsigned char * pPack, const int nPackLen)
{
	switch (*(pPack + 1))
	{
	case FUNC_HEARTBEAT:
		InsertIntoProcessList((char*)pPack, nPackLen);
		break;
	}

	return;
}

void CZigbeeControler::Translation(const char * pSourceData, int nSourceLen, char * pTransData, int & pTransLen)
{
	/*!
	 * 将数据中所有等于0xBA的数据转义为0xB0+0x01
	 * 将数据中所有等于0xBE的数据转义为0xB0+0x02
	 * 将数据中所有等于0xB0的数据转义为0xB0+0x03
	*/
	size_t max_tmp = (size_t)nSourceLen * 2;
	char* lpszTmp = new char[max_tmp];

	if (lpszTmp == nullptr)
	{
		return;
	}

	if (max_tmp < 4)
	{
		delete[] lpszTmp;
		return;
	}

	memset(lpszTmp, 0, max_tmp);

	int nT = 0;
	for (int nS = 0; nS < nSourceLen; ++nS, ++nT)
	{
		switch (*(pSourceData + nS) & 0xFF)
		{
		case 0xBA:
		{
			lpszTmp[nT] = (char)0xB0;
			lpszTmp[++nT] = (char)0x01;
			break;
		}
		case 0xBE:
		{
			lpszTmp[nT] = (char)0xB0;
			lpszTmp[++nT] = (char)0x02;
			break;
		}
		case 0xB0:
		{
			lpszTmp[nT] = (char)0xB0;
			lpszTmp[++nT] = (char)0x03;
			break;
		}
		default:
		{
			lpszTmp[nT] = *(pSourceData + nS) & 0xFF;
			break;
		}
		}
	}

	pTransLen = nT;
	memcpy_s(pTransData, nT, lpszTmp, nT);

	delete[] lpszTmp;

	return;
}

void CZigbeeControler::Retranslation(const char * pTransData, int pTransLen, char * pSourceData, int & nSourceLen)
{
	/*!
	 * 将数据中0xB0+0x01转义为0xBA
	 * 将数据中0xB0+0x02转义为0xBE
	 * 将数据中0xB0+0x03转义为0xB0
	*/
	char* lpszTmp = new char[pTransLen];

	if (lpszTmp == nullptr)
	{
		return;
	}

	memset(lpszTmp, 0, pTransLen);

	int nS = 0;
	for (int nT = 0; nT < pTransLen; ++nT, ++nS)
	{
		if ((*(pTransData + nT) & 0xFF) == 0xB0)
		{
			++nT;
			switch (*(pTransData + nT))
			{
			case 0x01:
			{
				lpszTmp[nS] = (char)0xBA;
				break;
			}
			case 0x02:
			{
				lpszTmp[nS] = (char)0xBE;
				break;
			}
			case 0x03:
			{
				lpszTmp[nS] = (char)0xB0;
				break;
			}
			}
		}
		else
		{
			lpszTmp[nS] = *(pTransData + nT) & 0xFF;
		}
	}

	nSourceLen = nS;
	memcpy_s(pSourceData, nS, lpszTmp, nS);

	delete[] lpszTmp;

	return;
}

DataBuffer CZigbeeControler::CreatePackage(const char * pData, const int nLen, char achMAC[4])
{
	int nSourceLen = nLen + 4;							/*!< 源数据长度 不包含首位 */
	int nTransLen = nSourceLen * 2 + 6;					/*!< 转义后的最大数据长度 包含首位 */

	char *lpszSource = (char*)malloc(nSourceLen);	/*!< 储存需要转义的源数据 */

	if (lpszSource == nullptr)
	{
		return DataBuffer();
	}

	if (nSourceLen < 2)
	{
		free(lpszSource);

		return DataBuffer();
	}

	memset(lpszSource, 0, nSourceLen);
	lpszSource[0] = (nLen >> 8) & 0xFF;		/*!< 储存数据长度（高位） */
	lpszSource[1] = nLen & 0xFF;			/*!< 储存数据长度（地位）*/
	memcpy_s(&lpszSource[2], nLen,pData, nLen);

	unsigned short usCheck = CRC::CRC16((unsigned char*)lpszSource,nLen + 2);	/*! 校验值 */
	lpszSource[nSourceLen - 2] = (usCheck >> 8) & 0xFF;	/*!< 储存校验值（高位）*/
	lpszSource[nSourceLen - 1] = usCheck & 0xFF;	/*!< 储存校验值(低位) */

	char* pTrans = (char*)malloc(nTransLen);	/*!< 储存转义后的数据 */
	int nTrans = 0;	/*!< 转义后的实际数据长度 */

	if (pTrans == nullptr)
	{
		free(lpszSource);
		return DataBuffer();
	}

	if (nTransLen < 2)
	{
		free(lpszSource);
		free(pTrans);

		return DataBuffer();
	}

	memset(pTrans, 0, nTransLen);
	Translation(lpszSource, nSourceLen, &pTrans[5], nTrans);

	free(lpszSource);	/*!< 释放源数据内存 */

	memcpy_s(pTrans, 4, achMAC, 4);
	pTrans[4] = (char)0xBA;
	pTrans[nTrans + 5] = (char)0xBE;

	DataBuffer package((unsigned char*)pTrans, nTrans+6);

	free(pTrans);	/*!< 释放转义数据内存 */

	return package;
}

void CZigbeeControler::SendPackage(const char* lpszStr, char achMAC[4])
{
	return SendPackage(lpszStr, strlen(lpszStr), achMAC);
}

void CZigbeeControler::SendPackage(const char* pData, const int nLen, char achMAC[4])
{
	InsertIntoSingleList(pData, nLen, achMAC);

	return;
}

void CZigbeeControler::InsertIntoWhileList(const char* pData, const int nLen, char achMAC[4])
{
	DataBuffer pack = CreatePackage(pData, nLen, achMAC);

	lock_guard<mutex> lock(m_mutexSend);

	for (list<DataBuffer>::iterator it = m_listWhile.begin(); it != m_listWhile.end(); ++it)
	{
		if (*it == pack)
		{
			return;
		}
	}

	if (m_listWhile.size() == m_listWhile.max_size())
	{
		m_listWhile.pop_front();
	}

	m_listWhile.push_back(pack);

	return;
}

void CZigbeeControler::InsertIntoSingleList(const char* pData, const int nLen, char achMAC[4])
{
	DataBuffer pack = CreatePackage(pData, nLen, achMAC);

	lock_guard<mutex> lock(m_mutexSend);

	for (list<DataBuffer>::iterator it = m_listSingle.begin(); it != m_listSingle.end(); ++it)
	{
		if (*it == pack)
		{
			return;
		}
	}

	if (m_listSingle.size() == m_listSingle.max_size())
	{
		m_listSingle.pop_front();
	}

	m_listSingle.push_back(pack);

	return;
}

void CZigbeeControler::RemoveFromWhileList(const char* pData, const int nLen, char achMAC[4])
{
	if (m_listWhile.size() == 0)
	{
		return;
	}

	DataBuffer pack = CreatePackage(pData, nLen, achMAC);

	lock_guard<mutex> lock(m_mutexSend);

	for (list<DataBuffer>::iterator it = m_listWhile.begin(); it != m_listWhile.end();)
	{
		if (*it == pack)
		{
			it = m_listWhile.erase(it);

			break;
		}
		else
		{
			++it;
		}
	}

	return;
}

void CZigbeeControler::InsertIntoProcessList(const char* pData, const int nLen)
{
	DataBuffer pack((unsigned char*)pData, nLen);

	lock_guard<mutex> lock(m_mutexProcess);

	for (list<DataBuffer>::iterator it = m_listProcess.begin(); it != m_listProcess.end();)
	{
		if (*it == pack)
		{
			it = m_listProcess.erase(it);
			return;
		}
		else
		{
			++it;
		}
	}

	if (m_listProcess.size() == m_listProcess.max_size())
	{
		m_listProcess.pop_front();
	}

	m_listProcess.push_back(pack);

	return;
}

#endif //_ZGBCTRLER_H