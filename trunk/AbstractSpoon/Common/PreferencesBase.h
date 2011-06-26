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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

#if !defined(AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_)
#define AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Preferences.h"
#include "../../CodeProject/Source/PropertyPageHost.h"

/////////////////////////////////////////////////////////////////////////////
// CPreferencesPageBase dialog

const UINT WM_PPB_CTRLCHANGE = ::RegisterWindowMessage(_T("WM_PPB_CTRLCHANGE"));

class CPreferencesPageBase : public CPropertyPage
{
	DECLARE_DYNCREATE(CPreferencesPageBase)

// Construction
public:
	CPreferencesPageBase(UINT nID = 0);
	~CPreferencesPageBase();

	virtual void LoadPreferences(const CPreferences& /*prefs*/)
	{
	}
	virtual void SavePreferences(CPreferences& /*prefs*/)
	{
	}

protected:
	CWnd* GetDlgItem(UINT nID) const;

	void OnControlChange(UINT nID = -1);
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlgBase dialog

class CPreferencesDlgBase : public CDialog
{
// Construction
public:
	CPreferencesDlgBase(UINT nID = 0, CWnd* pParent = NULL);   // standard constructor

	int DoModal(int nInitPage = -1);

protected:
	int m_nInitPage;

protected:
	afx_msg void OnDestroy();
	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnOK();
	virtual BOOL OnInitDialog();

	void OnApply(); // called from derived class

	BOOL EnsurePageCreated(int nPage)
	{
		return m_pphost.EnsurePageCreated(nPage);
	}
	BOOL EnsurePageCreated(const CPreferencesPageBase* pPage)
	{
		return m_pphost.EnsurePageCreated(pPage);
	}

	virtual BOOL SetActivePage(int nPage);
	virtual BOOL SetActivePage(CPreferencesPageBase* pPage);
	virtual BOOL AddPage(CPreferencesPageBase* pPage);

	void LoadPreferences();
	void SavePreferences();

	BOOL CreatePPHost(UINT nHostID);
	BOOL CreatePPHost(LPRECT pRect);

	int GetActiveIndex() const
	{
		return m_pphost.GetActiveIndex();
	}
	CPreferencesPageBase* GetActivePage()
	{
		return (CPreferencesPageBase*)m_pphost.GetActivePage();
	}
	int FindPage(CPreferencesPageBase* pPage)
	{
		return m_pphost.FindPage(pPage);
	}

	void ForwardMessage(UINT nMsg)
	{
		m_pphost.ForwardMessage(nMsg);
	}

private:
	CPropertyPageHost m_pphost;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREFERENCESBASE_H__19D0237F_28DC_4B45_9762_DF9B4F7D8492__INCLUDED_)
