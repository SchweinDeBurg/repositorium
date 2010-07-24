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
//*****************************************************************************

// TabbedComboBox.cpp : implementation file
//

#include "StdAfx.h"
#include "TabbedComboBox.h"
#include "../../CodeProject/Source/DlgUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabbedComboBox

CTabbedComboBox::CTabbedComboBox(int nTabStops) : TABSTOPS(nTabStops), m_bResized(FALSE)
{
}

CTabbedComboBox::~CTabbedComboBox()
{
}


BEGIN_MESSAGE_MAP(CTabbedComboBox, CComboBox)
	//{{AFX_MSG_MAP(CTabbedComboBox)
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabbedComboBox message handlers

BOOL CTabbedComboBox::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= CBS_OWNERDRAWFIXED | CBS_HASSTRINGS;

	return CComboBox::PreCreateWindow(cs);
}

void CTabbedComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;

	if (!dc.Attach(lpDrawItemStruct->hDC))
		return;

	int nDC = dc.SaveDC();

	BOOL bSelected = (lpDrawItemStruct->itemState & ODS_SELECTED);
	COLORREF crBack = GetSysColor(IsWindowEnabled() ? (bSelected ? COLOR_HIGHLIGHT : COLOR_WINDOW) : COLOR_3DFACE);
	COLORREF crText = GetSysColor(IsWindowEnabled() ? (bSelected ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT) : COLOR_GRAYTEXT);

	CRect rItem(lpDrawItemStruct->rcItem);

	dc.FillSolidRect(rItem, crBack);
	dc.SetTextColor(crText);

	if (lpDrawItemStruct->itemAction & ODA_FOCUS)
	{
		dc.DrawFocusRect(rItem);
	}
	else if (lpDrawItemStruct->itemAction & ODA_DRAWENTIRE)
	{
		if (lpDrawItemStruct->itemState & ODS_FOCUS)
			dc.DrawFocusRect(rItem);
	}

	// draw the item
	if (GetStyle() & CBS_HASSTRINGS)
	{
		if ((int)lpDrawItemStruct->itemID >= 0)	// Any item selected?
		{
			CDlgUnits dlu(GetParent());

			if (rItem.Height() <= dlu.ToPixelsY(9))
				rItem.DeflateRect(1, 1); // selected item
			else
				rItem.DeflateRect(2, 2); // listbox item

			CRect rColor(rItem);

			// draw text
			CRect rText(rItem);

			CString sText;
			GetLBText(lpDrawItemStruct->itemID, sText);

			// replace [\][t] with [\t]
			sText.Replace(_T("\\t"), _T("\t"));

			if (!sText.IsEmpty())
				dc.TabbedTextOut(rText.left, rText.top, sText, 1, (int*)&TABSTOPS, rText.left);
		}
	}

	// Restore the DC state
	dc.RestoreDC(nDC);
	dc.Detach();
}

void CTabbedComboBox::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos)
{
	CComboBox::OnWindowPosChanged(lpwndpos);
}

int CTabbedComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_bResized)
	{
		m_bResized = TRUE;

		CDlgUnits dlu(GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9));
	}

	return 0;
}

void CTabbedComboBox::PreSubclassWindow()
{
	if (!m_bResized)
	{
		m_bResized = TRUE;

		CDlgUnits dlu(GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9));
	}

	CComboBox::PreSubclassWindow();
}
