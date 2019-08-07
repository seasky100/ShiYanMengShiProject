// CAGVStatusDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CAGVStatusDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// CAGVStatusDlg 对话框

IMPLEMENT_DYNAMIC(CAGVStatusDlg, CDialogEx)

CAGVStatusDlg::CAGVStatusDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AGVSTATUS_DIALOG, pParent)
{
	m_bEdit = false;
	m_nUserLevel = 0;
}

CAGVStatusDlg::~CAGVStatusDlg()
{
}

void CAGVStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AGVCONFIG_LIST, m_listctrlAGVConfig);
	DDX_Control(pDX, IDC_AGVSTATUS_LIST, m_listctrlAGVStatus);
	DDX_Control(pDX, IDC_AGVNO_COMBO, m_comboNo);
	DDX_Control(pDX, IDC_AGVCTRL_COMBO, m_comboCtrl);
	DDX_Control(pDX, IDC_TARGET_COMBO, m_comboTarget);
}

void CAGVStatusDlg::InitListCtrl()
{
	m_listctrlAGVConfig.InsertColumn(0, _T(""), LVCFMT_CENTER);
	m_listctrlAGVConfig.InsertColumn(CITEM_NO + 1, _T("编号"), LVCFMT_CENTER);
	m_listctrlAGVConfig.InsertColumn(CITEM_LOCALIP + 1, _T("本地IP地址"), LVCFMT_CENTER);
	m_listctrlAGVConfig.InsertColumn(CITEM_LOCALPORT + 1, _T("本地端口"), LVCFMT_CENTER);
	m_listctrlAGVConfig.InsertColumn(CITEM_SERVERIP + 1, _T("远程IP地址"), LVCFMT_CENTER);
	m_listctrlAGVConfig.InsertColumn(CITEM_SERVERPORT + 1, _T("远程端口"), LVCFMT_CENTER);
	m_listctrlAGVConfig.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_listctrlAGVConfig.DeleteColumn(0);

	m_listctrlAGVStatus.InsertColumn(0, _T(""), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_NO + 1, _T("编号"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_IP + 1, _T("IP地址"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_PORT + 1, _T("端口"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_LINK + 1, _T("连接状态"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_CURRFID + 1, _T("当前坐标"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_ENDRFID + 1, _T("终点坐标"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_STATUS + 1, _T("运行状态"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_SPEED + 1, _T("速度"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_POWER + 1, _T("电量"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_CARGO + 1, _T("载货状态"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_LIFTER + 1, _T("升降杆状态"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_ACTION + 1, _T("动作信息"), LVCFMT_CENTER);
	m_listctrlAGVStatus.InsertColumn(RITEM_ERROR + 1, _T("异常信息"), LVCFMT_CENTER);
	m_listctrlAGVStatus.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_listctrlAGVStatus.DeleteColumn(0);

}

void CAGVStatusDlg::SortConfigList()
{
	int nRow = m_listctrlAGVConfig.GetItemCount();
	int nCol = m_listctrlAGVConfig.GetHeaderCtrl()->GetItemCount();

	if (nRow < 2)
	{
		return;
	}

	/*!
	 * 将列表成员放入数组集合
	 * 列表成员为字符串数组
	*/
	CString** ppCStrList = new CString * [nRow];

	for (int i = 0; i < nRow; i++)
	{
		ppCStrList[i] = new CString[nCol];
	}

	for (int i = 0; i < nRow; i++)
	{
		for (int l = 0; l < nCol; l++)
		{
			ppCStrList[i][l] = m_listctrlAGVConfig.GetItemText(i, l);
		}
	}

	/*!
	 * 根据条件进行排序
	 * AGV的排序条件为AGV编号
	 * 冒泡排序
	*/
	for (int i = 1; i < nRow; i++)
	{
		for (int j = 0; j < nRow - i; j++)
		{
			if (_ttoi(ppCStrList[j][0]) > _ttoi(ppCStrList[j + 1][0]))
			{
				CString* pCStrItem = ppCStrList[j];
				ppCStrList[j] = ppCStrList[j + 1];
				ppCStrList[j + 1] = pCStrItem;
			}
		}
	}

	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			m_listctrlAGVConfig.SetItemText(i, j, ppCStrList[i][j]);
		}
	}

	for (int i = 0; i < nRow; i++)
	{
		delete[] ppCStrList[i];
	}

	delete[] ppCStrList;

	return;
}

void CAGVStatusDlg::SortStatusList()
{
	int nRow = m_listctrlAGVStatus.GetItemCount();
	int nCol = m_listctrlAGVStatus.GetHeaderCtrl()->GetItemCount();

	if (nRow < 2)
	{
		return;
	}

	/*!
	 * 将列表成员放入数组集合
	 * 列表成员为字符串数组
	*/
	CString** ppCStrList = new CString * [nRow];

	for (int i = 0; i < nRow; i++)
	{
		ppCStrList[i] = new CString[nCol];
	}

	for (int i = 0; i < nRow; i++)
	{
		for (int l = 0; l < nCol; l++)
		{
			ppCStrList[i][l] = m_listctrlAGVStatus.GetItemText(i, l);
		}
	}

	/*!
	 * 根据条件进行排序
	 * AGV的排序条件为AGV编号
	 * 冒泡排序
	*/
	for (int i = 1; i < nRow; i++)
	{
		for (int j = 0; j < nRow - i; j++)
		{
			if (_ttoi(ppCStrList[j][0]) > _ttoi(ppCStrList[j + 1][0]))
			{
				CString* pCStrItem = ppCStrList[j];
				ppCStrList[j] = ppCStrList[j + 1];
				ppCStrList[j + 1] = pCStrItem;
			}
		}
	}

	for (int i = 0; i < nRow; i++)
	{
		for (int j = 0; j < nCol; j++)
		{
			m_listctrlAGVStatus.SetItemText(i, j, ppCStrList[i][j]);
		}
	}

	for (int i = 0; i < nRow; i++)
	{
		delete[] ppCStrList[i];
	}

	delete[] ppCStrList;

	return;
}

void CAGVStatusDlg::ListSize()
{
	if (GetDlgItem(IDC_ADDAGV_BUTTON)->GetSafeHwnd() == nullptr
		|| GetDlgItem(IDC_EDITAGV_BUTTON)->GetSafeHwnd() == nullptr
		|| GetDlgItem(IDC_DELAGV_BUTTON)->GetSafeHwnd() == nullptr
		|| m_listctrlAGVConfig.GetSafeHwnd() == nullptr
		|| m_listctrlAGVStatus.GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rect;
	GetClientRect(&rect);

	/*!
	 * 调整对话框中的空间位置
	*/

	int nWidth = 100;	/*!< 宽 */
	int nHeight = 25;	/*!< 高 */

	rect.top += 5;
	int nDis = 10; // (rect.Width() - 4 * nWidth) / 5;

	CRect rcButton = rect;

	if (m_bEdit)
	{
		rcButton = rect;
		rcButton.bottom = rcButton.top + nHeight;

		rcButton.left += nDis;
		rcButton.right = rcButton.left + nWidth;

		GetDlgItem(IDC_ADDAGV_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_EDITAGV_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_DELAGV_BUTTON)->MoveWindow(rcButton);

		rect.top = rcButton.bottom + 5;
	}
	else
	{
		rcButton.bottom = rcButton.top + nHeight;

		rcButton.left += nDis;
		rcButton.right = rcButton.left + nWidth;

		GetDlgItem(IDC_AGVNO_STATIC)->MoveWindow(rcButton);

		rcButton.bottom = rect.bottom;
		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rect.bottom;
		}

		m_comboNo.MoveWindow(rcButton);

		rcButton.bottom = rcButton.top + nHeight;
		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_AGVCTRL_STATIC)->MoveWindow(rcButton);

		rcButton.bottom = rect.bottom;
		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rect.bottom;
		}

		m_comboCtrl.MoveWindow(rcButton);

		rcButton.bottom = rcButton.top + nHeight;
		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_TARGET_STATIC)->MoveWindow(rcButton);

		rcButton.bottom = rect.bottom;
		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth * 2;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth * 2;
			rcButton.bottom = rect.bottom;
		}

		m_comboTarget.MoveWindow(rcButton);

		rcButton.bottom = rcButton.top + nHeight;
		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_EXE_BUTTON)->MoveWindow(rcButton);

		rcButton.bottom = rcButton.top + nHeight;
		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_CLEAR_BUTTON)->MoveWindow(rcButton);

		rect.top = rcButton.bottom + 5;
	}
	
	m_listctrlAGVStatus.MoveWindow(rect);
	m_listctrlAGVConfig.MoveWindow(rect);

	/*!
	 * 调整列表中列的宽度
	*/
	int nItemWidth = 0;
	int nMinWidt = 50;
	int nCol = m_listctrlAGVConfig.GetHeaderCtrl()->GetItemCount();

	m_listctrlAGVConfig.GetClientRect(&rect);

	nItemWidth = rect.Width() / nCol;

	m_listctrlAGVConfig.SetColumnWidth(CITEM_NO, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVConfig.SetColumnWidth(CITEM_LOCALIP, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlAGVConfig.SetColumnWidth(CITEM_LOCALPORT, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVConfig.SetColumnWidth(CITEM_SERVERIP, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlAGVConfig.SetColumnWidth(CITEM_SERVERPORT, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);

	nCol = m_listctrlAGVStatus.GetHeaderCtrl()->GetItemCount();
	m_listctrlAGVStatus.GetClientRect(&rect);

	nItemWidth = rect.Width() / nCol;

	m_listctrlAGVStatus.SetColumnWidth(RITEM_NO, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_IP, nItemWidth> nMinWidt * 2 ? nItemWidth: nMinWidt * 2);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_PORT, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_LINK, nItemWidth > nMinWidt ? nItemWidth  : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_CURRFID, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_ENDRFID, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_STATUS, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_SPEED, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_POWER, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_CARGO, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_LIFTER, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_ACTION, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlAGVStatus.SetColumnWidth(RITEM_ERROR, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);

	return;
}

void CAGVStatusDlg::OpenEditMode()
{
	m_bEdit = true;

	if (m_listctrlAGVConfig.GetSafeHwnd() == nullptr
		|| m_listctrlAGVStatus.GetSafeHwnd() == nullptr)
	{
		return;
	}
	
	m_listctrlAGVConfig.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_ADDAGV_BUTTON)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDITAGV_BUTTON)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_DELAGV_BUTTON)->ShowWindow(SW_SHOW);

	m_listctrlAGVStatus.ShowWindow(SW_HIDE);
	m_comboNo.ShowWindow(SW_HIDE);
	m_comboCtrl.ShowWindow(SW_HIDE);
	m_comboTarget.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_AGVNO_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_AGVCTRL_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TARGET_STATIC)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EXE_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CLEAR_BUTTON)->ShowWindow(SW_HIDE);

	SetLevel(m_nUserLevel);

	ListSize();

	return;
}

void CAGVStatusDlg::OpenRunMode()
{
	m_bEdit = false;

	if (m_listctrlAGVConfig.GetSafeHwnd() == nullptr
		|| m_listctrlAGVStatus.GetSafeHwnd() == nullptr)
	{
		return;
	}

	m_listctrlAGVConfig.ShowWindow(SW_HIDE);
	GetDlgItem(IDC_ADDAGV_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_EDITAGV_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_DELAGV_BUTTON)->EnableWindow(false);
	GetDlgItem(IDC_ADDAGV_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDITAGV_BUTTON)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_DELAGV_BUTTON)->ShowWindow(SW_HIDE);

	m_listctrlAGVStatus.ShowWindow(SW_SHOW);
	m_comboNo.ShowWindow(SW_SHOW);
	m_comboCtrl.ShowWindow(SW_SHOW);
	m_comboTarget.ShowWindow(SW_SHOW);
	GetDlgItem(IDC_AGVNO_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_AGVCTRL_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_TARGET_STATIC)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EXE_BUTTON)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_CLEAR_BUTTON)->ShowWindow(SW_SHOW);

	SetLevel(m_nUserLevel);

	ListSize();

	return;
}

