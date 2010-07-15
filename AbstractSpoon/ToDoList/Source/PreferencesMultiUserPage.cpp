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

// PreferencesMultiUserPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesMultiUserPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesMultiUserPage property page

IMPLEMENT_DYNCREATE(CPreferencesMultiUserPage, CPreferencesPageBase)

CPreferencesMultiUserPage::CPreferencesMultiUserPage() : 
   CPreferencesPageBase(CPreferencesMultiUserPage::IDD)
{
	//{{AFX_DATA_INIT(CPreferencesMultiUserPage)
	m_bUse3rdPartySourceControl = FALSE;
	//}}AFX_DATA_INIT
}

CPreferencesMultiUserPage::~CPreferencesMultiUserPage()
{
}

void CPreferencesMultiUserPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesMultiUserPage)
	DDX_Control(pDX, IDC_NOCHANGETIME, m_cbNoEditTime);
	DDX_Check(pDX, IDC_CHECKINONNOEDIT, m_bCheckinNoChange);
	DDX_Check(pDX, IDC_USE3RDPARTYSOURCECTRL, m_bUse3rdPartySourceControl);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_REMOTEFILECHECK, m_cbRemoteFileCheck);
	DDX_Check(pDX, IDC_PROMPTRELOADONWRITABLE, m_bPromptReloadOnWritable);
	DDX_Check(pDX, IDC_PROMPTRELOADONCHANGE, m_bPromptReloadOnTimestamp);
	DDX_Check(pDX, IDC_ENABLESOURCECONTROL, m_bEnableSourceControl);
	DDX_Check(pDX, IDC_SOURCECONTROLLANONLY, m_bSourceControlLanOnly);
	DDX_Check(pDX, IDC_AUTOCHECKOUT, m_bAutoCheckOut);
	DDX_Check(pDX, IDC_CHECKOUTONCHECKIN, m_bCheckoutOnCheckin);
	DDX_CBIndex(pDX, IDC_READONLYRELOADOPTION, m_nReadonlyReloadOption);
	DDX_CBIndex(pDX, IDC_TIMESTAMPRELOADOPTION, m_nTimestampReloadOption);
	DDX_Check(pDX, IDC_CHECKINONCLOSE, m_bCheckinOnClose);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		CString sFreq;
		m_cbRemoteFileCheck.GetLBText(m_cbRemoteFileCheck.GetCurSel(), sFreq);
		m_nRemoteFileCheckFreq = atoi(sFreq);

		m_cbNoEditTime.GetLBText(m_cbNoEditTime.GetCurSel(), sFreq);
		m_nCheckinNoEditTime = atoi(sFreq);
	}
	else
	{
		CString sFreq;
		sFreq.Format("%d", m_nRemoteFileCheckFreq);

		if (CB_ERR == m_cbRemoteFileCheck.SelectString(-1, sFreq))
		{
			m_nRemoteFileCheckFreq = 30;
			m_cbRemoteFileCheck.SelectString(-1, "30");
		}

		sFreq.Format("%d", m_nCheckinNoEditTime);

		if (CB_ERR == m_cbNoEditTime.SelectString(-1, sFreq))
		{
			m_nCheckinNoEditTime = 0;
			m_cbRemoteFileCheck.SelectString(-1, "10");
		}
	}
}


BEGIN_MESSAGE_MAP(CPreferencesMultiUserPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesMultiUserPage)
	ON_BN_CLICKED(IDC_CHECKINONNOEDIT, OnCheckinonnoedit)
	ON_BN_CLICKED(IDC_USE3RDPARTYSOURCECTRL, OnUse3rdpartysourcectrl)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ENABLESOURCECONTROL, OnEnablesourcecontrol)
	ON_BN_CLICKED(IDC_PROMPTRELOADONWRITABLE, OnPromptreloadonwritable)
	ON_BN_CLICKED(IDC_PROMPTRELOADONCHANGE, OnPromptreloadontimestamp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesMultiUserPage message handlers

BOOL CPreferencesMultiUserPage::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	GetDlgItem(IDC_ENABLESOURCECONTROL)->EnableWindow(!m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_SOURCECONTROLLANONLY)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_AUTOCHECKOUT)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_CHECKOUTONCHECKIN)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_CHECKINONCLOSE)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_CHECKINONNOEDIT)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_NOCHANGETIME)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl && m_bCheckinNoChange);
	
	GetDlgItem(IDC_READONLYRELOADOPTION)->EnableWindow(m_bPromptReloadOnWritable);
	GetDlgItem(IDC_TIMESTAMPRELOADOPTION)->EnableWindow(m_bPromptReloadOnTimestamp);

	GetDlgItem(IDC_USE3RDPARTYSOURCECTRL)->EnableWindow(!m_bEnableSourceControl);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesMultiUserPage::OnEnablesourcecontrol() 
{
	UpdateData();

	GetDlgItem(IDC_ENABLESOURCECONTROL)->EnableWindow(!m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_SOURCECONTROLLANONLY)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_AUTOCHECKOUT)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_CHECKOUTONCHECKIN)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_CHECKINONCLOSE)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_CHECKINONNOEDIT)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl);
	GetDlgItem(IDC_NOCHANGETIME)->EnableWindow(m_bEnableSourceControl && !m_bUse3rdPartySourceControl && m_bCheckinNoChange);

	// can't have simple source control and 3rd party source control
	GetDlgItem(IDC_USE3RDPARTYSOURCECTRL)->EnableWindow(!m_bEnableSourceControl);

	if (m_bEnableSourceControl)
	{
		m_bUse3rdPartySourceControl = FALSE;
		UpdateData(FALSE);
	}
}

