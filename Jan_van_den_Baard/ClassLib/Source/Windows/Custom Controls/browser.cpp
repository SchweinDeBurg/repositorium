//
// browser.cpp
//
// (C) Copyright 2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "browser.h"
#include "../../gdi/windowdc.h"
#include "../../gdi/getdc.h"
#include "../../gdi/selector.h"
#include "../../tools/theming.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsBrowser::ClsBrowser()
{
	// Setup defaults.
	m_bDown		= FALSE;
	m_bHasCapture	= FALSE;
	m_bRectValid	= FALSE;
	m_bClicked	= FALSE;
	m_hTheme	= NULL;

	// Setup default theme.
	wcscpy( m_szTheme, L"BUTTON" );
}

// Destructor.
ClsBrowser::~ClsBrowser()
{
	// Close theme if valid.
	if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
}

// Attach the handle and refresh the window so the
// new client area is calculated.
BOOL ClsBrowser::Attach( HWND hWnd, BOOL bDestroy /* = FALSE */ )
{
	// Call the base class.
	BOOL bRC = ClsEdit::Attach( hWnd, bDestroy );

	// Must make sure the new frame is calculated.
	if ( bRC ) SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );
	return bRC;
}

// Compute the browser button rectangle.
ClsRect ClsBrowser::GetButtonRect()
{
	// Is the rectangle valid?
	if ( m_bRectValid )
		return m_ButtonRect;

	// First we get the window rectangle.
	GetWindowRect( m_ButtonRect );

	// Make sure the coordinates are relative to 0,0.
	m_ButtonRect.Offset( -m_ButtonRect.Left(), -m_ButtonRect.Top());

	// Compute the button rectangle.
	m_ButtonRect.Left() = m_ButtonRect.Right() - m_nButtonWidth;

	// Do we have a border?
	if ( GetExStyle() & WS_EX_CLIENTEDGE )
	{
		m_ButtonRect.Offset( -::GetSystemMetrics( SM_CXEDGE ), 0 );
		m_ButtonRect.Inflate( 0, -::GetSystemMetrics( SM_CYEDGE ));
	}

	// Rectangle valid.
	m_bRectValid = TRUE;

	return m_ButtonRect;
}

// Determine if the cursor is currently located
// over the browser button.
BOOL ClsBrowser::CursorOnButton()
{
	// Get cursor position.
	ClsPoint pt;
	GetCursorPos( pt );

	// Get window rectangle in screen
	// coordinates.
	ClsRect	 rc = GetWindowRect();

	// Convert the cursor location to coordinates
	// relative to the top/left corner of the control.
	pt.Subtract( rc.TopLeft());

	// Is the point located in the browser button rectangle?
	return GetButtonRect().PtInRect( pt );
}

// Measure the button.
void ClsBrowser::GetButtonSize( ClsDC *pDC, ClsSize& sButtonSize )
{
	// Compute the size of an ellipsis.
	ClsRect		rcr;
	pDC->DrawText( _T( "···" ), 3, rcr, DT_CALCRECT );
	rcr.Right() += 3 * ::GetSystemMetrics( SM_CXFRAME );
	sButtonSize = rcr.Size();
}

// Render the button (overidable)...
void ClsBrowser::RenderButton( ClsDC *pDC, ClsRect rcClip )
{
	// Setup the DC.
	pDC->SetTextColor( IsWindowEnabled() ? ::GetSysColor( COLOR_BTNTEXT ) : ::GetSysColor( COLOR_GRAYTEXT ));
	pDC->SetBkMode( TRANSPARENT );

	// Theming?
	if ( ThemingAPI.IsThemingEnabled())
	{
		// Theme loaded? If not we simulate a WM_THEMECHANGED to load
		// it.
		if ( ! m_hTheme ) m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), m_szTheme );
		if ( m_hTheme )
		{
			// Determine the rendering flags.
			DWORD dwFlags = PBS_NORMAL;
			if ( IsWindowEnabled() == FALSE ) dwFlags = PBS_DISABLED;
			else 
			{
				if ( m_bDown ) dwFlags = PBS_PRESSED;
				else if ( m_bHasCapture && CursorOnButton()) dwFlags = PBS_HOT;
			}

			// Clear rectangle and render the button.
			rcClip.Inflate( 1, 1 );
			pDC->FillRect( rcClip, ( HBRUSH )( IsWindowEnabled() ? COLOR_WINDOW + 1 : COLOR_BTNFACE + 1 ));
			ThemingAPI.DrawThemeBackground( m_hTheme, *pDC, BP_PUSHBUTTON, dwFlags, rcClip, NULL );
			pDC->DrawText( _T( "···" ), 3, rcClip, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
			return;
		}
	}

	// Render the outer button.
	pDC->FillRect( rcClip, ( HBRUSH )( COLOR_BTNFACE + 1 ));
	pDC->DrawEdge( rcClip, BDR_RAISEDOUTER, BF_BOTTOMRIGHT );
	
	// If we are not in down mode we do not offset
	// the button frame.
	if ( ! m_bDown ) 
	{
		rcClip.Left()++; 
		rcClip.Top()++; 
	}

	// Draw the button frame.
	pDC->DrawFrameControl( rcClip, DFC_BUTTON, DFCS_BUTTONPUSH | ( m_bDown ? DFCS_FLAT : 0 ));

	// And the ellipsis.
	pDC->DrawText( _T( "···" ), 3, rcClip, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
}

