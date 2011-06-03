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
// - merged with ToDoList version 6.1.2-6.2.2 sources
//*****************************************************************************

#if !defined(AFX_TODOCOMMENTSCTRL_H__E9A5D982_4D40_43B2_A071_E3BE70D122B7__INCLUDED_)
#define AFX_TODOCOMMENTSCTRL_H__E9A5D982_4D40_43B2_A071_E3BE70D122B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ToDoCommentsCtrl.h : header file
//

#include "../../Common/UrlRichEditCtrl.h"
#include "../../Common/IContentControl.h"
#include "../../Common/RichEditSpellCheck.h"

/////////////////////////////////////////////////////////////////////////////
// CToDoCommentsCtrl window

class CToDoCommentsCtrl : public CUrlRichEditCtrl, public IContentControl
{
// Construction
public:
	CToDoCommentsCtrl();

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// IContentControl implementation
	// supports plain text content only
	int GetTextContent(TCHAR* szContent, int nLength) const;
	bool SetTextContent(const TCHAR* szContent, BOOL bResetSelection);
	int GetContent(unsigned char* /*pContent*/) const
	{
		return 0;
	}
	bool SetContent(unsigned char* /*pContent*/, int /*nLength*/, BOOL /*bResetSelection*/)
	{
		return false;
	}
	void SetReadOnly(bool bReadOnly);
	HWND GetHwnd() const
	{
		return GetSafeHwnd();
	}
	const char* GetTypeID() const
	{
		return "PLAIN_TEXT";
	}
	void Release()
	{
		DestroyWindow();
		delete this;
	}
	bool ProcessMessage(MSG* pMsg);
	ISpellCheck* GetSpellCheckInterface()
	{
		return &m_reSpellCheck;
	}
	bool Undo()
	{
		return CUrlRichEditCtrl::Undo() != 0;
	}
	bool Redo()
	{
		return CUrlRichEditCtrl::Redo() != 0;
	}
	void SetUITheme(const UITHEME* /*pTheme*/) {}
	void SavePreferences(IPreferences* pPrefs, LPCTSTR szKey) const;
	void LoadPreferences(const IPreferences* pPrefs, LPCTSTR szKey);

// Attributes
protected:
	BOOL m_bAllowNotify;
	BOOL m_bWordWrap;
	CRichEditSpellCheck m_reSpellCheck;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToDoCommentsCtrl)
protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CToDoCommentsCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CToDoCommentsCtrl)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	afx_msg void OnCommentsMenuCmd(UINT nCmdID);
	afx_msg void OnUpdateCommentsMenuCmd(CCmdUI* pCmdUI);
	afx_msg BOOL OnChangeText();
	afx_msg BOOL OnKillFocus();
	afx_msg LRESULT OnSetFont(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnSetWordWrap(WPARAM wp, LPARAM lp);

	DECLARE_MESSAGE_MAP()

	virtual LRESULT SendNotifyCustomUrl(LPCTSTR szUrl) const;

protected:
	BOOL IsTDLClipboardEmpty() const;
	void SetWordWrap(BOOL bWrap);
	void Copy();
	BOOL Paste();
	BOOL CanPaste();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TODOCOMMENTSCTRL_H__E9A5D982_4D40_43B2_A071_E3BE70D122B7__INCLUDED_)