void CAGVStatusDlg::AddNewConfig(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonConfig;

	if (strcmp(Json_ReadString(JsonRoot["Type"]).c_str(), "AGV") != 0)
	{
		return;
	}

	JsonParam = JsonRoot["Param"];
	JsonConfig = JsonParam["Param"];

	int nNo = Json_ReadInt(JsonParam["No"]);
	string strClientIP = Json_ReadString(JsonConfig["ClientIP"]).c_str();
	int nClientPort = Json_ReadInt(JsonConfig["ClientPort"]);
	string strServerIP = Json_ReadString(JsonConfig["ServerIP"]).c_str();
	int nServerPort = Json_ReadInt(JsonConfig["ServerPort"]);

	if (strClientIP == "")
	{
		strClientIP = "0.0.0.0";
	}

	if (strServerIP == "")
	{
		strServerIP = "0.0.0.0";
	}

	CString strText = _T("");
	strText.Format(_T("%d"), nNo);
	int nRow = m_listctrlAGVConfig.InsertItem(m_listctrlAGVConfig.GetItemCount(), strText);

	m_listctrlAGVConfig.SetItemText(nRow, CITEM_LOCALIP, StringEx::string_to_CString(strClientIP));
	m_listctrlAGVConfig.SetItemText(nRow, CITEM_SERVERIP, StringEx::string_to_CString(strServerIP));

	strText.Format(_T("%d"), nClientPort);
	m_listctrlAGVConfig.SetItemText(nRow, CITEM_LOCALPORT, strText);

	strText.Format(_T("%d"), nServerPort);
	m_listctrlAGVConfig.SetItemText(nRow, CITEM_SERVERPORT, strText);

	SortConfigList();

	AddNewStatus(nNo);

	return;
}

