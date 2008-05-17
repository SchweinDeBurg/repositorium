// CJOutlookBar.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 2:03p $
 * $Revision: 11 $
 * $Archive: /CodeJock/CJLibrary/CJOutlookBar.cpp $
 *
 * $History: CJOutlookBar.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:03p
 * Updated in $/CodeJock/CJLibrary
 * Added COLORREF members to class to eliminate continuous calls to
 * GetSysColor(...). Overrode OnSysColorChange() to re-initialize COLORREF
 * variables. 
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:44p
 * Updated in $/CodeJockey/CJ60Lib
 * Cleaned up code to eliminate level 4 compilier warnings.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 10:43p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 3/03/99    Time: 10:43p
 * Updated in $/CodeJockey/CJ60Lib
 * Changed images list's to pointer vars passed into class.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 10:42p
 * Updated in $/CodeJockey/CJ60Lib
 * Christopher Brown (Allknowing@earthlink.net) some modifications to
 * correct icon resource leak, and eliminate screen flicker.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 10:42p
 * Created in $/CodeJockey/CJ60Lib
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJOutlookBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJOutlookBar

CCJOutlookBar::CCJOutlookBar()
{
    m_bLBDown			= false;
    m_bHilight			= true;
	m_pImageListLarge	= NULL;
	m_pImageListSmall	= NULL;
	m_pOwner			= NULL;
	m_clrHilite			= ::GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrShadow			= ::GetSysColor(COLOR_BTNSHADOW);
	m_clrDkShad			= ::GetSysColor(COLOR_3DDKSHADOW);
	m_clrNormal			= ::GetSysColor(COLOR_BTNFACE);
	m_clrTextNm			= ::GetSysColor(COLOR_WINDOW);

	UpdateFont();
}

CCJOutlookBar::~CCJOutlookBar()
{
	// fix potential resource leak - KStowell - 10-22-99.
	m_Font.DeleteObject();
}

IMPLEMENT_DYNAMIC(CCJOutlookBar, CListBox)

BEGIN_MESSAGE_MAP(CCJOutlookBar, CListBox)
	//{{AFX_MSG_MAP(CCJOutlookBar)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJOutlookBar message handlers

// Checks mouse movement, sets point member, sets timer.
void CCJOutlookBar::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_point = point;
	SetTimer (1, 10, NULL);
	CListBox::OnMouseMove(nFlags, point);
}

// Sets bLBDown member to true, and sets current selection
void CCJOutlookBar::OnLButtonDown(UINT nFlags, CPoint point) 
{
    m_bLBDown = true;
    SetCurSel(GetCurSel());
	CListBox::OnLButtonDown(nFlags, point);
}

// Sets bLBDown member to false, and sets current selection
void CCJOutlookBar::OnLButtonUp(UINT nFlags, CPoint point) 
{
    m_bLBDown = false;
	int nIndex = GetCurSel();
    SetCurSel(nIndex);

	ASSERT(m_pOwner);
	m_pOwner->SendMessage(OBN_SELENDOK, (LPARAM)nIndex, (WPARAM)GetDlgCtrlID());

	CListBox::OnLButtonUp(nFlags, point);
}

// Sets the hilite flag based on cursor position.
void CCJOutlookBar::OnTimer(UINT /*nIDEvent*/) 
{
    BOOL   bOutSide;
    CRect  rItem;
	CRect  rWindow;
	CPoint pt;
    int    nIndex;

    // Get the rect of the current window,
	// get the current cursor position.
	GetWindowRect (rWindow);
    GetCursorPos (&pt);

    // If the mouse is in the rect of this window,
	// set hilite flag to true, get the item that
	// the mouse is over.
	if (rWindow.PtInRect (pt)) {
		m_bHilight = true;
		nIndex = ItemFromPoint (m_point, bOutSide);

		if (nIndex != GetCurSel() && !bOutSide) {
			SetCurSel( nIndex );
		}
    }

    // If the mouse is not in the rect of
	// this window, kill the timer, set hilite flag
	// to false, and return.
	else {
		m_bHilight = false;
        if( !m_bLBDown ) {
          SetCurSel (-1);
        }
        KillTimer(1);
	}
}

//////////////////////////////
// Initial draw routine, draws button on mouse over,
// on mouse press, and on mouse out.

