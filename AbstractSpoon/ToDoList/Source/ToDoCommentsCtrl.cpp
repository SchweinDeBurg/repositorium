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
// - merged with ToDoList version 6.1.2-6.2.6 sources
//*****************************************************************************

// ToDoCommentsCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "ToDoCommentsCtrl.h"
#include "TDCMsg.h"
#include "TDLSchemaDef.h"
#include "Resource.h"

#include "../../../CodeProject/Source/ToolBarHelper.h"
#include "../../Common/ITaskList.h"
#include "../../../CodeProject/Source/WClassDefines.h"
#include "../../../CodeProject/Source/AutoFlag.h"
#include "../../../CodeProject/Source/RichEditHelper.h"
#include "../../Common/RichEditSpellCheck.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../../CodeProject/Source/FileMisc.h"
#include "../../Common/IPreferences.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_SETWORDWRAP = (WM_USER + 1);

/////////////////////////////////////////////////////////////////////////////
// CToDoCommentsCtrl

CToDoCommentsCtrl::CToDoCommentsCtrl():
m_bAllowNotify(TRUE),
m_bWordWrap(TRUE),
#pragma warning (disable: 4355)
m_reSpellCheck(*this)
#pragma warning (default: 4355)
{
	// add custom protocol to comments field for linking to task IDs
	AddProtocol(TDL_PROTOCOL, TRUE);

	SetGotoErrorMsg(CEnString(IDS_COMMENTSGOTOERRMSG));

	EnableSelectOnFocus(FALSE);
}

CToDoCommentsCtrl::~CToDoCommentsCtrl()
{
}

/////////////////////////////////////////////////////////////////////////////
// IContentCtrl

int CToDoCommentsCtrl::GetTextContent(TCHAR* szContent, int nLength) const
{
	if (!szContent)
	{
		return GetWindowTextLength();
	}

	// else
	if (nLength == -1)
	{
		nLength = _tcslen(szContent) + 1;   // inluding null
	}

	GetWindowText(szContent, nLength);

	return nLength;
}

bool CToDoCommentsCtrl::SetTextContent(const TCHAR* szContent, BOOL bResetSelection)
{
	CReSaveCaret* pSave = NULL;

	// save caret position
	if (!bResetSelection)
	{
		pSave = new CReSaveCaret(*this);
	}

	SendMessage(WM_SETTEXT, 0, (LPARAM)szContent);

	// restore caret
	if (!bResetSelection)
	{
		delete pSave;
	}
	else // or reset
	{
		SetSel(0, 0);
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CToDoCommentsCtrl, CUrlRichEditCtrl)
	//{{AFX_MSG_MAP(CToDoCommentsCtrl)
	ON_WM_CONTEXTMENU()
	ON_WM_SETCURSOR()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_COMMENTS_CUT, ID_COMMENTS_FINDREPLACE, OnCommentsMenuCmd)
	ON_UPDATE_COMMAND_UI_RANGE(ID_COMMENTS_CUT, ID_COMMENTS_FINDREPLACE, OnUpdateCommentsMenuCmd)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChangeText)
	ON_CONTROL_REFLECT_EX(EN_KILLFOCUS, OnKillFocus)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_SETWORDWRAP, OnSetWordWrap)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToDoCommentsCtrl message handlers

BOOL CToDoCommentsCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	dwStyle |= ES_AUTOHSCROLL | WS_HSCROLL | ES_NOHIDESEL;

	return CUrlRichEditCtrl::Create(dwStyle, rect, pParentWnd, nID);
}

LRESULT CToDoCommentsCtrl::OnSetFont(WPARAM wp, LPARAM lp)
{
	// richedit2.0 sends a EN_CHANGE notification if it contains
	// text when it receives a font change.
	// to us though this is a bogus change so we prevent a notification
	// being sent
	CAutoFlag af(m_bAllowNotify, FALSE);

	return CUrlRichEditCtrl::OnSetFont(wp, lp);
}

BOOL CToDoCommentsCtrl::OnChangeText()
{
	CUrlRichEditCtrl::OnChangeText();

	if (m_bAllowNotify && IsWindowEnabled() && !(GetStyle() & ES_READONLY))
	{
		GetParent()->SendMessage(WM_TDCN_COMMENTSCHANGE);
	}

	return FALSE;
}

