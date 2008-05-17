// CJTabCtrl.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 1:58p $
 * $Revision: 15 $
 * $Archive: /CodeJock/CJLibrary/CJTabCtrl.cpp $
 *
 * $History: CJTabCtrl.cpp $
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 1:58p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 9:44p
 * Updated in $/CodeJockey/CJLibrary
 * Added _DYNAMIC macros for access to the CObject::IsKindOf to determine
 * the class object at runtime. Thanks to Sid Price
 * [sidprice@softtools.com] for suggesting this.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 6/14/99    Time: 10:45p
 * Updated in $/CodeJockey/CJ60Lib
 * Cleaned up code to eliminate level 4 compilier warnings.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 8:01p
 * Updated in $/CodeJockey/CJ60Lib
 * Changed popup menu parent from GetParentFrame() to  AfxGetMainWnd()
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 5/11/99    Time: 9:33p
 * Updated in $/CodeJockey/CJ60Lib
 * Renamed popup menu resource id to m_popupMenuID to be consistent with
 * other classes.  Added member variable m_popToolbarID to contain the
 * toolbar resource to be used for popup menus  and m_nPos to contain the
 * index of the submenu in the menu resource. Added
 * CCJTabCtrl::Create(...) to simplify the creation process, and to set
 * the tab font.  Cleaned up OnRButtonDown(..) message handler which
 * activates the popup menu. Modified SetMenuID(...) to accept the index
 * of the popup menu in the menu resource.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 5:25p
 * Updated in $/CodeJockey/CJ60Lib
 * Removed class from CCJTabCtrlBar files into its own files. Added popup
 * menu support for CCJMenu.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 4:33p
 * Created in $/CodeJockey/CJ60Lib
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrl

CCJTabCtrl::CCJTabCtrl()
{
	m_popupMenuID	= 0;
	m_popToolbarID	= 0;
	m_nPos			= 0;
}

CCJTabCtrl::~CCJTabCtrl()
{
	// fix potential resource leak - KStowell - 10-22-99.
	m_Font.DeleteObject();
}

IMPLEMENT_DYNAMIC(CCJTabCtrl, CTabCtrl)

BEGIN_MESSAGE_MAP(CCJTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CCJTabCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_MEASUREITEM()
	ON_WM_MENUCHAR()
	ON_WM_INITMENUPOPUP()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJTabCtrl message handlers

void CCJTabCtrl::OnRButtonDown(UINT /*nFlags*/, CPoint point) 
{
	// No menu was defined...
	if (!m_popupMenuID) {
		TRACE0( "Warning: No menu resource was associated with CCJTabCtrl.\n" ); 
	}

	else
	{
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
}

void CCJTabCtrl::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
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

	CTabCtrl::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

LRESULT CCJTabCtrl::OnMenuChar(UINT nChar, UINT nFlags, CMenu* pMenu) 
{
	if (m_popupMenu.IsMenu(pMenu)) {
		return CCJMenu::FindKeyboardShortcut(nChar, nFlags, pMenu);
	}

	return CTabCtrl::OnMenuChar(nChar, nFlags, pMenu);
}

void CCJTabCtrl::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu) 
{
	CTabCtrl::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	
	if(!bSysMenu) {
		if (m_popupMenu.IsMenu(pPopupMenu)) {
			CCJMenu::UpdateMenu(pPopupMenu);
		}
	}
}

BOOL CCJTabCtrl::Create(CWnd* pParentWnd, UINT nID, DWORD dwStyle, const CRect& rect)
{
	if (!CTabCtrl::Create(dwStyle, rect, pParentWnd, nID))
		return FALSE;

	UpdateFont();
	return TRUE;
}

BOOL CCJTabCtrl::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

void CCJTabCtrl::UpdateFont()
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	VERIFY(m_Font.CreateFontIndirect(&ncm.lfMessageFont));
	SetFont(&m_Font);
}

void CCJTabCtrl::OnPaint() 
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

	// KStowell - Repaint the background.
	memDC.FillSolidRect( rcClient, afxData.clrBtnFace );

	// let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// KStowell - Copy the memory device context back into the original DC via BitBlt().
	dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, 0,0, SRCCOPY);

	// KStowell - Cleanup resources.
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}
