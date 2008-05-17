// CJFlatHeaderCtrl.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 13/06/02 16:44 $
 * $Revision: 3 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/CJLibrary/CJFlatHeaderCtrl.cpp $
 *
 * $History: CJFlatHeaderCtrl.cpp $
 * 
 * *****************  Version 3  *****************
 * User: Andy         Date: 13/06/02   Time: 16:44
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Added CCJFlatHeaderCtrl::GetSortImageWidth() to assist in column
 * autosizing
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 28/03/01   Time: 15:44
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/CJLibrary
 * Bug fix by Paulo Breda Vieira 1999-11-19
 * 
 * *****************  Version 21  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:01p
 * Updated in $/CodeJock/CJLibrary
 * Further Updates to OnEraseBkgnd(...) and OnPaint() for flicker free
 * drawing.
 * 
 * *****************  Version 20  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 2:11p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnPaint(...) for flicker free drawing and fixed bug where
 * column is resized and border leaves a trail.
 * 
 * *****************  Version 19  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 7:34p
 * Updated in $/CodeJock/CJLibrary
 * 1. Fixed bug with flat header control where header items were not drawn
 * correctly whenever header items were dragged to a new location. Ioannhs
 * Stamatopoulos (Yiannhs) [ystamat@mail.datamedia.gr]
 * 
 * *****************  Version 18  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 17  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 8:23p
 * Updated in $/CodeJock/CJLibrary
 * Fixed size bug with header control where the end does not display
 * correctly.
 * 
 * *****************  Version 16  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:27p
 * Updated in $/CodeJockey/CJLibrary
 * Added vc5 compatibility.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:43p
 * Updated in $/CodeJockey/CJ60Lib
 * Cleaned up code to eliminate level 4 compilier warnings.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 8:00p
 * Updated in $/CodeJockey/CJ60Lib
 * Added popup menu support and vc5 compatibility.
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 5/12/99    Time: 5:32a
 * Updated in $/CodeJockey/CJ60Lib
 * Added support for vc5 compatibility
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 4/15/99    Time: 4:27p
 * Updated in $/CodeJockey/CJ60Lib
 * Igor Ostriz [iostriz@usa.net] - fixed bug with drag/drop and changing
 * column order.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 4:26p
 * Updated in $/CodeJockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 4:26p
 * Updated in $/CodeJockey/CJ60Lib
 * Fixed repainting problem when header is moved or window is resized. The
 * class now uses log font instead of hard coded Tahoma font.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 1/31/99    Time: 4:27p
 * Updated in $/CodeJockey/CJ60Lib
 * Fixed problem with sort arrow drawn. If hditem has an associated
 * bitmap, the arrow is not drawn.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 4:26p
 * Created in $/CodeJockey/CJ60Lib
 * Initial release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJFlatHeaderCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJFlatHeaderCtrl

CCJFlatHeaderCtrl::CCJFlatHeaderCtrl()
{
	m_nSortCol		= -1;
	m_bBoldFont		= FALSE;
	m_nOffset		= 6;
	m_bLBtnDown		= FALSE;
	m_popupMenuID	= 0;
	m_popToolbarID	= 0;
	m_nPos			= 0;
	m_pParentWnd    = NULL;
}

CCJFlatHeaderCtrl::~CCJFlatHeaderCtrl()
{
	// fix potential resource leak - KStowell - 10-22-99.
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CCJFlatHeaderCtrl, CHeaderCtrl)
	//{{AFX_MSG_MAP(CCJFlatHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
//	ON_WM_WINDOWPOSCHANGED()	// PBV
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJFlatHeaderCtrl message handlers

void CCJFlatHeaderCtrl::InitializeHeader(bool bBoldFont)
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	if(bBoldFont)
		ncm.lfMessageFont.lfWeight = 700;
	VERIFY(m_Font.CreateFontIndirect(&ncm.lfMessageFont));
	SetFont(&m_Font);

	m_bBoldFont = bBoldFont;
}

int CCJFlatHeaderCtrl::SetSortImage(int nCol, BOOL bAsc)
{
	int nPrevCol = m_nSortCol;
	
	m_nSortCol = nCol;
	m_bSortAsc = bAsc;
	
	// Change the item to ownder drawn
	HD_ITEM hditem;
	
	hditem.mask = HDI_BITMAP | HDI_FORMAT;
	GetItem( nCol, &hditem );
	if (hditem.hbm == NULL)
	{
		hditem.fmt |= HDF_OWNERDRAW;
		SetItem( nCol, &hditem );

		// Invalidate header control so that it gets redrawn
		Invalidate();
	}
	
	return nPrevCol;
}

void CCJFlatHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct != NULL);

	// define some temporary variables.
	CDC*	pDC			= CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect	rcItem		= lpDrawItemStruct->rcItem;
	CRect	rcIcon		= lpDrawItemStruct->rcItem;
    int 	nState		= lpDrawItemStruct->itemState;
	int		nItemID		= lpDrawItemStruct->itemID;
	int		nSavedDC	= pDC->SaveDC();

	// Paint the background.
	pDC->FillSolidRect(rcItem, afxData.clrBtnFace);
	pDC->SetBkMode(TRANSPARENT);
	
	// Set clipping region to limit drawing within column
	CRgn rgn;
	rgn.CreateRectRgnIndirect( &rcItem );
	pDC->SelectObject( &rgn );
	rgn.DeleteObject();

	// Get the column text and format
	TCHAR buf[256];
	HD_ITEM hditem;
	
	hditem.mask = HDI_TEXT | HDI_FORMAT;
	hditem.pszText = buf;
	hditem.cchTextMax = 255;
	
	GetItem( nItemID, &hditem );
	
	// Determine format for drawing column label
	UINT uFormat = DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP 
		| DT_VCENTER | DT_END_ELLIPSIS ;
	
	if( hditem.fmt & HDF_CENTER) {
		uFormat |= DT_CENTER;
	} else if( hditem.fmt & HDF_RIGHT) {
		uFormat |= DT_RIGHT;
	} else {
		uFormat |= DT_LEFT;
	}
	
	// Adjust the rect if the mouse button is pressed on it
	if( nState == ODS_SELECTED ) {
		rcItem.left++;
		rcItem.top += 2;
		rcItem.right++;
	}
	
	// Adjust the rect further if Sort arrow is to be displayed
	if( nItemID == m_nSortCol ) {
		rcItem.right -= GetSortImageWidth();
	}
	
	rcItem.left += m_nOffset;
	rcItem.right -= m_nOffset;

	// Draw column label
	if( rcItem.left < rcItem.right ) {
		if (m_bBoldFont) {
			CFont* pOldFont = pDC->SelectObject(&m_Font);
			pDC->DrawText(buf,-1,rcItem, uFormat);
			pDC->SelectObject(pOldFont);
		} else {
			pDC->DrawText(buf,-1,rcItem, uFormat);
		}
	}

	// Draw the Sort arrow
	if( nItemID == m_nSortCol )
	{
		// Set up pens to use for drawing the triangle
		CPen penLite(PS_SOLID, 1, afxData.clrBtnHilite);
		CPen penShad(PS_SOLID, 1, afxData.clrBtnShadow);
		CPen *pOldPen = pDC->SelectObject( &penLite );
		
		if( m_bSortAsc )
		{
			// Draw triangle pointing upwards
			pDC->MoveTo( rcIcon.right - 2*m_nOffset, m_nOffset-1);
			pDC->LineTo( rcIcon.right - 3*m_nOffset/2, rcIcon.bottom - m_nOffset );
			pDC->LineTo( rcIcon.right - 5*m_nOffset/2-2, rcIcon.bottom - m_nOffset );
			pDC->MoveTo( rcIcon.right - 5*m_nOffset/2-1, rcIcon.bottom - m_nOffset-1 );
			
			pDC->SelectObject( &penShad );
			pDC->LineTo( rcIcon.right - 2*m_nOffset, m_nOffset-2);
		}
		else
		{
			// Draw triangle pointing downwords
			pDC->MoveTo( rcIcon.right - 3*m_nOffset/2, m_nOffset-1);
			pDC->LineTo( rcIcon.right - 2*m_nOffset-1, rcIcon.bottom - m_nOffset + 1 );
			pDC->MoveTo( rcIcon.right - 2*m_nOffset-1, rcIcon.bottom - m_nOffset );
			
			pDC->SelectObject( &penShad );
			pDC->LineTo( rcIcon.right - 5*m_nOffset/2-1, m_nOffset -1 );
			pDC->LineTo( rcIcon.right - 3*m_nOffset/2, m_nOffset -1);
		}
		
		// Restore the pen
		pDC->SelectObject( pOldPen );

		// fix potential resource leak - KStowell - 10-21-99
		penLite.DeleteObject();
		penShad.DeleteObject();
	}

	// restore the device context
	pDC->RestoreDC(nSavedDC);
}

void CCJFlatHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// KStowell - Get the client rect.
	CRect rcClient;
	GetClientRect( &rcClient );

	// KStowell - Create a memory device-context. This is done to help reduce
	// screen flicker, since we will paint the entire control to the
	// off screen device context first.
	CDC memDC;
	CBitmap bitmap;
	memDC.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rcClient.Width(), rcClient.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.FillSolidRect(rcClient, afxData.clrBtnFace);
	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// KStowell - Repaint the background.
	DrawFlatBorder(&memDC);

	// KStowell - Copy the memory device context back into the original DC via BitBlt().
	dc.BitBlt( rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, 
		rcClient.left, rcClient.top, SRCCOPY );

	// KStowell - Cleanup resources.
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}


// Changes in code by Paulo Breda Vieira 1999-11-19
void CCJFlatHeaderCtrl::DrawFlatBorder(CDC* pDC)
{
	CRect rci;
	GetWindowRect(&rci);
	ScreenToClient(&rci);

	// Draw flat style border around entire header.
	rci.InflateRect(0,0,1,0);	// Breda
	pDC->Draw3dRect(rci, afxData.clrBtnHilite, afxData.clrBtnShadow);

	// Cover up thick 3D border.
//	pDC->Draw3dRect(rci, afxData.clrBtnFace, afxData.clrBtnFace);	// Breda
	rci.DeflateRect(1,1,0,1);	// Breda
	pDC->Draw3dRect(rci, afxData.clrBtnFace, afxData.clrBtnFace);
	
	// Create the pens for further cover-up.
	CPen penLite(PS_SOLID, 1, afxData.clrBtnHilite);
	CPen penShad(PS_SOLID, 1, afxData.clrBtnShadow);
	CPen penFace(PS_SOLID, 1, afxData.clrBtnFace);
	CPen *pOldPen = pDC->SelectObject( &penLite );
	
	// Set up the header item struct.
	HD_ITEM hdi;
	memset (&hdi, 0, sizeof(HD_ITEM));
	hdi.fmt  = HDF_STRING | HDF_LEFT | HDF_OWNERDRAW;
	hdi.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT | HDI_ORDER; // Yiannhs :1999-10-19
	int cx = 0;
	
    long j = GetItemCount();    //Yiannhs
    long i = 0;                 //Yiannhs
	
    long * ItemOrders = new long[j];    //Yiannhs :1999-10-19
    for( i=0; i<j; i++ ) {				//Yiannhs :1999-10-19
		GetItem(i, &hdi);				//Yiannhs :1999-10-19
		ItemOrders[hdi.iOrder] = i;     //Yiannhs :1999-10-19
    }
	
	hdi.fmt  = HDF_STRING | HDF_LEFT | HDF_OWNERDRAW;   //Yiannhs :1999-10-19
	hdi.mask = HDI_WIDTH | HDI_TEXT | HDI_FORMAT;       //Yiannhs :1999-10-19

	// For each header item found, do further cover up.
	for (i = 0; i < j; ++i)
	{
		GetItem( ItemOrders[i], &hdi);
		cx += hdi.cxy;
		
		pDC->SelectObject(&penShad);
		pDC->MoveTo(cx, 2);
		pDC->LineTo(cx, rci.bottom-1);		// Breda
		
		pDC->SelectObject(&penLite);
		pDC->MoveTo(cx+1, 2);
		pDC->LineTo(cx+1, rci.bottom-1);	// Breda
		
		pDC->SelectObject(&penFace);
		pDC->MoveTo(cx-1, 2);
		pDC->LineTo(cx-1, rci.bottom-1);	// Breda
		
		pDC->SelectObject(&penFace);
		pDC->MoveTo(cx-2, 2);
		pDC->LineTo(cx-2, rci.bottom-1);	// Breda
	}
	
	delete [] ItemOrders; //Yiannhs :1999-10-19
	
	// Restore the pen and release device context.
	pDC->SelectObject( pOldPen );

	// fix potential resource leak - KStowell - 10-21-99
	penLite.DeleteObject();
	penShad.DeleteObject();
	penFace.DeleteObject();
}

