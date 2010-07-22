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
#include "../../../zlib/Source/zlib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTFContentControl

CRTFContentControl::CRTFContentControl() : m_bAllowNotify(TRUE), m_reSpellCheck(m_rtf), m_bPrefsDone(FALSE)
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

BOOL CRTFContentControl::Compress(const unsigned char* pContentSrc, int nLenSrc,
								  unsigned char*& pContentDest, int& nLenDest)
{
	ULONG lDest = (int)(nLenSrc * 1.001) + 12; // zlib requirements
	pContentDest = new unsigned char[lDest + 4]; // extra DWORD for storing the source length

	// leave the first DWORD free
	int nRet = compress(pContentDest + 4, &lDest, pContentSrc, nLenSrc);

	if (nRet != Z_OK)
	{
		// cleanup
		delete [] pContentDest;
		nLenDest = 0;
	}
	else
	{
		nLenDest = lDest + 4;

		// encode the source length into the first 4 bytes
		((DWORD*)pContentDest)[0] = nLenSrc;
	}

	return (nRet == Z_OK);
}

BOOL CRTFContentControl::Decompress(const unsigned char* pContentSrc, int nLenSrc,
									unsigned char*& pContentDest, int& nLenDest)
{
	ASSERT (nLenSrc && strncmp((const char*)pContentSrc, RTFTAG, LENTAG) != 0);

	// get the source length from the first 4 bytes
	ULONG lDest = ((DWORD*)pContentSrc)[0];

	// sanity check
	if (lDest < (ULONG)nLenSrc || lDest > (ULONG)(1000 * nLenSrc))
		return FALSE;

	pContentDest = new unsigned char[lDest];

	int nRet = uncompress(pContentDest, &lDest, pContentSrc + 4, nLenSrc - 4);

	if (nRet != Z_OK)
	{
		delete [] pContentDest;
		nLenDest = 0;
	}
	else
		nLenDest = lDest;

	return (nRet == Z_OK);
}

void CRTFContentControl::OnChangeText()
{
	if (m_bAllowNotify && !GetRichEditCtrl().IsIMEComposing())
		GetParent()->SendMessage(WM_TDCN_COMMENTSCHANGE);
}

void CRTFContentControl::OnKillFocus()
{
	if (m_bAllowNotify)
		GetParent()->SendMessage(WM_TDCN_COMMENTSKILLFOCUS);
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

		if (Compress((unsigned char*)(LPSTR)(LPCTSTR)sContent, nLen, pCompressed, nLenCompressed) && nLenCompressed)
		{
			CopyMemory(pContent, pCompressed, nLenCompressed);
			nLen = nLenCompressed;
			delete [] pCompressed;
		}
		else
			nLen = 0;
	}
	else
		nLen = ((CRTFContentControl*)pCtrl)->GetRTFLength();

	return nLen;
}

bool CRTFContentControl::SetContent(unsigned char* pContent, int nLength)
{
	unsigned char* pDecompressed = NULL;

	// content may need decompressing
	if (nLength && strncmp((const char*)pContent, RTFTAG, LENTAG) != 0)
	{
		int nLenDecompressed = 0;

		if (Decompress(pContent, nLength, pDecompressed, nLenDecompressed))
		{
			pContent = pDecompressed;
			nLength = nLenDecompressed;
		}
		else
			return false;
	}

	// content must begin with rtf tag or be empty
	if (nLength && (nLength < LENTAG || strncmp((const char*)pContent, RTFTAG, LENTAG)))
		return false;

	CAutoFlag af(m_bAllowNotify, FALSE);
	CString sContent((LPCSTR)pContent, nLength);
	CReSaveCaret re(GetRichEditCtrl());

	SetRTF(sContent);
	GetRichEditCtrl().ParseAndFormatText(TRUE);

	delete [] pDecompressed;

	return true;
}

int CRTFContentControl::GetTextContent(char* szContent, int nLength) const
{
	if (!szContent)
		return GetWindowTextLength();

	// else
	if (nLength == -1)
		nLength = lstrlen(szContent);

	GetWindowText(szContent, nLength);
	return nLength;
}

