/*
 *	create.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Window creation code.
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

/*
 *	Set a color.
 */
LRESULT OnSetColor( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	static DWORD dwFlags[] =
	{
		CF_DEFAULT_TEXT,
		CF_DEFAULT_BACKGROUND,
		CF_DEFAULT_BACKGROUND_RO,
		CF_DEFAULT_MARGIN,
		CF_DEFAULT_BOOKMARK,
		CF_DEFAULT_NUMBER,
		CF_DEFAULT_DELIMITER,
		CF_DEFAULT_SELECTED_TEXT,
		CF_DEFAULT_SELECTED_BKGND,
		CF_DEFAULT_LINE_NUMBERS,
		CF_DEFAULT_LINE_NUMBERS_BKGND,
		CF_DEFAULT_BACKGROUND_NUMBER,
		CF_DEFAULT_BACKGROUND_DELIMITER,
		CF_DEFAULT_BRACKET_MATCH,
		CF_DEFAULT_LINE_HIGHLIGHT,
		CF_DEFAULT_HYPERLINK,
		CF_DEFAULT_BACKGROUND_HYPERLINK
	};

	/*
	 *	Index in range?
	 */
	if ( wParam >= 0 && wParam < CARR_SIZEOF )
	{
		/*
		 *	Get it's current value.
		 */
		COLORREF crCurrent = Parser->crColors[ wParam ];

		/*
		 *	Setup color-flags.
		 */
		if ( lParam == CLR_DEFAULT ) Parser->dwColorFlags |= dwFlags[ wParam ];
		else			     Parser->dwColorFlags &= ~dwFlags[ wParam ];

		/*
		 *	Setup the new value and send a WM_SYSCOLORCHANGE which
		 *	will re-create the brushes etc.
		 */
		Parser->crColors[ wParam ] = lParam;
		SendMessage( hWnd, WM_SYSCOLORCHANGE, 0, 0 );
		return crCurrent;
	}
	return CLR_NONE;
}

/*
 *	Get a color.
 */
LRESULT OnGetColor( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Index in range?
	 */
	if ( wParam >= 0 && wParam < CARR_SIZEOF )
		/*
		 *	Return color value.
		 */
		return Parser->crColors[ wParam ];
	return CLR_NONE;
}

/*
 *	Make sure we are using the correct
 *	system colors at all times.
 */
