// CTaskDlg.cpp: 实现文件
//

#include "pch.h"
#include "ProjectApp.h"
#include "CTaskDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "../json/json.h"


// CTaskDlg 对话框

IMPLEMENT_DYNAMIC(CTaskDlg, CDialogEx)

CTaskDlg::CTaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TASK_DIALOG, pParent)
{
	m_bEdit = false;
	m_nUserLevel = 0;
}

CTaskDlg::~CTaskDlg()
{
	m_mutex.lock();
	m_listUpdate.clear();
	m_mutex.unlock();
}

void CTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TASK_LIST, m_listctrlTask);
}

void CTaskDlg::InitListCtrl()
{
	m_listctrlTask.InsertColumn(0, _T(""), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_NO + 1, _T("ID"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_TYPE + 1, _T("类型"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_START + 1, _T("起始工位"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_END + 1, _T("终止工位"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_EXECUTER + 1, _T("执行AGV编号"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_STATUS + 1, _T("状态"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_PUBLISH + 1, _T("发布时间"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_UPDATE + 1, _T("更新时间"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_FINISH + 1, _T("完成时间"), LVCFMT_CENTER);
	m_listctrlTask.InsertColumn(ITEM_TEXT + 1, _T("备注"), LVCFMT_CENTER);
	m_listctrlTask.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_DOUBLEBUFFER);
	m_listctrlTask.DeleteColumn(0);

	return;
}

void CTaskDlg::ListSize()
{
	if (m_listctrlTask.GetSafeHwnd() == nullptr)
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

		GetDlgItem(IDC_CREATE_BUTTON)->MoveWindow(rcButton);

		rcButton.left = rcButton.right + nDis;
		rcButton.right = rcButton.left + nWidth;

		if (rcButton.right > rect.right)
		{
			rcButton.left = rect.left + nDis;
			rcButton.top = rcButton.bottom + 10;
			rcButton.right = rcButton.left + nWidth;
			rcButton.bottom = rcButton.top + nHeight;
		}

		GetDlgItem(IDC_CANCEL_BUTTON)->MoveWindow(rcButton);

		rect.top = rcButton.bottom + 5;
	}	

	m_listctrlTask.MoveWindow(rect);

	/*!
	 * 调整列表中列的宽度
	*/
	int nItemWidth = 0;
	int nMinWidt = 50;
	int nCol = m_listctrlTask.GetHeaderCtrl()->GetItemCount();

	m_listctrlTask.GetClientRect(&rect);

	nItemWidth = rect.Width() / nCol;

	m_listctrlTask.SetColumnWidth(ITEM_NO, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlTask.SetColumnWidth(ITEM_TYPE, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlTask.SetColumnWidth(ITEM_START, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlTask.SetColumnWidth(ITEM_END, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlTask.SetColumnWidth(ITEM_EXECUTER, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlTask.SetColumnWidth(ITEM_STATUS, nItemWidth > nMinWidt ? nItemWidth : nMinWidt);
	m_listctrlTask.SetColumnWidth(ITEM_UPDATE, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlTask.SetColumnWidth(ITEM_PUBLISH, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlTask.SetColumnWidth(ITEM_FINISH, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);
	m_listctrlTask.SetColumnWidth(ITEM_TEXT, nItemWidth > nMinWidt * 2 ? nItemWidth : nMinWidt * 2);

	return;
}

bool CTaskDlg::UpdateTask(Json::Value& JsonValue)
{
	m_mutex.lock();
	m_listUpdate.push_back(JsonValue);
	m_mutex.unlock();

	return true;
}

void CTaskDlg::UpdateStatus()
{
	if (m_listctrlTask.GetSafeHwnd() == nullptr)
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

	int nNo = Json_ReadInt(JsonRoot["No"]);

	int nRow = -1;
	for (int i = 0; i < m_listctrlTask.GetItemCount(); i++)
	{
		if (_ttoi(m_listctrlTask.GetItemText(i, ITEM_NO)) == nNo)
		{
			nRow = i;

			break;
		}
	}

	CString cstrText = _T("");
	cstrText.Format(_T("%d"), nNo);

	if (nRow == -1)
	{
		nRow = m_listctrlTask.InsertItem(0, cstrText);
	}

	unsigned char byStart = 0;	/*!< 起始工位编号 */
	unsigned char byEnd = 0;	/*!< 终止工位编号 */
	string strStatus = "";		/*!< 任务状态 */
	unsigned char byAGVNo = 0;	/*!< 执行AGV编号 */
	string strType = "";		/*!< 任务类型 */
	string strPublish = "";		/*!< 发布时间 */
	string strUpdate = "";		/*!< 发布时间 */
	string strFinish = "";		/*!< 完成时间 */
	string strText = "";		/*!< 备注 */

	byStart = Json_ReadInt(JsonRoot["Start"]);
	byEnd = Json_ReadInt(JsonRoot["End"]);
	strStatus = StringEx::Utf8ToAcsii(Json_ReadString(JsonRoot["Status"]).c_str());
	byAGVNo = Json_ReadInt(JsonRoot["Executer"]);
	strType = StringEx::Utf8ToAcsii(Json_ReadString(JsonRoot["Mission"]).c_str());
	strPublish = Json_ReadString(JsonRoot["PublishTime"]).c_str();
	strUpdate = Json_ReadString(JsonRoot["UpdateTime"]).c_str();
	strFinish = Json_ReadString(JsonRoot["FinishTime"]).c_str();
	strText = StringEx::Utf8ToAcsii(Json_ReadString(JsonRoot["Text"]).c_str());


	m_listctrlTask.SetItemText(nRow, ITEM_TYPE, StringEx::string_to_CString(strType));

	cstrText.Format(_T("%d"), byStart);
	m_listctrlTask.SetItemText(nRow, ITEM_START, cstrText);

	cstrText.Format(_T("%d"), byEnd);
	m_listctrlTask.SetItemText(nRow, ITEM_END, cstrText);

	cstrText.Format(_T("%d"), byAGVNo);
	m_listctrlTask.SetItemText(nRow, ITEM_EXECUTER, cstrText);

	m_listctrlTask.SetItemText(nRow, ITEM_STATUS, StringEx::string_to_CString(strStatus));
	m_listctrlTask.SetItemText(nRow, ITEM_PUBLISH, StringEx::string_to_CString(strPublish));
	m_listctrlTask.SetItemText(nRow, ITEM_UPDATE, StringEx::string_to_CString(strUpdate));
	m_listctrlTask.SetItemText(nRow, ITEM_FINISH, StringEx::string_to_CString(strFinish));
	m_listctrlTask.SetItemText(nRow, ITEM_TEXT, StringEx::string_to_CString(strText));

	Invalidate();

	return;
}

void CTaskDlg::SetEnableControl(bool bEdit)
{
	m_bEdit = bEdit;

	if (m_listctrlTask.GetSafeHwnd() == nullptr)
	{
		return;
	}

	SetLevel(m_nUserLevel);

	if (m_bEdit)
	{
		GetDlgItem(IDC_CANCEL_BUTTON)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CREATE_BUTTON)->ShowWindow(SW_SHOW);

		ListSize();
	}
	else
	{
		GetDlgItem(IDC_CANCEL_BUTTON)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CREATE_BUTTON)->ShowWindow(SW_HIDE);

		ListSize();
	}

	return;
}

void CTaskDlg::SetLevel(int nLevel)
{
	m_nUserLevel = nLevel;

	if (m_listctrlTask.GetSafeHwnd() == nullptr)
	{
		return;
	}

	if (m_nUserLevel > 1 && m_bEdit)
	{
		GetDlgItem(IDC_CREATE_BUTTON)->EnableWindow(true);
		GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(true);
	}
	else
	{
		GetDlgItem(IDC_CREATE_BUTTON)->EnableWindow(false);
		GetDlgItem(IDC_CANCEL_BUTTON)->EnableWindow(false);
	}

	return;
}


BEGIN_MESSAGE_MAP(CTaskDlg, CDialogEx)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_CREATE_BUTTON, &CTaskDlg::OnBnClickedCreateButton)
	ON_BN_CLICKED(IDC_CANCEL_BUTTON, &CTaskDlg::OnBnClickedCancelButton)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CTaskDlg 消息处理程序


void CTaskDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ListSize();
}


BOOL CTaskDlg::PreTranslateMessage(MSG* pMsg)
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


BOOL CTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	InitListCtrl();
	//InitADO();

	//LoadAllTask();

	SetEnableControl(m_bEdit);

	SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void CTaskDlg::OnBnClickedCreateButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_dlgCreateTask.DoModal() == IDOK)
	{
		if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->CreateTask(
			m_dlgCreateTask.m_byStart, m_dlgCreateTask.m_byEnd, m_dlgCreateTask.m_byAGV, m_dlgCreateTask.m_strType.c_str()) == false)
		{
			MessageBox(_T("任务取消失败"), _T("任务管理"), MB_ICONSTOP);
			return;
		}
	}

	return;
}


void CTaskDlg::OnBnClickedCancelButton()
{
	// TODO: 在此添加控件通知处理程序代码
	int nTask = 0;

	for (int i = 0; i < m_listctrlTask.GetItemCount(); i++)
	{
		if (m_listctrlTask.GetItemState(i, LVNI_SELECTED) == LVNI_SELECTED)
		{
			nTask = _ttoi(m_listctrlTask.GetItemText(i, ITEM_NO));

			m_listctrlTask.SetItemState(i, ~LVNI_SELECTED, LVNI_SELECTED);

			break;
		}
	}

	if (nTask == 0)
	{
		return;
	}

	if (((CMainFrame*)AfxGetApp()->m_pMainWnd)->CancelTask(nTask) == false)
	{
		MessageBox(_T("任务取消失败"),_T("任务管理"),MB_ICONSTOP);
		return;
	}

	return;

}


void CTaskDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UpdateStatus();

	CDialogEx::OnTimer(nIDEvent);
}
