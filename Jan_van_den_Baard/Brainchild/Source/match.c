/*
 *	match.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

/*
 *	Table of matchable characters.
 */
static TCHAR cBrackets[][ 2 ] =
{
	{ _T( '(' ), _T( ')' )  },
	{ _T( '[' ), _T( ']' )  },
	{ _T( '{' ), _T( '}' )  },
	{ _T( '<' ), _T( '>' )  },
	{ _T( '`' ), _T( '\'' ) },
	{ _T( '«' ), _T( '»' )  },
	{ 0,   0    }
};

BOOL MatchBracketSelect( LPARAM lParam, LPCLASSDATA lpcd )
{
	POINT ptStart;

	/*
	 *	Setup the start marker.
	 */
	ptStart = lpcd->ptCaretPos;

	/*
	 *	Match the bracket.
	 */
	if ( MatchBracket( lParam, lpcd ) == FALSE )
		return FALSE;

	/*
	 *	Setup starting position.
	 */
	lpcd->ptStartPos = ptStart;

	/*
	 *	Where are we?
	 */
	DisplayCaret( lpcd, FALSE );
	if ( lpcd->ptCaretPos.y < lpcd->ptStartPos.y || lpcd->ptCaretPos.y == lpcd->ptStartPos.y && lpcd->ptCaretPos.x < lpcd->ptStartPos.x )
	{
		/*
		 *	Setup new start position.
		 */
		lpcd->ptSelStart = lpcd->ptCaretPos;
		lpcd->ptSelEnd   = lpcd->ptStartPos;
		lpcd->ptSelEnd.x++;
	}
	else
	{
		CaretRight( lParam, lpcd );
		lpcd->ptSelStart = lpcd->ptStartPos;
		lpcd->ptSelEnd   = lpcd->ptCaretPos;
	}

	/*
	 *	Make sure the caret is visible.
	 */
	MakeCaretVisible( lpcd );
	DisplayCaret( lpcd, TRUE );

	/*
	 *	Update visuals.
	 */
	RenderLines( lpcd, lpcd->ptSelStart.y, lpcd->ptSelEnd.y );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );
	return TRUE;
}

BOOL MatchBracket( LPARAM lParam, LPCLASSDATA lpcd )
{
	// Look for a bracket match.
	POINT pt1, pt2;
	switch ( FindMatches( lpcd, &pt1, &pt2, FALSE ))
	{
		case	-1:
			/*
			 *	Not matchable...
			 */
			if ( lpcd->bSilentMode == FALSE )
				OkWarning( lpcd, NULL, GetString( IDS_NOT_MATCHABLE ));
			return FALSE;

		case	-2:
			/*
			 *	No match found...
			 */
			if ( lpcd->bSilentMode == FALSE )
				OkWarning( lpcd, NULL, GetString( IDS_NO_MATCH_FOUND ));

			/*
			 *	We need to go right.
			 */
			lpcd->ptCaretPos.x++;
			return FALSE;
	}

	/*
	 *	Clear markers.
	 */
	ClearMark( 0, lpcd );

	/*
	 *	Silent mode?
	 */
	if ( lpcd->bSilentMode == FALSE )
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Move to the match location.
		 */
		lpcd->ptCaretPos = pt2;

		/*
		 *	Make sure the caret is visible?
		 */
		MakeCaretVisible( lpcd );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
	return TRUE;
}

