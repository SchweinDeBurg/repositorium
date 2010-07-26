// CMiniCalendarCtrl
// Author:  Matt Gullett
//          gullettm@yahoo.com
// Copyright (c) 2001
//
// This is a user-interface componenet similar to the MS outlook mini
// calendar control.  (The one used in date picker control and the
// appointment (day view)).
//
// You may freely use this source code in personal, freeware, shareware
// or commercial applications provided that 1) my name is recognized in the
// code and if this code represents a substantial portion of the application
// that my name be included in the credits for the application (about box, etc)
//
// Use this code at your own risk.  This code is provided AS-IS.  No warranty
// is granted as to the correctness, usefulness or value of this code.
//
// Special thanks to Keith Rule for the CMemDC class
// http://www.codeproject.com/KB/GDI/flickerfree.aspx
//
// Original file written by Matt Gullett (http://www.codeproject.com/KB/miscctrl/minicalendar.aspx)
// Rewritten for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com
//
//////////////////////////////////////////////////////////////////////

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - taken out from the original TDL_Calendar package for better sharing
// - adjusted #include's paths
// - slightly reformatted source code
//*****************************************************************************

#include "StdAfx.h"
#include "../CalendarExt/Source/CalendarExtResource.h"
#include "CalendarData.h"
#include "CalendarDefines.h"
#include "CalendarUtils.h"
#include "MiniCalendarCtrl.h"
#include "../../CodeProject/Source/memdc.h"
#include "MiniCalendarMonthPicker.h"
#include <locale.h>
#include "../CalendarExt/Source/CalendarFrameWnd.h"
#include "../../CodeProject/Source/DateHelper.h"
#include "../../CodeProject/Source/GraphicsMisc.h"
#include "../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//fonts
#define MINICAL_FONT_NAME                       _T("MS Shell Dlg 2")
#define MINICAL_FONT_SIZE                       8

#define MINICAL_NUM_CELLS                       42

#define MINICAL_HEADER_TIMER_ID                 1001
#define MINICAL_HEADER_TIMER_INTERVAL           30

#define MINICALENDAR_HEIGHT                     133

#define MINICAL_COLOUR_HEADERFONT               RGB(40,40,255)
#define MINICAL_COLOUR_DAY                      RGB(0,0,0)
#define MINICAL_COLOUR_DAY_DIFFERENTMONTH       RGB(130,130,130)
#define MINICAL_COLOUR_HIDDEN_WEEKEND_DAY       RGB(190,190,190)
#define MINICAL_COLOUR_IMPORTANTDAY_BACKGROUND  RGB(255,255,100)


/////////////////////////////////////////////////////////////////////////////
// CMiniCalendarCtrl
CMiniCalendarCtrl::CMiniCalendarCtrl():
m_bSizeComputed(FALSE),
m_bTracking(FALSE),
m_bHeaderTracking(FALSE),
m_pCalendarData(NULL),
m_pFrameWnd(NULL),
m_pMonthPicker(NULL),
m_pHeaderCell(NULL),
m_parCells(NULL),
m_pFont(NULL),
m_pFontBold(NULL),
m_iHeaderTimerID(0),
m_iRows(0),
m_iFirstDayOfWeek(0),
m_iHeaderHeight(0),
m_iIndividualDayWidth(0),
m_iDaysOfWeekHeight(0),
m_iDaysHeight(0),
m_iCurrentMonth(0),
m_iCurrentYear(0),
m_cBackColor(0)
{
	m_cBackColor = ::GetSysColor(COLOR_WINDOW);

	m_szMonthSize = CSize(0, 0);

	m_rectScrollLeft.SetRectEmpty();
	m_rectScrollRight.SetRectEmpty();

	CCalendarUtils::GetToday(m_dateSelected);

	m_iCurrentMonth = m_dateSelected.GetMonth();
	m_iCurrentYear = m_dateSelected.GetYear();
	ASSERT((m_iCurrentYear >= 100) && (m_iCurrentYear <= 9999)); //From MSDN: The COleDateTime class handles
	ASSERT((m_iCurrentMonth >= 1) && (m_iCurrentMonth <= 12));   //dates from 1 January 100 to 31 December 9999.

	SetRows(1);

	// set month names
	for (int i = 0; i < 12; i++)
	{
		CString strMonthName = CDateHelper::GetMonth(i + 1, FALSE);
		m_arrMonthNames[i] = strMonthName;
	}

	// set days of week names
	SetFirstDayOfWeek(CDateHelper::FirstDayOfWeek());

	m_pFont = new CFont;
	GraphicsMisc::CreateFont(*m_pFont, MINICAL_FONT_NAME, MINICAL_FONT_SIZE);

	m_pFontBold = new CFont;
	GraphicsMisc::CreateFont(*m_pFontBold, MINICAL_FONT_NAME, MINICAL_FONT_SIZE, MFS_BOLD);

	//load left/right arrow bitmaps
	CBitmap bmpL, bmpR;
	bmpL.LoadBitmap(IDB_MINICAL_ARROW_L);
	bmpR.LoadBitmap(IDB_MINICAL_ARROW_R);
	BITMAP BM;
	bmpL.GetBitmap(&BM);
	m_imglistArrows.Create(BM.bmWidth, BM.bmHeight, ILC_COLOR24 | ILC_MASK, 2, 0);
	m_imglistArrows.Add(&bmpL, RGB(255, 0, 255));
	m_imglistArrows.Add(&bmpR, RGB(255, 0, 255));
}

