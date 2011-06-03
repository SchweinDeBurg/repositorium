// Copyright (C) 2003-2011 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TIMECOMBOBOX_H__E536C077_C228_4A44_89F5_D44F456A2EE7__INCLUDED_)
#define AFX_TIMECOMBOBOX_H__E536C077_C228_4A44_89F5_D44F456A2EE7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TimeComboBox.h : header file
//
#include "Subclass.h"

/////////////////////////////////////////////////////////////////////////////
// CTimeComboBox window

enum
{
	TCB_NOTIME      = 0x01,
	TCB_HALFHOURS   = 0x02,
	TCB_ISO         = 0x04,
};

class CTimeComboBox : public CComboBox, public CSubclasser
{
// Construction
public:
	CTimeComboBox(DWORD dwStyle = 0);

	double GetOleTime() const; // 0-1 suitable for COleDateTime, -1 for 'no time'
	BOOL SetOleTime(double dTime); // 0-1 suitable for COleDateTime, -1 for 'no time'

	double Get24HourTime() const; // -1 for 'no time'
	BOOL Set24HourTime(double dTime); // -1 for 'no time'

	DWORD GetStyle()
	{
		return m_dwStyle;
	}
	void SetStyle(DWORD dwStyle);

protected:
	DWORD m_dwStyle;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTimeComboBox)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTimeComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTimeComboBox)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void BuildCombo(BOOL bReset = FALSE);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TIMECOMBOBOX_H__E536C077_C228_4A44_89F5_D44F456A2EE7__INCLUDED_)
