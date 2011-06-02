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
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// HotTracker.h: interface for the CHotTracker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOTTRACKER_H__5F3B72B7_716D_47A0_B2A3_8815CE3FEBBE__INCLUDED_)
#define AFX_HOTTRACKER_H__5F3B72B7_716D_47A0_B2A3_8815CE3FEBBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Subclass.h"
#include <afxtempl.h>

const UINT WM_HTHOTCHANGE = ::RegisterWindowMessage(_T("WM_HTHOTCHANGE"));

class CHotTracker : public CSubclassWnd
{
public:
	CHotTracker();
	virtual ~CHotTracker();

	BOOL Initialize(CWnd* pWnd);
	void Reset();

	int AddRect(const CRect& rect);
	int AddRect(); // adds a placeholder
	BOOL UpdateRect(int nRect, const CRect& rect);
	int GetRectCount()
	{
		return m_aRects.GetSize();
	}

protected:
	CArray<CRect, CRect&> m_aRects;
	int m_nHotRect;

protected:
	int HitTest(CPoint ptScreen);
	virtual LRESULT WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp);
	void InitTracking();

};

#endif // !defined(AFX_HOTTRACKER_H__5F3B72B7_716D_47A0_B2A3_8815CE3FEBBE__INCLUDED_)
