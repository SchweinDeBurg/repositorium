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
// - merged with ToDoList version 6.1.2-6.2.3 sources
//*****************************************************************************

// UrlRichEditCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "UrlRichEditCtrl.h"
#include "../../CodeProject/Source/AutoFlag.h"
#include "../../CodeProject/Source/RichEditHelper.h"
#include "../../CodeProject/Source/WinClasses.h"
#include "../../CodeProject/Source/WClassDefines.h"
#include "../../CodeProject/Source/FileMisc.h"
#include "../../CodeProject/Source/Misc.h"

#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPTSTR URLDELIMS[] =
{
	_T(" "),
	_T("\n"),
	_T("\t"),
	_T(", "),
	_T(". "),
	//_T(";)",
	//_T("["),
	//_T("]"),
	//_T("("),
	//_T(")"),
	_T("<"),
	_T("{"),
	_T("}")
};

const LPCTSTR FILEPREFIX = _T("file://");
const CString ENDPUNCTUATION(_T(".,;:(){}[]<>&*~?\\\"\'"));
const UINT PAUSE = 1000; // 1 second

enum
{
	TIMER_REPARSE = 1,
};

CString CUrlRichEditCtrl::s_sGotoErrMsg;

/////////////////////////////////////////////////////////////////////////////
// CUrlRichEditCtrl

CUrlRichEditCtrl::CUrlRichEditCtrl():
m_nContextUrl(-1),
m_nFileProtocol(-1)
{
	EnableToolTips();

	AddProtocol(_T("www."), FALSE);
	AddProtocol(_T("http://"), FALSE);
	AddProtocol(_T("https://"), FALSE);
	AddProtocol(_T("ftp://"), FALSE);
	AddProtocol(_T("outlook:"), FALSE);
	AddProtocol(_T("mailto:"), FALSE);
	AddProtocol(_T("Notes://"), FALSE);
	AddProtocol(_T("evernote://"), FALSE);
	AddProtocol(_T("onenote:///"), FALSE);
	AddProtocol(_T("excel:"), FALSE);
	AddProtocol(_T("winword:"), FALSE);

	m_nFileProtocol = AddProtocol(FILEPREFIX, FALSE);
}

CUrlRichEditCtrl::~CUrlRichEditCtrl()
{
}

BEGIN_MESSAGE_MAP(CUrlRichEditCtrl, CRichEditBaseCtrl)
	//{{AFX_MSG_MAP(CUrlRichEditCtrl)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChangeText)
	ON_WM_CHAR()
	ON_WM_RBUTTONUP()
	ON_WM_KEYUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_WM_SYSKEYDOWN()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_MESSAGE(WM_DROPFILES, OnDropFiles)
	ON_NOTIFY_REFLECT_EX(EN_LINK, OnNotifyLink)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUrlRichEditCtrl message handlers

int CUrlRichEditCtrl::AddProtocol(LPCTSTR szProtocol, BOOL bWantNotify)
{
	int nExist = MatchProtocol(szProtocol);

	if (nExist == -1)
	{
		PROTOCOL prot(szProtocol, bWantNotify);
		return m_aProtocols.Add(prot);
	}

	return nExist;
}

int CUrlRichEditCtrl::MatchProtocol(LPCTSTR szText) const
{
	int nProt = m_aProtocols.GetSize();

	while (nProt--)
	{
		const PROTOCOL& prot = m_aProtocols[nProt];

		if (_tcsnicmp(szText, prot.sProtocol, prot.sProtocol.GetLength()) == 0)
		{
			return nProt;
		}
	}

	return -1;
}

BOOL CUrlRichEditCtrl::OnChangeText()
{
	// kill any existing timer
	KillTimer(TIMER_REPARSE);

	// and start a new one
	SetTimer(TIMER_REPARSE, PAUSE, NULL);

	return FALSE;
}

LRESULT CUrlRichEditCtrl::OnDropFiles(WPARAM wp, LPARAM /*lp*/)
{
	CHARRANGE crSelOrg;
	GetSel(crSelOrg); // save this off
	BOOL bEnable = !(GetStyle() & ES_READONLY) && IsWindowEnabled();

	if (!bEnable)
	{
		return 0;
	}

	CStringArray aFiles;
	CString sText;

	SetSel(m_crDropSel);

	if (Misc::GetDropFilePaths((HDROP)wp, aFiles))
	{
		::CloseClipboard();
		return CRichEditHelper::PasteFiles(*this, aFiles, REP_ASFILEURL);
	}

	return FALSE;
}

