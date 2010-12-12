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

#if !defined(AFX_PREFERENCESFILE2PAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
#define AFX_PREFERENCESFILE2PAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesFilePage.h : header file
//

#include "../../Common/FileEdit.h"
#include "../../Common/GroupLine.h"
#include "../../Common/PreferencesBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFile2Page dialog

class CImportExportMgr;

class CPreferencesFile2Page : public CPreferencesPageBase
{
	DECLARE_DYNCREATE(CPreferencesFile2Page)

	// Construction
public:
	CPreferencesFile2Page(const CImportExportMgr* pExportMgr = NULL);
	~CPreferencesFile2Page();

	// backup
	BOOL GetBackupOnSave() const
	{
		return m_bBackupOnSave;
	}
	CString GetBackupLocation() const
	{
		return m_bBackupOnSave ? m_sBackupLocation : _T("");
	}
	int GetKeepBackupCount() const
	{
		return m_bBackupOnSave ? m_nKeepBackups : 0;
	}

	// saving
	int GetAutoSaveFrequency() const
	{
		return m_nAutoSaveFrequency;
	}
	CString GetSaveExportStylesheet() const;
	BOOL GetAutoSaveOnSwitchTasklist() const
	{
		return m_bAutoSaveOnSwitchTasklist;
	}
	BOOL GetAutoSaveOnSwitchApp() const
	{
		return m_bAutoSaveOnSwitchApp;
	}
	CString GetAutoExportFolderPath() const;
	BOOL GetAutoExport() const
	{
		return m_bAutoExport;
	}
	BOOL GetExportToHTML() const
	{
		return m_bAutoExport && !m_bOtherExport;
	}
	int GetOtherExporter() const
	{
		return (m_bAutoExport && m_bOtherExport) ? m_nOtherExporter : -1;
	}
	BOOL GetExportFilteredOnly() const
	{
		return m_bAutoExport && m_bExportFilteredOnly;
	}

protected:
	// Dialog Data
	//{{AFX_DATA(CPreferencesFile2Page)
	enum { IDD = IDD_PREFFILE2_PAGE };
	CComboBox   m_cbKeepBackups;
	CFileEdit   m_eBackupLocation;
	BOOL    m_bBackupOnSave;
	CString m_sBackupLocation;
	int     m_nKeepBackups;
	BOOL    m_bExportFilteredOnly;
	//}}AFX_DATA
	CGroupLineManager m_mgrGroupLines;

	// saving
	CComboBox   m_cbOtherExporters;
	CFileEdit   m_eSaveExportStylesheet;
	CFileEdit   m_eExportFolderPath;
	BOOL    m_bExportToFolder;
	CString m_sExportFolderPath;
	CComboBox   m_cbAutoSave;
	int     m_nAutoSaveFrequency;
	BOOL    m_bAutoSave;
	BOOL    m_bAutoExport;
	BOOL    m_bAutoSaveOnSwitchTasklist;
	BOOL    m_bAutoSaveOnSwitchApp;
	int     m_bOtherExport;
	int     m_nOtherExporter;
	BOOL    m_bUseStylesheetForSaveExport;
	const CImportExportMgr* m_pExportMgr;
	CString m_sSaveExportStylesheet;

	// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesFile2Page)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesFile2Page)
	virtual BOOL OnInitDialog();
	afx_msg void OnBackuponsave();
	//}}AFX_MSG
	afx_msg void OnHtmlexport();
	afx_msg void OnOtherexport();
	afx_msg void OnUsestylesheetforsave();
	afx_msg void OnExporttofolder();
	afx_msg void OnAutoexport();
	afx_msg void OnAutosave();
	DECLARE_MESSAGE_MAP()

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESFILEPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
