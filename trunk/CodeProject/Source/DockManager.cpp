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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
//*****************************************************************************

// DockManager.cpp: implementation of the CDockManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DockManager.h"
#include "HoldRedraw.h"
#include "AutoFlag.h"

#include <multimon.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

const int MINMAXSIZE = GetSystemMetrics(SM_CYCAPTION) * 4;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDockManager::CDockManager():
m_nDockPos(DMP_UNDOCKED),
m_nLastDockPos(DMP_RIGHT),
m_nWidthDocked(-1),
m_nWidthDockedMax(-1),
m_nHeightDocked(-1),
m_nHeightDockedMax(-1),
m_bResizeUpdate(TRUE),
m_bSizeUpdate(TRUE),
m_sizeMainMin(-1, -1),
m_sizeDockMin(-1, -1)
{
}

CDockManager::~CDockManager()
{
}

BOOL CDockManager::Initialize(CWnd* pMainWnd, CWnd* pDockWnd, DM_POS nPos, DM_POS nLastPos, int nWidthDocked,
	int nWidthDockedMax, int nHeightDocked, int nHeightDockedMax)
{
	if (!HookWindow(*pMainWnd) || !ScHookWindow(*pDockWnd))
	{
		return FALSE;
	}

	m_nLastDockPos = nLastPos;
	m_nWidthDocked = nWidthDocked;
	m_nWidthDockedMax = nWidthDockedMax;
	m_nHeightDocked = nHeightDocked;
	m_nHeightDockedMax = nHeightDockedMax;

	return Dock(nPos);
}

BOOL CDockManager::Dock(DM_POS nPos)
{
	if (nPos == DMP_UNDOCKED)
	{
		return UnDock();
	}

	// check if no change
	if (IsDocked() && m_nDockPos == nPos)
	{
		return TRUE;
	}

	BOOL bDocked = IsDocked();
	m_nDockPos = nPos;

	CRect rDock;
	ScGetCWnd()->GetWindowRect(rDock);

	// save window pos if not currently docked
	if (!bDocked)
	{
		m_rUndocked = rDock;
	}

	// and restore previous width/height
	if (nPos == DMP_BELOW)
	{
		int nHeight = GetDockedHeight(Maximized());

		if (nHeight != -1)
		{
			rDock.bottom = rDock.top + nHeight;

			CAutoFlag af(m_bResizeUpdate, FALSE);
			CAutoFlag af2(m_bSizeUpdate, FALSE);
			MoveWindow(ScGetCWnd(), rDock);
		}
	}
	else
	{
		int nWidth = GetDockedWidth(Maximized());

		if (nWidth != -1)
		{
			rDock.right = rDock.left + nWidth;

			CAutoFlag af(m_bResizeUpdate, FALSE);
			CAutoFlag af2(m_bSizeUpdate, FALSE);
			MoveWindow(ScGetCWnd(), rDock);
		}
	}

	// also restore main window pos if maximized
	if (Maximized())
	{
		OnMaximize();
	}
	else
	{
		UpdateDockWindowPos();
		FitDockWindowToWorkArea(); // make sure it's visible
	}

	return TRUE;
}

BOOL CDockManager::UnDock()
{
	if (!IsDocked())
	{
		return TRUE;
	}

	m_nLastDockPos = m_nDockPos;
	m_nDockPos = DMP_UNDOCKED;

	// restore window pos
	ScGetCWnd()->MoveWindow(m_rUndocked);

	// also restore main window pos if maximized
	if (Maximized())
	{
		OnMaximize();
	}
	else
	{
		FitDockWindowToWorkArea();   // make sure it's visible
	}

	return TRUE;
}

