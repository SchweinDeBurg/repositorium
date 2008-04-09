// MFC wrapper for the Brainchild, A syntax coloring edit control
// Copyright (c) 2008 by Elijah Zarezky.
// Created from the code written by Ernest Laurentin.

#if !defined(__BrainchildCtrlMFC_h)
#define __BrainchildCtrlMFC_h

#include "brainchild.h"

/////////////////////////////////////////////////////////////////////////////
// CBrainchildCtrl - client side for brainchild edit control

class CBrainchildCtrl : public CWnd
{
public:
// Construction
	CBrainchildCtrl(void)
	{
	}

	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
	{
		CWnd* pWnd = this;
		return pWnd->Create(BRAINCHILD_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
	}

// Methods
	BOOL LoadFile(BOOL bForce, LPCTSTR lpszFileName)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (SendMessage(BCM_LOADFILE, bForce, (LPARAM)lpszFileName) == BCMR_OK);
	}

	BOOL SaveFile(LPCTSTR lpszFileName)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_SAVEFILE, 0L, (LPARAM)lpszFileName);
	}

	BOOL SetLogFont(const LOGFONT* lpLogFont)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_SETLOGFONT, 0L, (LPARAM)lpLogFont);
	}

	BOOL SetFileName(LPCTSTR lpszFileName)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_SETFILENAME, 0L, (LPARAM)lpszFileName);
	}

	BOOL GotoLine(int nLine)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_GOTOLINE, nLine, 0L);
	}

	LPTSTR GetText(LPTSTR lpszText)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) SendMessage(BCM_GETTEXT, 0L, (LPARAM)lpszText);
	}

	int GetTextLength()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_GETTEXTLENGTH, 0L, 0L);
	}

	LPTSTR GetWord(LPTSTR lpszText)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) SendMessage(BCM_GETWORD, 0L, (LPARAM)lpszText);
	}

	int GetWordLength()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_GETWORDLENGTH, 0L, 0L);
	}

	LPTSTR GetLine(LPTSTR lpszText)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) SendMessage(BCM_GETLINE, 0L, (LPARAM)lpszText);
	}

	int GetLineLength()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_GETLINELENGTH, 0L, 0L);
	}

	LPTSTR GetSelection(LPTSTR lpszText)
	{
		ASSERT(::IsWindow(m_hWnd));
		return (LPTSTR) SendMessage(BCM_GETSELECTION, 0L, (LPARAM)lpszText);
	}

	int GetSelectionLength()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_GETSELECTIONLENGTH, 0L, 0L);
	}

	int CheckModified()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_CHECKMODIFIED, 0L, 0L);
	}

	BOOL SetText(BOOL bForce, LPCTSTR lpszText)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_SETTEXT, bForce, (LPARAM)lpszText);
	}

	void SetSyntaxColoring(BOOL bActive, BOOL bGlobal)
	{
		// No return code is defined...
		ASSERT(::IsWindow(m_hWnd));
		SendMessage(BCM_SYNTAXCOLORING, bActive, (LPARAM)bGlobal);
	}

	BOOL CanUndo()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_CANUNDO, 0L, 0L);
	}

	BOOL SetReadOnly(BOOL bReadOnly)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_SETREADONLY, bReadOnly, 0L);
	}

	BOOL CanRedo()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_CANREDO, 0L, 0L);
	}

	COLORREF SetColor(int nIndex, COLORREF color)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_SETCOLOR, nIndex, (LPARAM)color);
	}

	COLORREF GetColor(int nIndex)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_GETCOLOR, nIndex, 0L);
	}

	BOOL InsertLine(int nPosition, LPCTSTR lpszText)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_INSERTLINE, nPosition, (LPARAM)lpszText);
	}

	BOOL CanCut()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_CANCUT, 0L, 0L);
	}

	BOOL CanCopy()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_CANCOPY, 0L, 0L);
	}

	BOOL CanPaste()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_CANPASTE, 0L, 0L);
	}

	BOOL CanDelete()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_CANDELETE, 0L, 0L);
	}

	BOOL IsModidifed()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_MODIFIED, 0L, 0L);
	}

	BOOL IsOverWrite()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_OVERWRITE, 0L, 0L);
	}

	BOOL IsReadOnly()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_READONLY, 0L, 0L);
	}

	int GetLineCount()
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_NUMBEROFLINES, 0L, 0L);
	}

	BOOL GetFileInfo(LPFILEINFO lpFileInfo)
	{
		ASSERT(::IsWindow(m_hWnd));
		return SendMessage(BCM_GETFILEINFO, 0L, (LPARAM) lpFileInfo);
	}

	// Dialogs
	void ShowGotoDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ASSERT(::IsWindow(m_hWnd));
		SendMessage(BCM_GOTO_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowFindDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ASSERT(::IsWindow(m_hWnd));
		SendMessage(BCM_FIND_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowReplaceDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ASSERT(::IsWindow(m_hWnd));
		SendMessage(BCM_REPLACE_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowPropertiesDialog(LPCDLGTEMPLATE lpDlgTemplate = NULL)
	{
		// No return code is defined...
		ASSERT(::IsWindow(m_hWnd));
		SendMessage(BCM_PROPERTIES_DIALOG, 0L, (LPARAM) lpDlgTemplate);
	}

	void ShowSettingsDialog()
	{
		// No return code is defined...
		ASSERT(::IsWindow(m_hWnd));
		SendMessage(BCM_SETTINGS_DIALOG, 0L, 0L);
	}
};


#endif   //__BrainchildCtrlMFC_h
