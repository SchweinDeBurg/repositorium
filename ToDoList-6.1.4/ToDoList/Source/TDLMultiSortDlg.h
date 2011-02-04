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

#if !defined(AFX_TDLMULTISORTDLG_H__95734250_E60B_48CF_B356_1394C11317DA__INCLUDED_)
#define AFX_TDLMULTISORTDLG_H__95734250_E60B_48CF_B356_1394C11317DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLMultiSortDlg.h : header file
//

#include "TDCEnum.h"
#include "TDCStruct.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLMultiSortDlg dialog

class CTDLMultiSortDlg : public CDialog
{
// Construction
public:
	CTDLMultiSortDlg(const TDSORTCOLUMNS& sort, CWnd* pParent = NULL);   // standard constructor

	void GetSortBy(TDSORTCOLUMNS& sort) const;

protected:
// Dialog Data
	//{{AFX_DATA(CTDLMultiSortDlg)
	enum { IDD = IDD_MULTISORT_DIALOG };
	CComboBox	m_cbSortBy3;
	CComboBox	m_cbSortBy2;
	CComboBox	m_cbSortBy1;
	//}}AFX_DATA
	TDC_SORTBY m_nSortBy1, m_nSortBy2, m_nSortBy3;
	BOOL m_bAscending1, m_bAscending2, m_bAscending3;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLMultiSortDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLMultiSortDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeSortby1();
	afx_msg void OnSelchangeSortby2();
	afx_msg void OnSelchangeSortby3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void BuildCombos();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLMULTISORTDLG_H__95734250_E60B_48CF_B356_1394C11317DA__INCLUDED_)
