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

// ColorButton.cpp : implementation file
//

#include "StdAfx.h"
#include "ColorButton.h"
#include "EnColorDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

#define WM_SHOWACCELERATORS 0x0128

CColorButton::CColorButton() : m_color(0)
{
}

CColorButton::~CColorButton()
{
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
	//{{AFX_MSG_MAP(CColorButton)
	ON_WM_PAINT()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_ENABLE()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SHOWACCELERATORS, OnShowAccelerators)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CColorButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	// create a temp dc to paint on
	CDC dcTemp;

	if (dcTemp.CreateCompatibleDC(&dc))
	{
		CBitmap bmTemp;
		CRect rClient;

		GetClientRect(rClient);

		if (bmTemp.CreateCompatibleBitmap(&dc, rClient.right, rClient.bottom))
		{
			// draw color on top
			CRect rColor(rClient);
			rColor.DeflateRect(5, 4, 5, 5);

			DWORD dwStyle = GetStyle();

			if (dwStyle & BS_LEFT)
				rColor.left = rColor.right - max(12, rColor.Height());

			else if (dwStyle & BS_RIGHT)
				rColor.right = rColor.left + max(12, rColor.Height());

			else
				rColor.SetRectEmpty();

			if (!rColor.IsRectEmpty())
			{
				CBitmap* pOld = dcTemp.SelectObject(&bmTemp);

				// default draw to temp dc
				DefWindowProc(WM_PAINT, (WPARAM)(HDC)dcTemp, 0);

				dcTemp.FillSolidRect(rColor, m_color);

				// black border
				if (m_color > 0)
				{
					if (IsWindowEnabled())
						::FrameRect(dcTemp, rColor, (HBRUSH)::GetStockObject(BLACK_BRUSH));
					else
						::FrameRect(dcTemp, rColor, (HBRUSH)::GetStockObject(GRAY_BRUSH));
				}

				dc.BitBlt(0, 0, rClient.right, rClient.bottom, &dcTemp, 0, 0, SRCCOPY);

				// cleanup
				dcTemp.SelectObject(pOld);

				return;
			}
		}
	}

	// else draw to default dc
	DefWindowProc(WM_PAINT, (WPARAM)(HDC)dc, 0);
}

void CColorButton::SetColor(COLORREF color)
{
	m_color = color;

	if (GetSafeHwnd())
		Invalidate();
}

void CColorButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	Invalidate();
	UpdateWindow();

	CButton::OnLButtonUp(nFlags, point);
}

void CColorButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDown(nFlags, point);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnMouseMove(UINT nFlags, CPoint point)
{
	CButton::OnMouseMove(nFlags, point);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDblClk(nFlags, point);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnKillFocus(CWnd* pNewWnd)
{
	CButton::OnKillFocus(pNewWnd);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnSetFocus(CWnd* pOldWnd)
{
	CButton::OnSetFocus(pOldWnd);

	Invalidate();
	UpdateWindow();
}

void CColorButton::OnEnable(BOOL bEnable)
{
	CButton::OnEnable(bEnable);

	Invalidate();
	UpdateWindow();
}

BOOL CColorButton::OnClicked()
{
	Invalidate();
	UpdateWindow();

	//	CEnColorDialog dialog(m_color, CC_FULLOPEN | CC_RGBINIT);
	CColorDialog dialog(m_color, CC_FULLOPEN | CC_RGBINIT);

	if (dialog.DoModal() == IDOK)
	{
		m_color = dialog.m_cc.rgbResult;

		Invalidate();
		UpdateWindow();

		return FALSE; // pass to parent
	}

	return TRUE; // no need to bother parent
}

LRESULT CColorButton::OnShowAccelerators(WPARAM /*wp*/, LPARAM /*lp*/)
{
	LRESULT lr = Default();
	RedrawWindow();

	return lr;
}
