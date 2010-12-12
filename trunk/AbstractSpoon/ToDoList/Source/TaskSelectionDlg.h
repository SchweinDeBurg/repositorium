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

#if !defined(AFX_TASKSELECTIONDLG_H__3A5D7E88_CEC3_47DD_8BA6_79EC2C3B7167__INCLUDED_)
#define AFX_TASKSELECTIONDLG_H__3A5D7E88_CEC3_47DD_8BA6_79EC2C3B7167__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TaskSelectionDlg.h : header file
//

#include "TDLAttribListBox.h"

const UINT WM_TASKSELDLG_CHANGE = ::RegisterWindowMessage(_T("WM_TASKSELDLG_CHANGE"));

enum TSD_ATTRIB
{
	TSDA_ALL,
	TSDA_VISIBLE,
	TSDA_CUSTOM,
};

enum TSD_TASKS
{
	TSDT_ALL,
	TSDT_FILTERED,
	TSDT_SELECTED,
};

/////////////////////////////////////////////////////////////////////////////
// CTaskSelectionDlg dialog

class CTaskSelectionDlg : public CDialog
{
	// Construction
public:
	CTaskSelectionDlg(LPCTSTR szRegKey = NULL, BOOL bShowSubtaskCheckbox = TRUE, BOOL bVisibleColumnsOnly = FALSE);

	BOOL Create(UINT nIDRefFrame, CWnd* pParent, UINT nID = IDC_STATIC);
	int DoModal()
	{
		ASSERT(0);
		return IDCANCEL;
	}

	TSD_TASKS GetWantWhatTasks() const
	{
		return (TSD_TASKS)m_nWhatTasks;
	}
	BOOL GetWantAllTasks() const
	{
		return m_nWhatTasks == TSDT_ALL;
	}
	BOOL GetWantFilteredTasks() const
	{
		return m_nWhatTasks == TSDT_FILTERED;
	}
	BOOL GetWantSelectedTasks() const
	{
		return m_nWhatTasks == TSDT_SELECTED;
	}

	BOOL GetWantSelectedSubtasks() const
	{
		return GetWantSelectedTasks() && m_bSelectedSubtasks && m_bShowSubtaskCheckbox;
	}
	BOOL GetWantCompletedTasks() const
	{
		return m_bCompletedTasks;
	}
	BOOL GetWantInCompleteTasks() const
	{
		return m_bIncompleteTasks;
	}

	TSD_ATTRIB GetAttributeOption() const
	{
		return (TSD_ATTRIB)m_nAttribOption;
	}
	int GetCustomAttributes(CTDCAttributeArray& aCols) const;
	int GetAllAttributes(CTDCAttributeArray& aCols) const
	{
		return m_lbAttribList.GetAllAttributes(aCols);
	}

	void SetWantWhatTasks(TSD_TASKS nWhat);
	void SetWantCompletedTasks(BOOL bWant = TRUE);
	void SetWantInCompleteTasks(BOOL bWant = TRUE);

protected:
	// Dialog Data
	//{{AFX_DATA(CTaskSelectionDlg)
	enum { IDD = IDD_TASKSELECTION_DIALOG };
	int     m_nAttribOption;
	//}}AFX_DATA
	CTDLAttributeListBox    m_lbAttribList;
	BOOL    m_bSelectedSubtasks;
	BOOL    m_bCompletedTasks;
	BOOL    m_bIncompleteTasks;
	int     m_nWhatTasks;
	CString m_sRegKey;
	BOOL    m_bShowSubtaskCheckbox;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTaskSelectionDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTaskSelectionDlg)
	afx_msg void OnChangeAttribOption();
	//}}AFX_MSG
	afx_msg void OnChangetasksOption();
	afx_msg void OnIncludeDone();
	afx_msg void OnIncludeNotDone();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TASKSELECTIONDLG_H__3A5D7E88_CEC3_47DD_8BA6_79EC2C3B7167__INCLUDED_)
