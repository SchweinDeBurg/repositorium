#ifndef _IPADDRESS_H_
#define _IPADDRESS_H_
//
// ipaddress.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commoncontrol.h"

// A wrapper for the IP Adress common control.
class ClsIPAddress : public ClsCommonControl
{
	_NO_COPY_PARAM( ClsIPAddress, ClsCommonControl );
public:
	// Constructor.
	ClsIPAddress() : ClsCommonControl( ICC_INTERNET_CLASSES )
	{;}

	// Destructor.
	virtual ~ClsIPAddress()
	{;}

	// Create an IP address control.
	inline BOOL Create( ClsWindow *parent, ClsRect& rBounds, UINT nID, DWORD dwStyle = WS_VISIBLE | WS_CHILD )
	{
		_ASSERT( m_hWnd == NULL ); // May not be valid.

		// Create the control.
		return ClsCommonControl::Create( 0, WC_IPADDRESS, NULL, dwStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Clear the address.
	inline void ClearAddress()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Clear the address.
		SendMessage( IPM_CLEARADDRESS );
	}

	// Get the address.
	inline int GetAddress( DWORD& dwAddress ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the address.
		return ( int )::SendMessage( m_hWnd, IPM_GETADDRESS, 0, ( LPARAM )&dwAddress );
	}

	// Is the address blank.
	inline BOOL IsBlank() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the contents state.
		return ( BOOL )::SendMessage( m_hWnd, IPM_ISBLANK, 0, 0 );
	}

	// Set the address.
	inline void SetAddress( DWORD dwAddress )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the address.
		SendMessage( IPM_SETADDRESS, 0, dwAddress );
	}

	// Set the focus to the specified field.
	inline void SetFieldFocus( int nField )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the focus.
		SendMessage( IPM_SETFOCUS, nField );
	}

	// Set the range for the specified field.
	inline BOOL SetRange( int nField, WORD wRange )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the range.
		return ( BOOL )SendMessage( IPM_SETRANGE, nField, wRange );
	}

protected:
	// Overidables.
	virtual LRESULT OnFieldChanged( LPNMIPADDRESS pAddress, BOOL& bNotifyParent )
	{ return 0; }

	// Reflected notification handler which will call
	// the virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// Evaluate notification code.
		if ( pNMHDR->code == IPN_FIELDCHANGED )
			return OnFieldChanged(( LPNMIPADDRESS )pNMHDR, bNotifyParent );
	
		// Call the base class.
		return ClsCommonControl::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _IPADDRESS_H_