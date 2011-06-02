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

// DlgUnits.cpp: implementation of the CDlgUnits class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DlgUnits.h"
#include "WinClasses.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDlgUnits::CDlgUnits(HWND hWnd) : m_hWnd(NULL)
{
	if (!hWnd || !CWinClasses::IsDialog(hWnd))
	{
		DWORD dwDLBU = ::GetDialogBaseUnits();

		m_dlgBaseUnits.cx = LOWORD(dwDLBU);
		m_dlgBaseUnits.cy = HIWORD(dwDLBU);
	}
	else
	{
		m_hWnd = hWnd;
		m_dlgBaseUnits.cx = m_dlgBaseUnits.cy = 0;
	}
}

CDlgUnits::CDlgUnits(CWnd* pWnd) : m_hWnd(NULL)
{
	if (!pWnd || !pWnd->GetSafeHwnd() || !CWinClasses::IsDialog(*pWnd))
	{
		DWORD dwDLBU = ::GetDialogBaseUnits();

		m_dlgBaseUnits.cx = LOWORD(dwDLBU);
		m_dlgBaseUnits.cy = HIWORD(dwDLBU);
	}
	else
	{
		m_hWnd = pWnd->GetSafeHwnd();
		m_dlgBaseUnits.cx = m_dlgBaseUnits.cy = 0;
	}
}

CDlgUnits::~CDlgUnits()
{
}

int CDlgUnits::ToPixelsX(int x) const
{
	if (m_hWnd)
	{
		CRect rect(0, 0, x, 0);
		MapDialogRect(m_hWnd, rect);

		return rect.right;
	}
	else
	{
		return MulDiv(x, m_dlgBaseUnits.cx, 4);
	}
}

int CDlgUnits::ToPixelsY(int y) const
{
	if (m_hWnd)
	{
		CRect rect(0, 0, 0, y);
		MapDialogRect(m_hWnd, rect);

		return rect.bottom;
	}
	else
	{
		return MulDiv(y, m_dlgBaseUnits.cy, 8);
	}
}

void CDlgUnits::ToPixels(long& x, long& y) const
{
	if (m_hWnd)
	{
		CRect rect(0, 0, x, y);
		MapDialogRect(m_hWnd, rect);

		x = rect.right;
		y = rect.bottom;
	}
	else
	{
		x = MulDiv(x, m_dlgBaseUnits.cx, 4);
		y = MulDiv(y, m_dlgBaseUnits.cy, 8);
	}
}

void CDlgUnits::ToPixels(int& x, int& y) const
{
	ToPixels((long&)x, (long&)y);
}

void CDlgUnits::ToPixels(short& x, short& y) const
{
	ToPixels((long&)x, (long&)y);
}

void CDlgUnits::ToPixels(POINT& point) const
{
	ToPixels(point.x, point.y);
}

void CDlgUnits::ToPixels(RECT& rect) const
{
	ToPixels(rect.left, rect.top);
	ToPixels(rect.right, rect.bottom);
}

void CDlgUnits::ToPixels(SIZE& size) const
{
	ToPixels(size.cx, size.cy);
}

void CDlgUnits::FromPixels(long& x, long& y) const
{
	if (m_hWnd)
	{
		// this is a bit messy because there is no MapDialogRect() in reverse
		CRect rect(0, 0, 1000, 1000);
		MapDialogRect(m_hWnd, rect);

		x = MulDiv(x, 1000, rect.right);
		y = MulDiv(y, 1000, rect.right);
	}
	else
	{
		x = MulDiv(x, 4, m_dlgBaseUnits.cx);
		y = MulDiv(y, 8, m_dlgBaseUnits.cy);
	}
}

void CDlgUnits::FromPixels(int& x, int& y) const
{
	FromPixels((long&)x, (long&)y);
}

void CDlgUnits::FromPixels(short& x, short& y) const
{
	FromPixels((long&)x, (long&)y);
}

void CDlgUnits::FromPixels(POINT& point) const
{
	FromPixels(point.x, point.y);
}

void CDlgUnits::FromPixels(RECT& rect) const
{
	FromPixels(rect.left, rect.top);
	FromPixels(rect.right, rect.bottom);
}

void CDlgUnits::FromPixels(SIZE& size) const
{
	FromPixels(size.cx, size.cy);
}
