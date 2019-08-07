// CCallerStatusDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CCallerStatusDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CCallerStatusDlg 对话框

IMPLEMENT_DYNAMIC(CCallerStatusDlg, CDialogEx)

CCallerStatusDlg::CCallerStatusDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CALLERSTATUS_DIALOG, pParent)
{
	m_bEdit = false;
	m_nUserLevel = 0;
}

CCallerStatusDlg::~CCallerStatusDlg()
{
}

void CCallerStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CALLERSTATUS_LIST, m_listctrlCaller);
}

void CCallerStatusDlg::InitListCtrl()
{
	m_listctrlCaller.InsertColumn(0, _T(""), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_NO + 1, _T("编号"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_STATION + 1, _T("绑定工位"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_USE + 1, _T("是否启用"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_MAC + 1, _T("MAC地址"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_CALLSIGNAL + 1, _T("呼叫信号"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_CANCELCALLSIGNAL + 1, _T("取消呼叫信号"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_PASSSIGNAL + 1, _T("放行"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_CANCELPASSSIGNAL + 1, _T("取消放行"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_NGSIGNAL + 1, _T("NG信号"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_CANCELNGSIGNAL + 1, _T("取消NG信号"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_SCREAM + 1, _T("急停信号"), LVCFMT_CENTER);
	m_listctrlCaller.InsertColumn(ITEM_PACKOUTBOUND + 1, _T("出站校验"), LVCFMT_CENTER);
	m_listctrlCaller.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_listctrlCaller.DeleteColumn(0);

	return;
}

void CCallerStatusDlg::SortList()
{
	int nRow = m_listctrlCaller.GetItemCount();
	int nCol = m_listctrlCaller.GetHeaderCtrl()->GetItemCount();

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
			ppCStrList[i][l] = m_listctrlCaller.GetItemText(i, l);
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
			m_listctrlCaller.SetItemText(i, j, ppCStrList[i][j]);
		}
	}

	for (int i = 0; i < nRow; i++)
	{
		delete[] ppCStrList[i];
	}

	delete[] ppCStrList;

	return;
}

void CCallerStatusDlg::ListSize()
{
	if (m_listctrlCaller.GetSafeHwnd() == nullptr)
	{
		return;
	}

	if (m_bEdit)
	{
		GetDlgItem(IDC_EDIT_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ENABLE_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DISABLE_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALLENABLE_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_ALLDISABLE_BUTTON)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_EDIT_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DEL_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ENABLE_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DISABLE_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALLENABLE_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ALLDISABLE_BUTTON)->ShowWindow(SW_HIDE);
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

	m_listctrlCaller.MoveWindow(rect);

	/*!
	 * 调整列表中列的宽度
	*/
	int nItemWidth = 0;
	int nMinWidt = 50;
	int nCol = m_listctrlCaller.GetHeaderCtrl()->GetItemCount();

	m_listctrlCaller.GetClientRect(&rect);

	nItemWidth = rect.Width() / nCol;

	m_listctrlCaller.SetColumnWidth(ITEM_NO, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_STATION, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_USE, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_MAC, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_CALLSIGNAL, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_CANCELCALLSIGNAL, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_PASSSIGNAL, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_CANCELPASSSIGNAL, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_NGSIGNAL, nItemWidth > nMinWidt? nItemWidth: nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_CANCELNGSIGNAL, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_SCREAM, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlCaller.SetColumnWidth(ITEM_PACKOUTBOUND, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);

	return;
}

void CCallerStatusDlg::AddNewConfig(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam;

	if (strcmp(Json_ReadString(JsonRoot["Type"]).c_str(), "Caller") != 0)
	{
		return;
	}

	JsonParam = JsonRoot["Param"];

	int nNo = Json_ReadInt(JsonParam["No"]);
	int nStation = Json_ReadInt(JsonParam["Station"]);
	bool bUse = Json_ReadInt(JsonParam["Param"]["Use"]);
	string strMAC = Json_ReadString(JsonParam["Param"]["MAC"]);

	CString strText = _T("");
	strText.Format(_T("%d"), nNo);
	int nRow = m_listctrlCaller.InsertItem(m_listctrlCaller.GetItemCount(), strText);

	strText.Format(_T("%d"), nStation);
	m_listctrlCaller.SetItemText(nRow, ITEM_STATION, strText);

	if (bUse)
	{
		m_listctrlCaller.SetItemText(nRow, ITEM_USE, _T("是"));
	}
	else
	{
		m_listctrlCaller.SetItemText(nRow, ITEM_USE, _T("否"));
	}

	m_listctrlCaller.SetItemText(nRow, ITEM_MAC, StringEx::string_to_CString(strMAC.c_str()));

	m_listctrlCaller.SetItemText(nRow, ITEM_CALLSIGNAL, _T("0"));
	m_listctrlCaller.SetItemText(nRow, ITEM_CANCELCALLSIGNAL, _T("0"));
	m_listctrlCaller.SetItemText(nRow, ITEM_PASSSIGNAL, _T("0"));
	m_listctrlCaller.SetItemText(nRow, ITEM_CANCELPASSSIGNAL, _T("0"));
	m_listctrlCaller.SetItemText(nRow, ITEM_NGSIGNAL, _T("0"));
	m_listctrlCaller.SetItemText(nRow, ITEM_CANCELNGSIGNAL, _T("0"));
	m_listctrlCaller.SetItemText(nRow, ITEM_SCREAM, _T("0"));
	m_listctrlCaller.SetItemText(nRow, ITEM_PACKOUTBOUND, _T("未校验"));

	SortList();

	return;
}

void CCallerStatusDlg::EditConfig(Json::Value& JsonValue)
{
	Json::Value& JsonRoot = JsonValue;
	Json::Value JsonParam;

	if (strcmp(Json_ReadString(JsonRoot["Type"]).c_str(), "Caller") != 0)
	{
		return;
	}

	JsonParam = JsonRoot["Param"];

	int nNo = Json_ReadInt(JsonParam["No"]);
	int nStation = Json_ReadInt(JsonParam["Station"]);
	bool bUse = Json_ReadInt(JsonParam["Param"]["Use"]);
	string strMAC = Json_ReadString(JsonParam["Param"]["MAC"]);

	int nRow = -1;
	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlCaller.GetItemText(i, ITEM_NO)) == nNo)
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
	strText.Format(_T("%d"), nStation);
	m_listctrlCaller.SetItemText(nRow, ITEM_STATION, strText);

	if (bUse)
	{
		m_listctrlCaller.SetItemText(nRow, ITEM_USE, _T("是"));
	}
	else
	{
		m_listctrlCaller.SetItemText(nRow, ITEM_USE, _T("否"));
	}

	m_listctrlCaller.SetItemText(nRow, ITEM_MAC, StringEx::string_to_CString(strMAC.c_str()));

	return;
}

bool CCallerStatusDlg::UpdateStatus(Json::Value& JsonValue)
{
	m_mutex.lock();
	m_listUpdate.push_back(JsonValue);
	m_mutex.unlock();

	return true;
}

void CCallerStatusDlg::UpdateStatus()
{
	if (m_listctrlCaller.GetSafeHwnd() == nullptr)
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

	int nRow = -1;

	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlCaller.GetItemText(i, ITEM_NO)) == Json_ReadInt(JsonRoot["No"]))
		{
			nRow = i;

			break;
		}
	}

	if (nRow == -1)
	{
		return;
	}

	if (Json_ReadBool(JsonRoot["PackOutBound"].asBool()))
	{
		m_listctrlCaller.SetItemText(nRow, ITEM_PACKOUTBOUND, _T("校验成功"));
	}
	else
	{
		m_listctrlCaller.SetItemText(nRow, ITEM_PACKOUTBOUND, _T("未校验"));
	}

	CString strText = _T("");

	strText.Format(_T("%d"), Json_ReadInt(JsonRoot["Signal"]["Call"]));
	m_listctrlCaller.SetItemText(nRow, ITEM_CALLSIGNAL, strText);

	strText.Format(_T("%d"), Json_ReadInt(JsonRoot["Signal"]["CancelCall"]));
	m_listctrlCaller.SetItemText(nRow, ITEM_CANCELCALLSIGNAL, strText);

	strText.Format(_T("%d"), Json_ReadInt(JsonRoot["Signal"]["Pass"]));
	m_listctrlCaller.SetItemText(nRow, ITEM_PASSSIGNAL, strText);

	strText.Format(_T("%d"), Json_ReadInt(JsonRoot["Signal"]["CancelPass"]));
	m_listctrlCaller.SetItemText(nRow, ITEM_CANCELPASSSIGNAL, strText);

	strText.Format(_T("%d"), Json_ReadInt(JsonRoot["Signal"]["NG"]));
	m_listctrlCaller.SetItemText(nRow, ITEM_NGSIGNAL, strText);

	strText.Format(_T("%d"), Json_ReadInt(JsonRoot["Signal"]["CancelNG"]));
	m_listctrlCaller.SetItemText(nRow, ITEM_CANCELNGSIGNAL, strText);

	strText.Format(_T("%d"), Json_ReadInt(JsonRoot["Signal"]["Scream"]));
	m_listctrlCaller.SetItemText(nRow, ITEM_SCREAM, strText);

	//Invalidate();

	return;
}

void CCallerStatusDlg::DeleteConfig(int nNo)
{
	int nRow = -1;
	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlCaller.GetItemText(i, ITEM_NO)) == nNo)
		{
			nRow = i;

			break;
		}
	}

	if (nRow == -1)
	{
		return;
	}

	m_listctrlCaller.DeleteItem(nRow);

	return;
}

void CCallerStatusDlg::SetEnableEdit(bool bEdit)
{
	m_bEdit = bEdit;

	if (m_listctrlCaller.GetSafeHwnd() == nullptr)
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

void CCallerStatusDlg::SetLevel(int nLevel)
{
	m_nUserLevel = nLevel;

	if (m_listctrlCaller.GetSafeHwnd() == nullptr)
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


BEGIN_MESSAGE_MAP(CCallerStatusDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_EDIT_BUTTON, &CCallerStatusDlg::OnBnClickedEditButton)
	ON_BN_CLICKED(IDC_DEL_BUTTON, &CCallerStatusDlg::OnBnClickedDelButton)
	ON_BN_CLICKED(IDC_ENABLE_BUTTON, &CCallerStatusDlg::OnBnClickedEnableButton)
	ON_BN_CLICKED(IDC_DISABLE_BUTTON, &CCallerStatusDlg::OnBnClickedDisableButton)
	ON_BN_CLICKED(IDC_ALLENABLE_BUTTON, &CCallerStatusDlg::OnBnClickedAllenableButton)
	ON_BN_CLICKED(IDC_ALLDISABLE_BUTTON, &CCallerStatusDlg::OnBnClickedAlldisableButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CCallerStatusDlg 消息处理程序


BOOL CCallerStatusDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitListCtrl();

	SetEnableEdit(m_bEdit);

	SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CCallerStatusDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ListSize();
}


BOOL CCallerStatusDlg::PreTranslateMessage(MSG* pMsg)
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


void CCallerStatusDlg::OnBnClickedEditButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		if (m_listctrlCaller.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlCaller.GetItemText(i, ITEM_NO));

			m_listctrlCaller.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	string strJson;
	Json::Value root, param;
	root["Type"] = Json::Value("Caller");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->EditWidget(root) == false)
	{
		return;
	}

	return;
}


void CCallerStatusDlg::OnBnClickedDelButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		if (m_listctrlCaller.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlCaller.GetItemText(i, ITEM_NO));

			m_listctrlCaller.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	Json::Value root, param;
	root["Type"] = Json::Value("Caller");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->DeleteWidget(root) == false)
	{
		return;
	}

	return;
}


