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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_FILTERBAR_H__FAABD9A1_72C4_4731_B7A4_48251860672C__INCLUDED_)
#define AFX_FILTERBAR_H__FAABD9A1_72C4_4731_B7A4_48251860672C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLFilterBar.h : header file
//

#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/CheckComboBox.h"
#include "../../../CodeProject/Source/EnEdit.h"

#include "TDCStruct.h"
#include "TDLPriorityComboBox.h"
#include "TDLRiskComboBox.h"
#include "TDLFilterOptionComboBox.h"
#include "TDLFilterComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterBar dialog

class CFilteredToDoCtrl;
class CDeferWndMove;

class CTDLFilterBar : public CDialog, public CDialogHelper
{
// Construction
public:
	CTDLFilterBar(CWnd* pParent = NULL);   // standard constructor

	BOOL Create(CWnd* pParentWnd, UINT nID, BOOL bVisible = TRUE);
	void GetFilter(FTDCFILTER& filter) const
	{
		filter = m_filter;
	}
	void SetFilter(const FTDCFILTER& filter, FTC_VIEW nView);

	void SetCustomFilter(BOOL bCustom = TRUE, LPCTSTR szFilter = NULL);
	void RemoveCustomFilter();

	void RefreshFilterControls(const CFilteredToDoCtrl& tdc);
	void SetFilterLabelAlignment(BOOL bLeft);
	void SetPriorityColors(const CDWordArray& aColors);
	int CalcHeight(int nWidth);
	void SetVisibleFilters(const CTDCColumnArray& aFilters);
	BOOL FilterMatches(const FTDCFILTER& filter)
	{
		return (filter == m_filter);
	}

	void EnableMultiSelection(BOOL bEnable);
	void SetUIColors(COLORREF crBack, COLORREF crText);

protected:
// Dialog Data
	//{{AFX_DATA(CFilterBar)
	//}}AFX_DATA
	CTDLFilterComboBox  m_cbTaskFilter;
	CEnEdit m_eTitleFilter;
	CEnCheckComboBox    m_cbAllocToFilter;
	CEnCheckComboBox    m_cbAllocByFilter;
	CEnCheckComboBox    m_cbCategoryFilter;
	CEnCheckComboBox    m_cbStatusFilter;
	CEnCheckComboBox    m_cbVersionFilter;
	CTDLPriorityComboBox    m_cbPriorityFilter;
	CTDLRiskComboBox    m_cbRiskFilter;
	CTDLFilterOptionComboBox    m_cbOptions;
	FTDCFILTER m_filter;
	CDWordArray m_aPriorityColors;
	CDWordArray m_aVisibility;
	FTC_VIEW m_nView;
	CBrush m_brUIBack;
	COLORREF m_crUIBack, m_crUIText;
	BOOL m_bCustomFilter;
	CString m_sCustomFilter;

protected:
	int DoModal()
	{
		return -1;
	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterBar)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK()
	{
		/* do nothing */
	}
	virtual void OnCancel()
	{
		/* do nothing */
	}
	virtual BOOL PreTranslateMessage(MSG* pMsg);

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	afx_msg void OnSelchangeFilter();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg LRESULT OnEEBtnClick(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

protected:
	int ReposControls(int nWidth = -1, BOOL bCalcOnly = FALSE);
	BOOL WantShowFilter(TDC_COLUMN nType);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERBAR_H__FAABD9A1_72C4_4731_B7A4_48251860672C__INCLUDED_)