void CCJFlatHeaderCtrl::OnRButtonDown(UINT /*nFlags*/, CPoint point) 
{
	// No menu was defined...
	if (!m_popupMenuID) {
		TRACE0( "Warning: No menu resource was associated with CCJTabCtrl.\n" ); 
		return;
	}

	// Since we are using the main window as the
	// parent, we need to convert the point to screen
	// coordinates...
	CPoint pt = point;
	ClientToScreen(&pt);
	
	// Load the popup menu.
	VERIFY(m_popupMenu.LoadMenu(m_popupMenuID));
	CCJMenu* pPopup = (CCJMenu*)m_popupMenu.GetSubMenu(m_nPos);
	ASSERT(pPopup != NULL);
	
	// Load toolbar resource if any for menu icons.
	if (m_popToolbarID) {
		pPopup->LoadToolbar(m_popToolbarID);
	}
	
	// Display the popup menu, use the main window as its parent.
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
		pt.x, pt.y, AfxGetMainWnd());
	
	m_popupMenu.DestroyMenu();
}

void CCJFlatHeaderCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
	if (lpMeasureItemStruct->CtlType == ODT_MENU) 
	{
		if (IsMenu((HMENU)lpMeasureItemStruct->itemID)) {
			CMenu* pMenu = CMenu::FromHandle((HMENU)lpMeasureItemStruct->itemID);
			
			if (m_popupMenu.IsMenu(pMenu)) {
				m_popupMenu.MeasureItem(lpMeasureItemStruct);
				return;
			}
		}
	}

	CHeaderCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CCJFlatHeaderCtrl::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	if (m_popupMenu.IsMenu(pMenu)) {
		return CCJMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	}

	return CHeaderCtrl::OnMenuChar(nChar, nFlags, pMenu);
}

void CCJFlatHeaderCtrl::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CHeaderCtrl::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if(!bSysMenu) {
		if (m_popupMenu.IsMenu(pPopupMenu)) {
			CCJMenu::UpdateMenu(pPopupMenu);
		}
	}
}

#ifdef _VC_VERSION_5
BOOL CCJFlatHeaderCtrl::GetOrderArray(LPINT piArray, int iCount /* = -1 */)
{
	ASSERT(::IsWindow(m_hWnd));

	// if -1 was passed, find the count ourselves

	int nCount = iCount;
	if (nCount == -1)
	{
		nCount = GetItemCount();

		if (nCount == -1)
			return FALSE;
	}

	ASSERT(AfxIsValidAddress(piArray, iCount * sizeof(int)));

	return (BOOL) ::SendMessage(m_hWnd, HDM_GETORDERARRAY,
		(WPARAM) iCount, (LPARAM) piArray);
}
#endif // _VC_VERSION_5

void CCJFlatHeaderCtrl::PreSubclassWindow() 
{
	CHeaderCtrl::PreSubclassWindow();

	m_pParentWnd = GetParent();
	ASSERT(m_pParentWnd);

// Changes in code by Paulo Breda Vieira 1999-11-19
//	ResizeWindow();
}

int CCJFlatHeaderCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CHeaderCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_pParentWnd = CWnd::FromHandle(lpCreateStruct->hwndParent);
	ASSERT(m_pParentWnd);

// Changes in code by Paulo Breda Vieira 1999-11-19
//	ResizeWindow();
	return 0;
}

// Changes in code by Paulo Breda Vieira 1999-11-19
//void CCJFlatHeaderCtrl::OnWindowPosChanged(WINDOWPOS FAR* lpwndpos) 
//{
//	CHeaderCtrl::OnWindowPosChanged(lpwndpos);
//	ResizeWindow();
//}
	
// Changes in code by Paulo Breda Vieira 1999-11-19
//void CCJFlatHeaderCtrl::ResizeWindow()
//{
//	ASSERT_VALID(this);
//	ASSERT_VALID(m_pParentWnd);
//
//	// Get the client size of the parent window, this will give us the
//	// actual width of the header we are looking for.
//	CRect rcParent;
//	m_pParentWnd->GetClientRect(&rcParent);
//
//	// Get the size of the header control, we will need this for the
//	// height of the header.
//	CRect rcWnd;
//	GetWindowRect(&rcWnd);
//
//	// Now, resize the window.
//	SetWindowPos(NULL, 0, 0, rcParent.Width(), rcWnd.Height(),
//		SWP_NOMOVE/*|SWP_FRAMECHANGED*/);
//}

BOOL CCJFlatHeaderCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}
