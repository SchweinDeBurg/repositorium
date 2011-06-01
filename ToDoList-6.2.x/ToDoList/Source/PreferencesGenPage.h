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

#if !defined(AFX_PREFERENCESGENPAGE_H__7A8CC153_F3FB_4FBA_8EB9_B8ADF0A59982__INCLUDED_)
#define AFX_PREFERENCESGENPAGE_H__7A8CC153_F3FB_4FBA_8EB9_B8ADF0A59982__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesGenPage.h : header file
//

#include "../../../CodeProject/Source/HotKeyCtrlEx.h"
#include "../../Common/PreferencesBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesGenPage dialog

const UINT WM_PGP_CLEARMRU = ::RegisterWindowMessage(_T("WM_PGP_CLEARMRU"));

enum // tray options
{
	STO_NONE = -1,
	STO_ONMINIMIZE,
	STO_ONCLOSE,
	STO_ONMINCLOSE,
};

class CPreferencesGenPage : public CPreferencesPageBase
{
	DECLARE_DYNCREATE(CPreferencesGenPage)

// Construction
public:
	CPreferencesGenPage();
	~CPreferencesGenPage();

	BOOL GetAlwaysOnTop() const
	{
		return m_bAlwaysOnTop;
	}
	BOOL GetUseSysTray() const
	{
		return m_bUseSysTray;
	}
	int GetSysTrayOption() const
	{
		return !m_bUseSysTray ? STO_NONE : m_nSysTrayOption;
	}
	BOOL GetConfirmDelete() const
	{
		return m_bConfirmDelete;
	}
	BOOL GetConfirmSaveOnExit() const
	{
		return m_bConfirmSaveOnExit;
	}
	BOOL GetShowOnStartup() const
	{
		return m_bShowOnStartup;
	}
	BOOL GetMultiInstance() const
	{
		return m_bMultiInstance;
	}
	BOOL GetToggleTrayVisibility() const
	{
		return m_bToggleTrayVisibility;
	}
	BOOL GetGlobalHotkey() const
	{
		return m_bSpecifyGlobalHotkey ? m_dwGlobalHotkey : 0;
	}
	BOOL GetAddFilesToMRU() const
	{
		return m_bAddFilesToMRU;
	}
	BOOL GetEnableTDLExtension() const
	{
		return m_bEnableTDLExtension;
	}
	BOOL GetAutoCheckForUpdates() const
	{
		return m_bAutoCheckForUpdates;
	}
	BOOL GetEscapeMinimizes() const
	{
		return m_bEscapeMinimizes;
	}
	BOOL GetEnableTDLProtocol() const
	{
		return m_bEnableTDLProtocol;
	}
	BOOL GetEnableDelayedLoading() const
	{
		return m_bEnableDelayedLoading;
	}

// Dialog Data
	//{{AFX_DATA(CPreferencesGenPage)
	enum { IDD = IDD_PREFGEN_PAGE };
	CStatic m_stVistaShield;
	CHotKeyCtrlEx   m_hkGlobal;
	BOOL    m_bAlwaysOnTop;
	BOOL    m_bUseSysTray;
	BOOL    m_bConfirmDelete;
	BOOL    m_bConfirmSaveOnExit;
	BOOL    m_bMultiInstance;
	BOOL    m_bShowOnStartup;
	int     m_nSysTrayOption;
	BOOL    m_bToggleTrayVisibility;
	BOOL    m_bSpecifyGlobalHotkey;
	BOOL    m_bAddFilesToMRU;
	BOOL    m_bEnableTDLExtension;
	BOOL    m_bAutoCheckForUpdates;
	BOOL    m_bEscapeMinimizes;
	BOOL    m_bEnableTDLProtocol;
	BOOL    m_bEnableDelayedLoading;
	//}}AFX_DATA
	DWORD   m_dwGlobalHotkey;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesGenPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesGenPage)
	afx_msg void OnUseSystray();
	virtual BOOL OnInitDialog();
	afx_msg void OnSpecifyglobalhotkey();
	afx_msg void OnMultiinstance();
	afx_msg void OnClearMRU();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESGENPAGE_H__7A8CC153_F3FB_4FBA_8EB9_B8ADF0A59982__INCLUDED_)
