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

// TDLImportDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLImportDialog.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLImportDialog dialog


CTDLImportDialog::CTDLImportDialog(const CImportExportMgr& mgr, CWnd* pParent /*=NULL*/)
	: CDialog(CTDLImportDialog::IDD, pParent),
	  m_mgrImportExport(mgr)
{
	//{{AFX_DATA_INIT(CTDLImportDialog)
	//}}AFX_DATA_INIT
	CPreferences prefs;

	m_bFromClipboard = prefs.GetProfileInt(_T("Importing"), _T("ImportOption"), FALSE);
	m_sFromFilePath = prefs.GetProfileString(_T("Importing"), _T("ImportFilePath"));
	m_nImportTo = prefs.GetProfileInt(_T("Importing"), _T("ImportToWhere"), TDIT_SELECTEDTASK);
	m_nFormatOption = prefs.GetProfileInt(_T("Importing"), _T("ImportFormat"), 0);

	m_nFormatOption = min(m_nFormatOption, mgr.GetNumImporters());
}


void CTDLImportDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLImportDialog)
	DDX_Control(pDX, IDC_FROMFILEPATH, m_eFilePath);
	DDX_Control(pDX, IDC_FORMATOPTIONS, m_cbFormat);
	DDX_Radio(pDX, IDC_FROMFILE, m_bFromClipboard);
	DDX_Text(pDX, IDC_FROMFILEPATH, m_sFromFilePath);
	DDX_Radio(pDX, IDC_TONEWTASKLIST, m_nImportTo);
	DDX_CBIndex(pDX, IDC_FORMATOPTIONS, m_nFormatOption);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTDLImportDialog, CDialog)
	//{{AFX_MSG_MAP(CTDLImportDialog)
	ON_BN_CLICKED(IDC_FROMCLIPBOARD, OnChangeImportFrom)
	ON_CBN_SELCHANGE(IDC_FORMATOPTIONS, OnSelchangeFormatoptions)
	ON_BN_CLICKED(IDC_FROMFILE, OnChangeImportFrom)
	ON_EN_CHANGE(IDC_FROMCLIPBOARDTEXT, OnChangeClipboardtext)
	ON_EN_CHANGE(IDC_FROMFILEPATH, OnChangeFilepath)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLImportDialog message handlers

void CTDLImportDialog::OnChangeImportFrom() 
{
	UpdateData();

	BOOL bHasFilter = CurImporterHasFilter();
	
	GetDlgItem(IDC_FROMFILEPATH)->EnableWindow(!m_bFromClipboard && bHasFilter);
	GetDlgItem(IDC_FROMCLIPBOARDTEXT)->EnableWindow(m_bFromClipboard && bHasFilter);

	EnableOK();
}

BOOL CTDLImportDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// build the format comboxbox
	for (int nImp = 0; nImp < m_mgrImportExport.GetNumImporters(); nImp++)
		m_cbFormat.AddString(m_mgrImportExport.GetImporterMenuText(nImp));
	
	// append standard tasklist to the end
	m_cbFormat.AddString(CEnString(AFX_IDS_APP_TITLE));
	m_cbFormat.SetCurSel(m_nFormatOption);
	
	// init file edit
	BOOL bHasFilter = CurImporterHasFilter();

	m_eFilePath.SetFilter(GetCurImporterFilter());
	m_eFilePath.EnableWindow(bHasFilter);
	
	GetDlgItem(IDC_FROMFILEPATH)->EnableWindow(!m_bFromClipboard && bHasFilter);
	GetDlgItem(IDC_FROMCLIPBOARDTEXT)->EnableWindow(m_bFromClipboard && bHasFilter);
	GetDlgItem(IDC_FROMFILE)->EnableWindow(bHasFilter);
	GetDlgItem(IDC_FROMCLIPBOARD)->EnableWindow(bHasFilter);
	
	m_sClipboardText = Misc::GetClipboardText(*this);

	if (bHasFilter)
		GetDlgItem(IDC_FROMCLIPBOARDTEXT)->SetWindowText(m_sClipboardText);

	EnableOK();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTDLImportDialog::CurImporterHasFilter() const
{
	return (ImportTasklist() || m_mgrImportExport.ImporterHasFileExtension(m_nFormatOption));
}

