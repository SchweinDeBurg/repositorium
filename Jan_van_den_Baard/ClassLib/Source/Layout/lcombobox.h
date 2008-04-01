#ifndef _LCOMBOBOX_H_
#define _LCOMBOBOX_H_
//
// lcombobox.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/controls/combobox.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../strings/string.h"

// A simple combobox control for the layout engine.
class ClsLComboBox : public ClsComboBox
{
	_NO_COPY( ClsLComboBox );
public:
	// Constructor.
	ClsLComboBox() {;}

	// Destructor.
	virtual ~ClsLComboBox() {;}

protected:
	// Compute the checkbox it's minimum size. Note that this code
	// will only work properly on non-ownerdraw combo boxes.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize )
	{
		// Get the width and height of a simple six
		// character string.
		ClsGetDC dc( this );

		// Setup the font to use.
		ClsSelector sel( &dc, ( HFONT )SendMessage( WM_GETFONT ));
		
		// Measure it.
		szMinSize = dc.GetTextExtent( _T( "MXWMXW" ), 6 );

		// Add the dropdown arrow width.
		szMinSize.CX() += ::GetSystemMetrics( SM_CXHSCROLL );
	
		// Add frame sizes.
		szMinSize.CX() += ::GetSystemMetrics( SM_CXEDGE ) * 4;
		szMinSize.CY() += ::GetSystemMetrics( SM_CYEDGE ) * 4;
		return TRUE;
	}
};

#endif // _LCOMBOBOX_H_