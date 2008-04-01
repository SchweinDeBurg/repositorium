#ifndef _LRADIOBUTTON_H_
#define _LRADIOBUTTON_H_
//
// lradiobutton.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/controls/radiobutton.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../strings/string.h"

// A simple radion button control for the layout engine.
class ClsLRadioButton : public ClsRadioButton
{
	_NO_COPY( ClsLRadioButton );
public:
	// Constructor.
	ClsLRadioButton() {;}

	// Destructor.
	virtual ~ClsLRadioButton() {;}

protected:
	// Compute the radiobutton it's minimum size.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize )
	{
		// Setup radio button width.
		szMinSize.CX() = ::GetSystemMetrics( SM_CXMENUCHECK ) + ( 4 * ::GetSystemMetrics( SM_CXEDGE ));		
	
		// Measure caption.
		ClsGetDC	dc( this );
		ClsString	str( GetSafeHWND());

		// Any caption?
		if ( str.GetStringLength())
		{
			// Setup the font to use.
			ClsSelector sel( &dc, ( HFONT )SendMessage( WM_GETFONT ));

			// Add the size of the caption.
			szMinSize.Add( dc.GetTextExtent( str ));

			// Take the largest. Either the text height or the
			// button height.
			int nHeight = ::GetSystemMetrics( SM_CYMENUCHECK );
			if ( nHeight > szMinSize.CY())
				szMinSize.CY() = nHeight;
		}
		return TRUE;
	}
};

#endif // _LRADIOBUTTON_H_