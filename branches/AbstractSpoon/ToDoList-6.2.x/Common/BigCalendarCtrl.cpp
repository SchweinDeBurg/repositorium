/* PERF__FREZ/PUB
 * ====================================================
 * BigCalendarCtrl.cpp : implementation file
 * Frederic Rezeau
 *
 * Perf'Control Personal Edition calendar
 *
 * If you like it, feel free to use it.
 *
 * www.performlabs.com
 * ====================================================
 * Original file written by Frederic Rezeau (http://www.codeproject.com/KB/miscctrl/CCalendarCtrl.aspx)
 * Rewritten for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
 * Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
 * by demon.code.monkey@googlemail.com
 */

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
// - adjusted #include's paths
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#include "StdAfx.h"
#include "BigCalendarCtrl.h"
#include "BigCalendarTask.h"
#include "../../CodeProject/Source/MemDC.h"
#include "CalendarDefines.h"
#include "CalendarUtils.h"
#include "CalendarData.h"
#include "../CalendarExt/Source/CalendarFrameWnd.h"
#include "../../CodeProject/Source/DateHelper.h"
#include "../../CodeProject/Source/GraphicsMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//fonts
#define BIGCAL_FONT_NAME            _T("MS Shell Dlg 2")
#define BIGCAL_FONT_SIZE            8

//colours
#define COLOUR_TASKS                RGB(0,0,0)
#define COLOUR_DAYNUMBERS           RGB(70,70,70)
#define COLOUR_WHITE                RGB(255,255,255)
#define COLOUR_BACKGROUND           COLOUR_WHITE
#define COLOUR_INVALID_DATE         RGB(255,225,225)
#define COLOUR_IMPORTANTDAY_MARKER  RGB(255,255,100)
#define COLOUR_GRIDLINES            RGB(125,175,255)
#define COLOUR_SELECTED_DAYNUMBER   COLOUR_DAYNUMBERS
#define COLOUR_MULTIPLE_TASK_ARROW  RGB(170,170,170)
#define COLOUR_MONTHNAMES           RGB(0,0,255)


/////////////////////////////////////////////////////////////////////////////
// CBigCalendarCtrl

CBigCalendarCtrl::CBigCalendarCtrl():
m_pFrameWnd(NULL),
m_pCalendarData(NULL),
m_pFont(NULL),
m_pFontBold(NULL),
m_hwndMessageRoutingWindow(NULL),
m_bTracking(FALSE),
m_bMonthIsOdd(FALSE),
m_nFirstWeekDay(-1),
m_nSelectedTaskID(-1),
m_nVscrollMax(100), //this is enough for a two-year range
m_nVscrollPos(50)
{
	m_nFirstWeekDay = CDateHelper::FirstDayOfWeek();

	CCalendarUtils::GetToday(m_dateSelected);   //today's date

	m_pFont = new CFont;
	GraphicsMisc::CreateFont(*m_pFont, BIGCAL_FONT_NAME, BIGCAL_FONT_SIZE);

	m_pFontBold = new CFont;
	GraphicsMisc::CreateFont(*m_pFontBold, BIGCAL_FONT_NAME, BIGCAL_FONT_SIZE, MFS_BOLD);
}

CBigCalendarCtrl::~CBigCalendarCtrl()
{
	for (POSITION pos = m_listCalendarTasks.GetHeadPosition(); pos != NULL;)
	{
		delete(CBigCalendarTask*)(m_listCalendarTasks.GetNext(pos));
	}
	m_listCalendarTasks.RemoveAll();

	m_pFont->DeleteObject();
	m_pFontBold->DeleteObject();
	delete m_pFont;
	delete m_pFontBold;
}

void CBigCalendarCtrl::Initialize(CCalendarFrameWnd* _pFrameWnd, CCalendarData* _pCalendarData,
	HWND _hwndMessageRoutingWindow)
{
	m_pFrameWnd = _pFrameWnd;
	m_pCalendarData = _pCalendarData;
	m_hwndMessageRoutingWindow = _hwndMessageRoutingWindow;
}

BEGIN_MESSAGE_MAP(CBigCalendarCtrl, CWnd)
	//{{AFX_MSG_MAP(CBigCalendarCtrl)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CBigCalendarCtrl::Create(DWORD _dwStyle, const CRect& _rect, CWnd* _pParent, UINT _nID)
{
	BOOL bResult = CWnd::CreateEx(NULL, NULL, NULL, _dwStyle, _rect, _pParent, _nID);

	//this line seems to be needed here, to avoid problems with tooltips only appearing in one listbox
	EnableToolTips();

	SetScrollRange(SB_VERT, 0, m_nVscrollMax, FALSE);

	COleDateTime dtToday;
	CCalendarUtils::GetToday(dtToday);

	if (!CCalendarUtils::IsDateValid(dtToday))
	{
		ASSERT(FALSE);
		return bResult;
	}

	if (dtToday.GetMonth() % 2)
	{
		m_bMonthIsOdd = TRUE;
	}

	if (m_pFrameWnd->IsWeekendsHidden())
	{
		if (dtToday.GetDayOfWeek() == 1)
		{
			//sun
			dtToday -= 2;
		}
		else if (dtToday.GetDayOfWeek() == 7)
		{
			//sat
			dtToday -= 1;
		}
	}

	GotoMonth(dtToday);

	SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);

	CreateTasks();

	return bResult;
}

