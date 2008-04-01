#ifndef _LLISTBOX_H_
#define _LLISTBOX_H_
//
// llistbox.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/controls/listbox.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../strings/string.h"

// A simple listbox control for the layout engine.
class ClsLListBox : public ClsListBox
{
	_NO_COPY( ClsLListBox );
public:
	// Constructor.
	ClsLListBox() {;}

	// Destructor.
	virtual ~ClsLListBox() {;}

protected:
	// Compute the listbox control it's minimum size.
	// Note that this code will not work for ownerdraw
	// controls.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize )
	{
		// Get the width and height of a simple six
		// character string.
		ClsGetDC dc( this );

		// Setup the font to use.
		ClsSelector sel( &dc, ( HFONT )SendMessage( WM_GETFONT ));
		
		// Measure it.
		szMinSize = dc.GetTextExtent( _T( "MXWMXW" ), 6 );

		// Show at least three items.
		szMinSize.CY() *= 3;

		// Add the scroller width.
		szMinSize.CX() += GetSystemMetrics( SM_CXHSCROLL );

		// Add frame sizes.
		DWORD dwExStyle = GetExStyle();
		if (( dwExStyle & WS_EX_CLIENTEDGE ) == WS_EX_CLIENTEDGE ) 
		{
			szMinSize.CX() += ::GetSystemMetrics( SM_CXEDGE ) * 4;
			szMinSize.CY() += ::GetSystemMetrics( SM_CYEDGE ) * 2;
		}
		return TRUE;
	}
};

#endif // _LLISTBOX_H_