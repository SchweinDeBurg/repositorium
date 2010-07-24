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

// MenuIconMgr.cpp: implementation of the CMenuIconMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MenuIconMgr.h"
#include "EnBitmapEx.h"
#include "Themed.h"

typedef unsigned long ULONG_PTR; 

#ifndef COLOR_MENUHILIGHT
#	define COLOR_MENUHILIGHT 29
#endif

#define NOCOLOR ((COLORREF)-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMenuIconMgr::CMenuIconMgr()
{
}

CMenuIconMgr::~CMenuIconMgr()
{
	// cleanup icons
	ClearImages();
}

BOOL CMenuIconMgr::Initialize(CWnd* pWnd)
{
	if (pWnd && !IsHooked())
		return HookWindow(pWnd->GetSafeHwnd());

	else if (!pWnd && IsHooked())
		return CSubclassWnd::HookWindow(NULL);

	// else
	return TRUE;
}

void CMenuIconMgr::ClearImages()
{
	POSITION pos = m_mapID2Icon.GetStartPosition();
	
	while (pos)
	{
		UINT nCmdID = 0;
		HICON hIcon = NULL;
		
		m_mapID2Icon.GetNextAssoc(pos, nCmdID, hIcon);
		::DestroyIcon(hIcon);
	}
	
	m_mapID2Icon.RemoveAll();
}

int CMenuIconMgr::AddImages(const CToolBar& toolbar)
{
	// iterate the non-separator items extracting their images 
	// from the imagelist
	const CToolBarCtrl& tbc = toolbar.GetToolBarCtrl();
	CImageList* pIL = tbc.GetImageList();
	int nBtnCount = tbc.GetButtonCount(), nImage = 0;
	
	for (int nBtn = 0; nBtn < nBtnCount; nBtn++)
	{
		UINT nCmdID = toolbar.GetItemID(nBtn);
		
		if (nCmdID != ID_SEPARATOR)
		{
			m_mapID2Icon[nCmdID] = pIL->ExtractIcon(nImage);
			nImage++;
		}
	}
	
	return nImage;
}

int CMenuIconMgr::AddImages(const CUIntArray& aCmdIDs, const CImageList& il)
{
	ASSERT (aCmdIDs.GetSize() == il.GetImageCount());
	
	if (aCmdIDs.GetSize() != il.GetImageCount())
		return 0;
	
	int nBtnCount = aCmdIDs.GetSize();
	CImageList* pIL = const_cast<CImageList*>(&il);
	
	for (int nBtn = 0; nBtn < nBtnCount; nBtn++)
		SetImage(aCmdIDs[nBtn], pIL->ExtractIcon(nBtn));
	   
	return nBtnCount;
	
}

int CMenuIconMgr::AddImages(const CUIntArray& aCmdIDs, UINT nIDBitmap, int nCx,
                            COLORREF crMask)
{
	CImageList il;
	CEnBitmapEx bm;
	
	if (il.Create(nCx, 16, ILC_COLOR32 | ILC_MASK, 0, 1) && bm.LoadBitmap(nIDBitmap))
	{
		il.Add(&bm, crMask);

		return AddImages(aCmdIDs, il);
	}
	
	return 0;
}

BOOL CMenuIconMgr::ChangeImageID(UINT nCmdID, UINT nNewCmdID)
{
	HICON hIcon = LoadItemImage(nCmdID);
	
	if (hIcon)
	{
		// normal icon
		m_mapID2Icon.RemoveKey(nCmdID);
		m_mapID2Icon[nNewCmdID] = hIcon;
		
		return TRUE;
	}
	
	// else
	return FALSE;
}

HICON CMenuIconMgr::LoadItemImage(UINT nCmdID) 
{
	HICON hIcon = NULL;
	
	m_mapID2Icon.Lookup(nCmdID, hIcon);
	
	return hIcon;
}

BOOL CMenuIconMgr::AddImage(UINT nCmdID, HICON hIcon)
{
	// we copy the icon's small image
	CImageList il;
	
	if (il.Create(16, 16, ILC_COLOR32 | ILC_MASK, 0, 1))
	{
		il.Add(hIcon);
		return SetImage(nCmdID, il.ExtractIcon(0));
	}
	
	return FALSE;
}

BOOL CMenuIconMgr::AddImage(UINT nCmdID, const CImageList& il, int nImage)
{
	CImageList* pIL = const_cast<CImageList*>(&il);

	return SetImage(nCmdID, pIL->ExtractIcon(nImage));
}

