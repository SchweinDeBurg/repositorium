//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - adjusted #include's paths
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.1.3 sources
//*****************************************************************************

// CalendarFrameWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "CalendarExtResource.h"
#include "../../Common/CalendarData.h"
#include "../../Common/CalendarUtils.h"
#include "DlgCompletedTasks.h"
#include "../../../CodeProject/Source/DateHelper.h"
#include "../../../CodeProject/Source/TimeHelper.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "../../../CodeProject/Source/VersionInfo.h"
#include "CalendarFrameWnd.h"

#include "../../ToDoList/Source/TDCMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CALENDAR_INI_KEY_SHOWMINICAL        _T("ShowMiniCalendar")
#define CALENDAR_INI_KEY_SHOWSTATUSBAR      _T("ShowStatusBar")
#define CALENDAR_INI_KEY_SHOWWEEKENDS       _T("ShowWeekends")
#define CALENDAR_INI_KEY_NUMWEEKS           _T("NumWeeks")
#define CALENDAR_INI_KEY_WINDOWMAXIMIZED    _T("WindowMaximized")
#define CALENDAR_INI_KEY_WINDOWSIZE         _T("WindowSize")
#define CALENDAR_INI_KEY_COMPLETEDTASKS     _T("CompletedTasks")

const int g_nMiniCalWidth = 172;

/////////////////////////////////////////////////////////////////////////////
// CCalendarFrameWnd

IMPLEMENT_DYNAMIC(CCalendarFrameWnd, CFrameWnd)

CCalendarFrameWnd::CCalendarFrameWnd():
m_hParentOfFrame(NULL),
m_pCalendarData(NULL),
m_nNumVisibleWeeks(0),
m_bShowMiniCalendar(FALSE),
m_bShowStatusBar(FALSE),
m_bShowWeekends(FALSE),
m_bWindowMaximized(FALSE),
m_rcWindowSize(0, 0, 0, 0),
m_dwStyleCompletedTasks(0)
{
	m_pCalendarData = new CCalendarData;

	//load Calendar settings from the INI file
	CCalendarData::ReadValueFromIni(CALENDAR_INI_KEY_SHOWMINICAL, m_bShowMiniCalendar, TRUE);
	CCalendarData::ReadValueFromIni(CALENDAR_INI_KEY_SHOWSTATUSBAR, m_bShowStatusBar, TRUE);
	CCalendarData::ReadValueFromIni(CALENDAR_INI_KEY_SHOWWEEKENDS, m_bShowWeekends, TRUE);
	CCalendarData::ReadValueFromIni(CALENDAR_INI_KEY_NUMWEEKS, m_nNumVisibleWeeks, 6);
	CCalendarData::ReadValueFromIni(CALENDAR_INI_KEY_WINDOWMAXIMIZED, m_bWindowMaximized, FALSE);
	CCalendarData::ReadValueFromIni(CALENDAR_INI_KEY_WINDOWSIZE, m_rcWindowSize, CRect(200, 200, 700, 600));
	CCalendarData::ReadValueFromIni(CALENDAR_INI_KEY_COMPLETEDTASKS, m_dwStyleCompletedTasks, COMPLETEDTASKS_STRIKETHRU | COMPLETEDTASKS_GREY);
}

CCalendarFrameWnd::~CCalendarFrameWnd()
{
	//save Calendar settings to the INI file
	CCalendarData::WriteValueToIni(CALENDAR_INI_KEY_SHOWMINICAL, m_bShowMiniCalendar);
	CCalendarData::WriteValueToIni(CALENDAR_INI_KEY_SHOWSTATUSBAR, m_bShowStatusBar);
	CCalendarData::WriteValueToIni(CALENDAR_INI_KEY_SHOWWEEKENDS, m_bShowWeekends);
	CCalendarData::WriteValueToIni(CALENDAR_INI_KEY_NUMWEEKS, m_nNumVisibleWeeks);
	CCalendarData::WriteValueToIni(CALENDAR_INI_KEY_WINDOWMAXIMIZED, m_bWindowMaximized);
	CCalendarData::WriteValueToIni(CALENDAR_INI_KEY_WINDOWSIZE, m_rcWindowSize);
	CCalendarData::WriteValueToIni(CALENDAR_INI_KEY_COMPLETEDTASKS, m_dwStyleCompletedTasks);

	delete m_pCalendarData;
}

