/*
 *	indent.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Smart indenting stuff.
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	Compute smart indention. Smart indention only
 *	works when automatic indention is turned on aswell.
 */
static int IndentDirection( LPCLASSDATA lpcd )
{
	LPLINE		lpLine = GETLINE( lpcd );
	LPSMART		lpSmart;
	TCHAR	       *pszPtr;

	/*
	 *	Smart indention on?
	 */
	if ( Parser->bSmartIndent )
	{
		/*
		 *	Any text?
		 */
		if (( pszPtr = lpLine->pcText ) == 0 )
			return -1;

		/*
		 *	Skip the blanks...
		 */
		while ( _istspace( *pszPtr )) pszPtr++;

		/*
		 *	Real text?
		 */
		if ( *pszPtr )
		{
			/*
			 *	Iterate indention nodes.
			 */
			for ( lpSmart = Parser->lSmartIndention.lpFirst; lpSmart->lpNext; lpSmart = lpSmart->lpNext )
			{
				/*
				 *	Always case sensitive.
				 */
				if ( memcmp( lpSmart->lpszSmartString, pszPtr, _tcslen( lpSmart->lpszSmartString )) == 0 )
					/*
					 *	Return the direction.
					 */
					return lpSmart->nDirection;
			}
		}
	}
	return -1;
}


/*
 *	Copy the leading blanks of the
 *	current line so we can perform
 *	automatic and smart indention.
 */
TCHAR *CopyLeadingBlanks( LPCLASSDATA lpcd, int *lpnCaretX )
{
	LPLINE		lpLine = GETLINE( lpcd );
	TCHAR	       *pcBlanks = NULL;
	int		nBlanks = 0, nDir, i, nOnScreen, nTabs, nSpaces;

	/*
	 *	Count the blanks.
	 */
	if ( lpLine->pcText )
	{
		/*
		 *	Count the number of blanks.
		 */
		while ( _istspace( lpLine->pcText[ nBlanks ] ) && nBlanks < lpLine->nLength ) nBlanks++;

		/*
		 *	Convert to the column position.
		 */
		nOnScreen = GetCaretOffset( lpcd, min( nBlanks, lpcd->ptCaretPos.x ));

		/*
		 *	How many tabs/spaces are required?
		 */
		nTabs   = nOnScreen / Parser->nTabSize;
		nSpaces = nOnScreen % Parser->nTabSize;

		/*
		 *	Smart indention?
		 */
		nDir = IndentDirection( lpcd );

		/*
		 *	Right?
		 */
		if ( nDir == INDENT_RIGHT )
			/*
			 *	One extra tab required.
			 */
			nTabs++;
		else if ( nDir == INDENT_LEFT )
		{
			/*
			 *	One less tab if there are any tabs.
			 *	Otherwise reduce the number of tabs
			 *	by as much needed or possible.
			 */
			if ( nTabs ) nTabs--;
			else nSpaces -= min( Parser->nTabSize, nSpaces );
		}

		/*
		 *	Allocate the string.
		 */
		if ( nTabs + nSpaces )
		{
			/*
			 *	Allocate...
			 */
			if (( pcBlanks = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( nTabs + nSpaces ) + 1 )) != NULL )
			{
				/*
				 *	Insert tabs.
				 */
				for ( i = 0; i < nTabs; i++ ) pcBlanks[ i ] = _T( '\t' );

				/*
				 *	And the spaces.
				 */
				for ( i = nTabs; i < nTabs + nSpaces; i++ ) pcBlanks[ i ] = _T( ' ' );

				/*
				 *	0-terminate.
				 */
				pcBlanks[ i ] = _T( '\0' );

				/*
				 *	Store desired caret position.
				 */
				*lpnCaretX = nTabs + nSpaces;
			}
		}
	}
	else
		*lpnCaretX = 0;

	return pcBlanks;
}

/*
 *	(Un)Indent text selection.
 */
