// CJCaption.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/05/02 15:54 $
 * $Revision: 2 $
 * $Archive: /Projects/Libraries/CJLibrary/CJLibrary/CJCaption.cpp $
 *
 * $History: CJCaption.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 10/05/02   Time: 15:54
 * Updated in $/Projects/Libraries/CJLibrary/CJLibrary
 * Changes for compatibility with VC 7.0
 * 
 * *****************  Version 17  *****************
 * User: Kirk Stowell Date: 10/31/99   Time: 11:41p
 * Updated in $/CodeJock/CJLibrary
 * Updated OnEraseBkgnd(...) and OnPaint() for flicker free drawing.
 * 
 * Fixed bug when popup window is closed, the mainframe resizes and then
 * the popup is reopened and pushpin is pressed, the child will not be
 * correctly sized and positioned. Added changes to handle flicker free
 * drawing of the control window. 
 * 
 * Modified resource include for static builds.
 * 
 * Fixed the bug where the icon on the right hand side of the bar doesn't
 * clip when you resize the bar to almost nothing. Added method
 * SetTextStyle(...) to set the CDC:: DrawText(...) text styles to be user
 * defined. Thanks to Adrian for help with this.
 * 
 * Fixed potential resource and memory leak problems.
 * 
 * Fixed small bug with UNICODE display placing unwanted character at the
 * end of the string.
 * 
 * Made class methods virtual for inheritance purposes.
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
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:36a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 7/24/99    Time: 9:32p
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/08/99    Time: 2:43a
 * Updated in $/CodeJockey/CJLibrary
 * Amundsen - Eirik [eirik.amundsen@siemens.no] - Fixed problem with
 * caption repainting when banner is smaller than displayed text.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 5/17/99    Time: 8:13p
 * Updated in $/CodeJockey/CJ60Lib
 * Fixed painting bug with border, cleaned up inline methods.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 12:35p
 * Updated in $/Code Jockey/CJ60Lib
 * Added comments and cleaned up code.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 3/03/99    Time: 12:27p
 * Updated in $/CodeJockey/CJ60Lib
 * Added further functionality to class so that you can define text size,
 * text color, border size, border and background color. Basically to
 * create a look similar to Outlook's Captions.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 2/01/99    Time: 12:28p
 * Updated in $/CodeJockey/CJ60Lib
 * Made some cosmetic fixes. 
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 1/16/99    Time: 12:27p
 * Created in $/CodeJockey/CJ60Lib
 * Initial first release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "CJResource.h"
#include "CJCaption.h"
#include "CJCaptionPopupWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCJCaptionButton

BEGIN_MESSAGE_MAP(CCJCaptionButton, CCJFlatButton)
	//{{AFX_MSG_MAP(CCJCaptionButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CCJCaptionButton::SetButtonStyle(CFont* pFont, COLORREF clrBackground, COLORREF clrFont)
{
	m_pFont	= pFont;
	SetAlternateColors(clrBackground, m_clrHilite, m_clrDkShad, clrFont);
	return TRUE;
}

void CCJCaptionButton::DrawButtonText(CDC* pDC, UINT nState, CRect& rcItem)
{
	// Get the window text, return if empty.
	CString strWindowText; GetWindowText(strWindowText);
	if( strWindowText.IsEmpty( ))
		return;

	if( m_pFont == NULL ) {
		m_pFont = &m_Font;
	}

	CRect rc = rcItem;
	rc.DeflateRect( 5,0 );

	// Set the text color and select the button font.
	pDC->SetTextColor(( nState & ODS_DISABLED )?m_clrTextGy:m_clrTextNm);
	CFont *oldFont = pDC->SelectObject( m_pFont );
	
	// get the height of the text to be displayed.
    int cy = pDC->DrawText( strWindowText, -1, rc, DT_SINGLELINE|DT_CALCRECT );
	rc.top = (rcItem.Height()-cy)/2;

	// draw the text, and select the original font.
	pDC->DrawState( rc.TopLeft(), strWindowText.GetLength(), strWindowText, 
		( nState & ODS_DISABLED )?DSS_DISABLED:DSS_NORMAL, TRUE, 0, (HBRUSH)NULL );
	
	// Set up pen to use for drawing the triangle
	CPen pen(PS_SOLID, 1, m_clrTextNm);
	CPen *pOldPen = pDC->SelectObject( &pen );
	
	// get the client rect.
	CRect rcWnd;
	GetClientRect(&rcWnd);

	// initialize coordinates for triangle.
	int x1 = rcWnd.right-11;
	int x2 = rcWnd.right-6;
	int y1 = rcWnd.bottom-7;
	int y2 = rcWnd.bottom-5;

	// draw the triangle.
	pDC->MoveTo( x1, y1 );
	pDC->LineTo( x2, y1 );
	pDC->MoveTo( x1+1, y1+1 );
	pDC->LineTo( x2-1, y1+1 );
	pDC->SetPixel( x1+2, y2, m_clrTextNm );

	// restore old pen and font.
	pDC->SelectObject( oldFont );
	pDC->SelectObject( pOldPen );

	// fix potential resource leak - KStowell - 10-21-99
	pen.DeleteObject();
}