void CBigCalendarCtrl::Repopulate()
{
	CreateTasks(TRUE);

	TasklistUpdated();

	//ensure that if number of weeks have been changed, current date may not be visible. if so, select it again
	if (!IsDateVisible(m_dateSelected))
	{
		Goto(m_dateSelected, TRUE);
	}
}

void CBigCalendarCtrl::SelectDate(const COleDateTime& _date)
{
	LeaveCell();
	Goto(_date, TRUE);
}

void CBigCalendarCtrl::TasklistUpdated()
{
	//just refresh the data in the view
	COleDateTime dtFirstCell(m_dayCells[0][0].date);   //current first-cell
	RepopulateAllCells(dtFirstCell);

	SendSelectDateMessageToParent(FALSE);
}

void CBigCalendarCtrl::ClickedOnTaskInListbox()
{
	SendSelectDateMessageToParent(TRUE);
}

void CBigCalendarCtrl::SelectTask(int _nTaskListboxID, int _nTaskID)
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

	//unselect all listboxes apart from _nTaskListboxID
	for (int i = 0; i < nNumWeeks; i++)
	{
		for (int u = 0; u < nNumColumns; u++)
		{
			if (m_dayCells[i][u].nListboxID == _nTaskListboxID)
			{
				//store the date of the newly selected task
				m_dateSelected = m_dayCells[i][u].date;
			}
			else
			{
				CBigCalendarTask* pWndTask = GetTaskListboxFromCell(i, u);
				if (pWndTask)
				{
					pWndTask->SetCurSel(-1);
				}
			}
		}
	}

	//store the newly selected task ID
	m_nSelectedTaskID = _nTaskID;

	Invalidate();

	//////////////////////////////////////////////////////////////////////////////////////////
	//select the newly selected task in the active tasklist, by sending a SELECTTASK message
	CWnd* pFocus = GetFocus();

	DWORD dwTaskID = 0;
	CBigCalendarTask* pListbox = GetTaskListboxFromTaskListboxID(_nTaskListboxID);
	if (pListbox)
	{
		dwTaskID = ((SItem*)pListbox->GetItemData(_nTaskID))->dwItemData;
	}

	if (dwTaskID != 0)
	{
		COPYDATASTRUCT cds;
		cds.dwData = 3;   //can't use SELECTTASK because including ToDoListWnd.h would break the build
		cds.cbData = sizeof(DWORD);
		cds.lpData = &dwTaskID;
		::SendMessage(m_hwndMessageRoutingWindow, WM_COPYDATA, NULL, (LPARAM)&cds);
	}
	else
	{
		ASSERT(FALSE);   //task ID not found
	}

	//set focus to the previously focussed window (the active tasklist steals the focus when it gets the SELECTTASK message)
	pFocus->SetFocus();
}

void CBigCalendarCtrl::GetSelectedDate(COleDateTime& _date) const
{
	_date = m_dateSelected;
}

DWORD CBigCalendarCtrl::GetSelectedTask() const
{
	DWORD dwTaskID = 0;

	if (m_nSelectedTaskID != -1)
	{
		CBigCalendarTask* plbTasks = GetTaskListboxFromDate(m_dateSelected);
		if (plbTasks != NULL)
		{
			if (m_nSelectedTaskID < plbTasks->GetCount())
			{
				dwTaskID = ((SItem*)(plbTasks->GetItemData(m_nSelectedTaskID)))->dwItemData;
			}
			else
			{
				ASSERT(FALSE);
			}
		}
	}

	return dwTaskID;
}

