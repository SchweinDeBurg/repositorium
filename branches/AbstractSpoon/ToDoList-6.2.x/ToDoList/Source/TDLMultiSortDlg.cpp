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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// TDLMultiSortDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLMultiSortDlg.h"
#include "TDCStatic.h"

#include "../../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLMultiSortDlg dialog

CTDLMultiSortDlg::CTDLMultiSortDlg(const TDSORTCOLUMNS& sort, const CTDCColumnArray& aVisibleColumns, CWnd* pParent /*=NULL*/):
CDialog(CTDLMultiSortDlg::IDD, pParent),
m_nSortBy1(sort.nBy1),
m_nSortBy2(sort.nBy2),
m_nSortBy3(sort.nBy3),
m_bAscending1(sort.bAscending1),
m_bAscending2(sort.bAscending2),
m_bAscending3(sort.bAscending3),
m_aVisibleColumns(aVisibleColumns)
{
	//{{AFX_DATA_INIT(CTDLMultiSortDlg)
	//}}AFX_DATA_INIT
}

void CTDLMultiSortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTDLMultiSortDlg)
	DDX_Control(pDX, IDC_SORTBY3, m_cbSortBy3);
	DDX_Control(pDX, IDC_SORTBY2, m_cbSortBy2);
	DDX_Control(pDX, IDC_SORTBY1, m_cbSortBy1);
	DDX_Check(pDX, IDC_ASCENDING1, m_bAscending1);
	DDX_Check(pDX, IDC_ASCENDING2, m_bAscending2);
	DDX_Check(pDX, IDC_ASCENDING3, m_bAscending3);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTDLMultiSortDlg, CDialog)
	//{{AFX_MSG_MAP(CTDLMultiSortDlg)
	ON_CBN_SELCHANGE(IDC_SORTBY1, OnSelchangeSortby1)
	ON_CBN_SELCHANGE(IDC_SORTBY2, OnSelchangeSortby2)
	ON_CBN_SELCHANGE(IDC_SORTBY3, OnSelchangeSortby3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLMultiSortDlg message handlers

BOOL CTDLMultiSortDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	BuildCombos();

	// disable 3rd combo if 2nd combo not set
	m_cbSortBy3.EnableWindow(m_nSortBy2 != TDC_UNSORTED);

	GetDlgItem(IDC_ASCENDING3)->EnableWindow(m_nSortBy2 != TDC_UNSORTED && m_nSortBy3 != TDC_UNSORTED);
	GetDlgItem(IDC_ASCENDING2)->EnableWindow(m_nSortBy2 != TDC_UNSORTED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CTDLMultiSortDlg::BuildCombos()
{
	for (int nCol = 0; nCol < NUM_COLUMNS; nCol++)
	{
		TDCCOLUMN& col = COLUMNS[nCol];

		if (col.nSortBy != TDC_UNSORTED)
		{
			// is this column visible
			if (IsColumnVisible(col.nColID))
			{
				int nIndex = m_cbSortBy1.AddString(CEnString(col.nIDLongName));
				m_cbSortBy1.SetItemData(nIndex, col.nSortBy);

				if (m_nSortBy1 == col.nSortBy)
				{
					m_cbSortBy1.SetCurSel(nIndex);
				}

				nIndex = m_cbSortBy2.AddString(CEnString(col.nIDLongName));
				m_cbSortBy2.SetItemData(nIndex, col.nSortBy);

				if (m_nSortBy2 == col.nSortBy)
				{
					m_cbSortBy2.SetCurSel(nIndex);
				}

				nIndex = m_cbSortBy3.AddString(CEnString(col.nIDLongName));
				m_cbSortBy3.SetItemData(nIndex, col.nSortBy);

				if (m_nSortBy3 == col.nSortBy)
				{
					m_cbSortBy3.SetCurSel(nIndex);
				}
			}
		}
	}

	// add blank item at top of 2nd and 3rd combo
	int nIndex = m_cbSortBy2.InsertString(0, _T(""));
	m_cbSortBy2.SetItemData(nIndex, TDC_UNSORTED);

	if (m_nSortBy2 == TDC_UNSORTED)
	{
		m_cbSortBy2.SetCurSel(nIndex);
	}

	nIndex = m_cbSortBy3.InsertString(0, _T(""));
	m_cbSortBy3.SetItemData(nIndex, TDC_UNSORTED);

	if (m_nSortBy3 == TDC_UNSORTED)
	{
		m_cbSortBy3.SetCurSel(nIndex);
	}

	// set selection to first item if first combo selection is not set
	if (m_cbSortBy1.GetCurSel() == -1)
	{
		m_cbSortBy1.SetCurSel(0);
		m_nSortBy1 = (TDC_SORTBY)m_cbSortBy1.GetItemData(0);
	}
}

BOOL CTDLMultiSortDlg::IsColumnVisible(TDC_COLUMN col) const
{
	// special case:
	if (col == TDCC_CLIENT)
	{
		return TRUE;
	}

	int nCol = m_aVisibleColumns.GetSize();

	while (nCol--)
	{
		if (col == m_aVisibleColumns[nCol])
		{
			return TRUE;
		}
	}

	// not found
	return FALSE;
}

void CTDLMultiSortDlg::OnSelchangeSortby1()
{
	UpdateData();

	int nSel = m_cbSortBy1.GetCurSel();
	m_nSortBy1 = (TDC_SORTBY)m_cbSortBy1.GetItemData(nSel);
}

void CTDLMultiSortDlg::OnSelchangeSortby2()
{
	UpdateData();

	int nSel = m_cbSortBy2.GetCurSel();
	m_nSortBy2 = (TDC_SORTBY)m_cbSortBy2.GetItemData(nSel);

	GetDlgItem(IDC_ASCENDING2)->EnableWindow(m_nSortBy2 != TDC_UNSORTED);

	// disable 3rd combo if 2nd combo not set
	m_cbSortBy3.EnableWindow(m_nSortBy2 != TDC_UNSORTED);
	GetDlgItem(IDC_ASCENDING3)->EnableWindow(m_nSortBy2 != TDC_UNSORTED && m_nSortBy3 != TDC_UNSORTED);
}

void CTDLMultiSortDlg::OnSelchangeSortby3()
{
	UpdateData();

	int nSel = m_cbSortBy3.GetCurSel();
	m_nSortBy3 = (TDC_SORTBY)m_cbSortBy3.GetItemData(nSel);

	GetDlgItem(IDC_ASCENDING3)->EnableWindow(m_nSortBy2 != TDC_UNSORTED && m_nSortBy3 != TDC_UNSORTED);
}

void CTDLMultiSortDlg::GetSortBy(TDSORTCOLUMNS& sort) const
{
	sort.nBy1 = m_nSortBy1;
	sort.bAscending1 = m_bAscending1;

	sort.nBy2 = m_nSortBy2;

	// if nBy2 is not set then make sure nBy3 isn't set
	if (sort.nBy2 != TDC_UNSORTED)
	{
		sort.bAscending2 = m_bAscending2;
		sort.nBy3 = m_nSortBy3;
		sort.bAscending3 = m_bAscending3;
	}
	else
	{
		sort.nBy3 = TDC_UNSORTED;
	}
}
