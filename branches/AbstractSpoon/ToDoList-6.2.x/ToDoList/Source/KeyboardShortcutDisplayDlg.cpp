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

// KeyboardShortcutDisplayDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "KeyboardShortcutDisplayDlg.h"

#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKeyboardShortcutDisplayDlg dialog


CKeyboardShortcutDisplayDlg::CKeyboardShortcutDisplayDlg(const CStringArray& aMapping, char cDelim, CWnd* pParent /*=NULL*/):
CDialog(CKeyboardShortcutDisplayDlg::IDD, pParent), m_aMapping(aMapping), m_cDelim(cDelim)

{
	//{{AFX_DATA_INIT(CKeyboardShortcutDisplayDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CKeyboardShortcutDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKeyboardShortcutDisplayDlg)
	DDX_Control(pDX, IDC_SHORTCUTS, m_lcShortcuts);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKeyboardShortcutDisplayDlg, CDialog)
	//{{AFX_MSG_MAP(CKeyboardShortcutDisplayDlg)
	ON_BN_CLICKED(IDC_COPYSHORTCUTS, OnCopyshortcuts)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyboardShortcutDisplayDlg message handlers

BOOL CKeyboardShortcutDisplayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_aMapping.GetSize() == 0)
	{
		EndDialog(IDCANCEL);
		return FALSE;
	}

	CRect rList;
	m_lcShortcuts.GetClientRect(rList);

	m_lcShortcuts.InsertColumn(0, _T(""), LVCFMT_LEFT, 130);
	m_lcShortcuts.InsertColumn(1, _T(""), LVCFMT_LEFT, rList.Width() - 130 - GetSystemMetrics(SM_CXVSCROLL));

	for (int nItem = 0; nItem < m_aMapping.GetSize(); nItem++)
	{
		const CString& sItem = m_aMapping[nItem];
		int nDelim = sItem.Find(m_cDelim);

		if (nDelim != -1)
		{
			int nIndex = m_lcShortcuts.InsertItem(nItem, sItem.Left(nDelim));
			m_lcShortcuts.SetItemText(nIndex, 1, sItem.Mid(nDelim + 1));
		}
		else
		{
			m_lcShortcuts.InsertItem(nItem, _T(""));
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CKeyboardShortcutDisplayDlg::OnCopyshortcuts()
{
	CString sText, sLine;

	for (int nItem = 0; nItem < m_aMapping.GetSize(); nItem++)
	{
		const CString& sItem = m_aMapping[nItem];
		int nDelim = sItem.Find(m_cDelim);

		if (nDelim != -1)
		{
			sLine.Format(_T("%s\t\t%s\n"), sItem.Left(nDelim), sItem.Mid(nDelim + 1));
			sText += sLine;
		}
		else
		{
			sText += _T("\n");
		}
	}

	Misc::CopyTexttoClipboard(sText, *this);
}
