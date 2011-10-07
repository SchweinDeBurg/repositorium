// Copyright (C) 2003-2011 AbstractSpoon Software.
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
// - adjusted #include's paths
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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// ShortcutManager.cpp: implementation of the CShortcutManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ShortcutManager.h"
#include "../../CodeProject/Source/WClassDefines.h"
#include "../../CodeProject/Source/RuntimeDlg.h"
#include "../../CodeProject/Source/WinClasses.h"
#include "Preferences.h"
#include "../../CodeProject/Source/Misc.h"

#include <afxtempl.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

enum
{
	VK_0 = 0x30,
	VK_9 = 0x39,
	VK_A = 0x41,
	VK_Z = 0x5A,
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CShortcutManager::CShortcutManager(BOOL bAutoSendCmds):
m_bAutoSendCmds(bAutoSendCmds),
m_wInvalidComb(0),
m_wFallbackModifiers(0)
{
}

CShortcutManager::~CShortcutManager()
{
}

BOOL CShortcutManager::Initialize(CWnd* pOwner, WORD wInvalidComb, WORD wFallbackModifiers)
{
	if (wInvalidComb && !IsHooked())
	{
		if (pOwner && HookWindow(*pOwner))
		{
			m_wInvalidComb = wInvalidComb;
			m_wFallbackModifiers = wFallbackModifiers;
			LoadSettings();

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CShortcutManager::Release()
{
	if (!IsHooked())
	{
		return TRUE;
	}

	return HookWindow(NULL);
}

void CShortcutManager::SetShortcut(UINT nCmdID, WORD wVirtKeyCode, WORD wModifiers)
{
	UINT nOtherCmdID = 0;
	DWORD dwShortcut = MAKELONG(wVirtKeyCode, wModifiers);

	// if the shortcut == 0 then remove the existing shortcut associated with nCmdID
	if (!dwShortcut)
	{
		m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

		m_mapShortcut2ID.RemoveKey(dwShortcut);

		// mark these commands explicitly as having no shortcut so that
		// the user's intent is clear.
		m_mapID2Shortcut[nCmdID] = NO_SHORTCUT;

		return;
	}
	// check for existing cmds using this shortcut to remove
	else if (m_mapShortcut2ID.Lookup(dwShortcut, nOtherCmdID))
	{
		m_mapShortcut2ID.RemoveKey(dwShortcut);

		// mark these commands explicitly as having no shortcut so that
		// they subsequently will not be overwritten
		m_mapID2Shortcut[nOtherCmdID] = NO_SHORTCUT;
	}

	// then simple add
	AddShortcut(nCmdID, wVirtKeyCode, wModifiers);
}

void CShortcutManager::SetShortcut(UINT nCmdID, DWORD dwShortcut)
{
	SetShortcut(nCmdID, LOWORD(dwShortcut), HIWORD(dwShortcut));
}

BOOL CShortcutManager::AddShortcut(UINT nCmdID, DWORD dwShortcut)
{
	return AddShortcut(nCmdID, LOWORD(dwShortcut), HIWORD(dwShortcut));
}

BOOL CShortcutManager::AddShortcut(UINT nCmdID, WORD wVirtKeyCode, WORD wModifiers)
{
	// test for invalid modifiers
	if (ValidateModifiers(wModifiers, wVirtKeyCode) != wModifiers)
	{
		return FALSE;
	}

	// check for existing cmds using this shortcut
	DWORD dwShortcut = MAKELONG(wVirtKeyCode, wModifiers);

	if (!nCmdID || !dwShortcut)
	{
		return FALSE;
	}

	UINT nOtherCmdID = 0;

	if (m_mapShortcut2ID.Lookup(dwShortcut, nOtherCmdID) && nOtherCmdID)
	{
		return FALSE;
	}

	// check for existing shortcut on this cmd that we'll need to clean up
	DWORD dwOtherShortcut = 0;

	if (m_mapID2Shortcut.Lookup(nCmdID, dwOtherShortcut))
	{
		m_mapShortcut2ID.RemoveKey(dwOtherShortcut);
	}

	m_mapShortcut2ID[dwShortcut] = nCmdID;
	m_mapID2Shortcut[nCmdID] = dwShortcut;

	return TRUE;
}

WORD CShortcutManager::ValidateModifiers(WORD wModifiers, WORD wVirtKeyCode) const
{
	if (!m_wInvalidComb) // optimization
	{
		return wModifiers;
	}

	// check for our special modifiers first
	if ((m_wInvalidComb & HKCOMB_EXFKEYS) && (wVirtKeyCode >= VK_F1 && wVirtKeyCode <= VK_F24))
	{
		return wModifiers;
	}

	// test for invalid combinations
	BOOL bCtrl = (wModifiers & HOTKEYF_CONTROL);
	BOOL bShift = (wModifiers & HOTKEYF_SHIFT);
	BOOL bAlt = (wModifiers & HOTKEYF_ALT);
	BOOL bExtended = (wModifiers & HOTKEYF_EXT);

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
		return (WORD)(m_wFallbackModifiers | (bExtended ? HOTKEYF_EXT : 0x0));
	}

	// else ok
	return wModifiers;
}

DWORD CShortcutManager::GetShortcut(WORD wVirtKeyCode, BOOL bExtended) const
{
	BOOL bCtrl = Misc::KeyIsPressed(VK_CONTROL);
	BOOL bShift = Misc::KeyIsPressed(VK_SHIFT);
	BOOL bAlt = Misc::KeyIsPressed(VK_MENU);

	WORD wModifiers = (WORD)((bCtrl ? HOTKEYF_CONTROL : 0) |
		(bShift ? HOTKEYF_SHIFT : 0) |
		(bAlt ? HOTKEYF_ALT : 0) |
		(bExtended ? HOTKEYF_EXT : 0));

	return MAKELONG(wVirtKeyCode, wModifiers);
}


UINT CShortcutManager::ProcessMessage(const MSG* pMsg, DWORD* pShortcut) const
{
	// only process accelerators if we are enabled and visible
	if (!IsWindowEnabled() || !IsWindowVisible())
	{
		return FALSE;
	}

	// we only process keypresses
	if (pMsg->message != WM_KEYDOWN && pMsg->message != WM_SYSKEYDOWN)
	{
		return FALSE;
	}

	// also check that it's one of our children with the focus
	// not another popup window
	CWnd* pWnd = CWnd::FromHandle(pMsg->hwnd);

	CWnd* pMainWnd = GetCWnd();
	CWnd* pTopParent = pWnd->GetParentOwner();

	if (pTopParent != pMainWnd)
	{
		return FALSE;
	}

	switch (pMsg->wParam)
	{
	case VK_CONTROL:
	case VK_SHIFT:
	case VK_MENU:
	case VK_NUMLOCK:
	case VK_SCROLL:
	case VK_CAPITAL:
		return FALSE;

		// don't handle return/cancel keys
	case VK_RETURN:
	case VK_CANCEL:
		return FALSE;

	case VK_MBUTTON:
		break;

		// shortcut keys
	default:
		{
			// don't process messages destined for hotkey controls!
			if (CWinClasses::IsClass(pMsg->hwnd, WC_HOTKEY))
			{
				return FALSE;
			}

			// don't process AltGr if destined for edit control
			BOOL bEdit = CWinClasses::IsEditControl(pMsg->hwnd);

			if (bEdit && Misc::KeyIsPressed(VK_RMENU))
			{
				return FALSE;
			}

			// get DWORD shortcut
			BOOL bExtKey = (pMsg->lParam & 0x01000000);
			DWORD dwShortcut = GetShortcut((WORD)pMsg->wParam, bExtKey);

			// look it up
			UINT nCmdID = 0;

			if (!m_mapShortcut2ID.Lookup(dwShortcut, nCmdID) || !nCmdID)
			{
				return FALSE;
			}

			// check if HKCOMB_EDITCTRLS is set and a edit has the focus
			// and the shortcut clashes
			if (bEdit && (m_wInvalidComb & HKCOMB_EDITCTRLS))
			{
				// 1. check does not clash with edit shortcuts
				if (IsEditShortcut(dwShortcut))
				{
					return FALSE;
				}

				//WORD wVirtKeyCode = LOWORD(dwShortcut);
				WORD wModifiers = HIWORD(dwShortcut);

				// 2. can be a function key
				if (pMsg->wParam >= VK_F1 && pMsg->wParam <= VK_F24)
				{
					// ok
				}
				// 3. else must have <ctrl> or <alt>
				else if (!(wModifiers & (HOTKEYF_ALT | HOTKEYF_CONTROL)))
				{
					return FALSE;
				}
			}

			// return command ID
			if (m_bAutoSendCmds)
			{
				SendMessage(WM_COMMAND, nCmdID);
			}

			if (pShortcut)
			{
				*pShortcut = dwShortcut;
			}

			return nCmdID;
		}
	}

	return FALSE;
}

BOOL CShortcutManager::IsEditShortcut(DWORD dwShortcut)
{
	switch (dwShortcut)
	{
	case MAKELONG('C', HOTKEYF_CONTROL): // copy
	case MAKELONG('V', HOTKEYF_CONTROL): // paste
	case MAKELONG('X', HOTKEYF_CONTROL): // cut
	case MAKELONG(VK_LEFT, HOTKEYF_CONTROL | HOTKEYF_EXT): // left one word
	case MAKELONG(VK_RIGHT, HOTKEYF_CONTROL | HOTKEYF_EXT): // right one word
	case MAKELONG(VK_DELETE, 0):
		return TRUE;
	}

	// all else
	return FALSE;
}

LRESULT CShortcutManager::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_INITMENUPOPUP:
		{
			// default processing so all text changes
			// are complete before we have a go
			LRESULT lr = Default();

			PrepareMenuItems(CMenu::FromHandle((HMENU)wp));

			return lr;
		}
		break;

	case WM_DESTROY:
		{
			// must call rest of chain first
			LRESULT lr = CSubclassWnd::WindowProc(hRealWnd, msg, wp, lp);
			HookWindow(NULL);

			SaveSettings();
			return lr;
		}
		break;
	}

	return CSubclassWnd::WindowProc(hRealWnd, msg, wp, lp);
}

void CShortcutManager::PrepareMenuItems(CMenu* pMenu) const
{
	if (!pMenu || !pMenu->GetSafeHmenu())
	{
		return;
	}

	// we iterate all the menu items
	// if we find a match we get the menu text and add the shortcut
	// first removing any existing one
	int nItem = pMenu->GetMenuItemCount();

	while (nItem--)
	{
		UINT nCmdID = pMenu->GetMenuItemID(nItem);
		DWORD dwShortcut = GetShortcut(nCmdID);

		if (!nCmdID || nCmdID == (UINT)-1)
		{
			continue;
		}

		// note: we must handle items without shortcuts as well
		// as ones with

		// get the menu text
		MENUITEMINFO minfo;
		minfo.cbSize = sizeof(minfo);
		minfo.fMask = MIIM_FTYPE | MIIM_STRING;
		minfo.fType = MFT_STRING;
		minfo.dwTypeData = NULL;
		minfo.cch = 0;

		::GetMenuItemInfo(pMenu->GetSafeHmenu(), nItem, TRUE, &minfo);
		CString sMenuText;

		if (!minfo.cch)
		{
			continue;   // ??
		}

		minfo.cch++;
		minfo.dwTypeData = sMenuText.GetBuffer(minfo.cch);
		::GetMenuItemInfo(pMenu->GetSafeHmenu(), nItem, TRUE, &minfo);
		sMenuText.ReleaseBuffer();

		// look for '\t' indicating existing hint
		int nTab = sMenuText.Find(_T('\t'));

		// remove it
		if (nTab >= 0)
		{
			sMenuText = sMenuText.Left(nTab);
		}
		// else if it didn't have one and it has no shortcut then continue
		else if (!dwShortcut)
		{
			continue;
		}

		// add new hint
		CString sShortcut = GetShortcutText(dwShortcut);

		if (!sShortcut.IsEmpty())
		{
			sMenuText += _T('\t');
			sMenuText += sShortcut;
		}

		// update menu item text
		minfo.dwTypeData = sMenuText.GetBuffer();
		::SetMenuItemInfo(pMenu->GetSafeHmenu(), nItem, TRUE, &minfo);
		sMenuText.ReleaseBuffer();
	}
}

int CShortcutManager::BuildMapping(UINT nMenuID, CStringArray& aMapping, char cDelim)
{
	CMenu menu;

	if (!menu.LoadMenu(nMenuID) || !menu.GetMenuItemCount())
	{
		return 0;
	}

	return BuildMapping(&menu, NULL, aMapping, cDelim);
}

int CShortcutManager::BuildMapping(CMenu* pMenu, LPCTSTR szParentName, CStringArray& aMapping, char cDelim)
{
	int nItems = pMenu->GetMenuItemCount();

	for (int nItem = 0; nItem < nItems; nItem++)
	{
		CString sMenuText, sItemText;

		pMenu->GetMenuString(nItem, sMenuText, MF_BYPOSITION);

		if (szParentName && *szParentName)
		{
			sItemText.Format(_T("%s > %s"), szParentName, sMenuText);
		}
		else
		{
			sItemText = sMenuText;
		}

		UINT nCmdID = pMenu->GetMenuItemID(nItem);

		if (nCmdID == (UINT)-1) // sub menu
		{
			CMenu* pSubMenu = pMenu->GetSubMenu(nItem);

			BuildMapping(pSubMenu, sItemText, aMapping, cDelim);
		}
		else
		{
			DWORD dwShortcut = GetShortcut(nCmdID);

			if (dwShortcut)
			{
				CString sShortcut = GetShortcutText(dwShortcut), sItem;
				sItem.Format(_T("%s%c%s"), sShortcut, cDelim, sItemText);

				// remove single '&'
				// rather painful way to do it to avoid replacing '&&'
				sItem.Replace(_T("&&"), _T("~~"));
				sItem.Remove(_T('&'));
				sItem.Replace(_T("~~"), _T("&&"));

				aMapping.Add(sItem);
			}
		}
	}

	// add a space between sections unless already added
	if (aMapping.GetSize() && !aMapping[aMapping.GetSize() - 1].IsEmpty())
	{
		aMapping.Add(_T(""));
	}

	return aMapping.GetSize();
}

UINT CShortcutManager::GetCommandID(DWORD dwShortcut)
{
	UINT nCmdID = 0;

	m_mapShortcut2ID.Lookup(dwShortcut, nCmdID);

	return nCmdID;
}

DWORD CShortcutManager::GetShortcut(UINT nCmdID) const
{
	DWORD dwShortcut = 0;

	m_mapID2Shortcut.Lookup(nCmdID, dwShortcut);

	return (dwShortcut == NO_SHORTCUT) ? 0 : dwShortcut;
}

void CShortcutManager::DeleteShortcut(UINT nCmdID)
{
	DWORD dwShortcut = 0;

	if (m_mapID2Shortcut.Lookup(nCmdID, dwShortcut))
	{
		m_mapID2Shortcut.RemoveKey(nCmdID);

		// remove reverse mapping too
		m_mapShortcut2ID.RemoveKey(dwShortcut);
	}
}

CString CShortcutManager::GetShortcutTextByCmd(UINT nCmdID)
{
	return GetShortcutText(GetShortcut(nCmdID));
}

CString CShortcutManager::GetShortcutText(DWORD dwShortcut)
{
	if (!dwShortcut || dwShortcut == NO_SHORTCUT)
	{
		return _T("");
	}

	CString sText;

	WORD wVirtKeyCode = LOWORD(dwShortcut);
	WORD wModifiers = HIWORD(dwShortcut);

	if (wModifiers & HOTKEYF_CONTROL)
	{
		sText += GetKeyName(VK_CONTROL);
		sText += _T("+");
	}

	if (wModifiers & HOTKEYF_SHIFT)
	{
		sText += GetKeyName(VK_SHIFT);
		sText += _T("+");
	}

	if (wModifiers & HOTKEYF_ALT)
	{
		sText += GetKeyName(VK_MENU);
		sText += _T("+");
	}

	CString sKey = GetKeyName(wVirtKeyCode, (wModifiers & HOTKEYF_EXT));

	if (!sKey.IsEmpty())
	{
		sText += sKey;
	}
	else
	{
		sText.Empty();
	}

	return sText;
}

CString CShortcutManager::GetKeyName(WORD wVirtKeyCode, BOOL bExtended)
{
	const int KEYNAMELEN = 64;
	static TCHAR szKeyName[64];

	WORD wScanCode = (WORD)MapVirtualKey(wVirtKeyCode, 0);

	// build lParam to send to GetKeyNameText
	LPARAM lParam = (wScanCode * 0x00010000);

	if (bExtended)
	{
		lParam += 0x01000000;
	}

	GetKeyNameText(lParam, szKeyName, KEYNAMELEN);

	return CString(szKeyName);
}

void CShortcutManager::LoadSettings()
{
	CPreferences prefs;

	// load shortcuts overriding any defaults
	int nItem = prefs.GetProfileInt(_T("KeyboardShortcuts"), _T("NumItems"), 0);

	while (nItem--)
	{
		CString sKey;
		sKey.Format(_T("KeyboardShortcuts\\Item%02d"), nItem);

		UINT nCmdID = (UINT)prefs.GetProfileInt(sKey, _T("CmdID"), 0);
		DWORD dwShortcut = (DWORD)prefs.GetProfileInt(sKey, _T("Shortcut"), 0);

		if (nCmdID && dwShortcut)
		{
			SetShortcut(nCmdID, dwShortcut);
		}
	}
}

void CShortcutManager::SaveSettings()
{
	CPreferences prefs;

	prefs.WriteProfileInt(_T("KeyboardShortcuts"), _T("NumItems"), m_mapID2Shortcut.GetCount());

	POSITION pos = m_mapID2Shortcut.GetStartPosition();
	int nItem = 0;

	while (pos)
	{
		UINT nCmdID = 0;
		DWORD dwShortcut = 0;

		m_mapID2Shortcut.GetNextAssoc(pos, nCmdID, dwShortcut);

		if (nCmdID && dwShortcut)
		{
			CString sKey;
			sKey.Format(_T("KeyboardShortcuts\\Item%02d"), nItem);

			prefs.WriteProfileInt(sKey, _T("CmdID"), nCmdID);
			prefs.WriteProfileInt(sKey, _T("Shortcut"), dwShortcut);

			nItem++;
		}
	}
}
