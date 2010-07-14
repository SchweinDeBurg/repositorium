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

// MenuIconMgr.h: interface for the CMenuIconMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MENUICONMGR_H__0FF0228C_515C_4E93_A957_1952AFD0F3A1__INCLUDED_)
#define AFX_MENUICONMGR_H__0FF0228C_515C_4E93_A957_1952AFD0F3A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subclass.h"
#include <afxtempl.h>

class CMenuIconMgr : public CSubclassWnd  
{
public:
	CMenuIconMgr();
	virtual ~CMenuIconMgr();
	
	BOOL Initialize(CWnd* pWnd);

	BOOL AddImage(UINT nCmdID, HICON hIcon); // hIcon will be copied
	BOOL SetImage(UINT nCmdID, HICON hIcon); // hIcon will be cleaned up

	BOOL AddImage(UINT nCmdID, const CImageList& il, int nImage);
	int AddImages(const CToolBar& toolbar);
	int AddImages(const CUIntArray& aCmdIDs, const CImageList& il);
	int AddImages(const CUIntArray& aCmdIDs, UINT nIDBitmap, int nCx, COLORREF crMask);
	
	BOOL ChangeImageID(UINT nCmdID, UINT nNewCmdID);
	void DeleteImage(UINT nCmdID);
	
	void ClearImages();
	
protected:
	CMap<UINT, UINT, HICON, HICON> m_mapID2Icon;
	
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	
	BOOL OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpdis);
	void OnInitMenuPopup(CMenu* pMenu);
	BOOL OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpmis);
	
	HICON LoadItemImage(UINT nCmdID);
};

#endif // !defined(AFX_MENUICONMGR_H__0FF0228C_515C_4E93_A957_1952AFD0F3A1__INCLUDED_)
