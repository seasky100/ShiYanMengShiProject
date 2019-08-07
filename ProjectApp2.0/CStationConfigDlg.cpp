// CStationConfigDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CStationConfigDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// CStationConfigDlg 对话框

IMPLEMENT_DYNAMIC(CStationConfigDlg, CDialogEx)

CStationConfigDlg::CStationConfigDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_STATIONCONFIG_DIALOG, pParent)
{
	m_bEdit = false;
	m_nUserLevel = 0;
}

CStationConfigDlg::~CStationConfigDlg()
{
}

void CStationConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIONCONFIG_LIST, m_listctrlStationConfig);
}

void CStationConfigDlg::InitListCtrl()
{
	m_listctrlStationConfig.InsertColumn(0, _T(""), LVCFMT_CENTER);
	m_listctrlStationConfig.InsertColumn(ITEM_NO + 1, _T("编号"), LVCFMT_CENTER);
	m_listctrlStationConfig.InsertColumn(ITEM_LOCATION + 1, _T("工位坐标"), LVCFMT_CENTER);
	m_listctrlStationConfig.InsertColumn(ITEM_NAME + 1, _T("工位名称"), LVCFMT_CENTER);
	m_listctrlStationConfig.InsertColumn(ITEM_PROCESS + 1, _T("绑定工序"), LVCFMT_CENTER);
	m_listctrlStationConfig.InsertColumn(ITEM_MESWP + 1, _T("MES工站编号"), LVCFMT_CENTER);
	m_listctrlStationConfig.InsertColumn(ITEM_MES + 1, _T("是否启用MES检测"), LVCFMT_CENTER);
	m_listctrlStationConfig.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_listctrlStationConfig.DeleteColumn(0);

	return;
}

void CStationConfigDlg::SortList()
{
	int nRow = m_listctrlStationConfig.GetItemCount();
	int nCol = m_listctrlStationConfig.GetHeaderCtrl()->GetItemCount();

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
			ppCStrList[i][l] = m_listctrlStationConfig.GetItemText(i, l);
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
			m_listctrlStationConfig.SetItemText(i, j, ppCStrList[i][j]);
		}
	}

	for (int i = 0; i < nRow; i++)
	{
		delete[] ppCStrList[i];
	}

	delete[] ppCStrList;

	return;
}

void CStationConfigDlg::ListSize()
{
	if (m_listctrlStationConfig.GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rect;
	GetClientRect(&rect);

	if (m_bEdit)
	{
		/*!
	 * 调整对话框中的空间位置
	*/
		int nWidth = 100;	/*!< 宽 */
		int nHeight = 25;	/*!< 高 */

		rect.top += 5;
		int nDis = 10; // (rect.Width() - 4 * nWidth) / 5;

		CRect rcButton = rect;
		rcButton.bottom = rcButton.top + nHeight;

		rcButton.left += nDis;
		rcButton.right = rcButton.left + nWidth;

		GetDlgItem(IDC_EDIT_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_DEL_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_ENABLE_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_DISABLE_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_ALLENABLE_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_ALLDISABLE_BUTTON)->MoveWindow(rcButton);

		rect.top = rcButton.bottom + 5;
	}

	m_listctrlStationConfig.MoveWindow(rect);

	int nCol = m_listctrlStationConfig.GetHeaderCtrl()->GetItemCount();

	/*!
	 * 调整列表中列的宽度
	*/
	int nItemWidth = 0;
	int nMinWidt = 50;

	m_listctrlStationConfig.GetClientRect(&rect);

	nItemWidth = rect.Width() / nCol;

	m_listctrlStationConfig.SetColumnWidth(ITEM_NO, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlStationConfig.SetColumnWidth(ITEM_LOCATION, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlStationConfig.SetColumnWidth(ITEM_NAME, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlStationConfig.SetColumnWidth(ITEM_PROCESS, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlStationConfig.SetColumnWidth(ITEM_MESWP, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlStationConfig.SetColumnWidth(ITEM_MES, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);

	return;
}

void CStationConfigDlg::AddNewConfig(const char* strJson)
{
	Json::Value JsonRoot, JsonParam;
	Json::CharReaderBuilder readerBuilder;
	Json::CharReader* reader = readerBuilder.newCharReader();
	JSONCPP_STRING JstrErrs;

	const char* pstr = strJson;

	if (!reader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &JstrErrs))
	{
		DebugPrint::Print(JstrErrs.c_str());
		DebugPrint::Print("添加图标失败:\n非JSON类型字符串");

		delete reader;
		JstrErrs.clear();
		return;
	}

	delete reader;

	if (strcmp(Json_ReadString(JsonRoot["Type"]).c_str(), "Station") != 0)
	{
		return;
	}

	JsonParam = JsonRoot["Param"];

	int nNo = Json_ReadInt(JsonParam["No"]);
	int nLocation = Json_ReadInt(JsonParam["Location"]);
	int nProcess = Json_ReadInt(JsonParam["Param"]["Process"]);
	bool bMes = Json_ReadInt(JsonParam["Param"]["MES"]);

	CString strText = _T("");
	strText.Format(_T("%d"), nNo);
	int nRow = m_listctrlStationConfig.InsertItem(m_listctrlStationConfig.GetItemCount(), strText);

	strText.Format(_T("%d"), nLocation);
	m_listctrlStationConfig.SetItemText(nRow, ITEM_LOCATION, strText);
	m_listctrlStationConfig.SetItemText(nRow, ITEM_NAME, 
		StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str()).c_str()));
	m_listctrlStationConfig.SetItemText(nRow, ITEM_MESWP,
		StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Param"]["MESWP"]).c_str()).c_str()));

	if (bMes)
	{
		m_listctrlStationConfig.SetItemText(nRow, ITEM_MES, _T("是"));
	}
	else
	{
		m_listctrlStationConfig.SetItemText(nRow, ITEM_MES, _T("否"));
	}
	
	switch (nProcess)
	{
	case 0:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("NG修理"));
		break;
	case 1:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体存放区"));
		break;
	case 2:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体附件安装缓存"));
		break;
	case 3:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体辅件安装"));
		break;
	case 4:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("水管安装"));
		break;
	case 5:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("下层模组吊装"));
		break;
	case 6:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("下层模组安装"));
		break;
	case 7:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("上层模组吊装"));
		break;
	case 8:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("上层模组安装"));
		break;
	case 9:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("线外BDU组装"));
		break;
	case 10:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体BDU组装"));
		break;
	case 11:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("预留工站1(从控板刷写)"));
		break;
	case 12:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("预留工站2(未知)"));
		break;
	case 13:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("EOL测试"));
		break;
	case 14:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("电池包测试1"));
		break;
	case 15:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体上盖安装缓存"));
		break;
	case 16:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("电池包测试2"));
		break;
	case 17:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体上盖安装"));
		break;
	case 18:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体气密测试"));
		break;
	case 19:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("成品包下料缓存"));
		break;
	case 20:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("成品包下料"));
		break;
	}

	SortList();

	return;
}

