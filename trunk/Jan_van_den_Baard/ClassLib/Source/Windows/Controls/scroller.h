#ifndef _SCROLLER_H_
#define _SCROLLER_H_
//
// scroller.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../coords/rect.h"

// A ClsWindow derived class handling scroller controls.
class ClsScroller : public ClsWindow
{
	_NO_COPY( ClsScroller );
public:
	// Default constructor. Does nothing.
	ClsScroller()
	{;}

	// Constructor. Initializes to the passed
	// handle.
	ClsScroller( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsScroller()
	{;}

	// Create a scroller control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, UINT nStyle = WS_CHILD | WS_VISIBLE |	SBS_BOTTOMALIGN | SBS_HORZ )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsWindow::Create( 0, _T( "SCROLLBAR" ), NULL, nStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Enable the arrows.
	inline BOOL EnableArrows( UINT nFlags )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( SBM_ENABLE_ARROWS, nFlags );
	}

	// Get scroller position.
	inline int GetPos() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the position.
		return ( int )::SendMessage( m_hWnd, SBM_GETPOS, 0, 0 );
	}

	// Get the scroller range.
	inline void GetRange( int& nMin, int& nMax ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the scroller range.
		::SendMessage( m_hWnd, SBM_GETRANGE, ( WPARAM )&nMin, ( LPARAM )&nMax );
	}

	// Get the scroll info.
	inline BOOL GetScrollInfo( LPSCROLLINFO lpsi ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the scroller info.
		return ( BOOL )::SendMessage( m_hWnd, SBM_GETSCROLLINFO, 0, ( LPARAM )lpsi );
	}

	// Set scroller position.
	inline int SetPos( int nPos, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the position.
		return ( int )SendMessage( SBM_SETPOS, nPos, bRedraw );
	}

	// Set scroller range.
	inline int SetRange( int nMin, int nMax, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set scroller range.
		return ( int )SendMessage( bRedraw ? SBM_SETRANGEREDRAW : SBM_SETRANGE, nMin, nMax );
	}

	// Set scroll info.
	inline int SetScrollInfo( LPSCROLLINFO lpsi, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set scroll info.
		return ( int )SendMessage( SBM_SETSCROLLINFO, bRedraw, ( LPARAM )lpsi );
	}
};

#endif // _SCROLLER_H_