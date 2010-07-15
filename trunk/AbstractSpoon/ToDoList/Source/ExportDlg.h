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

#if !defined(AFX_EXPORTDLG_H__2F5B4FD1_E968_464E_9734_AC995DB13B35__INCLUDED_)
#define AFX_EXPORTDLG_H__2F5B4FD1_E968_464E_9734_AC995DB13B35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ExportDlg.h : header file
//

#include "TaskSelectionDlg.h"
#include "../../Common/FileEdit.h"
#include "../../Common/ImportExportMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CExportDlg dialog

enum { ED_HTMLFMT, ED_TEXTFMT };

class CExportDlg : public CDialog
{
// Construction
public:
	CExportDlg(const CImportExportMgr& mgr, BOOL bSingleTaskList, BOOL bShowSubtaskCheckbox = TRUE, 
				LPCTSTR szFilePath = NULL, LPCTSTR szFolderPath = NULL, CWnd* pParent = NULL);

	BOOL GetExportAllTasklists();
	int GetExportFormat() { return s_nFormatOption; }
	CString GetExportPath(); // can be folder or path
	BOOL GetExportOneFile() { return (m_bSingleTaskList/* || m_bExportOneFile*/); }

	const CTaskSelectionDlg& GetTaskSelection() const { return m_taskSel; }

protected:
// Dialog Data
	//{{AFX_DATA(CExportDlg)
	CComboBox	m_cbFormat;
	CFileEdit	m_eExportPath;
	int		m_nExportOption;
	CString	m_sExportPath;
	BOOL	m_bExportOneFile;
	CString	m_sPathLabel;
	//}}AFX_DATA
	CTaskSelectionDlg m_taskSel;
	BOOL m_bSingleTaskList; 
	CString m_sFolderPath, m_sFilePath, m_sOrgFilePath, m_sOrgFolderPath;
	const CImportExportMgr& m_mgrImportExport;
	static int s_nFormatOption; // persists just for the current session

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CExportDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CExportDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTasklistoptions();
	afx_msg void OnSelchangeFormatoptions();
	afx_msg void OnExportonefile();
	afx_msg void OnChangeExportpath();
	afx_msg LRESULT OnChangeTaskSelOption(WPARAM wp, LPARAM lp);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableOK();

	void ReplaceExtension(CString& sPathName, int nFormat);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EXPORTDLG_H__2F5B4FD1_E968_464E_9734_AC995DB13B35__INCLUDED_)
