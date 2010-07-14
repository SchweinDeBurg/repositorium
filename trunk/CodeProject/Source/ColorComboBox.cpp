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

// ColorComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "ColorComboBox.h"
#include "DlgUnits.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox

CColorComboBox::CColorComboBox() : m_bResized(FALSE)
{
}

CColorComboBox::~CColorComboBox()
{
}


BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	//{{AFX_MSG_MAP(CColorComboBox)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorComboBox message handlers

void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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
	if (0 <= (int)lpDrawItemStruct->itemID)	// Any item selected?
	{
		CDlgUnits dlu(GetParent());

		BOOL bHasColor = (lpDrawItemStruct->itemData != (DWORD)-1);

		if (!bHasColor)
			rItem.DeflateRect(0, 1); 
		else if (rItem.Height() <= dlu.ToPixelsY(9))
			rItem.DeflateRect(1, 1); // selected item
		else
			rItem.DeflateRect(2, 2); // listbox item

		CRect rColor(rItem);
		
		// draw text
		if (GetStyle() & CBS_HASSTRINGS)
		{
			CRect rText(rItem);
			
			rColor.right = rColor.left + (bHasColor ? rColor.Height() : 0);
			rText.left = rColor.right + 2;
			
			CString sText;
			GetLBText(lpDrawItemStruct->itemID, sText);

			if (!sText.IsEmpty())
				dc.TextOut(rText.left, rText.top, sText);
		}

		// draw color
		if (bHasColor)
		{
			if (!IsWindowEnabled() || (lpDrawItemStruct->itemState & ODS_GRAYED))
				dc.FillSolidRect(rColor, GetSysColor(COLOR_3DFACE));
			else
				dc.FillSolidRect(rColor, (COLORREF)lpDrawItemStruct->itemData);
		}
	}

	// Restore the DC state
	dc.RestoreDC(nDC);
	dc.Detach();
}

int CColorComboBox::AddColor(COLORREF color, LPCTSTR szDescription)
{
	ASSERT (GetStyle() & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE));

	if (szDescription)
		ASSERT (GetStyle() & CBS_HASSTRINGS);
	else
		szDescription = _T("");

	int nIndex = AddString(szDescription);

	if (nIndex != CB_ERR)
		SetItemData(nIndex, (DWORD)color);

	return nIndex;
}

int CColorComboBox::InsertColor(int nIndex, COLORREF color, LPCTSTR szDescription)
{
	ASSERT (GetStyle() & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE));

	if (szDescription)
		ASSERT (GetStyle() & CBS_HASSTRINGS);
	else
		szDescription = _T("");

	nIndex = InsertString(nIndex, szDescription);

	if (nIndex != CB_ERR)
		SetItemData(nIndex, (DWORD)color);

	return nIndex;
}

void CColorComboBox::PreSubclassWindow() 
{
	if (!m_bResized)
	{
		m_bResized = TRUE;
		
		CDlgUnits dlu(GetParent());
		SetItemHeight(-1, dlu.ToPixelsY(9)); 
	}
	
	CComboBox::PreSubclassWindow();
}

int CColorComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
