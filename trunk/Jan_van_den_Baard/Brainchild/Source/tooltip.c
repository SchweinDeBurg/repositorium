/*
 *	tooltip.c
 *
 *	(C) Copyright 2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	Compute the tip position. It will always be
 *	two pixels below the line on which the mouse
 *	cursor is located.
 */
DWORD TipPositionByMouse( LPCLASSDATA lpcd )
{
	/*
	 *	Obtain the mouse position.
	 */
	POINT pt;
	GetCursorPos( &pt );

	/*
	 *	Convert to client coordinates.
	 */
	ScreenToClient( lpcd->hWnd, &pt );

	/*
	 *	Now adjust the y position to be 2 pixels
	 *	below the bottom of the line on which the 
	 *	mouse cursor is located.
	 */
	pt.y = ((( pt.y / Parser->szCharSize.cy ) + 1 ) * Parser->szCharSize.cy ) + 2;

	/*
	 *	Convert the coordinates back to
	 *	screen coordinates and return the
	 *	result.
	 */
	ClientToScreen( lpcd->hWnd, &pt );
	return ( MAKELONG( pt.x, pt.y ));
}

/*
 *	Static tooltip buffer. The code below will
 *	make sure that the tooltip will never grow
 *	beyond the size of this buffer.
 */
TCHAR szTipBuffer[ 1024 ];

void SetTooltipText( LPCLASSDATA lpcd, LPCTSTR pszText )
{
	/*
	 *	Show tips? Is the tooltip opened?
	 */
	if ( Parser->bShowHyperTips && lpcd->bTipShowing )
	{
		TOOLINFO ti = { 0 };

		/*
		 *	Truncate the tip string if it
		 *	grows beyond 1024 characters.
		 */
		int nLength = _tcslen( pszText );
		if ( nLength < 1024 ) _tcscpy( szTipBuffer, pszText );
		else _tcsncpy( szTipBuffer, pszText, 1024 );

		/*
		 *	Setup the tooltip.
		 */
		ti.cbSize   = sizeof( ti );
		ti.hwnd     = lpcd->hWnd;
		ti.uId      = ( UINT_PTR )( HWND )lpcd->hWnd;
		ti.hinst    = GetModuleHandle( NULL );
		ti.lpszText = szTipBuffer;

		/*
		 *	Get the current tip text.
		 */
		SendMessage( lpcd->hTooltip, TTM_GETTEXT, 0, ( LPARAM )&ti );

		/*
		 *	Did it realy change?
		 */
		if ( _tcscmp( szTipBuffer, pszText ) != 0 )
		{
			/*
			 *	Set the new text.
			 */
			ti.lpszText = ( LPTSTR )pszText;

			/*
			 *	Update and position the tooltip.
			 */
			SendMessage( lpcd->hTooltip, TTM_UPDATETIPTEXT, 0, ( LPARAM )&ti );
			SendMessage( lpcd->hTooltip, TTM_TRACKPOSITION, 0, TipPositionByMouse( lpcd ));
		}
	}
}

void ShowTooltip( LPCLASSDATA lpcd, LPCTSTR pszText )
{
	TOOLINFO ti = { 0 };

	/*
	 *	Show tips?
	 */
	if ( Parser->bShowHyperTips )
	{
		/*
		 *	Truncate the tooltip string if it
		 *	grows larger than 1024 characters.
		 */
		int nLength = _tcslen( pszText );
		if ( nLength < 1024 ) _tcscpy( szTipBuffer, pszText );
		else _tcsncpy( szTipBuffer, pszText, 1024 );

		/*
		 *	Setup the tooltip.
		 */
		ti.cbSize   = sizeof( ti );
		ti.hwnd     = lpcd->hWnd;
		ti.uId      = ( UINT_PTR )( HWND )lpcd->hWnd;
		ti.hinst    = GetModuleHandle( NULL );
		ti.lpszText = szTipBuffer;

		/*
		 *	Update and show the tooltip.
		 */
		SendMessage( lpcd->hTooltip, TTM_UPDATETIPTEXT, 0, ( LPARAM )&ti );
		SendMessage( lpcd->hTooltip, TTM_TRACKPOSITION, 0, TipPositionByMouse( lpcd ));
		SendMessage( lpcd->hTooltip, TTM_TRACKACTIVATE, TRUE, ( LPARAM )&ti );

		/*
		 *	Tooltip visible.
		 */
		lpcd->bTipShowing = TRUE;
	}
}

void HideTooltip( LPCLASSDATA lpcd )
{
	/*
	 *	Is the tooltip visible?
	 */
	if ( lpcd->bTipShowing )
	{
		/*
		 *	Setup the TOOLINFO structure.
		 */
		TOOLINFO ti = { 0 };
		ti.cbSize   = sizeof( ti );
		ti.hwnd     = lpcd->hWnd;
		ti.uId	    = ( UINT_PTR )( HWND )lpcd->hWnd;

		/*
		 *	Hide the tooltip.
		 */
		SendMessage( lpcd->hTooltip, TTM_TRACKACTIVATE, FALSE, ( LPARAM )&ti );
		lpcd->bTipShowing = FALSE;
	}
}
