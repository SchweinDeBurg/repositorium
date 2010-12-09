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
//*****************************************************************************

// HoldRedraw.h: interface and implementation of the CHoldRedraw class.
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_HOLDREDRAW_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_)
#define AFX_HOLDREDRAW_H__7E73ADE2_3848_4ED1_9E8B_8881813B4262__INCLUDED_

enum
{
	NCR_UPDATE = 0x01,
	NCR_PAINT = 0x02,
	NCR_NCPAINT = 0x04,
};

class CNcRedraw // note: there is no inheritance intentionally.
{
public:
	CNcRedraw(HWND hWnd, LPCTSTR szTrace = NULL) : m_hWnd(hWnd), m_sTrace(szTrace)
	{
	}

	virtual ~CNcRedraw()
	{
		if (m_hWnd && ::IsWindowVisible(m_hWnd))
		{
			TRACE(_T("~CNcRedraw(%s)\n"), m_sTrace);
			::SendMessage(m_hWnd, WM_NCPAINT, 0, 0);
		}
	}

protected:
	HWND m_hWnd;
	CString m_sTrace;
};

class CRedrawAll
{
public:
	CRedrawAll(HWND hWnd, DWORD dwFlags = NCR_PAINT | NCR_NCPAINT) : m_hWnd(hWnd), m_dwFlags(dwFlags)
	{
	}

	virtual ~CRedrawAll()
	{
		if (m_hWnd && ::IsWindowVisible(m_hWnd))
		{
			TRACE(_T("~CRedrawAll()\n"));

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

protected:
	HWND m_hWnd;
	DWORD m_dwFlags;
};

class CHoldRedraw : protected CRedrawAll
{
public:
	CHoldRedraw(HWND hWnd, DWORD dwFlags = NCR_PAINT | NCR_NCPAINT) : CRedrawAll(hWnd, dwFlags)
	{
		if (m_hWnd)
		{
			TRACE(_T("Disabling redraw on 0x%08x\n"), m_hWnd);
			::SendMessage(m_hWnd, WM_SETREDRAW, FALSE, 0);
		}
	}

	virtual ~CHoldRedraw()
	{
		if (m_hWnd)
		{
			TRACE(_T("Re-enabling redraw on 0x%08x\n"), m_hWnd);
			::SendMessage(m_hWnd, WM_SETREDRAW, TRUE, 0);
		}
	}
};

#endif
