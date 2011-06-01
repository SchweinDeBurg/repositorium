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

// PreferencesBase.cpp : implementation file
//

#include "StdAfx.h"
#include "PreferencesBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesPageBase property page

IMPLEMENT_DYNCREATE(CPreferencesPageBase, CPropertyPage)

CPreferencesPageBase::CPreferencesPageBase(UINT nID) : CPropertyPage(nID)
{
}

CPreferencesPageBase::~CPreferencesPageBase()
{
}

BEGIN_MESSAGE_MAP(CPreferencesPageBase, CPropertyPage)
	//{{AFX_MSG_MAP(CPreferencesPageBase)
	//}}AFX_MSG_MAP
	ON_CONTROL_RANGE(BN_CLICKED, 0, 0xffff, OnControlChange)
	ON_CONTROL_RANGE(EN_CHANGE, 0, 0xffff, OnControlChange)
	ON_CONTROL_RANGE(CBN_EDITCHANGE, 0, 0xffff, OnControlChange)
	ON_CONTROL_RANGE(CBN_SELCHANGE, 0, 0xffff, OnControlChange)
	ON_CONTROL_RANGE(CLBN_CHKCHANGE, 0, 0xffff, OnControlChange)
END_MESSAGE_MAP()

CWnd* CPreferencesPageBase::GetDlgItem(UINT nID) const
{
	static CWnd wnd;
	CWnd* pWnd = CDialog::GetDlgItem(nID);

	if (pWnd)
	{
		return pWnd;
	}

	// else
	wnd.m_hWnd = NULL;
	return &wnd;
}

void CPreferencesPageBase::OnControlChange(UINT /*nID*/)
{
	if (IsWindowVisible())
	{
		GetParent()->SendMessage(WM_PPB_CTRLCHANGE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPreferencesDlgBase dialog


CPreferencesDlgBase::CPreferencesDlgBase(UINT nID, CWnd* pParent) : CDialog(nID, pParent), m_nInitPage(-1)
{
}

BEGIN_MESSAGE_MAP(CPreferencesDlgBase, CDialog)
	//{{AFX_MSG_MAP(CPreferencesDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPreferencesDlgBase::OnOK()
{
	CDialog::OnOK();

	m_pphost.OnOK();
	SavePreferences();
}

void CPreferencesDlgBase::OnApply()
{
	m_pphost.OnApply();
	SavePreferences();
}

int CPreferencesDlgBase::DoModal(int nInitPage)
{
	if (nInitPage != -1)
	{
		m_nInitPage = nInitPage;
	}

	return CDialog::DoModal();
}

BOOL CPreferencesDlgBase::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPreferencesDlgBase::CreatePPHost(UINT nHostID)
{
	if (GetDlgItem(nHostID))
	{
		CRect rPPHost;
		GetDlgItem(nHostID)->GetWindowRect(rPPHost);
		ScreenToClient(rPPHost);

		return CreatePPHost(rPPHost);
	}

	// else
	return FALSE;
}

BOOL CPreferencesDlgBase::CreatePPHost(LPRECT pRect)
{
	if (m_pphost.Create(pRect, this))
	{
		if (m_nInitPage > 0 && m_nInitPage < m_pphost.GetPageCount())
		{
			return SetActivePage(m_nInitPage);
		}

		// else
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CPreferencesDlgBase::SetActivePage(int nPage)
{
	CPropertyPage* pPage = m_pphost.GetPage(nPage);
	ASSERT(pPage);

	return m_pphost.SetActivePage(pPage);
}

BOOL CPreferencesDlgBase::SetActivePage(CPreferencesPageBase* pPage)
{
	if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
	{
		return m_pphost.SetActivePage(pPage);
	}

	// else
	return FALSE;
}

BOOL CPreferencesDlgBase::AddPage(CPreferencesPageBase* pPage)
{
	if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
	{
		return m_pphost.AddPage(pPage);
	}

	// else
	return FALSE;
}

void CPreferencesDlgBase::OnDestroy()
{
	CDialog::OnDestroy();
}

void CPreferencesDlgBase::LoadPreferences()
{
	// new storage object
	CPreferences prefs;

	// cycle the page loading the preferences for each one
	int nPage = m_pphost.GetPageCount();

	while (nPage--)
	{
		CPreferencesPageBase* pPage = (CPreferencesPageBase*)m_pphost.GetPage(nPage);

		if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
		{
			pPage->LoadPreferences(prefs);
		}
	}

	// initial page
	if (m_nInitPage < 0 || m_nInitPage >= m_pphost.GetPageCount())
	{
		m_nInitPage = prefs.GetProfileInt(_T("Preferences"), _T("StartPage"), 0);
	}
}

void CPreferencesDlgBase::SavePreferences()
{
	// new storage object
	CPreferences prefs;

	// cycle the page saving the preferences for each one
	int nPage = m_pphost.GetPageCount();

	while (nPage--)
	{
		CPreferencesPageBase* pPage = (CPreferencesPageBase*)m_pphost.GetPage(nPage);

		if (pPage->IsKindOf(RUNTIME_CLASS(CPreferencesPageBase)))
		{
			pPage->SavePreferences(prefs);
		}
	}

	prefs.WriteProfileInt(_T("Preferences"), _T("StartPage"), m_pphost.GetActiveIndex());
}

/////////////////////////////////////////////////////////////////////////////
