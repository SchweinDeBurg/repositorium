// WTL wrapper for the Brainchild, A syntax coloring edit control
// Copyright (c) 2003 by Ernest Laurentin.
// Enchancements (c) 2008 by Elijah Zarezky.

#ifndef _BRAINCHILDEDIT_CTRL_
#define _BRAINCHILDEDIT_CTRL_

#ifndef __cplusplus
	#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLWIN_H__
	#error BrainChildEditCtrl.h requires atlwin.h to be included first
#endif

#include "brainchild.h"

/////////////////////////////////////////////////////////////////////////////
// CBrainChildEditCtrlT - client side for brainchild edit control

template <class TBase>
class CBrainChildEditCtrlT : public TBase
{
public:
// Constructors
	CBrainChildEditCtrlT(HWND hWnd = NULL) : TBase(hWnd) { }

	CBrainChildEditCtrlT< TBase >& operator=(HWND hWnd)
	{
		m_hWnd = hWnd;
		return *this;
	}

	HWND Create(HWND hWndParent, _U_RECT rect = NULL, LPCTSTR szWindowName = NULL,
			DWORD dwStyle = WS_CHILD|WS_VISIBLE, DWORD dwExStyle = 0,
			_U_MENUorID MenuOrID = 0U, LPVOID lpCreateParam = NULL)
	{
		dwStyle |= WS_CHILD;
		return CWindow::Create(GetWndClassName(), hWndParent, rect.m_lpRect, szWindowName, dwStyle, dwExStyle, MenuOrID.m_hMenu, lpCreateParam);
	}

// Attributes
	static LPCTSTR GetWndClassName()
	{
		return BRAINCHILD_CLASS;
	}

	// Methods
	BOOL LoadFile(BOOL bForce, LPCTSTR lpszFileName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (::SendMessage(m_hWnd, BCM_LOADFILE, bForce, (LPARAM)lpszFileName) == BCMR_OK);
	}

	BOOL SaveFile(LPCTSTR lpszFileName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_SAVEFILE, 0L, (LPARAM)lpszFileName);
	}

	BOOL SetLogFont(const LOGFONT* lpLogFont)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_SETLOGFONT, 0L, (LPARAM)lpLogFont);
	}

	BOOL SetFileName(LPCTSTR lpszFileName)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_SETFILENAME, 0L, (LPARAM)lpszFileName);
	}

	BOOL GotoLine(int nLine)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_GOTOLINE, nLine, 0L);
	}

	LPTSTR GetText(LPTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) ::SendMessage(m_hWnd, BCM_GETTEXT, 0L, (LPARAM)lpszText);
	}

	int GetTextLength()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_GETTEXTLENGTH, 0L, 0L);
	}

	LPTSTR GetWord(LPTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) ::SendMessage(m_hWnd, BCM_GETWORD, 0L, (LPARAM)lpszText);
	}

	int GetWordLength()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_GETWORDLENGTH, 0L, 0L);
	}

	LPTSTR GetLine(LPTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) ::SendMessage(m_hWnd, BCM_GETLINE, 0L, (LPARAM)lpszText);
	}

	int GetLineLength()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_GETLINELENGTH, 0L, 0L);
	}

	LPTSTR GetSelection(LPTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) ::SendMessage(m_hWnd, BCM_GETSELECTION, 0L, (LPARAM)lpszText);
	}

	int GetSelectionLength()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_GETSELECTIONLENGTH, 0L, 0L);
	}

	int CheckModified()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_CHECKMODIFIED, 0L, 0L);
	}

	BOOL SetText(BOOL bForce, LPCTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_SETTEXT, bForce, (LPARAM)lpszText);
	}

	void SetSyntaxColoring(BOOL bActive, BOOL bGlobal)
	{
		// No return code is defined...
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BCM_SYNTAXCOLORING, bActive, (LPARAM)bGlobal);
	}

	BOOL CanUndo()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_CANUNDO, 0L, 0L);
	}

	BOOL SetReadOnly(BOOL bReadOnly)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_SETREADONLY, bReadOnly, 0L);
	}

	BOOL CanRedo()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_CANREDO, 0L, 0L);
	}

	COLORREF SetColor(int nIndex, COLORREF color)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_SETCOLOR, nIndex, (LPARAM)color);
	}

	COLORREF GetColor(int nIndex)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_GETCOLOR, nIndex, 0L);
	}

	BOOL InsertLine(int nPosition, LPCTSTR lpszText)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_INSERTLINE, nPosition, (LPARAM)lpszText);
	}

	BOOL CanCut()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_CANCUT, 0L, 0L);
	}

	BOOL CanCopy()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_CANCOPY, 0L, 0L);
	}

	BOOL CanPaste()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_CANPASTE, 0L, 0L);
	}

	BOOL CanDelete()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_CANDELETE, 0L, 0L);
	}

	BOOL IsModidifed()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_MODIFIED, 0L, 0L);
	}

	BOOL IsOverWrite()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_OVERWRITE, 0L, 0L);
	}

	BOOL IsReadOnly()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_READONLY, 0L, 0L);
	}

	int GetLineCount()
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_NUMBEROFLINES, 0L, 0L);
	}

	BOOL GetFileInfo(LPFILEINFO lpFileInfo)
	{
		ATLASSERT(::IsWindow(m_hWnd));
		return ::SendMessage(m_hWnd, BCM_GETFILEINFO, 0L, (LPARAM) lpFileInfo);
	}

	// Dialogs
	void ShowGotoDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BCM_GOTO_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowFindDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BCM_FIND_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowReplaceDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BCM_REPLACE_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowPropertiesDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BCM_PROPERTIES_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowSettingsDialog()
	{
		// No return code is defined...
		ATLASSERT(::IsWindow(m_hWnd));
		::SendMessage(m_hWnd, BCM_SETTINGS_DIALOG, 0L, 0L);
	}
};


typedef CBrainChildEditCtrlT<ATL::CWindow> CBrainChildEditCtrl;

#endif //_BRAINCHILDEDIT_CTRL_