void CCallerStatusDlg::OnBnClickedEnableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		if (m_listctrlCaller.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlCaller.GetItemText(i, ITEM_NO));

			m_listctrlCaller.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	string strJson;
	Json::Value root, param;
	root["Type"] = Json::Value("Caller");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->SetEnableCaller(root,TRUE) == false)
	{
		return;
	}

	return;
}


void CCallerStatusDlg::OnBnClickedDisableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nNo = 0;

	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		if (m_listctrlCaller.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nNo = _ttoi(m_listctrlCaller.GetItemText(i, ITEM_NO));

			m_listctrlCaller.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nNo == 0)
	{
		return;
	}

	string strJson;
	Json::Value root, param;
	root["Type"] = Json::Value("Caller");
	param["No"] = Json::Value(nNo);
	root["Param"] = param;

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->SetEnableCaller(root, FALSE) == false)
	{
		return;
	}

	return;
}


void CCallerStatusDlg::OnBnClickedAllenableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		m_listctrlCaller.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);

		OnBnClickedEnableButton();
	}

	return;
}


void CCallerStatusDlg::OnBnClickedAlldisableButton()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = 0; i < m_listctrlCaller.GetItemCount(); i++)
	{
		m_listctrlCaller.SetItemState(i, LVNI_SELECTED, LVNI_SELECTED);

		OnBnClickedDisableButton();
	}

	return;
}

void CCallerStatusDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateStatus();

	CDialogEx::OnTimer(nIDEvent);
}
