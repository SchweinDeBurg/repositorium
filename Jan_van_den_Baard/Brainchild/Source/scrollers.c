/*
 *	scrollers.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Scroller functions.
 */

#include "stdafx.h"

#include "defs.h"

void SetupVScroller( LPCLASSDATA lpcd )
{
	SCROLLINFO	si;

	/*
	 *	Setup new situation.
	 */
	si.cbSize	= sizeof ( si );
	si.fMask	= SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin		= 0;
	si.nMax		= ArrayGetSize( lpcd->lpLines ) - 1;
	si.nPos         = lpcd->ptViewPos.y;
	si.nPage        = lpcd->szViewSize.cy;

        SetScrollInfo( lpcd->hWnd, SB_VERT, &si, TRUE );

	/*
	 *	Hide the tooltip.
	 */
	HideTooltip( lpcd );
}

void SetupHScroller( LPCLASSDATA lpcd )
{
	SCROLLINFO	si;
	
	/*
	 *	Setup new situation.
	 */
	si.cbSize	= sizeof ( si );
	si.fMask	= SIF_PAGE | SIF_POS | SIF_RANGE;
	si.nMin		= 0;
	si.nMax		= lpcd->nLongestLine ? lpcd->nLongestLine - 1 : 0;
	si.nPos         = lpcd->ptViewPos.x;
	si.nPage        = lpcd->szViewSize.cx;

        SetScrollInfo( lpcd->hWnd, SB_HORZ, &si, TRUE );

	/*
	 *	Hide the tooltip.
	 */
	HideTooltip( lpcd );
}

LRESULT OnVScroll( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	int		nLines = ArrayGetSize( lpcd->lpLines );
	LONG		lSTop, lOTop, lDiff = lpcd->ptViewPos.y;

	/*
	 *	Hide the tooltip.
	 */
	HideTooltip( lpcd );

	/*
	 *	Get current position.
	 */
	lSTop = lOTop = lpcd->ptViewPos.y;

	switch ( LOWORD( wParam )) 
	{
		case    SB_PAGELEFT:
			lSTop -= lpcd->szViewSize.cy;
			break;

		case    SB_PAGERIGHT:
			lSTop += lpcd->szViewSize.cy;
			break;

		case    SB_LINELEFT:
                        lSTop--;
			break;

		case    SB_LINERIGHT:
			lSTop++;
			break;

		case    SB_LEFT:
			lSTop = 0;
			break;

		case    SB_RIGHT:
			lSTop = nLines - lpcd->szViewSize.cy;
			break;

		case    SB_THUMBTRACK:
		{
			/*
			 *	Make sure we use a 32Bit position.
			 */
			SCROLLINFO	si;
			si.cbSize = sizeof( si );
			si.fMask = SIF_TRACKPOS;
			if ( GetScrollInfo( lpcd->hWnd, SB_VERT, &si ))
				lSTop = si.nTrackPos;
			break;
		}
	}

	/*
	 *	Make sure the position stays in range.
	 */
	if ( lSTop < 0 ) lSTop = 0;
        else if ( lSTop >= max( 0, ( nLines - lpcd->szViewSize.cy ))) 
		lSTop = max( 0, ( nLines - lpcd->szViewSize.cy ));

	/*
	 *	Store new position.
	 */
	lpcd->ptViewPos.y = lSTop;

	/*
	 *	Compute the difference.
	 */
	lDiff -= lpcd->ptViewPos.y;

	/*
	 *	Scroll view.
	 */
	if ( lDiff != 0 )
		VScroll( lpcd, lDiff );

	/*
	 *	Update.
	 */
        SetupVScroller( lpcd );
	UpdateCaret( lpcd );
	return 0;
}

LRESULT OnHScroll( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
        LONG            lSTop, lDiff;

	/*
	 *	Hide the tooltip.
	 */
	HideTooltip( lpcd );

	/*
	 *	Save current position...
	 */
        lSTop = lDiff = lpcd->ptViewPos.x;

        switch ( LOWORD( wParam )) 
	{
                case    SB_PAGELEFT:
                        lSTop -= lpcd->szViewSize.cx;
                        break;

                case    SB_PAGERIGHT:
                        lSTop += lpcd->szViewSize.cy;
                        break;

                case    SB_LINELEFT:
                        lSTop--;
                        break;

                case    SB_LINERIGHT:
                        lSTop++;
                        break;

                case    SB_LEFT:
                        lSTop = 0;
                        break;

                case    SB_RIGHT:
                        lSTop = lpcd->nLongestLine - lpcd->szViewSize.cx;
                        break;

                case    SB_THUMBTRACK:
		{
			/*
			 *	Make sure we use a 32Bit position.
			 */
			SCROLLINFO	si;
			si.cbSize = sizeof( si );
			si.fMask = SIF_TRACKPOS;
			if ( GetScrollInfo( lpcd->hWnd, SB_HORZ, &si ))
				lSTop = si.nTrackPos;
			break;
		}
        }

        /*
	 *	Make sure the position stays in range.
	 */
        if ( lSTop <= 0 ) 
		lSTop = 0;
        else if ( lSTop >= max( 0, ( lpcd->nLongestLine - lpcd->szViewSize.cx ))) 
		lSTop = max( 0, ( lpcd->nLongestLine - lpcd->szViewSize.cx ));

	/*
	 *	Store new position.
	 */
	lpcd->ptViewPos.x = lSTop;

	/*
	 *	Compute the difference.
	 */
	lDiff -= lpcd->ptViewPos.x;

	/*
	 *	Scroll view.
	 */
	if ( lDiff != 0 )
		HScroll( lpcd, lDiff );

	/*
	 *	Update.
	 */
        SetupHScroller( lpcd );
	UpdateCaret( lpcd );

	return 0;
}