LRESULT OnSysColorChange( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	HBRUSH		hBrush;
	HPEN		hPen;
	HBITMAP		hBitmap;
	COLORMAP	cMap;
	LPPARSER	lpp = Parser;

	/*
	 *	Reset bracket matches.
	 */
	lpcd->ptBracket1.x = lpcd->ptBracket1.y = -1;
	lpcd->ptBracket2.x = lpcd->ptBracket2.y = -1;
	if ( lpp->bColorBracketMatches ) SendCaretMessage( lpcd );

	/*
	 *	Update system dependant colors?
	 */
	if ( lpp->dwColorFlags & CF_DEFAULT_TEXT               ) lpp->crColors[ CARR_TEXT                    ] = GetSysColor( COLOR_WINDOWTEXT );
	if ( lpp->dwColorFlags & CF_DEFAULT_BACKGROUND         ) lpp->crColors[ CARR_BACKGROUND              ] = GetSysColor( COLOR_WINDOW );
	if ( lpp->dwColorFlags & CF_DEFAULT_BACKGROUND_RO      ) lpp->crColors[ CARR_BACKGROUND_READONLY     ] = GetSysColor( COLOR_3DLIGHT );
	if ( lpp->dwColorFlags & CF_DEFAULT_MARGIN             ) lpp->crColors[ CARR_SELECTION_MARGIN        ] = GetSysColor( COLOR_SCROLLBAR );
	if ( lpp->dwColorFlags & CF_DEFAULT_NUMBER             ) lpp->crColors[ CARR_NUMBER		     ] = GetSysColor( COLOR_WINDOWTEXT );
	if ( lpp->dwColorFlags & CF_DEFAULT_DELIMITER	       ) lpp->crColors[ CARR_DELIMITER		     ] = GetSysColor( COLOR_WINDOWTEXT );
	if ( lpp->dwColorFlags & CF_DEFAULT_SELECTED_TEXT      ) lpp->crColors[ CARR_SELECTED_TEXT	     ] = GetSysColor( COLOR_HIGHLIGHTTEXT );
	if ( lpp->dwColorFlags & CF_DEFAULT_SELECTED_BKGND     ) lpp->crColors[ CARR_SELECTED_BKGND	     ] = GetSysColor( COLOR_HIGHLIGHT );
	if ( lpp->dwColorFlags & CF_DEFAULT_LINE_NUMBERS_BKGND ) lpp->crColors[ CARR_BACKGROUND_LINE_NUMBERS ] = GetSysColor( COLOR_WINDOW );
	if ( lpp->dwColorFlags & CF_DEFAULT_BRACKET_MATCH      ) lpp->crColors[ CARR_BRACKET_MATCH	     ] = LightenColor( GetSysColor( COLOR_HIGHLIGHT ), 0.70, 0.70, 0.70 );
	if ( lpp->dwColorFlags & CF_DEFAULT_LINE_HIGHLIGHT     ) lpp->crColors[ CARR_LINE_HIGHLIGHT	     ] = DarkenColor( lpp->crColors[ CARR_BACKGROUND ], 0.10, 0.10, 0.10 );

	/*
	 *	Update pattern bitmaps.
	 */
	cMap.from = RGB( 192, 192, 192 );
	cMap.to   = lpp->crColors[ CARR_SELECTION_MARGIN ];
	if (( hBitmap = CreateMappedBitmap( hDLL, IDB_PATTERN, 0, &cMap, 1 )) != NULL )
	{
		DeleteObject( lpcd->hPattern );
		lpcd->hPattern = hBitmap;
	}
	cMap.from = RGB( 192, 192, 192 );
	cMap.to   = lpp->crColors[ CARR_LINE_NUMBERS ];
	if (( hBitmap = CreateMappedBitmap( hDLL, IDB_PATTERN, 0, &cMap, 1 )) != NULL )
	{
		DeleteObject( lpcd->hPatternLine );
		lpcd->hPatternLine = hBitmap;
	}

	/*
	 *	Update brushes.
	 */
	if (( hBrush = CreateSolidBrush( lpp->crColors[ CARR_BACKGROUND ] )) != NULL )
	{
		DeleteObject( lpcd->hbBackground );
		lpcd->hbBackground = hBrush;
	}

	if (( hBrush = CreateSolidBrush( lpp->crColors[ CARR_BACKGROUND_READONLY ] )) != NULL )
	{
		DeleteObject( lpcd->hbBackgroundRO );
		lpcd->hbBackgroundRO = hBrush;
	}

	if (( hBrush = CreatePatternBrush( lpcd->hPattern )) != NULL )
	{
		DeleteObject( lpcd->hbSelectionMargin );
		lpcd->hbSelectionMargin = hBrush;
	}

	if (( hBrush = CreateSolidBrush( lpp->crColors[ CARR_SELECTION_MARGIN ] )) != NULL )
	{
		DeleteObject( lpcd->hbSolidSelectionMargin );
		lpcd->hbSolidSelectionMargin = hBrush;
	}

	if (( hPen = CreatePen( PS_SOLID, 1, lpp->crColors[ CARR_SELECTION_MARGIN ] )) != NULL )
	{
		DeleteObject( lpcd->hpColumnMarker );
		lpcd->hpColumnMarker = hPen;
	}

	if (( hBrush = CreatePatternBrush( lpcd->hPatternLine )) != NULL )
	{
		DeleteObject( lpcd->hbLineMargin );
		lpcd->hbLineMargin = hBrush;
	}

	if (( hPen = CreatePen( PS_SOLID, 1, DarkenColor( lpp->crColors[ CARR_SELECTION_MARGIN ], 0.3, 0.3, 0.3 ))) != NULL )
	{
		DeleteObject( lpcd->hpMargin );
		lpcd->hpMargin = hPen;
	}

	if (( hBrush = CreateSolidBrush( lpp->crColors[ CARR_BOOKMARK ] )) != NULL )
	{
		DeleteObject( lpcd->hbBookmark );
		lpcd->hbBookmark = hBrush;
	}

	if (( hBrush = CreateSolidBrush( lpp->crColors[ CARR_SELECTED_BKGND ] )) != NULL )
	{
		DeleteObject( lpcd->hbSelBkgnd );
		lpcd->hbSelBkgnd = hBrush;
	}

	if (( hBrush = CreateSolidBrush( lpp->crColors[ CARR_BACKGROUND_LINE_NUMBERS ] )) != NULL )
	{
		DeleteObject( lpcd->hbLineMarginBkgnd );
		lpcd->hbLineMarginBkgnd = hBrush;
	}

	if (( hBrush = CreateSolidBrush( lpp->crColors[ CARR_LINE_HIGHLIGHT ] )) != NULL )
	{
		DeleteObject( lpcd->hbLineHighlight );
		lpcd->hbLineHighlight = hBrush;
	}

	/*
	 *	Re-render...
	 */
	InvalidateRect( lpcd->hWnd, NULL, TRUE );
	return 0;
}

