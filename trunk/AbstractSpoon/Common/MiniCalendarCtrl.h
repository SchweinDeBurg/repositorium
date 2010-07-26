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

#ifndef _MINICALENDARCTRL_H_
#define _MINICALENDARCTRL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//forward declarations
class CMiniCalendarMonthPicker;
class CCalendarData;
class CCalendarFrameWnd;

struct CMiniCalendarCtrlFontHotSpot
{
	CRect           m_rect;
	COleDateTime    m_date;
};

class CMiniCalendarCtrlCell
{
public:
	CMiniCalendarCtrlCell();
	virtual ~CMiniCalendarCtrlCell();

	void GetHeaderRect(CRect& _rect) const
	{
		_rect = m_rectHeader;
	}
	void SetHeaderRect(const CRect& _rect)
	{
		m_rectHeader = _rect;
	}
	void SetHeaderRectEmpty()
	{
		m_rectHeader.SetRectEmpty();
	}

	void GetRect(CRect& _rect) const
	{
		_rect = m_rect;
	}
	void SetRect(const CRect& _rect)
	{
		m_rect = _rect;
	}
	void SetRectEmpty()
	{
		m_rect.SetRectEmpty();
	}

	int  GetRow() const
	{
		return m_iRow;
	}
	void SetRow(int _iRow)
	{
		m_iRow = _iRow;
	}

	int  GetCol() const
	{
		return m_iCol;
	}
	void SetCol(int _iCol)
	{
		m_iCol = _iCol;
	}

	CMiniCalendarCtrlFontHotSpot* GetHotSpot(int _iDay)
	{
		return &(m_parHotSpots[_iDay]);
	}

private:
	CRect                           m_rectHeader;
	CRect                           m_rect;
	int                             m_iRow;
	int                             m_iCol;
	CMiniCalendarCtrlFontHotSpot*   m_parHotSpots;
};

class CMiniCalendarCtrl : public CWnd
{
public:
	CMiniCalendarCtrl();
	virtual ~CMiniCalendarCtrl();
	void Initialize(CCalendarFrameWnd* _pFrameWnd, CCalendarData* _pCalendarData);

	void GetSelectedDate(COleDateTime& _date) const;
	void Repopulate();
	BOOL SelectDate(const COleDateTime& _date);
	void SelectToday();
	void TasklistUpdated();

	//called from CMiniCalendarMonthPicker
	CString GetMonthName(int _iMonth) const;
	CFont*  GetDaysFont();

	//{{AFX_VIRTUAL(CMiniCalendarCtrl)
	BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

protected:
	BOOL    IsToday(const COleDateTime& _date) const;
	BOOL    IsDateSelected(const COleDateTime& _date) const;
	BOOL    IsDateVisible(const COleDateTime& _date) const;
	BOOL    IsWeekendDay(int _iDayOfWeek) const;

	void    AllocateCells();
	void    ClearHotSpots();
	CSize   ComputeSize();
	CSize   ComputeTotalSize();
	void    FireNotifySelectDate();
	void    FireNotifyClick();
	void    FireNotifyMouseWheel(BOOL _bScrollUp);
	void    ScrollLeft();
	void    ScrollRight();

	void    SetCellHeaderPosition(int _iMonthRow, const CRect& _rect);
	void    SetCellPosition(int _iMonthRow, const CRect& _rect);

	void    SetFirstDayOfWeek(int _iDayOfWeek);

	void    SetHotSpot(int _iMonthRow, int _iDayCounter, const COleDateTime& _date, const CRect& _rect);
	void    SetRows(int _nRows);

	int     DrawHeader(CDC& _dc, int _iY, int _iLeftX, int _iRow, int _iMonth, int _iYear);
	int     DrawDaysOfWeek(CDC& _dc, int _iY, int _iLeftX, int _iRow);
	int     DrawDays(CDC& _dc, int _iY, int _iLeftX, int _iRow, int _iMonth, int _iYear);

	CMiniCalendarCtrlFontHotSpot*   HitTest(const CPoint& _pt);
	CMiniCalendarCtrlCell*          HeaderHitTest(const CPoint& _pt);

	//{{AFX_MSG(CMiniCalendarCtrl)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDevModeChange(LPTSTR lpDeviceName);
	afx_msg void OnFontChange();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnTimeChange();
	afx_msg void OnSysColorChange();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	BOOL                            m_bSizeComputed;
	int                             m_iHeaderHeight;
	int                             m_iIndividualDayWidth;
	int                             m_iDaysOfWeekHeight;
	int                             m_iDaysHeight;
	CSize                           m_szMonthSize;

	COleDateTime                    m_dateSelected;
	CMiniCalendarCtrlCell*          m_parCells;

	CFont*                          m_pFont;
	CFont*                          m_pFontBold;

	CImageList                      m_imglistArrows;

	int                             m_iCurrentMonth;
	int                             m_iCurrentYear;
	COLORREF                        m_cBackColor;
	CString                         m_arrMonthNames[12];
	CString                         m_arrShortDayOfWeekNames[7];
	int                             m_iFirstDayOfWeek;
	int                             m_iRows;

	BOOL                            m_bTracking;
	BOOL                            m_bHeaderTracking;
	UINT                            m_iHeaderTimerID;
	CMiniCalendarCtrlCell*          m_pHeaderCell;
	CMiniCalendarMonthPicker*       m_pMonthPicker;

	CCalendarData*                  m_pCalendarData;
	CCalendarFrameWnd*              m_pFrameWnd;

	CRect                           m_rectScrollLeft;
	CRect                           m_rectScrollRight;
};

#endif//_MINICALENDARCTRL_H_
