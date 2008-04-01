//
// hotcombobox.cpp
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "hotcombobox.h"
#include "../../gdi/windowdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/paintdc.h"
#include "../../tools/theming.h"
#include "../../tools/xpcolors.h"
#include "../../tools/drawtools.h"
#include "../../exceptions/memoryexception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Mouse tracking timer.
#define MOUSE_TRACK	0xDEAD
#define MOUSE_TIMEOUT	50

// Constructor.
ClsHotComboBox::ClsHotComboBox()
{
	// Defaults.
	m_bXPStyle    = TRUE;
	m_bDoComplete = TRUE;
	m_bTracking   = FALSE;
}

// Destructor.
ClsHotComboBox::~ClsHotComboBox()
{
}

// Attach the handle and refresh the window so the
// new client area is calculated.
BOOL ClsHotComboBox::Attach( HWND hWnd, BOOL bDestroy /* = FALSE */ )
{
	// Call the base class.
	BOOL bRC = ClsComboBox::Attach( hWnd, bDestroy );
	return bRC;
}

// Mouse being tracked?
BOOL ClsHotComboBox::IsTracking()
{
	// Tracking?
	if ( m_bTracking )
		return TRUE;

	// When this is a combo-box with an edit control
	// tracking is valid as long as the mouse is over
	// the edit control area.
	if ( GetStyle() & ( CBS_SIMPLE | CBS_DROPDOWN ))
	{
		ClsRect rc;
		ClsPoint pt;
		::GetWindowRect( ::GetWindow( m_hWnd, GW_CHILD ), rc );
		GetCursorPos( pt );
		if ( rc.PtInRect( pt ))
		{
			// Start the timer which keeps running as long as the
			// mouse is over the edit control area.
			SetTimer( MOUSE_TRACK, MOUSE_TIMEOUT );
			return TRUE;
		}
	}
	return FALSE;
}

// Draw the drop-down arrow.
void ClsHotComboBox::DrawDDArrow( ClsDC *pDC )
{
	BOOL bHot = (( IsWindowEnabled() == FALSE || ! IsTracking()) && ! HasFocus()) ? FALSE : TRUE;
	COLORREF crOut, crIn, crFill;

	// Determine the dropdown area.
	ClsRect rc = GetClientRect();
	rc.Left() = rc.Right() -  ( ::GetSystemMetrics( SM_CXVSCROLL ) + 2 );

	// Fill the arrow area.
	pDC->FillSolidRect( rc.Left(), rc.Top() + 2, rc.Width() - 1, rc.Height() - 4, ::GetSysColor( COLOR_WINDOW )); 

	// Disabled or not hot?
	if ( IsWindowEnabled() == FALSE || ! bHot )
	{
		// Render arrow background.
		rc.Left()   -= 2;
		rc.Top()    += 2;
		rc.Right()  -= 2;
		rc.Bottom() -= 2;
		pDC->FillSolidRect( rc, ::GetSysColor( COLOR_BTNFACE ));

		if ( IsWindowEnabled())
			pDC->FillSolidRect( rc.Left(), rc.Top(), 4, rc.Height(),  ::GetSysColor( COLOR_WINDOW ));

		// And the drop-down arrow itself.
		rc.Top()++;
		rc.Left() += 6;
		if ( m_bXPStyle && IsWindowEnabled() == FALSE )
			// XP style disabled will not render the dropdown
			// arrow in the embossed style.
			ClsDrawTools::RenderMonoBitmap( *pDC, ClsGetApp()->GetClsImages(), ClsApp::CII_DROPDOWN_SMALL, rc, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ));
		else
			// Normal or disabled style.
			ClsDrawTools::RenderBitmap( *pDC, ClsGetApp()->GetClsImages(), ClsApp::CII_DROPDOWN_SMALL, rc, IsWindowEnabled() ? 0 : ClsDrawTools::CDSF_DISABLED );
		return;
	}

	// XP-Style rendering?
	if ( m_bXPStyle )
	{
		// Define colors.
		crOut = XPColors.GetXPColor( ClsXPColors::XPC_OUTER_SELECTION );
		crIn = GetDroppedState() ? XPColors.GetXPColor( ClsXPColors::XPC_INNER_CHECKED_SELECTED ) : XPColors.GetXPColor( ClsXPColors::XPC_INNER_SELECTION );
		crFill = crIn;

		// Draw the outer rectangle.
		rc.Left()++;
		ClsDrawTools::DrawRectangle( *pDC, rc, crOut );

		// Daw the inner rectangle.
		rc.Deflate( 1, 1 );
		ClsDrawTools::DrawRectangle( *pDC, rc, crIn );

		// Fill the dropdown arrow area.
		rc.Deflate( 1, 1 );
		pDC->FillSolidRect( rc, crFill );

		// Draw the dropdown arrow.
		rc.Offset( 1, 1 );
		ClsDrawTools::RenderMonoBitmap( *pDC, ClsGetApp()->GetClsImages(), ClsApp::CII_DROPDOWN_SMALL, rc, GetDroppedState() ? XPColors.GetXPColor( ClsXPColors::XPC_TEXT_BACKGROUND ) : RGB( 0, 0, 0 ), 0 );
	}
	else
	{
		// Define colors.
		crOut = ::GetSysColor( COLOR_BTNFACE );
		crIn = ::GetSysColor( COLOR_WINDOW );
		crFill = crOut;

		// Adjust the rectangle.
		rc.Deflate( 1, 1 );
		
		// Render arrow background.
		pDC->FillSolidRect( rc, crOut );
		
		// Deflate the rectangle.
		rc.Deflate( 1, 1 );
		pDC->DrawEdge( rc, GetDroppedState() ? BDR_SUNKENOUTER : BDR_RAISEDINNER, BF_RECT );

		// Draw the dropdown arrow.
		rc.Offset( 1, 1 );
		ClsDrawTools::RenderBitmap( *pDC, ClsGetApp()->GetClsImages(), ClsApp::CII_DROPDOWN_SMALL, rc, 0 );
	}
}


