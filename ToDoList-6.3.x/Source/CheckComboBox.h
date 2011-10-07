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
// - taken out from the original ToDoList package for better sharing
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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

#if !defined(AFX_CHECKCOMBOBOX_H__4DDD3C3D_BDB7_4F07_9C50_75495F6E7F66__INCLUDED_)
#define AFX_CHECKCOMBOBOX_H__4DDD3C3D_BDB7_4F07_9C50_75495F6E7F66__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// CheckComboBox.h : header file
//

#include "AutoComboBox.h"
#include "Subclass.h"

/////////////////////////////////////////////////////////////////////////////
// CCheckComboBox window

class CCheckComboBox : public CAutoComboBox, public CSubclasser
{
// Construction
public:
	CCheckComboBox(DWORD dwFlags = 0);

	BOOL GetCheck(int nIndex) const;
	int GetChecked(CStringArray& aItems) const;
	void SetChecked(const CStringArray& aItems);
	int SetCheck(int nIndex, BOOL bCheck = TRUE);
	void CheckAll(BOOL bCheck = TRUE);
	int GetCheckedCount() const;

	BOOL IsAnyChecked() const;
	BOOL IsAnyUnchecked() const;

	CString FormatCheckedItems(LPCTSTR szSep = NULL) const;
	CString GetTooltip() const;

	virtual int AddUniqueItem(const CString& sItem); // returns index or CB_ERR
	virtual int SelectString(int nStartAfter, LPCTSTR lpszString);
	int GetCurSel() const;

protected:
	CString m_sText;
	BOOL m_bItemHeightSet;
	mutable BOOL m_bDrawing;
	BOOL m_bTextFits;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckComboBox)
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// for listbox
	virtual LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

	// for editbox
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

// Implementation
public:
	virtual ~CCheckComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckComboBox)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg BOOL OnEditchange();
	afx_msg BOOL OnDropdown();
	afx_msg void OnLBSelChange();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()

	virtual void OnCheckChange(int /*nIndex*/)
	{
	}

protected:
	void RecalcText(BOOL bUpdate = TRUE, BOOL bNotify = TRUE);
	void ParseText();
	BOOL IsType(UINT nComboType);
	virtual BOOL DeleteSelectedLBItem();
	virtual void DrawItemText(HDC hdc, int nItem, CRect rText, const CString& sText, UINT nState);
	virtual void RefreshMaxDropWidth();
	virtual void HandleReturnKey();
	virtual CString GetSelectedItem() const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHECKCOMBOBOX_H__4DDD3C3D_BDB7_4F07_9C50_75495F6E7F66__INCLUDED_)
