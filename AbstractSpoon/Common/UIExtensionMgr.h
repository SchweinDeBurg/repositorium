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

// UIExtensionMgr.h: interface for the CUIExtensionMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UIEXTENSIONMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
#define AFX_UIEXTENSIONMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IUIExtension.h"
#include "ITaskList.h"
#include <afxtempl.h>

class IUIExtension;
class IUIExtensionWindow;

struct UIEXTENSIONWINDOW
{
	UIEXTENSIONWINDOW(IUIExtensionWindow* p = NULL) : pWindow(p)
	{
		bVisible = p ? p->IsShowing() : FALSE;
	}

	IUIExtensionWindow* pWindow;
	BOOL bVisible;
};

enum UI_SHOW
{
	UIS_SHOW,
	UIS_HIDE,
	UIS_RESTORE,
};

typedef CMap<DWORD, DWORD, UIEXTENSIONWINDOW*, UIEXTENSIONWINDOW*&> CUIExtensionWndMap;

class CUIExtension
{
public:
	CUIExtension(IUIExtension* pExt = NULL);
	virtual ~CUIExtension();

	CString GetMenuText();
	HICON GetIcon();

	IUIExtensionWindow* GetWindow(DWORD dwItemData, HWND hParent, BOOL bAutoCreate = TRUE);

	BOOL HasWindow(DWORD dwItemData, BOOL bVisibleOnly = FALSE) const;
	void RemoveWindow(DWORD dwItemData);
	BOOL ShowWindow(DWORD dwItemData, UI_SHOW nShow = UIS_RESTORE);
	void UpdateWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags = UIU_ALL);

protected:
	IUIExtension* m_pExtension;
	CUIExtensionWndMap* m_pMapWindows;

protected:
	void Release();
	UIEXTENSIONWINDOW* FindWindow(DWORD dwItemData) const;
	CUIExtensionWndMap* WindowMap();
};

class CUIExtensionMgr
{
public:
	CUIExtensionMgr();
	virtual ~CUIExtensionMgr();

	virtual void Initialize();

	int GetNumUIExtensions() const;
	CString GetUIExtensionMenuText(int nExtension) const;
	HICON GetUIExtensionIcon(int nExtension) const;

	IUIExtensionWindow* GetExtensionWindow(int nExtension, DWORD dwItemData, HWND hParent, BOOL bAutoCreate = TRUE);

	BOOL HasAnyExtensionWindows(DWORD dwItemData, BOOL bVisibleOnly = FALSE) const;
	void ShowAllExtensionsWindows(DWORD dwItemData, UI_SHOW nShow = UIS_RESTORE);
	void RemoveAllExtensionsWindows(DWORD dwItemData);
	void UpdateAllExtensionsWindow(DWORD dwItemData, const ITaskList* pTasks, DWORD dwFlags = UIU_ALL);

protected:
	BOOL m_bInitialized;
	CArray<CUIExtension*, CUIExtension*> m_aUIExtensions;

protected:
	static CString& Clean(CString& sText);

};

#endif // !defined(AFX_UIEXTENSIONMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
