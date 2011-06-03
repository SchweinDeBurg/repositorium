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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TDLTRANSFORMDIALOG_H__1A62F94F_687F_421C_97D2_300BAC4A3E7C__INCLUDED_)
#define AFX_TDLTRANSFORMDIALOG_H__1A62F94F_687F_421C_97D2_300BAC4A3E7C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLTransformDialog.h : header file
//

#include "../../Common/FileEdit.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "TaskSelectionDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLTransformDialog dialog

class CTDLTransformDialog : public CDialog
{
// Construction
public:
	CTDLTransformDialog(LPCTSTR szTitle, BOOL bShowSubtaskCheckbox = TRUE, CWnd* pParent = NULL);   // standard constructor

	BOOL GetWantDate()
	{
		return m_bDate;
	}
	CString GetStylesheet() const;
	CString GetTitle() const
	{
		return m_sTitle;
	}

	const CTaskSelectionDlg& GetTaskSelection() const
	{
		return m_taskSel;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CTDLTransformDialog)
	CFileEdit   m_eStylesheet;
	CString m_sStylesheet;
	CTaskSelectionDlg m_taskSel;
	//}}AFX_DATA
	CString m_sTitle;
	BOOL    m_bDate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLTransformDialog)
public:
	virtual void OnOK();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTDLTransformDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeStylesheet();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLTRANSFORMDIALOG_H__1A62F94F_687F_421C_97D2_300BAC4A3E7C__INCLUDED_)
