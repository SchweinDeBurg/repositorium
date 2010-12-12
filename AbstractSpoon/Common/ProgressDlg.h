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

#if !defined(AFX_PROGRESSDLG_H__A8A46BFB_E2CA_4BBF_A76D_C363065DEFA7__INCLUDED_)
#define AFX_PROGRESSDLG_H__A8A46BFB_E2CA_4BBF_A76D_C363065DEFA7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

#include "../../CodeProject/Source/RuntimeDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

class CProgressDlg : public CRuntimeDlg
{
// Construction
public:
	CProgressDlg(BOOL bCancelButton = TRUE, CWnd* pParent = NULL);   // standard constructor

	BOOL Create(LPCTSTR szCaption, LPCTSTR szDescription, CWnd* pParent);

	void SetCaption(LPCTSTR szCaption);
	void SetProgress(LPCTSTR szProgress);
	void SetProgress(int nProgress);
	void SetDescription(LPCTSTR szDescription);

	BOOL Continue();

protected:
// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	CProgressCtrl	m_progress;
	CString	m_sProgress;
	CString	m_sDescription;
	//}}AFX_DATA
	BOOL m_bContinue;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnCancel();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__A8A46BFB_E2CA_4BBF_A76D_C363065DEFA7__INCLUDED_)
