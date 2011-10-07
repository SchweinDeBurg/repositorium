// Written for the ToDoList (http://www.codeproject.com/KB/applications/todolist2.aspx)
// Design and latest version - http://www.codeproject.com/KB/applications/TDL_Calendar.aspx
// by demon.code.monkey@googlemail.com

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - adjusted #include's paths
// - slightly reformatted source code
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

#ifndef _DLGCOMPLETEDTASKS_H_
#define _DLGCOMPLETEDTASKS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDlgCompletedTasks dialog

class CDlgCompletedTasks : public CDialog
{
// Construction
public:
	CDlgCompletedTasks(DWORD _dwFlags = 0, CWnd* _pParent = NULL);

	DWORD GetFlags() const;

// Dialog Data
	//{{AFX_DATA(CDlgCompletedTasks)
	enum { IDD = IDD_COMPLETED_TASKS };
	CStatic m_staticSampleText;
	CButton m_chkHidden;
	CButton m_chkGrey;
	CButton m_chkStrikethru;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgCompletedTasks)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgCompletedTasks)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnChangeCheckbox();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateFlags();

	DWORD m_dwFlags;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif//_DLGCOMPLETEDTASKS_H_