bool CRTFContentControl::SetTextContent(const char* szContent)
{
	CAutoFlag af(m_bAllowNotify, TRUE);
	CReSaveCaret re(GetRichEditCtrl());

	SendMessage(WM_SETTEXT, 0, (LPARAM)szContent);
	return true;
}

void CRTFContentControl::SetUITheme(const UITHEME* pTheme)
{
	m_toolbar.SetBackgroundColors(pTheme->crToolbarLight, pTheme->crToolbarDark, pTheme->nStyle == UIS_GRADIENT);
	m_ruler.SetBackgroundColor(pTheme->crToolbarLight);
}

void CRTFContentControl::SetPreferenceLocation(const char* szKey)
{
	m_sPrefKey = szKey;

	// restore toolbar and ruler state first time only
	if (!m_bPrefsDone)
	{
		BOOL bShowToolbar = TRUE, bShowRuler = TRUE, bWordWrap = TRUE;

		if (!m_sPrefKey.IsEmpty() && !m_bPrefsDone)
		{
			bShowToolbar = AfxGetApp()->GetProfileInt(m_sPrefKey, "ShowToolbar", m_showToolbar);
			bShowRuler = AfxGetApp()->GetProfileInt(m_sPrefKey, "ShowRuler", m_showRuler);
			bWordWrap = AfxGetApp()->GetProfileInt(m_sPrefKey, "WordWrap", m_bWordWrap);

			m_bPrefsDone = TRUE;
		}

		ShowToolbar(bShowToolbar);
		ShowRuler(bShowRuler);
		SetWordWrap(bWordWrap);
	}
}

HWND CRTFContentControl::GetHwnd() const
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	return GetSafeHwnd();
}

const char* CRTFContentControl::GetTypeID() const
{
	static CString sID;

	Misc::GuidToString(RTF_TYPEID, sID);

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

					sText.Format("%s: %s", sMenu, re.GetUrl(nUrl, TRUE));
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
//						ITaskList4* pClip4 = NULL;

//						if (S_OK == pClipboard->QueryInterface(IID_TASKLIST4, (void**)&pClip4))
						if (pClip4)
						{
							sFileName = pClip4->GetAttribute(TDL_FILENAME);
							sFileName.Replace(" ", "%20");
						}
						else // get the clipboard for just this tasklist
							pClipboard = (ITaskList*)GetParent()->SendMessage(WM_TDCM_GETCLIPBOARD, 0, TRUE);

						if (pClipboard && pClipboard->GetFirstTask())
						{
							// build single string containing each top level item as a different ref
							CString sRefs, sRef;
							HTASKITEM hClip = pClipboard->GetFirstTask();

							while (hClip)
							{
								if (sFileName.IsEmpty())
									sRef.Format(" %s%d", TDL_PROTOCOL, pClipboard->GetTaskID(hClip));
								else
									sRef.Format(" %s%s?%d", TDL_PROTOCOL, sFileName, pClipboard->GetTaskID(hClip));

								sRefs += sRef;

								hClip = pClipboard->GetNextTask(hClip);
							}

							sRefs += ' ';
							re.ReplaceSel(sRefs, TRUE);
						}
					}
					break;

				case ID_EDIT_DELETE:
					re.ReplaceSel("");
					break;

				case ID_EDIT_SELECT_ALL:
					re.SetSel(0, -1);
					break;

				case ID_EDIT_OPENURL:
					if (nUrl != -1)
						re.GoToUrl(nUrl);
					break;

				case ID_EDIT_FILEBROWSE:
					{
						CString sFile;

						if (nUrl != -1)
						{
							sFile = re.GetUrl(nUrl, TRUE);

							if (GetFileAttributes(sFile) == 0xffffffff)
								sFile.Empty();
						}

						CFileDialog dialog(TRUE, NULL, sFile);
						dialog.m_ofn.lpstrTitle = "Select File";

						if (dialog.DoModal() == IDOK)
							re.PathReplaceSel(dialog.GetPathName(), TRUE);
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

//	if (S_OK == pClipboard->QueryInterface(IID_TASKLIST4, (void**)&pClip4))
	if (pClip4)
		return (pClipboard->GetFirstTask() == NULL);

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

	for( long i=0;  i<formats_count;  ++i )
		bCanPaste |= re.CanPaste( formats[i] );

	return bCanPaste;
}

int CRTFContentControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CAutoFlag af(m_bAllowNotify, FALSE);

	if (CRulerRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

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
			BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
			BOOL bAlt = (GetKeyState(VK_MENU) & 0x8000);
			BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);

			CRulerRichEdit& re = GetRichEditCtrl();

			if (bCtrl && !bAlt)
			{

				switch (pMsg->wParam)
				{
				case 'c':
				case 'C':
					re.Copy();
					return TRUE;

				case 'v':
				case 'V':
					re.Paste(bShift);
					return TRUE;

				case 'x':
				case 'X':
					re.Cut();
					return TRUE;

				case 'a':
				case 'A':
					re.SetSel(0, -1);
					return TRUE;

				case 'b':
				case 'B':
					DoBold();
					return TRUE;

				case 'i':
				case 'I':
					DoItalic();
					return TRUE;

				case 'u':
				case 'U':
					DoUnderline();
					return TRUE;

				case 'l':
				case 'L':
					DoLeftAlign();
					return TRUE;

				case 'e':
				case 'E':
					DoCenterAlign();
					return TRUE;

				case 'r':
				case 'R':
					DoRightAlign();
					return TRUE;

				case 0xBD:
					DoStrikethrough();
					return TRUE;

				case 'f':
				case 'F':
					re.DoEditFind();
					return TRUE;

				case 'h':
				case 'H':
					re.DoEditReplace();
					return TRUE;

				case 'z':
				case 'Z':
					return TRUE; // to prevent the richedit performing the undo

				case 'y':
				case 'Y':
					return TRUE; // to prevent the richedit performing the redo
				}
			}
			else
			{
				switch (pMsg->wParam)
				{
				case '\t':
					{
						CHARRANGE cr;
						re.GetSel(cr);

						// if nothing is selected then just insert tabs
						if (cr.cpMin == cr.cpMax)
							re.ReplaceSel("\t");
						else
						{
							if (!bShift)
								DoIndent();
							else
								DoOutdent();
						}
					}
					return TRUE;
				}
			}
		}
		break;
	}

	return false;
}

void CRTFContentControl::OnDestroy()
{
	CRulerRichEditCtrl::OnDestroy();

	// save toolbar and ruler state
	if (!m_sPrefKey.IsEmpty())
	{
		AfxGetApp()->WriteProfileInt(m_sPrefKey, "ShowToolbar", IsToolbarVisible());
		AfxGetApp()->WriteProfileInt(m_sPrefKey, "ShowRuler", IsRulerVisible());
		AfxGetApp()->WriteProfileInt(m_sPrefKey, "WordWrap", HasWordWrap());
	}
}

void CRTFContentControl::OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	if (nStyleType == GWL_EXSTYLE && (lpStyleStruct->styleNew & WS_EX_CLIENTEDGE))
		lpStyleStruct->styleNew &= ~WS_EX_CLIENTEDGE;

	CRulerRichEditCtrl::OnStyleChanging(nStyleType, lpStyleStruct);
}

LRESULT CRTFContentControl::OnCustomUrl(WPARAM wp, LPARAM lp)
{
	UNREFERENCED_PARAMETER(wp);
	ASSERT (wp == RTF_CONTROL);

	CString sUrl((LPCTSTR)lp);
	int nFind = sUrl.Find(TDL_PROTOCOL);

	if (nFind != -1)
		return GetParent()->SendMessage(WM_TDCM_TASKLINK, 0, lp);

	return 0;

}

BOOL CRTFContentControl::PreTranslateMessage(MSG* pMsg)
{
	return CRulerRichEditCtrl::PreTranslateMessage(pMsg);
}