static void TabIndent( LPCLASSDATA lpcd, BOOL bIndent )
{
	LPLINE		lpLine;
	POINT		ptDummy;
	BOOL		bGroup = TRUE;
	int		i;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Indent the lines?
	 */
	if ( bIndent )
	{
		/*
		 *	Yes. Add a tab at the start of the lines.
		 */
		for ( i = lpcd->ptSelStart.y; i <= lpcd->ptSelEnd.y; i++ )
		{
			/*
			 *	Anything selected on the last line?
			 */
			if ( i == lpcd->ptSelEnd.y && lpcd->ptSelEnd.x == 0 )
				break;

			/*
			 *	Add the tab.
			 */
			InsertText( lpcd, i, 0, _T( "\t" ), &ptDummy, bGroup );

			/*
			 *	No group start anymore.
			 */
			bGroup = FALSE;
		}
	}
	else
	{
		/*
		 *	Itterate the lines.
		 */
		for ( i = lpcd->ptSelStart.y; i <= lpcd->ptSelEnd.y; i++ )
		{
			POINT	ptFrom, ptTo;

			/*
			 *	Anything selected on the last line?
			 */
			if ( i == lpcd->ptSelEnd.y && lpcd->ptSelEnd.x == 0 )
				break;

			/*
			 *	Get the line.
			 */
			lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, i );

			/*
			 *	Create positions.
			 */
			ptFrom.x = 0;
			ptFrom.y = i;
			ptTo.x	 = 1;
			ptTo.y	 = i;

			/*
			 *	Any text?
			 */
			if ( lpLine->pcText )
			{
				/*
				 *	Is there a tab at the start of the line?
				 */
				if ( lpLine->pcText[ 0 ] == _T( '\t' ))
				{
					/*
					 *	Yes. Remove it.
					 */
					DeleteText( lpcd, &ptFrom, &ptTo, bGroup );

					/*
					 *	No group start anymore.
					 */
					bGroup = FALSE;
				}
				else if ( lpLine->pcText[ 0 ] == _T( ' ' ))
				{
					int nSpaces;

					/*
					 *	Count the number of spaces
					 *	to remove.
					 */
					for ( nSpaces = 0; nSpaces < Parser->nTabSize; nSpaces++ )
					{
						/*
						 *	Is the current character a tab
						 *	or is it a non-blank.
						 */
						if ( lpLine->pcText[ nSpaces ] == _T( '\t' ) || ( ! _istspace( lpLine->pcText[ nSpaces ] )))
							/*
							 *	Stop here.
							 */
							break;
					}

					/*
					 *	Adjust the point upto which we
					 *	remove the blanks.
					 */
					ptTo.x = nSpaces;

					/*
					 *	Remove the blanks.
					 */
					DeleteText( lpcd, &ptFrom, &ptTo, bGroup );

					/*
					 *	No group start anymore.
					 */
					bGroup = FALSE;
				}
			}
		}
	}

	/*
	 *	Get the last line.
	 */
	lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelEnd.y );

	/*
	 *	Make sure the selection covers whole lines...
	 */
	if ( lpcd->ptSelStart.x ) lpcd->ptSelStart.x = 0;
	if ( lpcd->ptSelEnd.x > 0 && lpcd->ptSelEnd.x < lpLine->nLength ) lpcd->ptSelEnd.x = lpLine->nLength;

	/*
	 *	Move the caret to the end of
	 *	the selection.
	 */
	lpcd->ptCaretPos = lpcd->ptSelEnd;

	/*
	 *	Update column position.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Is the caret inside
	 *	the view?
	 */
	if ( CaretInView( lpcd ) == FALSE )
		/*
		 *	No. Move the view to
		 *	make it visible.
		 */
		MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );

	/*
	 *	Setup scrollers.
	 */
	SetupHScroller( lpcd );
	SetupVScroller( lpcd );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
}

void DoTab( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Do we have a valid selection?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Indent the selection.
		 */
		TabIndent( lpcd, TRUE );
		return;
	}

	/*
	 *	Simple insert a tab.
	 */
	InsertChar( lpcd, _T( '\t' ));
}

void DoBackTab( LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Do we have a valid selection?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Indent the selection.
		 */
		TabIndent( lpcd, FALSE );
		return;
	}

	/*
	 *	Move the caret back a tab.
	 */
	CaretTabBack( lParam, lpcd );
}
