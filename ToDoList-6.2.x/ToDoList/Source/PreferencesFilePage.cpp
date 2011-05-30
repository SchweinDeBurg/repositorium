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

// PreferencesFilePage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesFilePage.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "../../Common/ImportExportMgr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFilePage property page

IMPLEMENT_DYNCREATE(CPreferencesFilePage, CPreferencesPageBase)

CPreferencesFilePage::CPreferencesFilePage():
CPreferencesPageBase(CPreferencesFilePage::IDD),
m_eDueTaskStylesheet(FES_COMBOSTYLEBTN,
CEnString(IDS_XSLFILEFILTER))
{
	//{{AFX_DATA_INIT(CPreferencesFilePage)
	//}}AFX_DATA_INIT
}

CPreferencesFilePage::~CPreferencesFilePage()
{
}

void CPreferencesFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesFilePage)
	DDX_Control(pDX, IDC_DUETASKSTYLESHEET, m_eDueTaskStylesheet);
	DDX_Check(pDX, IDC_DISPLAYDUETASKSINHTML, m_bDisplayDueTasksInHtml);
	DDX_Check(pDX, IDC_REFRESHFINDTASKS, m_bRefreshFindOnLoad);
	DDX_Check(pDX, IDC_DISPLAYDUETASKTITLESONLY, m_bDueTaskTitlesOnly);
	DDX_Text(pDX, IDC_DUETASKSTYLESHEET, m_sDueTasksStylesheet);
	DDX_Check(pDX, IDC_USESTYLESHEETFORDUEITEMS, m_bUseStyleSheetForDueTasks);
	DDX_Check(pDX, IDC_ONLYSHOWDUETASKFORPERSON, m_bOnlyShowDueTasksForPerson);
	DDX_Text(pDX, IDC_DUETASKPERSON, m_sDueTaskPerson);
	DDX_Check(pDX, IDC_WARNADDDELARCHIVE, m_bWarnAddDeleteArchive);
	DDX_Check(pDX, IDC_DONTREMOVEFLAGGED, m_bDontRemoveFlagged);
	DDX_Check(pDX, IDC_EXPANDTASKS, m_bExpandTasks);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_NOTIFYDUEBYONLOAD, m_nNotifyDueByOnLoad);
	DDX_CBIndex(pDX, IDC_NOTIFYDUEBYONSWITCH, m_nNotifyDueByOnSwitch);
	DDX_Check(pDX, IDC_NOTIFYDUEONLOAD, m_bNotifyDueOnLoad);
	DDX_Check(pDX, IDC_NOTIFYDUEONSWITCH, m_bNotifyDueOnSwitch);
	DDX_Check(pDX, IDC_NOTIFYREADONLY, m_bNotifyReadOnly);
	DDX_Check(pDX, IDC_AUTOARCHIVE, m_bAutoArchive);
	DDX_Check(pDX, IDC_REMOVEARCHIVEDITEMS, m_bRemoveArchivedTasks);
	DDX_Check(pDX, IDC_REMOVEONLYONABSCOMPLETION, m_bRemoveOnlyOnAbsoluteCompletion);
}

BEGIN_MESSAGE_MAP(CPreferencesFilePage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesFilePage)
	ON_BN_CLICKED(IDC_NOTIFYDUEONLOAD, OnNotifyDueOnLoad)
	ON_BN_CLICKED(IDC_DISPLAYDUETASKSINHTML, OnDisplayduetasksinhtml)
	ON_BN_CLICKED(IDC_USESTYLESHEETFORDUEITEMS, OnUsestylesheetfordueitems)
	ON_BN_CLICKED(IDC_ONLYSHOWDUETASKFORPERSON, OnOnlyshowduetaskforperson)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_NOTIFYDUEONSWITCH, OnNotifyDueOnSwitch)
	ON_BN_CLICKED(IDC_REMOVEARCHIVEDITEMS, OnRemovearchiveditems)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFilePage message handlers

