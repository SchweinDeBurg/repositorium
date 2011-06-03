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

#if !defined(AFX_FINDTASKEXPRESSIONLISTCTRL_H__42272309_6C54_4901_A56D_D6FDA87F1E48__INCLUDED_)
#define AFX_FINDTASKEXPRESSIONLISTCTRL_H__42272309_6C54_4901_A56D_D6FDA87F1E48__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLFindTaskExpressionListCtrl.h : header file
//

#include "TDCEnum.h"
#include "TDCStruct.h"
#include "../../Common/InputListCtrl.h"
#include "../../Common/TimeEdit.h"

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CTDLFindTaskExpressionListCtrl window

class CTDLFindTaskExpressionListCtrl : public CInputListCtrl
{
// Construction
public:
	CTDLFindTaskExpressionListCtrl();

	void SetSearchParams(const SEARCHPARAM& param);
	void SetSearchParams(const CSearchParamArray& params);
	int GetSearchParams(CSearchParamArray& params) const;

	void ClearSearch();

	BOOL AddRule();
	BOOL DeleteSelectedRule();
	BOOL CanDeleteSelectedRule() const
	{
		return CanDeleteSelectedCell();
	}
	BOOL HasRules() const
	{
		return m_aSearchParams.GetSize();
	}

	void MoveSelectedRuleUp();
	BOOL CanMoveSelectedRuleUp() const;
	void MoveSelectedRuleDown();
	BOOL CanMoveSelectedRuleDown() const;

// Attributes
protected:
	CComboBox   m_cbOperators;
	CComboBox   m_cbAttributes;
	CComboBox   m_cbAndOr;
	CDateTimeCtrl m_dtDate;
	CTimeEdit   m_eTime;

	CSearchParamArray m_aSearchParams;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLFindTaskExpressionListCtrl)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTDLFindTaskExpressionListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTDLFindTaskExpressionListCtrl)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnAttribEditCancel();
	afx_msg void OnAttribEditOK();
	afx_msg void OnOperatorEditCancel();
	afx_msg void OnOperatorEditOK();
	afx_msg void OnAndOrEditCancel();
	afx_msg void OnAndOrEditOK();
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	afx_msg void OnValueEditOK(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSelItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDateChange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDateCloseUp(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTimeChange();
	afx_msg LRESULT OnTimeUnitsChange(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	virtual void EditCell(int nItem, int nCol);
	virtual BOOL IsEditing() const;
	virtual BOOL DeleteSelectedCell();
	virtual BOOL CanEditSelectedCell() const;

	void PrepareEdit(int nRow, int nCol);
	void PrepareControl(CWnd& ctrl, int nRow, int nCol);
	int GetValueType(int nRow) const;
	void BuildListCtrl();
	int InsertRule(int nRow, const SEARCHPARAM& sp);
	int GetRuleCount() const
	{
		return m_aSearchParams.GetSize();
	}
	CWnd* GetEditControl(int nRow, int nCol);
	void ValidateListData() const;

	static CString GetAttribName(TDC_ATTRIBUTE attrib);
	static CString GetOpName(FIND_OPERATOR op);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FINDTASKEXPRESSIONLISTCTRL_H__42272309_6C54_4901_A56D_D6FDA87F1E48__INCLUDED_)
