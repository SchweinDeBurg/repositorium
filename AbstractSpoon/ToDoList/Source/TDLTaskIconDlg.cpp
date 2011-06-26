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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// TDLTaskIconDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLTaskIconDlg.h"

#include "../../Common/Preferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLTaskIconDlg dialog

CTDLTaskIconDlg::CTDLTaskIconDlg(const CImageList& ilIcons, int nSelIndex, CWnd* pParent /*=NULL*/):
CDialog(CTDLTaskIconDlg::IDD, pParent),
m_ilIcons(ilIcons),
m_nIconIndex(nSelIndex)
{
	//{{AFX_DATA_INIT(CTDLTaskIconDlg)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

void CTDLTaskIconDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLTaskIconDlg)
	DDX_Control(pDX, IDC_ICONLIST, m_lcIcons);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLTaskIconDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLTaskIconDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_ICONLIST, OnDblclkIconlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ICONLIST, OnItemchangedIconlist)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_ICONLIST, OnEndlabeleditIconlist)
	ON_WM_DESTROY()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLTaskIconDlg message handlers

BOOL CTDLTaskIconDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ListView_SetImageList(m_lcIcons, m_ilIcons, LVSIL_SMALL);
	CPreferences prefs;

	for (int nImage = 0; nImage < m_ilIcons.GetImageCount(); nImage++)
	{
		CString sImage, sKey;

		sKey.Format(_T("Icon%d"), nImage + 1);
		sImage = prefs.GetProfileString(_T("TaskIcons"), sKey);

		if (sImage.IsEmpty())
		{
			sImage.Format(_T("%d"), nImage + 1);
		}

		m_lcIcons.InsertItem(nImage, sImage, nImage);
	}

	if (m_nIconIndex >= 0)
	{
		m_lcIcons.SetItemState(m_nIconIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}

	// disable OK button if nothing selected
	GetDlgItem(IDOK)->EnableWindow(m_nIconIndex >= 0);
	m_lcIcons.SetFocus();

	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLTaskIconDlg::OnDblclkIconlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMITEMACTIVATE* pNMListView = (NMITEMACTIVATE*)pNMHDR;

	m_nIconIndex = pNMListView->iItem;

	// disable OK button if nothing selected
	GetDlgItem(IDOK)->EnableWindow(m_nIconIndex >= 0);

	if (m_nIconIndex >= 0)
	{
		EndDialog(IDOK);
	}

	*pResult = 0;
}

void CTDLTaskIconDlg::OnItemchangedIconlist(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	m_nIconIndex = pNMListView->iItem;

	// disable OK button if nothing selected
	GetDlgItem(IDOK)->EnableWindow(m_nIconIndex >= 0);

	*pResult = 0;
}

void CTDLTaskIconDlg::OnEndlabeleditIconlist(NMHDR* /*pNMHDR*/, LRESULT* pResult)
{
	*pResult = 1; // save changes
}

void CTDLTaskIconDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// save off any saved names to the preferences
	CPreferences prefs;

	for (int nImage = 0; nImage < m_ilIcons.GetImageCount(); nImage++)
	{
		CString sImage, sKey, sText;

		// only save the ones thsat have been modified
		sImage.Format(_T("%d"), nImage + 1);
		sText = m_lcIcons.GetItemText(nImage, 0);

		if (sText != sImage)
		{
			sKey.Format(_T("Icon%d"), nImage + 1);
			prefs.WriteProfileString(_T("TaskIcons"), sKey, sText);
		}
	}

}

HBRUSH CTDLTaskIconDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	if (pWnd->GetDlgCtrlID() == IDC_EDITTASKTEXT_LABEL)
	{
		pDC->SetTextColor(GetSysColor(COLOR_3DDKSHADOW));
	}

	return hbr;
}
