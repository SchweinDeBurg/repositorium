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
//*****************************************************************************

#if !defined(AFX_TDLPREFMIGRATIONDLG_H__57A43C77_570F_4B6D_A27B_4D33936B6CBF__INCLUDED_)
#define AFX_TDLPREFMIGRATIONDLG_H__57A43C77_570F_4B6D_A27B_4D33936B6CBF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLPrefMigrationDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTDLPrefMigrationDlg dialog

class CTDLPrefMigrationDlg : public CDialog
{
// Construction
public:
	CTDLPrefMigrationDlg(CWnd* pParent = NULL);   // standard constructor

protected:
// Dialog Data
	//{{AFX_DATA(CTDLPrefMigrationDlg)
	enum { IDD = IDD_REGMIGRATION_DIALOG };
	CAnimateCtrl m_animation;
	//}}AFX_DATA
	CString m_sAnimationFilePath;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLPrefMigrationDlg)
public:
	virtual BOOL Create(CWnd* pParentWnd);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	int DoModal()
	{
		return CDialog::DoModal();
	}

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLPrefMigrationDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLPREFMIGRATIONDLG_H__57A43C77_570F_4B6D_A27B_4D33936B6CBF__INCLUDED_)
