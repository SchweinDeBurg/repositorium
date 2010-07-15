// Copyright (C) 2003-2005 AbstractSpoon Software.
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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

#if !defined(AFX_TDLSETREMINDERDLG_H__DAAE865F_6EBD_4BA3_967C_A4F8675BDB94__INCLUDED_)
#define AFX_TDLSETREMINDERDLG_H__DAAE865F_6EBD_4BA3_967C_A4F8675BDB94__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLSetReminderDlg.h : header file
//

#include "TDCStruct.h"
#include "TDCEnum.h"

#include "../../Common/FileEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLSetReminderDlg dialog

class CTDLSetReminderDlg : public CDialog
{
// Construction
public:
	CTDLSetReminderDlg(CWnd* pParent = NULL);   // standard constructor

	int DoModal(const TDCREMINDER& rem);

	double GetLeadIn() { return m_dLeadIn; }
	TDC_REMINDER GetFromWhen() { return m_bFromDueDate ? TDCR_DUEDATE : TDCR_STARTDATE; }
	CString GetSoundFile() { return m_sSoundFile; }


// Dialog Data
	//{{AFX_DATA(CTDLSetReminderDlg)
	enum { IDD = IDD_SETREMINDER_DIALOG };
	CFileEdit	m_ePlaySound;
	int		m_bFromDueDate;
	CString	m_sTaskTitle;
	CComboBox m_cbLeadIn;
	int m_nLeadIn;
	double m_dLeadIn;
	CString	m_sSoundFile;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLSetReminderDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLSetReminderDlg)
	afx_msg void OnSelchangeLeadin();
	//}}AFX_MSG
	LRESULT OnPlaySound(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLREMINDERDLG_H__DAAE865F_6EBD_4BA3_967C_A4F8675BDB94__INCLUDED_)