void CStationConfigDlg::EditConfig(const char* strJson)
{
	Json::Value JsonRoot, JsonParam;
	Json::CharReaderBuilder readerBuilder;
	Json::CharReader* reader = readerBuilder.newCharReader();
	JSONCPP_STRING JstrErrs;

	const char* pstr = strJson;

	if (!reader->parse(pstr, pstr + strlen(pstr), &JsonRoot, &JstrErrs))
	{
		DebugPrint::Print(JstrErrs.c_str());
		DebugPrint::Print("添加图标失败:\n非JSON类型字符串");

		delete reader;
		JstrErrs.clear();
		return;
	}

	delete reader;

	if (strcmp(Json_ReadString(JsonRoot["Type"]).c_str(), "Station") != 0)
	{
		return;
	}

	JsonParam = JsonRoot["Param"];

	int nNo = Json_ReadInt(JsonParam["No"]);
	int nLocation = Json_ReadInt(JsonParam["Location"]);
	int nProcess = Json_ReadInt(JsonParam["Param"]["Process"]);
	bool bMes = Json_ReadInt(JsonParam["Param"]["MES"]);

	int nRow = -1;
	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlStationConfig.GetItemText(i, ITEM_NO)) == nNo)
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
	strText.Format(_T("%d"), nLocation);
	m_listctrlStationConfig.SetItemText(nRow, ITEM_LOCATION, strText);
	m_listctrlStationConfig.SetItemText(nRow, ITEM_NAME,
		StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Name"]).c_str()).c_str()));
	m_listctrlStationConfig.SetItemText(nRow, ITEM_MESWP,
		StringEx::string_to_CString(StringEx::Utf8ToAcsii(Json_ReadString(JsonParam["Param"]["MESWP"]).c_str()).c_str()));

	if (bMes)
	{
		m_listctrlStationConfig.SetItemText(nRow, ITEM_MES, _T("是"));
	}
	else
	{
		m_listctrlStationConfig.SetItemText(nRow, ITEM_MES, _T("否"));
	}

	switch (nProcess)
	{
	case 0:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("NG修理"));
		break;
	case 1:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体存放区"));
		break;
	case 2:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体附件安装缓存"));
		break;
	case 3:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体辅件安装"));
		break;
	case 4:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("水管安装"));
		break;
	case 5:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("下层模组吊装"));
		break;
	case 6:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("下层模组安装"));
		break;
	case 7:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("上层模组吊装"));
		break;
	case 8:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("上层模组安装"));
		break;
	case 9:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("线外BDU组装"));
		break;
	case 10:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体BDU组装"));
		break;
	case 11:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("预留工站1(从控板刷写)"));
		break;
	case 12:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("预留工站2(未知)"));
		break;
	case 13:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("EOL测试"));
		break;
	case 14:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("电池包测试1"));
		break;
	case 15:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体上盖安装缓存"));
		break;
	case 16:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("电池包测试2"));
		break;
	case 17:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体上盖安装"));
		break;
	case 18:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("箱体气密测试"));
		break;
	case 19:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("成品包下料缓存"));
		break;
	case 20:
		m_listctrlStationConfig.SetItemText(nRow, ITEM_PROCESS, _T("成品包下料"));
		break;
	}

	return;
}

