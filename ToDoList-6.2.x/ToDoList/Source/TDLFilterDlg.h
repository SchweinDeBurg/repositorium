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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#if !defined(AFX_FILTERDLG_H__9BFA276C_9BD6_4B23_A714_BB9D059CC2EB__INCLUDED_)
#define AFX_FILTERDLG_H__9BFA276C_9BD6_4B23_A714_BB9D059CC2EB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLFilterDlg.h : header file
//

#include "FilteredToDoCtrl.h"
#include "TDLFilterComboBox.h"
#include "TDLFilterOptionComboBox.h"
#include "TDLPriorityComboBox.h"
#include "TDLRiskComboBox.h"

#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/CheckComboBox.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CFilterDlg dialog

class CTDLFilterDlg : public CDialog, public CDialogHelper
{
// Construction
public:
	CTDLFilterDlg(BOOL bMultiSelFilters, CWnd* pParent = NULL);   // standard constructor

	int DoModal(const CFilteredToDoCtrl& tdc, const CDWordArray& aPriorityColors);
	void GetFilter(FTDCFILTER& filter);

protected:
// Dialog Data
	//{{AFX_DATA(CFilterDlg)
	enum { IDD = IDD_FILTER_DIALOG };
	//}}AFX_DATA
	CEnCheckComboBox    m_cbCategoryFilter;
	CEnCheckComboBox    m_cbStatusFilter;
	CEnCheckComboBox    m_cbAllocToFilter;
	CEnCheckComboBox    m_cbAllocByFilter;
	CEnCheckComboBox    m_cbVersionFilter;
	CTDLFilterComboBox  m_cbTaskFilter;
	CTDLPriorityComboBox    m_cbPriorityFilter;
	CTDLRiskComboBox    m_cbRiskFilter;
	FTDCFILTER m_filter;
	CStringArray m_aAllocTo;
	CStringArray m_aAllocBy;
	CStringArray m_aCategory;
	CStringArray m_aStatus;
	CStringArray m_aVersion;
	CDWordArray m_aPriorityColors;
	CTDLFilterOptionComboBox m_cbOptions;
	FTC_VIEW m_nView;
	CString m_sCustomFilter;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnClearfilter();
	afx_msg void OnSelchangeFiltercombo();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDLG_H__9BFA276C_9BD6_4B23_A714_BB9D059CC2EB__INCLUDED_)
