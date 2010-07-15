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

CPreferencesFilePage::CPreferencesFilePage(const CImportExportMgr* pExportMgr) : 
		CPreferencesPageBase(CPreferencesFilePage::IDD),
		m_eExportFolderPath(FES_FOLDERS | FES_COMBOSTYLEBTN),
		m_eSaveExportStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER)),
		m_eDueTaskStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER)),
		m_pExportMgr(pExportMgr)
{
//	m_psp.dwFlags &= ~PSP_HASHELP;

	//{{AFX_DATA_INIT(CPreferencesFilePage)
	m_bOtherExport = FALSE;
	m_nOtherExporter = 1;
	//}}AFX_DATA_INIT
}

CPreferencesFilePage::~CPreferencesFilePage()
{
}

void CPreferencesFilePage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesFilePage)
	DDX_Control(pDX, IDC_OTHEREXPORTERS, m_cbOtherExporters);
	DDX_Control(pDX, IDC_SAVEEXPORTSTYLESHEET, m_eSaveExportStylesheet);
	DDX_Control(pDX, IDC_DUETASKSTYLESHEET, m_eDueTaskStylesheet);
	DDX_Control(pDX, IDC_EXPORTFOLDER, m_eExportFolderPath);
	DDX_Check(pDX, IDC_EXPORTTOFOLDER, m_bExportToFolder);
	DDX_Text(pDX, IDC_EXPORTFOLDER, m_sExportFolderPath);
	DDX_Check(pDX, IDC_DISPLAYDUETASKSINHTML, m_bDisplayDueTasksInHtml);
	DDX_Check(pDX, IDC_REFRESHFINDTASKS, m_bRefreshFindOnLoad);
	DDX_Check(pDX, IDC_DISPLAYDUETASKTITLESONLY, m_bDueTaskTitlesOnly);
	DDX_Text(pDX, IDC_SAVEEXPORTSTYLESHEET, m_sSaveExportStylesheet);
	DDX_Text(pDX, IDC_DUETASKSTYLESHEET, m_sDueTasksStylesheet);
	DDX_Check(pDX, IDC_USESTYLESHEETFORSAVE, m_bUseStylesheetForSaveExport);
	DDX_Check(pDX, IDC_USESTYLESHEETFORDUEITEMS, m_bUseStyleSheetForDueTasks);
	DDX_Check(pDX, IDC_ONLYSHOWDUETASKFORPERSON, m_bOnlyShowDueTasksForPerson);
	DDX_Text(pDX, IDC_DUETASKPERSON, m_sDueTaskPerson);
	DDX_Check(pDX, IDC_WARNADDDELARCHIVE, m_bWarnAddDeleteArchive);
	DDX_Check(pDX, IDC_DONTREMOVEFLAGGED, m_bDontRemoveFlagged);
	DDX_Check(pDX, IDC_EXPANDTASKS, m_bExpandTasks);
	DDX_Check(pDX, IDC_AUTOSAVEONSWITCHTASKLIST, m_bAutoSaveOnSwitchTasklist);
	DDX_Check(pDX, IDC_AUTOSAVEONSWITCHAPP, m_bAutoSaveOnSwitchApp);
	DDX_Radio(pDX, IDC_HTMLEXPORT, m_bOtherExport);
	DDX_CBIndex(pDX, IDC_OTHEREXPORTERS, m_nOtherExporter);
	//}}AFX_DATA_MAP
	DDX_CBIndex(pDX, IDC_NOTIFYDUEBYONLOAD, m_nNotifyDueByOnLoad);
	DDX_CBIndex(pDX, IDC_NOTIFYDUEBYONSWITCH, m_nNotifyDueByOnSwitch);
	DDX_Check(pDX, IDC_NOTIFYDUEONLOAD, m_bNotifyDueOnLoad);
	DDX_Check(pDX, IDC_NOTIFYDUEONSWITCH, m_bNotifyDueOnSwitch);
	DDX_Check(pDX, IDC_NOTIFYREADONLY, m_bNotifyReadOnly);
	DDX_Check(pDX, IDC_AUTOARCHIVE, m_bAutoArchive);
	DDX_Control(pDX, IDC_AUTOSAVEFREQUENCY, m_cbAutoSave);
	DDX_Check(pDX, IDC_AUTOEXPORT, m_bAutoExport);
	DDX_Check(pDX, IDC_REMOVEARCHIVEDITEMS, m_bRemoveArchivedTasks);
	DDX_Check(pDX, IDC_REMOVEONLYONABSCOMPLETION, m_bRemoveOnlyOnAbsoluteCompletion);
	DDX_Check(pDX, IDC_AUTOSAVE, m_bAutoSave);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		if (m_bAutoSave)
			m_nAutoSaveFrequency = CDialogHelper::GetSelectedItemAsValue(m_cbAutoSave);
		else
			m_nAutoSaveFrequency = 0;
	}
	else
	{
		if (m_bAutoSave)
		{
			if (!CDialogHelper::SelectItemByValue(m_cbAutoSave, m_nAutoSaveFrequency))
			{
				m_nAutoSaveFrequency = 5;
				m_cbAutoSave.SelectString(-1, "5");
			}
		}
		else
			m_cbAutoSave.SetCurSel(2);
	}
}


