#ifndef _LPUSHBUTTON_H_
#define _LPUSHBUTTON_H_
//
// lpushbutton.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/controls/button.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../strings/string.h"

// A simple button control for the layout engine.
class ClsLPushButton : public ClsButton
{
	_NO_COPY( ClsLPushButton );
public:
	// Constructor.
	ClsLPushButton() {;}

	// Destructor.
	virtual ~ClsLPushButton() {;}

protected:
	// Compute the button it's minimum size. Please note that this
	// code will only work for simple text push buttons. Ownerdraw 
	// buttons and bitmap buttons need other code.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize )
	{
		// Setup frame sizes.
		szMinSize.CX() = ::GetSystemMetrics( SM_CXEDGE ) * 6;
		szMinSize.CY() = ::GetSystemMetrics( SM_CYEDGE ) * 5;

		// Measure caption.
		ClsGetDC	dc( this );
		ClsString	str( GetSafeHWND());

		// Any caption?
		if ( str.GetStringLength())
		{
			// Setup the font to use.
			ClsSelector sel( &dc, ( HFONT )SendMessage( WM_GETFONT ));

			// Measure the text length and add it
			// to the frame sizes.
			szMinSize.Add( dc.GetTextExtent( str ));
		}
		return TRUE;
	}
};

#endif // _LPUSHBUTTON_H_