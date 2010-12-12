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

// TDLColumnListBox.cpp : implementation file
//

#include "StdAfx.h"
#include "Resource.h"
#include "TDLColumnListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_INITLISTBOX (WM_APP+1)

/////////////////////////////////////////////////////////////////////////////
// CTDLColumnListBox

CTDLColumnListBox::CTDLColumnListBox()
{
	// same order as enum
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_POS,         TDCC_POSITION,  TRUE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_ID,          TDCC_ID,        FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_PRIORITY,    TDCC_PRIORITY,  TRUE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_PERCENT,     TDCC_PERCENT,   TRUE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_TIMEEST,     TDCC_TIMEEST,   FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_TIMESPENT,   TDCC_TIMESPENT, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_TRACKTIME,   TDCC_TRACKTIME, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_CREATEDBY,   TDCC_CREATEDBY, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_CREATEDATE,  TDCC_CREATIONDATE, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_STARTDATE,   TDCC_STARTDATE, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DUEDATE,     TDCC_DUEDATE,   TRUE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DONEDATE,    TDCC_DONEDATE,  FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_ALLOCTO,     TDCC_ALLOCTO,   TRUE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_ALLOCBY,     TDCC_ALLOCBY,   FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_STATUS,      TDCC_STATUS,    FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_CATEGORY,    TDCC_CATEGORY,  FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_FILEREF,     TDCC_FILEREF,   FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_FLAG,        TDCC_FLAG,      FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DONE,        TDCC_DONE,      FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_MODIFYDATE,  TDCC_LASTMOD,   FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_RISK,        TDCC_RISK,      FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_EXTERNALID,  TDCC_EXTERNALID, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_COST,        TDCC_COST,      FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_DEPENDS,     TDCC_DEPENDENCY, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_RECURRENCE,  TDCC_RECURRENCE, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_VERSION,     TDCC_VERSION,   FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_RECENTEDIT,  TDCC_RECENTEDIT, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_REMAINING,   TDCC_REMAINING, FALSE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_ICON,        TDCC_ICON,      TRUE));
	m_aColumns.Add(COLUMNVIS(IDS_TDLBC_REMINDER,    TDCC_REMINDER,  FALSE));
}

CTDLColumnListBox::~CTDLColumnListBox()
{
}

BEGIN_MESSAGE_MAP(CTDLColumnListBox, CCheckListBoxEx)
	//{{AFX_MSG_MAP(CTDLColumnListBox)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_CONTROL_REFLECT_EX(CLBN_CHKCHANGE, OnReflectCheckChange)
	ON_MESSAGE(WM_INITLISTBOX, OnInitListBox)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLColumnListBox message handlers

TDC_COLUMN CTDLColumnListBox::MapOldColumnIndex(int nOld)
{
	enum TDLB_COLUMN
	{
		TDLBC_PRIORITY,
		TDLBC_PERCENT,
		TDLBC_TIMEEST,
		TDLBC_STARTDATE,
		TDLBC_DUEDATE,
		TDLBC_DONEDATE,
		TDLBC_ALLOCTO,
		TDLBC_FILEREF,
		TDLBC_POSITION,
		TDLBC_ID,
		TDLBC_DONE,
		TDLBC_ALLOCBY,
		TDLBC_STATUS,
		TDLBC_CATEGORY,
		TDLBC_TIMESPENT,
		TDLBC_TRACKTIME,
		TDLBC_FLAG,
		TDLBC_CREATIONDATE,
		TDLBC_CREATEDBY,
		TDLBC_MODIFYDATE,
		TDLBC_RISK,
		TDLBC_EXTERNALID,
		TDLBC_COST,
		TDLBC_DEPENDS,
		TDLBC_RECURRENCE,
		TDLBC_VERSION,
	};

	switch (nOld)
	{
	case TDLBC_PRIORITY:
		return TDCC_PRIORITY;
	case TDLBC_PERCENT:
		return TDCC_PERCENT;
	case TDLBC_TIMEEST:
		return TDCC_TIMEEST;
	case TDLBC_STARTDATE:
		return TDCC_STARTDATE;
	case TDLBC_DUEDATE:
		return TDCC_DUEDATE;
	case TDLBC_DONEDATE:
		return TDCC_DONEDATE;
	case TDLBC_ALLOCTO:
		return TDCC_ALLOCTO;
	case TDLBC_FILEREF:
		return TDCC_FILEREF;
	case TDLBC_POSITION:
		return TDCC_POSITION;
	case TDLBC_ID:
		return TDCC_ID;
	case TDLBC_DONE:
		return TDCC_DONE;
	case TDLBC_ALLOCBY:
		return TDCC_ALLOCBY;
	case TDLBC_STATUS:
		return TDCC_STATUS;
	case TDLBC_CATEGORY:
		return TDCC_CATEGORY;
	case TDLBC_TIMESPENT:
		return TDCC_TIMESPENT;
	case TDLBC_TRACKTIME:
		return TDCC_TRACKTIME;
	case TDLBC_FLAG:
		return TDCC_FLAG;
	case TDLBC_CREATIONDATE:
		return TDCC_CREATIONDATE;
	case TDLBC_CREATEDBY:
		return TDCC_CREATEDBY;
	case TDLBC_MODIFYDATE:
		return TDCC_LASTMOD;
	case TDLBC_RISK:
		return TDCC_RISK;
	case TDLBC_EXTERNALID:
		return TDCC_EXTERNALID;
	case TDLBC_COST:
		return TDCC_COST;
	case TDLBC_DEPENDS:
		return TDCC_DEPENDENCY;
	case TDLBC_RECURRENCE:
		return TDCC_RECURRENCE;
	case TDLBC_VERSION:
		return TDCC_VERSION;
	}

	ASSERT(0);
	return (TDC_COLUMN) - 1;
}

