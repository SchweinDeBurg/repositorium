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
// - merged with ToDoList version 6.1 sources
//*****************************************************************************

// RichEditHelper.h: interface for the CRichEditHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHEDITHELPER_H__C498C86D_613F_42AD_9C93_6C773E6368E8__INCLUDED_)
#define AFX_RICHEDITHELPER_H__C498C86D_613F_42AD_9C93_6C773E6368E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <richedit.h>
#include <richole.h>

#if !defined EM_SETTYPOGRAPHYOPTIONS
#	define EM_SETTYPOGRAPHYOPTIONS (WM_USER + 202)
#	define EM_GETTYPOGRAPHYOPTIONS (WM_USER + 203)
#	define TO_ADVANCEDTYPOGRAPHY 1
#	define TO_SIMPLELINEBREAK 2 
#endif
struct ITextDocument;

class CReBase
{
public:
	CReBase(HWND hwndRichEdit);
	virtual ~CReBase();

protected:
	HWND m_hwndRichedit;
};

class CTextDocument : public CReBase
{
public:
	CTextDocument(HWND hwndRichEdit);
	virtual ~CTextDocument();
	
	BOOL Valid() const { return (m_pDoc != NULL);	}
	operator ITextDocument*() { return m_pDoc; }

	BOOL Undo();
	BOOL Redo();
	
protected:
	ITextDocument* m_pDoc;
	LPRICHEDITOLE m_pRichEditOle;
};

class CRePauseUndo : public CTextDocument
{
public:
	CRePauseUndo(HWND hwndRichEdit);
	virtual ~CRePauseUndo();
};

class CReSaveCaret : public CReBase
{
public:
	CReSaveCaret(HWND hwndRichEdit);
	virtual ~CReSaveCaret();

protected:
	long m_lCaret;
};

class CReFileObject : public CReBase
{
public:
	CReFileObject(HWND hwndRichEdit);
	virtual ~CReFileObject();
	
	BOOL Insert(LPCTSTR szFilePath);
	
protected:
	LPRICHEDITOLE m_pRichEditOle;
	LPOLEOBJECT m_pObject;
	LPSTORAGE m_pStorage;
	LPOLECLIENTSITE m_pClientSite;
	
protected:
	void Reset();
	
	BOOL Create(LPCTSTR szFilePath);
	BOOL GetReObject(REOBJECT& reObj) const;
	
};

class CRichEditHelper
{
public:
	static BOOL CreateRichEdit20(CWnd& wnd, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	static BOOL CreateRichEdit50(CWnd& wnd, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	static BOOL InitRichEdit();
	static void ClearUndo(HWND hWnd);
//	static BOOL Redo(HWND hWnd);

protected:
};

#endif // !defined(AFX_RICHEDITHELPER_H__C498C86D_613F_42AD_9C93_6C773E6368E8__INCLUDED_)
