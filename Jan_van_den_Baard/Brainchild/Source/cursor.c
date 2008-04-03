/*
 *	cursor.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Cursor related routines.
 */

#include "stdafx.h"

#include "defs.h"

LRESULT OnSetCursor( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Are we inside the client area?
	 */
	if ( LOWORD( lParam ) == HTCLIENT )
	{
		/*
		 *	Convert the current cursor position to
		 *	client coordinates.
		 */
		POINT		ptCursor;
		GetCursorPos( &ptCursor );
		ScreenToClient( hWnd, &ptCursor );

                /*
		 *	Inside the selection margin?
		 */
		if ( ptCursor.x < ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ))) SetCursor( lpcd->hSelection );
		else if ( MouseOnSelection( lpcd ) && Parser->bDnDEnabled == TRUE )	  SetCursor( lpcd->hArrow );
		else									  SetCursor( lpcd->hIBeam );

		return 0;
	}
	
	/*
	 *	Call the superclass.
	 */
	return DefWindowProc( hWnd, WM_SETCURSOR, wParam, lParam );
}