BOOL CBigCalendarCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		COleDateTime dtNew = m_dateSelected;
		switch (pMsg->wParam)
		{
		case VK_ESCAPE:
			{
				if (m_pFrameWnd != NULL)
				{
					m_pFrameWnd->Show(FALSE);
					return TRUE;   //no further processing
				}
				break;
			}
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			{
				int nOldScrollPos = m_nVscrollPos;

				int nRow = 0;
				int nCol = 0;
				GetLastSelectedGridCell(nRow, nCol);
				if (!CCalendarUtils::IsDateValid(m_dayCells[nRow][nCol].date))
				{
					ASSERT(FALSE);
					return FALSE;
				}

				int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
				int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

				if (pMsg->wParam == VK_UP)
				{
					//select previous task
					if (m_nSelectedTaskID != -1)
					{
						CBigCalendarTask* plbTasks = GetTaskListboxFromDate(m_dateSelected);
						if (plbTasks != NULL)
						{
							m_nSelectedTaskID--;
							if (m_nSelectedTaskID < 0)
							{
								m_nSelectedTaskID = -1;
							}
							plbTasks->SetCurSel(m_nSelectedTaskID);

							SendSelectDateMessageToParent(TRUE);
						}
					}

					if (m_nSelectedTaskID == -1)
					{
						//move to cell above
						if (nRow == 0)
						{
							CCalendarUtils::SubtractDay(dtNew, 7);
							if (!CCalendarUtils::IsDateValid(dtNew))
							{
								ASSERT(FALSE);
								return FALSE;
							}

							m_nVscrollPos--;
						}
						else
						{
							nRow--;
						}
					}
				}
				else if (pMsg->wParam == VK_DOWN)
				{
					//select next task
					if (m_nSelectedTaskID != -1)
					{
						CBigCalendarTask* plbTasks = GetTaskListboxFromDate(m_dateSelected);
						if (plbTasks != NULL)
						{
							m_nSelectedTaskID++;
							if (m_nSelectedTaskID >= plbTasks->GetCount())
							{
								m_nSelectedTaskID = -1;
							}
							plbTasks->SetCurSel(m_nSelectedTaskID);

							SendSelectDateMessageToParent(TRUE);
						}
					}

					if (m_nSelectedTaskID == -1)
					{
						LeaveCell();

						//move to cell below
						if (nRow == nNumWeeks - 1)
						{
							CCalendarUtils::AddDay(dtNew, 7);
							if (!CCalendarUtils::IsDateValid(dtNew))
							{
								ASSERT(FALSE);
								return FALSE;
							}

							m_nVscrollPos++;
						}
						else
						{
							nRow++;
						}
					}
				}
				else if (pMsg->wParam == VK_LEFT)
				{
					LeaveCell();

					//move to previous cell
					if (nCol > 0)
					{
						nCol--;
					}
					else
					{
						nCol = nNumColumns - 1;
						if (nRow == 0)
						{
							CCalendarUtils::SubtractDay(dtNew, 7);
							if (!CCalendarUtils::IsDateValid(dtNew))
							{
								ASSERT(FALSE);
								return FALSE;
							}

							m_nVscrollPos--;
						}
						else
						{
							nRow--;
						}
					}
				}
				else   //(pMsg->wParam == VK_RIGHT)
				{
					LeaveCell();

					//move to next cell
					if (nCol < nNumColumns - 1)
					{
						nCol++;
					}
					else
					{
						nCol = 0;
						if (nRow == nNumWeeks - 1)
						{
							CCalendarUtils::AddDay(dtNew, 7);
							if (!CCalendarUtils::IsDateValid(dtNew))
							{
								ASSERT(FALSE);
								return FALSE;
							}

							m_nVscrollPos++;
						}
						else
						{
							nRow++;
						}
					}
				}

				if (nOldScrollPos != m_nVscrollPos)   //scroll pos changed - update scrollpos and goto new date
				{
					SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);
					Goto(dtNew);
				}

				if (!CCalendarUtils::IsDateValid(m_dayCells[nRow][nCol].date))
				{
					ASSERT(FALSE);
					return FALSE;
				}

				m_dateSelected = m_dayCells[nRow][nCol].date;
				Invalidate(TRUE);

				if (m_nSelectedTaskID == -1)   //no need if we still have a task selected
				{
					FireNotifySelectDate();
				}

				return TRUE;
			}
		case VK_NEXT:
			{
				LeaveCell();

				CCalendarUtils::AddMonth(m_dateSelected);
				SelectDate(m_dateSelected);

				FireNotifySelectDate();

				break;
			}
		case VK_PRIOR:
			{
				LeaveCell();

				CCalendarUtils::SubtractMonth(m_dateSelected);
				SelectDate(m_dateSelected);

				FireNotifySelectDate();

				break;
			}
		case VK_RETURN:
			{
				if (m_nSelectedTaskID != -1)
				{
					CBigCalendarTask* plbTasks = GetTaskListboxFromDate(m_dateSelected);
					if (plbTasks != NULL)
					{
						SelectTask(plbTasks->GetDlgCtrlID(), m_nSelectedTaskID);
					}
				}
				break;
			}
		default:
			{
				break;
			}
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CBigCalendarCtrl::GetGridCellFromPoint(CPoint _point, int& _nRow,  int& _nCol) const
{
	if (_point.y > CALENDAR_HEADER_HEIGHT)
	{
		int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
		int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

		CRect rc;
		GetClientRect(&rc);
		int nHeight = (rc.Height() - CALENDAR_HEADER_HEIGHT) / nNumWeeks;
		int nWidth = rc.Width() / nNumColumns;
		if (nHeight && nWidth)
		{
			_nRow = (_point.y - CALENDAR_HEADER_HEIGHT) / nHeight;
			_nCol = _point.x / nWidth;
			if ((_nRow >= 0) && (_nRow < nNumWeeks) && (_nCol >= 0) && (_nCol < nNumColumns))
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

BOOL CBigCalendarCtrl::GetRectFromCell(int _nRow, int _nCol, CRect& _rect) const
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

	if (_nRow >= 0 && _nRow < nNumWeeks && _nCol >= 0 && _nCol < nNumColumns)
	{
		CRect rc;
		GetClientRect(&rc);
		int nHeight = (rc.Height() - CALENDAR_HEADER_HEIGHT) / nNumWeeks;
		int nWidth = rc.Width() / nNumColumns;
		_rect.left = nWidth * _nCol;
		_rect.right = _rect.left + nWidth;
		_rect.top = CALENDAR_HEADER_HEIGHT + _nRow * nHeight;
		_rect.bottom = _rect.top + nHeight;
		return TRUE;
	}
	return FALSE;
}

void CBigCalendarCtrl::GetLastSelectedGridCell(int& _nRow, int& _nCol) const
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

	_nRow = 0;
	_nCol = 0;
	for (int i = 0; i < nNumWeeks; i++)
	{
		for (int u = 0; u < nNumColumns; u++)
		{
			if (m_dateSelected == m_dayCells[i][u].date)
			{
				_nRow = i;
				_nCol = u;
				return;
			}
		}
	}
}

CBigCalendarTask* CBigCalendarCtrl::GetTaskListboxFromTaskListboxID(int _nListboxID) const
{
	CBigCalendarTask* pWndTask = (CBigCalendarTask*)GetDlgItem(_nListboxID);
	return pWndTask;
}

CBigCalendarTask* CBigCalendarCtrl::GetTaskListboxFromCell(int _nRow, int _nCol) const
{
	return GetTaskListboxFromTaskListboxID(m_dayCells[_nRow][_nCol].nListboxID);
}

CBigCalendarTask* CBigCalendarCtrl::GetTaskListboxFromDate(const COleDateTime& _date) const
{
	const CBigCalendarCell* pCell = GetCell(_date);
	if (pCell)
	{
		return GetTaskListboxFromTaskListboxID(pCell->nListboxID);
	}
	return NULL;
}

const CBigCalendarCell* CBigCalendarCtrl::GetCell(const COleDateTime& _date) const
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

	for (int i = 0; i < nNumWeeks; i++)
	{
		for (int u = 0; u < nNumColumns; u++)
		{
			if ((m_dayCells[i][u].date) == _date)
			{
				return &m_dayCells[i][u];
			}
		}
	}
	return NULL;
}

