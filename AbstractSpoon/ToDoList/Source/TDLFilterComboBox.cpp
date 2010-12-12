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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// TDLFilterComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "TDLFilterComboBox.h"
#include "TDCStatic.h"

#include "../../../CodeProject/Source/EnString.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLFilterComboBox

CTDLFilterComboBox::CTDLFilterComboBox()
{
}

CTDLFilterComboBox::~CTDLFilterComboBox()
{
}

BEGIN_MESSAGE_MAP(CTDLFilterComboBox, CTabbedComboBox)
	//{{AFX_MSG_MAP(CTDLFilterComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLFilterComboBox message handlers

void CTDLFilterComboBox::PreSubclassWindow()
{
	CTabbedComboBox::PreSubclassWindow();

	FillCombo();
}

int CTDLFilterComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CTabbedComboBox::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	FillCombo();

	return 0;
}

void CTDLFilterComboBox::FillCombo()
{
	ASSERT(GetSafeHwnd());

	if (GetCount())
	{
		return;   // already called
	}

	for (int nItem = 0; nItem < NUM_TASKFILTER; nItem++)
	{
		CEnString sFilter(TASK_FILTERS[nItem][0]);
		UINT nFilter = TASK_FILTERS[nItem][1];

		CEnString sText(_T("%c)\t%s"), _T('A') + nItem, sFilter);
		int nIndex = AddString(sText);
		ASSERT(nIndex != CB_ERR);

		if (nIndex != CB_ERR)
		{
			SetItemData(nIndex, nFilter);
		}
	}
}

void CTDLFilterComboBox::RemoveCustomFilter()
{
	int nFind = GetCount();

	// find an existing entry
	while (nFind--)
	{
		if (GetItemData(nFind) == (DWORD)FT_CUSTOM)
		{
			break;
		}
	}

	if (nFind != -1)
	{
		// save selection so we can restore it
		int nSel = GetCurSel();

		DeleteString(nFind);

		if (nFind < nSel)
		{
			nSel--;
		}

		SetCurSel(nSel);
	}
}

void CTDLFilterComboBox::SetCustomFilter(LPCTSTR szFilter)
{
	int nFind = GetCount();

	// find an existing entry
	while (nFind--)
	{
		if (GetItemData(nFind) == (DWORD)FT_CUSTOM)
		{
			break;
		}
	}

	CEnString sCustom(IDS_CUSTOMFILTER), sFilter(IDS_CUSTOMFILTER);

	if (szFilter && *szFilter)
	{
		sFilter.Format(_T("%s (%s)"), sCustom, szFilter);
	}

	// delete existing entry if the text has changed
	if (nFind != -1)
	{
		CString sPrevFilter;
		GetLBText(nFind, sPrevFilter);

		if (sFilter != sPrevFilter)
		{
			DeleteString(nFind);
		}
		else
		{
			return;   // nothing to do
		}
	}

	nFind = AddString(sFilter);
	SetItemData(nFind, (DWORD)FT_CUSTOM);
	SetCurSel(nFind);
}

FILTER_TYPE CTDLFilterComboBox::GetSelectedFilter() const
{
	int nSel = GetCurSel();
	ASSERT(nSel != CB_ERR);

	if (nSel == CB_ERR)
	{
		return (FILTER_TYPE) - 1;
	}

	// else
	return (FILTER_TYPE)GetItemData(nSel);
}

BOOL CTDLFilterComboBox::SetSelectedFilter(FILTER_TYPE nFilter)
{
	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		FILTER_TYPE nItemFilter = (FILTER_TYPE)GetItemData(nItem);

		if (nItemFilter == nFilter)
		{
			SetCurSel(nItem);
			return TRUE;
		}
	}

	return FALSE;
}