BOOL CPreferencesFilePage::OnInitDialog()
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_LOADGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_ARCHIVEGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_SWITCHGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_DUEGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_PRINTGROUP, *this);

	GetDlgItem(IDC_REMOVEONLYONABSCOMPLETION)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_DONTREMOVEFLAGGED)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_NOTIFYDUEBYONLOAD)->EnableWindow(m_bNotifyDueOnLoad);
	GetDlgItem(IDC_NOTIFYDUEBYONSWITCH)->EnableWindow(m_bNotifyDueOnSwitch);
	GetDlgItem(IDC_USESTYLESHEETFORDUEITEMS)->EnableWindow(m_bDisplayDueTasksInHtml);
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);
	GetDlgItem(IDC_DUETASKPERSON)->EnableWindow(m_bOnlyShowDueTasksForPerson);

	// init the stylesheet folder to point to the resource folder
	CString sXslFolder = FileMisc::GetModuleFolder() + _T("Resources");
	m_eDueTaskStylesheet.SetCurrentFolder(sXslFolder);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesFilePage::OnRemovearchiveditems()
{
	UpdateData();

	GetDlgItem(IDC_REMOVEONLYONABSCOMPLETION)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_DONTREMOVEFLAGGED)->EnableWindow(m_bRemoveArchivedTasks);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFilePage::OnNotifyDueOnLoad()
{
	UpdateData();

	GetDlgItem(IDC_NOTIFYDUEBYONLOAD)->EnableWindow(m_bNotifyDueOnLoad);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFilePage::OnNotifyDueOnSwitch()
{
	UpdateData();

	GetDlgItem(IDC_NOTIFYDUEBYONSWITCH)->EnableWindow(m_bNotifyDueOnSwitch);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFilePage::OnDisplayduetasksinhtml()
{
	UpdateData();

	GetDlgItem(IDC_USESTYLESHEETFORDUEITEMS)->EnableWindow(m_bDisplayDueTasksInHtml);
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFilePage::OnUsestylesheetfordueitems()
{
	UpdateData();

	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFilePage::OnOnlyshowduetaskforperson()
{
	UpdateData();

	GetDlgItem(IDC_DUETASKPERSON)->EnableWindow(m_bOnlyShowDueTasksForPerson);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFilePage::LoadPreferences(const CPreferences& prefs)
{
	m_bNotifyDueOnLoad = prefs.GetProfileInt(_T("Preferences"), _T("NotifyDue"), FALSE);
	m_bNotifyDueOnSwitch = prefs.GetProfileInt(_T("Preferences"), _T("NotifyDueOnSwitch"), FALSE);
	m_bAutoArchive = prefs.GetProfileInt(_T("Preferences"), _T("AutoArchive"), FALSE);
	m_bNotifyReadOnly = prefs.GetProfileInt(_T("Preferences"), _T("NotifyReadOnly"), TRUE);
	m_bRemoveArchivedTasks = prefs.GetProfileInt(_T("Preferences"), _T("RemoveArchivedTasks"), TRUE);
	m_bRemoveOnlyOnAbsoluteCompletion = prefs.GetProfileInt(_T("Preferences"), _T("RemoveOnlyOnAbsoluteCompletion"), TRUE);
	m_nNotifyDueByOnLoad = prefs.GetProfileInt(_T("Preferences"), _T("NotifyDueBy"), PFP_DUETODAY);
	m_nNotifyDueByOnSwitch = prefs.GetProfileInt(_T("Preferences"), _T("NotifyDueByOnSwitch"), PFP_DUETODAY);
	m_bDisplayDueTasksInHtml = prefs.GetProfileInt(_T("Preferences"), _T("DisplayDueTasksInHtml"), TRUE);
	m_bRefreshFindOnLoad = prefs.GetProfileInt(_T("Preferences"), _T("RefreshFindOnLoad"), FALSE);
	m_bDueTaskTitlesOnly = prefs.GetProfileInt(_T("Preferences"), _T("DueTaskTitlesOnly"), FALSE);
	m_sDueTasksStylesheet = prefs.GetProfileString(_T("Preferences"), _T("DueTasksStylesheet"), FALSE);
	m_sDueTaskPerson = prefs.GetProfileString(_T("Preferences"), _T("DueTaskPerson"));
	m_bWarnAddDeleteArchive = prefs.GetProfileInt(_T("Preferences"), _T("WarnAddDeleteArchive"), TRUE);
	m_bDontRemoveFlagged = prefs.GetProfileInt(_T("Preferences"), _T("DontRemoveFlagged"), FALSE);
	m_bExpandTasks = prefs.GetProfileInt(_T("Preferences"), _T("ExpandTasks"), FALSE);

	// these are dependent on the values they control for backward compat
	m_bOnlyShowDueTasksForPerson = prefs.GetProfileInt(_T("Preferences"), _T("OnlyShowDueTasksForPerson"), !m_sDueTaskPerson.IsEmpty());
	m_bUseStyleSheetForDueTasks = prefs.GetProfileInt(_T("Preferences"), _T("UseStylesheetForDueTasks"), !m_sDueTasksStylesheet.IsEmpty());
}

void CPreferencesFilePage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt(_T("Preferences"), _T("NotifyDue"), m_bNotifyDueOnLoad);
	prefs.WriteProfileInt(_T("Preferences"), _T("NotifyDueOnSwitch"), m_bNotifyDueOnSwitch);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoArchive"), m_bAutoArchive);
	prefs.WriteProfileInt(_T("Preferences"), _T("NotifyReadOnly"), m_bNotifyReadOnly);
	prefs.WriteProfileInt(_T("Preferences"), _T("RemoveArchivedTasks"), m_bRemoveArchivedTasks);
	prefs.WriteProfileInt(_T("Preferences"), _T("RemoveOnlyOnAbsoluteCompletion"), m_bRemoveOnlyOnAbsoluteCompletion);
	prefs.WriteProfileInt(_T("Preferences"), _T("NotifyDueBy"), m_nNotifyDueByOnLoad);
	prefs.WriteProfileInt(_T("Preferences"), _T("NotifyDueByOnSwitch"), m_nNotifyDueByOnSwitch);
	prefs.WriteProfileInt(_T("Preferences"), _T("DisplayDueTasksInHtml"), m_bDisplayDueTasksInHtml);
	prefs.WriteProfileInt(_T("Preferences"), _T("RefreshFindOnLoad"), m_bRefreshFindOnLoad);
	prefs.WriteProfileInt(_T("Preferences"), _T("DueTaskTitlesOnly"), m_bDueTaskTitlesOnly);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseStylesheetForDueTasks"), m_bUseStyleSheetForDueTasks);
	prefs.WriteProfileString(_T("Preferences"), _T("DueTasksStylesheet"), m_sDueTasksStylesheet);
	prefs.WriteProfileInt(_T("Preferences"), _T("OnlyShowDueTasksForPerson"), m_bOnlyShowDueTasksForPerson);
	prefs.WriteProfileString(_T("Preferences"), _T("DueTaskPerson"), m_sDueTaskPerson);
	prefs.WriteProfileInt(_T("Preferences"), _T("WarnAddDeleteArchive"), m_bWarnAddDeleteArchive);
	prefs.WriteProfileInt(_T("Preferences"), _T("DontRemoveFlagged"), m_bDontRemoveFlagged);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExpandTasks"), m_bExpandTasks);
}

CString CPreferencesFilePage::GetDueTaskStylesheet() const
{
	if (m_bUseStyleSheetForDueTasks && !m_sDueTasksStylesheet.IsEmpty())
	{
		return FileMisc::GetFullPath(m_sDueTasksStylesheet, TRUE);
	}

	// else
	return _T("");
}
