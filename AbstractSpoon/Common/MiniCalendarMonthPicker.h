// CMiniCalendarMonthPicker
// Author:  Matt Gullett
//          gullettm@yahoo.com
// Copyright (c) 2001
//
// This is a companion control for use by the CMiniCalendarCtrl.
// It is probably useless for any other purpose.
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
// - improved compatibility with the Visual C++ 2008
// - taken out from the original TDL_Calendar package for better sharing
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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#ifndef _MINICALENDARMONTHPICKER_H_
#define _MINICALENDARMONTHPICKER_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMiniCalendarCtrl;

class CMiniCalendarMonthPicker : public CWnd
{
// Construction
public:
	CMiniCalendarMonthPicker();
	virtual ~CMiniCalendarMonthPicker();

// Operations
public:
	int GetSelMonth()
	{
		return m_iSelMonth;
	}
	int GetSelYear()
	{
		return m_iSelYear;
	}

	void SetCalendar(CMiniCalendarCtrl* pWnd);
	void SetMiddleMonthYear(int iMonth, int iYear);
	void SetItemsPerPage(int iValue);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMiniCalendarMonthPicker)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ScrollDown(int iCount = 1);
	void ScrollUp(int iCount = 1);
	void AutoConfigure();
	void ForwardMessage(MSG* pMSG);

	// Generated message map functions
protected:
	BOOL IsSelected(int iX, CRect rectItem);

	//{{AFX_MSG(CMiniCalendarMonthPicker)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	int         m_iMiddleMonth;
	int         m_iMiddleYear;
	int         m_iItemsPerPage;

	int         m_iSelMonth;
	int         m_iSelYear;

	COLORREF    m_cBackColor;
	COLORREF    m_cTextColor;

	int         m_iUpFactor;
	int         m_iDownFactor;

	CMiniCalendarCtrl*          m_pwndCalendar;
};

#endif//_MINICALENDARMONTHPICKER_H_
