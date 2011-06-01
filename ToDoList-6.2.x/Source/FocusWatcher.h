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

// FocusWatcher.h: interface for the CFocusWatcher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FOCUSWATCHER_H__8AE280AF_E103_447C_AF22_8AA12C03E9BD__INCLUDED_)
#define AFX_FOCUSWATCHER_H__8AE280AF_E103_447C_AF22_8AA12C03E9BD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HookMgr.h"

const UINT WM_FW_FOCUSCHANGE = ::RegisterWindowMessage(_T("WM_FW_FOCUSCHANGE")); 

class CFocusWatcher : public CHookMgr<CFocusWatcher>  
{
	friend class CHookMgr<CFocusWatcher>;

public:
	CFocusWatcher();
	virtual ~CFocusWatcher();

	static BOOL Initialize(CWnd* pMainWnd);
	static void UpdateFocus();

protected:
	CWnd* m_pMainWnd;

protected:		
	static CFocusWatcher& Instance() { return CHookMgr<CFocusWatcher>::GetInstance(); }
	virtual BOOL OnCbt(int nCode, WPARAM wParam, LPARAM lParam); 
};

#endif // !defined(AFX_FOCUSWATCHER_H__8AE280AF_E103_447C_AF22_8AA12C03E9BD__INCLUDED_)