int CUrlRichEditCtrl::CharFromPoint(const CPoint& point)
{
	POINTL ptl = { point.x, point.y };

	return SendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
}

void CUrlRichEditCtrl::PreSubclassWindow()
{
	SetEventMask(GetEventMask() | ENM_CHANGE | ENM_DROPFILES | ENM_DRAGDROPDONE);
	DragAcceptFiles();

	// enable multilevel undo
	SendMessage(EM_SETTEXTMODE, TM_MULTILEVELUNDO);

	m_ncBorder.Initialize(GetSafeHwnd());

	CRichEditBaseCtrl::PreSubclassWindow();
}

LRESULT CUrlRichEditCtrl::OnSetText(WPARAM /*wp*/, LPARAM lp)
{
	// eat duplicate messages
	CString sText;
	GetWindowText(sText);

	BOOL bChange = (sText != (LPCTSTR)lp);
	LRESULT lr = 0;

	if (bChange)
	{
		CRichEditHelper::ClearUndo(GetSafeHwnd());

		lr = Default();
		ParseAndFormatText(TRUE);
	}

	return lr;
}

LRESULT CUrlRichEditCtrl::OnSetFont(WPARAM /*wp*/, LPARAM /*lp*/)
{
	LRESULT lr = Default();

	ParseAndFormatText(TRUE);

	return lr;
}