void CMenuIconMgr::DeleteImage(UINT nCmdID)
{
	SetImage(nCmdID, NULL);
}

BOOL CMenuIconMgr::SetImage(UINT nCmdID, HICON hIcon)
{
	if (nCmdID)
	{
		if (hIcon)
			m_mapID2Icon[nCmdID] = hIcon;
		else
		{
			// cleanup icon
			if (m_mapID2Icon.Lookup(nCmdID, hIcon))
				::DestroyIcon(hIcon);

			m_mapID2Icon.RemoveKey(nCmdID);
		}

		return TRUE;
	}
	
	return FALSE;
}

LRESULT CMenuIconMgr::WindowProc(HWND /*hRealWnd*/, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITMENUPOPUP:
		if (!HIWORD(lp)) // let windows look after the system menu 
		{
			LRESULT lr = CSubclassWnd::Default();
			OnInitMenuPopup(CMenu::FromHandle((HMENU)wp));
			return lr;
		}
		break;
		
	case WM_DRAWITEM:
		if (OnDrawItem(wp, (LPDRAWITEMSTRUCT)lp))
			return 0L;
		break;
		
	case WM_MEASUREITEM:
		if (OnMeasureItem(wp, (LPMEASUREITEMSTRUCT)lp))
			return 0L;
		break;
	}
	
	return CSubclassWnd::Default();
}

BOOL CMenuIconMgr::OnDrawItem(int /*nIDCtl*/, LPDRAWITEMSTRUCT lpdis)
{
	if (lpdis == NULL || lpdis->CtlType != ODT_MENU)
		return FALSE; // not for a menu

	HICON hIcon = LoadItemImage(lpdis->itemID);

	if (hIcon)
	{
		ICONINFO iconinfo;
		GetIconInfo(hIcon, &iconinfo);

		BITMAP bitmap;
		GetObject(iconinfo.hbmColor, sizeof(bitmap), &bitmap);

		::DrawIconEx(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top, hIcon, 
			bitmap.bmWidth, bitmap.bmHeight, 0, NULL, DI_IMAGE | DI_MASK);

		// cleanup
		::DeleteObject(iconinfo.hbmColor);
		::DeleteObject(iconinfo.hbmMask);

		return TRUE;
	}

	return FALSE;
}

void CMenuIconMgr::OnInitMenuPopup(CMenu* pMenu)
{
	ASSERT (pMenu);

	MENUINFO mnfo;
	mnfo.cbSize = sizeof(mnfo);
	mnfo.fMask = MIM_STYLE;
	mnfo.dwStyle = MNS_CHECKORBMP | MNS_AUTODISMISS;
	::SetMenuInfo(pMenu->GetSafeHmenu(), &mnfo);

	MENUITEMINFO minfo;
	minfo.cbSize = sizeof(minfo);

	for (UINT pos=0; pos<pMenu->GetMenuItemCount(); pos++)
	{
		minfo.fMask = MIIM_FTYPE | MIIM_ID;
		pMenu->GetMenuItemInfo(pos, &minfo, TRUE);

		CString sItem;
		pMenu->GetMenuString(pos, sItem, MF_BYPOSITION);

		HICON hIcon = LoadItemImage(minfo.wID);

		if (hIcon && !(minfo.fType & MFT_OWNERDRAW))
		{
			minfo.fMask = MIIM_BITMAP | MIIM_DATA;
			minfo.hbmpItem = HBMMENU_CALLBACK;

			::SetMenuItemInfo(pMenu->GetSafeHmenu(), pos, TRUE, &minfo);
		}
	}
}

BOOL CMenuIconMgr::OnMeasureItem(int /*nIDCtl*/, LPMEASUREITEMSTRUCT lpmis)
{
	if ((lpmis==NULL)||(lpmis->CtlType != ODT_MENU))
		return FALSE;

	lpmis->itemWidth = 16;
	lpmis->itemHeight = 16;

	HICON hIcon = LoadItemImage(lpmis->itemID);

	if (hIcon)
	{
		ICONINFO iconinfo;
		::GetIconInfo(hIcon, &iconinfo);

		BITMAP bitmap;
		::GetObject(iconinfo.hbmColor, sizeof(bitmap), &bitmap);

		lpmis->itemWidth = bitmap.bmWidth;
		lpmis->itemHeight = bitmap.bmHeight;

		// cleanup
		::DeleteObject(iconinfo.hbmColor);
		::DeleteObject(iconinfo.hbmMask);

		return TRUE;
	}

	return FALSE;
}