BEGIN_MESSAGE_MAP(CCalendarFrameWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CCalendarFrameWnd)
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_COMMAND(ID_VIEW_MINICALENDAR, OnViewMiniCalendar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MINICALENDAR, OnUpdateViewMiniCalendar)
	ON_COMMAND(ID_VIEW_STATUSBAR, OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUSBAR, OnUpdateViewStatusBar)
	ON_COMMAND(ID_VIEW_WEEKENDS, OnViewWeekends)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WEEKENDS, OnUpdateViewWeekends)
	ON_COMMAND(ID_VIEW_COMPLETEDTASKS, OnViewCompletedTasks)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMPLETEDTASKS, OnUpdateViewCompletedTasks)
	ON_COMMAND(ID_VIEW_NUMWEEKS_1, OnViewNumWeeks1)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_1, OnUpdateViewNumWeeks1)
	ON_COMMAND(ID_VIEW_NUMWEEKS_2, OnViewNumWeeks2)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_2, OnUpdateViewNumWeeks2)
	ON_COMMAND(ID_VIEW_NUMWEEKS_3, OnViewNumWeeks3)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_3, OnUpdateViewNumWeeks3)
	ON_COMMAND(ID_VIEW_NUMWEEKS_4, OnViewNumWeeks4)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_4, OnUpdateViewNumWeeks4)
	ON_COMMAND(ID_VIEW_NUMWEEKS_5, OnViewNumWeeks5)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_5, OnUpdateViewNumWeeks5)
	ON_COMMAND(ID_VIEW_NUMWEEKS_6, OnViewNumWeeks6)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_6, OnUpdateViewNumWeeks6)
	ON_COMMAND(ID_VIEW_NUMWEEKS_7, OnViewNumWeeks7)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_7, OnUpdateViewNumWeeks7)
	ON_COMMAND(ID_VIEW_NUMWEEKS_8, OnViewNumWeeks8)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_8, OnUpdateViewNumWeeks8)
	ON_COMMAND(ID_VIEW_NUMWEEKS_9, OnViewNumWeeks9)
	ON_UPDATE_COMMAND_UI(ID_VIEW_NUMWEEKS_9, OnUpdateViewNumWeeks9)
	ON_COMMAND(ID_GOTOTODAY, OnGoToToday)
	ON_UPDATE_COMMAND_UI(ID_GOTOTODAY, OnUpdateGoToToday)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCalendarFrameWnd message handlers

