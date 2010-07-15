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
	m_bShowCtrlsAsColumns = prefs.GetProfileInt("Preferences", "ShowCtrlsAsColumns", FALSE);
	m_bShowEditMenuAsColumns = prefs.GetProfileInt("Preferences", "ShowEditMenuAsColumns", FALSE);
	m_bShowCommentsAlways = prefs.GetProfileInt("Preferences", "ShowCommentsAlways", FALSE);
	m_bAutoReposCtrls = prefs.GetProfileInt("Preferences", "AutoReposCtrls", TRUE);
	m_bSpecifyToolbarImage = prefs.GetProfileInt("Preferences", "SpecifyToolbarImage", FALSE);
	m_bSharedCommentsHeight = prefs.GetProfileInt("Preferences", "SharedCommentsHeight", TRUE);
	m_bAutoHideTabbar = prefs.GetProfileInt("Preferences", "AutoHideTabbar", TRUE);
	m_bStackTabbarItems = prefs.GetProfileInt("Preferences", "StackTabbarItems", FALSE);
	m_bRightAlignLabels = prefs.GetProfileInt("Preferences", "RightAlignLabels", TRUE);
	m_bFocusTreeOnEnter = prefs.GetProfileInt("Preferences", "FocusTreeOnEnter", FALSE);
	m_nNewTaskPos = prefs.GetProfileInt("Preferences", "NewTaskPos", PUIP_BELOW);
	m_nNewSubtaskPos = prefs.GetProfileInt("Preferences", "NewSubtaskPos", PUIP_BOTTOM);
	m_bKeepTabsOrdered = prefs.GetProfileInt("Preferences", "KeepTabsOrdered", FALSE);
	m_bShowTasklistCloseButton = prefs.GetProfileInt("Preferences", "ShowTasklistCloseButton", TRUE);
	m_bSortDoneTasksAtBottom = prefs.GetProfileInt("Preferences", "SortDoneTasksAtBottom", TRUE);
	m_bRTLComments = prefs.GetProfileInt("Preferences", "RTLComments", FALSE);
	m_nCommentsPos = prefs.GetProfileInt("Preferences", "VertComments", PUIP_RIGHTCOMMENTS);
	m_bMultiSelFilters = prefs.GetProfileInt("Preferences", "MultiSelFilters", TRUE);
	m_bRestoreTasklistFilters = prefs.GetProfileInt("Preferences", "RestoreTasklistFilters", TRUE);
	m_bAutoRefilter = prefs.GetProfileInt("Preferences", "AutoRefilter", TRUE);
	m_bEnableLightboxMgr = prefs.GetProfileInt("Preferences", "EnableLightboxMgr", COSVersion() <= OSV_XP);
	m_bUseUITheme = CThemed::IsThemeActive() && prefs.GetProfileInt("Preferences", "UseUITheme", TRUE);
//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);

	// set default theme to blue
	if (CThemed::IsThemeActive())
	{
		CString sDefault = FileMisc::GetModuleFolder() + "Resources\\ThemeBlue.xml";
		m_sUIThemeFile = prefs.GetProfileString("Preferences", "UIThemeFile", sDefault);
	}
	else
		m_sUIThemeFile.Empty();

	// comments format
	if (m_cbCommentsFmt.IsInitialized())
	{
		m_cfDefault = prefs.GetProfileString("Preferences", "DefaultCommentsFormatID");
		m_nDefaultCommentsFormat = m_cbCommentsFmt.SetSelectedFormat(m_cfDefault);

		// fallback
		if (m_nDefaultCommentsFormat == CB_ERR)
			m_nDefaultCommentsFormat = prefs.GetProfileInt("Preferences", "DefaultCommentsFormat", -1);

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
	prefs.WriteProfileInt("Preferences", "ShowCtrlsAsColumns", m_bShowCtrlsAsColumns);
	prefs.WriteProfileInt("Preferences", "ShowEditMenuAsColumns", m_bShowEditMenuAsColumns);
	prefs.WriteProfileInt("Preferences", "ShowCommentsAlways", m_bShowCommentsAlways);
	prefs.WriteProfileInt("Preferences", "AutoReposCtrls", m_bAutoReposCtrls);
	prefs.WriteProfileInt("Preferences", "SpecifyToolbarImage", m_bSpecifyToolbarImage);
	prefs.WriteProfileInt("Preferences", "SharedCommentsHeight", m_bSharedCommentsHeight);
	prefs.WriteProfileInt("Preferences", "AutoHideTabbar", m_bAutoHideTabbar);
	prefs.WriteProfileInt("Preferences", "StackTabbarItems", m_bStackTabbarItems);
	prefs.WriteProfileInt("Preferences", "RightAlignLabels", m_bRightAlignLabels);
	prefs.WriteProfileInt("Preferences", "FocusTreeOnEnter", m_bFocusTreeOnEnter);
	prefs.WriteProfileInt("Preferences", "NewTaskPos", m_nNewTaskPos);
	prefs.WriteProfileInt("Preferences", "NewSubtaskPos", m_nNewSubtaskPos);
	prefs.WriteProfileInt("Preferences", "KeepTabsOrdered", m_bKeepTabsOrdered);
	prefs.WriteProfileInt("Preferences", "ShowTasklistCloseButton", m_bShowTasklistCloseButton);
	prefs.WriteProfileInt("Preferences", "SortDoneTasksAtBottom", m_bSortDoneTasksAtBottom);
	prefs.WriteProfileInt("Preferences", "RTLComments", m_bRTLComments);
	prefs.WriteProfileInt("Preferences", "VertComments", m_nCommentsPos);
	prefs.WriteProfileInt("Preferences", "MultiSelFilters", m_bMultiSelFilters);
	prefs.WriteProfileInt("Preferences", "RestoreTasklistFilters", m_bRestoreTasklistFilters);
	prefs.WriteProfileInt("Preferences", "AutoRefilter", m_bAutoRefilter);
	prefs.WriteProfileString("Preferences", "UIThemeFile", m_sUIThemeFile);
	prefs.WriteProfileInt("Preferences", "UseUITheme", m_bUseUITheme);
	prefs.WriteProfileInt("Preferences", "EnableLightboxMgr", m_bEnableLightboxMgr);
//	prefs.WriteProfileInt("Preferences", "", m_b);

	// comments format
	if (m_pContentMgr)
	{
		prefs.WriteProfileInt("Preferences", "DefaultCommentsFormat", m_nDefaultCommentsFormat);
		prefs.WriteProfileString("Preferences", "DefaultCommentsFormatID", m_cfDefault);
	}
}

void CPreferencesUIPage::OnUseuitheme() 
{
	UpdateData();
	GetDlgItem(IDC_UITHEMEFILE)->EnableWindow(m_bUseUITheme);
}