BOOL CToDoCommentsCtrl::OnKillFocus()
{
	if (m_bAllowNotify)
	{
		GetParent()->SendMessage(WM_TDCN_COMMENTSKILLFOCUS);
	}

	return FALSE;
}

void CToDoCommentsCtrl::SetReadOnly(bool bReadOnly)
{
	CUrlRichEditCtrl::SetReadOnly(bReadOnly);

	SetBackgroundColor(!bReadOnly, GetSysColor(COLOR_3DFACE));
}

void CToDoCommentsCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd == this)
	{
		// build the context menu ourselves
		// note: we could use the edit menu from user32.dll but because
		// we're adding items too, the languages would appear odd
		CMenu menu;

		if (menu.LoadMenu(IDR_MISC))
		{
			CMenu* pPopup = menu.GetSubMenu(4);

			if (pPopup)
			{
				CToolbarHelper::PrepareMenuItems(pPopup, this);
				pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, point.x, point.y, this);
			}
		}
	}
}

void CToDoCommentsCtrl::OnCommentsMenuCmd(UINT nCmdID)
{
	switch (nCmdID)
	{
	case ID_COMMENTS_UNDO:
		CTextDocument(GetSafeHwnd()).Undo();
		break;

	case ID_COMMENTS_REDO:
		SendMessage(EM_REDO);
		break;

	case ID_COMMENTS_CUT:
		Cut();
		break;

	case ID_COMMENTS_FIND:
		DoEditFind(IDS_FIND_TITLE);
		break;

	case ID_COMMENTS_FINDREPLACE:
		DoEditReplace(IDS_REPLACE_TITLE);
		break;

	case ID_COMMENTS_COPY:
		Copy();
		break;

	case ID_COMMENTS_PASTE:
		Paste();
		break;

	case ID_COMMENTS_PASTEASREF:
		{
			// try to get the clipboard for any tasklist
			ITaskList* pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, FALSE);

			// verify that we can get the corresponding filename
			CString sFileName;
			ITaskList4* pClip4 = GetITLInterface<ITaskList4>(pClipboard, IID_TASKLIST4);

			if (pClip4)
			{
				sFileName = pClip4->GetAttribute(ATL::CT2A(TDL_FILENAME));
				sFileName.Replace(_T(" "), _T("%20"));
			}
			else // get the clipboard for just this tasklist
			{
				pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, TRUE);
			}

			if (pClipboard && pClipboard->GetFirstTask())
			{
				// build single string containing each top level item as a different ref
				CString sRefs, sRef;
				HTASKITEM hClip = pClipboard->GetFirstTask();

				while (hClip)
				{
					if (sFileName.IsEmpty())
					{
						sRef.Format(_T(" %s%d"), TDL_PROTOCOL, pClipboard->GetTaskID(hClip));
					}
					else
					{
						sRef.Format(_T(" %s%s?%d"), TDL_PROTOCOL, (LPCTSTR)sFileName, pClipboard->GetTaskID(hClip));
					}

					sRefs += sRef;

					hClip = pClipboard->GetNextTask(hClip);
				}

				sRefs += ' ';
				ReplaceSel(sRefs, TRUE);
			}
		}
		break;

	case ID_COMMENTS_SELECTALL:
		SetSel(0, -1);
		break;

	case ID_COMMENTS_OPENURL:
		if (m_nContextUrl != -1)
		{
			GoToUrl(m_nContextUrl);
		}

		// reset
		m_nContextUrl = -1;
		break;

	case ID_COMMENTS_FILEBROWSE:
		{
			CString sFile;

			if (m_nContextUrl != -1)
			{
				sFile = GetUrl(m_nContextUrl, TRUE);

				if (FileMisc::FileExists(sFile))
				{
					sFile.Empty();
				}
			}

			CFileDialog dialog(TRUE, NULL, sFile);
			dialog.m_ofn.lpstrTitle = _T("Select File");

			if (dialog.DoModal() == IDOK)
			{
				PathReplaceSel(dialog.GetPathName(), TRUE);
			}

			// reset
			m_nContextUrl = -1;
		}
		break;

	case ID_COMMENTS_DATESTAMP_LOCAL:
		{
			COleDateTime date = COleDateTime::GetCurrentTime();
			ReplaceSel(date.Format(), TRUE);
		}
		break;

	case ID_COMMENTS_SPELLCHECK:
		GetParent()->PostMessage(WM_ICC_WANTSPELLCHECK);
		break;

	case ID_COMMENTS_WORDWRAP:
		SetWordWrap(!m_bWordWrap);
		break;
	}
}

