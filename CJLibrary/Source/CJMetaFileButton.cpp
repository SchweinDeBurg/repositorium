// CJMetaFileButton.cpp : implementation file
//
// Copyright © 1999 Ian Brumby
//
// This source code may be used in compiled form in any way you desire. 
// Source file(s) may be redistributed unmodified by any means PROVIDING
// they are not sold for profit without the authors expressed written consent,
// and providing that this notice and the authors name and all copyright
// notices remain intact.
//
// ==========================================================================  
// HISTORY:	  
// ==========================================================================  
//			1.20	21 Apr 1999	- Initial release.
// ==========================================================================  
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:04p $
 * $Revision: 6 $
 * $Archive: /CodeJock/CJLibrary/CJMetaFileButton.cpp $
 *
 * $History: CJMetaFileButton.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:04p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/25/99   Time: 10:52p
 * Updated in $/CodeJock/CJLibrary
 * Modified resource include for static builds.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 12:01a
 * Updated in $/CodeJock/CJLibrary
 * Fixed potential resource and memory leak problems.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:41p
 * Updated in $/CodeJock/CJLibrary
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJResource.h"
#include "CJMetaFileButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJMetaFileButton

CCJMetaFileButton::CCJMetaFileButton()
{
}

CCJMetaFileButton::~CCJMetaFileButton()
{
}

void CCJMetaFileButton::SetMetaFiles(HENHMETAFILE hMetaFile,
	HENHMETAFILE hMetaFileSel, HENHMETAFILE hMetaFileFocus, HENHMETAFILE hMetaFileDisabled)
{
	m_hEnhMetaFile         = hMetaFile;
	m_hEnhMetaFileSel      = hMetaFileSel;
	m_hEnhMetaFileFocus    = hMetaFileFocus;
	m_hEnhMetaFileDisabled = hMetaFileDisabled;
}


BEGIN_MESSAGE_MAP(CCJMetaFileButton, CButton)
	//{{AFX_MSG_MAP(CCJMetaFileButton)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJMetaFileButton message handlers

void CCJMetaFileButton::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Fix for the double click problem with owner drawn buttons
	POINT pt;
	::GetCursorPos(&pt);
	::MapWindowPoints(NULL, (HWND)(m_hWnd), &pt, 1);
	::SendMessage((HWND)(m_hWnd), WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));	
	CButton::OnLButtonDblClk(nFlags, point);
}

void CCJMetaFileButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	ASSERT(lpDrawItemStruct != NULL);

	// define some temporary variables.
	CDC*  pDC	 = CDC::FromHandle( lpDrawItemStruct->hDC );
	CRect rcItem = lpDrawItemStruct->rcItem;
	int   nState = lpDrawItemStruct->itemState;

	// Paint the background.
	pDC->FillSolidRect(rcItem, ::GetSysColor(COLOR_3DFACE));
	
	// draw 3D button outline
	CPen penOutline(PS_SOLID, 1, GetSysColor(COLOR_BTNTEXT));
	CPen penLight(PS_SOLID, 1, GetSysColor(COLOR_BTNHIGHLIGHT));
	CPen penDark(PS_SOLID, 1, GetSysColor(COLOR_BTNSHADOW));
	CPen* pOldPen = NULL;

	if (nState & ODS_SELECTED)
	{
		pOldPen = pDC->SelectObject(&penDark);
		pDC->MoveTo(0, rcItem.bottom - 1);
		pDC->LineTo(0, 1);
		pDC->LineTo(rcItem.right - 1, 1);
	}
	else
	{
		pOldPen = pDC->SelectObject(&penLight);
		pDC->MoveTo(0, rcItem.bottom - 2);
		pDC->LineTo(0, 1);
		pDC->LineTo(rcItem.right - 2, 1);
		pDC->SelectObject(&penDark);
		pDC->MoveTo(0, rcItem.bottom - 1);
		pDC->LineTo(rcItem.right - 2, rcItem.bottom - 1);
		pDC->LineTo(rcItem.right - 2, 0);
		pDC->MoveTo(1, rcItem.bottom - 2);
		pDC->LineTo(rcItem.right - 3, rcItem.bottom - 2);
		pDC->LineTo(rcItem.right - 3, 1);
	}
	pDC->SelectObject(&penOutline);
	pDC->MoveTo(0, 0);
	pDC->LineTo(rcItem.right - 1, 0);
	pDC->LineTo(rcItem.right - 1, rcItem.bottom);
	
	HENHMETAFILE hMetaFile = m_hEnhMetaFile;
	if ((nState & ODS_SELECTED) && m_hEnhMetaFileSel != NULL)
		hMetaFile = m_hEnhMetaFileSel;
	else if ((nState & ODS_FOCUS) && m_hEnhMetaFileFocus != NULL)
		hMetaFile = m_hEnhMetaFileFocus;   // third image for focused
	else if ((nState & ODS_DISABLED) && m_hEnhMetaFileDisabled != NULL)
		hMetaFile = m_hEnhMetaFileDisabled;   // last image for disabled
	
	ASSERT(hMetaFile != NULL);
	
	CRect rcBounds(5,4,0,0);
	int iButtonHeight = rcItem.Height();
	if (iButtonHeight > 15)
		iButtonHeight = 15;
	else if (iButtonHeight < 11)
		iButtonHeight = 11;
	int iArrowWidth = (iButtonHeight / 2) - 3;
	rcBounds.right = rcBounds.left + iArrowWidth - 1;
	rcBounds.bottom = rcBounds.top + (iArrowWidth * 2) - 2;
	
	if (nState & ODS_SELECTED)
		rcBounds += CPoint(1, 1);
	pDC->PlayMetaFile(hMetaFile, rcBounds);

	// fix potential resource leak - KStowell - 10-21-99
	pDC->SelectObject(pOldPen);
	penOutline.DeleteObject();
	penLight.DeleteObject();
	penDark.DeleteObject();
}

BOOL CCJMetaFileButton::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

void CCJMetaFileButton::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// KStowell - Get the client rect.
	CRect rcClient, rcClip;
	dc.GetClipBox( &rcClip );
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
	memDC.FillSolidRect( rcClient, ::GetSysColor(COLOR_WINDOW) );

	// let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// KStowell - Copy the memory device context back into the original DC via BitBlt().
	dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
		rcClip.left, rcClip.top, SRCCOPY );

	// KStowell - Cleanup resources.
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}