BOOL CUrlRichEditCtrl::IsDelim(LPCTSTR szText)
{
	if (!szText || !*szText)
	{
		return TRUE;   // end of string
	}

	int nDelims = sizeof(URLDELIMS) / sizeof(LPCTSTR);
	TCHAR ch = *szText;

	for (int nDelim = 0; nDelim < nDelims; nDelim++)
	{
		LPCTSTR szDelim = URLDELIMS[nDelim];

		if (ch == szDelim[0])
		{
			// test char after ch if 2 char delim
			if (szDelim[1] == 0 || *(szText + 1) == szDelim[1])
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CUrlRichEditCtrl::ParseAndFormatText(BOOL bForceReformat)
{
	AF_NOREENTRANT // prevent reentrancy

	// parse the control content
	CString sText;
	GetWindowText(sText);

	// richedit2 uses '\r\n' whereas richedit uses just '\n'
	if (!CWinClasses::IsClass(*this, WC_RICHEDIT))
	{
		sText.Replace(_T("\r\n"), _T("\n"));
	}

	// parse the text into an array of URLPOS
	CUrlArray aUrls;
	LPCTSTR szText = sText;
	BOOL bPrevDelim = TRUE;
	int nPos = 0;
	BOOL bBracedFile = FALSE;

	while (*szText)
	{
#if defined(UNICODE) || defined(_UNICODE)
		// no-op
#else
		TCHAR nChar = *szText;

		// if nChar < 0 then its a multibyte char and can't be part
		// of a url, so we bump the text buffer by 2 but the pos by 1
		if (IsDBCSLeadByte(nChar))
		{
			szText++;
			szText++;
			nPos++;
			continue;
		}
#endif   // UNICODE || _UNICODE

		// if the previous item was not a delimiter then there's no
		// point checking for a protocol match so we just update the
		// value of bPrevDelim for the current char
		if (!bPrevDelim)
		{
			bPrevDelim = IsDelim(szText);
			szText++;
			nPos++;
			continue;
		}
		// if the current char is a delim then this can't be the start
		// of a url either
		else if (IsDelim(szText))
		{
			bPrevDelim = TRUE;
			szText++;
			nPos++;
			continue;
		}

		// now check for a protocol
		int nProt = MatchProtocol(szText);

		// if no match then increment pos and go to next char
		if (nProt == -1)
		{
			bPrevDelim = FALSE;
			szText++;
			nPos++;
			continue;
		}

		// check for <file://.....>
		if (nProt == m_nFileProtocol && nPos > 0)
		{
			bBracedFile = (szText[-1] == _T('<'));
		}
		else
		{
			bBracedFile = FALSE;
		}

		// find the end of the url (URLDELIMS)
		int nLen = 0;
		LPCTSTR szStart = szText;

		if (bBracedFile)
		{
			while (*szText && *szText != _T('>'))
			{
				szText++;
				nLen++;
			}
		}
		else
		{
			while (!IsDelim(szText))
			{
				szText++;
				nLen++;
			}
		}

		bPrevDelim = TRUE;

		// save the result
		URLITEM urli;
		urli.cr.cpMin = nPos;
		urli.cr.cpMax = urli.cr.cpMin + nLen;
		nPos += nLen;

		// make sure the url does not end in a punctuation mark
		while (ENDPUNCTUATION.Find(szStart[nLen - 1]) != -1)
		{
			nLen--;
			urli.cr.cpMax--;
		}

		urli.sUrl = CString(szStart, nLen);
		urli.bWantNotify = m_aProtocols[nProt].bWantNotify;

		InsertInOrder(urli, aUrls);
	}

	// compare aUrls with m_aUrls to see if anything has changed
	BOOL bReformat = !sText.IsEmpty() && (bForceReformat || !UrlsMatch(aUrls));

	// then overwrite (always)
	m_aUrls.Copy(aUrls);

	if (bReformat)
	{
		BOOL bVisible = IsWindowVisible();
		CRePauseUndo rep(*this);

		if (bVisible)
		{
			SetRedraw(FALSE);
		}

		// save current selection
		CHARRANGE crSel;
		GetSel(crSel);

		// and first line
		int nFirstLine = GetFirstVisibleLine();

		// retrieve default character attribs
		CHARFORMAT cf;
		cf.cbSize = sizeof(cf);
		cf.dwMask = CFM_LINK;

		// format urls
		int nUrls = aUrls.GetSize();
		CHARRANGE cr = { 0, 0 };

		for (int nUrl = 0; nUrl < nUrls; nUrl++)
		{
			// clear formatting from the end of the previous
			// url to the start of this url
			cr.cpMax = aUrls[nUrl].cr.cpMin;
			cf.dwEffects = 0;

			SetSel(cr);
			SetSelectionCharFormat(cf);

			// update for next url
			cr.cpMin = aUrls[nUrl].cr.cpMax;

			// then format url
			cf.dwEffects = CFM_LINK;

			SetSel(aUrls[nUrl].cr);
			SetSelectionCharFormat(cf);
		}

		// clear formatting for whatever's left
		cr.cpMax = -1;
		cf.dwEffects = 0;

		SetSel(cr);
		SetSelectionCharFormat(cf);

		// restore selection
		SetSel(crSel);

		// and first line
		SetFirstVisibleLine(nFirstLine);

		if (bVisible)
		{
			SetRedraw(TRUE);
			Invalidate(FALSE);
		}
	}
}

BOOL CUrlRichEditCtrl::UrlsMatch(const CUrlArray& aUrls)
{
	int nUrls = aUrls.GetSize();

	if (nUrls != m_aUrls.GetSize())
	{
		return FALSE;
	}

	// compare aUrls with m_aUrls to see if anything has changed
	for (int nUrl = 0; nUrl < nUrls; nUrl++)
	{
		if (aUrls[nUrl].cr.cpMin != m_aUrls[nUrl].cr.cpMin ||
			aUrls[nUrl].cr.cpMax != m_aUrls[nUrl].cr.cpMax)
		{
			return FALSE;
		}
	}

	// no change
	return TRUE;
}

void CUrlRichEditCtrl::InsertInOrder(URLITEM& urli, CUrlArray& aUrls)
{
	// work backwards looking for first item that comes before us
	int nUrl = aUrls.GetSize();

	while (nUrl--)
	{
		if (aUrls[nUrl].cr.cpMin < urli.cr.cpMin)
		{
			aUrls.InsertAt(nUrl + 1, urli);
			return;
		}
	}

	// else insert at start
	aUrls.InsertAt(0, urli);
}

void CUrlRichEditCtrl::SetFirstVisibleLine(int nLine)
{
	int nFirst = GetFirstVisibleLine();

	if (nLine < nFirst)
	{
		int nPrevFirst;

		do
		{
			LineScroll(-1);

			nPrevFirst = nFirst;
			nFirst = GetFirstVisibleLine();
		}
		while (nLine < nFirst && (nFirst != nPrevFirst));
	}
	else if (nLine > nFirst)
	{
		int nPrevFirst;

		do
		{
			LineScroll(1);

			nPrevFirst = nFirst;
			nFirst = GetFirstVisibleLine();
		}
		while (nLine > nFirst && (nFirst != nPrevFirst));
	}
}

int CUrlRichEditCtrl::GetLineHeight() const
{
	CHARFORMAT cf;
	GetDefaultCharFormat(cf);

	// convert height in Twips to pixels
	int nTwipsPerInch = 1440;
	HDC hdc = ::GetDC(NULL);
	int nPixelsPerInch = GetDeviceCaps(hdc, LOGPIXELSY);
	::ReleaseDC(NULL, hdc);

	return (cf.yHeight * nPixelsPerInch) / nTwipsPerInch + 2;
}

CPoint CUrlRichEditCtrl::GetCaretPos()
{
	if (GetFocus() != this)
	{
		ASSERT(0);
		return CPoint(0, 0);
	}

	CPoint ptCaret = CWnd::GetCaretPos();
	ptCaret.y += GetLineHeight() / 2; // half line height
	ptCaret.x += 4; // estaimate 1/2 char width

	return ptCaret;
}

BOOL CUrlRichEditCtrl::GoToUrl(int nUrl) const
{
	if (nUrl < 0 || nUrl >= m_aUrls.GetSize())
	{
		return FALSE;
	}

	const URLITEM& urli = m_aUrls[nUrl];

	if (!urli.bWantNotify)
	{
		CString sUrl = GetUrl(nUrl, TRUE);

		// if it fails to run then forward on to parent
		if (FileMisc::Run(*this, sUrl) > 32)
		{
			return TRUE;
		}

		// else
		if (!SendNotifyCustomUrl(sUrl) && !s_sGotoErrMsg.IsEmpty())
		{
			AfxMessageBox(s_sGotoErrMsg, MB_OK | MB_ICONEXCLAMATION);
		}

		return FALSE;
	}

	// else
	SendNotifyCustomUrl(urli.sUrl);
	return TRUE;
}

BOOL CUrlRichEditCtrl::GoToUrl(const CString& sUrl) const
{
	int nUrl = m_aUrls.GetSize();

	while (nUrl--)
	{
		const URLITEM& urli = m_aUrls[nUrl];

		if (urli.sUrl.CompareNoCase(sUrl) == 0)
		{
			return GoToUrl(nUrl);
		}
	}

	// didn't match then it might be a file
	if (FileMisc::FileExists(sUrl))
	{
		if (FileMisc::Run(*this, sUrl) > 32)
		{
			return TRUE;
		}

		// else
		if (!s_sGotoErrMsg.IsEmpty())
		{
			AfxMessageBox(s_sGotoErrMsg, MB_OK | MB_ICONEXCLAMATION);
		}
	}

	return FALSE;
}

LRESULT CUrlRichEditCtrl::SendNotifyCustomUrl(LPCTSTR szUrl) const
{
	return GetParent()->SendMessage(WM_UREN_CUSTOMURL, GetDlgCtrlID(), (LPARAM)szUrl);
}

/////////////////////////////////////////////////////////////////////////////
// CUrlRichEditCtrl

HRESULT CUrlRichEditCtrl::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT* lpcfFormat, DWORD /*reco*/,
	BOOL /*fReally*/, HGLOBAL /*hMetaPict*/)
{
	BOOL bEnable = !(GetStyle() & ES_READONLY) && IsWindowEnabled();

	if (bEnable)
	{
		*lpcfFormat = GetAcceptableClipFormat(lpdataobj, *lpcfFormat);
		return S_OK;
	}

	// else
	return E_FAIL;
}

CLIPFORMAT CUrlRichEditCtrl::GetAcceptableClipFormat(LPDATAOBJECT lpDataOb, CLIPFORMAT format)
{
	CLIPFORMAT formats[] =
	{
		CF_HDROP,
		CF_TEXT,
	};

	const long nNumFmts = sizeof(formats) / sizeof(CLIPFORMAT);

	COleDataObject dataobj;
	dataobj.Attach(lpDataOb, FALSE);

	for (int nFmt = 0; nFmt < nNumFmts; nFmt++)
	{
		if (format && format == formats[nFmt])
		{
			return format;
		}

		if (dataobj.IsDataAvailable(formats[nFmt]))
		{
			return formats[nFmt];
		}
	}

	return CF_TEXT;
}

HRESULT CUrlRichEditCtrl::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	if (!fDrag) // allowable dest effects
	{
		BOOL bEnable = !(GetStyle() & ES_READONLY) && IsWindowEnabled();

		if (!bEnable)
		{
			*pdwEffect = DROPEFFECT_NONE;
		}
		else
		{
			DWORD dwEffect = DROPEFFECT_NONE;
			BOOL bFileDrop = ((*pdwEffect & DROPEFFECT_LINK) == DROPEFFECT_LINK);

			// we can deduce (I think) that what's being dragged is a file
			// by whether pdwEffect include the LINK effect.

			// if so save off the current selection pos (for now) because it gets reset
			// when the files are dropped
			if (bFileDrop)
			{
				dwEffect = DROPEFFECT_MOVE;

				// keep track of cursor
				TrackDragCursor();
			}
			else // it's text
			{
				if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
				{
					dwEffect = DROPEFFECT_COPY;
				}

				else // if ((grfKeyState & MK_SHIFT) == MK_SHIFT)
				{
					dwEffect = DROPEFFECT_MOVE;
				}
			}

			if (dwEffect & *pdwEffect) // make sure allowed type
			{
				*pdwEffect = dwEffect;
			}
		}
	}

	return S_OK;
}

void CUrlRichEditCtrl::TrackDragCursor()
{
	// also track the cursor for the drop position
	CPoint ptCursor(::GetMessagePos());

	ScreenToClient(&ptCursor);
	int nChar = CharFromPoint(ptCursor);
	m_crDropSel.cpMin = m_crDropSel.cpMax = nChar;

	SetFocus();
	SetSel(m_crDropSel);
	ShowCaret();
}

HRESULT CUrlRichEditCtrl::GetContextMenu(WORD /*seltype*/, LPOLEOBJECT /*lpoleobj*/, CHARRANGE* /*lpchrg*/,
	HMENU* /*lphmenu*/)
{
	CPoint point = m_ptContextMenu;

	// send on as a simple context menu message
	SendMessage(WM_CONTEXTMENU, (WPARAM)GetSafeHwnd(), MAKELPARAM(point.x, point.y));
	return E_NOTIMPL;
}

/////////////////////////////////////////////////////////////////////////////


void CUrlRichEditCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CRichEditBaseCtrl::OnChar(nChar, nRepCnt, nFlags);
}

