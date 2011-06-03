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
//*****************************************************************************

// ProgressDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

enum
{
	IDC_PROGRESSBAR = 101,
	IDC_DESCRIPTION,
	IDC_PROGRESS,
};


CProgressDlg::CProgressDlg(BOOL bCancelButton, CWnd* /*pParent*/ /*=NULL*/) : m_bContinue(FALSE)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	m_sProgress = _T("");
	m_sDescription = _T("");
	//}}AFX_DATA_INIT

	if (bCancelButton)
	{
		AddRCControl(_T("PUSHBUTTON"), _T(""), _T("Cancel"), 0, 0, 67, 73, 50, 14, IDCANCEL);
	}

	AddRCControl(_T("CONTROL"), _T("msctls_progress32"), _T(""), 0, 0, 7, 55, 172, 9, IDC_PROGRESSBAR);
	AddRCControl(_T("LTEXT"), _T(""), _T(""), 0, 0, 7, 7, 172, 30, IDC_DESCRIPTION);
	AddRCControl(_T("LTEXT"), _T(""), _T(""), 0, 0, 7, 42, 172, 8, IDC_PROGRESS);
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CRuntimeDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESSBAR, m_progress);
	DDX_Text(pDX, IDC_PROGRESS, m_sProgress);
	DDX_Text(pDX, IDC_DESCRIPTION, m_sDescription);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CRuntimeDlg)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

BOOL CProgressDlg::Create(LPCTSTR szCaption, LPCTSTR szDescription, CWnd* pParent)
{
	m_sDescription = szDescription;
	m_sProgress.Empty();

	return CRuntimeDlg::Create(szCaption, RTD_DEFSTYLE, RTD_DEFEXSTYLE, rectAuto, pParent);
}

void CProgressDlg::SetCaption(LPCTSTR szCaption)
{
	ASSERT(GetSafeHwnd());

	SetWindowText(szCaption);

	Continue();
}

void CProgressDlg::SetProgress(LPCTSTR szProgress)
{
	ASSERT(GetSafeHwnd());

	m_sProgress = szProgress;
	UpdateData(FALSE);

	if (GetDlgItem(IDCANCEL)->GetSafeHwnd())
	{
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	}

	Continue();
}

void CProgressDlg::SetProgress(int nProgress)
{
	ASSERT(GetSafeHwnd());

	m_progress.SetPos(nProgress);

	if (GetDlgItem(IDCANCEL)->GetSafeHwnd())
	{
		GetDlgItem(IDCANCEL)->EnableWindow(TRUE);
	}

	Continue();
}

void CProgressDlg::SetDescription(LPCTSTR szDescription)
{
	ASSERT(GetSafeHwnd());

	m_sDescription = szDescription;
	UpdateData(FALSE);

	Continue();
}

BOOL CProgressDlg::OnInitDialog()
{
	CRuntimeDlg::OnInitDialog();

	m_bContinue = TRUE;
	m_progress.SetRange(0, 100);

	if (GetDlgItem(IDCANCEL)->GetSafeHwnd())
	{
		GetDlgItem(IDCANCEL)->EnableWindow(FALSE);   // till SetProgress is called for the first time
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CProgressDlg::Continue()
{
	UpdateWindow();

	MSG msg;

	// check messages for the cancel dialog
	while (::PeekMessage((LPMSG) &msg, NULL, 0, 0, PM_REMOVE))
	{
		BOOL bDlgMsg = ::IsDialogMessage(*this, &msg);

		if (!bDlgMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return m_bContinue;
}

void CProgressDlg::OnCancel()
{
	m_bContinue = FALSE;

	ShowWindow(SW_HIDE);
}

void CProgressDlg::OnDestroy()
{
	CRuntimeDlg::OnDestroy();
}
