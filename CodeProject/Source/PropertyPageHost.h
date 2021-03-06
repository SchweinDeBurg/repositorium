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
// - merged with ToDoList version 6.1 sources
//*****************************************************************************

#if !defined(AFX_PROPERTYPAGEHOST_H__43CF5AE7_C70B_443D_BC8B_7DA1D0E082DD__INCLUDED_)
#define AFX_PROPERTYPAGEHOST_H__43CF5AE7_C70B_443D_BC8B_7DA1D0E082DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertyPageHost.h : header file
//

#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////
// CPropertyPageHost window

struct PAGEITEM
{
	PAGEITEM(CPropertyPage* _pPage = NULL, LPCTSTR szTitle = NULL, DWORD dwData = 0):
	pPage(_pPage),
	sTitle(szTitle),
	dwItemData(dwData)
	{
	}

	CPropertyPage* pPage;
	CString sTitle;
	DWORD dwItemData;
};

class CPropertyPageHost : public CWnd
{
	// Construction
public:
	CPropertyPageHost();

	BOOL Create(LPRECT lpRect, CWnd* pParent, UINT uCtrlID = AFX_IDW_PANE_FIRST);
	BOOL Create(UINT nRefCtrlID, CWnd* pParent, UINT uCtrlID = AFX_IDW_PANE_FIRST);
	void OnOK();
	void OnApply();

	int GetActiveIndex() const;
	int FindPage(CPropertyPage* pPage);

	CPropertyPage* GetActivePage();
	int GetPageIndex(const CPropertyPage* pPage) const;
	CPropertyPage* GetPage(int nIndex);
	const CPropertyPage* GetPage(int nIndex) const;
	CPropertyPage* FindPage(DWORD dwItemData);

	BOOL AddPage(CPropertyPage* pPage, LPCTSTR szTitle = NULL, DWORD dwItemData = 0);
	BOOL SetActivePage(int nIndex, BOOL bAndFocus = TRUE);
	BOOL SetActivePage(CPropertyPage* pPage, BOOL bAndFocus = TRUE);
	int GetPageCount()
	{
		return m_aPages.GetSize();
	}
	CString GetPageTitle(int nIndex) const;
	DWORD GetPageItemData(int nIndex) const;

	BOOL EnsurePageCreated(int nIndex);
	BOOL EnsurePageCreated(const CPropertyPage* pPage);
	BOOL IsPageCreated(int nIndex) const;

	void ForwardMessage(UINT nMsg)
	{
		m_aForwardMsgs.Add(nMsg);
	}

protected:
	CArray<PAGEITEM, PAGEITEM&> m_aPages;
	int m_nSelIndex;
	CUIntArray m_aForwardMsgs;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPageHost)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CPropertyPageHost();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyPageHost)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg LRESULT OnGetCurrentPageHwnd(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYPAGEHOST_H__43CF5AE7_C70B_443D_BC8B_7DA1D0E082DD__INCLUDED_)
