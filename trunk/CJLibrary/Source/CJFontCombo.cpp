/////////////////////////////////////////////////////////////////////////////
// CJFontCombo.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based on the article 'Owner Drawn Font Selection ComboBox' by Girish Bharadwaj
// http://www.codeguru.com/misc/font_selection_combo.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/31/99 11:01p $
 * $Revision: 6 $
 * $Archive: /CodeJock/CJLibrary/CJFontCombo.cpp $
 *
 * $History: CJFontCombo.cpp $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:01p
 * Updated in $/CodeJock/CJLibrary
 * Overrode OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:53p
 * Updated in $/CodeJock/CJLibrary
 * Added the DECLARE_DYNAMIC and IMPLEMENT_DYNAMIC macros to CCJFontCombo
 * for access to run-time information.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/CJLibrary
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/26/99    Time: 2:11p
 * Created in $/CodeJockey/CJLibrary
 * 
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJFontCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////
// CCJFontCombo

CCJFontCombo::CCJFontCombo()
{
     m_clrHilight		= ::GetSysColor(COLOR_HIGHLIGHT);
     m_clrNormalText	= ::GetSysColor(COLOR_WINDOWTEXT);
     m_clrHilightText	= ::GetSysColor(COLOR_HIGHLIGHTTEXT);
     m_clrBkgnd			= ::GetSysColor(COLOR_WINDOW);
}

CCJFontCombo::~CCJFontCombo()
{
}

IMPLEMENT_DYNAMIC(CCJFontCombo, CComboBox)

BEGIN_MESSAGE_MAP(CCJFontCombo, CComboBox)
	//{{AFX_MSG_MAP(CCJFontCombo)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
// CCJFontCombo message handlers

void CCJFontCombo::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC*	  pDC	   = CDC::FromHandle(lpDrawItemStruct->hDC);
	CRect	  rect	   = lpDrawItemStruct->rcItem;
	int		  nState   = lpDrawItemStruct->itemState;
	int		  nItem	   = lpDrawItemStruct->itemID;
	LPLOGFONT pLogFont = (LPLOGFONT)lpDrawItemStruct->itemData;

	pDC->SetBkMode( TRANSPARENT );

	if( nState & ODS_SELECTED ) {
		pDC->FillSolidRect( rect, m_clrHilight );
		pDC->SetTextColor( m_clrHilightText );
	}
	else {
		pDC->FillSolidRect( rect, m_clrBkgnd );
		pDC->SetTextColor( m_clrNormalText );
	}

	if( nItem >= 0 )
	{
		CString strText;
		GetLBText( nItem, strText );
		
		CFont newFont;
		pLogFont->lfHeight = 90; //9 point size
		pLogFont->lfWidth  = 0;
		newFont.CreatePointFontIndirect( pLogFont );

		CFont *pOldFont = pDC->SelectObject( &newFont );
		pDC->DrawText(strText, rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject( pOldFont );
		newFont.DeleteObject();
	}
}

void CCJFontCombo::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	UNUSED_ALWAYS( lpMeasureItemStruct );
}

void CCJFontCombo::InitializeFonts()
{
	// Clear the combobox.
     ResetContent();

	 HDC hDC  = ::GetDC(m_hWnd);
     ::EnumFonts( hDC, NULL, (FONTENUMPROC)EnumFontProc, (LPARAM)this );//Enumerate
	 ::ReleaseDC(m_hWnd, hDC);
}

BOOL CALLBACK CCJFontCombo::EnumFontProc(LPLOGFONT lpLogFont, LPTEXTMETRIC lpTextMetric, short nFontType, LPHANDLE lpData)
{
	UNUSED_ALWAYS( lpTextMetric );

	if((nFontType == TRUETYPE_FONTTYPE) && (lpLogFont->lfCharSet != SYMBOL_CHARSET))
     {
		 CCJFontCombo* pCombo = (CCJFontCombo*)lpData;
		 ASSERT(pCombo);

         int nIndex = pCombo->AddString(lpLogFont->lfFaceName);
		 LPLOGFONT pLogFont = new LOGFONT;
		 
		 ::CopyMemory((PVOID)pLogFont, (CONST VOID*)lpLogFont, (DWORD)sizeof( LOGFONT ));
		 pCombo->SetItemData( nIndex, (DWORD)pLogFont );
     }

     return TRUE;
}

void CCJFontCombo::OnDestroy()
{
	for( int nIndex = 0; nIndex <  GetCount(); ++nIndex ) {
		LPLOGFONT pLogFont = (LPLOGFONT)GetItemData(nIndex);
		_delete( pLogFont );
	}

     CComboBox::OnDestroy();
}

int CCJFontCombo::GetSelFont(LOGFONT& logFont)
{
     int nIndex = GetCurSel ();
     if(nIndex != LB_ERR)
	 {
		LPLOGFONT pLogFont = (LPLOGFONT)GetItemData(nIndex);
		::CopyMemory((PVOID)&logFont, (CONST VOID*)pLogFont, sizeof(LOGFONT));
		return nIndex;
	 }
      
	 return LB_ERR;
}

int CCJFontCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CComboBox::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeFonts();
	return 0;
}

void CCJFontCombo::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();
	InitializeFonts();
}

BOOL CCJFontCombo::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}

void CCJFontCombo::OnPaint() 
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
	memDC.FillSolidRect(rcClient, ::GetSysColor(COLOR_WINDOW));

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
