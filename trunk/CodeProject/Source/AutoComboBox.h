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
// - taken out from the original ToDoList package for better sharing
// - merged with ToDoList version 6.1 sources
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

#if !defined(AFX_AUTOCOMBOBOX_H__34DD95FF_CCAE_4D8E_9162_D860B65CD448__INCLUDED_)
#define AFX_AUTOCOMBOBOX_H__34DD95FF_CCAE_4D8E_9162_D860B65CD448__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// AutoComboBox.h : header file
//

#include "Subclass.h"
#include "Misc.h"

const UINT WM_ACB_ITEMADDED = ::RegisterWindowMessage(_T("WM_ACB_ITEMADDED"));
const UINT WM_ACB_ITEMDELETED = ::RegisterWindowMessage(_T("WM_ACB_ITEMDELETED"));

enum
{
	ACBS_ALLOWDELETE   = 0x01,
	ACBS_CASESENSITIVE = 0x02,
	ACBS_ADDTOSTART    = 0x04,
};

/////////////////////////////////////////////////////////////////////////////
// CAutoComboBox window

// replacement DDX routine
void AFXAPI DDX_AutoCBString(CDataExchange* pDX, int nIDC, CString& value);

class CAutoComboBox : public CComboBox, protected CSubclassWnd
{
	// Construction
public:
	CAutoComboBox(DWORD dwFlags = 0);
	virtual ~CAutoComboBox();

	// Operations
public:
	virtual int AddString(LPCTSTR szItem)
	{
		return AddUniqueItem(szItem);
	}
	virtual int InsertString(int nIndex, LPCTSTR szItem)
	{
		return InsertUniqueItem(nIndex, szItem);
	}

	virtual int DeleteString(UINT nIndex)
	{
		return CComboBox::DeleteString(nIndex);
	}
	virtual int DeleteString(LPCTSTR szItem, BOOL bCaseSensitive = FALSE);

	virtual int DeleteItems(const CStringArray& aItems, BOOL bCaseSensitive = FALSE); // returns deleted count

	virtual int AddUniqueItem(const CString& sItem); // returns index or CB_ERR
	virtual int InsertUniqueItem(int nIndex, const CString& sItem); // returns index or CB_ERR

	virtual int AddUniqueItems(const CStringArray& aItems); // returns num items added
	virtual int AddUniqueItems(const CAutoComboBox& cbItems); // returns num items added

	virtual int GetItems(CStringArray& aItems) const; // returns item count
	virtual int SelectString(int nStartAfter, LPCTSTR lpszString)
	{
		return CComboBox::SelectString(nStartAfter, lpszString);
	}

	// helpers
	int FindStringExact(int nIndexStart, const CString& sItem, BOOL bCaseSensitive) const;
	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
	{
		return FindStringExact(nIndexStart, lpszFind, FALSE);
	}

	void Flush()
	{
		if (m_bEditChange)
		{
			HandleReturnKey();
		}
	}

protected:
	DWORD m_dwFlags;
	HWND m_hwndListbox;
	BOOL m_bEditChange;
	BOOL m_bClosingUp;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoComboBox)
protected:
	//}}AFX_VIRTUAL
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);

	// Generated message map functions
protected:
	//{{AFX_MSG(CAutoComboBox)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	afx_msg BOOL OnSelEndCancel();
	afx_msg BOOL OnSelEndOK();
	afx_msg BOOL OnSelChange();
	afx_msg BOOL OnDropDown();
	afx_msg BOOL OnCloseUp();
	afx_msg BOOL OnEditChange();
	//	afx_msg BOOL OnKillFocus();

	DECLARE_MESSAGE_MAP()

protected:
	virtual BOOL DeleteSelectedLBItem();
	virtual void RefreshMaxDropWidth();

	BOOL AllowDelete() const
	{
		return Misc::HasFlag(m_dwFlags, ACBS_ALLOWDELETE);
	}
	BOOL CaseSensitive() const
	{
		return Misc::HasFlag(m_dwFlags, ACBS_CASESENSITIVE);
	}
	BOOL AddToStart() const
	{
		return Misc::HasFlag(m_dwFlags, ACBS_ADDTOSTART);
	}
	int AddUniqueItem(const CString& sItem, BOOL bAddToStart);

	BOOL IsSimpleCombo();
	virtual void HandleReturnKey();
	void NotifyParent(UINT nIDNotify);
	virtual CString GetSelectedItem() const;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUTOCOMBOBOX_H__34DD95FF_CCAE_4D8E_9162_D860B65CD448__INCLUDED_)
