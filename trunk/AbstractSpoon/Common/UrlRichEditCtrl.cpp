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

// UrlRichEditCtrl.cpp : implementation file
//

#include "StdAfx.h"
#include "UrlRichEditCtrl.h"
#include "../../CodeProject/Source/AutoFlag.h"
#include "../../CodeProject/Source/RichEditHelper.h"
#include "../../CodeProject/Source/WinClasses.h"
#include "../../CodeProject/Source/WClassDefines.h"
#include "../../CodeProject/Source/FileMisc.h"

#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static LPSTR URLDELIMS[] =
{
	" ",
	"\n",
	"\t",
	", ",
	". ",
	//	";",
	//	"[",
	//	"]",
	//	"(",
	//	")"
	"<",
	"{",
	"}",
};

const LPCTSTR FILEPREFIX = "file://";
const CString ENDPUNCTUATION(".,;:(){}[]<>&*~?\\\"'");

enum
{
	TIMER_REPARSE = 1,
};

const UINT PAUSE = 1000; // 1 second
CString CUrlRichEditCtrl::s_sGotoErrMsg;
CString CUrlRichEditCtrl::s_sCtrlClickMsg;

/////////////////////////////////////////////////////////////////////////////
// CUrlRichEditCtrl

CUrlRichEditCtrl::CUrlRichEditCtrl() : m_nContextUrl(-1), m_nFileProtocol(-1)
{
	EnableToolTips();

	AddProtocol("www.", FALSE);
	AddProtocol("http://", FALSE);
	AddProtocol("https://", FALSE);
	AddProtocol("ftp://", FALSE);
	AddProtocol("outlook:", FALSE);
	AddProtocol("mailto:", FALSE);
	AddProtocol("Notes://", FALSE);
	AddProtocol("evernote://", FALSE);
	AddProtocol("onenote:///", FALSE);

	m_nFileProtocol = AddProtocol(FILEPREFIX, FALSE);

	if (s_sCtrlClickMsg.IsEmpty())
		s_sCtrlClickMsg = "Ctrl+Click to follow link";
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
	ON_NOTIFY_RANGE(TTN_NEEDTEXT, 0, 0xffff, OnNeedTooltip)
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

		if (_strnicmp(szText, prot.sProtocol, prot.sProtocol.GetLength()) == 0)
			return nProt;
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
		return 0;

	long nChar = -1;
	GetSel(nChar, nChar);

	CString sText, sFile;
	TCHAR szFileName[_MAX_PATH];
	UINT nFileCount = ::DragQueryFile((HDROP)wp, 0xFFFFFFFF, NULL, 0);

	ASSERT(nFileCount != 0);

	for (UINT i=0; i < nFileCount; i++)
	{
		::DragQueryFile((HDROP)wp, i, szFileName, _MAX_PATH);
		::GetLongPathName(szFileName, szFileName, _MAX_PATH);

		sFile = CreateFileLink(szFileName) + " ";
		sText += sFile;
	}
	::DragFinish((HDROP)wp);

	// set selection to that which we saved during the drag
	SetSel(m_crDropSel);

	if (!sText.IsEmpty())
		PathReplaceSel(sText, FALSE);

	return FALSE;
}

int CUrlRichEditCtrl::CharFromPoint(const CPoint& point)
{
	/*	int nFirstLine = GetFirstVisibleLine();
	int nLineCount = GetLineCount();

	for (int nLine = nFirstLine; nLine < nLineCount; nLine++)
	{
	int nFirstChar = LineIndex(nLine);
	CPoint ptChar = GetCharPos(nFirstChar);
	int nLineHeight = GetLineHeight();

	if (point.y >= ptChar.y && point.y < ptChar.y + nLineHeight)
	{
	int nLineLength = LineLength(nFirstChar);

	for (int nChar = nFirstChar; nChar < (nFirstChar + nLineLength); nChar++)
	{
	ptChar = GetCharPos(nChar);

	if (point.x < ptChar.x)
	return nChar;
	}
	}
	}

	return GetTextLength();
	*/
	POINTL ptl = { point.x, point.y };

	return SendMessage(EM_CHARFROMPOS, 0, (LPARAM)&ptl);
}