BOOL ClsHotComboBox::HasFocus()
{
	// Get the window with the focus,
	// if it's us we return TRUE.
	HWND hFocus = ::GetFocus();
	if ( hFocus == m_hWnd )
		return TRUE;

	// If it is the edit control we als
	// return TRUE.
	HWND hEdit = ::GetWindow( m_hWnd, GW_CHILD );
	if ( hEdit && hEdit == ::GetFocus())
		return TRUE;

	// The combo does not have focus.
	return FALSE;
}

// Render the control frame.
void ClsHotComboBox::RenderTheFrame( ClsDC *pDC )
{
	// Valid DC?
	if ( pDC->IsValid())
	{
		// Obtain the rectangle.
		ClsRect rc = GetClientRect();

		// No touching the rectangle interior.
		HGDIOBJ hOld = pDC->SelectObject( ::GetStockObject( NULL_BRUSH ));

		// Cold or diabled frame.
		if (( IsWindowEnabled() == FALSE || ! IsTracking()) && ! HasFocus())
		{
			// This results in a COLOR_BTNFACE and
			// COLOR_WINDOW frame.
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

		// Draw the arrow.
		DrawDDArrow( pDC );

		// Restore brush.
		pDC->SelectObject( hOld );
	}
}

// Switch styles.
void ClsHotComboBox::SetXPStyle( BOOL bXPStyle /* = TRUE */, BOOL bRedraw /* = TRUE */ )
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

// CBN_DROPDOWN notification.
LRESULT ClsHotComboBox::OnDropDown( BOOL& bNotifyParent )
{
	// Do not bother the parent.
	bNotifyParent = FALSE;

	// Rerender the frame.
	ClsWindowDC dc( this );
	RenderTheFrame( &dc );
	return 0;
}

// CBN_CLOSEUP notification.
LRESULT ClsHotComboBox::OnCloseUp( BOOL& bNotifyParent )
{ 
	// No need to bother the parent.
	bNotifyParent = FALSE;

	// Rerender the frame.
	ClsWindowDC dc( this );
	RenderTheFrame( &dc );
	return 0;
}

// PreTranslateMessage() override.
BOOL ClsHotComboBox::PreTranslateMessage( LPMSG pMsg )
{
	// Auto-completion on?
	if ( m_bDoComplete )
		// Call the helper method.
		m_Completion.PreTranslateMessage( this, pMsg );
	return ClsComboBox::PreTranslateMessage( pMsg );
}

// OnEditUpdate() override.
LRESULT ClsHotComboBox::OnEditUpdate( BOOL& bNotifyParent )
{
	// Call the helper method.
	if ( m_bDoComplete )
	{
		bNotifyParent = FALSE;
		m_Completion.OnEditUpdate( this );
		return 0;
	}
	return ClsComboBox::OnEditUpdate( bNotifyParent );
}

// About to get focus.
LRESULT ClsHotComboBox::OnSetFocus( BOOL& bNotifyParent )
{
	// Re-render the frame.
	ClsWindowDC dc( this );
	RenderTheFrame( &dc );
	return 0;
}

// About to lose focus.
LRESULT ClsHotComboBox::OnKillFocus( BOOL& bNotifyParent )
{
	// Re-render the frame.
	ClsWindowDC dc( this );
	RenderTheFrame( &dc );
	return 0;
}

// Window procedure override.
LRESULT ClsHotComboBox::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_ENABLE:
			// Redraw entire control.
			RedrawWindow( NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_FRAME );
			break;

		case	WM_MOUSELEAVE:
			{
				// Tracking stopped.
				m_bTracking = FALSE;

				// Render the frame.
				ClsWindowDC dc( this );
				RenderTheFrame( &dc );
			}
			break;

		case	WM_MOUSEMOVE:
			{
				// Are we tracking?
				if ( ! m_bTracking )
				{
					// Start tracking the mouse.
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

		case	WM_PARENTNOTIFY:
			{
				HWND hEdit = ::GetWindow( m_hWnd, GW_CHILD );
				if ( hEdit && hEdit != ::GetFocus())
				{
					// Re-render the frame.
					ClsWindowDC dc( this );
					RenderTheFrame( &dc );
					KillTimer( 0xDEAD );
				}
			}

		case	WM_TIMER:
		{
			// Mouse tracking timer? I use a timer event to see if the
			// mouse has left the control because the WM_MOUSEMOVE event
			// is not always triggered when the user moves the mouse out 
			// of the edit control.
			if ( wParam == MOUSE_TRACK )
			{
				ClsRect rc;
				ClsPoint pt;
				::GetWindowRect( ::GetWindow( m_hWnd, GW_CHILD ), rc );
				GetCursorPos( pt );

				// No longer over the control area? Stop
				// the timer and redraw the frame.
				if ( ! rc.PtInRect( pt ))
				{
					KillTimer( MOUSE_TRACK );
					ClsWindowDC dc( this );
					RenderTheFrame( &dc );
					return 0;
				}
			}
			break;
		}
	}
	// Call the base class.
	return ClsComboBox::WindowProc( uMsg, wParam, lParam );
}

// WM_PAINT override.
LRESULT ClsHotComboBox::OnPaint( ClsDC *pDC )
{
	// Use SEH to catch memory problems
	try
	{
		// Get the client rectangle.
		ClsRect rc = GetClientRect();

		// If the input device context is valid we
		// use it to create a buffer off. Otherwise we
		// use the DC created by the ClsPaintDC class
		// to create a buffer off.
		BOOL bCreated = FALSE;
		ClsDC *pdc;
		if ( pDC )
			// Use the supplied DC.
			pdc = pDC;
		else	  
		{ 
			// Use the DC create by the ClsPaintDC object.
			// The DC is the one returned by the BeginPaint()
			// API.
			pdc = new ClsPaintDC( this ); 
			bCreated = TRUE; 
		}

		// Preset the return code.
		LRESULT lResult = -1;

		// Brace the code so that the buffer DC runs
		// out of scope before the ClsPaintDC object
		// is deleted.
		{
			// Create DC buffer.
			ClsBufferDC dc( *pdc, rc );

			// Render the background.
			SendMessage( WM_ERASEBKGND, ( WPARAM )( HDC )dc );

			// Call the default procedure to render the control.
			lResult = WindowProc( WM_PAINT, ( WPARAM )( HDC )dc, 0 );

			// Render the frame.
			RenderTheFrame( &dc );
		}

		// Delete the DC if necessary.
		if ( bCreated ) delete pdc;

		// Return the result.
		return lResult;
	}

	// If a memory problem occures in the above code
	// -1 is returned which calls the default message
	// handler.
	catch ( ClsMemoryException& me )
	{
		me;
	}
	return -1;
}