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

// TDLFilterOptionComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "TDLFilterOptionComboBox.h"
#include "TDCStatic.h"

#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFilterOptionComboBox

CTDLFilterOptionComboBox::CTDLFilterOptionComboBox() : CEnCheckComboBox(TRUE, 0, IDS_TDC_NONE), m_dwOptions((DWORD) - 1)
{
}

CTDLFilterOptionComboBox::~CTDLFilterOptionComboBox()
{
}

BEGIN_MESSAGE_MAP(CTDLFilterOptionComboBox, CEnCheckComboBox)
	//{{AFX_MSG_MAP(CFilterOptionComboBox)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterOptionComboBox message handlers

void CTDLFilterOptionComboBox::Initialize(DWORD dwFlags, DWORD dwOptions)
{
	ResetContent();
	m_dwOptions = (DWORD) - 1;

	CString sFlag;

	for (int nItem = 0; nItem < NUM_FILTEROPT; nItem++)
	{
		UINT nFlag = FILTER_OPTIONS[nItem][1];

		if (Misc::HasFlag(dwFlags, nFlag))
		{
			sFlag.LoadString(FILTER_OPTIONS[nItem][0]);

			int nIndex = AddString(sFlag);

			if (nIndex != CB_ERR)
			{
				m_mapItemFlag[sFlag] = nFlag; // map item to enum

				// is the item checked?
				SetCheck(nIndex, Misc::HasFlag(dwOptions, nFlag));
			}
		}
	}
}

void CTDLFilterOptionComboBox::Initialize(const FTDCFILTER& filter, FTC_VIEW /*nView*/)
{
	ResetContent();
	m_dwOptions = (DWORD) - 1;

	// add flags to droplist depending on the filter being used
	CString sFlag;

	for (int nItem = 0; nItem < NUM_FILTEROPT; nItem++)
	{
		UINT nFlag = FILTER_OPTIONS[nItem][1];
		BOOL bAddFlag = FALSE;

		switch (nFlag)
		{
		case FT_ANYCATEGORY:
		case FT_ANYALLOCTO:
			bAddFlag = TRUE;
			break;

		case FT_HIDEPARENTS:
		case FT_HIDECOLLAPSED:
			bAddFlag = TRUE;//(nView == FTCV_TASKLIST);
			break;

		case FT_HIDEOVERDUE:
			bAddFlag = (filter.nFilter == FT_DUETODAY) ||
				(filter.nFilter == FT_DUETOMORROW) ||
				(filter.nFilter == FT_DUEENDTHISWEEK) ||
				(filter.nFilter == FT_DUEENDNEXTWEEK) ||
				(filter.nFilter == FT_DUEENDTHISMONTH) ||
				(filter.nFilter == FT_DUEENDNEXTMONTH) ||
				(filter.nFilter == FT_DUEENDTHISYEAR) ||
				(filter.nFilter == FT_DUEENDNEXTYEAR);
			break;

		case FT_HIDEDONE:
			bAddFlag = (filter.nFilter != FT_NOTDONE && filter.nFilter != FT_DONE);
			break;

		default:
			ASSERT(0);
			break;
		}

		if (bAddFlag)
		{
			sFlag.LoadString(FILTER_OPTIONS[nItem][0]);

			if (AddString(sFlag) != CB_ERR)
			{
				m_mapItemFlag[sFlag] = nFlag;   // map item to enum
			}
		}
	}

	// set states
	SetOptions(filter.dwFlags);
}

void CTDLFilterOptionComboBox::OnCheckChange(int /*nIndex*/)
{
	// update m_dwOptions
	m_dwOptions = 0;

	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		if (GetCheck(nItem))
		{
			// get flag for item
			UINT nFlag = 0;
			CString sFlag;

			GetLBText(nItem, sFlag);
			VERIFY(m_mapItemFlag.Lookup(sFlag, nFlag));

			// aggregate
			m_dwOptions |= nFlag;
		}
	}
}

void CTDLFilterOptionComboBox::SetOptions(DWORD dwOptions)
{
	ASSERT(GetSafeHwnd());

	if (dwOptions == m_dwOptions)
	{
		return;
	}

	for (int nItem = 0; nItem < GetCount(); nItem++)
	{
		// get flag for item
		UINT nFlag = 0;
		CString sFlag;

		GetLBText(nItem, sFlag);
		VERIFY(m_mapItemFlag.Lookup(sFlag, nFlag));

		// set state
		SetCheck(nItem, Misc::HasFlag(dwOptions, nFlag));
	}

	m_dwOptions = dwOptions;
}

DWORD CTDLFilterOptionComboBox::GetOptions() const
{
	return m_dwOptions;
}