void CUrlRichEditCtrl::PreSubclassWindow()
{
	SetEventMask(GetEventMask() | ENM_CHANGE | ENM_DROPFILES | ENM_DRAGDROPDONE );
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
		return TRUE; // end of string

	int nDelims = sizeof(URLDELIMS) / sizeof(LPCTSTR);
	char ch = *szText;

	for (int nDelim = 0; nDelim < nDelims; nDelim++)
	{
		LPCTSTR szDelim = URLDELIMS[nDelim];

		if (ch == szDelim[0])
		{
			// test char after ch if 2 char delim
			if (szDelim[1] == 0 || *(szText + 1) == szDelim[1])
				return TRUE;
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
		sText.Replace("\r\n", "\n");

	// parse the text into an array of URLPOS
	CUrlArray aUrls;
	LPCTSTR szText = sText;
	BOOL bPrevDelim = TRUE;
	int nPos = 0;
	BOOL bBracedFile = FALSE;

	while (*szText)
	{
		BYTE nChar = *szText;

		// if nChar < 0 then its a multibyte char and can't be part
		// of a url, so we bump the text buffer by 2 but the pos by 1
		if (IsDBCSLeadByte(nChar))
		{
			szText++;
			szText++;
			nPos++;
			continue;
		}

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
			bBracedFile = (szText[-1] == '<');
		else
			bBracedFile = FALSE;

		// find the end of the url (URLDELIMS)
		int nLen = 0;
		LPCTSTR szStart = szText;

		if (bBracedFile)
		{
			while (*szText && *szText != '>')
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
			SetRedraw(FALSE);

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

	if (nUrls !=  m_aUrls.GetSize())
		return FALSE;

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
		while  (nLine < nFirst && (nFirst != nPrevFirst));
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
		while  (nLine > nFirst && (nFirst != nPrevFirst));
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
		ASSERT (0);
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
		return FALSE;

	const URLITEM& urli = m_aUrls[nUrl];

	if (!urli.bWantNotify)
	{
		CString sUrl = GetUrl(nUrl, TRUE);

		if (FileMisc::Run(*this, sUrl) > 32)
			return TRUE;

		// else
		if (!s_sGotoErrMsg.IsEmpty())
			AfxMessageBox(s_sGotoErrMsg, MB_OK | MB_ICONEXCLAMATION);

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
			return GoToUrl(nUrl);
	}

	// didn't match then it might be a file
	if (GetFileAttributes(sUrl) != 0xffffffff)
	{
		if (FileMisc::Run(*this, sUrl) > 32)
			return TRUE;

		// else
		if (!s_sGotoErrMsg.IsEmpty())
			AfxMessageBox(s_sGotoErrMsg, MB_OK | MB_ICONEXCLAMATION);
	}

	return FALSE;
}

LRESULT CUrlRichEditCtrl::SendNotifyCustomUrl(LPCTSTR szUrl) const
{
	return GetParent()->SendMessage(WM_UREN_CUSTOMURL, GetDlgCtrlID(), (LPARAM)szUrl);
}

/////////////////////////////////////////////////////////////////////////////
// CUrlRichEditCtrl

HRESULT CUrlRichEditCtrl::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT* lpcfFormat,
														DWORD /*reco*/, BOOL /*fReally*/, HGLOBAL /*hMetaPict*/)
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
			return format;

		if (dataobj.IsDataAvailable(formats[nFmt]))
			return formats[nFmt];
	}

	return CF_TEXT;
}

HRESULT CUrlRichEditCtrl::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	if (!fDrag) // allowable dest effects
	{
		BOOL bEnable = !(GetStyle() & ES_READONLY) && IsWindowEnabled();

		if (!bEnable)
			*pdwEffect = DROPEFFECT_NONE;
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
					dwEffect = DROPEFFECT_COPY;

				else // if ((grfKeyState & MK_SHIFT) == MK_SHIFT)
					dwEffect = DROPEFFECT_MOVE;
			}

			if (dwEffect & *pdwEffect) // make sure allowed type
				*pdwEffect = dwEffect;
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

HRESULT CUrlRichEditCtrl::GetContextMenu(WORD /*seltype*/, LPOLEOBJECT /*lpoleobj*/,
													  CHARRANGE* /*lpchrg*/, HMENU* /*lphmenu*/)
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
	ASSERT (nURL >= 0 && nURL < m_aUrls.GetSize());

	if (nURL >= 0 && nURL < m_aUrls.GetSize())
	{
		CString sUrl(m_aUrls[nURL].sUrl);
		CString sUrlLower(sUrl);
		sUrlLower.MakeLower();

		if (!bAsFile || sUrlLower.Find(FILEPREFIX) == -1)
			return sUrl;

		// else
		if (sUrl.Find('#') ==  -1)
		{
			sUrl = sUrl.Mid(sUrlLower.Find(FILEPREFIX) + lstrlen(FILEPREFIX));
			sUrl.Replace("%20", " ");
			sUrl.Replace('/', '\\');
		}

		return sUrl;
	}

	// else
	return "";
}

