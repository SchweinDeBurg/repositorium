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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// RichEditHelper.cpp: implementation of the CRichEditHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RichEditHelper.h"
#include "WClassDefines.h"

#include <tom.h>

#include "ClipboardBackup.h"

#include "Misc.h"
#include "FileMisc.h"
#include "EnBitmap.h"

#include <atlconv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define DEFINE_GUIDXXX(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	EXTERN_C const GUID CDECL name \
	= { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

DEFINE_GUIDXXX(IID_ITextDocument, 0x8CC497C0, 0xA1DF, 0x11CE, 0x80, 0x98,
	0x00, 0xAA, 0x00, 0x47, 0xBE, 0x5D);

#define RELEASE_INTERFACE(i) if (i) { i->Release(); i = NULL; }

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReBase::CReBase(HWND hwndRichEdit) : m_hwndRichedit(hwndRichEdit)
{
}

CReBase::~CReBase()
{
	m_hwndRichedit = NULL;
}

//////////////////////////////////////////////////////////////////////

CReSaveCaret::CReSaveCaret(HWND hwndRichEdit) : CReBase(hwndRichEdit)
{
	CHARRANGE cr;
	SendMessage(m_hwndRichedit, EM_EXGETSEL, 0, (LPARAM)&cr);

	// if the caret is at the end of the text then set it to LONG_MAX
	if (cr.cpMax == SendMessage(m_hwndRichedit, WM_GETTEXTLENGTH, 0, 0))
	{
		m_lCaret = LONG_MAX;
	}
	else
	{
		m_lCaret = cr.cpMax;
	}
}

CReSaveCaret::~CReSaveCaret()
{
	SendMessage(m_hwndRichedit, EM_SETSEL, m_lCaret, m_lCaret);
}

//////////////////////////////////////////////////////////////////////

CTextDocument::CTextDocument(HWND hwndRichEdit) : CReBase(hwndRichEdit), m_pDoc(NULL), m_pRichEditOle(NULL)
{
	SendMessage(hwndRichEdit, EM_GETOLEINTERFACE, 0, (LPARAM)&m_pRichEditOle);

	if (m_pRichEditOle)
	{
		m_pRichEditOle->QueryInterface(IID_ITextDocument, (void**)&m_pDoc);
	}
}

CTextDocument::~CTextDocument()
{
	if (m_pDoc)
	{
		m_pDoc->Release();
	}

	if (m_pRichEditOle)
	{
		m_pRichEditOle->Release();
	}
}

BOOL CTextDocument::Undo()
{
	return Valid() && (m_pDoc->Undo(1, NULL) == S_OK);
}

BOOL CTextDocument::Redo()
{
	return Valid() && (m_pDoc->Redo(1, NULL) == S_OK);
}

//////////////////////////////////////////////////////////////////////

CRePauseUndo::CRePauseUndo(HWND hwndRichEdit) : CTextDocument(hwndRichEdit)
{
	if (m_hwndRichedit && Valid())
	{
		m_pDoc->Undo(tomSuspend, NULL);
	}
}

CRePauseUndo::~CRePauseUndo()
{
	if (m_hwndRichedit && Valid())
	{
		m_pDoc->Undo(tomResume, NULL);
	}
}

//////////////////////////////////////////////////////////////////////

BOOL CRichEditHelper::CreateRichEdit20(CWnd& wnd, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	InitRichEdit();
	return wnd.Create(WC_RICHEDIT20, _T(""), dwStyle, rect, pParentWnd, nID);
}

BOOL CRichEditHelper::CreateRichEdit50(CWnd& wnd, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	InitRichEdit();
	return wnd.Create(ATL::CW2T(MSFTEDIT_CLASS), _T(""), dwStyle, rect, pParentWnd, nID);
}

BOOL CRichEditHelper::InitRichEdit()
{
	static HINSTANCE hRichEdit20 = NULL;
	static HINSTANCE hRichEdit50 = NULL;

	if (!AfxInitRichEdit())
	{
		return FALSE;
	}

	if (!hRichEdit20)
	{
		hRichEdit20 = LoadLibrary(_T("riched20.dll"));
	}

	if (!hRichEdit50)
	{
		hRichEdit50 = LoadLibrary(_T("msftedit.dll"));
	}

	return (hRichEdit20 != NULL || hRichEdit50 != NULL);
}

void CRichEditHelper::ClearUndo(HWND hWnd)
{
	ASSERT(hWnd);

	ITextDocument* pDoc;
	IUnknown* pUnk = NULL;

	SendMessage(hWnd, EM_GETOLEINTERFACE, 0, (LPARAM)&pUnk);

	if (pUnk)
	{
		if (pUnk->QueryInterface(IID_ITextDocument, (void**)&pDoc) == NOERROR)
		{
			pDoc->Undo(tomFalse, NULL);	// Prevents Undo and empties buffer.
			pDoc->Undo(tomTrue, NULL); //Restarts Undo again
			pDoc->Release();
		}
		pUnk->Release();
	}
}

BOOL CRichEditHelper::PasteFiles(CWnd& wnd, const CStringArray& aFiles, RE_PASTE nPasteHow)
{
	// save backup because we may be overwriting it
	CClipboardBackup clipBackup;
	clipBackup.Backup();

	UINT nFileCount = aFiles.GetSize(), nFilesPasted = 0;
	ASSERT(nFileCount != 0);

	for (UINT i = 0; i < nFileCount; i++)
	{
		if (PasteFileInternal(wnd, aFiles[i], nPasteHow))
		{
			nFilesPasted++;
		}
	}

	clipBackup.Restore();
	return (nFileCount == nFilesPasted);
}

BOOL CRichEditHelper::PasteFile(CWnd& wnd, LPCTSTR szFilePath, RE_PASTE nPasteHow)
{
	// save backup because we may be overwriting it
	CClipboardBackup clipBackup;
	clipBackup.Backup();

	BOOL bSuccess = PasteFileInternal(wnd, szFilePath, nPasteHow);

	clipBackup.Restore();
	return bSuccess;
}

BOOL CRichEditHelper::PasteFileInternal(CWnd& wnd, LPCTSTR szFilePath, RE_PASTE nPasteHow)
{
	if (FileMisc::FileExists(szFilePath))
	{
		switch (nPasteHow)
		{
		case REP_ASIMAGE:
			if (CEnBitmap::CopyImageFileToClipboard(szFilePath))
			{
				return wnd.SendMessage(EM_PASTESPECIAL, CF_BITMAP, NULL);
			}
			// else fall thru

		case REP_ASICON:
			if (CReFileObject(wnd).Insert(szFilePath))
			{
				return TRUE;
			}
			// else fall thru

		case REP_ASFILEURL:
			{
				CString sLink(szFilePath);
				sLink = _T("file://") + sLink;
				sLink.Replace(_T('\\'), _T('/'));

				// if the path contains spaces then brace it
				if (sLink.Find(_T(' ')) != -1)
				{
					sLink = _T("<") + sLink + _T(">");
				}

				// newline
				sLink += _T("\r\n");

				wnd.SendMessage(EM_REPLACESEL, TRUE, (LPARAM)(LPCTSTR)sLink);
			}
			// else fall thru

		default:
			break;
		}
	}

	// all else
	return FALSE;
}

//////////////////////////////////////////////////////////////////////

CReFileObject::CReFileObject(HWND hwndRichEdit) :
CReBase(hwndRichEdit),
m_pObject(NULL),
m_pClientSite(NULL),
m_pRichEditOle(NULL),
m_pStorage(NULL)
{
	SendMessage(m_hwndRichedit, EM_GETOLEINTERFACE, 0, (LPARAM)&m_pRichEditOle);
}

CReFileObject::~CReFileObject()
{
	Reset();
	RELEASE_INTERFACE(m_pRichEditOle);
}

BOOL CReFileObject::Create(LPCTSTR szFilePath)
{
	if (!m_pRichEditOle)
	{
		return FALSE;
	}

	if (!FileMisc::FileExists(szFilePath))
	{
		return FALSE;
	}

	Reset();

	LPLOCKBYTES pLockBytes = NULL;
	CLSID clsid = CLSID_NULL;

	SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &pLockBytes);
	ASSERT(pLockBytes);

	if (sc != S_OK)
	{
		return FALSE;
	}

	sc = ::StgCreateDocfileOnILockBytes(pLockBytes,
		STGM_SHARE_EXCLUSIVE |
		STGM_CREATE |
		STGM_READWRITE,
		0, &m_pStorage);

	if (sc != S_OK)
	{
		pLockBytes->Release();
		return FALSE;
	}

	FORMATETC formatEtc;

	formatEtc.cfFormat = 0;
	formatEtc.ptd = NULL;
	formatEtc.dwAspect = DVASPECT_CONTENT;
	formatEtc.lindex = -1;
	formatEtc.tymed = TYMED_NULL;

	// create object
	m_pRichEditOle->GetClientSite(&m_pClientSite);

	sc = ::OleCreateFromFile(clsid, ATL::CT2OLE(szFilePath),
		IID_IUnknown,
		OLERENDER_DRAW,
		NULL,
		m_pClientSite,
		m_pStorage,
		(void**)&m_pObject);

	if (m_pObject)
	{
		// convert to IOleObject
		LPUNKNOWN pUnk = m_pObject;

		pUnk->QueryInterface(IID_IOleObject, (void**)&m_pObject);
		pUnk->Release();
	}

	if (!m_pObject)
	{
		Reset();
	}
	else
	{
		OleSetContainedObject(m_pObject, TRUE);
	}

	return (m_pObject != NULL);
}

