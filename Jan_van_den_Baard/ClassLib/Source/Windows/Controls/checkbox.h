#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_
//
// checkbox.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "button.h"

// A ClsButton derived class handling checkbox button controls.
class ClsCheckBox : public ClsButton
{
	_NO_COPY( ClsCheckBox );
public:
	// Default constructor. Does nothing.
	ClsCheckBox()
	{;}

	// Constructor. Initializes to the passed
	// handle.
	ClsCheckBox( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsCheckBox()
	{;}

	// Create a checkbox control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszName, UINT nStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE | BS_AUTOCHECKBOX )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsButton::Create( parent, rBounds, nID, pszName, nStyle );
	}

	// Get the button checked state.
	inline UINT GetCheck() const
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Return the checked state.
		return ( UINT )::SendMessage( m_hWnd, BM_GETCHECK, 0, 0 );
	}
	
	// Set the button checked state.
	inline void SetCheck( UINT nCheck = BST_CHECKED )
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// Set the checked state.
		SendMessage( BM_SETCHECK, nCheck );
	}

	// Is the button checked?
	inline BOOL IsChecked() const
	{
		_ASSERT_VALID( GetSafeHWND()); // Must be valid.

		// return the checked state.
		return ( BOOL )( GetCheck() == BST_CHECKED );
	}
};

#endif // _CHECKBOX_H_