CString CUrlRichEditCtrl::CreateFileLink(LPCTSTR szFile)
{
	CString sLink(szFile);

	sLink = FILEPREFIX + sLink;
	sLink.Replace('\\', '/');

	// if the path contains spaces then brace it
	if (sLink.Find(' ') != -1)
		sLink = "<" + sLink + ">";

	return sLink;
}

void CUrlRichEditCtrl::PathReplaceSel(LPCTSTR lpszPath, BOOL bFile)
{
	CString sPath(lpszPath);

	if (bFile || sPath.Find(":\\") != -1 || sPath.Find("\\\\") != -1)
		sPath = CreateFileLink(lpszPath);

	// add space fore and aft depending on selection
	CHARRANGE crSel, crSelOrg;
	GetSel(crSelOrg); // save this off
	GetSel(crSel);

	// enlarge to include end items
	if (crSel.cpMin > 0)
		crSel.cpMin--;

	if (crSel.cpMax < GetTextLength() - 1)
		crSel.cpMax++;

	SetSel(crSel);
	CString sSelText = GetSelText();
	SetSel(crSelOrg);

	// test
	if (!sSelText.IsEmpty())
	{
		if (!isspace(sSelText[0]))
			sPath = ' ' + sPath;

		if (!isspace(sSelText[sSelText.GetLength() - 1]))
			sPath += ' ';
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
			SetSel(nChar, nChar);
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
		return -1;

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
	BOOL bCtrl = (GetKeyState(VK_CONTROL) & 0x8000);
	ENLINK* pENL = (ENLINK*)pNMHDR;

	switch (pENL->msg)
	{
	case WM_SETCURSOR:
		if (!bCtrl)
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
				nCursor = IDC_IBEAM;

			SetCursor(AfxGetApp()->LoadStandardCursor(nCursor));

			*pResult = TRUE;
			return TRUE;
		}
		break;

	case WM_LBUTTONUP:
		if (bCtrl)
		{
			if (GoToUrl(FindUrl(pENL->chrg)))
				return TRUE;
		}

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
			return nUrl;
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
			return nUrl;
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

		//		if (urli.cr.cpMax >= nPos && urli.cr.cpMin < nPos)
		//			return nUrl;

		if (rUrl.PtInRect(point))
			return nUrl;
	}

	// not found
	return -1;
}

BOOL CUrlRichEditCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	return CRichEditHelper::CreateRichEdit20(*this, dwStyle, rect, pParentWnd, nID);
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
	pTI->uId  = nHit;
	pTI->rect = CRect(CPoint(point.x-1,point.y-1),CSize(2,2));
	pTI->uFlags |= TTF_NOTBUTTON | TTF_ALWAYSTIP;
	pTI->lpszText = LPSTR_TEXTCALLBACK;

	return nHit;
}

void CUrlRichEditCtrl::OnNeedTooltip(UINT /*id*/, NMHDR* pNMHDR, LRESULT* pResult)
{
	*pResult = 0;
	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;

	CPoint point(GetMessagePos());
	ScreenToClient(&point);

	if (FindUrlEx(point) != -1)
	{
#if _MSC_VER >= 1400
		strcpy_s(pTTT->szText, s_sCtrlClickMsg);
#else
		strcpy(pTTT->szText, s_sCtrlClickMsg);
#endif
	}
}