//if date is NOT visible, and _pbAfter is NOT NULL, _pbAfter will contain TRUE if the date is AFTER _date
BOOL CBigCalendarCtrl::IsDateVisible(const COleDateTime& _date, BOOL* _pbAfter) const
{
	if (GetCell(_date) == NULL)
	{
		if (_pbAfter != NULL)
		{
			*_pbAfter = FALSE;
			int nLastRow = m_pFrameWnd->GetNumWeeksToDisplay() - 1;
			int nLastCol = m_pFrameWnd->GetNumDaysToDisplay() - 1;
			if (_date > (m_dayCells[nLastRow][nLastCol].date))
			{
				*_pbAfter = TRUE;
			}
		}
		return FALSE;
	}
	else
	{
		return TRUE;
	}
}

COLORREF CBigCalendarCtrl::GetFadedBlue(unsigned char _percent)
{
	int r = 180, g = 205, b = 255;
	int al = _percent * 75 / 100;
	return RGB((unsigned char)(r + ((255 - r) / (float)75) * al),
		(unsigned char)(g + ((255 - g) / (float)75) * al),
		(unsigned char)(b + ((255 - b) / (float)75) * al));
}

void CBigCalendarCtrl::EnterCell()
{
	//select first task in cell, if there is one
	CBigCalendarTask* plbTasks = GetTaskListboxFromDate(m_dateSelected);
	if (plbTasks != NULL)
	{
		if (plbTasks->GetCount() > 0)
		{
			SelectTask(plbTasks->GetDlgCtrlID(), 0);
			plbTasks->SetCurSel(m_nSelectedTaskID);
		}
	}
}

void CBigCalendarCtrl::LeaveCell()
{
	//ensure previously selected task is no longer selected
	CBigCalendarTask* plbTasks = GetTaskListboxFromDate(m_dateSelected);
	if (plbTasks != NULL)
	{
		if (m_nSelectedTaskID != -1)
		{
			m_nSelectedTaskID = -1;
			plbTasks->SetCurSel(m_nSelectedTaskID);
		}
		plbTasks->EnsureTopItemVisible();
	}
}

void CBigCalendarCtrl::GotoMonth(const COleDateTime& _date)
{
	if (!CCalendarUtils::IsDateValid(_date))
	{
		ASSERT(FALSE);
		return;
	}

	COleDateTime dtFirstCell(_date.GetYear(), _date.GetMonth(), 1, 0, 0, 0);

	int narr[7];
	for (int d = 0; d < 7; d++)
	{
		narr[((m_nFirstWeekDay - 1) + d) % 7] = d;
	}
	int nCellStart = narr[dtFirstCell.GetDayOfWeek() - 1];

	dtFirstCell -= nCellStart;

	if (m_pFrameWnd->IsWeekendsHidden())
	{
		int iDayOfWeek = dtFirstCell.GetDayOfWeek();
		int nDiff = abs(m_nFirstWeekDay - iDayOfWeek);
		dtFirstCell -= nDiff;

		if (dtFirstCell.GetDayOfWeek() == 1)
		{
			//sun
			dtFirstCell += 1;
		}
		else if (dtFirstCell.GetDayOfWeek() == 7)
		{
			//sat
			dtFirstCell += 2;
		}
	}

	RepopulateAllCells(dtFirstCell);
}

