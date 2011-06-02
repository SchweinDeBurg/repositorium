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
//*****************************************************************************

// TabCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "TabCtrlEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx

CTabCtrlEx::CTabCtrlEx(DWORD dwFlags) : m_dwFlags(dwFlags), m_bMBtnDown(FALSE)
{
}

CTabCtrlEx::~CTabCtrlEx()
{
}


BEGIN_MESSAGE_MAP(CTabCtrlEx, CXPTabCtrl)
	//{{AFX_MSG_MAP(CTabCtrlEx)
	ON_WM_PAINT()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_CAPTURECHANGED()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx message handlers

void CTabCtrlEx::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// always do default
	if (!IsExtendedTabThemedXP())
	{
		DefWindowProc(WM_PAINT, (WPARAM)dc.m_hDC, 0L);
	}
	else
	{
		CXPTabCtrl::DoPaint(&dc);
	}

	// then post draw if required
	if (m_dwFlags & TCE_POSTDRAW)
	{
		CRect rClip;
		dc.GetClipBox(rClip);

		DRAWITEMSTRUCT dis;
		dis.CtlType = ODT_TAB;
		dis.CtlID = GetDlgCtrlID();
		dis.hwndItem = GetSafeHwnd();
		dis.hDC = dc;
		dis.itemAction = ODA_DRAWENTIRE;

		// paint the tabs
		int nTab = GetItemCount();
		int nSel = GetCurSel();

		while (nTab--)
		{
			if (nTab != nSel)
			{
				dis.itemID = nTab;
				dis.itemState = 0;

				VERIFY(GetItemRect(nTab, &dis.rcItem));

				dis.rcItem.bottom -= 2;
				dis.rcItem.top += 2;
				dis.rcItem.left += 2;
				dis.rcItem.right -= 2;

				if (CRect().IntersectRect(rClip, &dis.rcItem))
				{
					GetParent()->SendMessage(WM_DRAWITEM, dis.CtlID, (LPARAM)&dis);
				}
			}
		}

		// now selected tab
		if (nSel != -1)
		{
			dis.itemID = nSel;
			dis.itemState = ODS_SELECTED;

			VERIFY(GetItemRect(nSel, &dis.rcItem));
			dis.rcItem.bottom += 2;

			if (CRect().IntersectRect(rClip, &dis.rcItem))
			{
				GetParent()->SendMessage(WM_DRAWITEM, dis.CtlID, (LPARAM)&dis);
			}
		}
	}
}

void CTabCtrlEx::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if (m_dwFlags & TCE_POSTDRAW)
	{
		return;   // ignore because we probably sent it
	}

	CXPTabCtrl::DrawItem(lpDrawItemStruct);
}

void CTabCtrlEx::OnMButtonDown(UINT nFlags, CPoint point)
{
	CXPTabCtrl::OnMButtonDown(nFlags, point);

	if (m_dwFlags & TCE_MBUTTONCLICK)
	{
		TCHITTESTINFO tchi = { { point.x, point.y }, 0 };

		if (HitTest(&tchi) != -1)
		{
			m_bMBtnDown = TRUE;
			m_ptMBtnDown = point;
		}
	}
}

void CTabCtrlEx::OnMButtonUp(UINT nFlags, CPoint point)
{
	if ((m_dwFlags & TCE_MBUTTONCLICK) && m_bMBtnDown)
	{
		int nXBorder = GetSystemMetrics(SM_CXDOUBLECLK) / 2;
		int nYBorder = GetSystemMetrics(SM_CYDOUBLECLK) / 2;

		CRect rect(m_ptMBtnDown.x - nXBorder,
			m_ptMBtnDown.y - nYBorder,
			m_ptMBtnDown.x + nXBorder,
			m_ptMBtnDown.y + nYBorder);

		if (rect.PtInRect(point))
		{
			TCHITTESTINFO tchi = { { point.x, point.y }, 0 };
			int nTab = HitTest(&tchi);

			if (nTab >= 0)
			{
				NMTCMBTNCLK tcnmh = { { *this, GetDlgCtrlID(), NM_MCLICK }, nTab, nFlags };
				GetParent()->SendMessage(WM_NOTIFY, tcnmh.hdr.idFrom, (LPARAM)&tcnmh);
			}
		}

		m_bMBtnDown = FALSE;
	}

	CXPTabCtrl::OnMButtonUp(nFlags, point);
}

void CTabCtrlEx::OnCaptureChanged(CWnd* pWnd)
{
	if ((m_dwFlags & TCE_MBUTTONCLICK) && m_bMBtnDown)
	{
		m_bMBtnDown = FALSE;
	}

	CXPTabCtrl::OnCaptureChanged(pWnd);
}