/////////////////////////////////////////////////////////////////////////////
// CCJCaption

CCJCaption::CCJCaption()
{
	m_strWindText	= _T("");
	m_pNotifyWnd	= NULL;
	m_pWndPopDown	= NULL;
	m_pChildWnd		= NULL;
	m_cx			= ::GetSystemMetrics(SM_CXSMICON);
	m_cy			= ::GetSystemMetrics(SM_CYSMICON);
	m_clrBtnHilite  = ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow  = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace    = ::GetSysColor(COLOR_BTNFACE);
	m_clrFont		= ::GetSysColor(COLOR_WINDOWTEXT);
	m_clrBorder		= ::GetSysColor(COLOR_BTNFACE);
	m_clrBackgnd	= ::GetSysColor(COLOR_BTNFACE);
	m_nBorder		= 0;
	m_pFont			= NULL;
	m_hIcon			= NULL;

	SetTextStyle( DT_SINGLELINE|DT_END_ELLIPSIS );
}

CCJCaption::~CCJCaption()
{
	// fix potential resource leak - KStowell - 10-22-99.
	m_Font.DeleteObject();
}

BEGIN_MESSAGE_MAP(CCJCaption, CStatic)
	//{{AFX_MSG_MAP(CCJCaption)
	ON_WM_PAINT()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_CAPT_BUTTON, OnCaptButton)
	ON_MESSAGE(CM_ONPUSHPINBUTTON, OnPushPinButton)
	ON_MESSAGE(CM_ONPUSHPINCANCEL, OnPushPinCancel)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJCaption message handlers

void CCJCaption::OnPaint()
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
	memDC.FillSolidRect( rcClient, m_clrBtnFace );
	
	// First let the control do its default drawing.
	CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );

	// draw the backgound, text and icon.
	DrawCaptionBack( &memDC, rcClient );
	DrawCaptionText( &memDC, rcClient );
	DrawCaptionIcon( &memDC, rcClient );

	// KStowell - Copy the memory device context back into the original DC via BitBlt().
	dc.BitBlt(rcClient.left, rcClient.top, rcClient.Width(), rcClient.Height(), &memDC, 0,0, SRCCOPY);

	// KStowell - Cleanup resources.
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}

void CCJCaption::DrawCaptionBack(CDC* pDC, CRect& rcItem)
{
	CRect rc = rcItem;

	// Set the background mode to transparent.
	pDC->SetBkMode( TRANSPARENT );

	// repaint the background.
	pDC->FillSolidRect( rc, m_clrBtnFace );

	// draw the caption border.
	if( m_dwExStyle & WS_EX_FLATEDGE ) {
		pDC->Draw3dRect(rc, m_clrBtnHilite, m_clrBtnShadow);
	}

	// draw the caption banner border.
	if( m_nBorder != 0 ) {
		rc.DeflateRect( m_nBorder, m_nBorder );
		pDC->FillSolidRect(rc, m_clrBackgnd);
	}
}

void CCJCaption::DrawCaptionText(CDC* pDC, CRect& rcItem)
{
	// if the caption text is empty, return.
	if( m_strWindText.IsEmpty( ))
		return;

	if( m_pFont == NULL ) {
		m_pFont = &m_Font;
	}

	CRect rc = rcItem;
	rc.OffsetRect( 10,0 );

	// Set the text color and select the caption font.
	pDC->SetTextColor( m_clrFont );
	CFont* oldFont = pDC->SelectObject( m_pFont );

	// get the height of the text to be displayed.
    int cy = pDC->DrawText( m_strWindText, -1, rc, DT_SINGLELINE|DT_CALCRECT );
	rc.top = (rcItem.Height()-cy)/2;
	rc.bottom = rc.top+cy;
	rc.right  = rcItem.right-(m_cx + m_nBorder);

	// draw the text, and select the original font.
	pDC->DrawText( m_strWindText, rc, m_dwTextStyle );
	pDC->SelectObject( oldFont );
}

