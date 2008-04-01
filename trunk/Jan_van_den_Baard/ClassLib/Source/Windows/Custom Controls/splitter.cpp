//
// splitter.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "splitter.h"
#include "../../gdi/brush.h"
#include "../../gdi/dc.h"
#include "../../gdi/selector.h"
#include "../../application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Defaults the data.
ClsSplitter::ClsSplitter()
{
	// Setup defaults.
	m_bIsHorizontal = TRUE;
	m_bIsMoving	= FALSE;
	m_bIsVisible	= FALSE;
	m_nPosition	= 0;
	m_nMinPane1	= m_nMinPane2 = 10;
	m_hPane1	= m_hPane2 = ( HWND )NULL;

	// Get cursors.
	m_hHorz = ClsGetCursor( ClsApp::STC_HSPLIT );
	m_hVert = ClsGetCursor( ClsApp::STC_VSPLIT );

	// Attach brush.
	m_HatchBrush.Attach( ClsGetApp()->GetPatternBrush(), FALSE );
}

// Destructor.
ClsSplitter::~ClsSplitter()
{
}

// Create the control.
BOOL ClsSplitter::Create( ClsWindow *parent, UINT nID, BOOL bHorizontal /* = TRUE */ )
{
	// Store the type. Horizontal or vertical.
	m_bIsHorizontal = bHorizontal;

	// Create the control.
	return ClsWindow::Create( 0, NULL, NULL, WS_VISIBLE | WS_CHILD, ClsRect( 0, 0, 0, 0 ), parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
}

// Setup splitter panes.
void ClsSplitter::SetPanes( HWND hPane1, HWND hPane2 )
{
	_ASSERT( ::IsWindow( hPane1 ) && ::IsWindow( hPane2 )); // These must be valid.

	// Store the handles.
	m_hPane1 = hPane1;
	m_hPane2 = hPane2;
}

// Get splitter pane handles.
void ClsSplitter::GetPanes( LPHANDLE pPane1, LPHANDLE pPane2 ) const
{
	// Store pane handles.
	if ( pPane1 ) *pPane1 = ( HANDLE )m_hPane1;
	if ( pPane2 ) *pPane2 = ( HANDLE )m_hPane2;
}

// Setup minimum pane sizes.
void ClsSplitter::SetPaneMinSize( int nMinPane1, int nMinPane2 )
{
	// Store values.
	if ( nMinPane1 ) m_nMinPane1 = nMinPane1;
	if ( nMinPane2 ) m_nMinPane2 = nMinPane2;
}

// Setup splitter position.
void ClsSplitter::SetSplitterPosition( int nSplitPos )
{
	// Get the splitter rectangle. If it is
	// empty we use the parent client rectangle.
	ClsRect rc = m_SplitRect;
	if ( rc.IsEmpty()) GetParent()->GetClientRect( rc );

	// Horizontal or vertical?
	int nMaxPos = m_bIsHorizontal ? rc.Width() : rc.Height();
	nMaxPos -= m_nMinPane2 + SPLITTER_SIZE;

	// Make sure the splitter stays in range.
	if ( nSplitPos > nMaxPos     ) nSplitPos = nMaxPos;
	if ( nSplitPos < m_nMinPane1 ) nSplitPos = m_nMinPane1;

	// Did it change?
	if ( m_nPosition != nSplitPos )
	{
		// If the splitter bar is visible we render
		// it again at the same position to make it
		// disappear.
		if ( m_bIsVisible ) DrawSplitterBar();

		// Setup the new position.
		m_nPosition = nSplitPos;

		// Draw the splitter bar.
		DrawSplitterBar();
	}
}

// Redraw splitter panes.
void ClsSplitter::RedrawPanes()
{
	// Make sure the splitter stays in range.
	SetSplitterPosition( m_nPosition );

	// Move the panes.
	MoveSplitter();
}

void ClsSplitter::DrawSplitterBar()
{
	// Are we moving?
	if ( m_bIsMoving )
	{
		// Get the splitter rectangle including
		// the panes.
		ClsRect rc( m_SplitRect );

		// Setup the splitter rectangle.
		if ( m_bIsHorizontal )
		{
			rc.Inflate( 1, 0 );
			rc.Left()   = m_nPosition;
			rc.Right()  = rc.Left() + SPLITTER_SIZE + 2;
		}
		else
		{
			rc.Inflate( 0, 1 );
			rc.Top()    = m_nPosition;
			rc.Bottom() = rc.Top() + SPLITTER_SIZE + 2;
		}

		// Render the splitter bar.
		m_pDC.Rectangle( rc );

		// Flip the visible flag.
		m_bIsVisible = !m_bIsVisible;
	}
	else
		// Move the splitter and the panes.
		MoveSplitter();
}

// Move the splitter and it's panes.
void ClsSplitter::MoveSplitter()
{
	// Are we horizontal?
	if ( m_bIsHorizontal )
	{
		// Do we have room to layout the panes?
		//if ( m_SplitRect.Width() >= ( SPLITTER_SIZE + m_nMinPane2 + m_nMinPane1 ))
		{
			// Set the bounds of the first pane.
			if ( m_hPane1 ) ::MoveWindow( m_hPane1, m_SplitRect.Left(),
								m_SplitRect.Top(),
								m_SplitRect.Left() + m_nPosition,
								m_SplitRect.Height(), TRUE );
			// And the second pane.
			if ( m_hPane2 ) ::MoveWindow( m_hPane2, m_SplitRect.Left() + m_nPosition + SPLITTER_SIZE,
								m_SplitRect.Top(),
								m_SplitRect.Width() - ( m_nPosition + SPLITTER_SIZE ),
								m_SplitRect.Height(), TRUE );

			// And the splitter itself.
			MoveWindow( m_SplitRect.Left() + m_nPosition,
				    m_SplitRect.Top(),
				    SPLITTER_SIZE,
				    m_SplitRect.Height(),
				    TRUE );
		}
	}
	else
	{
		// Do we have room to layout the panes?
		//if ( m_SplitRect.Height() >= ( SPLITTER_SIZE + m_nMinPane2 + m_nMinPane1 ))
		{
			// Set the bounds of the first pane.
			if ( m_hPane1 ) ::MoveWindow( m_hPane1, m_SplitRect.Left(),
								m_SplitRect.Top(),
								m_SplitRect.Width(),
								m_SplitRect.Top() + m_nPosition, TRUE );

			// And the second pane.
			if ( m_hPane2 ) ::MoveWindow( m_hPane2, m_SplitRect.Left(),
								m_SplitRect.Top() + m_nPosition + SPLITTER_SIZE,
								m_SplitRect.Width(),
								m_SplitRect.Height() - ( m_nPosition + SPLITTER_SIZE ), TRUE );

			// And the splitter itself.
			MoveWindow( m_SplitRect.Left(),
				    m_SplitRect.Top() + m_nPosition,
				    m_SplitRect.Width(),
				    SPLITTER_SIZE,
				    TRUE );
		}
	}

	// Redraw panes and splitter.
	Invalidate();
	if ( m_hPane1 ) ::InvalidateRect( m_hPane1, NULL, TRUE );
	if ( m_hPane2 ) ::InvalidateRect( m_hPane2, NULL, TRUE );
}

// Set splitter rectangle.
void ClsSplitter::SetSplitRect( LPRECT lpRect )
{
	_ASSERT_VALID( lpRect ); // Must be valid.

	// Copy the rectangle.
	m_SplitRect = *lpRect;

	// Adjust the splitter control size.
	if ( m_bIsHorizontal ) MoveWindow( m_SplitRect.Left() + m_nPosition, m_SplitRect.Top(), SPLITTER_SIZE, m_SplitRect.Height(), TRUE );
	else		       MoveWindow( m_SplitRect.Left(), m_SplitRect.Top() + m_nPosition, m_SplitRect.Width(), SPLITTER_SIZE, TRUE );
}

// Window procedure override.
LRESULT ClsSplitter::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_ERASEBKGND:
		{
			// Fill the entire client rectangle
			// with the same color as the button
			// background.
			ClsBrush b( ::GetSysColor( COLOR_BTNFACE ));
			::FillRect(( HDC )wParam, GetClientRect(), b );
			return 0;
		}

		case	WM_LBUTTONDOWN:
		{
			// Obtain a device context and attach it.
			if ( m_pDC.Attach( ::GetDC( *GetParent())))
			{
				// Snapshot the device context.
				m_nSaveDC = m_pDC.SaveDC();

				// OK?
				if ( m_nSaveDC )
				{
					// Mark the current position.
					m_nStartPosition = m_nPosition;

					// Setup DC.
					m_pDC.SetROP2( R2_NOTXORPEN );
					m_pDC.SelectObject( m_HatchBrush );
					m_pDC.SelectObject( ::GetStockObject( NULL_PEN ));
					m_pDC.SetTextColor( RGB( 0, 0, 0 ));

					// Capture the mouse.
					SetCapture();

					// We are moving the splitter.
					m_bIsMoving = TRUE;

					// We want the keyboard focus.
					m_hFocus = ::SetFocus( *this );

					// Show the track bar.
					DrawSplitterBar();
					return 0;
				}

				// Failure...
				::ReleaseDC( *GetParent(), m_pDC.Detach());
			}
			return 0;
		}

		case	WM_MOUSEMOVE:
			// Are we moving the splitter?
			if ( m_bIsMoving )
			{
				// Get the current mouse position.
				ClsPoint p;
				GetCursorPos( p );

				// Map the position to our parent.
				GetParent()->ScreenToClient(p);

				// Get position (Horizontal or vertical).
				int nPos = m_bIsHorizontal ? p.X() : p.Y();

				// Set splitter position.
				SetSplitterPosition( nPos );
				return 0;
			}
			break;

		case	WM_GETDLGCODE:
			// We want them all.
			return DLGC_WANTALLKEYS;

		case	WM_KEYDOWN:
			// Are we moving the splitter or was a key
			// pressed other than escape?
			if ( ! m_bIsMoving || wParam != VK_ESCAPE )
				// Pass the message to the base class.
				break;

			// Erase the splitter if necessary.
			if ( m_bIsVisible ) DrawSplitterBar();

			// Restore the old position.
			m_nPosition = m_nStartPosition;

			// Fall through which will stop the
			// mouse tracking etc.


		case	WM_LBUTTONUP:
			// Are we moving?
			if ( m_bIsMoving )
			{
				// Make sure we are invisible.
				if ( m_bIsVisible ) DrawSplitterBar();

				// Restore DC snapshot.
				m_pDC.RestoreDC( m_nSaveDC );

				// Release it and detach it from the object.
				::ReleaseDC( *GetParent(), m_pDC.Detach());

				// Release the mouse.
				ReleaseCapture();

				// No longer moving.
				m_bIsMoving = FALSE;

				// Move the panes.
				MoveSplitter();

				// Restore the focus.
				::SetFocus( m_hFocus );
			}
			return 0;

		case WM_SETCURSOR:
			// Setup the correct cursor.
			if ( m_bIsHorizontal ) SetCursor( m_hVert );
			else		       SetCursor( m_hHorz );
			return 0;
	}

	// Call the base class.
	return ClsWindow::WindowProc( uMsg, wParam, lParam );
}

// For the layout engine.
BOOL ClsSplitter::OnGetMinSize( ClsSize& szMinSize )
{
	// Setup values.
	szMinSize.CX() = szMinSize.CY() = SPLITTER_SIZE;
	return TRUE;
}