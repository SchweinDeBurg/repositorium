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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// HoldRedraw.h: interface of the CHoldRedraw class.
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOLDREDRAW_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_)
#define AFX_HOLDREDRAW_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_

#include <afxtempl.h>

enum
{
	NCR_UPDATE = 0x01,
	NCR_PAINT = 0x02,
	NCR_NCPAINT = 0x04,
};

class CNcRedraw // note: there is no inheritance intentionally.
{
public:
	CNcRedraw(HWND hWnd);
	virtual ~CNcRedraw();

protected:
	HWND m_hWnd;
};

class CRedrawAll
{
public:
	CRedrawAll(HWND hWnd, DWORD dwFlags = NCR_PAINT | NCR_NCPAINT);
	virtual ~CRedrawAll();

protected:
	HWND m_hWnd;
	DWORD m_dwFlags;
};

class CHoldRedraw : protected CRedrawAll
{
public:
	CHoldRedraw(HWND hWnd, DWORD dwFlags = NCR_PAINT | NCR_NCPAINT);
	virtual ~CHoldRedraw();

protected:
	static CList<HWND, HWND&> s_listHwnd;
};

#endif