void CBigCalendarCtrl::Goto(const COleDateTime& _date, BOOL _bSelect)
{
	if (!CCalendarUtils::IsDateValid(_date))
	{
		ASSERT(FALSE);
		return;
	}

	COleDateTime dtNew = _date;
	if (m_pFrameWnd->IsDateHidden(dtNew))   //it was a saturday or sunday
	{
		CCalendarUtils::SubtractDay(dtNew, 1);
		if (m_pFrameWnd->IsDateHidden(dtNew))   //it was a sunday
		{
			CCalendarUtils::SubtractDay(dtNew, 1);
		}
	}

	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

	if (_bSelect)
	{
		m_dateSelected = dtNew;

		// Scrolling pos
		COleDateTime dtToday;
		CCalendarUtils::GetToday(dtToday);
		m_nVscrollPos = (m_nVscrollMax / 2) + (m_dateSelected - dtToday).GetDays() / nNumColumns;
		SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);
	}

	COleDateTime dtFirstCell = COleDateTime(dtNew.GetYear(), dtNew.GetMonth(), dtNew.GetDay(), 0, 0, 0);

	BOOL bBelow = FALSE;
	if (IsDateVisible(dtNew, &bBelow))
	{
		//already visible - keep current 1st cell as the 1st cell
		dtFirstCell = m_dayCells[0][0].date;
	}
	else
	{
		//date not visible. get the date of the first day of the week that contains dtNew
		int narr[7];
		for (int d = 0; d < 7; d++)
		{
			narr[((m_nFirstWeekDay - 1) + d) % 7] = d;
		}
		int nCellStart = narr[dtFirstCell.GetDayOfWeek() - 1];

		CCalendarUtils::SubtractDay(dtFirstCell, nCellStart);

		if (bBelow)
		{
			//need this date on the bottom line. so scroll to a number of weeks before dtFirstCell
			CCalendarUtils::SubtractDay(dtFirstCell, 7 * (nNumWeeks - 1));
		}
	}

	//redraw all cells, populating each one with the new date
	RepopulateAllCells(dtFirstCell);
}

void CBigCalendarCtrl::DrawHeader(CDC* _pDC)
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int i = 0;

	CRect rc;
	GetClientRect(&rc);
	rc.bottom = rc.top + CALENDAR_HEADER_HEIGHT;

	CRect rcLine(rc);
	rcLine.top = rcLine.bottom - 1;
	for (i = 0; i < CALENDAR_HEADER_HEIGHT; i++)
	{
		_pDC->FillSolidRect(rcLine, GetFadedBlue((BYTE)i * 4));
		rcLine.bottom--;
		rcLine.top = rcLine.bottom - 1;
	}

	CStringList listDays;
	CStringList listDaysShort;
	CCalendarUtils::GetWeekdays(listDays, FALSE, m_pFrameWnd->IsWeekendsHidden());
	CCalendarUtils::GetWeekdays(listDaysShort, TRUE, m_pFrameWnd->IsWeekendsHidden());

	CFont* pOldFont = _pDC->SelectObject(m_pFont);
	int nWidth = rc.Width() / nNumColumns;
	POSITION pos;

	BOOL bShort = FALSE;
	for (pos = listDays.GetHeadPosition(); pos;)
	{
		CString strDay = listDays.GetNext(pos);
		CSize szTitle(_pDC->GetTextExtent(strDay));
		if (szTitle.cx > nWidth)
		{
			bShort = TRUE;
			break;
		}
		i++;
	}

	if (bShort)
	{
		//full names too long for columns - overwrite listDays with listDaysShort
		listDays.RemoveAll();
		listDays.AddTail(&listDaysShort);
	}

	i = 0;
	for (pos = listDays.GetHeadPosition(); pos;)
	{
		CString strDay = listDays.GetNext(pos);
		CRect rcText(i * nWidth, 2, (i + 1)*nWidth, CALENDAR_HEADER_HEIGHT);
		_pDC->DrawText(strDay, rcText, DT_CENTER | DT_VCENTER);
		i++;
	}
	_pDC->SelectObject(pOldFont);
}

void CBigCalendarCtrl::DrawGrid(CDC* _pDC)
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();
	int i;

	CRect rc;
	GetClientRect(&rc);                                                 //rect of the BigCalendar
	int nHeight = (rc.Height() - CALENDAR_HEADER_HEIGHT) / nNumWeeks;   //height of each cell
	int nWidth = rc.Width() / nNumColumns;                              //width of each cell

	CPen thinPen(PS_SOLID, 1, COLOUR_GRIDLINES);
	CPen* pOldPen = _pDC->SelectObject(&thinPen);
	for (i = 1; i < nNumColumns; i++)
	{
		_pDC->MoveTo(i * nWidth, CALENDAR_HEADER_HEIGHT);
		_pDC->LineTo(i * nWidth, rc.Height());
	}

	for (i = 0; i < nNumWeeks; i++)
	{
		int nNewPos = (i * nHeight) + CALENDAR_HEADER_HEIGHT;
		_pDC->MoveTo(0, nNewPos);
		_pDC->LineTo(rc.Width(), nNewPos);
	}
	_pDC->SelectObject(pOldPen);
}