int CPreferencesMultiUserPage::GetReadonlyReloadOption() const
{ 
	if (!m_bPromptReloadOnWritable)
		return RO_NO;
	else
		return m_nReadonlyReloadOption + 1; 
}

int CPreferencesMultiUserPage::GetTimestampReloadOption() const 
{ 
	if (!m_bPromptReloadOnTimestamp)
		return RO_NO;
	else
		return m_nTimestampReloadOption + 1; 
}

void CPreferencesMultiUserPage::OnPromptreloadonwritable() 
{
	UpdateData();
	GetDlgItem(IDC_READONLYRELOADOPTION)->EnableWindow(m_bPromptReloadOnWritable);
}

void CPreferencesMultiUserPage::OnPromptreloadontimestamp() 
{
	UpdateData();
	GetDlgItem(IDC_TIMESTAMPRELOADOPTION)->EnableWindow(m_bPromptReloadOnTimestamp);
}

void CPreferencesMultiUserPage::OnCheckinonnoedit() 
{
	UpdateData();
	GetDlgItem(IDC_NOCHANGETIME)->EnableWindow(m_bEnableSourceControl && m_bCheckinNoChange);
}

void CPreferencesMultiUserPage::LoadPreferences(const CPreferences& prefs)
{
	m_bEnableSourceControl = prefs.GetProfileInt("Preferences", "EnableSourceControl", FALSE);
	m_bSourceControlLanOnly = prefs.GetProfileInt("Preferences", "SourceControlLanOnly", TRUE);
	m_bPromptReloadOnWritable = prefs.GetProfileInt("Preferences", "PromptReloadOnWritable", TRUE);
	m_bAutoCheckOut = prefs.GetProfileInt("Preferences", "AutoCheckOut", FALSE);
	m_bPromptReloadOnTimestamp = prefs.GetProfileInt("Preferences", "PromptReloadOnTimestamp", TRUE);
	m_bCheckoutOnCheckin = prefs.GetProfileInt("Preferences", "CheckoutOnCheckin", TRUE);
	m_nReadonlyReloadOption = prefs.GetProfileInt("Preferences", "ReadonlyReloadOption", RO_ASK) - 1;
	m_nTimestampReloadOption = prefs.GetProfileInt("Preferences", "TimestampReloadOption", RO_ASK) - 1;
	m_bCheckinOnClose = prefs.GetProfileInt("Preferences", "CheckinOnClose", TRUE);
	m_nRemoteFileCheckFreq = prefs.GetProfileInt("Preferences", "RemoteFileCheckFrequency", 30);
	m_nCheckinNoEditTime = prefs.GetProfileInt("Preferences", "CheckinNoEditTime", 1);
	m_bCheckinNoChange = prefs.GetProfileInt("Preferences", "CheckinNoEdit", TRUE);
	m_bUse3rdPartySourceControl = !m_bEnableSourceControl && prefs.GetProfileInt("Preferences", "Use3rdPartySourceControl", FALSE);
}

void CPreferencesMultiUserPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "PromptReloadOnWritable", m_bPromptReloadOnWritable);
	prefs.WriteProfileInt("Preferences", "PromptReloadOnTimestamp", m_bPromptReloadOnTimestamp);
	prefs.WriteProfileInt("Preferences", "EnableSourceControl", m_bEnableSourceControl);
	prefs.WriteProfileInt("Preferences", "SourceControlLanOnly", m_bSourceControlLanOnly);
	prefs.WriteProfileInt("Preferences", "AutoCheckOut", m_bAutoCheckOut);
	prefs.WriteProfileInt("Preferences", "CheckoutOnCheckin", m_bCheckoutOnCheckin);
	prefs.WriteProfileInt("Preferences", "ReadonlyReloadOption", m_nReadonlyReloadOption + 1);
	prefs.WriteProfileInt("Preferences", "TimestampReloadOption", m_nTimestampReloadOption + 1);
	prefs.WriteProfileInt("Preferences", "CheckinOnClose", m_bCheckinOnClose);
	prefs.WriteProfileInt("Preferences", "RemoteFileCheckFrequency", m_nRemoteFileCheckFreq);
	prefs.WriteProfileInt("Preferences", "CheckinNoEditTime", m_nCheckinNoEditTime);
	prefs.WriteProfileInt("Preferences", "CheckinNoEdit", m_bCheckinNoChange);
	prefs.WriteProfileInt("Preferences", "Use3rdPartySourceControl", m_bUse3rdPartySourceControl);
//	prefs.WriteProfileInt("Preferences", "", m_b);
}

void CPreferencesMultiUserPage::OnUse3rdpartysourcectrl() 
{
	UpdateData();

	OnEnablesourcecontrol(); // to re-enable controls
}
