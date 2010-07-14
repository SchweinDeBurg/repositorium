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

// CheckListBoxEx.cpp : implementation file
//

#include "stdafx.h"
#include "CheckListBoxEx.h"
#include "Themed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCheckListBoxEx

CCheckListBoxEx::CCheckListBoxEx() : m_nImageHeight(0)
{
}

CCheckListBoxEx::~CCheckListBoxEx()
{
}


BEGIN_MESSAGE_MAP(CCheckListBoxEx, CCheckListBox)
	//{{AFX_MSG_MAP(CCheckListBoxEx)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCheckListBoxEx message handlers

BOOL CCheckListBoxEx::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult) 
{
	if (m_ilCheck.GetSafeHandle())
	{
		switch (message)
		{
		case WM_DRAWITEM:
			PreDrawItem((LPDRAWITEMSTRUCT)lParam);
			return TRUE;

		case WM_MEASUREITEM:
			PreMeasureItem((LPMEASUREITEMSTRUCT)lParam);
			return TRUE;
		}
	}
	
	// else default
	return CCheckListBox::OnChildNotify(message, wParam, lParam, pLResult);
}

void CCheckListBoxEx::OnDestroy()
{
	m_ilCheck.DeleteImageList();
}

void CCheckListBoxEx::PreSubclassWindow()
{
	CCheckListBox::PreSubclassWindow();

	const int nStates[] = { CBS_UNCHECKEDNORMAL, CBS_CHECKEDNORMAL };//, CBS_MIXEDNORMAL };
	const int nNumStates = sizeof(nStates) / sizeof(int);
	
	CThemed th;
	
	if (th.Open(this, _T("BUTTON")) && th.AreControlsThemed())
	{
		th.BuildImageList(m_ilCheck, BP_CHECKBOX, nStates, nNumStates);

		IMAGEINFO ii;
		m_ilCheck.GetImageInfo(0, &ii);
		m_nImageHeight = (ii.rcImage.bottom - ii.rcImage.top);
	}
}

void CCheckListBoxEx::PreDrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (!m_ilCheck.GetSafeHandle())
	{
		CCheckListBox::PreDrawItem(lpDrawItemStruct);
		return;
	}

	DRAWITEMSTRUCT drawItem;
	memcpy(&drawItem, lpDrawItemStruct, sizeof(DRAWITEMSTRUCT));

	if ((((LONG)drawItem.itemID) >= 0) &&
	   ((drawItem.itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0))
	{
		CDC* pDC = CDC::FromHandle(drawItem.hDC);

		COLORREF newBkColor = GetSysColor(COLOR_WINDOW);

		BOOL fDisabled = !IsWindowEnabled() || !IsEnabled(drawItem.itemID);
		if ((drawItem.itemState & ODS_SELECTED) && !fDisabled)
			newBkColor = GetSysColor(COLOR_HIGHLIGHT);

		COLORREF oldBkColor = pDC->SetBkColor(newBkColor);

		int nCheck = GetCheck(drawItem.itemID);
		m_ilCheck.Draw(pDC, nCheck, CPoint(drawItem.rcItem.left, drawItem.rcItem.top), ILD_TRANSPARENT);

		pDC->SetBkColor(oldBkColor);
	}

	drawItem.rcItem.left = drawItem.rcItem.left + m_nImageHeight + 2;

	DrawItem(&drawItem);
}

void CCheckListBoxEx::PreMeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	CCheckListBox::PreMeasureItem(lpMeasureItemStruct);

	if (m_ilCheck.GetSafeHandle())
		lpMeasureItemStruct->itemHeight = max(lpMeasureItemStruct->itemHeight, m_nImageHeight + 2);
}

LRESULT CCheckListBoxEx::OnSetFont(WPARAM , LPARAM)
{
	Default();

	// copied from CCheckListBox::OnSetFont
	if ((GetStyle() & (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS))
		== (LBS_OWNERDRAWFIXED | LBS_HASSTRINGS))
	{
		int nMinHeight = CalcMinimumItemHeight();
		SetItemHeight(0, max(nMinHeight, (int)m_nImageHeight));
	}

	return 0;
}
