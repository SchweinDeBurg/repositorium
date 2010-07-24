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
//*****************************************************************************

// PreferencesUIPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesUIPage.h"
#include "TDCEnum.h"

#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/OSVersion.h"
#include "../../../CodeProject/Source/Themed.h"
#include "../../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUIPage property page

IMPLEMENT_DYNCREATE(CPreferencesUIPage, CPreferencesPageBase)

CPreferencesUIPage::CPreferencesUIPage(const CContentMgr* pMgr) : 
	CPreferencesPageBase(CPreferencesUIPage::IDD), 
		m_pContentMgr(pMgr), 
		m_cbCommentsFmt(pMgr),
		m_nDefaultCommentsFormat(-1),
		m_eUITheme(FES_COMBOSTYLEBTN, CEnString(IDS_UITHEMEFILEFILTER))
{
	//{{AFX_DATA_INIT(CPreferencesUIPage)
	m_bAutoRefilter = FALSE;
	m_sUIThemeFile = _T("");
	m_bUseUITheme = FALSE;
	m_bEnableLightboxMgr = FALSE;
	//}}AFX_DATA_INIT

}

CPreferencesUIPage::~CPreferencesUIPage()
{
}

void CPreferencesUIPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesUIPage)
	DDX_Control(pDX, IDC_UITHEMEFILE, m_eUITheme);
	DDX_Check(pDX, IDC_SHOWCTRLSASCOLUMNS, m_bShowCtrlsAsColumns);
	DDX_Check(pDX, IDC_SHOWCOMMENTSALWAYS, m_bShowCommentsAlways);
	DDX_Check(pDX, IDC_AUTOREPOSCTRLS, m_bAutoReposCtrls);
	DDX_Check(pDX, IDC_SHAREDCOMMENTSHEIGHT, m_bSharedCommentsHeight);
	DDX_Check(pDX, IDC_AUTOHIDETABBAR, m_bAutoHideTabbar);
	DDX_Check(pDX, IDC_STACKTABBARITEMS, m_bStackTabbarItems);
	DDX_Check(pDX, IDC_RIGHTALIGNLABELS, m_bRightAlignLabels);
	DDX_Check(pDX, IDC_FOCUSTREEONENTER, m_bFocusTreeOnEnter);
	DDX_CBIndex(pDX, IDC_NEWTASKPOSITION, m_nNewTaskPos);
	DDX_CBIndex(pDX, IDC_NEWSUBTASKPOSITION, m_nNewSubtaskPos);
	DDX_Check(pDX, IDC_KEEPTABSORDERED, m_bKeepTabsOrdered);
	DDX_Check(pDX, IDC_SHOWTASKLISTCLOSEBTN, m_bShowTasklistCloseButton);
	DDX_Check(pDX, IDC_SHOWEDITMENUASCOLUMNS, m_bShowEditMenuAsColumns);
	DDX_Check(pDX, IDC_MULTISELFILTER, m_bMultiSelFilters);
	DDX_Check(pDX, IDC_RESTORETASKLISTFILTERS, m_bRestoreTasklistFilters);
	DDX_Check(pDX, IDC_CHECK4, m_bAutoRefilter);
	DDX_Text(pDX, IDC_UITHEMEFILE, m_sUIThemeFile);
	DDX_Check(pDX, IDC_USEUITHEME, m_bUseUITheme);
	DDX_CBIndex(pDX, IDC_COMMENTPOS, m_nCommentsPos);
	DDX_Check(pDX, IDC_ENABLELIGHTBOXMGR, m_bEnableLightboxMgr);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMMENTSFORMAT, m_cbCommentsFmt);
	DDX_CBIndex(pDX, IDC_COMMENTSFORMAT, m_nDefaultCommentsFormat);
	DDX_Check(pDX, IDC_SORTDONETASKSATBOTTOM, m_bSortDoneTasksAtBottom);
//	DDX_Check(pDX, IDC_RTLCOMMENTS, m_bRTLComments);
}

BEGIN_MESSAGE_MAP(CPreferencesUIPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesUIPage)
	ON_BN_CLICKED(IDC_USEUITHEME, OnUseuitheme)
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMMENTSFORMAT, OnSelchangeCommentsformat)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesUIPage message handlers

