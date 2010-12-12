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
//*****************************************************************************

// TDLPrefMigrationDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLPrefMigrationDlg.h"

#include "../../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLPrefMigrationDlg dialog

CTDLPrefMigrationDlg::CTDLPrefMigrationDlg(CWnd* pParent /*=NULL*/):
CDialog(CTDLPrefMigrationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTDLPrefMigrationDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTDLPrefMigrationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLPrefMigrationDlg)
	DDX_Control(pDX, IDC_ANIMATION, m_animation);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLPrefMigrationDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLPrefMigrationDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLPrefMigrationDlg message handlers

BOOL CTDLPrefMigrationDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_sAnimationFilePath = FileMisc::GetTempFileName(_T("tdl_migrate"), _T("avi"));

	if (FileMisc::ExtractResource(_T("shell32.dll"), 161, _T("AVI"), m_sAnimationFilePath))
	{
		m_animation.Open(m_sAnimationFilePath);
	}

	CenterWindow();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTDLPrefMigrationDlg::Create(CWnd* pParentWnd)
{
	return CDialog::Create(IDD, pParentWnd);
}

void CTDLPrefMigrationDlg::OnDestroy()
{
	CDialog::OnDestroy();

	::DeleteFile(m_sAnimationFilePath);
}
