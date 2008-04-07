/*
 *	hyperlink.c
 *
 *	(C) Copyright 2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	Possible hyperlinks. Should probably be made
 *	configurable but for now this will do.
 */
static struct 
{
	TCHAR	*pszURL;
	int	 nLength;
} 
Hyper[] = 
{ 
	{ _T("www."),	4 },
	{ _T("http:"),	5 },
	{ _T("file:"),	5 },
	{ _T("mailto:"),	7 },
	{ _T("ftp:"),	4 },
	{ _T("https:"),	6 },
	{ _T("gopher:"),	7 },
	{ _T("nntp:"),	5 },
	{ _T("telnet:"),	7 },
	{ _T("news:"),	5 },
	{ NULL,		0 }
};

/*
 *	Check if the given position is over
 *	a hyperlink. Also sets up the start
 *	and end points if a hyperlink is
 *	found.
 */
static BOOL CheckForHyperlink( LPCLASSDATA lpcd, LPPOINT lpPos, LPPOINT lpStart, LPPOINT lpEnd, BOOL bQuoted )
{
	LPLINE	lpLine = ( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, lpPos->y );
	int	nIndex = lpPos->x, i = 0;

	/*
	 *	Any text and, if so, any text on the
	 *	line?
	 */
	if ( AnyText( lpcd ) == FALSE || lpLine->pcText == NULL )
		return FALSE;

	/*
	 *	Are we in the text of the line?
	 */
	if ( nIndex >= lpLine->nLength )
		return FALSE;

	/*
	 *	Is the given position on a white space and
	 *	are we doing a non-quoted search?
	 */
	if ( ! bQuoted && _istspace( lpLine->pcText[ nIndex ] ))
	{
		/*
		 *	Try a quoted search now.
		 */
		return CheckForHyperlink( lpcd, lpPos, lpStart, lpEnd, TRUE );
	}

	/*
	 *	Store line numbers.
	 */
	lpStart->y = lpEnd->y = lpPos->y;

	/*
	 *	Search for a quoted hyperlink?
	 */
	if ( ! bQuoted )
	{
		/*
		 *	Find the first white space or the 
		 *	beginning of the line.
		 */
		while ( nIndex > 0 && ! _istspace( lpLine->pcText[ nIndex ] ))
			nIndex--;
	}
	else
	{
		/*
		 *	Find the first double quote or the 
		 *	beginning of the line.
		 */
		while ( nIndex > 0 && lpLine->pcText[ nIndex ] != _T( '"' ))
			nIndex--;
	}

	/*
	 *	If were looking for a quoted hyperlink and we
	 *	are not on a double quote we stop here.
	 */
	if ( bQuoted && lpLine->pcText[ nIndex ] != _T( '"' ))
		return FALSE;
	else if ( _istspace( lpLine->pcText[ nIndex ] ))
		nIndex++;

	/*
	 *	Store the start column.
	 */
	lpStart->x = nIndex;

	/*
	 *	Double quoted?
	 */
	if ( lpLine->pcText[ nIndex ] == _T( '"' ))
	{
		/*
		 *	Find the next double quote or the end
		 *	of the line.
		 */
		nIndex++;
		while ( nIndex < lpLine->nLength && lpLine->pcText[ nIndex ] != _T( '"' ))
			nIndex++;

		/*
		 *	Skip the double quote.
		 */
		lpStart->x++;
	}
	else
	{
		/*
		 *	Find the next white space or the end
		 *	of the line.
		 */
		while ( nIndex < lpLine->nLength && ! _istspace( lpLine->pcText[ nIndex ] ))
			nIndex++;
	}

	/*
	 *	Store the end column.
	 */
	lpEnd->x = nIndex - 1;

	/*
	 *	Is it a hyperlink?
	 */
	while ( Hyper[ i ].pszURL )
	{
		/*
		 *	Check if this hyperlink fits on the line
		 *	from this position.
		 */
		if ( lpLine->nLength - lpStart->x >= Hyper[ i ].nLength )
		{
			/*
			 *	Is it this hyperlink?
			 */
			if ( ! _tcsnicmp( &lpLine->pcText[ lpStart->x ], Hyper[ i ].pszURL, Hyper[ i ].nLength )) 
			{
				/*
				 *	If the control key is pressed we show a
				 *	hand cursors. If not we show the arrow
				 *	cursor.
				 */
				if ( GetAsyncKeyState( VK_CONTROL ) & ( 1 << 15 )) SetCursor( lpcd->hHand );
				else SetCursor( lpcd->hIBeam );
				return TRUE;
			}
		}
		i++;
	}

	/*
	 *	If we did not find a hyperlink and we are
	 *	not doing a quoted search we start one now.
	 */
	if ( ! bQuoted ) 
		return CheckForHyperlink( lpcd, lpPos, lpStart, lpEnd, TRUE );
	return FALSE;
}