int FindMatches( LPCLASSDATA lpcd, LPPOINT lpPoint1, LPPOINT lpPoint2, BOOL bFindInner )
{
	TCHAR	cBracket = 0, cBracket1 = 0, cBracket2 = 0, cToMatch = 0, *pcPtr;
	LPLINE	lpLine = GETLINE( lpcd );
	int	nDir = 0, i, nColumn = 0, nCnt = 1;
	int	nLine = lpcd->ptCaretPos.y;
	BOOL	bHasMatch = FALSE;

	/*
	 *	Reset points.
	 */
	lpPoint1->x = lpPoint1->y = -1;
	lpPoint2->x = lpPoint2->y = -1;

	/*
	 *	Is there a line?
	 */
	if ( lpLine == NULL ) return 0;

	/*
	 *	Get character at
	 *	the caret location.
	 */
	if ( lpLine->pcText && lpLine->nLength )
	{
		/*
		 *	Get the character to match.
		 */
		cBracket1 = lpLine->pcText[ lpcd->ptCaretPos.x ];
		if ( bFindInner == TRUE && lpcd->ptCaretPos.x > 0 ) 
			cBracket2 = lpLine->pcText[ lpcd->ptCaretPos.x - 1 ];

		/*
		 *	Find out the bracket we
		 *	must match.
		 */
		for ( i = 0; cBrackets[ i ][ 0 ]; i++ )
		{
			/*
			 *	Opening bracket?
			 */
			if (( bFindInner == FALSE && cBrackets[ i ][ 0 ] == cBracket1 ) ||
			    ( bFindInner == TRUE  && cBrackets[ i ][ 0 ] == cBracket2 ))
			{
				/*
				 *	Look forwards for the 
				 *	closing bracket.
				 */
				nDir = 1;
				cToMatch = cBrackets[ i ][ 1 ];
				if ( bFindInner ) cBracket = cBracket2;
				else		  cBracket = cBracket1;
				break;
			}
			else if ( cBrackets[ i ][ 1 ] == cBracket1 )
			{
				/*
				 *	Look backwards for the
				 *	opening bracket.
				 */
				nDir = -1;
				cToMatch = cBrackets[ i ][ 0 ];
				cBracket = cBracket1;
				break;
			}
		}
	}

	/*
	 *	Was it a matchable character?
	 */
	if ( cToMatch == 0 )
		return -1;

	/*
	 *	If we are looking for a match of a bracket
	 *	on the left-hand we move the caret left.
	 */
	if ( nDir == 1 && bFindInner == TRUE )
		lpcd->ptCaretPos.x--;

	/*
	 *	Store the position.
	 */
	*lpPoint1 = lpcd->ptCaretPos;

	for ( ;; )
	{
		/*
		 *	Get starting column.
		 */
		if ( nDir > 0 )
		{
			/*
			 *	Get the line we are on.
			 */
			lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );

			/*
			 *	Are we at the end?
			 */
			if ( lpcd->ptCaretPos.x == lpLine->nLength )
				/*
				 *	Can't be a matchable character.
				 */
				break;
			/*
			 *	Start one place further than
			 *	where we are at.
			 */
			nColumn = lpcd->ptCaretPos.x + 1;
		}
		else
		{
			/*
			 *	Are we at the start of
			 *	the line?
			 */
			if ( lpcd->ptCaretPos.x == 0 )
			{
				/*
				 *	If this is the top line we
				 *	can stop right here.
				 */
				if ( nLine == 0 )
					break;

				/*
				 *	Go up one line and move to
				 *	it's end.
				 */
				nLine--;
				lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );
				nColumn = lpLine->nLength;
			}
			else
				/*
				 *	Go left one place.
				 */
				nColumn = lpcd->ptCaretPos.x - 1;
		}

		/*
		 *	Search forwards?
		 */
		if ( nDir > 0 )
		{
			/*
			 *	Look for the match.
			 */
			while ( nLine < Brainchild_ArrayGetSize( lpcd->lpLines ) && bHasMatch == FALSE )
			{
				/*
				 *	Get a pointer to the line.
				 */
				lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );

				/*
				 *	Any text on this line?
				 */
				if ( lpLine->pcText && lpLine->nLength )
				{
					/*
					 *	Get a pointer to the
					 *	current position.
					 */
					pcPtr = &lpLine->pcText[ nColumn ];

					/*
					 *	Iterate line.
					 */
					while ( nColumn <= lpLine->nLength )
					{
						/*
						 *	Increase counter if we find
						 *	the bracket we are matching.
						 */
						if ( *pcPtr == cBracket ) 
							nCnt++;
						else if ( *pcPtr == cToMatch )
						{
							/*
							 *	Decrease counter.
							 */
							nCnt--;
							
							/*
							 *	If we have the match
							 *	mark it and break the
							 *	loop.
							 */
							if ( !nCnt )
							{
								bHasMatch = TRUE;
								break;
							}
						}

						/*
						 *	Next character.
						 */
						nColumn++;
						pcPtr++;
					}
				}
				/*
				 *	Do we have a match?
				 */
				if ( ! bHasMatch )
				{
					/*
					 *	No. Go on to the next line.
					 */
					nLine++;
					nColumn = 0;
				}
			}
		}
		else
		{
			/*
			 *	Look for the match.
			 */
			while ( nLine >= 0 && bHasMatch == FALSE )
			{
				/*
				 *	Get a pointer to the line we are on.
				 */
				lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, nLine );

				/*
				 *	Any text on this line?
				 */
				if ( lpLine->pcText && lpLine->nLength )
				{
					/*
					 *	Get a pointer to the 
					 *	current position.
					 */
					pcPtr = &lpLine->pcText[ nColumn ];
					
					/*
					 *	Iterate line.
					 */
					while ( nColumn >= 0 )
					{
						/*
						 *	Increase counter if we find
						 *	the bracket we are matching.
						 */
						if ( *pcPtr == cBracket )
							nCnt++;
						else if ( *pcPtr == cToMatch )
						{
							/*
							 *	Decrease the counter if
							 *	we found the bracket to match.
							 */
							nCnt--;

							/*
							 *	If we have the match
							 *	mark it and break the
							 *	loop.
							 */
							if ( ! nCnt )
							{
								bHasMatch = TRUE;
								break;
							}
						}
						/*
						 *	Previous...
						 */
						nColumn--;
						pcPtr--;
					}
				}

				/*
				 *	Do we have a match?
				 */
				if ( ! bHasMatch )
				{
					/*
					 *	Previous line...
					 */
					nLine--;
					if ( nLine >= 0 )
						nColumn = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, nLine ))->nLength;
				}
			}
		}
		break;
	}

	/*
	 *	If we moved the caret because we did a
	 *	left-hand character search we put the
	 *	caret back at it's original location
	 *	here.
	 */
	if ( nDir == 1 && bFindInner == TRUE )
		lpcd->ptCaretPos.x++;

	/*
	 *	Found match?
	 */
	if (  bHasMatch == FALSE )
		return -2;

	/*
	 *	Store the match location.
	 */
	lpPoint2->x = nColumn;
	lpPoint2->y = nLine;
	return 0;
}