LRESULT CDockManager::WindowProc(HWND /*hRealWnd*/, UINT msg, WPARAM wp, LPARAM lp)
{
	if (!IsDocked())
	{
		return Default();
	}

	switch (msg)
	{
	case WM_MOVE:
		{
			LRESULT lr = Default();

			if (m_bResizeUpdate)
			{
				UpdateDockWindowPos();
			}

			return lr;
		}
		break;

	case WM_SIZE:
		{
			LRESULT lr = Default();

			if (m_bResizeUpdate)
			{
				UpdateDockWindowPos();
			}

			return lr;
		}
		break;

	case WM_SYSCOMMAND:
		if (wp == SC_MAXIMIZE || wp == SC_RESTORE)
		{
			CAutoFlag af(m_bResizeUpdate, FALSE);
			LRESULT lr = Default();

			if (wp == SC_MAXIMIZE)
			{
				OnMaximize();
			}
			else
			{
				OnRestore();
			}

			return lr;
		}
		break;

	case WM_NCLBUTTONDBLCLK:
		if (wp == HTCAPTION)
		{
			CAutoFlag af(m_bResizeUpdate, FALSE);
			BOOL bIsZoomed = Maximized();

			LRESULT lr = Default();

			if (!bIsZoomed)
			{
				OnMaximize();
			}
			else
			{
				OnRestore();
			}

			return lr;
		}
		break;

	case WM_NCLBUTTONDOWN:
		// if this is in the caption and the main window in maxed
		// then eat
		if (wp == HTCAPTION && Maximized())
		{
			// activate the window first
			SendMessage(WM_ACTIVATE, WA_CLICKACTIVE, NULL);
			return 0;
		}
		break;

	case WM_GETMINMAXINFO:
		{
			// if the main window is zoomed then don't restrict how far
			// the docked window can be resized
			LRESULT lr = Default();

			// save off our last min size
			LPMINMAXINFO pMMI = (LPMINMAXINFO)lp;
			m_sizeMainMin = pMMI->ptMinTrackSize;

			OnMinMaxInfo(pMMI, TRUE);
			return lr;
		}

	}

	return Default();
}

void CDockManager::OnMaximize()
{
	ASSERT(Maximized());

	BOOL bDockVisible = ::IsWindowVisible(ScGetHwnd());
	CRect rMain = GetWorkArea();

	if (bDockVisible && IsDocked())
	{
		CRect rDock;
		::GetWindowRect(ScGetHwnd(), rDock);

		switch (m_nDockPos)
		{
		case DMP_LEFT:
			if (m_nWidthDockedMax == -1)
			{
				rMain.left += min(rDock.Width(), rMain.Width() / 2);
			}
			else
			{
				rMain.left += m_nWidthDockedMax;
			}
			break;

		case DMP_RIGHT:
			if (m_nWidthDockedMax == -1)
			{
				rMain.right -= min(rDock.Width(), rMain.Width() / 2);
			}
			else
			{
				rMain.right -= m_nWidthDockedMax;
			}
			break;

		case DMP_BELOW:
			if (m_nHeightDockedMax == -1)
			{
				rMain.bottom -= min(rDock.Height(), rMain.Height() / 2);
			}
			else
			{
				rMain.bottom -= m_nHeightDockedMax;
			}
			break;

		default:
			ASSERT(0);
			return;
		}
	}

	MoveWindow(GetCWnd(), rMain);

	if (IsDocked())
	{
		UpdateDockWindowPos();
	}
}

void CDockManager::OnRestore()
{
	ASSERT(!Maximized());

	BOOL bDockVisible = ::IsWindowVisible(ScGetHwnd());

	if (bDockVisible && IsDocked())
	{
		CRect rDock;
		GetWindowRect(rDock);

		switch (m_nDockPos)
		{
		case DMP_LEFT:
			if (m_nWidthDocked != -1)
			{
				rDock.right = rDock.left;
				rDock.left -= m_nWidthDocked;
			}
			break;

		case DMP_RIGHT:
			if (m_nWidthDocked != -1)
			{
				rDock.left = rDock.right;
				rDock.right += m_nWidthDocked;
			}
			break;

		case DMP_BELOW:
			if (m_nHeightDocked != -1)
			{
				rDock.top = rDock.bottom;
				rDock.bottom += m_nHeightDocked;
			}
			break;

		default:
			ASSERT(0);
			return;
		}

		MoveWindow(ScGetCWnd(), rDock);
	}
}

