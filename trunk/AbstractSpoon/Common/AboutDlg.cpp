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
//*****************************************************************************

// AboutDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "AboutDlg.h"
#include "StringRes.h"
#include "../../CodeProject/Source/EnString.h"
#include "../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog

enum // ctrl IDs
{
	IDC_APPICON = 100,
	IDC_APPNAME, // 101
	IDC_DIVIDER, // 102
	IDC_APPDESCRIPTION, // 103
	IDC_COPYRIGHT, // 104
	IDC_LICENSE // 105
};

CAboutDlg::CAboutDlg(UINT nAppIconID, AB_STYLE nStyle,
	LPCTSTR szAppName, LPCTSTR szAppDescription, LPCTSTR szCopyright, LPCTSTR szLicense,
	int nAppLines, int nDescLines, int nCopyrightLines, int nLicenseLines):
CRuntimeDlg(),
m_sAppName(szAppName),
m_sAppDescription(szAppDescription),
m_sCopyright(szCopyright),
m_sLicense(szLicense),
m_nStyle(nStyle)
{
	SetBordersDLU(5);

	// icon
	AddRCControl(_T("ICON"), _T(""), _T(""), SS_ICON, 0, 5, 5, 20, 20, IDC_APPICON);

	// variable height items
	const UINT ITEMIDS[] = { IDC_APPNAME, IDC_APPDESCRIPTION, IDC_COPYRIGHT, IDC_LICENSE };
	const UINT NUMLINES[] =  { max(nAppLines, 0), max(nDescLines, 0), max(nCopyrightLines, 0), max(nLicenseLines, 0) };
	const UINT NUMITEMS = sizeof(ITEMIDS) / sizeof(UINT);

	int nTop = 7;

	for (int nItem = 0; nItem < NUMITEMS; nItem++)
	{
		int nHeight = (NUMLINES[nItem] * 9); // 9 dlu / line

		// special case: copyright
		if (nItem == 2 && m_nStyle == ABS_EDITCOPYRIGHT)
		{
			AddRCControl(_T("CONTROL"), _T("RICHEDIT"), _T(""), ES_READONLY | ES_MULTILINE | WS_VSCROLL, WS_EX_STATICEDGE, 36, nTop, 174, nHeight, ITEMIDS[nItem]);
		}
		else
		{
			AddRCControl(_T("LTEXT"), _T(""), _T(""), SS_NOPREFIX, 0, 36, nTop, 174, nHeight, ITEMIDS[nItem]);
		}

		nTop += nHeight;

		if (nHeight && nItem < (NUMITEMS - 1))
		{
			nTop += 6;
		}
	}

	// divider and ok button
	AddRCControl(_T("CONTROL"), _T("static"), _T(""), SS_ETCHEDHORZ, 0, 7, nTop + 4, 203, 1, IDC_DIVIDER);
	AddRCControl(_T("DEFPUSHBUTTON"), _T(""), BTN_OK, WS_GROUP | BS_DEFPUSHBUTTON, 0, 83, nTop + 10, 50, 14, IDOK);

	m_hIcon = AfxGetApp()->LoadIcon(nAppIconID);
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_APPICON, m_stIcon);
	DDX_Text(pDX, IDC_APPNAME, m_sAppName);
	DDX_Text(pDX, IDC_APPDESCRIPTION, m_sAppDescription);
	DDX_Text(pDX, IDC_COPYRIGHT, m_sCopyright);
	DDX_Text(pDX, IDC_LICENSE, m_sLicense);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_APPNAME, m_stAppName);
	DDX_Control(pDX, IDC_APPDESCRIPTION, m_stAppDescription);
	DDX_Control(pDX, IDC_LICENSE, m_stLicense);

	if (m_nStyle == ABS_HTMLCOPYRIGHT)
	{
		DDX_Control(pDX, IDC_COPYRIGHT, m_stCopyright);
	}
}

BEGIN_MESSAGE_MAP(CAboutDlg, CRuntimeDlg)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg message handlers

int CAboutDlg::DoModal()
{
	return CRuntimeDlg::DoModal(ABOUT_TITLE, WS_VISIBLE | WS_POPUP | WS_BORDER | WS_CAPTION | DS_CENTER);
}

BOOL CAboutDlg::OnInitDialog()
{
	CRuntimeDlg::OnInitDialog();

	if (m_hIcon)
	{
		((CStatic*)GetDlgItem(IDC_APPICON))->SetIcon(m_hIcon);
	}

	CWnd* pCopyright = GetDlgItem(IDC_COPYRIGHT);

	if (pCopyright)
	{
		pCopyright->ModifyStyle(WS_BORDER, 0);
		pCopyright->ModifyStyleEx(WS_EX_CLIENTEDGE, 0);
		pCopyright->SetWindowPos(NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
	}

	m_stAppName.SetBkColor(GetSysColor(COLOR_3DFACE));
	m_stAppDescription.SetBkColor(GetSysColor(COLOR_3DFACE));
	m_stCopyright.SetBkColor(GetSysColor(COLOR_3DFACE));
	m_stLicense.SetBkColor(GetSysColor(COLOR_3DFACE));

	GetDlgItem(IDOK)->SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