void CAGVStatusDlg::AddNewStatus(int nNo)
{
	CString strText = _T("");
	strText.Format(_T("%d"), nNo);
	int nRow = m_listctrlAGVStatus.InsertItem(m_listctrlAGVStatus.GetItemCount(), strText);
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_IP, _T("0.0.0.0"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_PORT, _T("0"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_LINK, _T("未连接"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_CURRFID, _T("0"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_ENDRFID, _T("0"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_STATUS, _T("待机"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_SPEED, _T("0%"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_POWER, _T("0%"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_CARGO, _T("空载"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_LIFTER, _T("落下"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_ACTION, _T("无动作"));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_ERROR, _T("正常"));

	SortStatusList();

	AddAGV(nNo);
	return;
}

void CAGVStatusDlg::EditConfig(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam, JsonConfig;

	if (strcmp(Json_ReadString(JsonRoot["Type"]).c_str(), "AGV") != 0)
	{
		return;
	}

	JsonParam = JsonRoot["Param"];
	JsonConfig = JsonParam["Param"];

	int nNo = Json_ReadInt(JsonParam["No"]);
	string strClientIP = Json_ReadString(JsonConfig["ClientIP"]).c_str();
	int nClientPort = Json_ReadInt(JsonConfig["ClientPort"]);
	string strServerIP = Json_ReadString(JsonConfig["ServerIP"]).c_str();
	int nServerPort = Json_ReadInt(JsonConfig["ServerPort"]);

	if (strClientIP == "")
	{
		strClientIP = "0.0.0.0";
	}

	if (strServerIP == "")
	{
		strServerIP = "0.0.0.0";
	}

	int nRow = -1;
	for (int i = 0; i < m_listctrlAGVConfig.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlAGVConfig.GetItemText(i, CITEM_NO)) == nNo)
		{
			nRow = i;

			break;
		}
	}

	if (nRow == -1)
	{
		return;
	}

	CString strText = _T("");
	m_listctrlAGVConfig.SetItemText(nRow, CITEM_LOCALIP, StringEx::string_to_CString(strClientIP));
	m_listctrlAGVConfig.SetItemText(nRow, CITEM_SERVERIP, StringEx::string_to_CString(strServerIP));

	strText.Format(_T("%d"), nClientPort);
	m_listctrlAGVConfig.SetItemText(nRow, CITEM_LOCALPORT, strText);

	strText.Format(_T("%d"), nServerPort);
	m_listctrlAGVConfig.SetItemText(nRow, CITEM_SERVERPORT, strText);

	return;
}

bool CAGVStatusDlg::UpdateStatus(Json::Value& JsonValue)
{
	m_mutex.lock();
	m_listUpdate.push_back(JsonValue);
	m_mutex.unlock();
	
	return true;
}

void CAGVStatusDlg::UpdateStatus()
{
	if (m_listctrlAGVStatus.GetSafeHwnd() == nullptr)
	{
		return;
	}

	if (m_listUpdate.size() == 0)
	{
		return;
	}

	m_mutex.lock();
	Json::Value JsonRoot = m_listUpdate.front();
	m_listUpdate.pop_front();
	m_mutex.unlock();

	Json::Value JsonRFID, JsonMsg, JsonStatus, JsonPower, JsonCargo, JsonError;

	JsonMsg = JsonRoot["Message"];
	JsonRFID = JsonRoot["RFID"];
	JsonStatus = JsonMsg["Status"];
	JsonPower = JsonMsg["Power"];
	JsonCargo = JsonMsg["Cargo"];
	JsonError = JsonMsg["Error"];

	int nRow = -1;

	for (int i = 0; i < m_listctrlAGVStatus.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlAGVStatus.GetItemText(i, RITEM_NO)) == Json_ReadInt(JsonRoot["No"]))
		{
			nRow = i;

			break;
		}
	}

	if (nRow == -1)
	{
		return;
	}

	CString strText = _T("");
	strText.Format(_T("%d"), Json_ReadInt(JsonRFID["Cur"]));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_CURRFID, strText);

	strText.Format(_T("%d"), Json_ReadInt(JsonRFID["End"]));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_ENDRFID, strText);

	if (Json_ReadBool(JsonRoot["Connect"].asBool()))
	{
		m_listctrlAGVStatus.SetItemText(nRow, RITEM_LINK, _T("已连接"));
	}
	else
	{
		m_listctrlAGVStatus.SetItemText(nRow, RITEM_LINK, _T("未连接"));
	}

	m_listctrlAGVStatus.SetItemText(nRow, RITEM_IP, StringEx::string_to_CString(Json_ReadString(JsonMsg["IP"]).c_str()));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_PORT, StringEx::string_to_CString(Json_ReadString(JsonMsg["Port"]).c_str()));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_STATUS, StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonStatus["Value"]).c_str())));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_SPEED, StringEx::string_to_CString(Json_ReadString(JsonMsg["Speed"]).c_str()));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_POWER, StringEx::string_to_CString(Json_ReadString(JsonPower["Value"]).c_str()));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_CARGO, StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonCargo["Value"]).c_str())));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_LIFTER, StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonMsg["Lifter"]).c_str())));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_ACTION, StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonMsg["Action"]).c_str())));
	m_listctrlAGVStatus.SetItemText(nRow, RITEM_ERROR, StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonError["Value"]).c_str())));

	//Invalidate();

	return;
}

