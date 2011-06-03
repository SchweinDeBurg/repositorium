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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_PREFERENCESMULTIUSERPAGE_H__D7484AFF_704B_4FA9_94DA_9AB2F5364816__INCLUDED_)
#define AFX_PREFERENCESMULTIUSERPAGE_H__D7484AFF_704B_4FA9_94DA_9AB2F5364816__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PreferencesMultiUserPage.h : header file
//

#include "../../Common/PreferencesBase.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesMultiUserPage dialog

enum // source control
{
	STSS_NONE,
	STSS_LANONLY,
	STSS_ALL,
};

enum // reload
{
	RO_NO,
	RO_ASK,
	RO_AUTO,
	RO_NOTIFY,
};

class CPreferencesMultiUserPage : public CPreferencesPageBase
{
	DECLARE_DYNCREATE(CPreferencesMultiUserPage)

// Construction
public:
	CPreferencesMultiUserPage();
	~CPreferencesMultiUserPage();

	int GetReadonlyReloadOption() const;
	int GetTimestampReloadOption() const;
	BOOL GetEnableSourceControl() const
	{
		return m_bEnableSourceControl && !m_bUse3rdPartySourceControl;
	}
	BOOL GetSourceControlLanOnly() const
	{
		return m_bEnableSourceControl && m_bSourceControlLanOnly;
	}
	BOOL GetAutoCheckOut() const
	{
		return m_bEnableSourceControl && m_bAutoCheckOut;
	}
	BOOL GetCheckoutOnCheckin() const
	{
		return m_bEnableSourceControl && m_bCheckoutOnCheckin;
	}
	BOOL GetCheckinOnClose() const
	{
		return m_bEnableSourceControl && m_bCheckinOnClose;
	}
	BOOL GetIncludeUserNameInCheckout() const
	{
		return m_bEnableSourceControl && m_bIncludeUserNameInCheckout;
	}
	UINT GetRemoteFileCheckFrequency() const
	{
		return m_nRemoteFileCheckFreq;
	}
	UINT GetCheckinOnNoEditTime() const
	{
		return (m_bEnableSourceControl && m_bCheckinNoChange) ? m_nCheckinNoEditTime : 0;
	}
	BOOL GetUsing3rdPartySourceControl() const
	{
		return m_bUse3rdPartySourceControl && !m_bEnableSourceControl;
	}

protected:
// Dialog Data
	//{{AFX_DATA(CPreferencesMultiUserPage)
	enum { IDD = IDD_PREFMULTIUSER_PAGE };
	CComboBox   m_cbNoEditTime;
	BOOL    m_bCheckinNoChange;
	BOOL    m_bUse3rdPartySourceControl;
	BOOL    m_bIncludeUserNameInCheckout;
	//}}AFX_DATA
	CComboBox   m_cbRemoteFileCheck;
	BOOL    m_bEnableSourceControl;
	BOOL    m_bSourceControlLanOnly;
	BOOL    m_bAutoCheckOut;
	BOOL    m_bCheckoutOnCheckin;
	int     m_nReadonlyReloadOption;
	int     m_nTimestampReloadOption;
	BOOL    m_bCheckinOnClose;
	UINT    m_nRemoteFileCheckFreq;
	BOOL    m_bPromptReloadOnWritable;
	BOOL    m_bPromptReloadOnTimestamp;
	UINT    m_nCheckinNoEditTime;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPreferencesMultiUserPage)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPreferencesMultiUserPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckinonnoedit();
	afx_msg void OnUse3rdpartysourcectrl();
	//}}AFX_MSG
	afx_msg void OnEnablesourcecontrol();
	afx_msg void OnPromptreloadonwritable();
	afx_msg void OnPromptreloadontimestamp();
	DECLARE_MESSAGE_MAP()

	virtual void LoadPreferences(const CPreferences& prefs);
	virtual void SavePreferences(CPreferences& prefs);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESMULTIUSERPAGE_H__D7484AFF_704B_4FA9_94DA_9AB2F5364816__INCLUDED_)