/*
 *	Check is the mouse position is located
 *	on a hyperlink.
 */
BOOL MouseOnHyperLink( LPCLASSDATA lpcd )
{
	POINT		ptMousePos;

	/*
	 *	Are we parsing hyperlinks?
	 */
	if ( Parser->bParseHyperLinks )
	{
		/*
		*	Get mouse position and convert
		*	to client coordinates.
		*/
		GetCursorPos( &ptMousePos );
		ScreenToClient( lpcd->hWnd, &ptMousePos );

		/*
		*	Skip selection margin.
		*/
		ptMousePos.x -= ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ));
		return PointOnHyperlink( lpcd, ptMousePos.x, ptMousePos.y );
	}
	return FALSE;
}

/*
 *	Check is the given position is located
 *	on a hyperlink.
 */
BOOL PointOnHyperlink( LPCLASSDATA lpcd, int x, int y )
{
	POINT ptPos, ptDummy1, ptDummy2;

	/*
	 *	Are we parsing hyperlinks?
	 */
	if ( Parser->bParseHyperLinks )
	{
		/*
		 *	Translate the point to the
		 *	text buffer position.
		 */
		if ( MouseToCaret( lpcd, x, y, &ptPos ))
			/*
			 *	Check if this position is located
			 *	on a hyperlink.
			 */
			return CheckForHyperlink( lpcd, &ptPos, &ptDummy1, &ptDummy2, FALSE );
	}
	return FALSE;
}

/*
 *	Obtain the hyperlink located at the
 *	mouse position. If there is no hyperlink
 *	at the mouse position return NULL.
 */
TCHAR *GetHyperlink( LPCLASSDATA lpcd )
{
	POINT ptStart, ptEnd, ptMousePos;

	/*
	 *	Are we parsing hyperlinks?
	 */
	if ( Parser->bParseHyperLinks )
	{
		/*
		 *	Get mouse position and convert
		 *	to client coordinates.
		 */
		GetCursorPos( &ptMousePos );
		ScreenToClient( lpcd->hWnd, &ptMousePos );

		/*
		 *	Skip selection margin.
		 */
		ptMousePos.x -= ( GetMarginWidth( lpcd ) + GetLineMarginWidth( lpcd ));

		/*
		 *	Convert the coordinates to the character
		 *	position.
		 */
		if ( MouseToCaret( lpcd, ptMousePos.x, ptMousePos.y, &ptMousePos ))
		{
			/*
			 *	Check if the character is located inside
			 *	a hyperlink.
			 */
			if ( CheckForHyperlink( lpcd, &ptMousePos, &ptStart, &ptEnd, FALSE ))
			{
				/*
				 *	Allocate memory to store the
				 *	hyperlink text.
				 */
				TCHAR *pszUrl = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( ptEnd.x - ptStart.x + 2 ));
				if ( pszUrl )
				{
					/*
					 *	Copy the hyperlink text into the
				 	 *	allocated buffer.
					 */
					LPLINE lpLine = ( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, ptStart.y );
					memcpy( pszUrl, &lpLine->pcText[ ptStart.x ], ptEnd.x - ptStart.x + 1 );

					/*
					 *	0-terminate to be on the safe side.
					 */
					pszUrl[ ptEnd.x - ptStart.x + 1 ] = 0;
					return pszUrl;
				}
			}
		}
	}
	/*
	 *	No hyperlink or memory failure...
	 */
	return NULL;
}

/*
 *	See if the mouse is located on a hyperlink and,
 *	if so, run it.
 */
BOOL RunHyperlink( LPCLASSDATA lpcd )
{
	/*
	 *	Get the hyperlink at the current mouse
	 *	position.
	 */
	TCHAR *pszLink = GetHyperlink( lpcd );
	if ( pszLink )
	{
		/*
		 *	Open the hyperlink.
		 */
		if (( int )ShellExecute( GetDesktopWindow(), 
				_T( "open" ), 
				pszLink, 
				NULL, 
				NULL, 
				SW_SHOWNORMAL ) <= 32 )
			OkIOError( lpcd, lpcd->hWnd, _T( "%s" ), pszLink );

		/*
		 *	Release the url buffer.
		 */
		Brainchild_FreePooled( lpcd->pMemPool, pszLink );
		return TRUE;
	}
	/*
	 *	No hyperlink or memory failure...
	 */
	return FALSE;
}