CString CUrlRichEditCtrl::GetUrl(int nURL, BOOL bAsFile) const
{
	ASSERT(nURL >= 0 && nURL < m_aUrls.GetSize());

	if (nURL >= 0 && nURL < m_aUrls.GetSize())
	{
		CString sUrl(m_aUrls[nURL].sUrl);
		CString sUrlLower(sUrl);
		sUrlLower.MakeLower();

		if (!bAsFile || sUrlLower.Find(FILEPREFIX) == -1)
		{
			return sUrl;
		}

		// else
		if (sUrl.Find(_T('#')) == -1)
		{
			sUrl = sUrl.Mid(sUrlLower.Find(FILEPREFIX) + lstrlen(FILEPREFIX));
			sUrl.Replace(_T("%20"), _T(" "));
			sUrl.Replace(_T('/'), _T('\\'));
		}

		return sUrl;
	}

	// else
	return _T("");
}

CString CUrlRichEditCtrl::CreateFileLink(LPCTSTR szFile)
{
	CString sLink(szFile);

	sLink = FILEPREFIX + sLink;
	sLink.Replace(_T('\\'), _T('/'));

	// if the path contains spaces then brace it
	if (sLink.Find(_T(' ')) != -1)
	{
		sLink = _T("<") + sLink + _T(">");
	}

	return sLink;
}

