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

// TDLPrintDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLPrintDialog.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../Common/Preferences.h"
#include "../../../CodeProject/Source/FileMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLPrintDialog dialog


CTDLPrintDialog::CTDLPrintDialog(LPCTSTR szTitle, BOOL bPreview, BOOL bShowSubtaskCheckbox, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PRINT_DIALOG, pParent), 
	m_bPreview(bPreview), 
	m_taskSel(_T("Print"), bShowSubtaskCheckbox),
	m_sTitle(szTitle), 
	m_eStylesheet(FES_COMBOSTYLEBTN, CEnString(IDS_XSLFILEFILTER))
{
	//{{AFX_DATA_INIT(CTDLPrintDialog)
	//}}AFX_DATA_INIT
	CPreferences prefs;

	m_bDate = prefs.GetProfileInt(_T("Print"), _T("WantDate"), TRUE);
	m_sStylesheet = prefs.GetProfileString(_T("Print"), _T("Stylesheet"));

	// check whether user has set stylesheet in prefs
	if (m_sStylesheet.IsEmpty() || prefs.GetProfileInt(_T("Print"), _T("DefaultStylesheet"), FALSE))
	{
		CString sDefStylesheet = prefs.GetProfileString(_T("Preferences"), _T("PrintStylesheet"));

		if (!sDefStylesheet.IsEmpty())
			m_sStylesheet = sDefStylesheet;
	}

	m_bUseStylesheet = (!m_sStylesheet.IsEmpty());
}


void CTDLPrintDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLPrintDialog)
	DDX_Control(pDX, IDC_STYLESHEET, m_eStylesheet);
	DDX_Text(pDX, IDC_STYLESHEET, m_sStylesheet);
	//}}AFX_DATA_MAP
	DDX_Text(pDX, IDC_PRINTTITLE, m_sTitle);
	DDX_Check(pDX, IDC_PRINTDATE, m_bDate);
	DDX_Check(pDX, IDC_USESTYLESHEET, m_bUseStylesheet);
}


BEGIN_MESSAGE_MAP(CTDLPrintDialog, CDialog)
	//{{AFX_MSG_MAP(CTDLPrintDialog)
	ON_EN_CHANGE(IDC_STYLESHEET, OnChangeStylesheet)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_USESTYLESHEET, OnUsestylesheet)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLPrintDialog message handlers

void CTDLPrintDialog::OnOK() 
{
	CDialog::OnOK();
	
	// save settings
	CPreferences prefs;

	prefs.WriteProfileString(_T("Print"), _T("Stylesheet"), m_bUseStylesheet ? m_sStylesheet : _T(""));

	// we store whether this is the same as the default print stylesheet
	// so we can update as it does
	CString sDefStylesheet = prefs.GetProfileString(_T("Preferences"), _T("PrintStylesheet"));
	prefs.WriteProfileInt(_T("Print"), _T("DefaultStylesheet"), (m_sStylesheet.CompareNoCase(sDefStylesheet) == 0));
}


BOOL CTDLPrintDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

    VERIFY(m_taskSel.Create(IDC_FRAME, this));

	GetDlgItem(IDOK)->EnableWindow(!m_bUseStylesheet ||
									GetFileAttributes(m_sStylesheet) != 0xffffffff);

	if (m_bPreview)
		SetWindowText(CEnString(IDS_PRINTDLG_PREVIEW_TITLE));
	else
		SetWindowText(CEnString(IDS_PRINTDLG_PRINT_TITLE));

	GetDlgItem(IDC_STYLESHEET)->EnableWindow(m_bUseStylesheet);

	// init the stylesheet folder to point to the resource folder
	CString sXslFolder = FileMisc::GetModuleFolder() + _T("Resources");
	m_eStylesheet.SetCurrentFolder(sXslFolder);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLPrintDialog::OnUsestylesheet() 
{
	UpdateData();

	GetDlgItem(IDC_STYLESHEET)->EnableWindow(m_bUseStylesheet);
	GetDlgItem(IDOK)->EnableWindow(!m_bUseStylesheet ||
									GetFileAttributes(m_sStylesheet) != 0xffffffff);
}

void CTDLPrintDialog::OnChangeStylesheet() 
{
	UpdateData();
	GetDlgItem(IDOK)->EnableWindow(!m_bUseStylesheet ||
									GetFileAttributes(m_sStylesheet) != 0xffffffff);
}