// Render the button.
void ClsBrowser::InternalRenderButton( ClsDC *pDC )
{
	// Snapshot the device context.
	int nDC = pDC->SaveDC();

	// Get browser button rectangle and select the font.
	ClsRect rcButton = GetButtonRect();
	ClsSelector sel( pDC, ( HFONT )SendMessage( WM_GETFONT ));

	// Call the overidable.
	RenderButton( pDC, rcButton );

	// Restore the DC.
	pDC->RestoreDC( nDC );
}

LRESULT ClsBrowser::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_ENABLE:
			// Redraw entire control.
			RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME );
			break;

		case	WM_DROPFILES:
			// Call overridable.
			OnFileDropped(( HDROP )wParam );
			break;

		case	WM_NCCALCSIZE:
		{
			// Do we need to adjust the frame rectangle?
			if ( wParam )
			{
				// Cast the parameters.
				LPNCCALCSIZE_PARAMS pParams = reinterpret_cast< LPNCCALCSIZE_PARAMS >( lParam );
			
				// First we let the base class do it's thing.
				LRESULT rc = ClsEdit::WindowProc( uMsg, wParam, lParam );
			
				// Now we adjust the rectangle to make room for the
				// browser button.
				ClsGetDC	dc( this );
				ClsSelector	sel( &dc, ( HFONT )SendMessage( WM_GETFONT ));

				// Call the overridable to compute the button
				// size.
				ClsSize sz;
				GetButtonSize( &dc, sz );

				// Adjust right border.
				m_nButtonWidth = sz.CX();
				pParams->rgrc[ 0 ].right -= m_nButtonWidth;

				// Button rectangle no longer valid.
				m_bRectValid = FALSE;
				return rc;
			}
			break;
		}

		case	WM_NCHITTEST:
		{
			// First we let the base class have a go at it?
			LRESULT rc = ClsEdit::WindowProc( uMsg, wParam, lParam );

			// No known area?
			if ( rc == HTNOWHERE )
			{
				// If the cursor is over the browser button we
				// return a hit in the control border.
				if ( CursorOnButton())
					rc = HTBORDER;
			}
			return rc;
		}

		case	WM_LBUTTONDOWN:
			// Click on the button when we have
			// the capture?
			if ( m_bHasCapture && m_bClicked == FALSE && CursorOnButton())
			{
				// Yes. Set the focus.
				SetFocus();

				// Set pressed mode and clicked mode.
				m_bDown = TRUE;
				m_bClicked = TRUE;

				// Render the button.
				ClsWindowDC dc( this );
				InternalRenderButton( &dc );
			}
			break;

		case	WM_NCLBUTTONDBLCLK:
		case	WM_NCLBUTTONDOWN:
		{
			// Mouse on the browser button?
			if ( CursorOnButton())
			{
				// Capture the mouse.
				SetCapture();
				SetFocus();
				m_bHasCapture = TRUE;
				m_bClicked = TRUE;

				// We are down-pressed.
				m_bDown = TRUE;
				
				// Render the button.
				ClsWindowDC dc( this );
				InternalRenderButton( &dc );
				return 0;
			}
			break;
		}
		
		case	WM_LBUTTONUP:
		{
			// Have we captured the mouse?
			if ( m_bHasCapture && m_bClicked )
			{
				// Release the mouse capture.
				ReleaseCapture();
				m_bHasCapture = m_bClicked = FALSE;

				// Released while hovering over the
				// browser button?
				if ( CursorOnButton())
				{
					// No longer pressed.
					m_bDown = FALSE;

					// Render the button.
					ClsWindowDC dc( this );
					InternalRenderButton( &dc );
					
					// Call the overidable.
					OnBrowserClicked();
				}
			}
			break;
		}

		case	WM_NCMOUSEMOVE:
			// Do we have the capture?
			if ( ! m_bHasCapture )
			{
				// Take the capture.
				SetCapture();
				m_bHasCapture = TRUE;
			}
			break;
			
		case	WM_MOUSEMOVE:
		{
			// Where we tracking movement or a click?
			if ( m_bHasCapture && m_bClicked == FALSE && CursorOnButton() == FALSE )
			{
				// Release capture and update
				// the frame.
				ReleaseCapture();
				m_bHasCapture = FALSE;

				// Render the button.
				ClsWindowDC dc( this );
				InternalRenderButton( &dc );
			}
			else if ( m_bHasCapture && m_bClicked == TRUE )
			{
				// Get the current down-pressed state.
				BOOL	bDown = m_bDown;

				// Is the cursor located on the browser button?
				m_bDown = CursorOnButton();

				// Did the down-pressed state change?
				if ( bDown != m_bDown )
				{
					// Render the button.
					ClsWindowDC dc( this );
					InternalRenderButton( &dc );
				}
			}
			break;
		}

		case	WM_THEMECHANGED:
			// Theming API available?
			if ( ThemingAPI.IsThemingEnabled())
			{
				// Close current theme and re-open it.
				if ( m_hTheme ) ThemingAPI.CloseThemeData( m_hTheme );
				m_hTheme = ThemingAPI.OpenThemeData( GetSafeHWND(), m_szTheme );
			}
			break;

		case	WM_PRINT:
		{
			// Draw frame?
			ClsDC *pDC = ClsDC::FromHandle(( HDC )wParam );
			if ( pDC && lParam & PRF_NONCLIENT )
				InternalRenderButton( pDC );
			break;
		}
			
		case	WM_NCPAINT:
		{
			// Obtain window DC.
			ClsWindowDC dc( this );

			// Render the Button.
			InternalRenderButton( &dc );
			break;
		}
	}
	// Call the base class.
	return ClsEdit::WindowProc( uMsg, wParam, lParam );
}