void CAGVStatusDlg::DeleteConfig(int nNo)
{
	int nRow = -1;
	for (int i = 0; i < m_listctrlAGVConfig.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlAGVConfig.GetItemText(i, CITEM_NO)) == nNo)
		{
			nRow = i;

			break;
		}
	}

	if (nRow == -1)
	{
		return;
	}

	m_listctrlAGVConfig.DeleteItem(nRow);

	DeleteStatus(nNo);

	return;
}

void CAGVStatusDlg::DeleteStatus(int nNo)
{
	int nRow = -1;
	for (int i = 0; i < m_listctrlAGVStatus.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlAGVStatus.GetItemText(i, RITEM_NO)) == nNo)
		{
			nRow = i;

			break;
		}
	}

	if (nRow == -1)
	{
		return;
	}

	m_listctrlAGVStatus.DeleteItem(nRow);

	DeleteAGV(nNo);

	return;
}

void CAGVStatusDlg::SetLevel(int nLevel)
{
	m_nUserLevel = nLevel;

	if (m_listctrlAGVConfig.GetSafeHwnd() == nullptr
		|| m_listctrlAGVStatus.GetSafeHwnd() == nullptr)
	{
		return;
	}

	bool bEnable[7];
	int nIndex = 0;

	bEnable[nIndex++] = false;
	bEnable[nIndex++] = false;
	bEnable[nIndex++] = false;
	bEnable[nIndex++] = false;
	bEnable[nIndex++] = false;
	bEnable[nIndex++] = false;
	bEnable[nIndex++] = false;

	if (m_bEdit)
	{
		if (m_nUserLevel > 1)
		{
			nIndex = 4;

			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
		}
	}
	else
	{
		if (m_nUserLevel > 0)
		{
			nIndex = 0;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			if (m_comboCtrl.GetCurSel() == 0)
			{
				bEnable[nIndex++] = true;
			}
			else
			{
				bEnable[nIndex++] = false;
			}
			bEnable[nIndex++] = true;
		}
	}

	nIndex = 0;

	m_comboNo.EnableWindow(bEnable[nIndex++]);
	m_comboCtrl.EnableWindow(bEnable[nIndex++]);
	m_comboTarget.EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_EXE_BUTTON)->EnableWindow(bEnable[nIndex++]);

	GetDlgItem(IDC_ADDAGV_BUTTON)->EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_EDITAGV_BUTTON)->EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_DELAGV_BUTTON)->EnableWindow(bEnable[nIndex++]);

	return;
}

