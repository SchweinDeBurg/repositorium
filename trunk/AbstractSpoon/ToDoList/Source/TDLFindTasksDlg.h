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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#if !defined(AFX_TDLFINDTASKSDLG_H__9118493D_32FD_434D_B549_8947D00277CD__INCLUDED_)
#define AFX_TDLFINDTASKSDLG_H__9118493D_32FD_434D_B549_8947D00277CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLFindTasksDlg.h : header file
//
#include "TDLFindTaskExpressionListCtrl.h"
#include "TDLFindResultsListCtrl.h"
#include "TDLFilterOptionComboBox.h"

#include "../../../CodeProject/Source/DockManager.h"
#include "../../../CodeProject/Source/EnToolBar.h"
#include "../../../CodeProject/Source/ToolBarHelper.h"
#include "../../Common/TabbedComboBox.h"
#include "../../../CodeProject/Source/WndPrompt.h"
#include "../../Common/UITheme.h"

#include "../../../CodeProject/Source/LockableHeaderCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLFindTasksDlg dialog

const UINT WM_FTD_FIND = ::RegisterWindowMessage(_T("WM_FTD_FIND"));
const UINT WM_FTD_SELECTRESULT = ::RegisterWindowMessage(_T("WM_FTD_SELECTRESULT"));
const UINT WM_FTD_SELECTALL = ::RegisterWindowMessage(_T("WM_FTD_SELECTALL"));
const UINT WM_FTD_CLOSE = ::RegisterWindowMessage(_T("WM_FTD_CLOSE"));
const UINT WM_FTD_APPLYASFILTER = ::RegisterWindowMessage(_T("WM_FTD_APPLYASFILTER"));

class CTDLFindTasksDlg : public CDialog
{
// Construction
public:
	CTDLFindTasksDlg(CWnd* pParent = NULL);   // standard constructor
	~CTDLFindTasksDlg();

	BOOL Initialize(CWnd* pParent, BOOL bDockable = TRUE);
	BOOL Show(BOOL bShow = TRUE);
	void RefreshSearch();

	BOOL GetSearchAllTasklists();
	int GetSearchParams(SEARCHPARAMS& params);
	BOOL GetAutoSelectSingles();

	void AddHeaderRow(LPCTSTR szText, BOOL bSpaceAbove = TRUE);
	void AddResult(const SEARCHRESULT& result, LPCTSTR szTask, LPCTSTR szPath, const CFilteredToDoCtrl* pTDC);

	int GetResultCount() const; // all tasklists
	int GetResultCount(const CFilteredToDoCtrl* pTDC) const;
	int GetAllResults(CFTDResultsArray& aResults) const;
	int GetResults(const CFilteredToDoCtrl* pTDC, CFTDResultsArray& aResults) const;
	int GetResultIDs(const CFilteredToDoCtrl* pTDC, CDWordArray& aTaskIDs) const;
	void DeleteResults(const CFilteredToDoCtrl* pTDC);

	void RefreshUserPreferences()
	{
		m_lcResults.RefreshUserPreferences();
	}

	void SetUITheme(const UITHEME& theme);

protected:
// Dialog Data
	//{{AFX_DATA(CTDLFindTasksDlg)
	enum { IDD = IDD_FINDTASKS_DIALOG };
	CCheckComboBox m_cbInclude;
	CComboBox m_cbSearches;
	//}}AFX_DATA
	CTDLFindTaskExpressionListCtrl m_lcFindSetup;
	CTDLFindResultsListCtrl m_lcResults;

	CWndPromptManager m_mgrPrompts;
	CDockManager m_dockMgr;
	CEnToolBar m_toolbar;
	CToolbarHelper m_tbHelper;
	BOOL m_bDockable;
	int m_nCurSel;
	int m_bAllTasklists;
	CString m_sResultsLabel;
	BOOL m_bAutoSelectSingles;
	CString m_sActiveSearch;
	UITHEME m_theme;
	CBrush m_brBkgnd;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLFindTasksDlg)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnCancel()
	{
		ShowWindow(SW_HIDE);
	}
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	int DoModal()
	{
		return -1;   // not for public use
	}

	// Generated message map functions
	//{{AFX_MSG(CTDLFindTasksDlg)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnApplyasfilter();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	afx_msg void OnAddRule();
	afx_msg void OnDeleteRule();
	afx_msg void OnUpdateDeleteRule(CCmdUI* pCmdUI);
	afx_msg void OnFind();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSelectall();
	afx_msg void OnDockright();
	afx_msg void OnUpdateDockright(CCmdUI* pCmdUI);
	afx_msg void OnDockleft();
	afx_msg void OnUpdateDockleft(CCmdUI* pCmdUI);
	afx_msg void OnUndock();
	afx_msg void OnUpdateUndock(CCmdUI* pCmdUI);
	afx_msg void OnDockbelow();
	afx_msg void OnUpdateDockbelow(CCmdUI* pCmdUI);
	afx_msg void OnSearchresults();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnItemchangingResults(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSaveSearch();
	afx_msg void OnItemchangedRulelist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeSearchlist();
	afx_msg void OnNewSearch();
	afx_msg void OnDeleteSearch();
	afx_msg void OnEditchangeSearchlist();
	afx_msg void OnUpdateDeleteSearch(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSaveSearch(CCmdUI* pCmdUI);
	afx_msg void OnMoveRuleUp();
	afx_msg void OnUpdateMoveRuleUp(CCmdUI* pCmdUI);
	afx_msg void OnMoveRuleDown();
	afx_msg void OnUpdateMoveRuleDown(CCmdUI* pCmdUI);
	afx_msg void OnItemActivated(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

protected:
	void SaveSettings();
	void ResizeDlg(int cx = 0, int cy = 0);
	void LoadSettings();
	CSize GetMinDockedSize(DM_POS nPos);
	int GetNextResult(int nItem, BOOL bDown);
	void SelectItem(int nItem);
	int GetSelectedItem();
	CString GetCurrentSearch(BOOL bSelected);
	BOOL InitializeToolbar();
	void DeleteAllResults();
	void EnableApplyAsFilterButton();

	BOOL LoadSearch(LPCTSTR szName, CSearchParamArray& params) const;
	BOOL SaveSearch(LPCTSTR szName, const CSearchParamArray& params);
	int LoadSearches();
	int SaveSearches();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLFINDTASKSDLG_H__9118493D_32FD_434D_B549_8947D00277CD__INCLUDED_)
