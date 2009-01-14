// CJToolBarBase.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new IE4 controls'
// http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 13:51 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJToolBarBase.cpp $
 *
 * $History: CJToolBarBase.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 24/05/02   Time: 13:51
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/28/99    Time: 12:38a
 * Updated in $/CodeJockey/CJLibrary
 * Final pass for release 6.08.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/CJLibrary
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/

#include "stdafx.h"
#include "CJToolBarBase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if (_MFC_VER >= 0x0900)
#if !defined(CX_BORDER)
#define CX_BORDER AFX_CX_BORDER
#endif   // CX_BORDER
#if !defined(CY_BORDER)
#define CY_BORDER AFX_CY_BORDER
#endif   // CY_BORDER
#endif   // _MFC_VER

/////////////////////////////////////////////////////////////////////////////
// CCJToolBarBase

CCJToolBarBase::CCJToolBarBase()
{
	m_bInReBar = false;
	m_bExStyle = false;
}

CCJToolBarBase::~CCJToolBarBase()
{

}

BEGIN_MESSAGE_MAP(CCJToolBarBase, CControlBar)
	//{{AFX_MSG_MAP(CCJToolBarBase)
	ON_WM_PAINT()
	ON_WM_WINDOWPOSCHANGING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CCJToolBarBase::SetBorders(int cxLeft, int cyTop, int cxRight, int cyBottom)
{
	ASSERT(cxLeft >= 0);
	ASSERT(cyTop >= 0);
	ASSERT(cxRight >= 0);
	ASSERT(cyBottom >= 0);

	m_cxLeftBorder = cxLeft;
	m_cyTopBorder = cyTop;
	m_cxRightBorder = cxRight;
	m_cyBottomBorder = cyBottom;
}

LRESULT CCJToolBarBase::WindowProc(UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	ASSERT_VALID(this);

	LRESULT lResult;
	switch (nMsg)
	{
	case WM_NOTIFY:
	case WM_COMMAND:
	case WM_DRAWITEM:
	case WM_MEASUREITEM:
	case WM_DELETEITEM:
	case WM_COMPAREITEM:
	case WM_VKEYTOITEM:
	case WM_CHARTOITEM:
		// send these messages to the owner if not handled
		if (OnWndMsg(nMsg, wParam, lParam, &lResult))
			return lResult;
		else
		{
			// try owner next
			lResult = GetOwner()->SendMessage(nMsg, wParam, lParam);

			// special case for TTN_NEEDTEXTA and TTN_NEEDTEXTW
			if(nMsg == WM_NOTIFY)
			{
				NMHDR* pNMHDR = (NMHDR*)lParam;
				if (pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW)
				{
					TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
					TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
					if ((pNMHDR->code == TTN_NEEDTEXTA && (!pTTTA->lpszText || !*pTTTA->lpszText)) ||
						(pNMHDR->code == TTN_NEEDTEXTW && (!pTTTW->lpszText || !*pTTTW->lpszText)))
					{
						// not handled by owner, so let bar itself handle it
						lResult = CWnd::WindowProc(nMsg, wParam, lParam);
					}
				}
			}
			return lResult;
		}
	}

	// otherwise, just handle in default way
	lResult = CWnd::WindowProc(nMsg, wParam, lParam);
	return lResult;
}