BOOL CCalendarFrameWnd::Create(HWND hParent, BOOL bShow, LPSIZE pSize)
{
	m_hParentOfFrame = hParent;

	CWnd* pParent = CWnd::FromHandle(hParent);

	if (CFrameWnd::LoadFrame(IDR_CALENDAR, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME, pParent))
	{
		if (pSize)
		{
			CRect rWindow;
			GetWindowRect(rWindow);

			rWindow.right = rWindow.left + pSize->cx;
			rWindow.bottom = rWindow.top + pSize->cy;

			MoveWindow(rWindow);
		}

		if (bShow)
		{
			ShowWindow(SW_SHOW);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CCalendarFrameWnd::Show(BOOL bShow)
{
	if (!GetSafeHwnd())
	{
		return FALSE;
	}

	if (bShow)
	{
		if (IsShowing())
		{
			//already shown
		}
		else
		{
			//resize the window
			MoveWindow(m_rcWindowSize);

			//show the window
			if (m_bWindowMaximized)
			{
				ShowWindow(SW_SHOWMAXIMIZED);
			}
			else
			{
				ShowWindow(SW_SHOW);
			}
		}

		//set focus to big calendar, to make sure that it receives keypresses immediately after showing the window
		m_BigCalendar.SetFocus();
	}
	else
	{
		if (!IsShowing())
		{
			//already hidden
			return FALSE;
		}
		else
		{
			//save the window size
			if (IsZoomed())
			{
				m_bWindowMaximized = TRUE;
				ShowWindow(SW_RESTORE);
			}
			else
			{
				m_bWindowMaximized = FALSE;
			}
			GetWindowRect(m_rcWindowSize);

			//hide the window
			ShowWindow(SW_HIDE);
		}
	}

	return TRUE;
}

BOOL CCalendarFrameWnd::IsShowing() const
{
	if (!GetSafeHwnd())
	{
		return FALSE;
	}

	return IsWindowVisible();
}

void CCalendarFrameWnd::Update(const ITaskList* pTasks, DWORD dwFlags)
{
	m_strTasklistName = pTasks->GetProjectName();
	UpdateTitleBarText();

	if (m_pCalendarData->TasklistUpdated(pTasks, dwFlags))
	{
		//notify bigcal that data has been updated
		m_BigCalendar.TasklistUpdated();

		//notify minical that data has been updated
		m_MiniCalendar.TasklistUpdated();
	}
	else
	{
		// TODO
	}
}

void CCalendarFrameWnd::Release()
{
	if (GetSafeHwnd())
	{
		if (IsShowing())
		{
			//need to call this to save the current window position
			Show(FALSE);
		}

		DestroyWindow();
	}

	delete this;
}

void CCalendarFrameWnd::PostNcDestroy()
{
	//DON'T call CFrameWnd::PostNcDestroy, because it will destroy this object
	//CFrameWnd::PostNcDestroy();
}

void CCalendarFrameWnd::OnSetFocus(CWnd* /*pOldWnd*/)
{
	//set focus to big calendar (seems the focus gets lost after switching back to the Calendar window from another app)
	m_BigCalendar.SetFocus();
}

void CCalendarFrameWnd::OnClose()
{
	Show(FALSE);
}

void CCalendarFrameWnd::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	if (m_BigCalendar.GetSafeHwnd())
	{
		ResizeControls(cx, cy);
		m_MiniCalendar.OnSize(nType, cx, cy - GetStatusBarHeight());
	}
}

int CCalendarFrameWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	//set the title of the window
	UpdateTitleBarText();

	//set the pointer to the data object
	m_MiniCalendar.Initialize(this, m_pCalendarData);
	m_BigCalendar.Initialize(this, m_pCalendarData, m_hParentOfFrame);

	//create mini-calendar ctrl
	m_MiniCalendar.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1);

	//create big-calendar ctrl
	m_BigCalendar.Create(WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, 1);

	//create statusbar ctrl
	VERIFY(InitStatusBar());

	return 0;
}

BOOL CCalendarFrameWnd::OnEraseBkgnd(CDC* /*pDC*/)
{
	//no need to erase background - eliminate flicker on resize
	return TRUE;
}

void CCalendarFrameWnd::OnViewMiniCalendar()
{
	m_bShowMiniCalendar = !m_bShowMiniCalendar;

	//show/hide the minicalendar as necessary
	if (m_bShowMiniCalendar)
	{
		m_MiniCalendar.ShowWindow(SW_SHOW);
	}
	else
	{
		m_MiniCalendar.ShowWindow(SW_HIDE);
	}

	//call ResizeControls which will resize both calendars
	CRect rcFrame;
	GetClientRect(rcFrame);
	ResizeControls(rcFrame.Width(), rcFrame.Height());
}
void CCalendarFrameWnd::OnUpdateViewMiniCalendar(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);

	if (m_bShowMiniCalendar)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CCalendarFrameWnd::OnViewStatusBar()
{
	m_bShowStatusBar = !m_bShowStatusBar;

	//show/hide the status-bar as necessary
	if (m_bShowStatusBar)
	{
		m_StatusBar.ShowWindow(SW_SHOW);
	}
	else
	{
		m_StatusBar.ShowWindow(SW_HIDE);
	}

	//call ResizeControls which will resize both calendars
	CRect rcFrame;
	GetClientRect(rcFrame);
	ResizeControls(rcFrame.Width(), rcFrame.Height());
}

