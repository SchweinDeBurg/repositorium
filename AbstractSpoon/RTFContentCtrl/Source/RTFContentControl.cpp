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
// - adjusted #include's paths
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
// - merged with ToDoList versions 6.1.2-6.1.7 sources
//*****************************************************************************

// RTFContentControl.cpp : implementation file
//

#include "StdAfx.h"
#include "RTFContentCtrlApp.h"
#include "RTFContentControl.h"

#include "../../Common/ITaskList.h"
#include "../../../CodeProject/Source/AutoFlag.h"
#include "../../ToDoList/Source/TDCMsg.h"
#include "../../../CodeProject/Source/RichEditHelper.h"
#include "../../ToDoList/Source/TDLSchemaDef.h"
#include "../../../CodeProject/Source/Misc.h"
#include "../../Common/UITheme.h"
#include "../../../CodeProject/Source/EnString.h"
#include "../../Common/Preferences.h"

#include "../../../CodeProject/Source/Compression.h"
#include "../../../zlib/Source/zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTFContentControl

CRTFContentControl::CRTFContentControl() : m_bAllowNotify(TRUE), m_reSpellCheck(m_rtf)
{
	// add custom protocol to comments field for linking to task IDs
	GetRichEditCtrl().AddProtocol(TDL_PROTOCOL, TRUE);

	GetRichEditCtrl().SetCtrlClickMsg(CEnString(IDS_COMMENTSCTRLCLICKMSG));
	GetRichEditCtrl().SetGotoErrorMsg(CEnString(IDS_COMMENTSGOTOERRMSG));
}

CRTFContentControl::~CRTFContentControl()
{
}

BEGIN_MESSAGE_MAP(CRTFContentControl, CRulerRichEditCtrl)
	//{{AFX_MSG_MAP(CRTFContentControl)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(RTF_CONTROL, OnChangeText)
	ON_EN_KILLFOCUS(RTF_CONTROL, OnKillFocus)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_STYLECHANGING()
	ON_REGISTERED_MESSAGE(WM_UREN_CUSTOMURL, OnCustomUrl)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTFContentControl message handlers

void CRTFContentControl::OnChangeText()
{
	if (m_bAllowNotify && !GetRichEditCtrl().IsIMEComposing())
	{
		GetParent()->SendMessage(WM_TDCN_COMMENTSCHANGE);
	}
}

void CRTFContentControl::OnKillFocus()
{
	if (m_bAllowNotify)
	{
		GetParent()->SendMessage(WM_TDCN_COMMENTSKILLFOCUS);
	}
}

LRESULT CRTFContentControl::OnSetFont(WPARAM wp, LPARAM lp)
{
	// richedit2.0 sends a EN_CHANGE notification if it contains
	// text when it receives a font change.
	// to us though this is a bogus change so we prevent a notification
	// being sent
	CAutoFlag af(m_bAllowNotify, FALSE);

	return CRulerRichEditCtrl::OnSetFont(wp, lp);
}

// ICustomControl implementation
int CRTFContentControl::GetContent(unsigned char* pContent) const
{
	return GetContent(this, pContent);
}

// hack to get round GetRTF not being const
int CRTFContentControl::GetContent(const CRTFContentControl* pCtrl, unsigned char* pContent)
{
	int nLen = 0;

	if (pContent)
	{
		CString sContent;

		// cast away constness
		sContent = ((CRTFContentControl*)pCtrl)->GetRTF();
		nLen = sContent.GetLength();

		// compress it
		unsigned char* pCompressed = NULL;
		int nLenCompressed = 0;

		if (Compression::Compress((unsigned char*)(LPSTR)(LPCTSTR)sContent, nLen, pCompressed, nLenCompressed) && nLenCompressed)
		{
			CopyMemory(pContent, pCompressed, nLenCompressed);
			nLen = nLenCompressed;
			delete [] pCompressed;
		}
		else
		{
			nLen = 0;
		}
	}
	else
	{
		nLen = ((CRTFContentControl*)pCtrl)->GetRTFLength();
	}

	return nLen;
}

