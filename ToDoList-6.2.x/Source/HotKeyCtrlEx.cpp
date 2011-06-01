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
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// HotKeyCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "HotKeyCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHotKeyCtrlEx

CHotKeyCtrlEx::CHotKeyCtrlEx(BOOL bGlobal) :
m_wInvalidComb(0), m_wModifiers(0), m_bGlobal(bGlobal)
{
}

CHotKeyCtrlEx::~CHotKeyCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CHotKeyCtrlEx, CHotKeyCtrl)
	//{{AFX_MSG_MAP(CHotKeyCtrlEx)
	//}}AFX_MSG_MAP
	ON_MESSAGE(HKM_SETRULES, OnSetRules)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHotKeyCtrlEx message handlers

LRESULT CHotKeyCtrlEx::OnSetRules(WPARAM wParam, LPARAM lParam)
{
	m_wInvalidComb = (WORD)wParam;
	m_wModifiers = LOWORD(lParam);

	return 0L;
}

void CHotKeyCtrlEx::SetHotKey(WORD wVirtualKeyCode, WORD wModifiers)
{
	CHotKeyCtrl::SetHotKey(wVirtualKeyCode, wModifiers);
}

void CHotKeyCtrlEx::SetHotKey(DWORD dwHotkey)
{
	SetHotKey(LOWORD(dwHotkey), HIWORD(dwHotkey));
}

BOOL CHotKeyCtrlEx::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->hwnd == *this)
	{
		UINT nChar = pMsg->wParam;
		UINT nFlags = HIWORD(pMsg->lParam);

		BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
		BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
		BOOL bAlt = (GetKeyState(VK_MENU) & 0x8000);
		BOOL bExtended = (nFlags & 0x100);

		WORD wModifiers = (WORD)((bCtrl ? HOTKEYF_CONTROL : 0) |
			(bShift ? HOTKEYF_SHIFT : 0) |
			(bAlt ? HOTKEYF_ALT : 0) |
			(bExtended ? HOTKEYF_EXT : 0));

		switch (nChar)
		{
		case VK_DELETE:
		case VK_BACK:
		case VK_SPACE:
		case VK_TAB:
			{
				// some keys not to process by themselves
				if (!bCtrl && !bShift && !bAlt)
				{
					switch (nChar)
					{
					case VK_BACK:
						return CHotKeyCtrl::PreTranslateMessage(pMsg);

					case VK_TAB:
						{
							CWnd* pNext = GetParent()->GetNextDlgTabItem(this);

							if (pNext != this)
							{
								pNext->SetFocus();
								return TRUE; // eat the message
							}
						}
						break;
					}
				}

				// check for breaking rules
				BOOL bFail = ((m_wInvalidComb & HKCOMB_NONE) && !bCtrl && !bShift && !bAlt);

				bFail |= ((m_wInvalidComb & HKCOMB_S) && !bCtrl && bShift && !bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_C) && bCtrl && !bShift && !bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_A) && !bCtrl && !bShift && bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_SC) && bCtrl && bShift && !bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_SA) && !bCtrl && bShift && bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_CA) && bCtrl && !bShift && bAlt);
				bFail |= ((m_wInvalidComb & HKCOMB_SCA) && bCtrl && bShift && bAlt);

				if (bFail)
				{
					wModifiers = (WORD)(m_wModifiers | (bExtended ? HOTKEYF_EXT : 0));
				}

				SetHotKey((WORD)nChar, wModifiers);

				// send parent notification manually
				GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_CHANGE), (LPARAM)GetSafeHwnd());

				return TRUE;
			}
			break;

			// keys not allowed as global (MS reserved)
		case VK_F12:
			if (m_bGlobal && !bCtrl && !bShift && !bAlt)
			{
				return TRUE;   // eat
			}
			break;

			// keys having too much meaning elsewhere
		case VK_RETURN:
		case VK_CANCEL:
			return TRUE; // eat

		}
	}

	return CHotKeyCtrl::PreTranslateMessage(pMsg);
}
