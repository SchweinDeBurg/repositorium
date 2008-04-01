//
// recttracker.cpp
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "recttracker.h"
#include "../gdi/selector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsRectTracker::ClsRectTracker()
{
	// Setup defaults.
	m_nStyle    = rttHatched;
	m_bVisible  = FALSE;
	m_bTracking = FALSE;
}

// Destruction.
ClsRectTracker::~ClsRectTracker()
{
}

// Obtain tracker rectangle.
void ClsRectTracker::GetTrackedRect( ClsWindow *pWindow, ClsRect& rcRect, BOOL bClient /* = FALSE */ ) 
{ 
	// Copy rectangle (screen coordinates).
	rcRect = m_rcRect;

	// Client?
	if ( bClient )
	{
		_ASSERT_VALID( pWindow ); // Must be valid.
		pWindow->ScreenToClient( rcRect );
	}
}

// Setup rectangle style.
void ClsRectTracker::SetRectStyle( int nStyle )
{
	_ASSERT( nStyle >= rttSolid && nStyle <= rttHatched );

	// Did the style change?
	if ( m_nStyle != nStyle )
	{
		// Are we tracking? If so we also need
		// to make sure the rectangle is not
		// visible.
		if ( m_bTracking && m_bVisible )
			Rectangle();

		// Save the style.
		m_nStyle = nStyle;
	}
}

// Draw the rectangle.
void ClsRectTracker::Rectangle()
{
	// Anything to render?
	if ( m_rcRect.IsEmpty())
		return;

	// Hatched?
	if ( m_nStyle == rttHatched )
	{
		// Select the brush into the DC.
		ClsSelector sel( &m_DC, ClsGetApp()->GetPatternBrush());

		// The rectange must be atleast 6*6 pixels to render
		// a rectangle.
		if ( m_rcRect.Width() <= 6 || m_rcRect.Height() <= 6 )
			m_DC.PatBlt( m_rcRect.Left(), m_rcRect.Top(), m_rcRect.Width(), m_rcRect.Height(), PATINVERT );
		else
		{
			// Render a rectangle.
			m_DC.PatBlt( m_rcRect.Left(),	   m_rcRect.Top(),	  m_rcRect.Width(), 3,			   PATINVERT );
			m_DC.PatBlt( m_rcRect.Left(),	   m_rcRect.Top() + 3,	  3,		    m_rcRect.Height() - 6, PATINVERT );
			m_DC.PatBlt( m_rcRect.Right() - 3, m_rcRect.Top() + 3,	  3,		    m_rcRect.Height() - 6, PATINVERT );
			m_DC.PatBlt( m_rcRect.Left(),	   m_rcRect.Bottom() - 3, m_rcRect.Width(), 3,			   PATINVERT );
		}
	}
	else
	{
		// Select a black stock pen and render the rectangle.
		ClsSelector sel( &m_DC, ::GetStockObject( BLACK_PEN ));
		m_DC.Rectangle( m_rcRect );
	}

	// Invert visibility flag.
	m_bVisible = ! m_bVisible;
}

// Set rectangle size. This may only be called while
// tracking is enabled.
void ClsRectTracker::SetRectSize( LPSIZE lpSize )
{
	_ASSERT( m_bTracking ); // Must be tracking.
	_ASSERT( lpSize->cx > 0 && lpSize->cy > 0 ); // Values must be valid.

	// Are we visible?
	if ( m_bVisible ) Rectangle();

	// Setup new rectangle size.
	m_rcRect.Right() = m_rcRect.Left() + lpSize->cx;
	m_rcRect.Bottom() = m_rcRect.Top() + lpSize->cy;
}

// Handle mousebutton event.
BOOL ClsRectTracker::OnMouseButton( ClsWindow *pWindow, UINT uMsg )
{
	// We stop tracking when the left-mouse
	// button is released.
	if ( uMsg == WM_LBUTTONUP )
		return FALSE;
	return TRUE;
}

// Render tracker rectangle.
void ClsRectTracker::DrawRect( ClsWindow *pWindow )
{
	// Are we visible? If so we render again at the last
	// spot to erase the current rectangle.
	if ( m_bVisible ) Rectangle();

	// Get the current rectangle position.
	ComputeRect( pWindow );

	// Render rectangle.
	Rectangle();
}

// Compute rectangle position.
void ClsRectTracker::ComputeRect( ClsWindow *pWindow )
{
	// Get the current mouse position in screen coordinates
	// and compute the difference with the last position.
	ClsPoint pt = m_ptCurrent;
	pWindow->ClientToScreen( pt );
	ClsPoint offset = m_ptCurrPos;
	offset -= pt;

	// Offset the rectangle.
	m_rcRect.Offset( -offset.X(), -offset.Y());

	// Save current position is screen coordinates.
	m_ptCurrPos = pt;	
}

// Enter tracking loop.
BOOL ClsRectTracker::OnTrackEnter( ClsWindow *pWindow )
{
	// Create DC.
	HDC hDC = ::GetDC( NULL );
	if ( hDC )
	{
		// Attach it to the object.
		m_DC.Attach( hDC );

		// Setup initial values.
		m_bVisible = FALSE;
		pWindow->GetWindowRect( m_rcRect );
		m_ptCurrPos = m_ptCurrent;
		pWindow->ClientToScreen( m_ptCurrPos );
		m_bTracking = TRUE;
		
		// Setup the device context.
		m_nOldROP2 = m_DC.SetROP2( R2_NOTXORPEN );

		// Draw rectangle.
		DrawRect( pWindow );
		return TRUE;
	}
	return FALSE;
}

// Canceled tracking loop?
BOOL ClsRectTracker::OnTrackCancel( ClsWindow *pWindow )
{
	// Canceled?
	if ( ClsTracker::OnTrackCancel( pWindow ) == TRUE )
	{
		// Erase rectangle if it is visible.
		if ( m_bVisible ) Rectangle();

		// Recompute the rectangle.
		ComputeRect( pWindow );
		return TRUE;
	}
	return FALSE;
}

// Exit tracking loop.
void ClsRectTracker::OnTrackExit( ClsWindow *pWindow )
{
	// Erase rectangle if it is visible.
	if ( m_bVisible ) Rectangle();

	// Get the current rectangle position.
	ComputeRect( pWindow );

	// Release the DC.
	m_DC.SetROP2( m_nOldROP2 );
	::ReleaseDC( NULL, m_DC.Detach());

	// No longer tracking.
	m_bTracking = FALSE;
}

// Moving...
BOOL ClsRectTracker::OnTrackMove( ClsWindow *pWindow )
{
	// Render the rectangle.
	DrawRect( pWindow );
	return TRUE;
}