void CToDoCommentsCtrl::SetWordWrap(BOOL bWrap)
{
	SetTargetDevice(NULL, bWrap ? 0 : 1);
	m_bWordWrap = bWrap;
}

void CToDoCommentsCtrl::OnUpdateCommentsMenuCmd(CCmdUI* pCmdUI)
{
	BOOL bReadOnly = (GetStyle() & ES_READONLY) || !IsWindowEnabled();

	switch (pCmdUI->m_nID)
	{
	case ID_COMMENTS_UNDO:
		pCmdUI->Enable(CanUndo());
		break;

	case ID_COMMENTS_REDO:
		pCmdUI->Enable(SendMessage(EM_CANREDO));
		break;

	case ID_COMMENTS_CUT:
		if (!bReadOnly)
		{
			CHARRANGE crSel;
			GetSel(crSel);
			pCmdUI->Enable(crSel.cpMin != crSel.cpMax);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
		break;

	case ID_COMMENTS_COPY:
		{
			CHARRANGE crSel;
			GetSel(crSel);
			pCmdUI->Enable(crSel.cpMin != crSel.cpMax);
		}
		break;

	case ID_COMMENTS_PASTE:
		pCmdUI->Enable(!bReadOnly && CanPaste());
		break;

	case ID_COMMENTS_PASTEASREF:
		pCmdUI->Enable(!bReadOnly && !IsTDLClipboardEmpty());
		break;

	case ID_COMMENTS_SELECTALL:
		pCmdUI->Enable(GetTextLength());
		break;

	case ID_COMMENTS_SPELLCHECK:
		pCmdUI->Enable(GetTextLength() && !bReadOnly);
		break;

	case ID_COMMENTS_OPENURL:
		// if pCmdUI->m_pMenu is NOT null then we need to set the menu
		// text to the url, else it's just MFC checking that the option
		// is stil enabled
		pCmdUI->Enable(m_nContextUrl != -1);

		if (m_nContextUrl != -1 && pCmdUI->m_pMenu)
		{
			CString sText, sMenu;
			pCmdUI->m_pMenu->GetMenuString(ID_COMMENTS_OPENURL, sMenu, MF_BYCOMMAND);

			// restrict url length to 250 pixels
			CEnString sUrl(GetUrl(m_nContextUrl, TRUE));
			CClientDC dc(this);
			sUrl.FormatDC(&dc, 250, ES_PATH);

			sText.Format(_T("%s: %s"), sMenu, sUrl);
			pCmdUI->SetText(sText);
		}
		break;

	case ID_COMMENTS_FILEBROWSE:
		pCmdUI->Enable(!bReadOnly);
		break;

	case ID_COMMENTS_DATESTAMP_LOCAL:
		pCmdUI->Enable(!bReadOnly);
		break;

	case ID_COMMENTS_WORDWRAP:
		pCmdUI->SetCheck(m_bWordWrap ? 1 : 0);
		break;
	}
}

BOOL CToDoCommentsCtrl::Paste()
{
	if (Misc::ClipboardHasFormat(CF_HDROP, *this))
	{
		if (::OpenClipboard(*this))
		{
			HANDLE hData = ::GetClipboardData(CF_HDROP);
			ASSERT(hData);

			CStringArray aFiles;
			Misc::GetDropFilePaths((HDROP)hData, aFiles);

			::CloseClipboard();

			if (aFiles.GetSize())
			{
				return CRichEditHelper::PasteFiles(*this, aFiles, REP_ASFILEURL);
			}
		}
	}

	// else
	CUrlRichEditCtrl::Paste();
	return TRUE;
}

BOOL CToDoCommentsCtrl::CanPaste()
{
	// for reasons that I'm not entirely clear on even if we
	// return that CF_HDROP is okay, the richedit itself will
	// veto the drop. So I'm experimenting with handling this ourselves
	if (Misc::ClipboardHasFormat(CF_HDROP, *this))
	{
		return TRUE;
	}

	return CUrlRichEditCtrl::CanPaste(CF_TEXT);
}


BOOL CToDoCommentsCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// bit of a hack but this is what we get just before the context
	// menu appears so we set the cursor back to the arrow
	if (nHitTest == HTCAPTION)
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}

	return CUrlRichEditCtrl::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CToDoCommentsCtrl::IsTDLClipboardEmpty() const
{
	// try for any clipboard first
	ITaskList* pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, FALSE);
	ITaskList4* pClip4 = GetITLInterface<ITaskList4>(pClipboard, IID_TASKLIST4);

	if (pClip4)
	{
		return (pClipboard->GetFirstTask() == NULL);
	}

	// else try for 'our' clipboard only
	return (!GetParent()->SendMessage(WM_TDCM_HASCLIPBOARD, 0, TRUE));
}