void CBigCalendarCtrl::DrawCells(CDC* _pDC)
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

	CRect rc;
	GetClientRect(&rc);   //rect of the BigCalendar

	CFont* pOldFont = _pDC->SelectObject(m_pFont);

	CPen blackPen(PS_SOLID, 1, COLOUR_TASKS);
	CPen* pOldPen = _pDC->SelectObject(&blackPen);

	for (int i = 0; i < nNumWeeks; i++)
	{
		for (int u = 0; u < nNumColumns; u++)
		{
			CRect rect;
			if (GetRectFromCell(i, u, rect))
			{
				if (u == nNumColumns - 1)
				{
					rect.right = rc.right;
				}
				if (i == nNumWeeks - 1)
				{
					rect.bottom = rc.bottom;
				}

				if ((m_bMonthIsOdd && !(m_dayCells[i][u].date.GetMonth() % 2)) ||
					(!m_bMonthIsOdd && (m_dayCells[i][u].date.GetMonth() % 2)))
				{
					CBrush br;
					br.CreateSolidBrush(CALENDAR_LIGHTGREY);
					_pDC->FillRect(&rect , &br);
				}

				BOOL bToday = FALSE;

				COleDateTime dtToday;
				CCalendarUtils::GetToday(dtToday);

				if (dtToday == m_dayCells[i][u].date)
				{
					// Draw the frame
					CRect rcLine(rect);
					rcLine.bottom = rcLine.top + 15;
					rcLine.top = rcLine.bottom - 1;
					for (int c = 0; c < 15; c++)
					{
						_pDC->FillSolidRect(rcLine, GetFadedBlue((BYTE)c * 6));
						rcLine.bottom--;
						rcLine.top = rcLine.bottom - 1;
					}
					bToday = TRUE;
				}

				// Draw the selection
				if (m_dateSelected == m_dayCells[i][u].date)
				{
					CRect selRect(rect);
					CBrush br;
					br.CreateSolidBrush(GetFadedBlue(70));
					if (bToday)
					{
						selRect.top += 15;
					}
					_pDC->FillRect(&selRect, &br);
				}

				// Out of range
				if (!CCalendarUtils::IsDateValid(m_dayCells[i][u].date))
				{
					CRect selRect(rect);
					CBrush br;
					br.CreateSolidBrush(COLOUR_INVALID_DATE);
					_pDC->FillRect(&selRect, &br);
				}

				//draw Important marker
				if (m_dayCells[i][u].arrTasks.GetSize() > 0)
				{
					CBrush br;
					br.CreateSolidBrush(COLOUR_IMPORTANTDAY_MARKER);
					CRect rcMark(rect);
					rcMark.left = rcMark.right - 15;
					rcMark.bottom = rcMark.top + 13;
					_pDC->FillRect(&rcMark, &br);
				}

				// draw inside...
				rect.DeflateRect(1, 1);

				int nDay = m_dayCells[i][u].date.GetDay();

				if (nDay == 1 || (i == 0 && u == 0))
				{
					//draw month name
					CRect rcMonth(rect);
					rcMonth.right -= 15;

					int nMonth = m_dayCells[i][u].date.GetMonth();
					CString csMonth = CDateHelper::GetMonth(nMonth, FALSE);
					CSize dtSize(_pDC->GetTextExtent(csMonth));
					if (dtSize.cx > rcMonth.Width())
					{
						//no room for long version, must use short version (e.g. must use "Dec" instead of "December")
						csMonth = CDateHelper::GetMonth(nMonth, TRUE);
					}

					unsigned long nOldColor = _pDC->SetTextColor(COLOUR_MONTHNAMES);
					_pDC->DrawText(csMonth, rcMonth, DT_RIGHT | DT_TOP);
					_pDC->SetTextColor(nOldColor);
				}

				if (m_dayCells[i][u].arrTasks.GetSize() > 0)
				{
					_pDC->SelectObject(m_pFontBold);
				}
				else
				{
					_pDC->SelectObject(m_pFont);
				}

				//draw day number
				CString strDay;
				strDay.Format(_T("%d"), nDay);
				unsigned long nOldColor = _pDC->SetTextColor(COLOUR_DAYNUMBERS);
				_pDC->DrawText(strDay, rect, DT_RIGHT | DT_TOP);
				_pDC->SetTextColor(nOldColor);
			}
		}
	}

	_pDC->SelectObject(pOldFont);
	_pDC->SelectObject(pOldPen);
}

void CBigCalendarCtrl::FireNotifySelectDate()
{
	EnterCell();
	SendSelectDateMessageToParent(FALSE);
}

void CBigCalendarCtrl::SendSelectDateMessageToParent(BOOL bAndSendSelectTaskIDMsg)
{
	NMHDR NotifyArea;
	NotifyArea.code = CALENDAR_MSG_SELECTDATE;
	NotifyArea.hwndFrom = m_hWnd;
	NotifyArea.idFrom = ::GetDlgCtrlID(m_hWnd);
	GetParent()->SendMessage(WM_NOTIFY, ::GetDlgCtrlID(m_hWnd), (WPARAM)&NotifyArea);

	if (bAndSendSelectTaskIDMsg)
	{
		NotifyArea.code = CALENDAR_MSG_SELECTTASK;
		NotifyArea.hwndFrom = m_hWnd;
		NotifyArea.idFrom = ::GetDlgCtrlID(m_hWnd);
		GetParent()->SendMessage(WM_NOTIFY, ::GetDlgCtrlID(m_hWnd), (WPARAM)&NotifyArea);
	}
}

void CBigCalendarCtrl::ScrollDown(int _nLines)
{
	ASSERT(_nLines > 0);

	COleDateTime dtFirstCell(m_dayCells[0][0].date);   //current first-cell
	CCalendarUtils::AddDay(dtFirstCell, 7 * _nLines);

	if (!CCalendarUtils::IsDateValid(dtFirstCell))
	{
		ASSERT(FALSE);
		return;
	}

	m_nVscrollPos += _nLines;
	RepopulateAllCells(dtFirstCell);

	SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);
}

