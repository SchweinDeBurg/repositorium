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

// FocusWatcher.cpp: implementation of the CFocusWatcher class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FocusWatcher.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFocusWatcher::CFocusWatcher() : m_pMainWnd(NULL)
{

}

CFocusWatcher::~CFocusWatcher()
{

}

BOOL CFocusWatcher::Initialize(CWnd* pMainWnd)
{
	if (Instance().CHookMgr<CFocusWatcher>::InitHooks(HM_CBT))
	{
		Instance().m_pMainWnd = pMainWnd;
		return TRUE;
	}

	return FALSE;
}

BOOL CFocusWatcher::OnCbt(int nCode, WPARAM wParam, LPARAM lParam)
{   
	ASSERT (m_hCbtHook);

	if (nCode == HCBT_SETFOCUS)
	{
		//ASSERT_VALID(m_pMainWnd);
		m_pMainWnd->SendMessage(WM_FW_FOCUSCHANGE, wParam, lParam);
	}
	
	return FALSE; // to continue routing
}

void CFocusWatcher::UpdateFocus()
{
	//ASSERT_VALID(Instance().m_pMainWnd);
	Instance().m_pMainWnd->SendMessage(WM_FW_FOCUSCHANGE, (WPARAM)::GetFocus(), 0L);
}