LRESULT CToDoCommentsCtrl::SendNotifyCustomUrl(LPCTSTR szUrl) const
{
	CString sUrl(szUrl);
	sUrl.MakeLower();

	if (sUrl.Find(TDL_PROTOCOL) != -1 || sUrl.Find(TDL_EXTENSION) != -1)
	{
		return GetParent()->SendMessage(WM_TDCM_TASKLINK, 0, (LPARAM)(LPCTSTR)sUrl);
	}

	return 0;
}

void CToDoCommentsCtrl::PreSubclassWindow()
{
	EnableToolTips();

	CUrlRichEditCtrl::PreSubclassWindow();
}

void CToDoCommentsCtrl::SavePreferences(IPreferences* pPrefs, LPCTSTR szKey) const
{
	pPrefs->WriteProfileInt(szKey, _T("WordWrap"), m_bWordWrap);
}

void CToDoCommentsCtrl::LoadPreferences(const IPreferences* pPrefs, LPCTSTR szKey)
{
	BOOL bWordWrap = pPrefs->GetProfileInt(szKey, _T("WordWrap"), TRUE);

	// we need to post the wordwrap initialization else the richedit
	// get very confused about something and doesn't repaint properly
	// when resizing
	PostMessage(WM_SETWORDWRAP, bWordWrap, (LPARAM)GetSafeHwnd());
}

LRESULT CToDoCommentsCtrl::OnSetWordWrap(WPARAM wp, LPARAM lp)
{
	UNREFERENCED_PARAMETER(lp);
	ASSERT(lp == (LPARAM)GetSafeHwnd());

	SetWordWrap(wp);
	return 0L;
}

int CToDoCommentsCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUrlRichEditCtrl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	CAutoFlag af(m_bAllowNotify, FALSE); // else we can get a false edit change

	// set max edit length
	LimitText(1024 * 1024 * 1024); // one gigabyte

	return 0;
}

bool CToDoCommentsCtrl::ProcessMessage(MSG* pMsg)
{
	// process editing shortcuts
	if (pMsg->message == WM_KEYDOWN)
	{
		BOOL bCtrl = Misc::KeyIsPressed(VK_CONTROL);
		BOOL bAlt = Misc::KeyIsPressed(VK_MENU);

		if (bCtrl && !bAlt)
		{
			switch (pMsg->wParam)
			{
			case _T('c'):
			case _T('C'):
				Copy();
				return TRUE;

			case _T('v'):
			case _T('V'):
				Paste();
				return TRUE;

			case _T('x'):
			case _T('X'):
				Copy();
				Clear();
				return TRUE;

			case _T('a'):
			case _T('A'):
				SetSel(0, -1);
				return TRUE;

			case _T('f'):
			case _T('F'):
				DoEditFind(IDS_FIND_TITLE);
				return TRUE;

			case _T('h'):
			case _T('H'):
				DoEditReplace(IDS_REPLACE_TITLE);
				return TRUE;

			case _T('z'):
			case _T('Z'):
				return TRUE;

			case _T('y'):
			case _T('Y'):
				return TRUE;
			}
		}
		else if (pMsg->wParam == _T('\t'))
		{
			CHARRANGE cr;
			GetSel(cr);

			// if nothing is selected then just insert tabs
			CString sSel = _T("\t");

			if (cr.cpMax > cr.cpMin)
			{
				sSel += GetTextRange(cr);
			}

			// bump selection by 1 to account for the tab
			cr.cpMin++;
			cr.cpMax++;

			ReplaceSel(sSel, TRUE);
			SetSel(cr);

			return TRUE;
		}
	}

	return false;
}