CMiniCalendarCtrl::~CMiniCalendarCtrl()
{
	ClearHotSpots();

	if (m_parCells)
	{
		delete[] m_parCells;
	}

	if (m_pMonthPicker)
	{
		m_pMonthPicker->DestroyWindow();
		delete m_pMonthPicker;
	}

	if (m_iHeaderTimerID != 0)
	{
		KillTimer(m_iHeaderTimerID);
	}

	m_pFont->DeleteObject();
	m_pFontBold->DeleteObject();
	delete m_pFont;
	delete m_pFontBold;
}

void CMiniCalendarCtrl::Initialize(CCalendarFrameWnd* _pFrameWnd, CCalendarData* _pCalendarData)
{
	m_pCalendarData = _pCalendarData;
	m_pFrameWnd = _pFrameWnd;
}

BEGIN_MESSAGE_MAP(CMiniCalendarCtrl, CWnd)
	//{{AFX_MSG_MAP(CMiniCalendarCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_DEVMODECHANGE()
	ON_WM_FONTCHANGE()
	ON_WM_PALETTECHANGED()
	ON_WM_SETTINGCHANGE()
	ON_WM_TIMECHANGE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CMiniCalendarCtrl::GetSelectedDate(COleDateTime& _date) const
{
	_date = m_dateSelected;
}

CString CMiniCalendarCtrl::GetMonthName(int _iMonth) const
{
	if (_iMonth > 0 && _iMonth <= 12)
	{
		return m_arrMonthNames[_iMonth-1];
	}

	ASSERT(FALSE);
	return _T("");
}

CFont* CMiniCalendarCtrl::GetDaysFont()
{
	return m_pFont;
}

void CMiniCalendarCtrl::Repopulate()
{
	TasklistUpdated();
}

//returns TRUE if the new date set is different from the previous one
BOOL CMiniCalendarCtrl::SelectDate(const COleDateTime& _date)
{
	BOOL bReturn = FALSE;
	if (_date.GetStatus() == COleDateTime::valid)
	{
		if (m_dateSelected != _date)
		{
			bReturn = TRUE;
			m_dateSelected = _date;
		}
	}
	else
	{
		m_dateSelected.SetStatus(COleDateTime::invalid);
	}

	if (bReturn)
	{
		Invalidate(TRUE);
	}

	if (!IsDateVisible(m_dateSelected))
	{
		m_iCurrentYear = m_dateSelected.GetYear();
		m_iCurrentMonth = m_dateSelected.GetMonth();
		RedrawWindow();
	}

	return bReturn;
}

void CMiniCalendarCtrl::TasklistUpdated()
{
	//just refresh the data in the view
	Invalidate();
}

