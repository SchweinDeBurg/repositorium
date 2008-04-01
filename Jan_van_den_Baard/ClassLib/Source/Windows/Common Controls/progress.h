#ifndef _PROGRESS_H_
#define _PROGRESS_H_
//
// progress.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Progress common control.
class ClsProgress : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsProgress, ClsCommonControl );
public:
	// Constructor.
	ClsProgress() : ClsCommonControl( ICC_PROGRESS_CLASS )
	{;}

	// Destructor.
	virtual ~ClsProgress()
	{;}

	// Create a progress control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_VISIBLE | WS_CHILD )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, PROGRESS_CLASS, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Increase position.
	inline int DeltaPos( int nDelta )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Increase position.
		return ( int )SendMessage( PBM_DELTAPOS, nDelta );
	}

	// Get the position.
	inline UINT GetPos() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get position.
		return ( UINT )::SendMessage( m_hWnd, PBM_GETPOS, 0, 0 );
	}

	// Get the range
	inline int GetRange( BOOL fLowLimit, PBRANGE& range ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the range.
		return ( int )::SendMessage( m_hWnd, PBM_GETRANGE, fLowLimit, ( LPARAM )&range );
	}

	// Set bar color.
	inline COLORREF SetBarColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the color.
		return ( COLORREF )SendMessage( PBM_SETBARCOLOR, 0, crColor );
	}

	// Set the background color.
	inline COLORREF SetBkColor( COLORREF crColor )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the color.
		return ( COLORREF )SendMessage( PBM_SETBKCOLOR, 0, crColor );
	}

	// Set the position.
	inline int SetPos( int nPos )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set position.
		return ( int )SendMessage( PBM_SETPOS, nPos );
	}

	// Set the range.
	inline DWORD SetRange( int nMin, int nMax )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the range.
		return ( DWORD )SendMessage( PBM_SETRANGE, 0, MAKELPARAM( nMin, nMax ));
	}

	// Set the range.
	inline DWORD SetRange32( int nMin, int nMax )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set range.
		return ( DWORD )SendMessage( PBM_SETRANGE32, nMin, nMax );
	}

	// Set step increment.
	inline int SetStep( int nStep )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set step value.
		return ( int )SendMessage( PBM_SETSTEP, nStep );
	}

	// Step position.
	inline int StepIt()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Step...
		return ( int )SendMessage( PBM_STEPIT );
	}
};

#endif // _PROGRESS_H_