void CCJOutlookBar::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    CDC*            pDC = CDC::FromHandle( lpDrawItemStruct->hDC );
    CRect           rcItem(lpDrawItemStruct->rcItem);
    CRect           rClient(rcItem);
    CRect           rText;
    CPoint          Pt(((rcItem.Width() - 32) /2 ), rcItem.top + 15);
    CContentItems   pContents = m_pContents[ lpDrawItemStruct->itemID ];
    HICON           hIcon = m_pImageListLarge->ExtractIcon( pContents.GetImageID());

	pDC->SetBkColor(m_clrShadow);

	// deflate the client rect, and set up item and
	// text rect's.
	rClient.DeflateRect( 8, 5 );
	rcItem.CopyRect( rClient );
    rText.CopyRect( rClient );
    rText.top += 45;
	rText.bottom += 10;

    // set the text color to white, and background mode
	// to transparent.
	pDC->SetTextColor( m_clrTextNm );
	pDC->SetBkMode( TRANSPARENT );
	CFont* pOldFont = pDC->SelectObject(&m_Font);

	// draw the button icon and text.
	if( lpDrawItemStruct->itemAction& ODA_DRAWENTIRE ) {
        pDC->DrawIcon( Pt, hIcon );
        pDC->DrawText( pContents.GetText(), rText, DT_CENTER | DT_WORDBREAK );
    }

    // if the left mouse button is pressed, offset
	// the button icon.
	if( m_bLBDown ) {
        Pt.x += 2;
        Pt.y += 2;
        rcItem.OffsetRect( 2, 2 );
    }

	// deflate the button rect, and move up.
	rcItem.DeflateRect( 4, 16 );
	rcItem.top -= 8;

	// Draw the button icon and text, if the mouse is over the
	// button, draw the 3d rect, either pressed,
	// or not pressed depending on left mouse button.
	if(( lpDrawItemStruct->itemState& ODS_SELECTED ) &&
		( lpDrawItemStruct->itemAction& ( ODA_SELECT | ODA_DRAWENTIRE ))) {
        
		rcItem.left = ( rcItem.Width()/2 )-5;
		rcItem.right = rcItem.left+37;

		if( m_bHilight ) {
            if( m_bLBDown ) {
              pDC->Draw3dRect( rcItem, m_clrDkShad, m_clrNormal );
			} else {
              pDC->Draw3dRect( rcItem, m_clrNormal, m_clrDkShad );
			}
        }

        pDC->DrawIcon( Pt, hIcon );
        pDC->DrawText( pContents.GetText(), rText, DT_CENTER | DT_WORDBREAK );
	}

	if( !( lpDrawItemStruct->itemState& ODS_SELECTED ) &&
		( lpDrawItemStruct->itemAction& ODA_SELECT )) {
        pDC->FillSolidRect( rClient, m_clrShadow );
        pDC->DrawIcon(Pt, hIcon);
        pDC->DrawText(pContents.GetText(), rText, DT_CENTER|DT_WORDBREAK);
    }

    // To avoid ugly resource leaks that will 
    // eventually TRASH the system, call DestroyIcon().
    // To reproduce bug, try resizing the window for a 
    // good minute. Christopher Brown

    ::DestroyIcon( hIcon );
	pDC->SelectObject(pOldFont);
}

//////////////////////////////
// Sets item height

void CCJOutlookBar::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	lpMeasureItemStruct->itemHeight = 70;
}

//////////////////////////////
// Sets the background color.

BOOL CCJOutlookBar::OnEraseBkgnd(CDC* pDC)
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

void CCJOutlookBar::OnPaint() 
{
    // Christopher Brown
    // Use a "Offscreen" DC to fill rect and  
    // send to DefWindowProc...
	CClientDC memDC(this); // Henrik E. Rasmussen
	
    CRect rcClip;
    memDC.GetClipBox(&rcClip);
    memDC.FillSolidRect(rcClip, m_clrShadow);
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, (LPARAM)0 );
	
    // Default is called for cleanup.
    Default();
}

void CCJOutlookBar::UpdateFont()
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	VERIFY(m_Font.CreateFontIndirect(&ncm.lfMessageFont));
}

void CCJOutlookBar::OnSysColorChange() 
{
	CListBox::OnSysColorChange();
	
	m_clrHilite	= ::GetSysColor(COLOR_BTNHIGHLIGHT);
	m_clrShadow	= ::GetSysColor(COLOR_BTNSHADOW);
	m_clrDkShad	= ::GetSysColor(COLOR_3DDKSHADOW);
	m_clrNormal	= ::GetSysColor(COLOR_BTNFACE);
	m_clrTextNm = ::GetSysColor(COLOR_WINDOW);
}
