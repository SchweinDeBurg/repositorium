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
//*****************************************************************************

// EditShortcutMgr.cpp: implementation of the CEditShortcutMgr class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EditShortcutMgr.h"
#include "Misc.h"
#include "WinClasses.h"
#include "WClassDefines.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEditShortcutMgr::CEditShortcutMgr()
{
}

CEditShortcutMgr::~CEditShortcutMgr()
{
}

BOOL CEditShortcutMgr::Initialize(DWORD dwSelectAllShortcut)
{
	GetInstance().m_dwSelectAllShortcut = dwSelectAllShortcut;

	return GetInstance().InitHooks(HM_KEYBOARD, WC_EDIT);
}

BOOL CEditShortcutMgr::OnKeyboard(UINT uVirtKey, UINT /*uFlags*/)
{
	WORD wModifiers = 0;
	
	if (Misc::KeyIsPressed(VK_CONTROL))
		wModifiers |= HOTKEYF_CONTROL;
	
	if (Misc::KeyIsPressed(VK_SHIFT))
		wModifiers |= HOTKEYF_SHIFT;
	
	if (Misc::KeyIsPressed(VK_MENU))
		wModifiers |= HOTKEYF_ALT;
	
	DWORD dwShortcut = MAKELONG(uVirtKey, wModifiers);
	
	if (dwShortcut == m_dwSelectAllShortcut)
	{
		HWND hwnd = ::GetFocus();
		
		if (CWinClasses::IsEditControl(hwnd))
		{
			::SendMessage(hwnd, EM_SETSEL, 0, -1);
			return TRUE;
		}
	}
	
	// all else
	return FALSE;
}
