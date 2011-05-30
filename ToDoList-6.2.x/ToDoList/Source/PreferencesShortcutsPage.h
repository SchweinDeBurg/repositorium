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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#if !defined(AFX_PREFERENCESSHORTCUTSPAGE_H__DA5D005D_C6CC_453A_A431_A2B85A920CE5__INCLUDED_)
#define AFX_PREFERENCESSHORTCUTSPAGE_H__DA5D005D_C6CC_453A_A431_A2B85A920CE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PrefererencesShortcutsPage.h : header file
//

#include "../../Common/ShortcutManager.h"
#include "../../../CodeProject/Source/OrderedTreeCtrl.h"
#include "../../../CodeProject/Source/HotKeyCtrlEx.h"
#include "../../Common/PreferencesBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesShortcutsPage dialog

#define PSP_SHORTCUTCOLUMNID (OTC_POSCOLUMNID - 1)

class CPreferencesShortcutsPage : public CPreferencesPageBase
{
// Construction
public:
	CPreferencesShortcutsPage(CShortcutManager* pMgr, UINT nMenuID, BOOL bIgnoreGrayedItems = TRUE);
	~CPreferencesShortcutsPage();

	UINT GetShortcutCmdID(DWORD dwShortcut)
	{
		return m_pShortcutMgr->GetCommandID(dwShortcut);
	}

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesShortcutsPage)
	enum { IDD = IDD_PREFSHORTCUTS_PAGE };
	CHotKeyCtrlEx   m_hkCur;
	COrderedTreeCtrl    m_tcCommands;
	CHotKeyCtrlEx   m_hkNew;
	CString m_sOtherCmdID;
	//}}AFX_DATA

	CShortcutManager* m_pShortcutMgr;
	UINT m_nMenuID;
	CMap<UINT, UINT, DWORD, DWORD&> m_mapID2Shortcut;
	CMap<DWORD, DWORD, HTREEITEM, HTREEITEM&> m_mapShortcut2HTI;
	BOOL m_bIgnoreGrayedItems;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesShortcutsPage)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesShortcutsPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangedShortcuts(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAssignshortcut();
	//}}AFX_MSG
	afx_msg void OnChangeShortcut();
	afx_msg LRESULT OnGutterDrawItem(WPARAM wParam, LPARAM lParam); // for drawing priority
	afx_msg LRESULT OnGutterRecalcColWidth(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGutterGetItemColors(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTreeCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	DECLARE_MESSAGE_MAP()

	HTREEITEM AddMenuItem(HTREEITEM htiParent, const CMenu* pMenu, int nPos);
	int GetLongestShortcutText(HTREEITEM hti, CDC* pDC);

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERERENCESSHORTCUTSPAGE_H__DA5D005D_C6CC_453A_A431_A2B85A920CE5__INCLUDED_)
