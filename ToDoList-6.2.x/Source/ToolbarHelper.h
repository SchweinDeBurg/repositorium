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
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
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
//*****************************************************************************

// ToolbarHelper.h: interface for the CToolbarHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TOOLBARHELPER_H__86A32540_80BF_421C_97E3_6E760BF427A8__INCLUDED_)
#define AFX_TOOLBARHELPER_H__86A32540_80BF_421C_97E3_6E760BF427A8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subclass.h"
#include <afxtempl.h>

class CToolbarHelper : protected CSubclassWnd, protected CSubclasser
{
public:
	CToolbarHelper();
	virtual ~CToolbarHelper();

	BOOL Initialize(CToolBar* pToolbar, CWnd* pToolbarParent);
	BOOL Release(BOOL bClearDropBtns = TRUE);

	BOOL ClearDropButton(UINT nBtnCmdID, BOOL bRedraw = TRUE);
	BOOL SetDropButton(UINT nBtnCmdID, UINT nMenuID, int nSubMenu, UINT nDefCmdID = 0, char cHotkey = 0);
	BOOL SetDefaultMenuID(UINT nBtnCmdID, UINT nDefCmdID = 0);
	BOOL SetTooltip(UINT nBtnCmdID, LPCTSTR szTip);
	BOOL SetTooltip(UINT nBtnCmdID, UINT nIDTip);

	BOOL IsInitialized()
	{
		return IsHooked();
	}

	void EnableMultilineText(BOOL bEnable = TRUE, int nWidth = 200);
	BOOL ProcessMessage(MSG* pMsg); // called by parent

	static void PrepareMenuItems(CMenu* pMenu, CWnd* pWnd);
	static CString GetTip(UINT nID);

protected:
	CToolBar* m_pToolbar;

	struct THButton
	{
		UINT nMenuID;
		int nSubMenu;
		UINT nDefCmdID;
		char cHotKey;
		TCHAR szTip[128];
	};

	CMap<UINT, UINT, THButton, THButton&> m_mapTHButtons;
	BOOL m_bMultiline;
	int m_nMultilineWidth;
	CToolTipCtrl m_tt;

protected:
	LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	CString GetTip(UINT nID, LPPOINT pPoint) const;
	BOOL DisplayDropMenu(UINT nCmdID, BOOL bPressBtn = FALSE);
	BOOL IsCmdEnabled(UINT nCmdID) const;
	BOOL SetButton(UINT nBtnCmdID, UINT nMenuID, int nSubMenu, UINT nDefCmdID, char cHotkey, LPCTSTR szTip);
	void RefreshTooltipRects();
	void InitTooltips();

	LRESULT ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
};

#endif // !defined(AFX_TOOLBARHELPER_H__86A32540_80BF_421C_97E3_6E760BF427A8__INCLUDED_)