BEGIN_MESSAGE_MAP(CPreferencesFilePage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesFilePage)
	ON_BN_CLICKED(IDC_EXPORTTOFOLDER, OnExporttofolder)
	ON_BN_CLICKED(IDC_AUTOEXPORT, OnAutoexport)
	ON_BN_CLICKED(IDC_NOTIFYDUEONLOAD, OnNotifyDueOnLoad)
	ON_BN_CLICKED(IDC_DISPLAYDUETASKSINHTML, OnDisplayduetasksinhtml)
	ON_BN_CLICKED(IDC_USESTYLESHEETFORDUEITEMS, OnUsestylesheetfordueitems)
	ON_BN_CLICKED(IDC_USESTYLESHEETFORSAVE, OnUsestylesheetforsave)
	ON_BN_CLICKED(IDC_ONLYSHOWDUETASKFORPERSON, OnOnlyshowduetaskforperson)
	ON_BN_CLICKED(IDC_HTMLEXPORT, OnHtmlexport)
	ON_BN_CLICKED(IDC_OTHEREXPORT, OnOtherexport)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_NOTIFYDUEONSWITCH, OnNotifyDueOnSwitch)
	ON_BN_CLICKED(IDC_REMOVEARCHIVEDITEMS, OnRemovearchiveditems)
	ON_BN_CLICKED(IDC_AUTOSAVE, OnAutosave)
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
	m_mgrGroupLines.AddGroupLine(IDC_SAVEGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_PRINTGROUP, *this);

	m_eExportFolderPath.SetFolderPrompt(CEnString(IDS_PFP_SELECTFOLDER));
	
	GetDlgItem(IDC_REMOVEONLYONABSCOMPLETION)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_DONTREMOVEFLAGGED)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_AUTOSAVEFREQUENCY)->EnableWindow(m_bAutoSave);
	GetDlgItem(IDC_EXPORTTOFOLDER)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoExport && m_bExportToFolder);
	GetDlgItem(IDC_NOTIFYDUEBYONLOAD)->EnableWindow(m_bNotifyDueOnLoad);
	GetDlgItem(IDC_NOTIFYDUEBYONSWITCH)->EnableWindow(m_bNotifyDueOnSwitch);
	GetDlgItem(IDC_USESTYLESHEETFORDUEITEMS)->EnableWindow(m_bDisplayDueTasksInHtml);
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_HTMLEXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_OTHEREXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_DUETASKPERSON)->EnableWindow(m_bOnlyShowDueTasksForPerson);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);

	// build the exporter format comboxbox
	ASSERT(m_pExportMgr);

	for (int nExp = 0; nExp < m_pExportMgr->GetNumExporters(); nExp++)
		m_cbOtherExporters.AddString(m_pExportMgr->GetExporterMenuText(nExp));

	m_cbOtherExporters.SetCurSel(m_nOtherExporter);

	// init the stylesheet folder to point to the resource folder
	CString sXslFolder = FileMisc::GetModuleFolder() + "Resources";
	m_eSaveExportStylesheet.SetCurrentFolder(sXslFolder);
	m_eDueTaskStylesheet.SetCurrentFolder(sXslFolder);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesFilePage::OnRemovearchiveditems() 
{
	UpdateData();

	GetDlgItem(IDC_REMOVEONLYONABSCOMPLETION)->EnableWindow(m_bRemoveArchivedTasks);
	GetDlgItem(IDC_DONTREMOVEFLAGGED)->EnableWindow(m_bRemoveArchivedTasks);
}

void CPreferencesFilePage::OnAutosave() 
{
	UpdateData();

	GetDlgItem(IDC_AUTOSAVEFREQUENCY)->EnableWindow(m_bAutoSave);

	if (m_bAutoSave && !m_nAutoSaveFrequency)
	{
		m_nAutoSaveFrequency = 5;
		m_cbAutoSave.SetCurSel(2);
	}
}

void CPreferencesFilePage::OnExporttofolder() 
{
	UpdateData();

	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoExport && m_bExportToFolder);
}

