#ifndef _RADIOBUTTON_H_
#define _RADIOBUTTON_H_
//
// radiobutton.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "checkbox.h"

// A ClsCheckBox derived class handling radiobutton controls.
class ClsRadioButton : public ClsCheckBox
{
	_NO_COPY( ClsRadioButton );
public:
	// Default constructor. Does nothing.
	ClsRadioButton()
	{;}

	// Constructor. Initializes to the passed
	// handle.
	ClsRadioButton( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsRadioButton()
	{;}

	// Create a radiobutton control.
	inline BOOL Create( ClsWindow* parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszName, UINT nStyle = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE | BS_AUTORADIOBUTTON )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsButton::Create( parent, rBounds, nID, pszName, nStyle );
	}
};

#endif // _RADIOBUTTON_H_