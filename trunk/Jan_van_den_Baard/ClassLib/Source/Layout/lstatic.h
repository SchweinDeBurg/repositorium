#ifndef _LSTATIC_H_
#define _LSTATIC_H_
//
// lstatic.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../windows/controls/static.h"
#include "../gdi/bitmap.h"
#include "../gdi/getdc.h"
#include "../gdi/selector.h"
#include "../strings/string.h"

// A simple static control for the layout engine.
class ClsLStatic : public ClsStatic
{
	_NO_COPY( ClsLStatic );
public:
	// Constructor.
	ClsLStatic() {;}

	// Destructor.
	virtual ~ClsLStatic() {;}

protected:
	// Compute the static control it's minimum size.
	// Note that this code will not work for ownerdraw
	// controls.
	virtual BOOL OnGetMinSize( ClsSize& szMinSize )
	{
		// Get styles and extended styles.
		DWORD dwStyle = GetStyle();
		
		// Horizontal/vertical etched line?
		if ((( dwStyle & SS_ETCHEDHORZ ) == SS_ETCHEDHORZ ) || (( dwStyle & SS_ETCHEDVERT ) == SS_ETCHEDVERT )) 
		{
			szMinSize.CX() = szMinSize.CY() = 2;
			return TRUE;
		}

		// What type is the control?
		if (( dwStyle & SS_BITMAP ) == SS_BITMAP )
		{
			// Get the bitmap.
			ClsBitmap	bm;
			bm.Attach(( HBITMAP )SendMessage( STM_GETIMAGE, IMAGE_BITMAP ), FALSE );

			// Get it's information.
			BITMAP bMap;
			bm.GetBitmap( &bMap );

			// Store sizes.
			szMinSize.CX() = bMap.bmWidth;
			szMinSize.CY() = bMap.bmHeight;
		}
		else if (( dwStyle & SS_ICON ) == SS_ICON )
		{
			// Store system icon sizes.
			szMinSize.CX() = ::GetSystemMetrics( SM_CXICON );
			szMinSize.CY() = ::GetSystemMetrics( SM_CYICON );
		}
		else
		{
			// Get the caption.
			ClsString str( GetSafeHWND());
			
			// Any text?
			if ( str.GetStringLength())
			{
				// Get DC.
				ClsGetDC dc( this );

				// Select font.
				ClsSelector sel( &dc, ( HFONT )SendMessage( WM_GETFONT ));

				// Measure the text.
				ClsRect rc( 0, 0, 0, 0 );
				dc.DrawText( str, rc, DT_CALCRECT );

				// Store values.
				szMinSize.Add( rc.Size());
			}
		}

		// Adjust for a sunken edge.
		if ( dwStyle & SS_SUNKEN ) 
		{
			szMinSize.CX() += 4;
			szMinSize.CY() += 4;
		}
		return TRUE;
	}
};

#endif // _LSTATIC_H_