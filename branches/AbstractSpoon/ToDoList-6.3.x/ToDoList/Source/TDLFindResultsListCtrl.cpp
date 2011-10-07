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

// TDLFindResultsListCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "Resource.h"
#include "TDLFindResultsListCtrl.h"

#include "../../Common/Preferences.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/GraphicsMisc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTDLFindResultsListCtrl

CTDLFindResultsListCtrl::CTDLFindResultsListCtrl()
{
}

CTDLFindResultsListCtrl::~CTDLFindResultsListCtrl()
{
	m_fontBold.DeleteObject();
	m_fontStrike.DeleteObject();
}

BEGIN_MESSAGE_MAP(CTDLFindResultsListCtrl, CEnListCtrl)
	//{{AFX_MSG_MAP(CTDLFindResultsListCtrl)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTDLFindResultsListCtrl message handlers

void CTDLFindResultsListCtrl::PreSubclassWindow()
{
	CEnListCtrl::PreSubclassWindow();

	// setup up result list
	InsertColumn(0, CEnString(IDS_FT_TASK));
	InsertColumn(1, CEnString(IDS_FT_WHATMATCHED), LVCFMT_LEFT, 250);
	SetExtendedStyle(GetExtendedStyle() | LVS_EX_ONECLICKACTIVATE | LVS_EX_UNDERLINEHOT);
	SetHotCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	SetView(LVS_REPORT);
	EnableHeaderTracking(FALSE);
	SetFirstColumnStretchy(TRUE);
	RefreshUserPreferences();
}

int CTDLFindResultsListCtrl::GetResultCount() const
{
	return GetResultCount(NULL);
}

int CTDLFindResultsListCtrl::GetResultCount(const CFilteredToDoCtrl* pTDC) const
{
	int nCount = 0;
	int nItem = GetItemCount();

	while (nItem--)
	{
		FTDRESULT* pRes = GetResult(nItem);

		if (pRes && (pTDC == NULL || pRes->pTDC == pTDC))
		{
			nCount++;
		}
	}

	return nCount;
}

int CTDLFindResultsListCtrl::GetAllResults(CFTDResultsArray& aResults) const
{
	return GetResults(NULL, aResults);
}

int CTDLFindResultsListCtrl::GetResults(const CFilteredToDoCtrl* pTDC, CFTDResultsArray& aResults) const
{
	int nNumItem = GetItemCount();
	int nCount = 0;

	aResults.RemoveAll();
	aResults.SetSize(GetResultCount(pTDC));

	for (int nItem = 0; nItem < nNumItem; nItem++)
	{
		FTDRESULT* pRes = GetResult(nItem);

		if (pRes && (pTDC == NULL || pRes->pTDC == pTDC))
		{
			aResults.SetAt(nCount, *pRes);
			nCount++;
		}
	}

	return aResults.GetSize();
}

int CTDLFindResultsListCtrl::GetResultIDs(const CFilteredToDoCtrl* pTDC, CDWordArray& aTaskIDs) const
{
	CFTDResultsArray aResults;
	int nNumRes = GetResults(pTDC, aResults);

	for (int nRes = 0; nRes < nNumRes; nRes++)
	{
		aTaskIDs.Add(aResults[nRes].dwTaskID);
	}

	return aResults.GetSize();
}

void CTDLFindResultsListCtrl::DeleteResults(const CFilteredToDoCtrl* pTDC)
{
	// work backwards from the last list res
	int nItem = GetItemCount();

	while (nItem--)
	{
		FTDRESULT* pRes = GetResult(nItem);

		if (pRes && pRes->pTDC == pTDC)
		{
			DeleteItem(nItem);
			delete pRes;
		}
	}
}

void CTDLFindResultsListCtrl::DeleteAllResults()
{
	// work backwards from the last list res
	int nItem = GetItemCount();

	while (nItem--)
	{
		FTDRESULT* pRes = GetResult(nItem);
		delete pRes;

		DeleteItem(nItem);
	}
}

void CTDLFindResultsListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// we only need handle header items
	int nItem = lpDrawItemStruct->itemID;
	FTDRESULT* pRes = GetResult(nItem);

	if (pRes)
	{
		CEnListCtrl::DrawItem(lpDrawItemStruct);
		return;
	}

	// else
	CString sText = GetItemText(nItem, 0);

	if (!sText.IsEmpty())
	{
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

		CRect rLabel, rItem;
		GetItemRect(nItem, rLabel, LVIR_LABEL);
		GetItemRect(nItem, rItem, LVIR_BOUNDS);

		rLabel.left -= 2;

		int nSave = pDC->SaveDC();

		pDC->SelectObject(&m_fontBold);
		pDC->SetTextColor(0);
		pDC->SetBkColor(GetSysColor(COLOR_WINDOW));
		pDC->SetBkMode(OPAQUE);
		pDC->DrawText(sText, rLabel, DT_TOP | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX);

		// draw a horizontal dividing line
		pDC->DrawText(sText, rLabel, DT_TOP | DT_LEFT | DT_END_ELLIPSIS | DT_NOPREFIX | DT_CALCRECT);
		pDC->FillSolidRect(rLabel.right + 6, rLabel.top + rLabel.Height() / 2, rItem.right - rLabel.right, 1,
			GetSysColor(COLOR_3DSHADOW));

		pDC->RestoreDC(nSave);
	}
}

COLORREF CTDLFindResultsListCtrl::GetItemTextColor(int nItem, int nSubItem, BOOL bSelected, BOOL bDropHighlighted,
	BOOL bWndFocus) const
{
	if (!bSelected && !bDropHighlighted)
	{
		FTDRESULT* pRes = (FTDRESULT*)GetItemData(nItem);

		if (pRes && pRes->bDone && nSubItem == 0 && m_crDone != (COLORREF)-1)
		{
			return m_crDone;
		}
	}

	// else
	return CEnListCtrl::GetItemTextColor(nItem, nSubItem, bSelected, bDropHighlighted, bWndFocus);
}

CFont* CTDLFindResultsListCtrl::GetItemFont(int nItem, int nSubItem)
{
	FTDRESULT* pRes = GetResult(nItem);

	if (pRes && pRes->bDone && nSubItem == 0 && m_fontStrike.GetSafeHandle())
	{
		return &m_fontStrike;
	}

	// else
	return CEnListCtrl::GetItemFont(nItem, nSubItem);
}

void CTDLFindResultsListCtrl::RefreshUserPreferences()
{
	CPreferences prefs;

	// update user completed tasks colour
	if (prefs.GetProfileInt(_T("Preferences"), _T("SpecifyDoneColor"), FALSE))
	{
		m_crDone = (COLORREF)prefs.GetProfileInt(_T("Preferences\\Colors"), _T("TaskDone"), -1);
	}
	else
	{
		m_crDone = (COLORREF)-1;
	}

	// update strike thru font
	if (prefs.GetProfileInt(_T("Preferences"), _T("StrikethroughDone"), FALSE))
	{
		if (!m_fontStrike.GetSafeHandle())
		{
			GraphicsMisc::CreateFont(m_fontStrike, (HFONT)SendMessage(WM_GETFONT), MFS_STRIKETHRU);
		}
	}
	else
	{
		m_fontStrike.DeleteObject();
	}

	if (IsWindowVisible())
	{
		Invalidate();
	}
}

int CTDLFindResultsListCtrl::AddResult(const SEARCHRESULT& result, LPCTSTR szTask, LPCTSTR/*szPath*/,
	const CFilteredToDoCtrl* pTDC)
{
	int nPos = GetItemCount();

	// add result
	int nIndex = InsertItem(nPos, szTask);
	SetItemText(nIndex, 1, Misc::FormatArray(result.aMatched));
	UpdateWindow();

	// map identifying data
	FTDRESULT* pRes = new FTDRESULT(result.dwID, pTDC, result.HasFlag(RF_DONE));
	SetItemData(nIndex, (DWORD)pRes);

	return nIndex;
}

BOOL CTDLFindResultsListCtrl::AddHeaderRow(LPCTSTR szText, BOOL bSpaceAbove)
{
	int nPos = GetItemCount();

	// add space above?
	if (bSpaceAbove)
	{
		int nIndex = InsertItem(nPos, _T(""));
		SetItemData(nIndex, 0);
		nPos++;
	}

	// add result
	int nIndex = InsertItem(nPos, szText);
	SetItemData(nIndex, 0);

	// bold font for rendering
	if (m_fontBold.GetSafeHandle() == NULL)
	{
		GraphicsMisc::CreateFont(m_fontBold, (HFONT)SendMessage(WM_GETFONT), MFS_BOLD);
	}

	return (nIndex != -1);
}
