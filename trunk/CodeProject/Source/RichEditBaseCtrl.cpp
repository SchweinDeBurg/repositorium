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
// - taken out from the original ToDoList package for better sharing
//*****************************************************************************

// RichEditBaseCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RichEditBaseCtrl.h"
#include "RichEditHelper.h"

#include <atlconv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_FINDREPLACE = ::RegisterWindowMessage(FINDMSGSTRING);

/////////////////////////////////////////////////////////////////////////////
// CRichEditBaseCtrl

CRichEditBaseCtrl::CRichEditBaseCtrl()
{
	m_callback.SetOwner(this);
}

CRichEditBaseCtrl::~CRichEditBaseCtrl()
{
	m_callback.Release();
}


BEGIN_MESSAGE_MAP(CRichEditBaseCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRichEditBaseCtrl)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_FINDREPLACE, OnFindReplaceCmd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRichEditBaseCtrl message handlers

int CRichEditBaseCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	SetOLECallback(&m_callback);

	return 0;
}

void CRichEditBaseCtrl::PreSubclassWindow() 
{
	SetOLECallback(&m_callback);

	CRichEditCtrl::PreSubclassWindow();
}

BOOL CRichEditBaseCtrl::Undo()
{
	return CTextDocument(GetSafeHwnd()).Undo();
}

BOOL CRichEditBaseCtrl::Redo()
{
	return CTextDocument(GetSafeHwnd()).Redo();
}

CRichEditBaseCtrl::CRichEditOleCallback::CRichEditOleCallback() : m_pOwner(NULL)
{
	m_pStorage = NULL;
	m_iNumStorages = 0;
	m_dwRef = 0;

	// set up OLE storage
	HRESULT hResult = ::StgCreateDocfile(NULL, STGM_TRANSACTED | 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DELETEONRELEASE,
		0, &m_pStorage );

	if ( m_pStorage == NULL ||
		hResult != S_OK )
	{
		AfxThrowOleException( hResult );
	}
}

CRichEditBaseCtrl::CRichEditOleCallback::~CRichEditOleCallback()
{
	if (m_pStorage)
	{
		m_pStorage->Release();
		m_pStorage = NULL;
	}
}


HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::GetNewStorage(LPSTORAGE* lplpstg)
{
	m_iNumStorages++;
	WCHAR tName[50];

#if _MSC_VER >= 1400
	swprintf_s(tName, 50, L"REOLEStorage%d", m_iNumStorages);
#else
	swprintf(tName, L"REOLEStorage%d", m_iNumStorages);
#endif

	HRESULT hResult = m_pStorage->CreateStorage(tName, 
		STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE ,
		0, 0, lplpstg );

	if (hResult != S_OK )
	{
		::AfxThrowOleException( hResult );
	}

	return hResult;
}

HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::QueryInterface(REFIID iid, void ** ppvObject)
{

	HRESULT hr = S_OK;
	*ppvObject = NULL;

	if ( iid == IID_IUnknown ||
		iid == IID_IRichEditOleCallback )
	{
		*ppvObject = this;
		AddRef();
		hr = NOERROR;
	}
	else
	{
		hr = E_NOINTERFACE;
	}

	return hr;
}

ULONG STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::AddRef()
{
	return ++m_dwRef;
}



ULONG STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::Release()
{
	if ( --m_dwRef == 0 )
	{
		if (m_pStorage)
			m_pStorage->Release();

		m_pStorage = NULL;

		return 0;
	}

	return m_dwRef;
}


HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::GetInPlaceContext(LPOLEINPLACEFRAME FAR *lplpFrame,
	LPOLEINPLACEUIWINDOW FAR *lplpDoc, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	if (m_pOwner)
		return m_pOwner->GetInPlaceContext(lplpFrame, lplpDoc, lpFrameInfo);

	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::ShowContainerUI(BOOL fShow)
{
	if (m_pOwner)
		return m_pOwner->ShowContainerUI(fShow);

	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg, LONG cp)
{
	if (m_pOwner)
		return m_pOwner->QueryInsertObject(lpclsid, lpstg, cp);

	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::DeleteObject(LPOLEOBJECT lpoleobj)
{
	if (m_pOwner)
		return m_pOwner->DeleteObject(lpoleobj);

	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::QueryAcceptData(LPDATAOBJECT lpdataobj, CLIPFORMAT FAR *lpcfFormat,
	DWORD reco, BOOL fReally, HGLOBAL hMetaPict)
{
	if (m_pOwner)
		return m_pOwner->QueryAcceptData(lpdataobj, lpcfFormat, reco, fReally, hMetaPict);

	return S_OK;
}


HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::ContextSensitiveHelp(BOOL fEnterMode)
{
	if (m_pOwner)
		return m_pOwner->ContextSensitiveHelp(fEnterMode);

	return S_OK;
}



HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::GetClipboardData(CHARRANGE FAR *lpchrg, DWORD reco, LPDATAOBJECT FAR *lplpdataobj)
{
	if (m_pOwner)
		return m_pOwner->GetClipboardData(lpchrg, reco, lplpdataobj);

	return E_NOTIMPL;
}


HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	if (m_pOwner)
		return m_pOwner->GetDragDropEffect(fDrag, grfKeyState, pdwEffect);

	return S_OK;
}

HRESULT STDMETHODCALLTYPE 
CRichEditBaseCtrl::CRichEditOleCallback::GetContextMenu(WORD seltyp, LPOLEOBJECT lpoleobj, CHARRANGE FAR *lpchrg,
	HMENU FAR *lphmenu)
{
	if (m_pOwner)
		return m_pOwner->GetContextMenu(seltyp, lpoleobj, lpchrg, lphmenu);

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRichEditBaseCtrl Find & Replace

void CRichEditBaseCtrl::DoEditFind(UINT nIDTitle)
{
	ASSERT_VALID(this);
	DoEditFindReplace(TRUE, nIDTitle);
}

void CRichEditBaseCtrl::DoEditReplace(UINT nIDTitle)
{
	ASSERT_VALID(this);
	DoEditFindReplace(FALSE, nIDTitle);
}

void CRichEditBaseCtrl::AdjustDialogPosition(CDialog* pDlg)
{
	ASSERT(pDlg != NULL);
	long lStart, lEnd;
	GetSel(lStart, lEnd);
	CPoint point = GetCharPos(lStart);
	ClientToScreen(&point);
	CRect rectDlg;
	pDlg->GetWindowRect(&rectDlg);
	if (rectDlg.PtInRect(point))
	{
		if (point.y > rectDlg.Height())
			rectDlg.OffsetRect(0, point.y - rectDlg.bottom - 20);
		else
		{
			int nVertExt = GetSystemMetrics(SM_CYSCREEN);
			if (point.y + rectDlg.Height() < nVertExt)
				rectDlg.OffsetRect(0, 40 + point.y - rectDlg.top);
		}
		pDlg->MoveWindow(&rectDlg);
	}
}

void CRichEditBaseCtrl::DoEditFindReplace(BOOL bFindOnly, UINT nIDTitle)
{
	ASSERT_VALID(this);
	m_bFirstSearch = TRUE;

	if (m_findState.pFindReplaceDlg != NULL)
	{
		if (m_findState.bFindOnly == bFindOnly)
		{
			m_findState.pFindReplaceDlg->SetActiveWindow();
			m_findState.pFindReplaceDlg->ShowWindow(SW_SHOW);
			return;
		}
		else
		{
			ASSERT(m_findState.bFindOnly != bFindOnly);
			m_findState.pFindReplaceDlg->SendMessage(WM_CLOSE);
			ASSERT(m_findState.pFindReplaceDlg == NULL);
			ASSERT_VALID(this);
		}
	}
	CString strFind = GetSelText();
	// if selection is empty or spans multiple lines use old find text
	if (strFind.IsEmpty() || (strFind.FindOneOf(_T("\n\r")) != -1))
		strFind = m_findState.strFind;
	CString strReplace = m_findState.strReplace;
	m_findState.pFindReplaceDlg = new CFindReplaceDialog;
	ASSERT(m_findState.pFindReplaceDlg != NULL);
	DWORD dwFlags = NULL;
	if (m_findState.bNext)
		dwFlags |= FR_DOWN;
	if (m_findState.bCase)
		dwFlags |= FR_MATCHCASE;
	if (m_findState.bWord)
		dwFlags |= FR_WHOLEWORD;
	// hide stuff that RichEdit doesn't support
	dwFlags |= FR_HIDEUPDOWN;
	if (!m_findState.pFindReplaceDlg->Create(bFindOnly, strFind,
		strReplace, dwFlags, this))
	{
		m_findState.pFindReplaceDlg = NULL;
		ASSERT_VALID(this);
		return;
	}
	ASSERT(m_findState.pFindReplaceDlg != NULL);

	// set the title
	if (nIDTitle)
	{
		CString sTitle;
		sTitle.LoadString(nIDTitle);

		m_findState.pFindReplaceDlg->SetWindowText(sTitle);
	}

	m_findState.bFindOnly = bFindOnly;
	m_findState.pFindReplaceDlg->SetActiveWindow();
	m_findState.pFindReplaceDlg->ShowWindow(SW_SHOW);
	ASSERT_VALID(this);
}

void CRichEditBaseCtrl::OnFindNext(LPCTSTR lpszFind, BOOL bNext, BOOL bCase, BOOL bWord)
{
	ASSERT_VALID(this);

	m_findState.strFind = lpszFind;
	m_findState.bCase = bCase;
	m_findState.bWord = bWord;
	m_findState.bNext = bNext;

	if (!FindText())
		TextNotFound(m_findState.strFind);
	else
		AdjustDialogPosition(m_findState.pFindReplaceDlg);
	ASSERT_VALID(this);
}

void CRichEditBaseCtrl::OnReplaceSel(LPCTSTR lpszFind, BOOL bNext, BOOL bCase,
	BOOL bWord, LPCTSTR lpszReplace)
{
	ASSERT_VALID(this);

	m_findState.strFind = lpszFind;
	m_findState.strReplace = lpszReplace;
	m_findState.bCase = bCase;
	m_findState.bWord = bWord;
	m_findState.bNext = bNext;

	if (!SameAsSelected(m_findState.strFind, m_findState.bCase, m_findState.bWord))
	{
		if (!FindText())
			TextNotFound(m_findState.strFind);
		else
			AdjustDialogPosition(m_findState.pFindReplaceDlg);
		return;
	}

	ReplaceSel(m_findState.strReplace);

	if (!FindText())
		TextNotFound(m_findState.strFind);
	else
		AdjustDialogPosition(m_findState.pFindReplaceDlg);
	ASSERT_VALID(this);
}

void CRichEditBaseCtrl::OnReplaceAll(LPCTSTR lpszFind, LPCTSTR lpszReplace, BOOL bCase, BOOL bWord)
{
	ASSERT_VALID(this);

	m_findState.strFind = lpszFind;
	m_findState.strReplace = lpszReplace;
	m_findState.bCase = bCase;
	m_findState.bWord = bWord;
	m_findState.bNext = TRUE;

	CWaitCursor wait;
	// no selection or different than what looking for
	if (!SameAsSelected(m_findState.strFind, m_findState.bCase, m_findState.bWord))
	{
		if (!FindText())
		{
			TextNotFound(m_findState.strFind);
			return;
		}
	}

	HideSelection(TRUE, FALSE);

	do
	{
		ReplaceSel(m_findState.strReplace);
	} 
	while (FindTextSimple());

	TextNotFound(m_findState.strFind);
	HideSelection(FALSE, FALSE);

	ASSERT_VALID(this);
}

LRESULT CRichEditBaseCtrl::OnFindReplaceCmd(WPARAM, LPARAM lParam)
{
	ASSERT_VALID(this);
	CFindReplaceDialog* pDialog = CFindReplaceDialog::GetNotifier(lParam);
	ASSERT(pDialog != NULL);

	ASSERT(pDialog == m_findState.pFindReplaceDlg);
	if (pDialog->IsTerminating())
		m_findState.pFindReplaceDlg = NULL;
	else if (pDialog->FindNext())
	{
		OnFindNext(pDialog->GetFindString(), pDialog->SearchDown(),
			pDialog->MatchCase(), pDialog->MatchWholeWord());
	}
	else if (pDialog->ReplaceCurrent())
	{
		ASSERT(!m_findState.bFindOnly);
		OnReplaceSel(pDialog->GetFindString(),
			pDialog->SearchDown(), pDialog->MatchCase(), pDialog->MatchWholeWord(),
			pDialog->GetReplaceString());
	}
	else if (pDialog->ReplaceAll())
	{
		ASSERT(!m_findState.bFindOnly);
		OnReplaceAll(pDialog->GetFindString(), pDialog->GetReplaceString(),
			pDialog->MatchCase(), pDialog->MatchWholeWord());
	}
	ASSERT_VALID(this);
	return 0;
}

BOOL CRichEditBaseCtrl::SameAsSelected(LPCTSTR lpszCompare, BOOL bCase, BOOL /*bWord*/)
{
	// check length first
	size_t nLen = lstrlen(lpszCompare);
	long lStartChar, lEndChar;
	GetSel(lStartChar, lEndChar);
	if (nLen != (size_t)(lEndChar - lStartChar))
		return FALSE;

	// length is the same, check contents
	CString strSelect = GetSelText();
	return (bCase && lstrcmp(lpszCompare, strSelect) == 0) ||
		(!bCase && lstrcmpi(lpszCompare, strSelect) == 0);
}

BOOL CRichEditBaseCtrl::FindText()
{
	return FindText(m_findState.strFind, m_findState.bCase, m_findState.bWord);
}

BOOL CRichEditBaseCtrl::FindText(LPCTSTR lpszFind, BOOL bCase, BOOL bWord)
{
	ASSERT_VALID(this);
	CWaitCursor wait;
	return FindTextSimple(lpszFind, bCase, bWord);
}

BOOL CRichEditBaseCtrl::FindTextSimple()
{
	return FindTextSimple(m_findState.strFind, m_findState.bCase, m_findState.bWord);
}

BOOL CRichEditBaseCtrl::FindTextSimple(LPCTSTR lpszFind, BOOL bCase, BOOL bWord)
{
	ASSERT(lpszFind != NULL);
	FINDTEXTEX ft;

	GetSel(ft.chrg);
	if (m_bFirstSearch)
	{
		m_lInitialSearchPos = ft.chrg.cpMin;
		m_bFirstSearch = FALSE;
	}

	ft.lpstrText = lpszFind;
	if (ft.chrg.cpMin != ft.chrg.cpMax) // i.e. there is a selection
	{

#ifndef _UNICODE
		// If byte at beginning of selection is a DBCS lead byte,
		// increment by one extra byte.
		TEXTRANGE textRange;
		TCHAR ch[2];
		textRange.chrg.cpMin = ft.chrg.cpMin;
		textRange.chrg.cpMax = ft.chrg.cpMin + 1;
		textRange.lpstrText = ch;
		SendMessage(EM_GETTEXTRANGE, 0, (LPARAM)&textRange);
		if (_istlead(ch[0]))
		{
			ASSERT(ft.chrg.cpMax - ft.chrg.cpMin >= 2);
			ft.chrg.cpMin++;
		}
#endif

		ft.chrg.cpMin++;
	}

	if (m_lInitialSearchPos >= 0)
		ft.chrg.cpMax = GetTextLength();
	else
		ft.chrg.cpMax = GetTextLength()+m_lInitialSearchPos;

	DWORD dwFlags = FR_DOWN;

	dwFlags |= bCase ? FR_MATCHCASE : 0;
	dwFlags |= bWord ? FR_WHOLEWORD : 0;

	// if we find the text return TRUE
	if (FindAndSelect(dwFlags, ft) != -1)
		return TRUE;
	// if the original starting point was not the beginning of the buffer
	// and we haven't already been here
	else if (m_lInitialSearchPos > 0)
	{
		ft.chrg.cpMin = 0;
		ft.chrg.cpMax = m_lInitialSearchPos;
		m_lInitialSearchPos = m_lInitialSearchPos - GetTextLength();
		return FindAndSelect(dwFlags, ft) != -1;
	}
	// not found
	else
		return FALSE;
}

long CRichEditBaseCtrl::FindAndSelect(DWORD dwFlags, FINDTEXTEX& ft)
{
	long index = (long)::SendMessage(m_hWnd, EM_FINDTEXTEX, dwFlags, (LPARAM)&ft);

	if (index == -1)
		index = (long)::SendMessage(m_hWnd, EM_FINDTEXTEXW, dwFlags, (LPARAM)&ft);

	if (index != -1) // i.e. we found something
		SetSel(ft.chrgText);

	return index;
}

void CRichEditBaseCtrl::TextNotFound(LPCTSTR lpszFind)
{
	ASSERT_VALID(this);
	m_bFirstSearch = TRUE;
	OnTextNotFound(lpszFind);
}

void CRichEditBaseCtrl::OnTextNotFound(LPCTSTR)
{
	MessageBeep(MB_ICONHAND);
}