void CCJToolBarBase::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
	// WINBUG: We call DefWindowProc here instead of CWnd::OnWindowPosChanging
	//  (which calls CWnd::Default, which calls through the super wndproc)
	//  because certain control bars that are system implemented (such as
	//  CToolBar with TBSTYLE_FLAT) do not implement WM_WINDOWPOSCHANGING
	//  correctly, causing repaint problems.  This code bypasses that whole
	//  mess.
	::DefWindowProc(m_hWnd, WM_WINDOWPOSCHANGING, 0, (LPARAM)lpWndPos);

	if (lpWndPos->flags & SWP_NOSIZE)
		return;

	// invalidate borders on the right
	CRect rect;
	GetWindowRect(&rect);
	CSize sizePrev = rect.Size();
	int cx = lpWndPos->cx;
	int cy = lpWndPos->cy;
	if (cx != sizePrev.cx && (m_dwStyle & CBRS_BORDER_RIGHT))
	{
		rect.SetRect(cx-afxData.cxBorder2, 0, cx, cy);
		InvalidateRect(&rect);
		rect.SetRect(sizePrev.cx-afxData.cxBorder2, 0, sizePrev.cx, cy);
		InvalidateRect(&rect);
	}

	// invalidate borders on the bottom
	if (cy != sizePrev.cy && (m_dwStyle & CBRS_BORDER_BOTTOM))
	{
		rect.SetRect(0, cy-afxData.cyBorder2, cx, cy);
		InvalidateRect(&rect);
		rect.SetRect(0, sizePrev.cy-afxData.cyBorder2, cx, sizePrev.cy);
		InvalidateRect(&rect);
	}
}

void CCJToolBarBase::OnPaint()
{
	// background is already filled in gray
	CPaintDC dc(this);

	// erase background now
	if (IsVisible())
		DoPaint(&dc);       // delegate to paint helper
}

void CCJToolBarBase::EraseNonClient()
{
	// get window DC that is clipped to the non-client area
	CWindowDC dc(this);
	CRect rectClient;
	GetClientRect(rectClient);
	CRect rectWindow;
	GetWindowRect(rectWindow);
	ScreenToClient(rectWindow);
	rectClient.OffsetRect(-rectWindow.left, -rectWindow.top);
	dc.ExcludeClipRect(rectClient);

	// draw borders in non-client area
	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
	DrawBorders(&dc, rectWindow);

	// erase parts not drawn
	dc.IntersectClipRect(rectWindow);
	SendMessage(WM_ERASEBKGND, (WPARAM)dc.m_hDC);

	// draw gripper in non-client area
	DrawGripper(&dc, rectWindow);
}

void CCJToolBarBase::DoPaint(CDC* pDC)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	// paint inside client area
	CRect rect;
	GetClientRect(rect);
	DrawBorders(pDC, rect);
	DrawGripper(pDC, rect);
}

