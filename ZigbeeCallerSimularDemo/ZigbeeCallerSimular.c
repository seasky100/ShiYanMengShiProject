#include "ZigbeeCallerSimular.h"

#ifdef _C_ZIGBEECALLER

char g_achReceive[MAX_RECEIVE_LEN];					/*!< 数据接收缓存区:用以存放接收的数据 */
char g_achPackage[MAX_PACKAGE_LEN];					/*!< 报文数据体缓存区:用以存放解析出的JSON字符串 */
char g_achSend[MAX_SEND_LEN];						/*!< 数据发送缓存区:用以存放要发送给的数据 */

unsigned char g_byNo = 0;							/*!< 编号 */
unsigned char g_byCall = SIGNAL_OFF;					/*!< 呼叫标识 */
unsigned char g_byCancelCall = SIGNAL_OFF;			/*!< 取消呼叫标识 */
unsigned char g_byPass = SIGNAL_OFF;					/*!< 放行标识 */
unsigned char g_byCancelPass = SIGNAL_OFF;			/*!< 取消放行标识 */
unsigned char g_byNG = SIGNAL_OFF;					/*!< NG标识 */
unsigned char g_byCancelNG = SIGNAL_OFF;				/*!< 取消NG标识 */
unsigned char g_byScream = SIGNAL_OFF;				/*!< 急停标识 */

void InitCaller(unsigned char byNo)
{
	g_byNo = byNo;
	g_byCall = SIGNAL_OFF;
	g_byCancelCall = SIGNAL_OFF;
	g_byPass = SIGNAL_OFF;
	g_byCancelPass = SIGNAL_OFF;
	g_byNG = SIGNAL_OFF;
	g_byCancelNG = SIGNAL_OFF;
	g_byScream = SIGNAL_OFF;

	memset(g_achReceive, 0, MAX_RECEIVE_LEN);
	memset(g_achPackage, 0, MAX_PACKAGE_LEN);
	memset(g_achSend, 0, MAX_SEND_LEN);
}

