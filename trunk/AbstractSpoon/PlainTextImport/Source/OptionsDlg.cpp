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
//*****************************************************************************

// OptionsDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "PlainTextImportApp.h"
#include "OptionsDlg.h"

#include "../../../CodeProject/Source/EnString.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg dialog


COptionsDlg::COptionsDlg(BOOL bImport, CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OPTIONSDIALOG, pParent)
{
	//{{AFX_DATA_INIT(COptionsDlg)

	//}}AFX_DATA_INIT
	m_sLabel.LoadString(bImport ? IDS_IMPORTLABEL : IDS_EXPORTLABEL);
	m_sTitle.LoadString(bImport ? IDS_IMPORTTITLE : IDS_EXPORTTITLE);
	m_sProjectLabel.LoadString(bImport ? IDS_IMPORTPROJECTLABEL : IDS_EXPORTPROJECTLABEL);

	m_bIncludeProject = AfxGetApp()->GetProfileInt(_T("Importers\\PlainText"), _T("IncludeProject"), FALSE);

	BOOL bSpaceIndent = AfxGetApp()->GetProfileInt(_T("Importers\\PlainText"), _T("UseSpaceIndents"), -1);
	int nSpaces = 2;

	if (bSpaceIndent == -1) // first time
	{
		bSpaceIndent = AfxGetApp()->GetProfileInt(_T("Preferences"), _T("UseSpaceIndents"), TRUE);
		nSpaces = AfxGetApp()->GetProfileInt(_T("Preferences"), _T("TextIndent"), 2);
	}
	else
	{
		nSpaces = AfxGetApp()->GetProfileInt(_T("Importers\\PlainText"), _T("TextIndent"), 2);
	}

	if (bSpaceIndent)
	{
		m_nIndent = (int)((log((double)nSpaces) + 0.001) / log((double)2)) + 1;
	}
	else // tabs
	{
		m_nIndent = 0;
	}
}


void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDlg)
	DDX_Control(pDX, IDC_TABWIDTHS, m_cbIndent);
	DDX_Text(pDX, IDC_CBLABEL, m_sLabel);
	DDX_CBIndex(pDX, IDC_TABWIDTHS, m_nIndent);
	DDX_Check(pDX, IDC_PROJECTINCLUDED, m_bIncludeProject);
	//}}AFX_DATA_MAP

	if (pDX->m_bSaveAndValidate)
	{
		if (m_nIndent == 0)
		{
			m_sIndent = _T("\t");
		}
		else
		{
			CString sSpaces;
			m_cbIndent.GetLBText(m_nIndent, sSpaces);

			m_sIndent = CString(_T(' '), _ttoi(sSpaces));
		}
	}
}


BEGIN_MESSAGE_MAP(COptionsDlg, CDialog)
	//{{AFX_MSG_MAP(COptionsDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDlg message handlers

BOOL COptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText(m_sTitle);
	GetDlgItem(IDC_PROJECTINCLUDED)->SetWindowText(m_sProjectLabel);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void COptionsDlg::OnOK()
{
	CDialog::OnOK();

	// save state
	AfxGetApp()->WriteProfileInt(_T("Importers\\PlainText"), _T("UseSpaceIndents"), m_nIndent != 0);
	AfxGetApp()->WriteProfileInt(_T("Importers\\PlainText"), _T("TextIndent"), m_nIndent == 0 ? 0 : m_sIndent.GetLength());
	AfxGetApp()->WriteProfileInt(_T("Importers\\PlainText"), _T("IncludeProject"), m_bIncludeProject);
}