void CUrlRichEditCtrl::PathReplaceSel(LPCTSTR lpszPath, BOOL bFile)
{
	CString sPath(lpszPath);

	if (bFile || sPath.Find(_T(":\\")) != -1 || sPath.Find(_T("\\\\")) != -1)
	{
		sPath = CreateFileLink(lpszPath);
	}

	// add space fore and aft depending on selection
	CHARRANGE crSel, crSelOrg;
	GetSel(crSelOrg); // save this off
	GetSel(crSel);

	// enlarge to include end items
	if (crSel.cpMin > 0)
	{
		crSel.cpMin--;
	}

	if (crSel.cpMax < GetTextLength() - 1)
	{
		crSel.cpMax++;
	}

	SetSel(crSel);
	CString sSelText = GetSelText();
	SetSel(crSelOrg);

	// test
	if (!sSelText.IsEmpty())
	{
		if (!isspace(sSelText[0]))
		{
			sPath = _T(' ') + sPath;
		}

		if (!isspace(sSelText[sSelText.GetLength() - 1]))
		{
			sPath += _T(' ');
		}
	}

	ReplaceSel(sPath, TRUE);
	ParseAndFormatText();

	// set the new selection to be the dropped text
	SetSel(crSelOrg.cpMin, crSelOrg.cpMin + sPath.GetLength());
	SetFocus();
}

