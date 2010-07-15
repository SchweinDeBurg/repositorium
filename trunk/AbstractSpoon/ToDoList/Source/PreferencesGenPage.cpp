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

// PreferencesGenPage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesGenPage.h"

#include "../../../CodeProject/Source/OSVersion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifndef BCM_SETSHIELD
#	define BCM_SETSHIELD 0x0000160C
#endif
/////////////////////////////////////////////////////////////////////////////
// CPreferencesGenPage property page

IMPLEMENT_DYNCREATE(CPreferencesGenPage, CPreferencesPageBase)

CPreferencesGenPage::CPreferencesGenPage() : 
   CPreferencesPageBase(CPreferencesGenPage::IDD), m_hkGlobal(TRUE)
{
	//{{AFX_DATA_INIT(CPreferencesGenPage)
	//}}AFX_DATA_INIT

}

CPreferencesGenPage::~CPreferencesGenPage()
{
}

void CPreferencesGenPage::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesGenPage)
	DDX_Control(pDX, IDC_VISTASHIELD, m_stVistaShield);
	DDX_Control(pDX, IDC_GLOBALHOTKEY, m_hkGlobal);
	DDX_Check(pDX, IDC_MULTIINSTANCE, m_bMultiInstance);
	DDX_Check(pDX, IDC_SHOWONSTARTUP, m_bShowOnStartup);
	DDX_CBIndex(pDX, IDC_SYSTRAYOPTION, m_nSysTrayOption);
	DDX_Check(pDX, IDC_TOGGLETRAYVISIBILITY, m_bToggleTrayVisibility);
	DDX_Check(pDX, IDC_SPECIFYGLOBALHOTKEY, m_bSpecifyGlobalHotkey);
	DDX_Check(pDX, IDC_ADDFILESTOMRU, m_bAddFilesToMRU);
	DDX_Check(pDX, IDC_ENABLETDLEXTENSION, m_bEnableTDLExtension);
	DDX_Check(pDX, IDC_CHECKFORUPDATES, m_bAutoCheckForUpdates);
	DDX_Check(pDX, IDC_ESCAPEMINIMIZES, m_bEscapeMinimizes);
	DDX_Check(pDX, IDC_ENABLETDLPROTOCOL, m_bEnableTDLProtocol);
	DDX_Check(pDX, IDC_ENABLEDELAYEDLOADING, m_bEnableDelayedLoading);
	//}}AFX_DATA_MAP
	DDX_Check(pDX, IDC_ALWAYSONTOP, m_bAlwaysOnTop);
	DDX_Check(pDX, IDC_USESYSTRAY, m_bUseSysTray);
	DDX_Check(pDX, IDC_CONFIRMDELETE, m_bConfirmDelete);
	DDX_Check(pDX, IDC_CONFIRMSAVEONEXIT, m_bConfirmSaveOnExit);

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		WORD wVKeyCode = 0, wModifiers = 0;
		m_hkGlobal.GetHotKey(wVKeyCode, wModifiers);
		m_dwGlobalHotkey = MAKELONG(wVKeyCode, wModifiers);
	}
	else if (m_bSpecifyGlobalHotkey)
		m_hkGlobal.SetHotKey(m_dwGlobalHotkey);
}

BEGIN_MESSAGE_MAP(CPreferencesGenPage, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesGenPage)
	ON_BN_CLICKED(IDC_SPECIFYGLOBALHOTKEY, OnSpecifyglobalhotkey)
	ON_BN_CLICKED(IDC_MULTIINSTANCE, OnMultiinstance)
	ON_BN_CLICKED(IDC_CLEARMRU, OnClearMRU)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USESYSTRAY, OnUseSystray)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesGenPage message handlers

