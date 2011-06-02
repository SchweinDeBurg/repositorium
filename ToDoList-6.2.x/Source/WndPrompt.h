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

// WndPrompt.h: interface for the CWndPrompt class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WNDPROMPT_H__485A738F_5BCB_4D7E_9B3E_6E9382AC62D8__INCLUDED_)
#define AFX_WNDPROMPT_H__485A738F_5BCB_4D7E_9B3E_6E9382AC62D8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subclass.h"
#include <afxtempl.h>

class CWndPrompt : public CSubclassWnd
{
public:
	CWndPrompt();
	virtual ~CWndPrompt();

	BOOL Initialize(HWND hWnd, LPCTSTR szPrompt, UINT nCheckMsg, LRESULT lRes = 0, int nVertOffset = 0);
	void SetPrompt(LPCTSTR szPrompt);

protected:
	CString m_sPrompt;
	UINT m_nCheckMsg;
	LRESULT m_lCheckResult;
	int m_nVertOffset;
	CString m_sClass; // for some tweaking

protected:
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	BOOL WantPrompt(BOOL bCheckFocus = TRUE);
	void DrawPrompt(HDC hdc);
};

class CWndPromptManager
{
public:
	CWndPromptManager();
	virtual ~CWndPromptManager();

	BOOL SetPrompt(UINT nIDCtrl, HWND hwndParent, LPCTSTR szPrompt,
		UINT nCheckMsg, LRESULT lRes = 0, int nVertOffset = 0);
	BOOL SetPrompt(HWND hWnd, LPCTSTR szPrompt, UINT nCheckMsg,
		LRESULT lRes = 0, int nVertOffset = 0);

	BOOL SetPrompt(UINT nIDCtrl, HWND hwndParent, UINT nIDPrompt,
		UINT nCheckMsg, LRESULT lRes = 0, int nVertOffset = 0);
	BOOL SetPrompt(HWND hWnd, UINT nIDPrompt, UINT nCheckMsg,
		LRESULT lRes = 0, int nVertOffset = 0);

	// special cases
	BOOL SetEditPrompt(UINT nIDEdit, HWND hwndParent, LPCTSTR szPrompt);
	BOOL SetEditPrompt(HWND hwndEdit, LPCTSTR szPrompt);
	BOOL SetComboPrompt(UINT nIDCombo, HWND hwndParent, LPCTSTR szPrompt);
	BOOL SetComboPrompt(HWND hwndCombo, LPCTSTR szPrompt);
	BOOL SetComboEditPrompt(UINT nIDCombo, HWND hwndParent, LPCTSTR szPrompt);
	BOOL SetComboEditPrompt(HWND hwndCombo, LPCTSTR szPrompt);

	BOOL SetEditPrompt(UINT nIDEdit, HWND hwndParent, UINT nIDPrompt);
	BOOL SetEditPrompt(HWND hwndEdit, UINT nIDPrompt);
	BOOL SetComboPrompt(UINT nIDCombo, HWND hwndParent, UINT nIDPrompt);
	BOOL SetComboPrompt(HWND hwndCombo, UINT nIDPrompt);
	BOOL SetComboEditPrompt(UINT nIDCombo, HWND hwndParent, UINT nIDPrompt);
	BOOL SetComboEditPrompt(HWND hwndCombo, UINT nIDPrompt);

protected:
	CMap<HWND, HWND, CWndPrompt*, CWndPrompt*&> m_mapWnds;

};

#endif // !defined(AFX_WNDPROMPT_H__485A738F_5BCB_4D7E_9B3E_6E9382AC62D8__INCLUDED_)