void CCJToolBarBase::DrawBorders(CDC* pDC, CRect& rect)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);

	DWORD dwStyle = m_dwStyle;
	if (!(dwStyle & CBRS_BORDER_ANY))
		return;

	// prepare for dark lines
	ASSERT(rect.top == 0 && rect.left == 0);
	if(m_bInReBar)
	{
		CRect rect1, rect2;
		rect1 = rect;
		rect2 = rect;

		#if _MSC_VER >= 1300		// VC 7.0 (.NET)
			COLORREF clr = afxData.clrBtnShadow;
		#else						// VC 5.0/6.0
			COLORREF clr = afxData.bWin4 ? afxData.clrBtnShadow : afxData.clrWindowFrame;
		#endif
		
		// draw dark line one pixel back/up
		if (dwStyle & CBRS_BORDER_3D)
		{
			rect1.right -= CX_BORDER;
			rect1.bottom -= CY_BORDER;
		}
		if (dwStyle & CBRS_BORDER_TOP)
			rect2.top += afxData.cyBorder2;
		if (dwStyle & CBRS_BORDER_BOTTOM)
			rect2.bottom -= afxData.cyBorder2;
		
		// draw left and top
		if (dwStyle & CBRS_BORDER_LEFT)
			pDC->FillSolidRect(0, rect2.top, CX_BORDER, rect2.Height(), clr);
		if (dwStyle & CBRS_BORDER_TOP)
			pDC->FillSolidRect(0, 0, rect.right, CY_BORDER, clr);
		
		// draw right and bottom
		if (dwStyle & CBRS_BORDER_RIGHT)
			pDC->FillSolidRect(rect1.right, rect2.top, -CX_BORDER, rect2.Height(), clr);
		if (dwStyle & CBRS_BORDER_BOTTOM)
			pDC->FillSolidRect(0, rect1.bottom, rect.right, -CY_BORDER, clr);
		
		if (dwStyle & CBRS_BORDER_3D)
		{
			// prepare for hilite lines
			clr = afxData.clrBtnHilite;
			
			// draw left and top
			if (dwStyle & CBRS_BORDER_LEFT)
				pDC->FillSolidRect(1, rect2.top, CX_BORDER, rect2.Height(), clr);
			if (dwStyle & CBRS_BORDER_TOP)
				pDC->FillSolidRect(0, 1, rect.right, CY_BORDER, clr);
			
			// draw right and bottom
			if (dwStyle & CBRS_BORDER_RIGHT)
				pDC->FillSolidRect(rect.right, rect2.top, -CX_BORDER, rect2.Height(), clr);
			if (dwStyle & CBRS_BORDER_BOTTOM)
				pDC->FillSolidRect(0, rect.bottom, rect.right, -CY_BORDER, clr);
		}
		
		if (dwStyle & CBRS_BORDER_LEFT)
			rect.left += afxData.cxBorder2;
		if (dwStyle & CBRS_BORDER_TOP)
			rect.top += afxData.cyBorder2;
		if (dwStyle & CBRS_BORDER_RIGHT)
			rect.right -= afxData.cxBorder2;
		if (dwStyle & CBRS_BORDER_BOTTOM)
			rect.bottom -= afxData.cyBorder2;
	}
	else
	{
		if(dwStyle & CBRS_BORDER_RIGHT) {
			pDC->FillSolidRect(rect.right-1, 0, rect.right, rect.bottom, afxData.clrBtnShadow);
			rect.right--;
		}
		if(dwStyle & CBRS_BORDER_BOTTOM) {
			pDC->FillSolidRect(0, rect.bottom-1, rect.right, rect.bottom, afxData.clrBtnShadow);
			rect.bottom--;
		}
		if(dwStyle & CBRS_BORDER_TOP) {
			pDC->FillSolidRect(0, 0, rect.right, 1, afxData.clrBtnHilite);
			rect.top++;
		}
		if(dwStyle & CBRS_BORDER_LEFT) {
			pDC->FillSolidRect(0, 0, 1, rect.bottom, afxData.clrBtnHilite);
			rect.left++;
		}
	}
}

#define CX_GRIPPER			3
#define CY_GRIPPER			3
#define CX_BORDER_GRIPPER	2
#define CY_BORDER_GRIPPER	2

