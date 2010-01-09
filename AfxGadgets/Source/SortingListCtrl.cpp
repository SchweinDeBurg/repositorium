// AfxGadgets library.
// Copyright (c) 2004-2010 by Elijah Zarezky,
// All rights reserved.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// SortingListCtrl.cpp - implementation of the CSortingListCtrl class

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "SortingListCtrl.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// avoid unwanted ICL warnings

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
#endif	// __INTEL_COMPILER

//////////////////////////////////////////////////////////////////////////////////////////////
// debugging support

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////
// object model

IMPLEMENT_DYNAMIC(CSortingListCtrl, CListCtrl)

//////////////////////////////////////////////////////////////////////////////////////////////
// message map

BEGIN_MESSAGE_MAP(CSortingListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
END_MESSAGE_MAP()

CSortingListCtrl::CSortingListCtrl(void):
m_iSortColumn(-1),
m_nSortOrder(0)
{
}

CSortingListCtrl::~CSortingListCtrl(void)
{
}

void CSortingListCtrl::SortItems(int iColumn, SORT_ORDER eOrder)
{
	m_iSortColumn = iColumn;
	m_nSortOrder = eOrder;
	ASSERT(sizeof(this) <= sizeof(DWORD));
	ListView_SortItemsEx(GetSafeHwnd(), CompareProc, reinterpret_cast<LPARAM>(this));
}

void CSortingListCtrl::OnColumnClick(NMHDR* pHdr, LRESULT* /*pnResult*/)
{
	int iColumn = reinterpret_cast<NMLISTVIEW*>(pHdr)->iSubItem;
	if (m_iSortColumn != iColumn)
	{
		// sort by another column in ascending order
		m_iSortColumn = iColumn;
		m_nSortOrder = SORT_ASCENDING;
	}
	else {
		// reverse sort order for the current column
		m_nSortOrder *= -1;
	}
	ASSERT(sizeof(this) <= sizeof(DWORD));
	ListView_SortItemsEx(GetSafeHwnd(), CompareProc, reinterpret_cast<LPARAM>(this));
}

int CALLBACK CSortingListCtrl::CompareProc(LPARAM iItemLhs, LPARAM iItemRhs, LPARAM nData)
{
	CSortingListCtrl* pListCtrl = reinterpret_cast<CSortingListCtrl*>(nData);
	ASSERT_VALID(pListCtrl);
	return (pListCtrl->CompareItems(iItemLhs, iItemRhs));
}

#if defined(_DEBUG)

void CSortingListCtrl::AssertValid(void) const
{
	// first perform inherited validity check...
	CListCtrl::AssertValid();

	// ...and then verify our own state as well
}

void CSortingListCtrl::Dump(CDumpContext& dumpCtx) const
{
	try
	{
		// first invoke inherited dumper...
		CListCtrl::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_iSortColumn = " << m_iSortColumn << "\n";
		dumpCtx << "m_nSortOrder = " << m_nSortOrder;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
