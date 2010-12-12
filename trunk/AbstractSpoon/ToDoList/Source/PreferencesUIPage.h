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

#if !defined(AFX_PREFERENCESUIPAGE_H__5AE787F2_44B0_4A48_8D75_24C6C16B45DF__INCLUDED_)
#define AFX_PREFERENCESUIPAGE_H__5AE787F2_44B0_4A48_8D75_24C6C16B45DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesUIPage.h : header file
//

#include "../../Common/ContentMgr.h"
#include "../../Common/FileEdit.h"
#include "../../Common/GroupLine.h"
#include "../../Common/PreferencesBase.h"
#include "../../Common/ContentTypeComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUIPage dialog

enum
{
	PUIP_TOP,
	PUIP_BOTTOM,
	PUIP_ABOVE,
	PUIP_BELOW,
};

enum
{
	PUIP_BOTTOMCOMMENTS,
	PUIP_RIGHTCOMMENTS,
};

class CPreferencesUIPage : public CPreferencesPageBase
{
	DECLARE_DYNCREATE(CPreferencesUIPage)

// Construction
public:
	CPreferencesUIPage(const CContentMgr* pMgr = NULL);
	~CPreferencesUIPage();

	BOOL GetShowCtrlsAsColumns() const
	{
		return m_bShowCtrlsAsColumns;
	}
	BOOL GetShowEditMenuAsColumns() const
	{
		return m_bShowEditMenuAsColumns;
	}
	BOOL GetShowCommentsAlways() const
	{
		return m_bShowCommentsAlways;
	}
	BOOL GetAutoReposCtrls() const
	{
		return m_bAutoReposCtrls;
	}
	BOOL GetSharedCommentsHeight() const
	{
		return m_bSharedCommentsHeight;
	}
	BOOL GetAutoHideTabbar() const
	{
		return m_bAutoHideTabbar;
	}
	BOOL GetStackTabbarItems() const
	{
		return m_bStackTabbarItems;
	}
	BOOL GetRightAlignLabels() const
	{
		return m_bRightAlignLabels;
	}
	BOOL GetFocusTreeOnEnter() const
	{
		return m_bFocusTreeOnEnter;
	}
	int GetNewTaskPos() const
	{
		return m_nNewTaskPos;
	}
	int GetNewSubtaskPos() const
	{
		return m_nNewSubtaskPos;
	}
	BOOL GetKeepTabsOrdered() const
	{
		return m_bKeepTabsOrdered;
	}
	BOOL GetShowTasklistCloseButton() const
	{
		return m_bShowTasklistCloseButton;
	}
	BOOL GetSortDoneTasksAtBottom() const
	{
		return m_bSortDoneTasksAtBottom;
	}
	BOOL GetRTLComments() const
	{
		return m_bRTLComments;
	}
	int GetCommentsPos() const
	{
		return m_nCommentsPos;
	}
	CONTENTFORMAT GetDefaultCommentsFormat() const
	{
		return m_cfDefault;
	}
	BOOL GetMultiSelFilters() const
	{
		return m_bMultiSelFilters;
	}
	BOOL GetRestoreTasklistFilters() const
	{
		return m_bRestoreTasklistFilters;
	}
	BOOL GetReFilterOnModify() const
	{
		return m_bAutoRefilter;
	}
	CString GetUITheme() const;
	BOOL GetEnableLightboxMgr() const
	{
		return m_bEnableLightboxMgr;
	}
protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesUIPage)
	enum { IDD = IDD_PREFUI_PAGE };
	CFileEdit   m_eUITheme;
	BOOL    m_bShowCtrlsAsColumns;
	BOOL    m_bShowCommentsAlways;
	BOOL    m_bAutoReposCtrls;
	BOOL    m_bSpecifyToolbarImage;
	BOOL    m_bSharedCommentsHeight;
	BOOL    m_bAutoHideTabbar;
	BOOL    m_bStackTabbarItems;
	BOOL    m_bRightAlignLabels;
	BOOL    m_bFocusTreeOnEnter;
	int     m_nNewTaskPos;
	int     m_nNewSubtaskPos;
	BOOL    m_bKeepTabsOrdered;
	BOOL    m_bShowTasklistCloseButton;
	BOOL    m_bRTLComments;
	BOOL    m_bShowEditMenuAsColumns;
	BOOL    m_bMultiSelFilters;
	BOOL    m_bRestoreTasklistFilters;
	BOOL    m_bAutoRefilter;
	CString m_sUIThemeFile;
	BOOL    m_bUseUITheme;
	BOOL    m_bEnableLightboxMgr;
	//}}AFX_DATA
	int     m_nCommentsPos;
	CContentTypeComboBox    m_cbCommentsFmt;
	BOOL    m_bSortDoneTasksAtBottom;
	const CContentMgr* m_pContentMgr;
	CGroupLineManager m_mgrGroupLines;
	CONTENTFORMAT m_cfDefault;
	int m_nDefaultCommentsFormat;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesUIPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesUIPage)
	afx_msg void OnUseuitheme();
	//}}AFX_MSG
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCommentsformat();
	DECLARE_MESSAGE_MAP()

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESUIPAGE_H__5AE787F2_44B0_4A48_8D75_24C6C16B45DF__INCLUDED_)