BOOL CMiniCalendarCtrl::PreTranslateMessage(MSG* pMsg)
{
	if (m_bHeaderTracking && m_pMonthPicker)
	{
		m_pMonthPicker->ForwardMessage(pMsg);
	}

	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CMiniCalendarCtrl::IsToday(const COleDateTime& _date) const
{
	BOOL bReturn = FALSE;

	COleDateTime dtToday;
	CCalendarUtils::GetToday(dtToday);

	if (_date == dtToday)
	{
		bReturn = TRUE;
	}

	return bReturn;
}

BOOL CMiniCalendarCtrl::IsDateSelected(const COleDateTime& _date) const
{
	BOOL bReturn = FALSE;

	if (m_dateSelected.GetStatus() == COleDateTime::valid)
	{
		if (_date == m_dateSelected)
		{
			bReturn = TRUE;
		}
	}

	return bReturn;
}

BOOL CMiniCalendarCtrl::IsDateVisible(const COleDateTime& _date) const
{
	for (int iRow = 0; iRow < m_iRows; iRow++)
	{
		for (int iDay = 0; iDay < MINICAL_NUM_CELLS; iDay++)
		{
			if (_date == m_parCells[iRow].GetHotSpot(iDay)->m_date)
			{
				return TRUE;
			}
		}
	}
	return FALSE;
}

//1st day = 1-sun, return true for 1-sun and 7-sat
//1st day = 2-mon, return true for 7-sun and 6-sat
//1st day = 3-tue, return true for 6-sun and 5-sat
//etc
BOOL CMiniCalendarCtrl::IsWeekendDay(int _iDayOfWeek) const
{
	int nSat = 8 - m_iFirstDayOfWeek;
	int nSun = nSat + 1;
	if (nSun > 7)
	{
		nSun = 1;
	}

	if ((_iDayOfWeek == nSat) || (_iDayOfWeek == nSun))
	{
		return TRUE;
	}
	return FALSE;
}

void CMiniCalendarCtrl::AllocateCells()
{
	if (m_iRows > 0)
	{
		if (m_parCells)
		{
			delete[] m_parCells;
		}
		m_parCells = NULL;

		m_parCells = new CMiniCalendarCtrlCell[m_iRows];

		ASSERT(m_parCells);
		if (!m_parCells)
		{
			throw(new CMemoryException());
		}

		m_parCells->SetRectEmpty();
	}
}

// clear all hot-spots
void CMiniCalendarCtrl::ClearHotSpots()
{
	for (int iRow = 0; iRow < m_iRows; iRow++)
	{
		for (int iDay = 0; iDay < MINICAL_NUM_CELLS; iDay++)
		{
			m_parCells[iRow].GetHotSpot(iDay)->m_date.SetStatus(COleDateTime::valid);
			m_parCells[iRow].GetHotSpot(iDay)->m_rect.SetRectEmpty();
		}
	}
}

// determine size of a given cell based on actual font settings
CSize CMiniCalendarCtrl::ComputeSize()
{
	const int iXDaysSpaceFromBorder = 15;
	const int iHeaderSpaceForBorder = 15;
	const int iHeaderYSpaceForBorder = 3;
	const int iDaysXSpacing = 5;
	const int iDaysYSpacing = 2;

	CSize szReturn(0, 0);
	int iWidthByDays = 0;
	int iWidthByDaysOfWeekIndividual = 0;
	int iWidthByHeader = 0;
	int iHeaderHeight = 0;
	int iDaysHeight = 0;

	// allocate a DC to use when testing sizes, etc
	CClientDC* pDC = NULL;
	if (::IsWindow(GetSafeHwnd()))
	{
		pDC = new CClientDC(this);
	}
	else
	{
		pDC = new CClientDC(AfxGetMainWnd());
	}
	ASSERT(pDC);

	if (!pDC)
	{
		throw(new CMemoryException());
	}

	// get current font and save for later restoration
	CFont* pOldFont = pDC->GetCurrentFont();

	// first, use days to determine width
	// NOTE: typically, most fonts use the same pixel sizes for all numbers,
	// but this is not mandatory and many "fancy" fonts change this.  To deal
	// with this, I am calculating the width of all possible dates the control will display
	pDC->SelectObject(m_pFont);
	int iX = 1;
	for (; iX <= 31; iX++)
	{
		CString strTemp = _T("00");
		strTemp += CCalendarUtils::LongToString(iX);
		strTemp = strTemp.Right(2);

		CSize szTemp = pDC->GetTextExtent(strTemp);
		if (szTemp.cx > iWidthByDays)
		{
			iWidthByDays = szTemp.cx;
		}
		if (szTemp.cy > iDaysHeight)
		{
			iDaysHeight = szTemp.cy;
		}
	}

	// make sure we also try it with the special days font
	pDC->SelectObject(m_pFontBold);
	for (iX = 1; iX <= 31; iX++)
	{
		CString strTemp = _T("00");
		strTemp += CCalendarUtils::LongToString(iX);
		strTemp = strTemp.Right(2);

		CSize szTemp = pDC->GetTextExtent(strTemp);
		if (szTemp.cx > iWidthByDays)
		{
			iWidthByDays = szTemp.cx;
		}
		if (szTemp.cy > iDaysHeight)
		{
			iDaysHeight = szTemp.cy;
		}
	}

	// finish computation
	m_iDaysHeight = iDaysHeight;
	m_iIndividualDayWidth = iWidthByDays;

	iWidthByDays = (iWidthByDays * 7) + (iDaysXSpacing * 6) + (iXDaysSpaceFromBorder * 2);
	iDaysHeight = (iDaysHeight * 6) + (iDaysYSpacing * 6);

	// next use days of week to determine width and height. here again, we test each variant
	pDC->SelectObject(m_pFont);

	int iWidthByDaysOfWeek = pDC->GetTextExtent(_T("S")).cx;
	if (pDC->GetTextExtent(_T("M")).cx > iWidthByDaysOfWeek)
	{
		iWidthByDaysOfWeek = pDC->GetTextExtent(_T("M")).cx;
	}
	if (pDC->GetTextExtent(_T("T")).cx > iWidthByDaysOfWeek)
	{
		iWidthByDaysOfWeek = pDC->GetTextExtent(_T("T")).cx;
	}
	if (pDC->GetTextExtent(_T("W")).cx > iWidthByDaysOfWeek)
	{
		iWidthByDaysOfWeek = pDC->GetTextExtent(_T("W")).cx;
	}
	if (pDC->GetTextExtent(_T("F")).cx > iWidthByDaysOfWeek)
	{
		iWidthByDaysOfWeek = pDC->GetTextExtent(_T("F")).cx;
	}

	int iDaysOfWeekHeight = pDC->GetTextExtent(_T("S")).cy;
	if (pDC->GetTextExtent(_T("M")).cy > iDaysOfWeekHeight)
	{
		iDaysOfWeekHeight = pDC->GetTextExtent(_T("M")).cy;
	}
	if (pDC->GetTextExtent(_T("T")).cy > iDaysOfWeekHeight)
	{
		iDaysOfWeekHeight = pDC->GetTextExtent(_T("T")).cy;
	}
	if (pDC->GetTextExtent(_T("W")).cy > iDaysOfWeekHeight)
	{
		iDaysOfWeekHeight = pDC->GetTextExtent(_T("W")).cy;
	}
	if (pDC->GetTextExtent(_T("F")).cy > iDaysOfWeekHeight)
	{
		iDaysOfWeekHeight = pDC->GetTextExtent(_T("F")).cy;
	}

	// finish computation
	iWidthByDaysOfWeekIndividual = iWidthByDaysOfWeek;
	iWidthByDaysOfWeek = (iWidthByDaysOfWeek * 7) + (iDaysXSpacing * 6) + (iXDaysSpaceFromBorder * 2);

	if (iWidthByDaysOfWeekIndividual > m_iIndividualDayWidth)
	{
		m_iIndividualDayWidth = iWidthByDaysOfWeekIndividual;
	}

	m_iDaysOfWeekHeight = iDaysOfWeekHeight;

	// next compute width and height of header (month name and year)
	// again, because of font variations we will use a 20 year window and
	// attempt the calculation with every month name
	pDC->SelectObject(m_pFontBold);
	for (int iYear = m_iCurrentYear - 10; iYear <= m_iCurrentYear + 10; iYear++)
	{
		for (int iMonth = 1; iMonth <= 12; iMonth++)
		{
			CString strTest = GetMonthName(iMonth);
			strTest += _T(" ");
			strTest += CCalendarUtils::LongToString(iYear);

			if (pDC->GetTextExtent(strTest).cx > iWidthByHeader)
			{
				iWidthByHeader = pDC->GetTextExtent(strTest).cx;
			}
			if (pDC->GetTextExtent(strTest).cy > iHeaderHeight)
			{
				iHeaderHeight = pDC->GetTextExtent(strTest).cy;
			}
		}
	}

	// finish computation
	iWidthByHeader += (iHeaderSpaceForBorder * 2);
	iHeaderHeight += (iHeaderYSpaceForBorder * 2);
	m_iHeaderHeight = iHeaderHeight;

	// cleanup DC
	pDC->SelectObject(pOldFont);

	// destroy DC
	delete pDC;

	// NOW, adjust return size if needed
	// start with widths (these are easy at this point)
	int iTotalWidth = iWidthByHeader;
	if (iWidthByDaysOfWeek > iTotalWidth)
	{
		iTotalWidth = iWidthByDaysOfWeek;
	}
	if (iWidthByDays > iTotalWidth)
	{
		iTotalWidth = iWidthByDays;
	}
	if (iTotalWidth > szReturn.cx)
	{
		szReturn.cx = iTotalWidth;
	}

	// adjust heights
	int iTotalHeight = iHeaderHeight + iDaysOfWeekHeight + iDaysHeight + 12;
	if (iTotalHeight > szReturn.cy)
	{
		szReturn.cy = iTotalHeight;
	}

	m_szMonthSize = szReturn;
	m_bSizeComputed = TRUE;

	return szReturn;
}

// determine total size of control (all rows/cols + border)
CSize CMiniCalendarCtrl::ComputeTotalSize()
{
	CSize size = ComputeSize();
	size.cy *= m_iRows;
	return size;
}

void CMiniCalendarCtrl::FireNotifySelectDate()
{
	NMHDR NotifyArea;
	NotifyArea.code = CALENDAR_MSG_SELECTDATE;
	NotifyArea.hwndFrom = m_hWnd;
	NotifyArea.idFrom = ::GetDlgCtrlID(m_hWnd);
	GetParent()->SendMessage(WM_NOTIFY, ::GetDlgCtrlID(m_hWnd), (WPARAM)&NotifyArea);
}

void CMiniCalendarCtrl::FireNotifyClick()
{
	NMHDR NotifyArea;
	NotifyArea.code = NM_CLICK;
	NotifyArea.hwndFrom = m_hWnd;
	NotifyArea.idFrom = ::GetDlgCtrlID(m_hWnd);
	GetParent()->SendMessage(WM_NOTIFY, ::GetDlgCtrlID(m_hWnd), (WPARAM)&NotifyArea);
}

void CMiniCalendarCtrl::FireNotifyMouseWheel(BOOL _bScrollUp)
{
	NMHDR NotifyArea;
	NotifyArea.code = _bScrollUp ? CALENDAR_MSG_MOUSEWHEEL_UP : CALENDAR_MSG_MOUSEWHEEL_DOWN;
	NotifyArea.hwndFrom = m_hWnd;
	NotifyArea.idFrom = ::GetDlgCtrlID(m_hWnd);
	GetParent()->SendMessage(WM_NOTIFY, ::GetDlgCtrlID(m_hWnd), (WPARAM)&NotifyArea);
}

void CMiniCalendarCtrl::SelectToday()
{
	CCalendarUtils::GetToday(m_dateSelected);

	m_iCurrentMonth = m_dateSelected.GetMonth();
	m_iCurrentYear = m_dateSelected.GetYear();

	if (m_pFrameWnd->IsWeekendsHidden())
	{
		if (m_dateSelected.GetDayOfWeek() == 7)
		{
			//sat
			m_dateSelected -= 1;
		}
		else if (m_dateSelected.GetDayOfWeek() == 1)
		{
			//sun
			m_dateSelected -= 2;
		}
	}

	RedrawWindow();

	FireNotifySelectDate();
}

void CMiniCalendarCtrl::ScrollLeft()
{
	ClearHotSpots();

	m_iCurrentMonth--;
	if (m_iCurrentMonth < 1)
	{
		m_iCurrentMonth = 12;
		m_iCurrentYear--;
	}
	RedrawWindow();
}

void CMiniCalendarCtrl::ScrollRight()
{
	ClearHotSpots();

	m_iCurrentMonth++;
	if (m_iCurrentMonth > 12)
	{
		m_iCurrentMonth = 1;
		m_iCurrentYear++;
	}
	RedrawWindow();
}

void CMiniCalendarCtrl::SetCellHeaderPosition(int _iMonthRow,
      const CRect& _rect)
{
	int iRow = _iMonthRow - 1;

	ASSERT(_iMonthRow >= 0);
	ASSERT(_iMonthRow <= m_iRows);
	ASSERT(iRow >= 0);

	if ((iRow >= 0) && (iRow < m_iRows))
	{
		m_parCells[iRow].SetHeaderRect(_rect);
		m_parCells[iRow].SetRow(_iMonthRow);
		m_parCells[iRow].SetCol(1);
	}
}

void CMiniCalendarCtrl::SetCellPosition(int _iMonthRow, const CRect& _rect)
{
	int iRow = _iMonthRow - 1;

	ASSERT(_iMonthRow >= 0);
	ASSERT(_iMonthRow <= m_iRows);
	ASSERT(iRow >= 0);

	if ((iRow >= 0) && (iRow < m_iRows))
	{
		m_parCells[iRow].SetRect(_rect);
	}
}

void CMiniCalendarCtrl::SetFirstDayOfWeek(int _iDayOfWeek)
{
	if (_iDayOfWeek > 0 && _iDayOfWeek <= 7)
	{
		m_iFirstDayOfWeek = _iDayOfWeek;

		COleDateTime dtTemp;
		CCalendarUtils::GetToday(dtTemp);

		// find the specified day of the week
		while (dtTemp.GetDayOfWeek() != _iDayOfWeek)
		{
			dtTemp += 1;
		}

		for (int iX = 0; iX < 7; iX++)
		{
			CString strName = CCalendarUtils::GetWeekday(iX, 1);

			if (!Misc::IsMultibyteString(strName))
			{
				//not multibyte - only want first character of this string
				strName = strName.Left(1);
			}

			m_arrShortDayOfWeekNames[iX] = strName;
			dtTemp += 1;
		}
	}
	else
	{
		ASSERT(FALSE);
	}
}

void CMiniCalendarCtrl::SetHotSpot(int _iMonthRow, int _iDayCounter, const COleDateTime& _date, const CRect& _rect)
{
	int iRow = _iMonthRow - 1;

	ASSERT(_iMonthRow >= 0);
	ASSERT(_iMonthRow <= m_iRows);
	ASSERT(iRow >= 0);
	ASSERT((_iDayCounter >= 0) && (_iDayCounter < MINICAL_NUM_CELLS));

	if ((iRow >= 0) && (iRow < m_iRows) && (_iDayCounter >= 0) && (_iDayCounter < MINICAL_NUM_CELLS))
	{
		m_parCells[iRow].GetHotSpot(_iDayCounter)->m_date = _date;
		m_parCells[iRow].GetHotSpot(_iDayCounter)->m_rect = _rect;
	}
}

void CMiniCalendarCtrl::SetRows(int _iRows)
{
	if (_iRows != m_iRows)
	{
		if ((_iRows > 0) && (_iRows < 100))
		{
			m_iRows = _iRows;
			AllocateCells();

			if ((m_hWnd != NULL) && (m_dateSelected.GetStatus() != COleDateTime::valid))
			{
				//first time SetRows is called after window is shown. make today's date selected
				SelectToday();
			}
		}
		else
		{
			ASSERT(FALSE);
		}
	}
}

// draw the month/year header (ie. January 2000)
int CMiniCalendarCtrl::DrawHeader(CDC& _dc, int _iY, int _iLeftX, int _iRow, int _iMonth, int _iYear)
{
	CString strText = GetMonthName(_iMonth);
	strText += " ";
	strText += CCalendarUtils::LongToString(_iYear);

	_dc.FillSolidRect(_iLeftX, _iY, m_szMonthSize.cx - 1, m_iHeaderHeight, ::GetSysColor(COLOR_BTNFACE));
	_dc.Draw3dRect(_iLeftX, _iY, m_szMonthSize.cx - 1, m_iHeaderHeight, m_cBackColor, ::GetSysColor(COLOR_3DSHADOW));

	CFont* pOldFont = _dc.SelectObject(m_pFontBold);
	_dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	_dc.SetTextColor(MINICAL_COLOUR_HEADERFONT);
	_dc.SetBkMode(TRANSPARENT);

	_dc.DrawText(strText, CRect(_iLeftX + 1, _iY + 1, _iLeftX + m_szMonthSize.cx - 2, _iY + m_iHeaderHeight - 2), DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	_dc.SelectObject(pOldFont);

	SetCellHeaderPosition(_iRow, CRect(_iLeftX + 30, _iY + 1, _iLeftX + m_szMonthSize.cx - 30, _iY + m_iHeaderHeight - 2));

	if (_iRow == 1)
	{
		//draw Left and Right scroll arrows (only on top row)
		IMAGEINFO imginfo;
		m_imglistArrows.GetImageInfo(0, &imginfo);
		m_rectScrollLeft = imginfo.rcImage;
		m_rectScrollRight = imginfo.rcImage;

		int iTop = 5;
		int iLeft = 5;

		m_rectScrollLeft.OffsetRect(iLeft, iTop);
		m_imglistArrows.Draw(&_dc, 0, CPoint(iLeft, iTop), ILD_NORMAL);

		iLeft = m_szMonthSize.cx - 1 - 5 - m_rectScrollRight.Width();
		m_rectScrollRight.OffsetRect(iLeft, iTop);
		m_imglistArrows.Draw(&_dc, 1, CPoint(iLeft, iTop), ILD_NORMAL);
	}

	return m_iHeaderHeight;
}

// draw days of week header (ie. M T W T F S S)
int CMiniCalendarCtrl::DrawDaysOfWeek(CDC& _dc, int _iY, int _iLeftX, int /*_iRow*/)
{
	// calculate starting X position
	int iStartX = ((_iLeftX + (m_szMonthSize.cx / 2))) - (((m_iIndividualDayWidth * 7) + 30) / 2);
	int iEndX = ((_iLeftX + (m_szMonthSize.cx / 2))) + (((m_iIndividualDayWidth * 7) + 30) / 2);

	int iX = iStartX;

	CFont* pOldFont = _dc.SelectObject(m_pFont);
	_dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	_dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
	_dc.SetBkMode(TRANSPARENT);

	//weekdays
	for (int iDay = 0; iDay < 7; iDay++)
	{
		CString strDayName = m_arrShortDayOfWeekNames[iDay];
		CRect rcDay(iX, _iY + 1, iX + m_iIndividualDayWidth, _iY + 2 + m_iDaysOfWeekHeight);

		//change text colour for hidden weekend days
		if (m_pFrameWnd->IsWeekendsHidden())
		{
			if (IsWeekendDay(iDay + 1))
			{
				//select a grey font
				_dc.SetTextColor(MINICAL_COLOUR_HIDDEN_WEEKEND_DAY);
			}
			else
			{
				_dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
			}
		}

		_dc.DrawText(strDayName, rcDay, DT_SINGLELINE | DT_RIGHT | DT_BOTTOM);
		iX += m_iIndividualDayWidth + 5;
	}

	_dc.SelectObject(pOldFont);

	// draw the divider line
	CPen* pPen = new CPen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
	CPen* pOldPen = _dc.SelectObject(pPen);

	_dc.MoveTo(iStartX, _iY + 3 + m_iDaysOfWeekHeight);
	_dc.LineTo(iEndX, _iY + 3 + m_iDaysOfWeekHeight);

	_dc.SelectObject(pOldPen);
	delete pPen;

	return m_iDaysOfWeekHeight + 4;
}

// draw days of month
int CMiniCalendarCtrl::DrawDays(CDC& _dc, int _iY, int _iLeftX, int _iRow, int _iMonth, int _iYear)
{
	int iReturn = 0;
	int iStartY = _iY;

	COleDateTime dtStart;
	dtStart.SetDate(_iYear, _iMonth, 1);

	COleDateTime dt(dtStart);
	while (dt.GetDayOfWeek() != m_iFirstDayOfWeek)
	{
		dt -= 1;
	}

	// calculate starting X position
	int iStartX = ((_iLeftX + (m_szMonthSize.cx / 2))) - (((m_iIndividualDayWidth * 7) + 30) / 2);
	int iEndX = ((_iLeftX + (m_szMonthSize.cx / 2))) + (((m_iIndividualDayWidth * 7) + 30) / 2);

	int iX = iStartX;

	CFont* pOldFont = _dc.SelectObject(m_pFont);
	_dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	_dc.SetTextColor(::GetSysColor(COLOR_BTNTEXT));
	_dc.SetBkMode(TRANSPARENT);

	int iDayCounter = 0;

	// we allow up to 6 rows of days.  This is the actual maximum # of calendar
	// weeks that can occur in a month.
	int iRow = 1;
	for (; iRow <= 6; iRow++)
	{
		int iX = iStartX;

		for (int iDayOfWeek = 1; iDayOfWeek <= 7; iDayOfWeek++)
		{
			if (dt.GetMonth() == dtStart.GetMonth() || (dt > dtStart && _iRow == m_iRows) || (dt < dtStart && _iRow == 1))
			{
				CString strText = CCalendarUtils::LongToString(dt.GetDay());

				COLORREF cTextColor = MINICAL_COLOUR_DAY;
				if (dt.GetMonth() != _iMonth)
				{
					cTextColor = MINICAL_COLOUR_DAY_DIFFERENTMONTH;
				}

				//initialize as a normal date
				pOldFont = _dc.SelectObject(m_pFont);
				_dc.SetTextColor(cTextColor);

				if (IsDateSelected(dt))
				{
					//selected date
					if (m_pCalendarData->IsImportantDate(dt))
					{
						//important date too
						pOldFont = _dc.SelectObject(m_pFontBold);
					}
					else
					{
						pOldFont = _dc.SelectObject(m_pFont);
					}

					_dc.SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
					_dc.FillSolidRect(iX - 1, _iY, m_iIndividualDayWidth + 3, m_iDaysHeight + 1, ::GetSysColor(COLOR_HIGHLIGHT));
				}
				else if (m_pCalendarData->IsImportantDate(dt))
				{
					BOOL bImportantDate = TRUE;

					DWORD dwStyleCompletedTasks = m_pFrameWnd->GetCompletedTasksStyle();
					if (dwStyleCompletedTasks & COMPLETEDTASKS_HIDDEN)
					{
						//completed tasks are hidden. only consider this date important if there is one or more non-hidden tasks
						bImportantDate = FALSE;

						CTaskInfoList listTasks;
						m_pCalendarData->GetTasks(dt, listTasks);
						ASSERT(!listTasks.IsEmpty());

						//look for non-hidden tasks
						for (POSITION pos = listTasks.GetHeadPosition(); (pos != NULL) && !bImportantDate;)
						{
							CTaskInfo ti = listTasks.GetNext(pos);
							if (!ti.IsComplete())
							{
								bImportantDate = TRUE;
							}
						}
					}

					if (bImportantDate)
					{
						//important date
						pOldFont = _dc.SelectObject(m_pFontBold);
						if (m_pFrameWnd->IsDateHidden(dt))
						{
							//hidden weekend date
							_dc.SetTextColor(MINICAL_COLOUR_HIDDEN_WEEKEND_DAY);
						}
						_dc.FillSolidRect(iX - 1, _iY, m_iIndividualDayWidth + 3, m_iDaysHeight + 1, MINICAL_COLOUR_IMPORTANTDAY_BACKGROUND);
					}
				}
				else if (m_pFrameWnd->IsDateHidden(dt))
				{
					//hidden weekend date
					pOldFont = _dc.SelectObject(m_pFont);
					_dc.SetTextColor(MINICAL_COLOUR_HIDDEN_WEEKEND_DAY);
				}

				_dc.DrawText(strText, CRect(iX, _iY, iX + m_iIndividualDayWidth, _iY + m_iDaysHeight), DT_BOTTOM | DT_RIGHT | DT_SINGLELINE);

				//highlight today
				if (IsToday(dt))
				{
					_dc.Draw3dRect(iX - 2, _iY, m_iIndividualDayWidth + 5, m_iDaysHeight + 1, MINICAL_COLOUR_HEADERFONT, MINICAL_COLOUR_HEADERFONT);
				}

				SetHotSpot(_iRow, iDayCounter, dt, CRect(iX - 3, _iY, iX - 3 + m_iIndividualDayWidth + 5, _iY + m_iDaysHeight + 2));
			}

			dt += 1;
			iX += (m_iIndividualDayWidth + 5);
			iDayCounter++;
		}

		_iY += (2 + m_iDaysHeight);
		iReturn += (2 + m_iDaysHeight);
	}

	_dc.SelectObject(pOldFont);

	// draw the divider line
	if (iRow == m_iRows)
	{
		CPen* pPen = new CPen(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
		CPen* pOldPen = _dc.SelectObject(pPen);

		_dc.MoveTo(iStartX, _iY);
		_dc.LineTo(iEndX, _iY);

		_dc.SelectObject(pOldPen);
		delete pPen;
	}

	iReturn += 5;

	SetCellPosition(_iRow, CRect(iStartX, iStartY, iEndX, _iY));

	return iReturn;
}

CMiniCalendarCtrlFontHotSpot* CMiniCalendarCtrl::HitTest(const CPoint& _pt)
{
	CMiniCalendarCtrlFontHotSpot* pReturn = NULL;
	for (int iRow = 0; iRow < m_iRows && !pReturn; iRow++)
	{
		CRect rc;
		m_parCells[iRow].GetRect(rc);
		if (rc.PtInRect(_pt))
		{
			for (int iDay = 0; iDay < MINICAL_NUM_CELLS && !pReturn; iDay++)
			{
				if (m_parCells[iRow].GetHotSpot(iDay)->m_rect.PtInRect(_pt))
				{
					pReturn = m_parCells[iRow].GetHotSpot(iDay);
				}
			}
		}
	}
	return pReturn;
}

CMiniCalendarCtrlCell* CMiniCalendarCtrl::HeaderHitTest(const CPoint& _pt)
{
	CMiniCalendarCtrlCell* pReturn = NULL;
	for (int iRow = 0; iRow < m_iRows && !pReturn; iRow++)
	{
		CRect rcHeader;
		m_parCells[iRow].GetHeaderRect(rcHeader);
		if (rcHeader.PtInRect(_pt))
		{
			pReturn = &m_parCells[iRow];
		}
	}
	return pReturn;
}

/////////////////////////////////////////////////////////////////////////////
// CMiniCalendarCtrl message handlers
//
void CMiniCalendarCtrl::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	CMemDC dcDraw(&dc);

	if (!m_bSizeComputed)
	{
		ComputeSize();
	}

	CRect rcClient;
	GetClientRect(rcClient);

	dcDraw.FillSolidRect(0, 0, rcClient.Width(), rcClient.Height(), m_cBackColor);

	int iY = 0;
	int iMonth = m_iCurrentMonth;
	int iYear = m_iCurrentYear;

	// draw each row individually
	for (int iRow = 1; iRow <= m_iRows; iRow++)
	{
		int iCurrentX = 0;
		int iCurrentY = iY;

		iCurrentY += DrawHeader(dcDraw, iCurrentY, iCurrentX, iRow, iMonth, iYear);
		iCurrentY += DrawDaysOfWeek(dcDraw, iCurrentY, iCurrentX, iRow);
		iCurrentY += DrawDays(dcDraw, iCurrentY, iCurrentX, iRow, iMonth, iYear);
		iCurrentX += m_szMonthSize.cx;

		iMonth++;
		if (iMonth > 12)
		{
			iMonth = 1;
			iYear++;
		}

		iY += m_szMonthSize.cy;
	}
}

BOOL CMiniCalendarCtrl::OnEraseBkgnd(CDC*)
{
	return FALSE;
}

void CMiniCalendarCtrl::OnDevModeChange(LPTSTR lpDeviceName)
{
	CWnd::OnDevModeChange(lpDeviceName);
	RedrawWindow();
}

void CMiniCalendarCtrl::OnFontChange()
{
	CWnd::OnFontChange();
	RedrawWindow();
}

void CMiniCalendarCtrl::OnPaletteChanged(CWnd* pFocusWnd)
{
	CWnd::OnPaletteChanged(pFocusWnd);
	RedrawWindow();
}

void CMiniCalendarCtrl::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CWnd::OnSettingChange(uFlags, lpszSection);
	RedrawWindow();
}

void CMiniCalendarCtrl::OnTimeChange()
{
	CWnd::OnTimeChange();
	RedrawWindow();
}

void CMiniCalendarCtrl::OnSysColorChange()
{
	CWnd::OnSysColorChange();
	RedrawWindow();
}

void CMiniCalendarCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	SetFocus();

	if (m_rectScrollLeft.PtInRect(point))
	{
		//left month scroll-arrow
		ScrollLeft();
	}
	else if (m_rectScrollRight.PtInRect(point))
	{
		//right month scroll-arrow
		ScrollRight();
	}
	else if (HeaderHitTest(point))
	{
		//month name, show month dropdown
		CMiniCalendarCtrlCell* pCell = HeaderHitTest(point);
		if (pCell)
		{
			m_pHeaderCell = pCell;

			int iCell = ((pCell->GetRow() - 1) + pCell->GetCol()) - 1;

			// determine month/year of header
			int iMonth = m_iCurrentMonth;
			int iYear = m_iCurrentYear;

			for (int iX = 0; iX < iCell; iX++)
			{
				iMonth++;
				if (iMonth > 12)
				{
					iMonth = 1;
					iYear++;
				}
			}

			// make sure list is not already created
			if (m_pMonthPicker)
			{
				ASSERT(FALSE);
				m_pMonthPicker->DestroyWindow();
				delete m_pMonthPicker;
				m_pMonthPicker = NULL;
			}

			// create list
			m_pMonthPicker = new CMiniCalendarMonthPicker;

			// create list control
			DWORD dwStyle = WS_POPUP | WS_EX_TOPMOST | WS_EX_WINDOWEDGE | WS_BORDER;
			LPCTSTR szWndCls = AfxRegisterWndClass(CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS, 0, 0, 0);

			m_iHeaderTimerID = SetTimer(MINICAL_HEADER_TIMER_ID, MINICAL_HEADER_TIMER_INTERVAL, NULL);

			m_pMonthPicker->SetMiddleMonthYear(iMonth, iYear);
			m_pMonthPicker->SetCalendar(this);
			m_pMonthPicker->CreateEx(0, szWndCls, _T(""), dwStyle, 0, 0, 0, 0, m_hWnd, NULL, NULL);
			m_pMonthPicker->AutoConfigure();
			m_pMonthPicker->ShowWindow(TRUE);

			SetCapture();
			m_bHeaderTracking = TRUE;
		}
		else
		{
			ASSERT(FALSE);
		}
	}
	else
	{
		CMiniCalendarCtrlFontHotSpot* pSpot = HitTest(point);
		if (pSpot)
		{
			//clicked on a date hotspot
			if (!m_pFrameWnd->IsDateHidden(pSpot->m_date))
			{
				BOOL bDifferentDate = SelectDate(pSpot->m_date);
				m_dateSelected = pSpot->m_date;

				SetCapture();
				m_bTracking = TRUE;

				if (bDifferentDate)
				{
					FireNotifySelectDate();
				}
			}
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CMiniCalendarCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		ReleaseCapture();
		m_bTracking = FALSE;
		RedrawWindow();
		FireNotifyClick();
	}
	else if (m_bHeaderTracking)
	{
		ASSERT(m_pMonthPicker);

		if (m_pMonthPicker && m_pHeaderCell)
		{
			int iSelMonth = m_pMonthPicker->GetSelMonth();
			int iSelYear = m_pMonthPicker->GetSelYear();

			int iCell = ((m_pHeaderCell->GetRow() - 1) + m_pHeaderCell->GetCol()) - 1;

			for (int iX = 0; iX < iCell; iX++)
			{
				iSelMonth--;
				if (iSelMonth < 1)
				{
					iSelMonth = 12;
					iSelYear--;
				}
			}

			m_iCurrentMonth = iSelMonth;
			m_iCurrentYear = iSelYear;
			ASSERT((m_iCurrentYear >= 100) && (m_iCurrentYear <= 9999)); //From MSDN: The COleDateTime class handles
			ASSERT((m_iCurrentMonth >= 1) && (m_iCurrentMonth <= 12));   //dates from 1 January 100 to 31 December 9999.
		}

		if (m_pMonthPicker)
		{
			m_pMonthPicker->DestroyWindow();
			delete m_pMonthPicker;
		}
		m_pMonthPicker = NULL;

		m_pHeaderCell = NULL;

		if (m_iHeaderTimerID != 0)
		{
			KillTimer(m_iHeaderTimerID);
		}
		m_iHeaderTimerID = 0;

		m_bHeaderTracking = FALSE;
		ReleaseCapture();
		RedrawWindow();

		FireNotifyClick();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CMiniCalendarCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}

void CMiniCalendarCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bTracking)
	{
		CMiniCalendarCtrlFontHotSpot* pSpot = HitTest(point);
		if (pSpot)
		{
			if (!m_pFrameWnd->IsDateHidden(pSpot->m_date))
			{
				BOOL bDifferentDate = SelectDate(pSpot->m_date);
				RedrawWindow();
				if (bDifferentDate)
				{
					FireNotifySelectDate();
				}
			}
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

BOOL CMiniCalendarCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	FireNotifyMouseWheel(zDelta > 0);
	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CMiniCalendarCtrl::OnSize(UINT nType, int cx, int cy)
{
	if (cy > 3 * MINICALENDAR_HEIGHT)
	{
		SetRows(3);
	}
	else if (cy > 2 * MINICALENDAR_HEIGHT)
	{
		SetRows(2);
	}
	else
	{
		SetRows(1);
	}

	CWnd::OnSize(nType, cx, cy);
}

////////////////////////////////////////////////////////
// CMiniCalendarCtrlCell functions
//
CMiniCalendarCtrlCell::CMiniCalendarCtrlCell()
	:	m_iRow(0),
	   m_iCol(0)
{
	m_parHotSpots = new CMiniCalendarCtrlFontHotSpot[MINICAL_NUM_CELLS];
}

CMiniCalendarCtrlCell::~CMiniCalendarCtrlCell()
{
	if (m_parHotSpots != NULL)
	{
		delete[] m_parHotSpots;
	}
}