void CBigCalendarCtrl::ScrollUp(int _nLines)
{
	ASSERT(_nLines > 0);

	COleDateTime dtFirstCell(m_dayCells[0][0].date);	//current first-cell
	CCalendarUtils::SubtractDay(dtFirstCell, 7 * _nLines);

	if (!CCalendarUtils::IsDateValid(dtFirstCell))
	{
		ASSERT(FALSE);
		return;
	}

	m_nVscrollPos -= _nLines;
	RepopulateAllCells(dtFirstCell);

	SetScrollPos(SB_VERT, m_nVscrollPos, TRUE);
}

void CBigCalendarCtrl::CreateTasks(BOOL _bRecreate/*=FALSE*/)
{
	if (_bRecreate)
	{
		for (POSITION pos = m_listCalendarTasks.GetHeadPosition(); pos;)
		{
			CBigCalendarTask* pTaskWnd = (CBigCalendarTask*)m_listCalendarTasks.GetNext(pos);
			pTaskWnd->DestroyWindow();
			delete pTaskWnd;
		}
		m_listCalendarTasks.RemoveAll();
	}

	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();
	DWORD dwStyleCompletedTasks = m_pFrameWnd->GetCompletedTasksStyle();

	int nTaskWindowID = 1234;

	for (int i = 0; i < nNumWeeks; i++)
	{
		for (int u = 0; u < nNumColumns; u++)
		{
			//create task windows
			CBigCalendarTask* pTaskWnd = new CBigCalendarTask(this, dwStyleCompletedTasks);
			m_listCalendarTasks.AddTail(pTaskWnd);

			pTaskWnd->Create(LBS_OWNERDRAWFIXED | LBS_SORT | LBS_HASSTRINGS | WS_VISIBLE,
				CRect(0, 0, 0, 0), this, nTaskWindowID);

			m_dayCells[i][u].nListboxID = nTaskWindowID;
			nTaskWindowID++;
		}
	}
}

void CBigCalendarCtrl::ResizeTasks(BOOL _bRepopulateTasks/*=FALSE*/)
{
	int nNumColumns = m_pFrameWnd->GetNumDaysToDisplay();
	int nNumWeeks = m_pFrameWnd->GetNumWeeksToDisplay();

	for (int i = 0; i < nNumWeeks; i++)
	{
		for (int u = 0; u < nNumColumns; u++)
		{
			CRect rcCell;
			if (GetRectFromCell(i, u, rcCell))
			{
				CBigCalendarTask* pWndTask = GetTaskListboxFromCell(i, u);
				if (pWndTask)
				{
					if (_bRepopulateTasks)
					{
						//remove existing items from all listboxes
						pWndTask->ResetContent();

						//re-add values to all listboxes
						int iNumTasks = m_dayCells[i][u].arrTasks.GetSize();
						for (int iTask = 0; iTask < iNumTasks; iTask++)
						{
							CString strOut = m_dayCells[i][u].arrTasks.GetAt(iTask);
							DWORD dwTaskID = m_dayCells[i][u].arrTaskIDs[iTask];
							BOOL bIsStartTask = m_dayCells[i][u].arrIsStartTask[iTask];
							BOOL bIsCompleteTask = m_dayCells[i][u].arrIsCompleteTask[iTask];
							pWndTask->AddItem(strOut, dwTaskID, bIsStartTask, bIsCompleteTask);
						}

						//does this cell contain any task that should be selected?
						if (m_dayCells[i][u].date == m_dateSelected)
						{
							//re-select task now that it has been redrawn
							pWndTask->SetCurSel(m_nSelectedTaskID);
						}
					}

					int nMaxHeight = rcCell.Height() - 12;
					int nRequiredHeight = pWndTask->GetTotalItemHeight();
					if (nRequiredHeight > nMaxHeight)
					{
						nRequiredHeight = nMaxHeight;
					}

					//resize/move the tasks listbox
					pWndTask->SetWindowPos(NULL, rcCell.left + 3, rcCell.top + 14, rcCell.Width() - 5, nRequiredHeight, 0);
				}
			}
		}
	}
}

