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

// BrowserDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "BrowserDlg.h"

#include <afxdisp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBrowserDlg dialog


CBrowserDlg::CBrowserDlg(BOOL bBrowser) : CRuntimeDlg(), m_bBrowser(bBrowser)
{
	//{{AFX_DATA_INIT(CBrowserDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	AddControl("static", "", WS_CHILD, 0, 0, 0, 0, 0, (UINT)-1);
}


void CBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBrowserDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBrowserDlg, CDialog)
	//{{AFX_MSG_MAP(CBrowserDlg)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_EN_SETFOCUS(1, OnEditSetFocus)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBrowserDlg message handlers

int CBrowserDlg::DoModal(LPCTSTR szCaption, LPCTSTR szUrlPath, CWnd* pParentWnd)
{
	if (!szUrlPath || !*szUrlPath)
		return IDCANCEL;

	m_sUrl = szUrlPath;

	return CRuntimeDlg::DoModal(szCaption, (DWORD)RTD_DEFSTYLE | WS_THICKFRAME, (DWORD)RTD_DEFEXSTYLE,
		rectAuto, pParentWnd);
}

BOOL CBrowserDlg::Create(LPCTSTR szCaption, LPCTSTR szUrlPath, CWnd* pParentWnd)
{
	if (!szUrlPath || !*szUrlPath)
		return IDCANCEL;

	m_sUrl = szUrlPath;

	return CRuntimeDlg::Create(szCaption, (DWORD)RTD_DEFSTYLE | WS_THICKFRAME, (DWORD)RTD_DEFEXSTYLE,
		rectAuto, pParentWnd);
}

BOOL CBrowserDlg::Navigate(LPCTSTR szUrlPath)
{
	COleVariant vFlags(0L), vFrame(""), vData(""), vHeaders("");
	m_browser.Navigate(szUrlPath, vFlags, vFrame, vData, vHeaders);

	return TRUE;
}

BOOL CBrowserDlg::OnInitDialog()
{
	CRuntimeDlg::OnInitDialog();

	if (m_bBrowser)
	{
		AfxEnableControlContainer();

		if (m_browser.Create("", WS_CHILD | WS_TABSTOP | WS_VISIBLE, CRect(0, 0, 400, 400),
			this, 1))
		{
			COleVariant vFlags(0L), vFrame(""), vData(""), vHeaders("");
			m_browser.Navigate(m_sUrl, vFlags, vFrame, vData, vHeaders);
		}
	}
	else if (m_edit.Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_READONLY | ES_MULTILINE | WS_VSCROLL | ES_NOHIDESEL,
		CRect(0, 0, 400, 400), this, 1))
	{
		CStdioFile file;

		if (file.Open(m_sUrl, CFile::modeRead | CFile::typeText))
		{
			CString sLine, sFile;

			while (file.ReadString(sLine))
			{
				sFile += sLine;
				sFile += '\n';
			}

			// replace carriage returns by linefeed/cr because that's
			// what edit control prefer
			sFile.Replace("\n", "\r\n");

			m_edit.SetWindowText(sFile);
		}

		if (GetParent())
			m_edit.SetFont(GetParent()->GetFont());
	}

	AutoFit();
	CenterWindow();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CBrowserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (m_browser.GetSafeHwnd())
		m_browser.MoveWindow(0, 0, cx, cy);

	else if (m_edit.GetSafeHwnd())
		m_edit.MoveWindow(0, 0, cx, cy);
}

void CBrowserDlg::OnEditSetFocus()
{
	if (!m_bBrowser)
		m_edit.SetSel(-1, -1);
}