LRESULT OnCreate( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpDummy )
{
	LPCREATESTRUCT		lpcs = ( LPCREATESTRUCT )lParam;
	LPCLASSDATA		lpcd;
	LPPARSER		lpp;
	COLORMAP		cMap;
	TOOLINFO		ti = { 0 };

	/*
	 *	Add window.
	 */
	if ( AddWindow( hWnd ) == FALSE )
		return -1;

	/*
	 *	Allocate class data.
	 */
	lpcd = malloc( sizeof( CLASSDATA ));
	if ( lpcd != NULL )
	{
		/*
		 *	Zero memory.
		 */
		memset( lpcd, 0, sizeof( CLASSDATA ));

		/*
		 *	Setup the data.
		 */
		SetWindowLong( hWnd, GWL_INSTANCEDATA, ( LONG )lpcd );

		/*
		 *	Initialize lists.
		 */
		NewList(( LPLIST )&lpcd->slSearchList );
		NewList(( LPLIST )&lpcd->slReplaceList );

		/*
		 *	Setup owner window handle.
		 */
		lpcd->hWnd = hWnd;

		/*
		 *	Create general memory pool.
		 */
		if (( lpcd->pMemPool = Brainchild_GetMemoryPool( 4096 )) == NULL )
			return -1;

		/*
		 *	Find the default parser.
		 */
		LockParserList();
		for ( lpp = plParsers.lpFirst; lpp->lpNext; lpp = lpp->lpNext )
		{
			/*
			 *	Default?
			 */
			if ( lpp->bDefault == TRUE )
			{
				Parser = lpp;
				break;
			}
		}
		UnlockParserList();

		/*
		 *	Parser found?
		 */
		if ( Parser == NULL )
			/*
			 *	Default is required.
			 */
			return -1;

		/*
		 *	Create cursors.
		 */
		if (( lpcd->hIBeam = LoadCursor( NULL, IDC_IBEAM )) == NULL )
			return -1;
		if (( lpcd->hArrow = LoadCursor( NULL, IDC_ARROW )) == NULL )
			return -1;
		if (( lpcd->hWait = LoadCursor( NULL, IDC_WAIT )) == NULL )
			return -1;
		if (( lpcd->hSelection = LoadCursor( hDLL, MAKEINTRESOURCE( IDR_SELECTION_POINTER ))) == NULL )
			return -1;
		if (( lpcd->hHand = LoadCursor( hDLL, MAKEINTRESOURCE( IDR_HYPERLINK ))) == NULL )
			return -1;

		/*
		 *	Create lines and undo/redo arrays.
		 */
		if (( lpcd->lpLines = Brainchild_ArrayCreate( 0, 4096, sizeof( LINE ))) == NULL )
			return -1;
		if (( lpcd->lpUndoRedo = Brainchild_ArrayCreate( 0, 1024, sizeof( UNDOREDO ))) == NULL )
			return -1;

		/*
		 *	Create a tooltip control.
		 */
		lpcd->hTooltip = CreateWindowEx( WS_EX_TOPMOST,
						 TOOLTIPS_CLASS,
						 NULL,
						 WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,		
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 CW_USEDEFAULT,
						 hWnd,
						 NULL,
						 hDLL,
						 NULL );
		if ( lpcd->hTooltip == NULL )
			return -1;

		/*
		 *	Add a tool.
		 */
		ti.cbSize   = sizeof( ti );
		ti.uFlags   = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE | TTF_TRANSPARENT;
		ti.hwnd     = hWnd;
		ti.hinst    = hDLL;
		ti.lpszText = NULL;
		ti.uId	    = ( UINT_PTR )hWnd;
		if ( SendMessage( lpcd->hTooltip, TTM_ADDTOOL, 0, ( LPARAM )&ti ) == FALSE )
			return -1;

		/*
		 *	Turns on the multi-line tooltips
		 *	if common controls 4.70 or better
		 *	is installed. 
		 */
		SendMessage( lpcd->hTooltip, TTM_SETMAXTIPWIDTH, 0, GetSystemMetrics( SM_CXSCREEN ) / 3 );


		/*
		 *	Setup default flags.
		 */
		lpcd->bCaretVisible	   = FALSE;
		lpcd->bHasFocus		   = FALSE;
		lpcd->bModified		   = FALSE;
		lpcd->bBreakExec	   = FALSE;
		lpcd->bRepeatMode	   = FALSE;
		lpcd->bSyntaxColoring	   = lpp->bGlobalSyntaxColoring;
		lpcd->bUnicode		   = FALSE;
		lpcd->bHoverTracking	   = FALSE;
		lpcd->bTipShowing	   = FALSE;

		/*
		 *	Setup pattern bitmaps.
		 */
		cMap.from = RGB( 192, 192, 192 );
		cMap.to   = lpp->crColors[ CARR_SELECTION_MARGIN ];
		lpcd->hPattern = CreateMappedBitmap( hDLL, IDB_PATTERN, 0, &cMap, 1 );
		cMap.from = RGB( 192, 192, 192 );
		cMap.to   = lpp->crColors[ CARR_LINE_NUMBERS ];
		lpcd->hPatternLine = CreateMappedBitmap( hDLL, IDB_PATTERN, 0, &cMap, 1 );

		/*
		 *	Setup brushes.
		 */
		lpcd->hbBackground	     = CreateSolidBrush( lpp->crColors[ CARR_BACKGROUND ] );
		lpcd->hbBackgroundRO	     = CreateSolidBrush( lpp->crColors[ CARR_BACKGROUND_READONLY ] );
		lpcd->hbSelectionMargin	     = CreatePatternBrush( lpcd->hPattern );
		lpcd->hbSolidSelectionMargin = CreateSolidBrush( lpp->crColors[ CARR_SELECTION_MARGIN ] );
		lpcd->hbBookmark	     = CreateSolidBrush( lpp->crColors[ CARR_BOOKMARK ] );
		lpcd->hbSelBkgnd             = CreateSolidBrush( lpp->crColors[ CARR_SELECTED_BKGND ] );
		lpcd->hbLineMargin           = CreatePatternBrush( lpcd->hPatternLine );
		lpcd->hbLineMarginBkgnd      = CreateSolidBrush( lpp->crColors[ CARR_BACKGROUND_LINE_NUMBERS ] );
		lpcd->hbLineHighlight	     = CreateSolidBrush( lpp->crColors[ CARR_LINE_HIGHLIGHT ] );

		/*
		 *	Setup the rest of the defaults.
		 */
		lpcd->ptCaretPos.x    = lpcd->ptCaretPos.y  = 0;
		lpcd->ptViewPos.x     = lpcd->ptViewPos.y   = 0;
		lpcd->szViewSize.cx   = lpcd->szViewSize.cy = 0;
		lpcd->ptSelStart.x    = lpcd->ptSelStart.y  = -1;
		lpcd->ptSelEnd.x      = lpcd->ptSelEnd.y    = -1;
		lpcd->ptStartPos.x    = lpcd->ptStartPos.y  = -1;
		lpcd->dwFileAttr      = FILE_ATTRIBUTE_ARCHIVE;
		lpcd->nUndoSize       = lpp->nUndoSize;
		lpcd->nBookmarkAnchor = -1;
		lpcd->nBookmarks      = 0;
		lpcd->dwParsedUpTo    = 0;
		lpcd->nMaxTrack	      = lpp->nMaxTrack;
		lpcd->nWheelDelta     = 0;
		lpcd->hFindWnd	      = NULL;
		lpcd->hReplaceWnd     = NULL;

		/*
		 *	No notification messages sent yet.
		 */
		memset(( void * )&lpcd->ptLastPositionSent, -1, sizeof( POINT ) + sizeof( NMSTATUSUPDATE ));
		
		/*
		 *	OK?
		 */
		if ( lpcd->hbBackground && lpcd->hbBackgroundRO && lpcd->hbSelectionMargin && lpcd->hbSolidSelectionMargin && lpcd->hbBookmark && lpcd->hPattern && lpcd->hbSelBkgnd && lpcd->hbLineMargin && lpcd->hPatternLine && lpcd->hbLineMarginBkgnd && lpcd-> hbLineHighlight )
		{
			/*
			 *	Need at least one empty line...
			 */
			if ( InsertLine( lpcd, NULL, 0, -1 ) == FALSE )
				return -1;
			
			/*
			 *	Setup filename.
			 */
			SetFileName( lpcd, lpcs->lpszName ? lpcs->lpszName : _T( "" ));

			/*
			 *	Send status message.
			 */
			SendStatusMessage( lpcd );

			/*
			 *	Setup OLE interfaces.
			 */
			SetupInterfaces( lpcd );
			return 0;
		}
	}
	return -1;
}