void CUrlRichEditCtrl::OnRButtonUp(UINT nHitTest, CPoint point)
{
	m_ptContextMenu = point;
	ClientToScreen(&m_ptContextMenu);

	CRichEditBaseCtrl::OnRButtonUp(nHitTest, point);
}

void CUrlRichEditCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_APPS)
	{
		m_ptContextMenu = GetCaretPos();

		// does this location lie on a url?
		m_nContextUrl = FindUrl(m_ptContextMenu);

		// convert point to screen coords
		ClientToScreen(&m_ptContextMenu);
	}

	CRichEditBaseCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CUrlRichEditCtrl::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_F10 && (::GetKeyState(VK_SHIFT) & 0x8000))
	{
		m_ptContextMenu = GetCaretPos();

		// does this location lie on a url?
		m_nContextUrl = FindUrl(m_ptContextMenu);

		// convert point to screen coords
		ClientToScreen(&m_ptContextMenu);

		// eat message else we'll get a WM_KEYUP with VK_APPS
	}

	CRichEditBaseCtrl::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CUrlRichEditCtrl::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_nContextUrl = -1;

	// move the caret to the pos clicked
	int nChar = CharFromPoint(point);

	if (nChar >= 0)
	{
		// don't reset the selection if the character
		// falls within the current selection
		CHARRANGE crSel;
		GetSel(crSel);

		if (nChar < crSel.cpMin || nChar > crSel.cpMax)
		{
			SetSel(nChar, nChar);
		}
	}

	CRichEditBaseCtrl::OnRButtonDown(nFlags, point);
}

void CUrlRichEditCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CRichEditBaseCtrl::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}

int CUrlRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CRichEditBaseCtrl::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	SetEventMask(GetEventMask() | ENM_CHANGE | ENM_DROPFILES | ENM_DRAGDROPDONE | ENM_LINK);
	DragAcceptFiles();

	// enable multilevel undo
	SendMessage(EM_SETTEXTMODE, TM_MULTILEVELUNDO);

	m_ncBorder.Initialize(GetSafeHwnd());

	return 0;
}

void CUrlRichEditCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	// if we arrived here then it means that noone had derived
	// from us and handled OnContextMenu. sow e must forward to
	// our parent else we'll end up in a recursive loop
	GetParent()->SendMessage(WM_CONTEXTMENU, (WPARAM)GetSafeHwnd(), MAKELPARAM(point.x, point.y));
}