LRESULT CDockManager::ScWindowProc(HWND hRealWnd, UINT msg, WPARAM wp, LPARAM lp)
{

	switch (msg)
	{
	case WM_MOVE:
		if (IsDocked())
		{
			LRESULT lr = ScDefault(hRealWnd);

			if (m_bResizeUpdate)
			{
				UpdateMainWindowPos();
			}

			return lr;
		}
		break;

	case WM_SIZE:
		if (IsDocked())
		{
			LRESULT lr = ScDefault(hRealWnd);

			if (m_bResizeUpdate)
			{
				UpdateMainWindowPos();
			}

			// save dock width
			if (m_bSizeUpdate && ::IsWindowVisible(ScGetHwnd()))
			{
				CRect rDock;
				::GetWindowRect(ScGetHwnd(), rDock);

				if (Maximized())
				{
					if (m_nDockPos == DMP_BELOW)
					{
						m_nHeightDockedMax = rDock.Height();
					}
					else
					{
						m_nWidthDockedMax = rDock.Width();
					}
				}
				else
				{
					if (m_nDockPos == DMP_BELOW)
					{
						m_nHeightDocked = rDock.Height();
					}
					else
					{
						m_nWidthDocked = rDock.Width();
					}
				}
			}

			return lr;
		}
		break;

	case WM_SYSCOMMAND:
		if (IsDocked())
		{
			switch (wp)
			{
				// hide system menu
			case SC_KEYMENU:
			case SC_MOUSEMENU:
				return 0;

				// don't allow docked window to be minimized or maximized directly
				// instead, send the message to the main window
			case SC_MAXIMIZE:
			case SC_MINIMIZE:
				return SendMessage(msg, wp, lp);

				// if the dock window is being closed and the main window is maximized
				// then readjust the main window rect
			case SC_CLOSE:
				if (Maximized())
				{
					LRESULT lr = ScDefault(hRealWnd);

					OnMaximize();

					return lr;
				}
				break;
			}
		}
		break;

	case WM_NCRBUTTONDOWN:
		// if this is in the caption then eat it
		if (IsDocked() && wp == HTCAPTION)
		{
			// activate the window first
			ScSendMessage(WM_ACTIVATE, WA_CLICKACTIVE, NULL);
			return 0;
		}
		break;

	case WM_NCLBUTTONDOWN:
		// if this is in the caption and the main window in maxed
		// then eat
		if (IsDocked() && wp == HTCAPTION && Maximized())
		{
			// activate the window first
			ScSendMessage(WM_ACTIVATE, WA_CLICKACTIVE, NULL);
			return 0;
		}
		break;

	case WM_NCLBUTTONDBLCLK:
		if (wp == HTCAPTION)
		{
			// toggle the docked state
			if (IsDocked())
			{
				UnDock();
			}
			else
			{
				Dock(m_nLastDockPos);
			}

			// and eat the message
			return 0;
		}
		break;

	case WM_NCHITTEST:
		if (IsDocked())
		{
			UINT nHitTest = ScDefault(hRealWnd);

			// if the main window is _not_ unmaximized then don't let the
			// docked window be resized on it's docked edge
			// because its not intuitive and causes no end of trouble :)
			if (!Maximized())
			{
				switch (m_nDockPos)
				{
				case DMP_LEFT:
					if (nHitTest == HTRIGHT || nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMRIGHT)
					{
						nHitTest = HTCLIENT;
					}
					break;

				case DMP_RIGHT:
					if (nHitTest == HTLEFT || nHitTest == HTTOPLEFT || nHitTest == HTBOTTOMLEFT)
					{
						nHitTest = HTCLIENT;
					}
					break;

				case DMP_BELOW:
					if (nHitTest == HTTOP || nHitTest == HTTOPLEFT || nHitTest == HTTOPRIGHT)
					{
						nHitTest = HTCLIENT;
					}
					break;

				default:
					ASSERT(0);
					break;
				}
			}
			// else main window is maximized so _only_ let it be resized on its
			// docked edge and resize the main window afterwards
			else
			{
				switch (m_nDockPos)
				{
				case DMP_LEFT:
					if (nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMRIGHT ||
						nHitTest == HTLEFT || nHitTest == HTTOPLEFT ||
						nHitTest == HTBOTTOMLEFT || nHitTest == HTTOP || nHitTest == HTBOTTOM)
					{
						nHitTest = HTCLIENT;
					}
					break;

				case DMP_RIGHT:
					if (nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMRIGHT ||
						nHitTest == HTRIGHT || nHitTest == HTTOPLEFT ||
						nHitTest == HTBOTTOMLEFT || nHitTest == HTTOP || nHitTest == HTBOTTOM)
					{
						nHitTest = HTCLIENT;
					}
					break;

				case DMP_BELOW:
					if (nHitTest == HTTOPRIGHT || nHitTest == HTBOTTOMRIGHT ||
						nHitTest == HTRIGHT || nHitTest == HTTOPLEFT ||
						nHitTest == HTBOTTOMLEFT || nHitTest == HTLEFT || nHitTest == HTBOTTOM)
					{
						nHitTest = HTCLIENT;
					}
					break;

				default:
					ASSERT(0);
					break;
				}
			}

			return nHitTest;
		}
		break;

	case WM_GETMINMAXINFO:
		if (IsDocked())
		{
			LRESULT lr = ScDefault(hRealWnd);

			// save off our last min size
			LPMINMAXINFO pMMI = (LPMINMAXINFO)lp;
			m_sizeDockMin = pMMI->ptMinTrackSize;

			OnMinMaxInfo(pMMI, FALSE);
			return lr;
		}

	case WM_WINDOWPOSCHANGED:
		// if the dock window is being shown/hidden and the main window is maximized
		// then adjust the main window rect
		if (IsDocked() && Maximized())
		{
			LPWINDOWPOS lpwp = (LPWINDOWPOS)lp;

			if (lpwp->flags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW))
			{
				LRESULT lr = ScDefault(hRealWnd);

				OnMaximize();

				return lr;
			}
		}
		break;
	}

	return ScDefault(hRealWnd);
}

