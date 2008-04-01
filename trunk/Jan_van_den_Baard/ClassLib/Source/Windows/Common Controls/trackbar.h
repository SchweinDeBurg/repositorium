#ifndef _TRACKBAR_H_
#define _TRACKBAR_H_
//
// trackbar.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Trackbar common control.
class ClsTrackbar : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsTrackbar, ClsCommonControl );
public:
	// Constructor.
	ClsTrackbar() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsTrackbar()
	{;}

	// Create a trackbar control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, LPCTSTR pszName, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, TRACKBAR_CLASS, pszName, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Clear selection range.
	inline void ClearSel( BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_CLEARSEL, bRedraw );
	}

	// Clear tic marks.
	inline void ClearTics( BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_CLEARTICS, bRedraw );
	}

	// Get the buddy control.
	inline HWND GetBuddy( BOOL bLocation ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )::SendMessage( m_hWnd, TBM_GETBUDDY, bLocation, 0 );
	}

	// Get the channel rect.
	inline void GetChannelRect( LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, TBM_GETCHANNELRECT, 0, ( LPARAM )pRect );
	}

	// Get the line size.
	inline long GetLineSize() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETLINESIZE, 0, 0 );
	}
	
	// Get the number of tics.
	inline UINT GetNumTics() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, TBM_GETNUMTICS, 0, 0 );
	}

	// Get the page size.
	inline long GetPageSize() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETPAGESIZE, 0, 0 );
	}

	// Get the slider position.
	inline long GetPos() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETPOS, 0, 0 );
	}

	// Get the trackbar tic positions.
	inline LPDWORD GetPTics() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( LPDWORD )::SendMessage( m_hWnd, TBM_GETPTICS, 0, 0 );
	}

	// Get maximum range.
	inline long GetRangeMax() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETRANGEMAX, 0, 0 );
	}

	// Get the minimum range.
	inline long GetRangeMin() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETRANGEMIN, 0, 0 );
	}

	// Get the current selection end.
	inline long GetSelEnd() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETSELEND, 0, 0 );
	}

	// Get the current selection start.
	inline long GetSelStart() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETSELSTART, 0, 0 );
	}

	// Get the thumb length.
	inline int GetThumbLength() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TBM_GETTHUMBLENGTH, 0, 0 );
	}

	// Get the thumb bounds.
	inline void GetThumbRect( LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, TBM_GETTHUMBRECT, 0, ( LPARAM )pRect );
	}

	// Get the logical position of a tic mark.
	inline long GetTic( int iTic ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )::SendMessage( m_hWnd, TBM_GETTIC, iTic, 0 );
	}

	// Get the position of a tic mark.
	inline int GetTicPos( int iTic ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )::SendMessage( m_hWnd, TBM_GETTICPOS, iTic, 0 );
	}

	// Get the tooltip control.
	inline HWND GetToolTips() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )::SendMessage( m_hWnd, TBM_GETTOOLTIPS, 0, 0 );
	}

	// Get unicode flag.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, TBM_GETUNICODEFORMAT, 0, 0 );
	}

	// Set buddy control.
	inline HWND SetBuddy( BOOL bLocation, HWND hWndBuddy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )SendMessage( TBM_SETBUDDY, bLocation, ( LPARAM )hWndBuddy );
	}

	// Set line size.
	inline long SetLineSize( long lLineSize )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )SendMessage( TBM_SETLINESIZE, 0, lLineSize );
	}

	// Set page size.
	inline long SetPageSize( long lPageSize )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( long )SendMessage( TBM_SETPAGESIZE, 0, lPageSize );
	}

	// Set thumb position.
	inline void SetPos( long lPos, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETPOS, bRedraw, lPos );
	}

	// Set slider range.
	inline void SetRange( int nMin, int nMax, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETRANGE, bRedraw, MAKELONG( nMin, nMax ));
	}

	// Set maximum slider position.
	inline void SetRangeMax( long lMax, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETRANGEMAX, bRedraw, lMax );
	}

	// Set minimum slider position.
	inline void SetRangeMin( long lMin, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETRANGEMIN, bRedraw, lMin );
	}

	// Set the selection.
	inline void SetSel( int nMin, int nMax, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETSEL, bRedraw, MAKELONG( nMin, nMax ));
	}

	// Set selection end.
	inline void SetSelEnd( long lEnd, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETSELEND, bRedraw, lEnd );
	}

	// Set selection start.
	inline void SetSelStart( long lStart, BOOL bRedraw = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETSELSTART, bRedraw, lStart );
	}

	// Set the thumb length.
	inline void SetThumbLength( int iLength )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETTHUMBLENGTH, iLength );
	}

	// Set tic at position.
	inline BOOL SetTic( long lPosition )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TBM_SETTIC, 0, lPosition );
	}

	// Set the tic mark frequency.
	inline void SetTicFreq( WORD wFreq )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETTICFREQ, wFreq );
	}

	// Set the tip location.
	inline int SetTipSide( int iSide )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( TBM_SETTIPSIDE, iSide );
	}

	// Set the tooltip control.
	inline void SetToolTips( HWND hWndTooltip )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( TBM_SETTOOLTIPS, ( WPARAM )hWndTooltip );
	}

	// Set the unicode format flag.
	inline BOOL SetUnicodeFormat( BOOL bFormat )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( TBM_SETUNICODEFORMAT, bFormat );
	}
};

#endif // _TRACKBAR_H_
