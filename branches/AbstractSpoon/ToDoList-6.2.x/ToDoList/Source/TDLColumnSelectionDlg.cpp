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

// TDLColumnSelectionDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLColumnSelectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLColumnSelectionDlg dialog

CTDLColumnSelectionDlg::CTDLColumnSelectionDlg(const CTDCColumnArray& aColumns, BOOL bActiveTasklist, CWnd* pParent /*=NULL*/):
CDialog(CTDLColumnSelectionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTDLColumnSelectionDlg)
	m_bActiveTasklist = bActiveTasklist;
	//}}AFX_DATA_INIT

	m_aVisibleColumns.Copy(aColumns);
}

void CTDLColumnSelectionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLColumnSelectionDlg)
	DDX_Control(pDX, IDC_COLUMNS, m_lbColumns);
	DDX_Radio(pDX, IDC_ALLTASKLISTS, m_bActiveTasklist);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLColumnSelectionDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLColumnSelectionDlg)
	ON_BN_CLICKED(IDC_SELECTALLCOLS, OnSelectallcols)
	ON_BN_CLICKED(IDC_CLEARALLCOLS, OnClearallcols)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLColumnSelectionDlg message handlers

int CTDLColumnSelectionDlg::GetVisibleColumns(CTDCColumnArray& aColumns) const
{
	aColumns.Copy(m_aVisibleColumns);

	return aColumns.GetSize();
}

void CTDLColumnSelectionDlg::OnOK()
{
	CDialog::OnOK();

	m_lbColumns.GetVisibleColumns(m_aVisibleColumns);
}

BOOL CTDLColumnSelectionDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_lbColumns.SetVisibleColumns(m_aVisibleColumns);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLColumnSelectionDlg::OnSelectallcols()
{
	m_lbColumns.SetAllColumnsVisible(TRUE);
}

void CTDLColumnSelectionDlg::OnClearallcols()
{
	m_lbColumns.SetAllColumnsVisible(FALSE);
}