void CDockManager::OnMinMaxInfo(LPMINMAXINFO pMMI, BOOL /*bMain*/)
{
	if (Maximized())
	{
		CSize sizeMin = GetMinMaximizedSize();
		CRect rWork = GetWorkArea();

		if (m_nDockPos == DMP_BELOW)
		{
			pMMI->ptMinTrackSize.y = sizeMin.cy;
			pMMI->ptMaxTrackSize.y = rWork.Height() - sizeMin.cy;
		}
		else
		{
			pMMI->ptMinTrackSize.x = sizeMin.cx;
			pMMI->ptMaxTrackSize.x = rWork.Width() - sizeMin.cx;
		}
	}
	else
	{
		CSize sizeMin;
		if (m_nDockPos == DMP_BELOW)
		{
			sizeMin.cx = max(m_sizeMainMin.cx, m_sizeDockMin.cx);
			sizeMin.cy = m_sizeDockMin.cy;
		}
		else
		{
			sizeMin.cx = m_sizeDockMin.cx;
			sizeMin.cy = max(m_sizeMainMin.cy, m_sizeDockMin.cy);
		}

		pMMI->ptMinTrackSize.y = max(sizeMin.cy, pMMI->ptMinTrackSize.y);
		pMMI->ptMinTrackSize.x = max(sizeMin.cx, pMMI->ptMinTrackSize.x);
	}
}

void CDockManager::FitDockWindowToWorkArea()
{
	// make sure the dock window is fully visible
	CRect rDock, rMain, rWorkArea = GetWorkArea();
	::GetWindowRect(ScGetHwnd(), rDock);
	GetWindowRect(rMain);

	CRect rIntersect;

	// if it is then we're done
	if (rIntersect.IntersectRect(rWorkArea, rDock) && rIntersect == rDock)
	{
		return;
	}

	// else adjust the edges to be within the work area
	// try adjust the size until we hit m_sizeDockMin
	switch (m_nDockPos)
	{
	case DMP_LEFT:
		rDock.left = max(rDock.left, rWorkArea.left);
		rDock.right = max(rDock.left + m_sizeDockMin.cx, rDock.right);

		ScGetCWnd()->MoveWindow(rDock);

		// check we've not pushed the main window over
		if (rDock.right > rMain.left)
		{
			rMain.OffsetRect(rDock.right - rMain.left, 0);

			// and stop the right edge of the main window spilling out
			rMain.right = min(rMain.right, rWorkArea.right);

			GetCWnd()->MoveWindow(rMain);
		}
		break;

	case DMP_RIGHT:
		rDock.right = min(rDock.right, rWorkArea.right);
		rDock.left = min(rDock.right - m_sizeDockMin.cx, rDock.left);

		ScGetCWnd()->MoveWindow(rDock);

		// check we've not pushed the main window over
		if (rDock.left < rMain.right)
		{
			rMain.OffsetRect(rDock.left - rMain.right, 0);

			// and stop the left edge of the main window spilling out
			rMain.left = max(rMain.left, rWorkArea.left);

			GetCWnd()->MoveWindow(rMain);
		}
		break;

	case DMP_BELOW:
		rDock.bottom = min(rDock.bottom, rWorkArea.bottom);
		rDock.top = min(rDock.bottom - m_sizeDockMin.cy, rDock.top);

		ScGetCWnd()->MoveWindow(rDock);

		// check we've not pushed the main window over
		if (rDock.top < rMain.bottom)
		{
			rMain.OffsetRect(0, rDock.top - rMain.bottom);

			// and stop the top edge of the main window spilling out
			rMain.top = max(rMain.top, rWorkArea.top);

			GetCWnd()->MoveWindow(rMain);
		}
		break;

	case DMP_UNDOCKED:
		// just centre the window on the desktop
		ScGetCWnd()->CenterWindow();
		break;
	}
}

