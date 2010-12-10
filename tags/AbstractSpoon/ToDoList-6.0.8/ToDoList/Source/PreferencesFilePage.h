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

#if !defined(AFX_PREFERENCESFILEPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
#define AFX_PREFERENCESFILEPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesFilePage.h : header file
//

#include "../../Common/FileEdit.h"
#include "../../Common/GroupLine.h"
#include "../../Common/PreferencesBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFilePage dialog

enum
{
	PFP_DONTNOTIFY = -1,
	PFP_DUETODAY,
	PFP_DUETOMORROW,
	PFP_DUETHISWEEK,
	PFP_DUENEXTWEEK,
	PFP_DUETHISMONTH,
	PFP_DUENEXTMONTH,
};

class CImportExportMgr;

class CPreferencesFilePage : public CPreferencesPageBase
{
	DECLARE_DYNCREATE(CPreferencesFilePage)

// Construction
public:
	CPreferencesFilePage(const CImportExportMgr* pExportMgr = NULL);
	~CPreferencesFilePage();

	BOOL GetAutoArchive() const { return m_bAutoArchive; }
	BOOL GetNotifyReadOnly() const { return m_bNotifyReadOnly; }
	BOOL GetAutoExport() const { return m_bAutoExport; }
	BOOL GetExportToHTML() const { return m_bAutoExport && !m_bOtherExport; }
	int GetOtherExporter() const { return (m_bAutoExport && m_bOtherExport) ? m_nOtherExporter : -1; }
	int GetAutoSaveFrequency() const { return m_nAutoSaveFrequency; }
	BOOL GetRemoveArchivedTasks() const { return m_bRemoveArchivedTasks; }
	BOOL GetRemoveOnlyOnAbsoluteCompletion() const { return m_bRemoveOnlyOnAbsoluteCompletion; }
	CString GetAutoExportFolderPath() const;
	int GetNotifyDueByOnLoad() const { return m_bNotifyDueOnLoad ? m_nNotifyDueByOnLoad : PFP_DONTNOTIFY; }
	int GetNotifyDueByOnSwitch() const { return m_bNotifyDueOnSwitch ? m_nNotifyDueByOnSwitch : PFP_DONTNOTIFY; }
	BOOL GetDisplayDueTasksInHtml() const { return m_bDisplayDueTasksInHtml; }
	BOOL GetRefreshFindOnLoad() const { return m_bRefreshFindOnLoad; }
	BOOL GetDueTaskTitlesOnly() const { return m_bDueTaskTitlesOnly; }
	CString GetDueTaskStylesheet() const { return m_sDueTasksStylesheet; }
	CString GetSaveExportStylesheet() const { return m_sSaveExportStylesheet; }
	CString GetDueTaskPerson() const { return m_bOnlyShowDueTasksForPerson ? m_sDueTaskPerson : _T(""); }
	BOOL GetWarnAddDeleteArchive() const { return m_bWarnAddDeleteArchive; }
	BOOL GetDontRemoveFlagged() const { return m_bDontRemoveFlagged; }
	BOOL GetExpandTasksOnLoad() const { return m_bExpandTasks; }
	BOOL GetAutoSaveOnSwitchTasklist() const { return m_bAutoSaveOnSwitchTasklist; }
	BOOL GetAutoSaveOnSwitchApp() const { return m_bAutoSaveOnSwitchApp; }

//	BOOL Get() const { return m_b; }

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesFilePage)
	enum { IDD = IDD_PREFFILE_PAGE };
	CComboBox	m_cbOtherExporters;
	CFileEdit	m_eSaveExportStylesheet;
	CFileEdit	m_eDueTaskStylesheet;
	CComboBox	m_cbFontSize;
	CFileEdit	m_eExportFolderPath;
	BOOL	m_bExportToFolder;
	CString	m_sExportFolderPath;
	BOOL	m_bDisplayDueTasksInHtml;
	BOOL	m_bRefreshFindOnLoad;
	BOOL	m_bDueTaskTitlesOnly;
	CString	m_sSaveExportStylesheet;
	CString	m_sDueTasksStylesheet;
	BOOL	m_bUseStylesheetForSaveExport;
	BOOL	m_bUseStyleSheetForDueTasks;
	BOOL	m_bOnlyShowDueTasksForPerson;
	CString	m_sDueTaskPerson;
	BOOL	m_bWarnAddDeleteArchive;
	BOOL	m_bDontRemoveFlagged;
	BOOL	m_bExpandTasks;
	BOOL	m_bAutoSaveOnSwitchTasklist;
	BOOL	m_bAutoSaveOnSwitchApp;
	int		m_bOtherExport;
	int		m_nOtherExporter;
	//}}AFX_DATA
	BOOL	m_bNotifyDueOnLoad, m_bNotifyDueOnSwitch;
	int		m_nNotifyDueByOnLoad, m_nNotifyDueByOnSwitch;
	BOOL	m_bAutoArchive;
	BOOL	m_bNotifyReadOnly;
	CComboBox	m_cbAutoSave;
	BOOL	m_bRemoveArchivedTasks;
	BOOL	m_bRemoveOnlyOnAbsoluteCompletion;
	int		m_nAutoSaveFrequency;
	BOOL	m_bAutoSave;
	BOOL	m_bAutoExport;
	CGroupLineManager m_mgrGroupLines;
	const CImportExportMgr* m_pExportMgr;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesFilePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesFilePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnExporttofolder();
	afx_msg void OnAutoexport();
	afx_msg void OnNotifyDueOnLoad();
	afx_msg void OnDisplayduetasksinhtml();
	afx_msg void OnUsestylesheetfordueitems();
	afx_msg void OnUsestylesheetforsave();
	afx_msg void OnOnlyshowduetaskforperson();
	afx_msg void OnHtmlexport();
	afx_msg void OnOtherexport();
	//}}AFX_MSG
	afx_msg void OnNotifyDueOnSwitch();
	afx_msg void OnRemovearchiveditems();
	afx_msg void OnAutosave();
	DECLARE_MESSAGE_MAP()

   virtual void LoadPreferences(const CPreferences& prefs);
   virtual void SavePreferences(CPreferences& prefs);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESFILEPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
