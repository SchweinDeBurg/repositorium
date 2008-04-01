#ifndef _UPDOWN_H_
#define _UPDOWN_H_
//
// updown.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the Up-Down common control.
class ClsUpDown : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsUpDown, ClsCommonControl );
public:
	// Constructor.
	ClsUpDown() : ClsCommonControl( ICC_WIN95_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsUpDown()
	{;}

	// Create an up-down control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_CHILD | WS_VISIBLE )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, UPDOWN_CLASS, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Get accelerator information.
	inline UINT GetAccel( int cAccel, LPUDACCEL pAccelArray ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, UDM_GETACCEL, cAccel, ( LPARAM )pAccelArray );
	}

	// Get the current radix base,
	inline UINT GetBase() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )::SendMessage( m_hWnd, UDM_GETBASE, 0, 0 );
	}

	// Get the buddy window.
	inline HWND GetBuddy() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )::SendMessage( m_hWnd, UDM_GETBUDDY, 0, 0 );
	}

	// Get the position.
	inline DWORD GetPos() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, UDM_GETPOS, 0, 0 );
	}

	// Get the range.
	inline DWORD GetRange() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( DWORD )::SendMessage( m_hWnd, UDM_GETRANGE, 0, 0 );
	}

	// Get the range.
	inline void GetRange32( int& nLow, int& nHight ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		::SendMessage( m_hWnd, UDM_GETRANGE32, ( WPARAM )&nLow, ( LPARAM )&nHight );
	}

	// Get the unicode format.
	inline BOOL GetUnicodeFormat() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )::SendMessage( m_hWnd, UDM_GETUNICODEFORMAT, 0, 0 );
	}

	// Set accelerators.
	inline BOOL SetAccel( int nAccel, LPUDACCEL pAccelArray )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( UDM_SETACCEL, nAccel, ( LPARAM )pAccelArray );
	}

	// Set the radix base.
	inline UINT SetBase( UINT nBase )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( UINT )SendMessage( UDM_SETBASE, nBase );
	}

	// Set the buddy.
	inline HWND SetBuddy( HWND hWndBuddy )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( HWND )SendMessage( UDM_SETBUDDY, ( WPARAM )hWndBuddy );
	}

	// Set the position.
	inline int SetPos( int nPos )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( int )SendMessage( UDM_SETPOS, 0, MAKELONG( nPos, 0 ));
	}

	// Set the range.
	inline void SetRange( int nLow, int nHigh )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( UDM_SETRANGE, 0, MAKELONG( nHigh, nLow ));
	}

	// Set the range.
	inline void SetRange32( int nLow, int nHigh )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		SendMessage( UDM_SETRANGE32, nLow, nHigh );
	}

	// Set unicode flag.
	inline BOOL SetUnicodeFormat( BOOL bUnicode )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		return ( BOOL )SendMessage( UDM_SETUNICODEFORMAT, bUnicode );
	}
protected:
	// Overidables.
	virtual LRESULT OnDeltaPos( LPNMUPDOWN pUpDown, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		if ( pNMHDR->code == UDN_DELTAPOS )
			return OnDeltaPos(( LPNMUPDOWN )pNMHDR, bNotifyParent );

		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _UPDOWN_H_