bool CRTFContentControl::SetContent(unsigned char* pContent, int nLength, BOOL bResetSelection)
{
	unsigned char* pDecompressed = NULL;

	// content may need decompressing
	if (nLength && strncmp((const char*)pContent, RTFTAG, LENTAG) != 0)
	{
		int nLenDecompressed = 0;

		if (Compression::Decompress(pContent, nLength, pDecompressed, nLenDecompressed))
		{
			pContent = pDecompressed;
			nLength = nLenDecompressed;
		}
		else
		{
			return false;
		}
	}

	// content must begin with rtf tag or be empty
	if (nLength && (nLength < LENTAG || strncmp((const char*)pContent, RTFTAG, LENTAG)))
	{
		return false;
	}

	CAutoFlag af(m_bAllowNotify, FALSE);
	CString sContent((LPCSTR)pContent, nLength);
	CReSaveCaret* pSave = NULL;

	// save caret position
	if (!bResetSelection)
	{
		pSave = new CReSaveCaret(GetRichEditCtrl());
	}

	SetRTF(sContent);
	GetRichEditCtrl().ParseAndFormatText(TRUE);

	delete [] pDecompressed;

	// restore caret
	if (!bResetSelection)
	{
		delete pSave;
	}
	else // or reset
	{
		GetRichEditCtrl().SetSel(0, 0);
	}

	return true;
}

int CRTFContentControl::GetTextContent(TCHAR* szContent, int nLength) const
{
	if (!szContent)
	{
		return GetWindowTextLength();
	}

	// else
	if (nLength == -1)
	{
		nLength = _tcslen(szContent);
	}

	GetWindowText(szContent, nLength);
	return nLength;
}

bool CRTFContentControl::SetTextContent(const TCHAR* szContent, BOOL bResetSelection)
{
	CAutoFlag af(m_bAllowNotify, TRUE);
	CReSaveCaret* pSave = NULL;

	// save caret position
	if (!bResetSelection)
	{
		pSave = new CReSaveCaret(GetRichEditCtrl());
	}

	SendMessage(WM_SETTEXT, 0, (LPARAM)szContent);

	// restore caret
	if (!bResetSelection)
	{
		delete pSave;
	}
	else // or reset
	{
		GetRichEditCtrl().SetSel(0, 0);
	}

	return true;
}

void CRTFContentControl::SetUITheme(const UITHEME* pTheme)
{
	m_toolbar.SetBackgroundColors(pTheme->crToolbarLight, pTheme->crToolbarDark, pTheme->nStyle == UIS_GRADIENT);
	m_ruler.SetBackgroundColor(pTheme->crToolbarLight);
}

HWND CRTFContentControl::GetHwnd() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return GetSafeHwnd();
}

const char* CRTFContentControl::GetTypeID() const
{
	static CStringA sID;

#if defined(UNICODE) || defined(_UNICODE)
	CString strTemp;
	Misc::GuidToString(RTF_TYPEID, strTemp);
	sID = ATL::CT2A(strTemp);
#else
	Misc::GuidToString(RTF_TYPEID, sID);
#endif   // UNICODE || _UNICODE

	return sID;
}

void CRTFContentControl::SetReadOnly(bool bReadOnly)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CRulerRichEditCtrl::SetReadOnly((BOOL)bReadOnly);
}

void CRTFContentControl::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	DestroyWindow();
	delete this;
}

