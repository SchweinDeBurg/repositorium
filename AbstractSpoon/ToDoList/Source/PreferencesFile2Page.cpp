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

// PreferencesFilePage.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "PreferencesFile2Page.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/DialogHelper.h"
#include "../../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFile2Page property page

IMPLEMENT_DYNCREATE(CPreferencesFile2Page, CPreferencesPageBase)

CPreferencesFile2Page::CPreferencesFile2Page() : 
		CPreferencesPageBase(CPreferencesFile2Page::IDD),
		m_eBackupLocation(FES_FOLDERS | FES_COMBOSTYLEBTN)
{
//	m_psp.dwFlags &= ~PSP_HASHELP;

	//{{AFX_DATA_INIT(CPreferencesFile2Page)
	m_nKeepBackups = 50;
	//}}AFX_DATA_INIT
}

CPreferencesFile2Page::~CPreferencesFile2Page()
{
}

void CPreferencesFile2Page::DoDataExchange(CDataExchange* pDX)
{
	CPreferencesPageBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPreferencesFile2Page)
	DDX_Control(pDX, IDC_NUMBACKUPSTOKEEP, m_cbKeepBackups);
	DDX_Control(pDX, IDC_BACKUPLOCATION, m_eBackupLocation);
	DDX_Check(pDX, IDC_BACKUPONSAVE, m_bBackupOnSave);
	DDX_Text(pDX, IDC_BACKUPLOCATION, m_sBackupLocation);
	DDX_CBIndex(pDX, IDC_NUMBACKUPSTOKEEP, m_nKeepBackups);
	//}}AFX_DATA_MAP

	// custom
	if (pDX->m_bSaveAndValidate)
	{
		m_nKeepBackups = CDialogHelper::GetSelectedItemAsValue(m_cbKeepBackups);

	}
	else
	{
		if (!CDialogHelper::SelectItemByValue(m_cbKeepBackups, m_nKeepBackups))
		{
			if (m_nKeepBackups == 0) // all
				m_cbKeepBackups.SelectString(-1, "All");
			else
			{
				m_nKeepBackups = 10;
				m_cbKeepBackups.SelectString(-1, "10");
			}
		}
	}
}


BEGIN_MESSAGE_MAP(CPreferencesFile2Page, CPreferencesPageBase)
	//{{AFX_MSG_MAP(CPreferencesFile2Page)
	ON_BN_CLICKED(IDC_BACKUPONSAVE, OnBackuponsave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreferencesFile2Page message handlers

BOOL CPreferencesFile2Page::OnInitDialog() 
{
	CPreferencesPageBase::OnInitDialog();

	m_mgrGroupLines.AddGroupLine(IDC_BACKUPGROUP, *this);

	GetDlgItem(IDC_BACKUPLOCATION)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPLOCATIONLABEL)->EnableWindow(m_bBackupOnSave);

	GetDlgItem(IDC_BACKUPCOUNTLABEL)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_NUMBACKUPSTOKEEP)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPCOUNTTRAIL)->EnableWindow(m_bBackupOnSave);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CPreferencesFile2Page::LoadPreferences(const CPreferences& prefs)
{
	m_bBackupOnSave = prefs.GetProfileInt("Preferences", "BackupOnSave", TRUE);
	m_sBackupLocation = prefs.GetProfileString("Preferences", "BackupLocation", "backup");
	m_nKeepBackups = prefs.GetProfileInt("Preferences", "KeepBackups", 10);
//	m_b = prefs.GetProfileInt("Preferences", "", FALSE);
}

void CPreferencesFile2Page::SavePreferences(CPreferences& prefs)
{
	// save settings
	prefs.WriteProfileInt("Preferences", "BackupOnSave", m_bBackupOnSave);
	prefs.WriteProfileString("Preferences", "BackupLocation", m_sBackupLocation);
	prefs.WriteProfileInt("Preferences", "KeepBackups", m_nKeepBackups);

//	prefs.WriteProfileInt("Preferences", "", m_b);
}

void CPreferencesFile2Page::OnBackuponsave() 
{
	UpdateData();

	GetDlgItem(IDC_BACKUPLOCATION)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPLOCATIONLABEL)->EnableWindow(m_bBackupOnSave);

	GetDlgItem(IDC_BACKUPCOUNTLABEL)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_NUMBACKUPSTOKEEP)->EnableWindow(m_bBackupOnSave);
	GetDlgItem(IDC_BACKUPCOUNTTRAIL)->EnableWindow(m_bBackupOnSave);
}