BOOL CPreferencesGenPage::OnInitDialog() 
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_SYSTRAYOPTION)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_TOGGLETRAYVISIBILITY)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_MULTIINSTANCE)->EnableWindow(!m_bSpecifyGlobalHotkey);
	GetDlgItem(IDC_SPECIFYGLOBALHOTKEY)->EnableWindow(!m_bMultiInstance);
	GetDlgItem(IDC_GLOBALHOTKEY)->EnableWindow(!m_bMultiInstance && m_bSpecifyGlobalHotkey);

	//GetDlgItem(IDC_ENABLETDLPROTOCOL)->SendMessage(BCM_SETSHIELD, 0, TRUE );
	if (COSVersion() <= OSV_XP)
		GetDlgItem(IDC_VISTASHIELD)->ShowWindow(SW_HIDE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesGenPage::OnUseSystray() 
{
	UpdateData();

	GetDlgItem(IDC_SYSTRAYOPTION)->EnableWindow(m_bUseSysTray);
	GetDlgItem(IDC_TOGGLETRAYVISIBILITY)->EnableWindow(m_bUseSysTray);
}

void CPreferencesGenPage::OnSpecifyglobalhotkey() 
{
	UpdateData();
	
	GetDlgItem(IDC_GLOBALHOTKEY)->EnableWindow(m_bSpecifyGlobalHotkey);
	GetDlgItem(IDC_MULTIINSTANCE)->EnableWindow(!m_bSpecifyGlobalHotkey);
}

void CPreferencesGenPage::OnMultiinstance() 
{
	UpdateData();
	
	GetDlgItem(IDC_SPECIFYGLOBALHOTKEY)->EnableWindow(!m_bMultiInstance);
	GetDlgItem(IDC_GLOBALHOTKEY)->EnableWindow(!m_bMultiInstance && m_bSpecifyGlobalHotkey);
}

void CPreferencesGenPage::OnClearMRU() 
{
   GetParent()->SendMessage(WM_PGP_CLEARMRU);
}

void CPreferencesGenPage::LoadPreferences(const CPreferences& prefs)
{
	// load settings
	m_bAlwaysOnTop = prefs.GetProfileInt("Preferences", "AlwaysOnTop", FALSE);
	m_bUseSysTray = prefs.GetProfileInt("Preferences", "UseSysTray", FALSE);
	m_bConfirmDelete = prefs.GetProfileInt("Preferences", "ConfirmDelete", FALSE);
	m_bConfirmSaveOnExit = prefs.GetProfileInt("Preferences", "ConfirmSaveOnExit", TRUE);
	m_bMultiInstance = prefs.GetProfileInt("Preferences", "MultiInstance", FALSE);
	m_bShowOnStartup = prefs.GetProfileInt("Preferences", "ShowOnStartup", TRUE);
	m_nSysTrayOption = prefs.GetProfileInt("Preferences", "SysTrayOption", STO_ONCLOSE);
	m_bToggleTrayVisibility = prefs.GetProfileInt("Preferences", "ToggleTrayVisibility", FALSE);
	m_dwGlobalHotkey = prefs.GetProfileInt("Preferences", "GlobalHotkey", 0);
	m_bSpecifyGlobalHotkey = (m_dwGlobalHotkey ? 1 : 0);
	m_bAddFilesToMRU = prefs.GetProfileInt("Preferences", "AddFilesToMRU", TRUE);
	m_bEnableTDLExtension = prefs.GetProfileInt("Preferences", "EnableTDLExtension", TRUE);
	m_bEnableTDLProtocol = prefs.GetProfileInt("Preferences", "EnableTDLProtocol", FALSE);
	m_bAutoCheckForUpdates = prefs.GetProfileInt("Preferences", "AutoCheckForUpdates", FALSE);
	m_bEscapeMinimizes = prefs.GetProfileInt("Preferences", "EscapeMinimizes", TRUE);
	m_bEnableDelayedLoading = prefs.GetProfileInt("Preferences", "EnableDelayedLoading", TRUE);
//	m_b = prefs.GetProfileInt("Preferences", "", TRUE);
}

void CPreferencesGenPage::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "AlwaysOnTop", m_bAlwaysOnTop);
	prefs.WriteProfileInt("Preferences", "UseSysTray", m_bUseSysTray);
	prefs.WriteProfileInt("Preferences", "ConfirmDelete", m_bConfirmDelete);
	prefs.WriteProfileInt("Preferences", "ConfirmSaveOnExit", m_bConfirmSaveOnExit);
	prefs.WriteProfileInt("Preferences", "ShowOnStartup", m_bShowOnStartup);
	prefs.WriteProfileInt("Preferences", "SysTrayOption", m_nSysTrayOption);
	prefs.WriteProfileInt("Preferences", "ToggleTrayVisibility", m_bToggleTrayVisibility);
	prefs.WriteProfileInt("Preferences", "MultiInstance", m_bMultiInstance);
	prefs.WriteProfileInt("Preferences", "GlobalHotkey", (m_bSpecifyGlobalHotkey ? m_dwGlobalHotkey : 0));
	prefs.WriteProfileInt("Preferences", "AddFilesToMRU", m_bAddFilesToMRU);
	prefs.WriteProfileInt("Preferences", "EnableTDLExtension", m_bEnableTDLExtension);
	prefs.WriteProfileInt("Preferences", "EnableTDLProtocol", m_bEnableTDLProtocol);
	prefs.WriteProfileInt("Preferences", "AutoCheckForUpdates", m_bAutoCheckForUpdates);
	prefs.WriteProfileInt("Preferences", "EscapeMinimizes", m_bEscapeMinimizes);
	prefs.WriteProfileInt("Preferences", "EnableDelayedLoading", m_bEnableDelayedLoading);
//	prefs.WriteProfileInt("Preferences", "", m_b);
}
