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

// ContentMgr.h: interface for the CContentMgr class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONTENTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
#define AFX_CONTENTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ContentCtrl.h"
#include <afxtempl.h>

class IContent;

class CContentMgr
{
public:
	CContentMgr();
	virtual ~CContentMgr();

	virtual BOOL Initialize();
	BOOL SomePluginsHaveBadversions() const { return m_bSomeBadVersions; }

	int GetNumContent() const;

	int FindContent(LPCTSTR szID) const;

	CString GetContentTypeID(int nContent) const;
	CString GetContentTypeDescription(int nContent) const;
	CONTENTFORMAT GetContentFormat(int nContent) const;

	BOOL ContentFormatIsText(int nContent) const;
	BOOL ContentFormatIsText(const CString& sTypeID) const;

	BOOL CreateContentControl(int nContent, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle,
		DWORD dwExStyle, const CRect& rect, HWND hwndParent);
	BOOL CreateContentControl(const CONTENTFORMAT& cf, CContentCtrl& ctrl, UINT nCtrlID, DWORD nStyle,
		DWORD dwExStyle, const CRect& rect, HWND hwndParent);

	BOOL ConvertContentToHtml(const CString& sContent, CString& sHtml, LPCTSTR szID);


protected:
	BOOL m_bInitialized;
	BOOL m_bSomeBadVersions;
	CArray<IContent*, IContent*> m_aContent;

protected:
};

#endif // !defined(AFX_CONTENTMGR_H__C258D849_69ED_46A7_A2F0_351C5C9FB3B3__INCLUDED_)
