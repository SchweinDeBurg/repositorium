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

// TDLTransformDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLTransformDialog.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLTransformDialog dialog

CTDLTransformDialog::CTDLTransformDialog(LPCTSTR szTitle, BOOL bShowSubtaskCheckbox, CWnd* pParent /*=NULL*/):
CDialog(IDD_TRANSFORM_DIALOG, pParent),
m_taskSel(_T("Transform"), bShowSubtaskCheckbox),
m_sTitle(szTitle),
m_eStylesheet(FES_COMBOSTYLEBTN | FES_RELATIVEPATHS, CEnString(IDS_XSLFILEFILTER))
{
	//{{AFX_DATA_INIT(CTDLTransformDialog)
	//}}AFX_DATA_INIT
	// see what we had last time
	CPreferences prefs;

	m_sStylesheet = prefs.GetProfileString(_T("Transform"), _T("Stylesheet"));
	m_sStylesheet = FileMisc::GetRelativePath(m_sStylesheet, FileMisc::GetAppResourceFolder(), FALSE);

	m_bDate = prefs.GetProfileInt(_T("Transform"), _T("WantDate"), TRUE);
}

void CTDLTransformDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLTransformDialog)
	DDX_Control(pDX, IDC_STYLESHEET, m_eStylesheet);
	DDX_Text(pDX, IDC_STYLESHEET, m_sStylesheet);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_TRANSFORMTITLE, m_sTitle);
	DDX_Check(pDX, IDC_TRANSFORMDATE, m_bDate);
}

BEGIN_MESSAGE_MAP(CTDLTransformDialog, CDialog)
	//{{AFX_MSG_MAP(CTDLTransformDialog)
	ON_EN_CHANGE(IDC_STYLESHEET, OnChangeStylesheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLTransformDialog message handlers

void CTDLTransformDialog::OnOK()
{
	CDialog::OnOK();

	CPreferences prefs;

	prefs.WriteProfileString(_T("Transform"), _T("Stylesheet"), m_sStylesheet);
	prefs.WriteProfileInt(_T("Transform"), _T("WantDate"), m_bDate);
}

BOOL CTDLTransformDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	VERIFY(m_taskSel.Create(IDC_FRAME, this));

	BOOL bEnable = FileMisc::FileExists(GetStylesheet());
	GetDlgItem(IDOK)->EnableWindow(bEnable);

	// init the stylesheet folder to point to the resource folder
	m_eStylesheet.SetCurrentFolder(FileMisc::GetAppResourceFolder());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLTransformDialog::OnChangeStylesheet()
{
	UpdateData();

	BOOL bEnable = FileMisc::FileExists(GetStylesheet());
	GetDlgItem(IDOK)->EnableWindow(bEnable);
}

CString CTDLTransformDialog::GetStylesheet() const
{
	return FileMisc::GetFullPath(m_sStylesheet, FileMisc::GetAppResourceFolder());
}