void CCalendarFrameWnd::OnUpdateViewStatusBar(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);

	if (m_bShowStatusBar)
	{
		pCmdUI->SetCheck(1);
	}
	else
	{
		pCmdUI->SetCheck(0);
	}
}

void CCalendarFrameWnd::OnViewWeekends()
{
	m_bShowWeekends = !m_bShowWeekends;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();

	//call ResizeControls which will resize both calendars
	CRect rcFrame;
	GetClientRect(rcFrame);
	ResizeControls(rcFrame.Width(), rcFrame.Height());
	Invalidate();
}
void CCalendarFrameWnd::OnUpdateViewWeekends(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_bShowWeekends);
}

void CCalendarFrameWnd::OnViewCompletedTasks()
{
	//show dialog
	CDlgCompletedTasks dlg(m_dwStyleCompletedTasks);
	if (dlg.DoModal() == IDOK)
	{
		m_dwStyleCompletedTasks = dlg.GetFlags();

		//repopulate the calendars
		m_MiniCalendar.Repopulate();
		m_BigCalendar.Repopulate();
	}

	//set focus to big calendar (seems the focus gets lost after the dialog is shown)
	m_BigCalendar.SetFocus();
}
void CCalendarFrameWnd::OnUpdateViewCompletedTasks(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CCalendarFrameWnd::OnViewNumWeeks1()
{
	m_nNumVisibleWeeks = 1;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks1(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 1);
}

void CCalendarFrameWnd::OnViewNumWeeks2()
{
	m_nNumVisibleWeeks = 2;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks2(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 2);
}

void CCalendarFrameWnd::OnViewNumWeeks3()
{
	m_nNumVisibleWeeks = 3;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks3(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 3);
}

void CCalendarFrameWnd::OnViewNumWeeks4()
{
	m_nNumVisibleWeeks = 4;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks4(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 4);
}

void CCalendarFrameWnd::OnViewNumWeeks5()
{
	m_nNumVisibleWeeks = 5;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks5(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 5);
}

void CCalendarFrameWnd::OnViewNumWeeks6()
{
	m_nNumVisibleWeeks = 6;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks6(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 6);
}

void CCalendarFrameWnd::OnViewNumWeeks7()
{
	m_nNumVisibleWeeks = 7;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks7(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 7);
}

void CCalendarFrameWnd::OnViewNumWeeks8()
{
	m_nNumVisibleWeeks = 8;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks8(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 8);
}

void CCalendarFrameWnd::OnViewNumWeeks9()
{
	m_nNumVisibleWeeks = 9;

	//repopulate the calendars
	m_MiniCalendar.Repopulate();
	m_BigCalendar.Repopulate();
}
void CCalendarFrameWnd::OnUpdateViewNumWeeks9(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
	pCmdUI->SetCheck(m_nNumVisibleWeeks == 9);
}

void CCalendarFrameWnd::OnGoToToday()
{
	m_MiniCalendar.SelectToday();
}
void CCalendarFrameWnd::OnUpdateGoToToday(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CCalendarFrameWnd::ResizeControls(int _nxFrame, int _nyFrame)
{
	int nMiniCalWidth = 0;
	if (m_bShowMiniCalendar)
	{
		nMiniCalWidth = g_nMiniCalWidth;
	}

	int nFrameHeightWithoutStatusBar = _nyFrame - GetStatusBarHeight();

	m_MiniCalendar.MoveWindow(0, 0, nMiniCalWidth, nFrameHeightWithoutStatusBar);
	m_BigCalendar.MoveWindow(nMiniCalWidth, 0, _nxFrame - nMiniCalWidth, nFrameHeightWithoutStatusBar);
}

BOOL CCalendarFrameWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	NMHDR* pNotifyArea = (NMHDR*)lParam;
	if (pNotifyArea->code == CALENDAR_MSG_SELECTDATE)
	{
		if (pNotifyArea->hwndFrom == m_MiniCalendar.GetSafeHwnd())
		{
			//send SelectDate message to BigCalendar
			COleDateTime dt;
			m_MiniCalendar.GetSelectedDate(dt);
			m_BigCalendar.SelectDate(dt);

			//set focus to big calendar
			m_BigCalendar.SetFocus();
		}
		else if (pNotifyArea->hwndFrom == m_BigCalendar.GetSafeHwnd())
		{
			//send SelectDate message to MiniCalendar (even if hidden)
			COleDateTime dt;
			m_BigCalendar.GetSelectedDate(dt);
			m_MiniCalendar.SelectDate(dt);
		}
		else
		{
			ASSERT(FALSE);	//who sent it then?
		}

		//selection changed (possibly) - update the status bar with the info of the newly selected item
		UpdateStatusBar();
	}
	else if (pNotifyArea->code == CALENDAR_MSG_SELECTTASK)
	{
		::SendMessage(m_hParentOfFrame, WM_TDCM_TASKLINK, m_BigCalendar.GetSelectedTask(), 0L);
	}
	else if (pNotifyArea->code == CALENDAR_MSG_MOUSEWHEEL_UP)
	{
		m_BigCalendar.OnMouseWheel(0, 1, CPoint(0, 0));
	}
	else if (pNotifyArea->code == CALENDAR_MSG_MOUSEWHEEL_DOWN)
	{
		m_BigCalendar.OnMouseWheel(0, -1, CPoint(0, 0));
	}
	else if (pNotifyArea->code == NM_CLICK)
	{
		if (pNotifyArea->hwndFrom == m_MiniCalendar.GetSafeHwnd())
		{
			//set focus to big calendar
			m_BigCalendar.SetFocus();
		}
		else if (pNotifyArea->hwndFrom == m_BigCalendar.GetSafeHwnd())
		{
			//not bothered
		}
		else if (pNotifyArea->hwndFrom == m_StatusBar.GetSafeHwnd())
		{
			//not bothered
		}
		else
		{
			ASSERT(FALSE);	//who sent it then?
		}
	}
	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

DWORD CCalendarFrameWnd::GetCompletedTasksStyle() const
{
	return m_dwStyleCompletedTasks;
}

int CCalendarFrameWnd::GetNumWeeksToDisplay() const
{
	int nReturn = 6;
	if ((m_nNumVisibleWeeks > 0) && (m_nNumVisibleWeeks < 10))
	{
		nReturn = m_nNumVisibleWeeks;
	}
	else
	{
		ASSERT(FALSE);
	}
	return nReturn;
}

int CCalendarFrameWnd::GetNumDaysToDisplay() const
{
	if (m_bShowWeekends)
	{
		return 7;
	}
	else
	{
		return 5;
	}
}

BOOL CCalendarFrameWnd::IsWeekendsHidden() const
{
	return !m_bShowWeekends;
}

//if date is a weekend and weekends are hidden, returns TRUE
BOOL CCalendarFrameWnd::IsDateHidden(const COleDateTime& _dt) const
{
	if (!m_bShowWeekends)
	{
		int nDayOfWeek = _dt.GetDayOfWeek();
		if ((nDayOfWeek == 1) || (nDayOfWeek == 7)) //7=sat 1=sun
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CCalendarFrameWnd::UpdateTitleBarText()
{
	CString strAppName;
	strAppName.LoadString(IDR_CALENDAR);

	CString strWindowText(strAppName);

	if (!m_strTasklistName.IsEmpty())
	{
		strWindowText.Format(_T("%s - %s"), m_strTasklistName, strAppName);
	}

	SetWindowText(strWindowText);
}

int CCalendarFrameWnd::GetStatusBarHeight() const
{
	int nHeight = 0;
	if (m_bShowStatusBar)
	{
		CRect rcStatusBar;
		m_StatusBar.GetWindowRect(rcStatusBar);
		nHeight = rcStatusBar.Height();
	}
	return nHeight;
}

BOOL CCalendarFrameWnd::InitStatusBar()
{
	static SBACTPANEINFO SB_PANES[] =
	{
		{ ID_SB_STARTDATE,    MAKEINTRESOURCE(ID_SB_STARTDATE_TIP), SBACTF_NORMAL | SBACTF_RESOURCETIP },
		{ ID_SB_DUEDATE,      MAKEINTRESOURCE(ID_SB_DUEDATE_TIP), SBACTF_NORMAL | SBACTF_RESOURCETIP },
		{ ID_SB_DONEDATE,     MAKEINTRESOURCE(ID_SB_DONEDATE_TIP), SBACTF_STRETCHY | SBACTF_RESOURCETIP },
		{ ID_SB_TIMEESTIMATE, MAKEINTRESOURCE(ID_SB_TIMEESTIMATE_TIP), SBACTF_NORMAL | SBACTF_RESOURCETIP },
		{ ID_SB_TIMESPENT,    MAKEINTRESOURCE(ID_SB_TIMESPENT_TIP), SBACTF_NORMAL | SBACTF_RESOURCETIP },
	};

	static int SB_PANECOUNT = sizeof(SB_PANES) / sizeof(SBACTPANEINFO);

	if (!m_StatusBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, IDC_STATUSBAR))
	{
		return -1;
	}

	if (!m_StatusBar.SetPanes(SB_PANES, SB_PANECOUNT))
	{
		return -1;
	}

	return TRUE;
}

void CCalendarFrameWnd::UpdateStatusBar()
{
	if (m_StatusBar.GetSafeHwnd())
	{
		CString strPaneTextStartDate;
		CString strPaneTextDueDate;
		CString strPaneTextDoneDate;
		CString strPaneTextTimeEstimate;
		CString strPaneTextTimeSpent;

		DWORD dwSelectedTaskID = m_BigCalendar.GetSelectedTask();
		if (dwSelectedTaskID != 0)
		{
			COleDateTime date;

			//start date
			if (m_pCalendarData->GetTaskStartDate(dwSelectedTaskID, date))
			{
				CString strDate = CDateHelper::FormatDate(date);
				strPaneTextStartDate.Format(ID_SB_STARTDATE, strDate);
			}

			//due date
			if (m_pCalendarData->GetTaskDueDate(dwSelectedTaskID, date))
			{
				CString strDate = CDateHelper::FormatDate(date);
				strPaneTextDueDate.Format(ID_SB_DUEDATE, strDate);
			}

			//completed date
			if (m_pCalendarData->GetTaskDoneDate(dwSelectedTaskID, date))
			{
				CString strDate = CDateHelper::FormatDate(date);
				strPaneTextDoneDate.Format(ID_SB_DONEDATE, strDate);
			}

			//time estimate
			double dTimeEstimate = 0;
			TCHAR cUnit = 0;
			if (m_pCalendarData->GetTaskTimeEstimate(dwSelectedTaskID, dTimeEstimate, cUnit))
			{
				CString strTime = CTimeHelper().FormatTime(dTimeEstimate, cUnit, 2);
				strPaneTextTimeEstimate.Format(ID_SB_TIMEESTIMATE, strTime, cUnit);
			}

			//time spent
			double dTimeSpent = 0;
			cUnit = 0;
			if (m_pCalendarData->GetTaskTimeSpent(dwSelectedTaskID, dTimeSpent, cUnit))
			{
				CString strTime = CTimeHelper().FormatTime(dTimeSpent, cUnit, 2);
				strPaneTextTimeSpent.Format(ID_SB_TIMESPENT, strTime, cUnit);
			}
		}

		m_StatusBar.SetPaneText(m_StatusBar.CommandToIndex(ID_SB_STARTDATE), strPaneTextStartDate);
		m_StatusBar.SetPaneText(m_StatusBar.CommandToIndex(ID_SB_DUEDATE), strPaneTextDueDate);
		m_StatusBar.SetPaneText(m_StatusBar.CommandToIndex(ID_SB_DONEDATE), strPaneTextDoneDate);
		m_StatusBar.SetPaneText(m_StatusBar.CommandToIndex(ID_SB_TIMEESTIMATE), strPaneTextTimeEstimate);
		m_StatusBar.SetPaneText(m_StatusBar.CommandToIndex(ID_SB_TIMESPENT), strPaneTextTimeSpent);
	}
}