void CPreferencesFilePage::OnAutoexport() 
{
	UpdateData();	

	GetDlgItem(IDC_EXPORTTOFOLDER)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_HTMLEXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_OTHEREXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoExport && m_bExportToFolder);
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);
}

CString CPreferencesFilePage::GetAutoExportFolderPath() const 
{ 
	if (m_bAutoExport && m_bExportToFolder)
		return m_sExportFolderPath;
	else
		return "";
}

void CPreferencesFilePage::OnNotifyDueOnLoad() 
{
	UpdateData();
	
	GetDlgItem(IDC_NOTIFYDUEBYONLOAD)->EnableWindow(m_bNotifyDueOnLoad);
}

void CPreferencesFilePage::OnNotifyDueOnSwitch() 
{
	UpdateData();
	
	GetDlgItem(IDC_NOTIFYDUEBYONSWITCH)->EnableWindow(m_bNotifyDueOnSwitch);
}

void CPreferencesFilePage::OnDisplayduetasksinhtml() 
{
	UpdateData();
	
	GetDlgItem(IDC_USESTYLESHEETFORDUEITEMS)->EnableWindow(m_bDisplayDueTasksInHtml);
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);
}

void CPreferencesFilePage::OnUsestylesheetfordueitems() 
{
	UpdateData();
	
	GetDlgItem(IDC_DUETASKSTYLESHEET)->EnableWindow(m_bDisplayDueTasksInHtml && m_bUseStyleSheetForDueTasks);
}

void CPreferencesFilePage::OnUsestylesheetforsave() 
{
	UpdateData();
	
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && m_bUseStylesheetForSaveExport);
}

void CPreferencesFilePage::OnOnlyshowduetaskforperson() 
{
	UpdateData();
	
	GetDlgItem(IDC_DUETASKPERSON)->EnableWindow(m_bOnlyShowDueTasksForPerson);
}

void CPreferencesFilePage::LoadPreferences(const CPreferences& prefs)
{
	m_bNotifyDueOnLoad = prefs.GetProfileInt("Preferences", "NotifyDue", FALSE);
	m_bNotifyDueOnSwitch = prefs.GetProfileInt("Preferences", "NotifyDueOnSwitch", FALSE);
	m_bAutoArchive = prefs.GetProfileInt("Preferences", "AutoArchive", FALSE);
	m_bNotifyReadOnly = prefs.GetProfileInt("Preferences", "NotifyReadOnly", TRUE);
	m_bRemoveArchivedTasks = prefs.GetProfileInt("Preferences", "RemoveArchivedTasks", TRUE);
	m_bRemoveOnlyOnAbsoluteCompletion = prefs.GetProfileInt("Preferences", "RemoveOnlyOnAbsoluteCompletion", TRUE);
	m_nAutoSaveFrequency = prefs.GetProfileInt("Preferences", "AutoSaveFrequency", 1);
	m_bAutoExport = prefs.GetProfileInt("Preferences", "AutoHtmlExport", FALSE);
	m_sExportFolderPath = prefs.GetProfileString("Preferences", "ExportFolderPath", "");
	m_nNotifyDueByOnLoad = prefs.GetProfileInt("Preferences", "NotifyDueBy", PFP_DUETODAY);
	m_nNotifyDueByOnSwitch = prefs.GetProfileInt("Preferences", "NotifyDueByOnSwitch", PFP_DUETODAY);
	m_bDisplayDueTasksInHtml = prefs.GetProfileInt("Preferences", "DisplayDueTasksInHtml", TRUE);
	m_bRefreshFindOnLoad = prefs.GetProfileInt("Preferences", "RefreshFindOnLoad", FALSE);
	m_bDueTaskTitlesOnly = prefs.GetProfileInt("Preferences", "DueTaskTitlesOnly", FALSE);
	m_sDueTasksStylesheet = prefs.GetProfileString("Preferences", "DueTasksStylesheet", FALSE);
	m_sSaveExportStylesheet = prefs.GetProfileString("Preferences", "SaveExportStylesheet");
	m_sDueTaskPerson = prefs.GetProfileString("Preferences", "DueTaskPerson");
	m_bWarnAddDeleteArchive = prefs.GetProfileInt("Preferences", "WarnAddDeleteArchive", TRUE);
	m_bDontRemoveFlagged = prefs.GetProfileInt("Preferences", "DontRemoveFlagged", FALSE);
	m_bExpandTasks = prefs.GetProfileInt("Preferences", "ExpandTasks", FALSE);
	m_bAutoSaveOnSwitchTasklist = prefs.GetProfileInt("Preferences", "AutoSaveOnSwitchTasklist", FALSE);
	m_bAutoSaveOnSwitchApp = prefs.GetProfileInt("Preferences", "AutoSaveOnSwitchApp", FALSE);
	m_bOtherExport = prefs.GetProfileInt("Preferences", "OtherExport", FALSE);
	m_nOtherExporter = prefs.GetProfileInt("Preferences", "OtherExporter", 1);
//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);

	// these are dependent on the values they control for backward compat
	m_bOnlyShowDueTasksForPerson = prefs.GetProfileInt("Preferences", "OnlyShowDueTasksForPerson", !m_sDueTaskPerson.IsEmpty());
	m_bUseStylesheetForSaveExport = prefs.GetProfileInt("Preferences", "UseStylesheetForSaveExport", !m_sSaveExportStylesheet.IsEmpty());
	m_bUseStyleSheetForDueTasks = prefs.GetProfileInt("Preferences", "UseStylesheetForDueTasks", !m_sDueTasksStylesheet.IsEmpty());
	m_bExportToFolder = prefs.GetProfileInt("Preferences", "ExportToFolder", !m_sExportFolderPath.IsEmpty());

	m_sExportFolderPath.TrimLeft();
	m_sExportFolderPath.TrimRight();

	m_bAutoSave = (m_nAutoSaveFrequency > 0);
}