void CCJCaption::DrawCaptionIcon(CDC* pDC, CRect& rcItem)
{
	// If no icon was defined, just return.
	if( m_hIcon == NULL )
		return; 
	
	CRect rc = rcItem;
	rc.left = ( rc.right - m_cx )-( m_nBorder + 2 ); 
	
	// stop the icon at the left hand border
	if (rc.left < m_nBorder) {
		rc.left = m_nBorder;
	}
	
	rc.top = ( rcItem.Height() - m_cy )/2;
	rc.right = rc.left + m_cx;
	rc.bottom = rc.top + m_cy; 
	
	// Only draw the icon if it will fit into the space between the borders
	if (rc.right < rcItem.right - m_nBorder) { 
		// draw the icon associated with the caption.
		::DrawIconEx( pDC->GetSafeHdc(), rc.left, rc.top, m_hIcon,
			rc.Width(), rc.Height(), NULL, (HBRUSH)NULL, DI_NORMAL); 
	}
}

void CCJCaption::OnSysColorChange() 
{
	CStatic::OnSysColorChange();

	m_clrBtnHilite = ::GetSysColor(COLOR_BTNHILIGHT);
	m_clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	m_clrBtnFace   = ::GetSysColor(COLOR_BTNFACE);
}

BOOL CCJCaption::Create(CWnd* pParentWnd, LPCTSTR lpszWindowName, DWORD dwExStyle, DWORD dwStyle, const CRect& rect, UINT nID) 
{
	// Let the base class create the control.
	if( !CStatic::Create( NULL, dwStyle, rect, pParentWnd, nID ))
	{
		TRACE(_T("Unable to create caption.\n"));
		return FALSE;
	}

	UpdateFont();

	// save the style.
	if( dwExStyle & WS_EX_FLATEDGE ) {
		m_dwExStyle = dwExStyle;
	}

	else {
		ModifyStyleEx( 0, dwExStyle );
	}

	// Save the window text.
	m_strWindText = lpszWindowName;

	// Create the button to be used with child window.
	if( !m_CaptionButton.Create( m_strWindText, BS_ICON|BS_OWNERDRAW|BS_VCENTER,
		CRect(0,0,0,0), this, IDC_CAPT_BUTTON ))
	{
		TRACE0( "Unable to create caption button.\n" );
		return FALSE;
	}

	// set the button font and colors.
	m_CaptionButton.SetButtonStyle( m_pFont, m_clrBackgnd, m_clrFont );

	return TRUE;
}

void CCJCaption::ModifyCaptionStyle(int nBorderSize, CFont* pFont, LPCTSTR lpszWindText, HICON hIcon)
{
	ASSERT(::IsWindow(m_hWnd));
	
	if( nBorderSize ) {
		m_nBorder = nBorderSize;
	}
	
	if( pFont ) {
		m_pFont	= pFont;
		m_CaptionButton.SetButtonStyle(	m_pFont, m_clrBackgnd, m_clrFont );
	}
	
	if( lpszWindText ) {
		m_strWindText = lpszWindText;
	}
	
	if( hIcon )	{
		m_hIcon	= hIcon;
	}

	Invalidate();
}

void CCJCaption::SetChildWindow(CWnd* pChild, CWnd* pNotifyWnd)
{
	ASSERT( pChild != NULL );		// must be valid.
	ASSERT( pNotifyWnd != NULL );	// must be valid.

	// save window pointers, and show caption button.
	m_pChildWnd  = pChild;
	m_pNotifyWnd = pNotifyWnd;

	// set the size of the child window.
	m_pChildWnd->GetWindowRect(&m_rcChild);
	
	CRect rcClient;
	m_pChildWnd->GetParent()->GetClientRect(&rcClient);
	m_nOffset = rcClient.Height()-m_rcChild.Height();

	m_CaptionButton.ModifyStyle( 0, WS_VISIBLE );
	Invalidate();
}

void CCJCaption::KillChildWindow()
{
	m_CaptionButton.SetState(FALSE);
	m_CaptionButton.ModifyStyle( WS_VISIBLE, 0 );
	m_pWndPopDown->DestroyWindow();
	_delete( m_pWndPopDown );
}

void CCJCaption::OnSize(UINT nType, int cx, int cy) 
{
	CStatic::OnSize(nType, cx, cy);
	
	if( m_CaptionButton.GetSafeHwnd())
	{
		CDC* pDC = GetDC();
		CFont* pOldFont = pDC->SelectObject(m_pFont);

		TEXTMETRIC tm;
		pDC->GetTextMetrics(&tm);

		CSize size = pDC->GetTextExtent(m_strWindText, m_strWindText.GetLength());
		size.cx += tm.tmAveCharWidth+21;
		pDC->SelectObject(pOldFont);
		ReleaseDC(pDC);
		
		CRect rc( 0, 0, size.cx, cy );
		rc.DeflateRect(m_nBorder+1, m_nBorder+1);
		m_CaptionButton.MoveWindow(rc);
	}
}

