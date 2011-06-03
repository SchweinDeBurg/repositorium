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
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// HoldRedraw.cpp: implementation of the CHoldRedraw class.
//
/////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HoldRedraw.h"

CList<HWND, HWND&> CHoldRedraw::s_listHwnd;

// ------------------------------------------------------------------------------

CNcRedraw::CNcRedraw(HWND hWnd) : m_hWnd(hWnd)
{
}

CNcRedraw::~CNcRedraw()
{
	if (m_hWnd && ::IsWindowVisible(m_hWnd))
	{
		::SendMessage(m_hWnd, WM_NCPAINT, 0, 0);
	}
}

// ------------------------------------------------------------------------------

CRedrawAll::CRedrawAll(HWND hWnd, DWORD dwFlags) : m_hWnd(hWnd), m_dwFlags(dwFlags)
{
}

CRedrawAll::~CRedrawAll()
{
	if (m_hWnd && ::IsWindowVisible(m_hWnd))
	{
		if (m_dwFlags & NCR_NCPAINT)
		{
			::SendMessage(m_hWnd, WM_NCPAINT, 0, 0);
		}

		if (m_dwFlags & NCR_PAINT)
		{
			::InvalidateRect(m_hWnd, NULL, FALSE);

			if (m_dwFlags & NCR_UPDATE)
			{
				::UpdateWindow(m_hWnd);
			}
		}
	}
}

// ------------------------------------------------------------------------------

CHoldRedraw::CHoldRedraw(HWND hWnd, DWORD dwFlags) : CRedrawAll(hWnd, dwFlags)
{
	if (m_hWnd)
	{
		// check if there is already a hold on this window
		POSITION pos = s_listHwnd.Find(m_hWnd);

		if (pos) // found
		{
			m_hWnd = NULL; // prevent another hold being taken
		}
		else
		{
			s_listHwnd.AddTail(m_hWnd);
			::SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
		}
	}
}

CHoldRedraw::~CHoldRedraw()
{
	if (m_hWnd)
	{
		::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);

		// remove from list
		POSITION pos = s_listHwnd.Find(m_hWnd);
		ASSERT(pos);

		s_listHwnd.RemoveAt(pos);
	}
}