void CStationConfigDlg::DeleteConfig(int nNo)
{
	int nRow = -1;
	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlStationConfig.GetItemText(i, ITEM_NO)) == nNo)
		{
			nRow = i;

			break;
		}
	}

	if (nRow == -1)
	{
		return;
	}

	m_listctrlStationConfig.DeleteItem(nRow);

	return;
}

void CStationConfigDlg::SetEnableEdit(bool bEdit)
{
	m_bEdit = bEdit;

	if (m_listctrlStationConfig.GetSafeHwnd() == nullptr)
	{
		return;
	}

	SetLevel(m_nUserLevel);

	if (m_bEdit)
	{
		GetDlgItem(IDC_EDIT_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ENABLE_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DISABLE_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALLENABLE_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALLDISABLE_BUTTON)->ShowWindow(SW_SHOW);

		ListSize();
	}
	else
	{
		GetDlgItem(IDC_EDIT_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ENABLE_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DISABLE_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALLENABLE_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALLDISABLE_BUTTON)->ShowWindow(SW_HIDE);

		ListSize();
	}

	return;
}

void CStationConfigDlg::SetLevel(int nLevel)
{
	m_nUserLevel = nLevel;

	if (m_listctrlStationConfig.GetSafeHwnd() == nullptr)
	{
		return;
	}

	bool bEnable[6];
	int nIndex = 0;

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
			nIndex = 0;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
		}
		else if (m_nUserLevel > 0)
		{
			nIndex = 2;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
			bEnable[nIndex++] = true;
		}
	}

	nIndex = 0;

	GetDlgItem(IDC_EDIT_BUTTON)->EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_DEL_BUTTON)->EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_ENABLE_BUTTON)->EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_DISABLE_BUTTON)->EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_ALLENABLE_BUTTON)->EnableWindow(bEnable[nIndex++]);
	GetDlgItem(IDC_ALLDISABLE_BUTTON)->EnableWindow(bEnable[nIndex++]);

	return;
}

BEGIN_MESSAGE_MAP(CStationConfigDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CStationConfigDlg::OnBnClickedEditButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON, &CStationConfigDlg::OnBnClickedDelButton)
	ON_BN_CLICKED(IDC_ENABLE_BUTTON, &CStationConfigDlg::OnBnClickedEnableButton)
	ON_BN_CLICKED(IDC_DISABLE_BUTTON, &CStationConfigDlg::OnBnClickedDisableButton)
	ON_BN_CLICKED(IDC_ALLENABLE_BUTTON, &CStationConfigDlg::OnBnClickedAllenableButton)
	ON_BN_CLICKED(IDC_ALLDISABLE_BUTTON, &CStationConfigDlg::OnBnClickedAlldisableButton)
END_MESSAGE_MAP()


// CStationConfigDlg 消息处理程序


BOOL CStationConfigDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitListCtrl();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


BOOL CStationConfigDlg::PreTranslateMessage(MSG* pMsg)
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


void CStationConfigDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ListSize();
}


void CStationConfigDlg::OnBnClickedEditButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		if (m_listctrlStationConfig.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlStationConfig.GetItemText(i, ITEM_NO));

			m_listctrlStationConfig.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	string strJson;
	Json::Value root, param;
	root["Type"] = Json::Value("Station");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->EditWidget(root) == false)
	{
		return;
	}

	return;
}


void CStationConfigDlg::OnBnClickedDelButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		if (m_listctrlStationConfig.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlStationConfig.GetItemText(i, ITEM_NO));

			m_listctrlStationConfig.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	Json::Value root, param;
	root["Type"] = Json::Value("Station");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->DeleteWidget(root) == false)
	{
		return;
	}

	return;
}


void CStationConfigDlg::OnBnClickedEnableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		if (m_listctrlStationConfig.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlStationConfig.GetItemText(i, ITEM_NO));

			m_listctrlStationConfig.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	string strJson;
	Json::Value root, param;
	root["Type"] = Json::Value("Station");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->SetEnableStation(root, TRUE) == false)
	{
		return;
	}

	return;
}


void CStationConfigDlg::OnBnClickedDisableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		if (m_listctrlStationConfig.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlStationConfig.GetItemText(i, ITEM_NO));

			m_listctrlStationConfig.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	string strJson;
	Json::Value root, param;
	root["Type"] = Json::Value("Station");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->SetEnableStation(root, FALSE) == false)
	{
		return;
	}

	return;
}


void CStationConfigDlg::OnBnClickedAllenableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		m_listctrlStationConfig.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);

		OnBnClickedEnableButton();
	}

	return;
}


void CStationConfigDlg::OnBnClickedAlldisableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < m_listctrlStationConfig.GetItemCount(); i++)
	{
		m_listctrlStationConfig.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);

		OnBnClickedDisableButton();
	}

	return;
}
