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

#if !defined(AFX_TDLIMPORTDIALOG_H__F3B10AEE_B46C_4183_AC05_FB72D7C5AFA4__INCLUDED_)
#define AFX_TDLIMPORTDIALOG_H__F3B10AEE_B46C_4183_AC05_FB72D7C5AFA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TDLImportDialog.h : header file
//

#include "../../Common/FileEdit.h"
#include "../../Common/ImportExportMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CTDLImportDialog dialog

enum TDLID_IMPORTTO
{
	TDIT_NEWTASKLIST,
	TDIT_TOPTASKLIST,
	TDIT_SELECTEDTASK,
};

class CTDLImportDialog : public CDialog
{
// Construction
public:
	CTDLImportDialog(const CImportExportMgr& mgr, CWnd* pParent = NULL);   // standard constructor

	BOOL ImportTasklist() const;
	int GetImporterIndex() const; // returns -1 if 'ToDoList'
	TDLID_IMPORTTO GetImportTo() const;
	BOOL GetImportFromClipboard() const;
	CString GetImportFilePath() const;
	CString GetImportClipboardText() const;

protected:
// Dialog Data
	//{{AFX_DATA(CTDLImportDialog)
	enum { IDD = IDD_IMPORT_DIALOG };
	CFileEdit	m_eFilePath;
	CComboBox	m_cbFormat;
	int		m_bFromClipboard;
	CString	m_sFromFilePath;
	int		m_nImportTo;
	int		m_nFormatOption;
	CString	m_sClipboardText;
	//}}AFX_DATA
	const CImportExportMgr& m_mgrImportExport;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTDLImportDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	virtual void OnOK();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTDLImportDialog)
	afx_msg void OnChangeImportFrom();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeFormatoptions();
	afx_msg void OnChangeClipboardtext();
	afx_msg void OnChangeFilepath();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL CurImporterHasFilter() const;
	CString GetCurImporterFilter() const;
	void EnableOK();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TDLIMPORTDIALOG_H__F3B10AEE_B46C_4183_AC05_FB72D7C5AFA4__INCLUDED_)