void CRTFContentControl::EnableMenuItem(CMenu* pMenu, UINT nCmdID, BOOL bEnable)
{
	pMenu->EnableMenuItem(nCmdID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
}

void CRTFContentControl::CheckMenuItem(CMenu* pMenu, UINT nCmdID, BOOL bCheck)
{
	pMenu->CheckMenuItem(nCmdID, MF_BYCOMMAND | (bCheck ? MF_CHECKED  : MF_UNCHECKED));
}

void CRTFContentControl::OnContextMenu(CWnd* pWnd, CPoint point)
{
	if (pWnd == &GetRichEditCtrl())
	{
		// prepare a simple edit menu
		CMenu menu;

		if (menu.LoadMenu(IDR_POPUP))
		{
			CMenu* pPopup = menu.GetSubMenu(0);

			if (pPopup)
			{
				CRulerRichEdit& re = GetRichEditCtrl();

				BOOL bCanEdit = re.IsWindowEnabled() && !(re.GetStyle() & ES_READONLY);
				BOOL bHasText = re.GetTextLength();

				CHARRANGE cr;
				re.GetSel(cr);

				BOOL bHasSel = (cr.cpMax - cr.cpMin);

				CharFormat	cf;
				cf.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_COLOR;
				re.GetSelectionCharFormat(cf);

				CheckMenuItem(pPopup, BUTTON_BOLD, (cf.dwEffects & CFE_BOLD));
				CheckMenuItem(pPopup, BUTTON_ITALIC, (cf.dwEffects & CFE_ITALIC));
				CheckMenuItem(pPopup, BUTTON_UNDERLINE, (cf.dwEffects & CFE_UNDERLINE));
				CheckMenuItem(pPopup, BUTTON_STRIKETHRU, (cf.dwEffects & CFE_STRIKEOUT));

				CheckMenuItem(pPopup, ID_EDIT_SUPERSCRIPT, (cf.dwEffects & CFE_SUPERSCRIPT));
				CheckMenuItem(pPopup, ID_EDIT_SUBSCRIPT, (cf.dwEffects & CFE_SUBSCRIPT));

				EnableMenuItem(pPopup, BUTTON_BOLD, bCanEdit);
				EnableMenuItem(pPopup, BUTTON_ITALIC, bCanEdit);
				EnableMenuItem(pPopup, BUTTON_UNDERLINE, bCanEdit);
				EnableMenuItem(pPopup, BUTTON_STRIKETHRU, bCanEdit);

				EnableMenuItem(pPopup, ID_EDIT_SUPERSCRIPT, bCanEdit);
				EnableMenuItem(pPopup, ID_EDIT_SUBSCRIPT, bCanEdit);

				EnableMenuItem(pPopup, ID_EDIT_CUT, bCanEdit && bHasSel);
				EnableMenuItem(pPopup, ID_EDIT_COPY, bHasSel);
				EnableMenuItem(pPopup, ID_EDIT_PASTE, bCanEdit && CanPaste());
				EnableMenuItem(pPopup, ID_EDIT_PASTESIMPLE, bCanEdit && CanPaste());
				EnableMenuItem(pPopup, ID_EDIT_DELETE, bCanEdit && bHasSel);
				EnableMenuItem(pPopup, ID_EDIT_SELECT_ALL, bHasText);

				EnableMenuItem(pPopup, ID_EDIT_PASTEASREF, bCanEdit && !IsClipboardEmpty());

#ifndef _DEBUG
				EnableMenuItem(pPopup, ID_EDIT_HORZRULE, FALSE);
#endif

				int nUrl = re.GetContextUrl();
				EnableMenuItem(pPopup, ID_EDIT_OPENURL, nUrl != -1);

				if (nUrl != -1)
				{
					CString sText, sMenu;
					pPopup->GetMenuString(ID_EDIT_OPENURL, sMenu, MF_BYCOMMAND);

					sText.Format(_T("%s: %s"), sMenu, re.GetUrl(nUrl, TRUE));
					pPopup->ModifyMenu(ID_EDIT_OPENURL, MF_BYCOMMAND, ID_EDIT_OPENURL, sText);
				}

				EnableMenuItem(pPopup, ID_EDIT_FILEBROWSE, bCanEdit);
				EnableMenuItem(pPopup, ID_EDIT_INSERTDATESTAMP, bCanEdit);
				EnableMenuItem(pPopup, ID_EDIT_SPELLCHECK, bCanEdit && bHasText);

				EnableMenuItem(pPopup, ID_EDIT_FIND, bHasText);
				EnableMenuItem(pPopup, ID_EDIT_FINDREPLACE, bCanEdit && bHasText);

				CheckMenuItem(pPopup, ID_EDIT_SHOWTOOLBAR, IsToolbarVisible());
				CheckMenuItem(pPopup, ID_EDIT_SHOWRULER, IsRulerVisible());
				CheckMenuItem(pPopup, ID_EDIT_WORDWRAP, HasWordWrap());

				// check pos
				if (point.x == -1 && point.y == -1)
				{
					point = GetCaretPos();
					::ClientToScreen(m_rtf, &point);
				}

				UINT nCmdID = ::TrackPopupMenu(*pPopup, TPM_RETURNCMD | TPM_LEFTALIGN | TPM_LEFTBUTTON,
					point.x, point.y, 0, *this, NULL);

				switch (nCmdID)
				{
				case BUTTON_BOLD:
				case BUTTON_ITALIC:
				case BUTTON_UNDERLINE:
				case BUTTON_STRIKETHRU:
					SendMessage(WM_COMMAND, nCmdID);
					break;

				case ID_EDIT_SUPERSCRIPT:
					DoSuperscript();
					break;

				case ID_EDIT_SUBSCRIPT:
					DoSubscript();
					break;

				case ID_EDIT_COPY:
					re.Copy();
					break;

				case ID_EDIT_CUT:
					re.Cut();
					break;

				case ID_EDIT_FIND:
					re.DoEditFind(IDS_FIND_TITLE);
					break;

				case ID_EDIT_HORZRULE:
					//InsertHorizontalRule();
					break;

				case ID_EDIT_FINDREPLACE:
					re.DoEditReplace(IDS_REPLACE_TITLE);
					break;

				case ID_EDIT_PASTE:
					re.Paste();
					break;

				case ID_EDIT_PASTESIMPLE:
					re.Paste(TRUE); // TRUE ==  simple
					break;

				case ID_EDIT_PASTEASREF:
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
									sRef.Format(_T(" %s%s?%d"), TDL_PROTOCOL, sFileName, pClipboard->GetTaskID(hClip));
								}

								sRefs += sRef;

								hClip = pClipboard->GetNextTask(hClip);
							}

							sRefs += ' ';
							re.ReplaceSel(sRefs, TRUE);
						}
					}
					break;

				case ID_EDIT_DELETE:
					re.ReplaceSel(_T(""));
					break;

				case ID_EDIT_SELECT_ALL:
					re.SetSel(0, -1);
					break;

				case ID_EDIT_OPENURL:
					if (nUrl != -1)
					{
						re.GoToUrl(nUrl);
					}
					break;

				case ID_EDIT_FILEBROWSE:
					{
						CString sFile;

						if (nUrl != -1)
						{
							sFile = re.GetUrl(nUrl, TRUE);

							if (GetFileAttributes(sFile) == 0xffffffff)
							{
								sFile.Empty();
							}
						}

						CFileDialog dialog(TRUE, NULL, sFile);
						dialog.m_ofn.lpstrTitle = _T("Select File");

						if (dialog.DoModal() == IDOK)
						{
							re.PathReplaceSel(dialog.GetPathName(), TRUE);
						}
					}
					break;

				case ID_EDIT_INSERTDATESTAMP:
					{
						COleDateTime date = COleDateTime::GetCurrentTime();
						re.ReplaceSel(date.Format(), TRUE);
					}
					break;

				case ID_EDIT_SPELLCHECK:
					GetParent()->PostMessage(WM_ICC_WANTSPELLCHECK);
					break;

				case ID_EDIT_SHOWTOOLBAR:
					ShowToolbar(!IsToolbarVisible());
					break;

				case ID_EDIT_SHOWRULER:
					ShowRuler(!IsRulerVisible());
					break;

				case ID_EDIT_WORDWRAP:
					SetWordWrap(!HasWordWrap());
					break;
				}
			}
		}
	}
}