BOOL CReFileObject::GetReObject(REOBJECT& reObj) const
{
	if (!m_pObject)
	{
		return FALSE;
	}

	ZeroMemory(&reObj, sizeof(reObj));

	reObj.cbStruct = sizeof(reObj);

	CLSID clsid;
	SCODE sc = m_pObject->GetUserClassID(&clsid);

	if (sc != S_OK)
	{
		return FALSE;
	}

	reObj.clsid = clsid;
	reObj.cp = REO_CP_SELECTION;
	reObj.dvaspect = DVASPECT_CONTENT;
	reObj.dwFlags = REO_RESIZABLE | REO_BELOWBASELINE;
	reObj.dwUser = 0;
	reObj.poleobj = m_pObject;
	reObj.polesite = m_pClientSite;
	reObj.pstg = m_pStorage;
	reObj.sizel.cx = reObj.sizel.cy = 0;

	return TRUE;
}


void CReFileObject::Reset()
{
	RELEASE_INTERFACE(m_pObject);
	RELEASE_INTERFACE(m_pStorage);
	RELEASE_INTERFACE(m_pClientSite);
}

BOOL CReFileObject::Insert(LPCTSTR szFilePath)
{
	if (Create(szFilePath))
	{
		REOBJECT reObj;

		if (GetReObject(reObj))
		{
			return (S_OK == m_pRichEditOle->InsertObject(&reObj));
		}
	}

	return FALSE;
}

//////////////////////////////////////////////////////////////////////
