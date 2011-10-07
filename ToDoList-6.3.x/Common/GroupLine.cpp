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
// - adjusted #include's paths
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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// GroupLine.cpp: implementation of the CGroupLine class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "GroupLine.h"
#include "../../CodeProject/Source/WinClasses.h"
#include "../../CodeProject/Source/WClassDefines.h"
#include "../../CodeProject/Source/Themed.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGroupLine::CGroupLine()
{
}

CGroupLine::~CGroupLine()
{
}

BOOL CGroupLine::Initialize(HWND hwndStatic)
{
	ASSERT(hwndStatic);
	ASSERT(!IsHooked());
	ASSERT(CWinClasses::IsClass(hwndStatic, WC_STATIC));

	if (!IsHooked() && hwndStatic && HookWindow(hwndStatic))
	{
		Invalidate();
		return TRUE;
	}

	return FALSE;
}

LRESULT CGroupLine::WindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_PAINT:
		{
			CWnd* pWnd = GetCWnd();
			CPaintDC dc(pWnd);

			CRect rClient;
			GetClientRect(rClient);

			CString sText;
			pWnd->GetWindowText(sText);

			DWORD dwStyle = pWnd->GetStyle();
			CThemed theme;

			UINT nFlags = DT_TOP;

			if (dwStyle & SS_RIGHT)
			{
				nFlags |= DT_RIGHT;
			}

			else if (dwStyle & SS_CENTER)
			{
				nFlags |= DT_CENTER;
			}
			else
			{
				nFlags |= DT_LEFT;
			}

			HFONT hFont = (HFONT)SendMessage(WM_GETFONT);

			if (!hFont)
			{
				hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
			}

			HFONT hOld = (HFONT)::SelectObject(dc, hFont);
			dc.SetBkMode(TRANSPARENT);

			CSize sizeText = dc.GetTextExtent(sText); // default

			if (theme.Open(pWnd, _T("BUTTON")))
			{
				theme.DrawText(&dc, BP_GROUPBOX, GBS_NORMAL, sText, nFlags, 0, rClient);

				CRect rExtents;
				theme.GetTextExtent(&dc, BP_GROUPBOX, GBS_NORMAL, sText, nFlags, rExtents, rClient);

				sizeText = rExtents.Size();
			}
			else // unthemed
			{
				dc.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
				dc.DrawText(sText, rClient, nFlags);
			}

			// draw line
			sizeText.cx += 4;

			int nY = ((rClient.top + rClient.bottom) / 2);

			if (dwStyle & SS_RIGHT)
			{
				dc.Draw3dRect(rClient.left, nY, rClient.Width() - sizeText.cx, 2, GetSysColor(COLOR_3DSHADOW),
					GetSysColor(COLOR_3DHIGHLIGHT));
			}
			else if (dwStyle & SS_CENTER)
			{
				dc.Draw3dRect(rClient.left, nY, (rClient.Width() - sizeText.cx) / 2, 2, GetSysColor(COLOR_3DSHADOW),
					GetSysColor(COLOR_3DHIGHLIGHT));
				dc.Draw3dRect((rClient.Width() + sizeText.cx) / 2, nY, (rClient.Width() - sizeText.cx) / 2, 2,
					GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));
			}
			else
			{
				dc.Draw3dRect(rClient.left + sizeText.cx, nY, rClient.Width() - sizeText.cx, 2,
					GetSysColor(COLOR_3DSHADOW), GetSysColor(COLOR_3DHIGHLIGHT));
			}

			// cleanup
			::SelectObject(dc, hOld);


			return 0;
		}
		break;
	}

	return CSubclassWnd::WindowProc(hRealWnd, msg, wp, lp);
}

//////////////////////////////////////////////////////////////////////

CGroupLineManager::CGroupLineManager()
{
}

CGroupLineManager::~CGroupLineManager()
{
	// cleanup
	HWND hwnd;
	CGroupLine* pGroupLine;

	POSITION pos = m_mapGroupLines.GetStartPosition();

	while (pos)
	{
		m_mapGroupLines.GetNextAssoc(pos, hwnd, pGroupLine);

		if (pGroupLine->IsValid())
		{
			pGroupLine->HookWindow(NULL);
		}

		delete pGroupLine;
	}
}

BOOL CGroupLineManager::AddGroupLine(UINT nIDStatic, HWND hwndParent)
{
	HWND hwndStatic = ::GetDlgItem(hwndParent, nIDStatic);

	if (!hwndStatic)
	{
		return FALSE;
	}

	// have we already got it?
	CGroupLine* pGroupLine = NULL;

	if (m_mapGroupLines.Lookup(hwndStatic, pGroupLine))
	{
		return TRUE;
	}

	// else create new editprompt
	pGroupLine = new CGroupLine;

	if (pGroupLine && pGroupLine->Initialize(hwndStatic))
	{
		m_mapGroupLines[hwndStatic] = pGroupLine;
		return TRUE;
	}

	// else
	delete pGroupLine;
	return FALSE;
}
