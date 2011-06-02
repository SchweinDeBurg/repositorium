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

#if !defined(AFX_HOTKEYCTRLEX_H__D798C9D0_5F7D_42A1_BA5C_0411A65D234B__INCLUDED_)
#define AFX_HOTKEYCTRLEX_H__D798C9D0_5F7D_42A1_BA5C_0411A65D234B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HotKeyCtrlEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CHotKeyCtrlEx window

class CHotKeyCtrlEx : public CHotKeyCtrl
{
	// Construction
public:
	CHotKeyCtrlEx(BOOL bGlobal = FALSE);

	void SetHotKey(WORD wVirtualKeyCode, WORD wModifiers);
	void SetHotKey(DWORD dwHotkey);

	// Attributes
protected:
	WORD m_wInvalidComb;
	WORD m_wModifiers;
	BOOL m_bGlobal;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHotKeyCtrlEx)
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	// Implementation
public:
	virtual ~CHotKeyCtrlEx();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHotKeyCtrlEx)
	//}}AFX_MSG
	afx_msg LRESULT OnSetRules(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HOTKEYCTRLEX_H__D798C9D0_5F7D_42A1_BA5C_0411A65D234B__INCLUDED_)
