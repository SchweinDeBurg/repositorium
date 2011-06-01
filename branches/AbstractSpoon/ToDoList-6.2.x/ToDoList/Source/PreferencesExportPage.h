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
// - added AbstractSpoon Software copyright notice and license information
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

#if !defined(AFX_PREFERENCESEXPORTPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
#define AFX_PREFERENCESEXPORTPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesExportPage.h : header file
//

#include "../../../CodeProject/Source/FontComboBox.h"
#include "../../Common/PreferencesBase.h"
#include "../../../CodeProject/Source/EnEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesExportPage dialog

class CPreferencesExportPage : public CPreferencesPageBase
{
	DECLARE_DYNCREATE(CPreferencesExportPage)

// Construction
public:
	CPreferencesExportPage();
	~CPreferencesExportPage();

	int GetTextIndent() const
	{
		return m_bUseSpaceIndents ? m_nTextIndent : -1;
	}
	BOOL GetExportVisibleColsOnly() const
	{
		return m_bExportVisibleOnly;
	}
	CString GetHtmlFont() const
	{
		return m_sHtmlFont;
	}
	CString GetHtmlCharSet() const;
	int GetHtmlFontSize() const
	{
		return m_nHtmlFontSize;
	}
	BOOL GetPreviewExport() const
	{
		return m_bPreviewExport;
	}
	BOOL GetExportParentTitleCommentsOnly() const
	{
		return m_bExportParentTitleCommentsOnly;
	}
	int GetExportSpaceForNotes() const
	{
		return m_bExportSpaceForNotes ? m_nLineSpaces : 0;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesExportPage)
	enum { IDD = IDD_PREFEXPORT_PAGE };
	CMaskEdit   m_eLineSpaces;
	CComboBox   m_cbFontSize;
	BOOL    m_bExportParentTitleCommentsOnly;
	BOOL    m_bExportSpaceForNotes;
	CString m_sHtmlCharSet;
	int     m_bUseSpaceIndents;
	int     m_nTextIndent;
	int     m_nLineSpaces;
	//}}AFX_DATA
	CFontComboBox   m_cbFonts;
	BOOL    m_bPreviewExport;
	BOOL    m_bExportVisibleOnly;
	CString m_sHtmlFont;
	int m_nHtmlFontSize;
	CEnEdit m_eCharset;
	CMaskEdit m_eTextIndent;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesExportPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesExportPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeTextIndentType();
	afx_msg void OnExportspacefornotes();
	//}}AFX_MSG
	afx_msg LRESULT OnEEBtnClick(WPARAM wp, LPARAM lp);
	DECLARE_MESSAGE_MAP()

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESEXPORTPAGE_H__0A884806_5921_4C13_B368_6D14A441ADAC__INCLUDED_)