void Translation(const char* pSourceData, int nSourceLen, char* pTransData, int* pTransLen)
{
	/*!
	 * 将数据中所有等于0xBA的数据转义为0xB0+0x01
	 * 将数据中所有等于0xBE的数据转义为0xB0+0x02
	 * 将数据中所有等于0xB0的数据转义为0xB0+0x03
	*/
	size_t max_tmp = (size_t)nSourceLen * 2;
	char* lpszTmp = (char*)malloc(max_tmp);	/*!<  临时数据 */

	int nT = 0;	/*!< 转义后数据长度 */
	int nS = 0;

	if (lpszTmp == NULL)
	{
		return;
	}

	if (max_tmp < 4)
	{
		free(lpszTmp);
		return;
	}

	// 初始化
	memset(lpszTmp, 0, max_tmp);

	for (nS = 0; nS < nSourceLen; ++nS, ++nT)
	{
		switch (*(pSourceData + nS) & 0xFF)
		{
		case PACKAGE_HEAD:
		{
			lpszTmp[nT] = (char)PACKAGE_TRANS;
			lpszTmp[++nT] = (char)0x01;
			break;
		}
		case PACKAGE_TAIL:
		{
			lpszTmp[nT] = (char)PACKAGE_TRANS;
			lpszTmp[++nT] = (char)0x02;
			break;
		}
		case PACKAGE_TRANS:
		{
			lpszTmp[nT] = (char)PACKAGE_TRANS;
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

	*pTransLen = nT;
	// 拷贝转义数据
	memcpy(pTransData, lpszTmp, nT);

	// 释放内存
	free(lpszTmp);

	return;
}

void Retranslation(const char* pTransData, int pTransLen, char* pSourceData, int* nSourceLen)
{
	/*!
	 * 将数据中0xB0+0x01转义为0xBA
	 * 将数据中0xB0+0x02转义为0xBE
	 * 将数据中0xB0+0x03转义为0xB0
	*/
	char* lpszTmp = (char*)malloc(pTransLen);	/*!< 临时数据 */
	int nS = 0;	/*!< 源数据长度 */
	int nT = 0;

	if (lpszTmp == NULL)
	{
		return;
	}

	// 初始化
	memset(lpszTmp, 0, pTransLen);

	for (nT = 0; nT < pTransLen; ++nT, ++nS)
	{
		if ((*(pTransData + nT) & 0xFF) == PACKAGE_TRANS)
		{
			++nT;
			switch (*(pTransData + nT))
			{
			case 0x01:
			{
				lpszTmp[nS] = (char)PACKAGE_HEAD;
				break;
			}
			case 0x02:
			{
				lpszTmp[nS] = (char)PACKAGE_TAIL;
				break;
			}
			case 0x03:
			{
				lpszTmp[nS] = (char)PACKAGE_TRANS;
				break;
			}
			}
		}
		else
		{
			lpszTmp[nS] = *(pTransData + nT) & 0xFF;
		}
	}

	*nSourceLen = nS;
	// 拷贝源数据
	memcpy(pSourceData, lpszTmp, nS);

	// 释放内存
	free(lpszTmp);

	return;
}

int ProcessData(const unsigned char* pProcessData, const unsigned int MAX_LEN, unsigned char* pJson, const unsigned int MAX_JSON_LEN)
{
	char* pLeftData = (char*)pProcessData;	/*!< 剩余数据 */
	int nLeftDataLen = MAX_LEN;				/*!< 剩余数据长度 */
	char* pHead = NULL;						/*!< 报文头地址 */
	char* pTail = NULL;						/*!< 报文尾地址 */
	int nBody = 0;							/*!< 数据体长度 */
	char* pSourceData = NULL;				/*!< 源数据 */
	int nSourceLen = 0;						/*!< 源数据长度 */
	unsigned short usPackLen = 0;			/*!< 报文长度 */
	unsigned short usPackCheck = 0;			/*!< 校验值 */
	unsigned short usRealLen = 0;			/*!< 实际长度 */
	unsigned short usRealCheck = 0;			/*!< 实际校验值 */

	pHead = (char*)memchr(pLeftData, PACKAGE_HEAD, nLeftDataLen);

	// 剩余数据中无报文头
	if (pHead == NULL)
	{
		return -1;
	}

	pTail = (char*)memchr(pHead + 1, PACKAGE_TAIL, MAX_LEN - (pHead - (char*)pProcessData) - 1);

	// 剩余数据中无报文尾
	if (pTail == NULL)
	{
		return 0;
	}

	nBody = pTail - pHead - 1;
	pSourceData = (char*)malloc(nBody);
	nSourceLen = 0;

	if (pSourceData == NULL)
	{
		return -1;
	}

	if (nBody < 2)
	{
		free(pSourceData);

		return -1;
	}

	// 初始化
	memset(pSourceData, 0, nBody);
	// 反转义
	Retranslation(pHead + 1, nBody, pSourceData, &nSourceLen);

	usPackLen = 0;
	usPackLen = ((pSourceData[0] & 0xFF) << 8) | (pSourceData[1] & 0xFF);

	usRealLen = nSourceLen - 4;

	// 数据长度不相同
	if (usPackLen != usRealLen)
	{
		// 释放源数据
		free(pSourceData);

		return -1;
	}

	usPackCheck = 0;
	usPackCheck = (*(pSourceData + 2 + usPackLen) & 0xFF) << 8 | (*(pSourceData + 3 + usPackLen) & 0xFF);

	usRealCheck = CRC16((unsigned char*)pSourceData, usRealLen + 2);

	// 数据长度不相同
	if (usPackCheck != usRealCheck)
	{
		// 释放源数据
		free(pSourceData);

		memset(g_achReceive, 0, MAX_RECEIVE_LEN);

		return -1;
	}

	// 初始化
	memset(pJson, 0, MAX_JSON_LEN);

	// 拷贝数据体
	memcpy(pJson, pSourceData + 2, usPackLen);

	// 释放源数据
	free(pSourceData);

	return usPackLen;
}

void CreatePackage(const char* lpszStr, const int nLen, char* pData, int* nSize)
{
	int nSourceLen = nLen + 4;						/*!< 源数据长度 不包含首位 */
	int nTransLen = nSourceLen * 2 + 2;				/*!< 转义后的最大数据长度 包含首位 */
	unsigned short usCheck = 0;						/*!< 校验值 */
	char* pTrans = NULL;							/*!< 储存转义后的数据 */
	int nTrans = 0;									/*!< 转义后的实际数据长度 */

	char* lpszSource = (char*)malloc(nSourceLen);	/*!< 储存需要转义的源数据 */

	if (lpszSource == NULL)
	{
		return;
	}

	if (nSourceLen < 5)
	{
		free(lpszSource);
		return;
	}

	// 初始化源数据
	memset(lpszSource, 0, nSourceLen);
	// 储存数据长度（高位）
	lpszSource[0] = (nLen >> 8) & 0xFF;
	// 储存数据长度（低位）
	lpszSource[1] = nLen & 0xFF;
	// 拷贝数据内容
	memcpy(&lpszSource[2], lpszStr, nLen);

	usCheck = CRC16((unsigned char*)lpszSource, nLen + 2);
	// 储存校验值（高位）
	lpszSource[nSourceLen - 2] = (usCheck >> 8) & 0xFF;
	// 储存校验值(低位)
	lpszSource[nSourceLen - 1] = usCheck & 0xFF;

	pTrans = (char*)malloc(nTransLen);
	nTrans = 0;

	if (pTrans == NULL)
	{
		free(lpszSource);
		return;
	}

	// 初始化转义数据
	memset(pTrans, 0, nTransLen);
	// 数据转义
	Translation(lpszSource, nSourceLen, &pTrans[1], &nTrans);
	//  释放源数据内存
	free(lpszSource);

	pTrans[0] = PACKAGE_HEAD;
	pTrans[nTrans + 1] = PACKAGE_TAIL;

	*nSize = nTrans + 2;

	// 拷贝转义数据
	memcpy(pData, pTrans, *nSize);

	// 释放转义数据内存 
	free(pTrans);

	return;
}

int FeedbackHeartbeat(const char* pSource, const int pSourceLen, char* pPack, const int MAX_PACK_LEN)
{
	char* lpszStr = (char*)malloc(MAX_PACK_LEN);	/*!< JSON字符串 */
	int nPackLen = 0;							/*!< 数据包长度 */
	unsigned char byIndex = 0;

	if (lpszStr == NULL)
	{
		return -1;
	}

	if (MAX_PACK_LEN < 9)
	{
		free(lpszStr);
		return -1;
	}

	// 初始化
	memset(lpszStr, 0, MAX_PACK_LEN);

	lpszStr[byIndex] = *pSource;			// 编号
	lpszStr[++byIndex] = *(pSource + 1);	// 功能
	lpszStr[++byIndex] = g_byCall;
	lpszStr[++byIndex] = g_byCancelCall;
	lpszStr[++byIndex] = g_byPass;
	lpszStr[++byIndex] = g_byCancelPass;
	lpszStr[++byIndex] = g_byNG;
	lpszStr[++byIndex] = g_byCancelNG;
	lpszStr[++byIndex] = g_byScream;

	/*
	// 合成JSON字符串
	sprintf(lpszStr,
		"{\"Message\":%s,\"Result\":{\"Call\":%d,\"CancelCall\":%d,\"Pass\":%d,\"CancelPass\":%d,\"NG\":%d,\"CancelNG\":%d,\"Scream\":%d}}"
		, pSource, g_byCall, g_byCancelCall, g_byPass, g_byCancelPass, g_byNG, g_byCancelNG, g_byScream);
	*/

	// 初始化
	memset(pPack, 0, MAX_PACK_LEN);

	CreatePackage(lpszStr, byIndex, pPack, &nPackLen);

	// 释放内存
	free(lpszStr);

	return nPackLen;
}

int FeedbackOther(const char* pSource, const int pSourceLen, char* pPack, const int MAX_PACK_LEN)
{
	char* lpszStr = (char*)malloc(MAX_PACK_LEN);	/*!< JSON字符串 */
	int nPackLen = 0;								/*!< 数据包长度 */
	unsigned char byIndex = 0;

	if (lpszStr == NULL)
	{
		return -1;
	}

	if (MAX_PACK_LEN < 2)
	{
		free(lpszStr);
		return -1;
	}

	if (MAX_PACK_LEN < pSourceLen)
	{
		free(lpszStr);
		return -1;
	}

	// 初始化
	memset(lpszStr, 0, MAX_PACK_LEN);

	for (; byIndex < pSourceLen; ++byIndex)
	{
		lpszStr[byIndex] = *(pSource + byIndex);
	}

	/*
	// 合成JSON字符串
	sprintf(lpszStr,
		"{\"Message\":%s,\"Result\":\"Success\"}"
		, pSource);
	*/

	// 初始化
	memset(pPack, 0, MAX_PACK_LEN);

	CreatePackage(lpszStr, byIndex, pPack, &nPackLen);

	// 释放内存
	free(lpszStr);

	return nPackLen;
}

#endif //_C_ZIGBEECALLER