BOOL CUrlRichEditCtrl::OnNotifyLink(NMHDR* pNMHDR, LRESULT* pResult)
{
	BOOL bShift = (GetKeyState(VK_SHIFT) & 0x8000);
	ENLINK* pENL = (ENLINK*)pNMHDR;

	switch (pENL->msg)
	{
	case WM_SETCURSOR:
		if (bShift)
		{
			// because we're overriding the default behaviour we need to
			// handle the cursor being over a selected block
			CHARRANGE crSel;
			GetSel(crSel);

			CPoint ptCursor(GetMessagePos());
			ScreenToClient(&ptCursor);

			LPCTSTR nCursor = IDC_ARROW;

			int nChar = CharFromPoint(ptCursor);

			if (nChar < crSel.cpMin || nChar > crSel.cpMax)
			{
				nCursor = IDC_IBEAM;
			}

			SetCursor(AfxGetApp()->LoadStandardCursor(nCursor));

			*pResult = TRUE;
			return TRUE;
		}
		break;

	case WM_LBUTTONDOWN:
		if (!bShift)
		{
			m_nContextUrl = FindUrl(pENL->chrg);

			// might be a hidden link
			if (m_nContextUrl == -1)
			{
				//CString sUrl = GetTextRange(pENL->chrg);
			}
		}
		break;

	case WM_LBUTTONUP:
		if (bShift || FindUrl(pENL->chrg) != m_nContextUrl)
		{
			m_nContextUrl = -1;
		}

		if (GoToUrl(m_nContextUrl))
		{
			m_nContextUrl = -1;
			return TRUE;
		}
		else if (m_nContextUrl == -1)
		{
			CString sUrl = GetTextRange(pENL->chrg);

			if (sUrl.IsEmpty())
			{
				if (FileMisc::Run(*this, sUrl) > 32)
				{
					return TRUE;
				}
			}
		}
		break;

	case WM_RBUTTONUP:
		m_nContextUrl = FindUrl(pENL->chrg);
		break;
	}

	return FALSE;
}

int CUrlRichEditCtrl::FindUrl(const CHARRANGE& cr)
{
	int nUrl = m_aUrls.GetSize();

	while (nUrl--)
	{
		const URLITEM& urli = m_aUrls[nUrl];

		if (urli.cr.cpMax == cr.cpMax && urli.cr.cpMin == cr.cpMin)
		{
			return nUrl;
		}
	}

	// not found
	return -1;
}

int CUrlRichEditCtrl::FindUrl(const CPoint& point)
{
	int nPos = CharFromPoint(point);

	int nUrl = m_aUrls.GetSize();

	while (nUrl--)
	{
		const URLITEM& urli = m_aUrls[nUrl];

		if (urli.cr.cpMax >= nPos && urli.cr.cpMin < nPos)
		{
			return nUrl;
		}
	}

	// not found
	return -1;
}

int CUrlRichEditCtrl::FindUrlEx(const CPoint& point)
{
	int nUrl = m_aUrls.GetSize();

	while (nUrl--)
	{
		const URLITEM& urli = m_aUrls[nUrl];

		CRect rUrl(GetCharPos(urli.cr.cpMin), GetCharPos(urli.cr.cpMax));

		rUrl.bottom += GetLineHeight();

		if (rUrl.PtInRect(point))
		{
			return nUrl;
		}
	}

	// not found
	return -1;
}

BOOL CUrlRichEditCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	if (!CRichEditHelper::CreateRichEdit50(*this, dwStyle, rect, pParentWnd, nID))
	{
		return CRichEditHelper::CreateRichEdit20(*this, dwStyle, rect, pParentWnd, nID);
	}

	// else
	return TRUE; // CreateRichEdit50
}

void CUrlRichEditCtrl::OnTimer(UINT nIDEvent)
{
	// if we've arrived here then it means that the user
	// has paused for long enough to reparse the latest changes
	if (nIDEvent == TIMER_REPARSE)
	{
		KillTimer(TIMER_REPARSE);
		ParseAndFormatText();
	}

	CRichEditBaseCtrl::OnTimer(nIDEvent);
}

int CUrlRichEditCtrl::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	int nHit = MAKELONG(point.x, point.y);
	pTI->hwnd = m_hWnd;
	pTI->uId = nHit;
	pTI->rect = CRect(CPoint(point.x - 1, point.y - 1), CSize(2, 2));
	pTI->uFlags |= TTF_NOTBUTTON | TTF_ALWAYSTIP;
	pTI->lpszText = LPSTR_TEXTCALLBACK;

	return nHit;
}

