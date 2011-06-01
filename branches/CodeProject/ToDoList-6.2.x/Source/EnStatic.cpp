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
//*****************************************************************************

// EnStatic.cpp : implementation file
//

#include "stdafx.h"
#include "EnStatic.h"
#include "Themed.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnStatic

const int GRIPPERSIZE = 16;

CEnStatic::CEnStatic(BOOL bEnableGripper) : m_bGripper(bEnableGripper ? 1 : 0)
{
}

CEnStatic::~CEnStatic()
{
}


BEGIN_MESSAGE_MAP(CEnStatic, CStatic)
	//{{AFX_MSG_MAP(CEnStatic)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnStatic message handlers

void CEnStatic::EnableGripper(BOOL bEnable)
{
	bEnable = bEnable ? 1 : 0;

	if (GetSafeHwnd() && m_bGripper != bEnable)
	{
		Invalidate();
	}

	m_bGripper = bEnable;
}

void CEnStatic::OnPaint()
{
	if (IsShowingGripper())
	{
		CPaintDC dc(this); // device context for painting

		// default first
		DefWindowProc(WM_PAINT, (WPARAM)(HDC)dc, 0);

		// then the gripper
		CThemed::DrawFrameControl(this, &dc, GetGripperRect(), DFC_SCROLL, DFCS_SCROLLSIZEGRIP);

		return;
	}
	else // all else
	{
		Default();
	}
}

BOOL CEnStatic::IsShowingGripper()
{
	if (m_bGripper)
	{
		int nType = (GetStyle() & SS_TYPEMASK);

		switch (nType)
		{
		case SS_LEFT:
		case SS_CENTER:
		case SS_RIGHT:
		case SS_SIMPLE:
		case SS_LEFTNOWORDWRAP:
			return TRUE;
		}
	}

	return FALSE;
}

void CEnStatic::OnSize(UINT nType, int cx, int cy)
{
	if (IsShowingGripper())
	{
		Invalidate();
	}

	CStatic::OnSize(nType, cx, cy);
}

#if _MSC_VER >= 1400
LRESULT CEnStatic::OnNcHitTest(CPoint point)
#else
UINT CEnStatic::OnNcHitTest(CPoint point)
#endif
{
	if (IsShowingGripper())
	{
		CRect rClient;
		GetClientRect(rClient);

		rClient.left = rClient.right - GRIPPERSIZE;
		rClient.top = rClient.bottom - GRIPPERSIZE;

		ClientToScreen(rClient);

		if (rClient.PtInRect(point))
		{
			return HTBOTTOMRIGHT;
		}
	}

	return CStatic::OnNcHitTest(point);
}

BOOL CEnStatic::OnEraseBkgnd(CDC* pDC)
{
	CStatic::OnEraseBkgnd(pDC);

	// draw gripper here too
	if (IsShowingGripper())
	{
		pDC->DrawFrameControl(GetGripperRect(), DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}

	return TRUE;
}

CRect CEnStatic::GetGripperRect()
{
	if (IsShowingGripper())
	{
		CRect rClient;
		GetClientRect(rClient);

		rClient.left = rClient.right - GRIPPERSIZE;
		rClient.top = rClient.bottom - GRIPPERSIZE;

		return rClient;
	}
	else
	{
		return CRect(0, 0, 0, 0);
	}
}

void CEnStatic::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
{
	CStatic::OnWindowPosChanging(lpwndpos);

	InvalidateRect(GetGripperRect(), FALSE);
}
