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
// - added AbstractSpoon Software copyright notice and license information
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

#if !defined(AFX_EDITWEBLINKDLG_H__DD0DB833_7D2B_4E92_8478_FD1B43B3207E__INCLUDED_)
#define AFX_EDITWEBLINKDLG_H__DD0DB833_7D2B_4E92_8478_FD1B43B3207E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditWebLinkDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CEditWebLinkDlg dialog

class CEditWebLinkDlg : public CDialog
{
// Construction
public:
	CEditWebLinkDlg(CWnd* pParent = NULL);   // standard constructor

	CString GetWebLink()
	{
		return m_sWebLink;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CEditWebLinkDlg)
	enum { IDD = IDD_EDITWEBLINK_DIALOG };
	CString	m_sWebLink;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditWebLinkDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEditWebLinkDlg)
	// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITWEBLINKDLG_H__DD0DB833_7D2B_4E92_8478_FD1B43B3207E__INCLUDED_)