void CDockManager::UpdateDockWindowPos()
{
	ASSERT(IsDocked());

	if (!IsDocked())
	{
		return;
	}

	CRect rMain, rDock;

	GetWindowRect(rMain);
	::GetWindowRect(ScGetHwnd(), rDock);

	if (Maximized())
	{
		CRect rWorkArea = GetWorkArea();

		switch (m_nDockPos)
		{
		case DMP_LEFT:
			rDock.top = rMain.top;
			rDock.bottom = rMain.bottom;
			rDock.left = rWorkArea.left;
			rDock.right = rMain.left;
			break;

		case DMP_RIGHT:
			rDock.top = rMain.top;
			rDock.bottom = rMain.bottom;
			rDock.right = rWorkArea.right;
			rDock.left = rMain.right;
			break;

		case DMP_BELOW:
			rDock.left = rMain.left;
			rDock.right = rMain.right;
			rDock.bottom = rWorkArea.bottom;
			rDock.top = rMain.bottom;
			break;

		default:
			ASSERT(0);
			return;
		}
	}
	else // not maximized
	{
		switch (m_nDockPos)
		{
		case DMP_LEFT:
			rDock.top = rMain.top;
			rDock.bottom = rMain.bottom;
			rDock.left = rMain.left - rDock.Width();
			rDock.right = rMain.left;
			break;

		case DMP_RIGHT:
			rDock.top = rMain.top;
			rDock.bottom = rMain.bottom;
			rDock.right = rMain.right + rDock.Width();
			rDock.left = rMain.right;
			break;

		case DMP_BELOW:
			rDock.left = rMain.left;
			rDock.right = rMain.right;
			rDock.bottom = rMain.bottom + rDock.Height();
			rDock.top = rMain.bottom;
			break;

		default:
			ASSERT(0);
			return;
		}
	}

	MoveWindow(ScGetCWnd(), rDock);
}

void CDockManager::UpdateMainWindowPos()
{
	ASSERT(IsDocked());

	if (!IsDocked())
	{
		return;
	}

	CRect rMain, rDock;

	GetWindowRect(rMain);
	::GetWindowRect(ScGetHwnd(), rDock);

	// if the main window is maximized then shrink/enlarge
	// the window
	if (Maximized())
	{
		rMain = GetWorkArea();

		switch (m_nDockPos)
		{
		case DMP_LEFT:
			rMain.left = rDock.right;
			break;

		case DMP_RIGHT:
			rMain.right = rDock.left;
			break;

		case DMP_BELOW:
			rMain.bottom = rDock.top;
			break;

		default:
			ASSERT(0);
			return;
		}
	}
	// else just move the main window
	else
	{
		switch (m_nDockPos)
		{
		case DMP_LEFT:
			rMain.top = rDock.top;
			rMain.bottom = rDock.bottom;
			rMain.right = rDock.right + rMain.Width();
			rMain.left = rDock.right;
			break;

		case DMP_RIGHT:
			rMain.top = rDock.top;
			rMain.bottom = rDock.bottom;
			rMain.left = rDock.left - rMain.Width();
			rMain.right = rDock.left;
			break;

		case DMP_BELOW:
			rMain.left = rDock.left;
			rMain.right = rDock.right;
			rMain.top = rDock.top - rMain.Height();
			rMain.bottom = rDock.top;
			break;

		default:
			ASSERT(0);
			return;
		}
	}

	MoveWindow(GetCWnd(), rMain);
}

void CDockManager::MoveWindow(CWnd* pWnd, CRect rect)
{
	CRect rOrg;
	pWnd->GetWindowRect(rOrg);

	// don't move if nuthin's changed
	if (rect != rOrg)
	{
		pWnd->SetWindowPos(NULL, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
	}
}

CRect CDockManager::GetUnDockedRect()
{
	if (IsDocked())
	{
		return m_rUndocked;
	}

	// else
	CRect rect;
	::GetWindowRect(ScGetHwnd(), rect);

	return rect;
}

CRect CDockManager::GetWorkArea(BOOL bMaximized)
{
	CRect rWorkArea(0, 0, 0, 0);

	HMONITOR hMonitor = MonitorFromWindow(GetHwnd(), MONITOR_DEFAULTTONEAREST);
	ASSERT(hMonitor);

	if (hMonitor)
	{
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		rWorkArea = mi.rcWork;

		if (bMaximized)
		{
			rWorkArea.InflateRect(4, 4);
		}
	}

	return rWorkArea;
}

CSize CDockManager::GetMinMaximizedSize()
{
	CSize sizeMin = GetWorkArea().Size();

	if (IsDocked() && ::IsWindowVisible(ScGetHwnd()))
	{
		switch (m_nDockPos)
		{
		case DMP_LEFT:
		case DMP_RIGHT:
			sizeMin.cx = MINMAXSIZE;
			break;

		case DMP_BELOW:
			sizeMin.cy = MINMAXSIZE;
			break;

		default:
			ASSERT(0);
		}
	}

	return sizeMin;
}