BOOL CRTFContentControl::IsClipboardEmpty() const
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

BOOL CRTFContentControl::CanPaste()
{
	static CLIPFORMAT formats[] =
	{
		(CLIPFORMAT)::RegisterClipboardFormat(CF_RTF),
		(CLIPFORMAT)::RegisterClipboardFormat(CF_RETEXTOBJ),
		(CLIPFORMAT)::RegisterClipboardFormat(_T("Embedded Object")),
		(CLIPFORMAT)::RegisterClipboardFormat(_T("Rich Text Format")),
		CF_BITMAP,
		CF_TEXT,
		CF_DIB,
		CF_UNICODETEXT,
		CF_HDROP
	};
	const long formats_count = sizeof(formats) / sizeof(CLIPFORMAT);

	CUrlRichEditCtrl& re = GetRichEditCtrl();

	BOOL bCanPaste = FALSE;

	for (long i = 0;  i < formats_count;  ++i)
	{
		bCanPaste |= re.CanPaste(formats[i]);
	}

	return bCanPaste;
}

int CRTFContentControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CAutoFlag af(m_bAllowNotify, FALSE);

	if (CRulerRichEditCtrl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	GetRichEditCtrl().SetEventMask(GetRichEditCtrl().GetEventMask() | ENM_CHANGE);

	// set max edit length
	GetRichEditCtrl().LimitText(1024 * 1024 * 1024); // one gigabyte

	m_tbHelper.Initialize(&m_toolbar, this);

	return 0;
}

