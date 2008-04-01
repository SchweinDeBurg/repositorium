#ifndef _LEDIT_H_
#define _LEDIT_H_
//
// ledit.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/controls/edit.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../strings/string.h"

// A simple edit control for the layout engine.
class ClsLEdit : public ClsEdit
{
	_NO_COPY( ClsLEdit );
public:
	// Constructor.
	ClsLEdit() {;}

	// Destructor.
	virtual ~ClsLEdit() {;}

protected:
	// Compute the edit it's minimum size.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize )
	{
		// Measure a four character string.
		ClsGetDC dc( this );

		// Setup the font to use.
		ClsSelector sel( &dc, ( HFONT )SendMessage( WM_GETFONT ));

		// Measure it.
		szMinSize = dc.GetTextExtent( _T( "MWMW" ), 4 );

		// Multiline controls will get three 
		// lines as minimum height. Also we add space
		// for the vertical scroller.
		if ( GetStyle() & ES_MULTILINE ) 
		{
			szMinSize.CY() *= 3;
			szMinSize.CX() += ::GetSystemMetrics( SM_CXVSCROLL );
		}

		// Add frame sizes.
		if ( GetExStyle() & WS_EX_CLIENTEDGE ) 
		{
			szMinSize.CX() += ::GetSystemMetrics( SM_CXEDGE ) * 4;
			szMinSize.CY() += ::GetSystemMetrics( SM_CYEDGE ) * 4;
		}
		return TRUE;
	}
};

#endif // _LEDIT_H_