void CPreferencesFilePage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "NotifyDue", m_bNotifyDueOnLoad);
	prefs.WriteProfileInt("Preferences", "NotifyDueOnSwitch", m_bNotifyDueOnSwitch);
	prefs.WriteProfileInt("Preferences", "AutoArchive", m_bAutoArchive);
	prefs.WriteProfileInt("Preferences", "NotifyReadOnly", m_bNotifyReadOnly);
	prefs.WriteProfileInt("Preferences", "RemoveArchivedTasks", m_bRemoveArchivedTasks);
	prefs.WriteProfileInt("Preferences", "RemoveOnlyOnAbsoluteCompletion", m_bRemoveOnlyOnAbsoluteCompletion);
	prefs.WriteProfileInt("Preferences", "AutoSaveFrequency", m_nAutoSaveFrequency);
	prefs.WriteProfileInt("Preferences", "AutoHtmlExport", m_bAutoExport);
	prefs.WriteProfileInt("Preferences", "ExportToFolder", m_bExportToFolder);
	prefs.WriteProfileString("Preferences", "ExportFolderPath", m_sExportFolderPath);
	prefs.WriteProfileInt("Preferences", "NotifyDueBy", m_nNotifyDueByOnLoad);
	prefs.WriteProfileInt("Preferences", "NotifyDueByOnSwitch", m_nNotifyDueByOnSwitch);
	prefs.WriteProfileInt("Preferences", "DisplayDueTasksInHtml", m_bDisplayDueTasksInHtml);
	prefs.WriteProfileInt("Preferences", "RefreshFindOnLoad", m_bRefreshFindOnLoad);
	prefs.WriteProfileInt("Preferences", "DueTaskTitlesOnly", m_bDueTaskTitlesOnly);
	prefs.WriteProfileInt("Preferences", "UseStylesheetForDueTasks", m_bUseStyleSheetForDueTasks);
	prefs.WriteProfileString("Preferences", "DueTasksStylesheet", m_sDueTasksStylesheet);
	prefs.WriteProfileInt("Preferences", "UseStylesheetForSaveExport", m_bUseStylesheetForSaveExport);
	prefs.WriteProfileString("Preferences", "SaveExportStylesheet", m_sSaveExportStylesheet);
	prefs.WriteProfileInt("Preferences", "OnlyShowDueTasksForPerson", m_bOnlyShowDueTasksForPerson);
	prefs.WriteProfileString("Preferences", "DueTaskPerson", m_sDueTaskPerson);
	prefs.WriteProfileInt("Preferences", "WarnAddDeleteArchive", m_bWarnAddDeleteArchive);
	prefs.WriteProfileInt("Preferences", "DontRemoveFlagged", m_bDontRemoveFlagged);
	prefs.WriteProfileInt("Preferences", "ExpandTasks", m_bExpandTasks);
	prefs.WriteProfileInt("Preferences", "AutoSaveOnSwitchTasklist", m_bAutoSaveOnSwitchTasklist);
	prefs.WriteProfileInt("Preferences", "AutoSaveOnSwitchApp", m_bAutoSaveOnSwitchApp);
	prefs.WriteProfileInt("Preferences", "OtherExport", m_bOtherExport);
	prefs.WriteProfileInt("Preferences", "OtherExporter", m_nOtherExporter);

//	prefs.WriteProfileInt("Preferences", "", m_b);
}

void CPreferencesFilePage::OnHtmlexport() 
{
	UpdateData();	

	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);
}

void CPreferencesFilePage::OnOtherexport() 
{
	UpdateData();	

	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);
}