void CTDLColumnListBox::PreSubclassWindow()
{
	CCheckListBoxEx::PreSubclassWindow();

	// too early to initialize the list box so we post a message
	PostMessage(WM_INITLISTBOX);
}

int CTDLColumnListBox::FindColumn(TDC_COLUMN nTDCCol) const
{
	int nIndex = m_aColumns.GetSize();

	while (nIndex--)
	{
		if (m_aColumns[nIndex].nTDCCol == nTDCCol)
		{
			return nIndex;
		}
	}

	ASSERT(0);
	return -1;
}

void CTDLColumnListBox::SetAllColumnsVisible(BOOL bVisible)
{
	int nIndex = m_aColumns.GetSize();

	while (nIndex--)
	{
		m_aColumns[nIndex].bVisible = bVisible;

		if (GetSafeHwnd())
		{
			SetCheck(nIndex, bVisible ? 1 : 0);
		}
	}
}

int CTDLColumnListBox::GetAllColumns(CTDCColumnArray& aColumns) const
{
	int nIndex = m_aColumns.GetSize();

	while (nIndex--)
	{
		TDC_COLUMN col = m_aColumns[nIndex].nTDCCol;
		aColumns.Add(col);
	}

	return aColumns.GetSize();
}

BOOL CTDLColumnListBox::OnReflectCheckChange()
{
	// update all check states because we don't know which one changed
	int nIndex = m_aColumns.GetSize();

	while (nIndex--)
	{
		TDC_COLUMN col = (TDC_COLUMN)GetItemData(nIndex);
		int nCol = FindColumn(col);

		if (nCol != -1)
		{
			m_aColumns[nCol].bVisible = GetCheck(nIndex);
		}
	}

	// continue routing
	return FALSE;
}

LRESULT CTDLColumnListBox::OnInitListBox(WPARAM /*wp*/, LPARAM /*lp*/)
{
	ASSERT(GetCount() == 0);

	for (int nIndex = 0; nIndex < (int)m_aColumns.GetSize(); nIndex++)
	{
		const COLUMNVIS& cs = m_aColumns[nIndex];

		int nPos = AddString(cs.sName); // same order as enum
		SetCheck(nPos, cs.bVisible ? 1 : 0);
		SetItemData(nPos, (DWORD)cs.nTDCCol);
	}

	return 0L;
}

void CTDLColumnListBox::SetColumnVisible(TDC_COLUMN nTDCCol, BOOL bVisible)
{
	int nCol = FindColumn(nTDCCol);

	if (nCol != -1)
	{
		m_aColumns[nCol].bVisible = bVisible;

		if (GetSafeHwnd())
		{
			// flip the check box
			int nIndex = m_aColumns.GetSize();

			while (nIndex--)
			{
				if ((DWORD)nTDCCol == GetItemData(nIndex))
				{
					SetCheck(nIndex, bVisible ? 1 : 0);
					break;
				}
			}
		}
	}
}

void CTDLColumnListBox::SetVisibleColumns(const CTDCColumnArray& aColumns)
{
	SetAllColumnsVisible(FALSE);

	int nCol = (int)aColumns.GetSize();

	while (nCol--)
	{
		SetColumnVisible(aColumns[nCol]);
	}
}

int CTDLColumnListBox::GetVisibleColumns(CTDCColumnArray& aColumns) const
{
	aColumns.RemoveAll();

	int nIndex = m_aColumns.GetSize();

	while (nIndex--)
	{
		COLUMNVIS cs = m_aColumns[nIndex];

		if (cs.bVisible)
		{
			aColumns.Add(cs.nTDCCol);
		}
	}

	return aColumns.GetSize();
}
