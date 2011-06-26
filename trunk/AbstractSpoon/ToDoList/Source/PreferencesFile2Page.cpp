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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

// PreferencesFilePage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesFile2Page.h"

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
// CPreferencesFile2Page property page

IMPLEMENT_DYNCREATE(CPreferencesFile2Page, CPreferencesPageBase)

CPreferencesFile2Page::CPreferencesFile2Page(const CImportExportMgr* pExportMgr):
CPreferencesPageBase(CPreferencesFile2Page::IDD),
m_eExportFolderPath(FES_FOLDERS | FES_COMBOSTYLEBTN | FES_RELATIVEPATHS),
m_eSaveExportStylesheet(FES_COMBOSTYLEBTN | FES_RELATIVEPATHS, CEnString(IDS_XSLFILEFILTER)),
m_pExportMgr(pExportMgr),
m_eBackupLocation(FES_FOLDERS | FES_COMBOSTYLEBTN | FES_RELATIVEPATHS)
{
	//{{AFX_DATA_INIT(CPreferencesFile2Page)
	m_nKeepBackups = 50;
	m_bExportFilteredOnly = FALSE;
	//}}AFX_DATA_INIT
	m_bOtherExport = FALSE;
	m_nOtherExporter = 1;
	m_bBackupOnSave = FALSE;
	m_nKeepBackups = 0;
	m_bExportFilteredOnly = FALSE;
	m_bExportToFolder = FALSE;
	m_nAutoSaveFrequency = 5;
	m_bAutoSave = TRUE;
	m_bAutoExport = FALSE;
	m_bAutoSaveOnSwitchTasklist = FALSE;
	m_bAutoSaveOnSwitchApp = FALSE;
	m_bOtherExport = FALSE;
	m_nOtherExporter = -1;
	m_bUseStylesheetForSaveExport = FALSE;
}

CPreferencesFile2Page::~CPreferencesFile2Page()
{
}

void CPreferencesFile2Page::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesFile2Page)
	DDX_Control(pDX, IDC_NUMBACKUPSTOKEEP, m_cbKeepBackups);
	DDX_Control(pDX, IDC_BACKUPLOCATION, m_eBackupLocation);
	DDX_Check(pDX, IDC_BACKUPONSAVE, m_bBackupOnSave);
	DDX_Text(pDX, IDC_BACKUPLOCATION, m_sBackupLocation);
	DDX_CBIndex(pDX, IDC_NUMBACKUPSTOKEEP, m_nKeepBackups);
	DDX_Check(pDX, IDC_EXPORTFILTERED, m_bExportFilteredOnly);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_OTHEREXPORTERS, m_cbOtherExporters);
	DDX_Control(pDX, IDC_SAVEEXPORTSTYLESHEET, m_eSaveExportStylesheet);
	DDX_Control(pDX, IDC_EXPORTFOLDER, m_eExportFolderPath);
	DDX_Check(pDX, IDC_EXPORTTOFOLDER, m_bExportToFolder);
	DDX_Text(pDX, IDC_EXPORTFOLDER, m_sExportFolderPath);
	DDX_Text(pDX, IDC_SAVEEXPORTSTYLESHEET, m_sSaveExportStylesheet);
	DDX_Check(pDX, IDC_USESTYLESHEETFORSAVE, m_bUseStylesheetForSaveExport);
	DDX_Check(pDX, IDC_AUTOSAVEONSWITCHTASKLIST, m_bAutoSaveOnSwitchTasklist);
	DDX_Check(pDX, IDC_AUTOSAVEONSWITCHAPP, m_bAutoSaveOnSwitchApp);
	DDX_Radio(pDX, IDC_HTMLEXPORT, m_bOtherExport);
	DDX_CBIndex(pDX, IDC_OTHEREXPORTERS, m_nOtherExporter);
	DDX_Control(pDX, IDC_AUTOSAVEFREQUENCY, m_cbAutoSave);
	DDX_Check(pDX, IDC_AUTOEXPORT, m_bAutoExport);
	DDX_Check(pDX, IDC_AUTOSAVE, m_bAutoSave);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		m_nKeepBackups = CDialogHelper::GetSelectedItemAsValue(m_cbKeepBackups);

		if (m_bAutoSave)
		{
			m_nAutoSaveFrequency = CDialogHelper::GetSelectedItemAsValue(m_cbAutoSave);
		}
		else
		{
			m_nAutoSaveFrequency = 0;
		}
	}
	else
	{
		if (!CDialogHelper::SelectItemByValue(m_cbKeepBackups, m_nKeepBackups))
		{
			if (m_nKeepBackups == 0) // all
			{
				m_cbKeepBackups.SelectString(-1, _T("All"));
			}
			else
			{
				m_nKeepBackups = 10;
				m_cbKeepBackups.SelectString(-1, _T("10"));
			}
		}

		if (m_bAutoSave)
		{
			if (!CDialogHelper::SelectItemByValue(m_cbAutoSave, m_nAutoSaveFrequency))
			{
				m_nAutoSaveFrequency = 5;
				m_cbAutoSave.SelectString(-1, _T("5"));
			}
		}
		else
		{
			m_cbAutoSave.SetCurSel(2);
		}
	}
}