//redraws all cells, populating each one with the new date
void CBigCalendarCtrl::RepopulateAllCells(const COleDateTime& _dateFirstCell)
{
	COleDateTime dateCurrent = _dateFirstCell;
	int iFirstCellDayOfWeek = dateCurrent.GetDayOfWeek();

	if (m_pFrameWnd->IsWeekendsHidden())
	{
		if (iFirstCellDayOfWeek == 7)   //sat
		{
			dateCurrent += 2;   //mon
		}
		else if (iFirstCellDayOfWeek == 1)   //sun
		{
			dateCurrent += 1;   //mon
		}
	}
	else
	{
		dateCurrent += (m_nFirstWeekDay - iFirstCellDayOfWeek);
	}

	DWORD dwStyleCompletedTasks = m_pFrameWnd->GetCompletedTasksStyle();

	for (int iRow = 0; iRow < CALENDAR_ROWS; iRow++)
	{
		for (int iCol = 0; iCol < CALENDAR_COLUMNS; iCol++)
		{
			// Init the cell
			m_dayCells[iRow][iCol].date = dateCurrent;
			m_dayCells[iRow][iCol].arrTasks.RemoveAll();
			m_dayCells[iRow][iCol].arrTaskIDs.RemoveAll();
			m_dayCells[iRow][iCol].arrIsStartTask.RemoveAll();
			m_dayCells[iRow][iCol].arrIsDueTask.RemoveAll();
			m_dayCells[iRow][iCol].arrIsCompleteTask.RemoveAll();

			if (m_pCalendarData->IsImportantDate(dateCurrent))
			{
				//this date is special - get the tasks for this date
				CTaskInfoList listTasks;
				m_pCalendarData->GetTasks(dateCurrent, listTasks);
				ASSERT(!listTasks.IsEmpty());

				//add the tasks in listTasks to this date's cell
				CBigCalendarCell* pCell = (CBigCalendarCell*)GetCell(dateCurrent);   //frig to de-const the cell
				if (pCell)
				{
					for (POSITION pos = listTasks.GetHeadPosition(); pos != NULL;)
					{
						CTaskInfo ti = listTasks.GetNext(pos);
						if (!(dwStyleCompletedTasks & COMPLETEDTASKS_HIDDEN) || (!ti.IsComplete()))
						{
							pCell->arrTasks.Add(ti.GetTaskTitle());
							pCell->arrTaskIDs.Add(ti.GetTaskID());
							pCell->arrIsStartTask.Add(ti.IsStart());
							pCell->arrIsDueTask.Add(ti.IsDue());
							pCell->arrIsCompleteTask.Add(ti.IsComplete());
						}
					}
				}
			}

			CCalendarUtils::AddDay(dateCurrent);
		}
	}

	ResizeTasks(TRUE);

	Invalidate();

	if (m_pFrameWnd->IsWeekendsHidden())
	{
		if (m_dateSelected.GetDayOfWeek() == 7)
		{
			//sat
			m_dateSelected -= 1;
			SelectDate(m_dateSelected);
			FireNotifySelectDate();
		}
		else if (m_dateSelected.GetDayOfWeek() == 1)
		{
			//sun
			m_dateSelected -= 2;
			SelectDate(m_dateSelected);
			FireNotifySelectDate();
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CBigCalendarCtrl message handlers

void CBigCalendarCtrl::OnPaint()
{
	ResizeTasks();

	CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);

	CDC MemDC;
	MemDC.CreateCompatibleDC(&dc);
	CBitmap MemBitmap;
	MemBitmap.CreateCompatibleBitmap(&dc, rc.Width(), rc.Height());
	CBitmap* pOldBitmap = MemDC.SelectObject(&MemBitmap);

	CBrush brBkGnd;
	brBkGnd.CreateSolidBrush(COLOUR_BACKGROUND);
	MemDC.FillRect(&rc , &brBkGnd);
	MemDC.SetBkMode(TRANSPARENT);

	// Draw calendar elements
	DrawHeader(&MemDC);
	DrawCells(&MemDC);
	DrawGrid(&MemDC);

	// Render
	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &MemDC, 0, 0, SRCCOPY);
	MemDC.SelectObject(pOldBitmap);
}

void CBigCalendarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	int nRow = 0;
	int nCol = 0;
	if (GetGridCellFromPoint(point, nRow, nCol))
	{
		if (!CCalendarUtils::IsDateValid(m_dayCells[nRow][nCol].date))
		{
			ASSERT(FALSE);
			return;
		}

		if (m_dateSelected != m_dayCells[nRow][nCol].date)
		{
			LeaveCell();

			m_bTracking = TRUE;
			m_dateSelected = m_dayCells[nRow][nCol].date;
			SetCapture();
			Invalidate(TRUE);

			FireNotifySelectDate();
		}
	}

	SetFocus();
	CWnd::OnLButtonDown(nFlags, point);
}

void CBigCalendarCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		m_bTracking = FALSE;

		int nRow = 0;
		int nCol = 0;
		if (GetGridCellFromPoint(point, nRow, nCol))
		{
			if (!CCalendarUtils::IsDateValid(m_dayCells[nRow][nCol].date))
			{
				ASSERT(FALSE);
				return;
			}

			m_dateSelected = m_dayCells[nRow][nCol].date;
		}
		ReleaseCapture();
	}

	Invalidate(TRUE);
	CWnd::OnLButtonUp(nFlags, point);
}

void CBigCalendarCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		int nRow = 0;
		int nCol = 0;
		if (GetGridCellFromPoint(point, nRow, nCol))
		{
			if (!CCalendarUtils::IsDateValid(m_dayCells[nRow][nCol].date))
			{
				ASSERT(FALSE);
				return;
			}

			if (m_dateSelected != m_dayCells[nRow][nCol].date)
			{
				LeaveCell();

				m_dateSelected = m_dayCells[nRow][nCol].date;
				Invalidate(TRUE);

				FireNotifySelectDate();
			}
		}
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CBigCalendarCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nLines = 0;
	switch (nSBCode)
	{
	case SB_TOP:
	case SB_LINEUP:
	case SB_PAGEUP:
		nLines = -1;
		break;
	case SB_BOTTOM:
	case SB_LINEDOWN:
	case SB_PAGEDOWN:
		nLines = 1;
		break;
	case SB_THUMBTRACK:
		nLines = nPos - m_nVscrollPos;
		break;
	default:
		nLines = 0;
	}

	if (nLines > 0)
	{
		ScrollDown(nLines);
	}
	else if (nLines < 0)
	{
		ScrollUp(abs(nLines));
	}

	SetFocus();
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CBigCalendarCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta < 0)
	{
		SendMessage(WM_VSCROLL, SB_LINEDOWN);
	}
	else if (zDelta > 0)
	{
		SendMessage(WM_VSCROLL, SB_LINEUP);
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