CString CTDLImportDialog::GetCurImporterFilter() const
{
	if (ImportTasklist())
		return CEnString(IDS_TDLFILEFILTER);
	
	// else
	return m_mgrImportExport.GetImporterFileFilter(m_nFormatOption);
}

void CTDLImportDialog::OnOK()
{
	CDialog::OnOK();
	
	CPreferences prefs;
	
	prefs.WriteProfileInt(_T("Importing"), _T("ImportOption"), m_bFromClipboard);
	prefs.WriteProfileString(_T("Importing"), _T("ImportFilePath"), m_sFromFilePath);
	prefs.WriteProfileInt(_T("Importing"), _T("ImportToWhere"), m_nImportTo);
	prefs.WriteProfileInt(_T("Importing"), _T("ImportFormat"), m_nFormatOption);

	// retrieve clipboard text
	if (CurImporterHasFilter())
		GetDlgItem(IDC_FROMCLIPBOARDTEXT)->GetWindowText(m_sClipboardText);
}

BOOL CTDLImportDialog::ImportTasklist() const
{
	return (m_nFormatOption == m_mgrImportExport.GetNumImporters());
}

int CTDLImportDialog::GetImporterIndex() const
{
	if (ImportTasklist())
		return -1;
	
	// else
	return m_nFormatOption;
}

TDLID_IMPORTTO CTDLImportDialog::GetImportTo() const
{
	return (TDLID_IMPORTTO)m_nImportTo;
}

BOOL CTDLImportDialog::GetImportFromClipboard() const
{
	return (m_bFromClipboard);
}

CString CTDLImportDialog::GetImportFilePath() const
{
	return (m_bFromClipboard || !CurImporterHasFilter()) ? _T("") : m_sFromFilePath;
}

CString CTDLImportDialog::GetImportClipboardText() const
{
	return (m_bFromClipboard && CurImporterHasFilter()) ? m_sClipboardText : _T("");
}

void CTDLImportDialog::OnSelchangeFormatoptions() 
{
	BOOL bHadFilter = m_mgrImportExport.ImporterHasFileExtension(m_nFormatOption);

	UpdateData(TRUE);
	
	// change the filter on the CFileEdit and clear the filepath
	// and clear/restore clipboard text depending
	BOOL bHasFilter = CurImporterHasFilter();

	m_eFilePath.SetFilter(GetCurImporterFilter());
	m_eFilePath.EnableWindow(bHasFilter);
	
	GetDlgItem(IDC_FROMFILE)->EnableWindow(bHasFilter);
	GetDlgItem(IDC_FROMCLIPBOARD)->EnableWindow(bHasFilter);
	GetDlgItem(IDC_FROMFILEPATH)->EnableWindow(!m_bFromClipboard && bHasFilter);
	GetDlgItem(IDC_FROMCLIPBOARDTEXT)->EnableWindow(m_bFromClipboard && bHasFilter);

	if (bHadFilter && !bHasFilter)
	{
		GetDlgItem(IDC_FROMCLIPBOARDTEXT)->GetWindowText(m_sClipboardText); // update
		GetDlgItem(IDC_FROMCLIPBOARDTEXT)->SetWindowText(_T("")); // clear field
	}
	else if (!bHadFilter && bHasFilter)
	{
		GetDlgItem(IDC_FROMCLIPBOARDTEXT)->SetWindowText(m_sClipboardText); // restore field
	}
	
	m_sFromFilePath.Empty();
	UpdateData(FALSE);

	EnableOK();
}

void CTDLImportDialog::EnableOK()
{
	if (!CurImporterHasFilter())
		GetDlgItem(IDOK)->EnableWindow(TRUE);

	else if (GetImportFromClipboard())
	{
		GetDlgItem(IDOK)->EnableWindow(!m_sClipboardText.IsEmpty());
	}
	else // import from file
	{
		m_sFromFilePath.TrimLeft();
		m_sFromFilePath.TrimRight();

		GetDlgItem(IDOK)->EnableWindow(!m_sFromFilePath.IsEmpty());
	}
}


void CTDLImportDialog::OnChangeClipboardtext() 
{
	GetDlgItem(IDC_FROMCLIPBOARDTEXT)->GetWindowText(m_sClipboardText); // update
	EnableOK();
}

void CTDLImportDialog::OnChangeFilepath() 
{
	UpdateData();
	EnableOK();
}