void CCJToolBarBase::DrawGripper(CDC* pDC, const CRect& rect)
{
	// if we are floating return.
	if( m_dwStyle & CBRS_FLOATING ) {
		return;
	}

	// if we are to draw the extended "office" style gripper.
	if(( m_dwStyle & CBRS_GRIPPER ) && ( m_bExStyle == true ))
	{
		// draw the gripper in the border
		if (m_dwStyle & CBRS_ORIENT_HORZ)
		{
			// Draw the first gripper.
			pDC->Draw3dRect( rect.left+3, rect.top+2, 3, rect.Height()-4,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
			
			pDC->SetPixel( rect.left+3, rect.top+2+rect.Height()-5,
				afxData.clrBtnHilite);

			// Draw the second gripper.
			pDC->Draw3dRect( rect.left+6, rect.top+2, 3, rect.Height()-4,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
			
			pDC->SetPixel( rect.left+6, rect.top+2+rect.Height()-5,
				afxData.clrBtnHilite);
		}
		else
		{
			// Draw the first gripper.
			pDC->Draw3dRect( rect.left+2, rect.top+3, rect.Width()-4, 3,
				afxData.clrBtnHilite, afxData.clrBtnShadow);

			pDC->SetPixel( rect.left+2+rect.Width()-5, rect.top+3,
				afxData.clrBtnHilite);
			
			// Draw the second gripper.
			pDC->Draw3dRect( rect.left+2, rect.top+6, rect.Width()-4, 3,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
			
			pDC->SetPixel( rect.left+2+rect.Width()-5, rect.top+6,
				afxData.clrBtnHilite);
		}

		GdiFlush();
	}

	// if we are to draw the "ReBar" style gripper.
	else
	if(( m_dwStyle & CBRS_GRIPPER ) && ( m_bInReBar == true ))
	{
		// draw the gripper in the border
		if (m_dwStyle & CBRS_ORIENT_HORZ)
		{
			// Draw the first gripper.
			pDC->Draw3dRect( rect.left+CX_BORDER_GRIPPER, rect.top+m_cyTopBorder,
				CX_GRIPPER, rect.Height()-m_cyTopBorder-m_cyBottomBorder,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
		}
		else
		{
			// Draw the first gripper.
			pDC->Draw3dRect( rect.left+m_cyTopBorder, rect.top+CY_BORDER_GRIPPER,
				rect.Width()-m_cyTopBorder-m_cyBottomBorder, CY_GRIPPER,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
		}

		GdiFlush();
	}

	// by default, we will draw the "visual studio" style gripper.
	else
	if( m_dwStyle & CBRS_GRIPPER )
	{
		// draw the gripper in the border
		if (m_dwStyle & CBRS_ORIENT_HORZ)
		{
			// Draw the first gripper.
			pDC->Draw3dRect( rect.left+1, rect.top+2, 3, rect.Height()-4,
				afxData.clrBtnHilite, afxData.clrBtnShadow);

			// Draw the second gripper.
			pDC->Draw3dRect( rect.left+5, rect.top+2, 3, rect.Height()-4,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
		}
		else
		{
			// Draw the first gripper.
			pDC->Draw3dRect( rect.left+2, rect.top+1, rect.Width()-4, 3,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
			
			// Draw the second gripper.
			pDC->Draw3dRect( rect.left+2, rect.top+5, rect.Width()-4, 3,
				afxData.clrBtnHilite, afxData.clrBtnShadow);
		}

		GdiFlush();
	}
}

// input CRect should be client rectangle size
void CCJToolBarBase::CalcInsideRect(CRect& rect, BOOL bHorz) const
{
	ASSERT_VALID(this);
	DWORD dwStyle = m_dwStyle;
	
	if (dwStyle & CBRS_BORDER_LEFT)
		rect.left += afxData.cxBorder2;
	if (dwStyle & CBRS_BORDER_TOP)
		rect.top += afxData.cyBorder2;
	if (dwStyle & CBRS_BORDER_RIGHT)
		rect.right -= afxData.cxBorder2;
	if (dwStyle & CBRS_BORDER_BOTTOM)
		rect.bottom -= afxData.cyBorder2;
	
	// inset the top and bottom.
	if (bHorz)
	{
		rect.left += m_cxLeftBorder;
		rect.top += m_cyTopBorder;
		rect.right -= m_cxRightBorder;
		rect.bottom -= m_cyBottomBorder;
		if ((m_dwStyle & (CBRS_GRIPPER|CBRS_FLOATING)) == CBRS_GRIPPER)
		{
			rect.left += CX_BORDER_GRIPPER+CX_GRIPPER+CX_BORDER_GRIPPER;
		
			if( m_bExStyle == true ) {
				rect.left += CX_GRIPPER;
			}

			if(( m_bExStyle == false ) && ( m_bInReBar == false )) {
				rect.left -= CX_BORDER_GRIPPER+CX_GRIPPER;
			}
		}
	}
	else
	{
		rect.left += m_cyTopBorder;
		rect.top += m_cxLeftBorder;
		rect.right -= m_cyBottomBorder;
		rect.bottom -= m_cxRightBorder;
		if ((m_dwStyle & (CBRS_GRIPPER|CBRS_FLOATING)) == CBRS_GRIPPER)
		{
			rect.top += CY_BORDER_GRIPPER+CY_GRIPPER+CY_BORDER_GRIPPER;
		
			if( m_bExStyle == true ) {
				rect.top += CY_GRIPPER;
			}

			if(( m_bExStyle == false ) && ( m_bInReBar == false )) {
				rect.top -= CY_BORDER_GRIPPER+CY_GRIPPER;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CCJToolBarBase, CControlBar)

/////////////////////////////////////////////////////////////////////////////