BEGIN_MESSAGE_MAP(CPreferencesFile2Page, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesFile2Page)
	ON_BN_CLICKED(IDC_BACKUPONSAVE, OnBackuponsave)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_EXPORTTOFOLDER, OnExporttofolder)
	ON_BN_CLICKED(IDC_AUTOEXPORT, OnAutoexport)
	ON_BN_CLICKED(IDC_USESTYLESHEETFORSAVE, OnUsestylesheetforsave)
	ON_BN_CLICKED(IDC_HTMLEXPORT, OnHtmlexport)
	ON_BN_CLICKED(IDC_OTHEREXPORT, OnOtherexport)
	ON_BN_CLICKED(IDC_AUTOSAVE, OnAutosave)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFile2Page message handlers

BOOL CPreferencesFile2Page::OnInitDialog()
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_BACKUPGROUP, *this);
	m_mgrGroupLines.AddGroupLine(IDC_SAVEGROUP, *this);

	m_eExportFolderPath.SetFolderPrompt(CEnString(IDS_PFP_SELECTFOLDER));

	GetDlgItem(IDC_BACKUPLOCATION)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPLOCATIONLABEL)->EnableWindow(m_bBackupOnSave);

	GetDlgItem(IDC_BACKUPCOUNTLABEL)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_NUMBACKUPSTOKEEP)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPCOUNTTRAIL)->EnableWindow(m_bBackupOnSave);

	GetDlgItem(IDC_AUTOSAVEFREQUENCY)->EnableWindow(m_bAutoSave);
	GetDlgItem(IDC_EXPORTTOFOLDER)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoExport && m_bExportToFolder);
	GetDlgItem(IDC_HTMLEXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);
	GetDlgItem(IDC_EXPORTFILTERED)->EnableWindow(m_bAutoExport);

	// build the exporter format comboxbox
	ASSERT(m_pExportMgr);

	for (int nExp = 0; nExp < m_pExportMgr->GetNumExporters(); nExp++)
	{
		m_cbOtherExporters.AddString(m_pExportMgr->GetExporterMenuText(nExp));
	}

	m_cbOtherExporters.SetCurSel(m_nOtherExporter);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesFile2Page::LoadPreferences(const CPreferences& prefs)
{
	m_bBackupOnSave = prefs.GetProfileInt(_T("Preferences"), _T("BackupOnSave"), TRUE);
	m_sBackupLocation = prefs.GetProfileString(_T("Preferences"), _T("BackupLocation"), _T("backup"));
	m_nKeepBackups = prefs.GetProfileInt(_T("Preferences"), _T("KeepBackups"), 10);

	// saving
	m_nAutoSaveFrequency = prefs.GetProfileInt(_T("Preferences"), _T("AutoSaveFrequency"), 1);
	m_bAutoExport = prefs.GetProfileInt(_T("Preferences"), _T("AutoHtmlExport"), FALSE);
	m_sExportFolderPath = prefs.GetProfileString(_T("Preferences"), _T("ExportFolderPath"), _T(""));
	m_sSaveExportStylesheet = prefs.GetProfileString(_T("Preferences"), _T("SaveExportStylesheet"));
	m_bAutoSaveOnSwitchTasklist = prefs.GetProfileInt(_T("Preferences"), _T("AutoSaveOnSwitchTasklist"), FALSE);
	m_bAutoSaveOnSwitchApp = prefs.GetProfileInt(_T("Preferences"), _T("AutoSaveOnSwitchApp"), FALSE);
	m_bOtherExport = prefs.GetProfileInt(_T("Preferences"), _T("OtherExport"), FALSE);
	m_nOtherExporter = prefs.GetProfileInt(_T("Preferences"), _T("OtherExporter"), 1);
	m_bExportFilteredOnly = prefs.GetProfileInt(_T("Preferences"), _T("ExportFilteredOnly"), FALSE);

	// these are dependent on the values they control for backward compat
	m_bUseStylesheetForSaveExport = prefs.GetProfileInt(_T("Preferences"), _T("UseStylesheetForSaveExport"),
		!m_sSaveExportStylesheet.IsEmpty());
	m_bExportToFolder = prefs.GetProfileInt(_T("Preferences"), _T("ExportToFolder"), !m_sExportFolderPath.IsEmpty());

	m_sExportFolderPath.TrimLeft();
	m_sExportFolderPath.TrimRight();

	m_bAutoSave = (m_nAutoSaveFrequency > 0);

	// set file edit directories and make paths relative
	CString sFolder = FileMisc::GetAppFolder();

	m_eExportFolderPath.SetCurrentFolder(sFolder);
	m_sExportFolderPath = FileMisc::GetRelativePath(m_sExportFolderPath, sFolder, TRUE);

	m_eBackupLocation.SetCurrentFolder(sFolder);
	m_sBackupLocation = FileMisc::GetRelativePath(m_sBackupLocation, sFolder, TRUE);

	sFolder = FileMisc::GetAppResourceFolder();

	m_eSaveExportStylesheet.SetCurrentFolder(sFolder);
	m_sSaveExportStylesheet = FileMisc::GetRelativePath(m_sSaveExportStylesheet, sFolder, FALSE);
}