void CAGVStatusDlg::InitComboCtrl()
{
	m_comboCtrl.InsertString(0, _T("移动"));
	m_comboCtrl.InsertString(1, _T("交通管制通行"));
	m_comboCtrl.InsertString(2, _T("急停"));
	m_comboCtrl.InsertString(3, _T("状态复位"));
	m_comboCtrl.InsertString(4, _T("升降杆上升"));
	m_comboCtrl.InsertString(5, _T("升降杆下降"));
	
	return;
}

void CAGVStatusDlg::AddStation(const unsigned char nNo, const char* strName)
{
	m_mapStation[nNo].clear();
	m_mapStation[nNo] = StringEx::string_format("%d：%s", nNo, strName);

	InitComboStation();

	return;
}

void CAGVStatusDlg::DeleteStation(const unsigned char nNo)
{
	if (m_mapStation.find(nNo) == m_mapStation.end())
	{
		return;
	}

	m_mapStation.erase(m_mapStation.find(nNo));

	InitComboStation();

	return;
}

void CAGVStatusDlg::AddAGV(const unsigned char nNo)
{
	if (m_setAGV.find(nNo) != m_setAGV.end())
	{
		return;
	}

	m_setAGV.insert(nNo);

	InitComboAGV();

	return;
}

void CAGVStatusDlg::DeleteAGV(const unsigned char nNo)
{
	if (m_setAGV.find(nNo) == m_setAGV.end())
	{
		return;
	}

	m_setAGV.erase(m_setAGV.find(nNo));

	InitComboAGV();

	return;
}