void CCJCaption::OnCaptButton() 
{
	CRect rc;
	m_CaptionButton.GetWindowRect(&rc);
	rc.right  = rc.left + m_rcChild.Width();
	rc.top    = rc.bottom;
	rc.bottom = rc.top + m_rcChild.Height();

	//if window is already present delete it
	_delete( m_pWndPopDown );
	
	// Get the class name and create the window
	m_pWndPopDown = new CCJCaptionPopupWnd;
	m_pWndPopDown->Create( rc, this, m_pChildWnd );
	m_pChildWnd->ShowWindow(SW_SHOW);
	m_CaptionButton.SetState(TRUE);
}

LRESULT CCJCaption::OnPushPinButton(WPARAM wParam, LPARAM lParam)
{
	UNUSED_ALWAYS( wParam );
	UNUSED_ALWAYS( lParam );

	KillChildWindow();
	m_pNotifyWnd->SendMessage(CM_ONPUSHPINBUTTON, 0, 0);
	m_pChildWnd->GetParent()->GetClientRect(&m_rcChild);
	if(m_nOffset>0) {
		m_rcChild.top += m_nOffset;
	}
	m_pChildWnd->MoveWindow(m_rcChild);
	Invalidate();

	return 0L;
}


LRESULT CCJCaption::OnPushPinCancel(WPARAM wParam, LPARAM lParam)
{
	UNUSED_ALWAYS( wParam );
	UNUSED_ALWAYS( lParam );

	m_CaptionButton.SetState(FALSE);
	m_pWndPopDown->DestroyWindow();

	_delete( m_pWndPopDown );
	
	m_pNotifyWnd->SendMessage(CM_ONPUSHPINCANCEL, 0, 0);

	return 0L;
}

void CCJCaption::UpdateCaption(LPCTSTR lpszWindowText, HICON hIcon)
{
	ASSERT(::IsWindow(m_hWnd));
	
	CRect rcClient;
	GetClientRect(&rcClient);
	CDC* pDC = GetDC();
	
	UpdateCaptionBack(pDC, rcClient);

	if( lpszWindowText )
	{
		m_strWindText = lpszWindowText;
		UpdateCaptionText(pDC, rcClient);
	}
	
	if( hIcon )	{
		m_hIcon	= hIcon;
		DrawCaptionIcon( pDC, rcClient );
	}

	ReleaseDC(pDC);
}

void CCJCaption::UpdateCaptionBack(CDC* pDC, CRect& rcItem)
{
	CRect rc = rcItem;

	// Set the background mode to transparent.
	pDC->SetBkMode( TRANSPARENT );

	rc.DeflateRect(1,1);
	// repaint the background.
	pDC->FillSolidRect( rc, m_clrBtnFace );

	// draw the caption banner border.
	if( m_nBorder != 0 ) {
		rc.DeflateRect( m_nBorder-1, m_nBorder-1 );
		pDC->FillSolidRect(rc, m_clrBackgnd);
	}
}

void CCJCaption::UpdateCaptionText(CDC* pDC, CRect& rcItem)
{
	CFont* pOldFont = pDC->SelectObject(m_pFont);
	
	TEXTMETRIC tm;
	pDC->GetTextMetrics(&tm);
	
	CSize size = pDC->GetTextExtent(m_strWindText, m_strWindText.GetLength());
	size.cx += tm.tmAveCharWidth+21;
	pDC->SelectObject(pOldFont);
	
	CRect rc( 0, 0, size.cx, rcItem.Height() );
	rc.DeflateRect(m_nBorder+1, m_nBorder+1);
	m_CaptionButton.MoveWindow(rc);
	m_CaptionButton.SetWindowText(m_strWindText);
	
	DrawCaptionText( pDC, rcItem );
}

void CCJCaption::UpdateFont()
{
	m_Font.DeleteObject();
	NONCLIENTMETRICS ncm;
	ncm.cbSize = sizeof(ncm);
	SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(ncm), &ncm, 0);
	VERIFY(m_Font.CreateFontIndirect(&ncm.lfMessageFont));
	m_pFont = &m_Font;
}

BOOL CCJCaption::OnEraseBkgnd(CDC* pDC) 
{
	// KStowell - overridden for flicker-free drawing.
	UNUSED_ALWAYS(pDC);
	return TRUE;
}
