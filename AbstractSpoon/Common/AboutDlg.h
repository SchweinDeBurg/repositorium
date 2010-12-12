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
//*****************************************************************************

#if !defined(AFX_ABOUTDLG_H__31BB99EB_BB66_4016_82F1_8688520BE14A__INCLUDED_)
#define AFX_ABOUTDLG_H__31BB99EB_BB66_4016_82F1_8688520BE14A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDlg.h : header file
//

#include "../../CodeProject/Source/RuntimeDlg.h"
#include "../../CodeProject/Source/XHtmlStatic.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

enum AB_STYLE
{
	ABS_HTMLCOPYRIGHT,
	ABS_EDITCOPYRIGHT,
};

class CAboutDlg : public CRuntimeDlg
{
	// Construction
public:
	CAboutDlg(UINT nAppIconID, AB_STYLE nStyle,
		LPCTSTR szAppName, LPCTSTR szAppDescription, LPCTSTR szCopyright, LPCTSTR szLicense,
		int nAppLines = 1, int nDescLines = 3, int nCopyrightLines = 2, int nLicenseLines = 2);

	int DoModal();

protected:
	// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	CStatic m_stIcon;
	CString m_sAppName;
	CString m_sAppDescription;
	CString m_sCopyright;
	CString m_sLicense;
	//}}AFX_DATA
	CXHTMLStatic m_stAppName;
	CXHTMLStatic m_stAppDescription;
	CXHTMLStatic m_stCopyright;
	CXHTMLStatic m_stLicense;
	HICON m_hIcon;
	AB_STYLE m_nStyle;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ABOUTDLG_H__31BB99EB_BB66_4016_82F1_8688520BE14A__INCLUDED_)