void CAGVStatusDlg::InitComboStation()
{
	if (m_comboTarget.GetSafeHwnd() == nullptr)
	{
		return;
	}

	m_comboTarget.ResetContent();

	for (map<unsigned char, string>::iterator it = m_mapStation.begin(); it != m_mapStation.end(); ++it)
	{
		m_comboTarget.AddString(StringEx::string_to_CString(it->second.c_str()));
	}

	m_comboTarget.SetCurSel(-1);

	return;
}

void CAGVStatusDlg::InitComboAGV()
{
	if (m_comboNo.GetSafeHwnd() == nullptr)
	{
		return;
	}

	m_comboNo.ResetContent();

	CString itemText = _T("");

	for (set<unsigned char>::iterator it = m_setAGV.begin(); it != m_setAGV.end(); ++it, itemText = _T(""))
	{
		itemText.Format(_T("%d"), *it);
		m_comboNo.AddString(itemText);
	}

	m_comboNo.SetCurSel(-1);

	return;
}


BEGIN_MESSAGE_MAP(CAGVStatusDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_ADDAGV_BUTTON, &CAGVStatusDlg::OnBnClickedAddagvButton)
	ON_BN_CLICKED(IDC_EDITAGV_BUTTON, &CAGVStatusDlg::OnBnClickedEditagvButton)
	ON_BN_CLICKED(IDC_DELAGV_BUTTON, &CAGVStatusDlg::OnBnClickedDelagvButton)
//	ON_WM_LBUTTONDBLCLK()
ON_CBN_SELCHANGE(IDC_AGVCTRL_COMBO, &CAGVStatusDlg::OnCbnSelchangeAgvctrlCombo)
ON_BN_CLICKED(IDC_EXE_BUTTON, &CAGVStatusDlg::OnBnClickedExeButton)
ON_WM_TIMER()
ON_BN_CLICKED(IDC_CLEAR_BUTTON, &CAGVStatusDlg::OnBnClickedClearButton)
END_MESSAGE_MAP()


// CAGVStatusDlg 消息处理程序


void CAGVStatusDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ListSize();
}


BOOL CAGVStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitListCtrl();
	InitComboCtrl();
	InitComboAGV();
	InitComboStation();

	if (m_bEdit)
	{
		OpenEditMode();
	}
	else
	{
		OpenRunMode();
	}

	SetTimer(1, 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CAGVStatusDlg::OnBnClickedAddagvButton()
{
	// TODO: 在此添加控件通知处理程序代码
	string strJson;
	Json::Value root;
	root["Type"] = Json::Value("AGV");
	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->CreateWidget(root) == false)
	{
		return;
	}

	return;
}


