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
// - taken out from the original ToDoList package for better sharing
//*****************************************************************************

// MenuEx.cpp: implementation of the CMenuEx class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuEx.h"
#include "Themed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#ifndef HBMMENU_MBAR_CLOSE
#	define HBMMENU_MBAR_RESTORE        ((HBITMAP)  2)
#	define HBMMENU_MBAR_MINIMIZE       ((HBITMAP)  3)
#	define HBMMENU_MBAR_CLOSE          ((HBITMAP)  5)
#	define HBMMENU_MBAR_CLOSE_D        ((HBITMAP)  6)
#	define HBMMENU_MBAR_MINIMIZE_D     ((HBITMAP)  7)
#endif

#ifndef ODS_HOTLIGHT
#	define ODS_HOTLIGHT        0x0040
#	define ODS_INACTIVE        0x0080
#endif

#define BTNBORDER 0
#define FUDGE 8

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuEx::CMenuEx()
{

}

CMenuEx::~CMenuEx()
{
}

BOOL CMenuEx::AddMDIButton(MENUEX_BTN nBtn, UINT nCmdID, BOOL bRightJustify)
{
	ASSERT (GetSafeHmenu());

	if (!GetSafeHmenu())
		return FALSE;

	HBITMAP hbm = NULL;

	if (!IsThemed())
	{
		switch (nBtn)
		{
		case MEB_MINIMIZE:
			hbm = HBMMENU_MBAR_MINIMIZE;
			break;
			
		case MEB_RESTORE:
			hbm = HBMMENU_MBAR_RESTORE;
			break;
			
		case MEB_CLOSE:
			hbm = HBMMENU_MBAR_CLOSE;
			break;
			
		default:
			return FALSE;
		}
	}
	
	UINT nFlags = (IsThemed() ? MFT_OWNERDRAW : MFT_BITMAP) | 
					(bRightJustify ? MFT_RIGHTJUSTIFY : 0);
		
	if (InsertMenu((UINT)-1, nFlags, nCmdID, CBitmap::FromHandle(hbm)))
	{
		m_mapCmd2ID[nCmdID] = nBtn;
		return TRUE;
	}

	// else
	return FALSE;
}

BOOL CMenuEx::DeleteMDIMenu(UINT nCmdID)
{
	// CMenu::DeleteMenu won't work on bitmap buttons directly
	// so we must traverse all menu items looking for nCmdID
	int nItem = GetMenuItemCount();

	while (nItem--)
	{
		UINT nMenuCmdID = GetMenuItemID(nItem);

		if (nCmdID == nMenuCmdID)
		{
			DeleteMenu(nItem, MF_BYPOSITION);
			m_mapCmd2ID.RemoveKey(nCmdID);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMenuEx::IsThemed()
{
	return CThemed().IsNonClientThemed();
}

BOOL CMenuEx::DrawMDIButton(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (!IsThemed())
		return FALSE;

	// draw the button
	CRect rect(lpDrawItemStruct->rcItem);
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
//	UINT nState = lpDrawItemStruct->itemState;

	int nBtn = -1;
	m_mapCmd2ID.Lookup(lpDrawItemStruct->itemID, nBtn);

	CThemed th;
		
	if (!th.Open(AfxGetMainWnd(), _T("WINDOW")))
		return FALSE;

	int nThPart = 0, nThState = 0;

	switch (nBtn)
	{
	case MEB_MINIMIZE:
		nThPart = WP_MDIMINBUTTON;
		break;

	case MEB_RESTORE:
		nThPart = WP_MDIRESTOREBUTTON;
		break;

	case MEB_CLOSE:
		nThPart = WP_MDICLOSEBUTTON;
/*
		if (nState & ODS_HOTLIGHT)
		{
			nThPart = WP_CLOSEBUTTON;
			nThState = CBS_HOT;
		}
		else if (nState & ODS_SELECTED)
		{
			nThPart = WP_CLOSEBUTTON;
			nThState = CBS_PUSHED;
		}
		else
			nThPart = WP_MDICLOSEBUTTON;
*/
		break;
	}
		
	th.DrawBackground(pDC, nThPart, nThState, rect, NULL);
//	th.DrawParentBackground(pWnd, pDC, (LPRECT)(pClip ? pClip : pRect));

	return TRUE;
}

BOOL CMenuEx::MeasureMDIButton(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = 4;
	lpMeasureItemStruct->itemWidth = 4;

	return TRUE;
}

void CMenuEx::SetBackgroundColor(COLORREF color)
{
	ASSERT(GetSafeHmenu());

	// menu background color
	m_brBkgnd.DeleteObject();
	m_brBkgnd.CreateSolidBrush(color);
	
	MENUINFO MenuInfo = {0};
	MenuInfo.cbSize = sizeof(MenuInfo);
	MenuInfo.hbrBack = m_brBkgnd; 
	MenuInfo.fMask = MIM_BACKGROUND;
	MenuInfo.dwStyle = MNS_AUTODISMISS;
	
	::SetMenuInfo(GetSafeHmenu(), &MenuInfo);
}
