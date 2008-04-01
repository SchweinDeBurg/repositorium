#ifndef _LCHECKBOX_H_
#define _LCHECKBOX_H_
//
// lcheckbox.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/controls/checkbox.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../strings/string.h"

// A simple checkbox control for the layout engine.
class ClsLCheckBox : public ClsCheckBox
{
	_NO_COPY( ClsLCheckBox );
public:
	// Constructor.
	ClsLCheckBox() {;}

	// Destructor.
	virtual ~ClsLCheckBox() {;}

protected:
	// Compute the checkbox it's minimum size.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize )
	{
		// Setup checkbox width.
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
			// checkmark height.
			int nHeight = ::GetSystemMetrics( SM_CYMENUCHECK );
			if ( nHeight > szMinSize.CY())
				szMinSize.CY() = nHeight;
		}
		return TRUE;
	}
};

#endif // _LCHECKBOX_H_