void CAGVStatusDlg::OnBnClickedEditagvButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlAGVConfig.GetItemCount(); i++)
	{
		if (m_listctrlAGVConfig.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlAGVConfig.GetItemText(i, CITEM_NO));

			m_listctrlAGVConfig.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	string strJson;
	Json::Value root,param;
	root["Type"] = Json::Value("AGV");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->EditWidget(root) == false)
	{
		return;
	}

	return;
}


void CAGVStatusDlg::OnBnClickedDelagvButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlAGVConfig.GetItemCount(); i++)
	{
		if (m_listctrlAGVConfig.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlAGVConfig.GetItemText(i, CITEM_NO));

			m_listctrlAGVConfig.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	Json::Value root, param;
	root["Type"] = Json::Value("AGV");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->DeleteWidget(root) == false)
	{
		return;
	}

	return;
}


BOOL CAGVStatusDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	switch (pMsg->wParam)
	{
	case VK_ESCAPE:
		return TRUE;
	case VK_RETURN:
		return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


//void CAGVStatusDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	CDialogEx::OnLButtonDblClk(nFlags, point);
//}


void CAGVStatusDlg::OnCbnSelchangeAgvctrlCombo()
{
	// TODO: 在此添加控件通知处理程序代码
	int nSel = m_comboCtrl.GetCurSel();

	if (nSel == 0)
	{
		if (m_nUserLevel > 0)
		{
			m_comboTarget.EnableWindow(true);
		}
	}
	else
	{
		m_comboTarget.EnableWindow(false);
	}
}


void CAGVStatusDlg::OnBnClickedExeButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_comboNo.GetCurSel() == -1)
	{
		MessageBox(_T("未选择执行AGV"),_T("操作AGV"),MB_ICONERROR);
		return;
	}

	if (m_comboCtrl.GetCurSel() == -1)
	{
		MessageBox(_T("未选择命令"), _T("操作AGV"), MB_ICONERROR);
		return;
	}

	int nIndexCtrl = m_comboCtrl.GetCurSel();

	if (nIndexCtrl == 0)
	{
		if (m_comboTarget.GetCurSel() == -1)
		{
			MessageBox(_T("未选择目标工位"), _T("操作AGV"), MB_ICONERROR);
			return;
		}
	}

	CString cstrNo = _T(""), cstrTarget = _T("");

	m_comboNo.GetLBText(m_comboNo.GetCurSel(), cstrNo);

	Json::Value JsonRoot;

	JsonRoot["No"] = Json::Value(_ttoi(cstrNo));

	switch (nIndexCtrl)
	{
	case 0:
		JsonRoot["Cmd"] = Json::Value("Move");

		m_comboTarget.GetLBText(m_comboTarget.GetCurSel(), cstrTarget);

		for (map<unsigned char, string>::iterator it = m_mapStation.begin(); it != m_mapStation.end(); ++it)
		{
			if (strcmp(it->second.c_str(), StringEx::CString_to_string(cstrTarget).c_str()) == 0)
			{
				JsonRoot["Target"] = Json::Value(it->first);
				break;
			}
		}

		break;
	case 1:
		JsonRoot["Cmd"] = Json::Value("TrafficPass");
		break;
	case 2:
		JsonRoot["Cmd"] = Json::Value("Scream");
		break;
	case 3:
		JsonRoot["Cmd"] = Json::Value("Reset");
		break;
	case 4:
		JsonRoot["Cmd"] = Json::Value("LiftUp");
		break;
	case 5:
		JsonRoot["Cmd"] = Json::Value("LiftDown");
		break;
	}

	((CMainFrame*)AfxGetApp()->m_pMainWnd)->ControlAGV(JsonRoot);

	return;
}


void CAGVStatusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateStatus();

	CDialogEx::OnTimer(nIDEvent);
}


void CAGVStatusDlg::OnBnClickedClearButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_comboNo.GetCurSel() == -1)
	{
		MessageBox(_T("未选择执行AGV"), _T("操作AGV"), MB_ICONERROR);
		return;
	}

	CString cstrNo = _T("");

	m_comboNo.GetLBText(m_comboNo.GetCurSel(), cstrNo);

	Json::Value JsonRoot;

	JsonRoot["No"] = Json::Value(_ttoi(cstrNo));

	JsonRoot["Cmd"] = Json::Value("ClearStatus");

	((CMainFrame*)AfxGetApp()->m_pMainWnd)->ControlAGV(JsonRoot);

	return;
}
