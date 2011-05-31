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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

#if !defined(AFX_BROWSERDLG_H__3AD7A73A_4A17_4625_92E9_F74C6D930C34__INCLUDED_)
#define AFX_BROWSERDLG_H__3AD7A73A_4A17_4625_92E9_F74C6D930C34__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BrowserDlg.h : header file
//

#include "../../CodeProject/Source/RuntimeDlg.h"
#include "WebBrowserCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CBrowserDlg dialog

class CBrowserDlg : public CRuntimeDlg
{
// Construction
public:
	CBrowserDlg(BOOL bBrowser = TRUE, LPCTSTR szSettingsKey = NULL);

	virtual int DoModal(LPCTSTR szCaption, LPCTSTR szUrlPath, CWnd* pParentWnd = NULL);
	virtual BOOL Create(LPCTSTR szCaption, LPCTSTR szUrlPath, CWnd* pParentWnd);
	BOOL Navigate(LPCTSTR szUrlPath);

protected:
	CWebBrowserCtrl m_browser;
	CEdit m_edit;
	CString m_sUrl;
	BOOL m_bBrowser;

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBrowserDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBrowserDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnEditSetFocus();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BROWSERDLG_H__3AD7A73A_4A17_4625_92E9_F74C6D930C34__INCLUDED_)
