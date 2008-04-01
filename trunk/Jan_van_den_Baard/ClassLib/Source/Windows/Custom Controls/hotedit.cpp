//
// hotedit.cpp
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "hotedit.h"
#include "../../gdi/windowdc.h"
#include "../../tools/theming.h" 
#include "../../tools/xpcolors.h"
#include "../../tools/drawtools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsHotEdit::ClsHotEdit()
{
	// Defaults.
	m_bXPStyle  = TRUE;
	m_bTracking = FALSE;
}

// Destructor.
ClsHotEdit::~ClsHotEdit()
{
}

// Attach the handle and refresh the window so the
// new client area is calculated.
BOOL ClsHotEdit::Attach( HWND hWnd, BOOL bDestroy /* = FALSE */ )
{
	// Call the base class.
	BOOL bRC = ClsEdit::Attach( hWnd, bDestroy );

	// Force a WM_NCCALCSIZE message.
	if ( bRC ) SetWindowPos( NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED );
	return bRC;
}

// Render the control frame.
void ClsHotEdit::RenderTheFrame( ClsDC *pDC )
{
	// Valid DC?
	if ( pDC->IsValid())
	{
		// Obtain the rectangle.
		ClsRect rc = GetWindowRect();
		rc.Offset( -rc.Left(), -rc.Top());

		// No touching the rectangle interior.
		HGDIOBJ hOld = pDC->SelectObject( ::GetStockObject( NULL_BRUSH ));

		// Cold or diabled frame.
		if (( IsWindowEnabled() == FALSE || m_bTracking == FALSE ) && GetFocus() != this )
		{
			ClsDrawTools::DrawRectangle( *pDC, rc, ::GetSysColor( COLOR_BTNFACE ));
			rc.Deflate( 1, 1 );
			ClsDrawTools::DrawRectangle( *pDC, rc, ::GetSysColor( COLOR_WINDOW ));
		}
		else
		{
			if ( m_bXPStyle )
			{
				// This results in a XPC_OUTER_SELECTION and
				// COLOR_WINDOW frame.
				ClsDrawTools::DrawRectangle( *pDC, rc, XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION ));
				rc.Deflate( 1, 1 );
				ClsDrawTools::DrawRectangle( *pDC, rc, ::GetSysColor( COLOR_WINDOW ));
			}
			else
			{
				// This results in a BDR_SUNKENOUTER and
				// COLOR_BTNFACE frame.
				pDC->DrawEdge( rc, BDR_SUNKENOUTER, BF_RECT );
				rc.Deflate( 1, 1 );
				ClsDrawTools::DrawRectangle( *pDC, rc, ::GetSysColor( COLOR_BTNFACE ));
			}
		}
		// Restore brush.
		pDC->SelectObject( hOld );
	}
}

// Switch styles.
void ClsHotEdit::SetXPStyle( BOOL bXPStyle /* = TRUE */, BOOL bRedraw /* = TRUE */ )
{
	// Did it actually change?
	if ( bXPStyle != m_bXPStyle )
	{
		// Setup the new style.
		m_bXPStyle = bXPStyle;

		// Rerender if requested.
		if ( bRedraw )
			Invalidate();
	}
}

// Window procedure override.
LRESULT ClsHotEdit::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_ENABLE:
			// Redraw entire control.
			RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME );
			break;

		case	WM_NCCALCSIZE:
		{
			// Do we need to adjust the frame rectangle?
			if ( wParam )
			{
				// Cast the parameters.
				LPNCCALCSIZE_PARAMS pParams = reinterpret_cast< LPNCCALCSIZE_PARAMS >( lParam );

				// Adjust for the 2-pixel frame.
				::InflateRect( &pParams->rgrc[ 0 ], -2, -2 );
				return 0;
			}
			break;
		}

		case	WM_MOUSELEAVE:
			{
				// No longer tracking.
				m_bTracking = FALSE;

				// Render the frame.
				ClsWindowDC dc( this );
				RenderTheFrame( &dc );
			}
			break;


		case	WM_MOUSEMOVE:
		{
			// Already tracking?
			if ( ! m_bTracking )
			{
				// Start tracking.
				TRACKMOUSEEVENT tme = { 0 };
				tme.cbSize    = sizeof( tme );
				tme.dwFlags   = TME_LEAVE;
				tme.hwndTrack = m_hWnd;
				if (( m_bTracking = _TrackMouseEvent( &tme )) != FALSE )
				{
					// Render the frame.
					ClsWindowDC dc( this );
					RenderTheFrame( &dc );
				}
			}
			break;
		}

		case	WM_SETFOCUS:
		case	WM_KILLFOCUS:
		{
			// Re-render the frame.
			ClsWindowDC dc( this );
			RenderTheFrame( &dc );
			break;
		}

		case	WM_NCPAINT:
		{
			// Render the frame.
			ClsWindowDC dc( this );
			RenderTheFrame( &dc );
			return 0;
		}
	}
	// Call the base class.
	return ClsEdit::WindowProc( uMsg, wParam, lParam );
}