void CPreferencesFile2Page::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt(_T("Preferences"), _T("BackupOnSave"), m_bBackupOnSave);
	prefs.WriteProfileString(_T("Preferences"), _T("BackupLocation"), m_sBackupLocation);
	prefs.WriteProfileInt(_T("Preferences"), _T("KeepBackups"), m_nKeepBackups);

	// saving
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoSaveFrequency"), m_nAutoSaveFrequency);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoHtmlExport"), m_bAutoExport);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExportToFolder"), m_bExportToFolder);
	prefs.WriteProfileString(_T("Preferences"), _T("ExportFolderPath"), m_sExportFolderPath);
	prefs.WriteProfileInt(_T("Preferences"), _T("UseStylesheetForSaveExport"), m_bUseStylesheetForSaveExport);
	prefs.WriteProfileString(_T("Preferences"), _T("SaveExportStylesheet"), m_sSaveExportStylesheet);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoSaveOnSwitchTasklist"), m_bAutoSaveOnSwitchTasklist);
	prefs.WriteProfileInt(_T("Preferences"), _T("AutoSaveOnSwitchApp"), m_bAutoSaveOnSwitchApp);
	prefs.WriteProfileInt(_T("Preferences"), _T("OtherExport"), m_bOtherExport);
	prefs.WriteProfileInt(_T("Preferences"), _T("OtherExporter"), m_nOtherExporter);
	prefs.WriteProfileInt(_T("Preferences"), _T("ExportFilteredOnly"), m_bExportFilteredOnly);
}

void CPreferencesFile2Page::OnBackuponsave()
{
	UpdateData();

	GetDlgItem(IDC_BACKUPLOCATION)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPLOCATIONLABEL)->EnableWindow(m_bBackupOnSave);

	GetDlgItem(IDC_BACKUPCOUNTLABEL)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_NUMBACKUPSTOKEEP)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPCOUNTTRAIL)->EnableWindow(m_bBackupOnSave);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFile2Page::OnAutosave()
{
	UpdateData();

	GetDlgItem(IDC_AUTOSAVEFREQUENCY)->EnableWindow(m_bAutoSave);

	if (m_bAutoSave && !m_nAutoSaveFrequency)
	{
		m_nAutoSaveFrequency = 5;
		m_cbAutoSave.SetCurSel(2);
	}

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFile2Page::OnExporttofolder()
{
	UpdateData();

	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoExport && m_bExportToFolder);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFile2Page::OnAutoexport()
{
	UpdateData();

	GetDlgItem(IDC_EXPORTTOFOLDER)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_HTMLEXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_OTHEREXPORT)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_EXPORTFOLDER)->EnableWindow(m_bAutoExport && m_bExportToFolder);
	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);
	GetDlgItem(IDC_EXPORTFILTERED)->EnableWindow(m_bAutoExport);

	CPreferencesPageBase::OnControlChange();
}

CString CPreferencesFile2Page::GetAutoExportFolderPath() const
{
	if (m_bAutoExport && m_bExportToFolder && !m_sExportFolderPath.IsEmpty())
	{
		return FileMisc::GetFullPath(m_sExportFolderPath, FileMisc::GetAppFolder());
	}

	// else
	return _T("");
}

void CPreferencesFile2Page::OnUsestylesheetforsave()
{
	UpdateData();

	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && m_bUseStylesheetForSaveExport);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFile2Page::OnHtmlexport()
{
	UpdateData();

	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);

	CPreferencesPageBase::OnControlChange();
}

void CPreferencesFile2Page::OnOtherexport()
{
	UpdateData();

	GetDlgItem(IDC_USESTYLESHEETFORSAVE)->EnableWindow(m_bAutoExport && !m_bOtherExport);
	GetDlgItem(IDC_SAVEEXPORTSTYLESHEET)->EnableWindow(m_bAutoExport && !m_bOtherExport && m_bUseStylesheetForSaveExport);
	GetDlgItem(IDC_OTHEREXPORTERS)->EnableWindow(m_bAutoExport && m_bOtherExport);

	CPreferencesPageBase::OnControlChange();
}

CString CPreferencesFile2Page::GetSaveExportStylesheet() const
{
	if (m_bUseStylesheetForSaveExport && !m_sSaveExportStylesheet.IsEmpty())
	{
		return FileMisc::GetFullPath(m_sSaveExportStylesheet, FileMisc::GetAppResourceFolder());
	}

	// else
	return _T("");
}

CString CPreferencesFile2Page::GetBackupLocation() const
{
	if (m_bBackupOnSave)
	{
		return FileMisc::GetFullPath(m_sBackupLocation, FileMisc::GetAppFolder());
	}

	// else
	return _T("");
}