LRESULT OnDestroy( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Remove window.
	 */
	FreeWindow( hWnd );

	/*
	 *	Valid data?
	 */
	if ( lpcd )
	{
		/*
		 *	Destroy search/replace windows.
		 */
		if ( lpcd->hFindWnd ) DestroyWindow( lpcd->hFindWnd );
		if ( lpcd->hReplaceWnd ) DestroyWindow( lpcd->hReplaceWnd );

		/*
		 *	Delete file icons.
		 */
		if ( lpcd->hIcon      ) DestroyIcon( lpcd->hIcon );
		if ( lpcd->hIconLarge ) DestroyIcon( lpcd->hIconLarge );

		/*
		 *	Delete offscreen rendering
		 *	environment.
		 */
		if ( lpcd->hdcCompat ) DeleteDC( lpcd->hdcCompat );
		if ( lpcd->hbmCompat ) DeleteObject( lpcd->hbmCompat );

		/*
		 *	Release the drop target.
		 */
		if ( lpcd->bTargetRegistered )
			RevokeDragDrop( hWnd );

		/*
		 *	Free the coloring array.
		 */
		if ( lpcd->lpscColorArray ) Brainchild_FreePooled( lpcd->pMemPool, lpcd->lpscColorArray );

		/*
		 *	Free string lists.
		 */
		FreeStringNodes( lpcd, &lpcd->slSearchList );
		FreeStringNodes( lpcd, &lpcd->slReplaceList );

		/*
		 *	Free memory pool.
		 */
		if ( lpcd->pMemPool ) Brainchild_FreeMemoryPool( lpcd->pMemPool, TRUE );
		
		/*
		 *	Destroy cursors.
		 */
		/*if ( lpcd->hIBeam     ) DestroyCursor( lpcd->hIBeam );*/
		/*if ( lpcd->hArrow     ) DestroyCursor( lpcd->hArrow );*/
		/*if ( lpcd->hWait      ) DestroyCursor( lpcd->hWait  );*/
		if ( lpcd->hSelection ) DestroyCursor( lpcd->hSelection );
		if ( lpcd->hHand )	DestroyCursor( lpcd->hHand );

		/*
		 *	Free bitmaps.
		 */
		if ( lpcd->hPattern     ) DeleteObject( lpcd->hPattern );
		if ( lpcd->hPatternLine ) DeleteObject( lpcd->hPatternLine );

		/*
		 *	Destroy the tooltip.
		 */
		if ( lpcd->hTooltip ) DestroyWindow( lpcd->hTooltip );

		/*
		 *	Free the arrays.
		 */
		if ( lpcd->lpLines    ) Brainchild_ArrayDelete( lpcd->lpLines );
		if ( lpcd->lpUndoRedo ) Brainchild_ArrayDelete( lpcd->lpUndoRedo );

		/*
		 *	Destroy brushes.
		 */
		if ( lpcd->hbBookmark             ) DeleteObject( lpcd->hbBookmark );
		if ( lpcd->hbSelectionMargin      ) DeleteObject( lpcd->hbSelectionMargin );
		if ( lpcd->hbSolidSelectionMargin ) DeleteObject( lpcd->hbSolidSelectionMargin );
		if ( lpcd->hbBackgroundRO         ) DeleteObject( lpcd->hbBackgroundRO );
		if ( lpcd->hbBackground	          ) DeleteObject( lpcd->hbBackground );
		if ( lpcd->hbSelBkgnd	          ) DeleteObject( lpcd->hbSelBkgnd );
		if ( lpcd->hbLineMarginBkgnd      ) DeleteObject( lpcd->hbLineMarginBkgnd );
		if ( lpcd->hbLineHighlight	  ) DeleteObject( lpcd->hbLineHighlight );

		/*
		 *	Destroy pens.
		 */
		if ( lpcd->hpMargin	  ) DeleteObject( lpcd->hpMargin );
		if ( lpcd->hpColumnMarker ) DeleteObject( lpcd->hpColumnMarker );

		/*
		 *	Delete the buffer.
		 */
		if ( lpcd->pszBuffer ) free( lpcd->pszBuffer );

		/*
		 *	Free the class data.
		 */
		free( lpcd );

		/*
		 *	Clear the data pointer. This prevents the message
		 *	handlers being called after the control has been
		 *	destroyed.
		 */
		SetWindowLong( hWnd, GWL_INSTANCEDATA, 0L );
	}
	return 0;
}