BOOL CPreferencesUIPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_TOOLBARGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_SORTGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_TABBARGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_COMMENTSGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_FILTERGROUP, *this);

	m_cbCommentsFmt.SetCurSel(m_nDefaultCommentsFormat);
	GetDlgItem(IDC_COMMENTSFORMAT)->EnableWindow(m_nDefaultCommentsFormat != CB_ERR);

	// theming only available if XP themes are active
	BOOL bThemeActive = CThemed::IsThemeActive();
	GetDlgItem(IDC_USEUITHEME)->EnableWindow(bThemeActive);
	GetDlgItem(IDC_UITHEMEFILE)->EnableWindow(m_bUseUITheme && bThemeActive);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesUIPage::OnSelchangeCommentsformat() 
{
	m_cbCommentsFmt.GetSelectedFormat(m_cfDefault);
}

void CPreferencesUIPage::LoadPreferences(const CPreferences& prefs)
{
	// load settings
	m_bShowCtrlsAsColumns = prefs.GetProfileInt(_T("Preferences"), _T("ShowCtrlsAsColumns"), FALSE);
	m_bShowEditMenuAsColumns = prefs.GetProfileInt(_T("Preferences"), _T("ShowEditMenuAsColumns"), FALSE);
	m_bShowCommentsAlways = prefs.GetProfileInt(_T("Preferences"), _T("ShowCommentsAlways"), FALSE);
	m_bAutoReposCtrls = prefs.GetProfileInt(_T("Preferences"), _T("AutoReposCtrls"), TRUE);
	m_bSpecifyToolbarImage = prefs.GetProfileInt(_T("Preferences"), _T("SpecifyToolbarImage"), FALSE);
	m_bSharedCommentsHeight = prefs.GetProfileInt(_T("Preferences"), _T("SharedCommentsHeight"), TRUE);
	m_bAutoHideTabbar = prefs.GetProfileInt(_T("Preferences"), _T("AutoHideTabbar"), TRUE);
	m_bStackTabbarItems = prefs.GetProfileInt(_T("Preferences"), _T("StackTabbarItems"), FALSE);
	m_bRightAlignLabels = prefs.GetProfileInt(_T("Preferences"), _T("RightAlignLabels"), TRUE);
	m_bFocusTreeOnEnter = prefs.GetProfileInt(_T("Preferences"), _T("FocusTreeOnEnter"), FALSE);
	m_nNewTaskPos = prefs.GetProfileInt(_T("Preferences"), _T("NewTaskPos"), PUIP_BELOW);
	m_nNewSubtaskPos = prefs.GetProfileInt(_T("Preferences"), _T("NewSubtaskPos"), PUIP_BOTTOM);
	m_bKeepTabsOrdered = prefs.GetProfileInt(_T("Preferences"), _T("KeepTabsOrdered"), FALSE);
	m_bShowTasklistCloseButton = prefs.GetProfileInt(_T("Preferences"), _T("ShowTasklistCloseButton"), TRUE);
	m_bSortDoneTasksAtBottom = prefs.GetProfileInt(_T("Preferences"), _T("SortDoneTasksAtBottom"), TRUE);
	m_bRTLComments = prefs.GetProfileInt(_T("Preferences"), _T("RTLComments"), FALSE);
	m_nCommentsPos = prefs.GetProfileInt(_T("Preferences"), _T("VertComments"), PUIP_RIGHTCOMMENTS);
	m_bMultiSelFilters = prefs.GetProfileInt(_T("Preferences"), _T("MultiSelFilters"), TRUE);
	m_bRestoreTasklistFilters = prefs.GetProfileInt(_T("Preferences"), _T("RestoreTasklistFilters"), TRUE);
	m_bAutoRefilter = prefs.GetProfileInt(_T("Preferences"), _T("AutoRefilter"), TRUE);
	m_bEnableLightboxMgr = prefs.GetProfileInt(_T("Preferences"), _T("EnableLightboxMgr"), COSVersion() <= OSV_XP);
	m_bUseUITheme = CThemed::IsThemeActive() && prefs.GetProfileInt(_T("Preferences"), _T("UseUITheme"), TRUE);

	// set default theme to blue
	if (CThemed::IsThemeActive())
	{
		CString sDefault = FileMisc::GetModuleFolder() + _T("Resources\\ThemeBlue.xml");
		m_sUIThemeFile = prefs.GetProfileString(_T("Preferences"), _T("UIThemeFile"), sDefault);
	}
	else
		m_sUIThemeFile.Empty();

	// comments format
	if (m_cbCommentsFmt.IsInitialized())
	{
		m_cfDefault = prefs.GetProfileString(_T("Preferences"), _T("DefaultCommentsFormatID"));
		m_nDefaultCommentsFormat = m_cbCommentsFmt.SetSelectedFormat(m_cfDefault);

		// fallback
		if (m_nDefaultCommentsFormat == CB_ERR)
			m_nDefaultCommentsFormat = prefs.GetProfileInt(_T("Preferences"), _T("DefaultCommentsFormat"), -1);

		if (m_nDefaultCommentsFormat == CB_ERR || m_nDefaultCommentsFormat >= m_cbCommentsFmt.GetCount())
		{
			ASSERT (m_cbCommentsFmt.GetCount());

			m_nDefaultCommentsFormat = 0;
		}

		m_cbCommentsFmt.SetCurSel(m_nDefaultCommentsFormat);
		m_cbCommentsFmt.GetSelectedFormat(m_cfDefault);
	}
}

void CPreferencesUIPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt(_T("Preferences"), _T("ShowCtrlsAsColumns"), m_bShowCtrlsAsColumns);
	prefs.WriteProfileInt(_T("Preferences"), _T("ShowEditMenuAsColumns"), m_bShowEditMenuAsColumns);
	prefs.WriteProfileInt(_T("Preferences"), _T("ShowCommentsAlways"), m_bShowCommentsAlways);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoReposCtrls"), m_bAutoReposCtrls);
	prefs.WriteProfileInt(_T("Preferences"), _T("SpecifyToolbarImage"), m_bSpecifyToolbarImage);
	prefs.WriteProfileInt(_T("Preferences"), _T("SharedCommentsHeight"), m_bSharedCommentsHeight);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoHideTabbar"), m_bAutoHideTabbar);
	prefs.WriteProfileInt(_T("Preferences"), _T("StackTabbarItems"), m_bStackTabbarItems);
	prefs.WriteProfileInt(_T("Preferences"), _T("RightAlignLabels"), m_bRightAlignLabels);
	prefs.WriteProfileInt(_T("Preferences"), _T("FocusTreeOnEnter"), m_bFocusTreeOnEnter);
	prefs.WriteProfileInt(_T("Preferences"), _T("NewTaskPos"), m_nNewTaskPos);
	prefs.WriteProfileInt(_T("Preferences"), _T("NewSubtaskPos"), m_nNewSubtaskPos);
	prefs.WriteProfileInt(_T("Preferences"), _T("KeepTabsOrdered"), m_bKeepTabsOrdered);
	prefs.WriteProfileInt(_T("Preferences"), _T("ShowTasklistCloseButton"), m_bShowTasklistCloseButton);
	prefs.WriteProfileInt(_T("Preferences"), _T("SortDoneTasksAtBottom"), m_bSortDoneTasksAtBottom);
	prefs.WriteProfileInt(_T("Preferences"), _T("RTLComments"), m_bRTLComments);
	prefs.WriteProfileInt(_T("Preferences"), _T("VertComments"), m_nCommentsPos);
	prefs.WriteProfileInt(_T("Preferences"), _T("MultiSelFilters"), m_bMultiSelFilters);
	prefs.WriteProfileInt(_T("Preferences"), _T("RestoreTasklistFilters"), m_bRestoreTasklistFilters);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoRefilter"), m_bAutoRefilter);
	prefs.WriteProfileString(_T("Preferences"), _T("UIThemeFile"), m_sUIThemeFile);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseUITheme"), m_bUseUITheme);
	prefs.WriteProfileInt(_T("Preferences"), _T("EnableLightboxMgr"), m_bEnableLightboxMgr);

	// comments format
	if (m_pContentMgr)
	{
		prefs.WriteProfileInt(_T("Preferences"), _T("DefaultCommentsFormat"), m_nDefaultCommentsFormat);
		prefs.WriteProfileString(_T("Preferences"), _T("DefaultCommentsFormatID"), m_cfDefault);
	}
}

void CPreferencesUIPage::OnUseuitheme() 
{
	UpdateData();
	GetDlgItem(IDC_UITHEMEFILE)->EnableWindow(m_bUseUITheme);
}
