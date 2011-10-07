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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.6-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TDLADDLOGGEDTIMEDLG_H__1E431AC9_0AA0_44E5_9CAE_723D199D910E__INCLUDED_)
#define AFX_TDLADDLOGGEDTIMEDLG_H__1E431AC9_0AA0_44E5_9CAE_723D199D910E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLAddLoggedTimeDlg.h : header file
//

#include "../../Common/TimeEdit.h"
#include "../../../CodeProject/Source/TimeComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLAddLoggedTimeDlg dialog

class CTDLAddLoggedTimeDlg : public CDialog
{
// Construction
public:
	CTDLAddLoggedTimeDlg(DWORD dwTaskID, LPCTSTR szTaskTitle, CWnd* pParent = NULL);   // standard constructor
	double GetLoggedTime() const; // in hours
	COleDateTime GetWhen() const;
	BOOL GetAddToTimeSpent() const
	{
		return m_bAddTimeToTimeSpent;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CTDLAddLoggedTimeDlg)
	enum { IDD = IDD_ADDLOGGEDTIME_DIALOG };
	CDateTimeCtrl m_dateWhen;
	CTimeComboBox m_cbTimeWhen;
	double m_dLoggedTime;
	DWORD m_dwTaskID;
	CString m_sTaskTitle;
	BOOL m_bAddTimeToTimeSpent;
	//}}AFX_DATA
	CTimeEdit m_eLoggedTime;
	int m_nUnits;
	COleDateTime m_dtWhen;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLAddLoggedTimeDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLAddLoggedTimeDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLADDLOGGEDTIMEDLG_H__1E431AC9_0AA0_44E5_9CAE_723D199D910E__INCLUDED_)