bool CRTFContentControl::ProcessMessage(MSG* pMsg)
{
	// process editing shortcuts
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState());

			BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
			BOOL bAlt = (GetKeyState(VK_MENU) & 0x8000);
			BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);

			CRulerRichEdit& re = GetRichEditCtrl();

			if (bCtrl && !bAlt)
			{
				switch (pMsg->wParam)
				{
				case _T('c'):
				case _T('C'):
					re.Copy();
					return TRUE;

				case _T('v'):
				case _T('V'):
					re.Paste(bShift);
					return TRUE;

				case _T('x'):
				case _T('X'):
					re.Cut();
					return TRUE;

				case _T('a'):
				case _T('A'):
					re.SetSel(0, -1);
					return TRUE;

				case _T('b'):
				case _T('B'):
					DoBold();
					return TRUE;

				case _T('i'):
				case _T('I'):
					DoItalic();
					return TRUE;

				case _T('u'):
				case _T('U'):
					DoUnderline();
					return TRUE;

				case _T('l'):
				case _T('L'):
					DoLeftAlign();
					return TRUE;

				case _T('e'):
				case _T('E'):
					DoCenterAlign();
					return TRUE;

				case _T('r'):
				case _T('R'):
					DoRightAlign();
					return TRUE;

				case 0xBD:
					DoStrikethrough();
					return TRUE;

				case _T('f'):
				case _T('F'):
					re.DoEditFind();
					return TRUE;

				case _T('h'):
				case _T('H'):
					re.DoEditReplace();
					return TRUE;

				case _T('z'):
				case _T('Z'):
					return TRUE; // to prevent the richedit performing the undo

				case _T('y'):
				case _T('Y'):
					return TRUE; // to prevent the richedit performing the redo

				case _T('j'):
				case _T('J'):
					if (bShift)
					{
						DoOutdent();
					}
					else
					{
						DoIndent();
					}
					return TRUE;
				}
			}
			else
			{
				switch (pMsg->wParam)
				{
				case _T('\t'):
					{
						CHARRANGE cr;
						re.GetSel(cr);

						// if nothing is selected then just insert tabs
						if (cr.cpMin == cr.cpMax)
						{
							re.ReplaceSel(_T("\t"));
						}
						else
						{
							if (!bShift)
							{
								DoIndent();
							}
							else
							{
								DoOutdent();
							}
						}
						return TRUE;
					}
					break;
				}
			}
		}
		break;
	}

	return false;
}

void CRTFContentControl::SavePreferences(IPreferences* pPrefs, LPCTSTR szKey) const
{
	pPrefs->WriteProfileInt(szKey, _T("ShowToolbar"), IsToolbarVisible());
	pPrefs->WriteProfileInt(szKey, _T("ShowRuler"), IsRulerVisible());
	pPrefs->WriteProfileInt(szKey, _T("WordWrap"), HasWordWrap());
}

void CRTFContentControl::LoadPreferences(const IPreferences* pPrefs, LPCTSTR szKey)
{
	BOOL bShowToolbar = pPrefs->GetProfileInt(szKey, _T("ShowToolbar"), m_showToolbar);
	BOOL bShowRuler = pPrefs->GetProfileInt(szKey, _T("ShowRuler"), m_showRuler);
	BOOL bWordWrap = pPrefs->GetProfileInt(szKey, _T("WordWrap"), TRUE);

	ShowToolbar(bShowToolbar);
	ShowRuler(bShowRuler);
	SetWordWrap(bWordWrap);
}

void CRTFContentControl::OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	if (nStyleType == GWL_EXSTYLE && (lpStyleStruct->styleNew & WS_EX_CLIENTEDGE))
	{
		lpStyleStruct->styleNew &= ~WS_EX_CLIENTEDGE;
	}

	CRulerRichEditCtrl::OnStyleChanging(nStyleType, lpStyleStruct);
}

LRESULT CRTFContentControl::OnCustomUrl(WPARAM wp, LPARAM lp)
{
	UNREFERENCED_PARAMETER(wp);
	ASSERT(wp == RTF_CONTROL);

	CString sUrl((LPCTSTR)lp);
	int nFind = sUrl.Find(TDL_PROTOCOL);

	if (nFind != -1)
	{
		return GetParent()->SendMessage(WM_TDCM_TASKLINK, 0, lp);
	}

	return 0;
}
