/*
 *	search.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

void DestroyReplaceDialog( HWND );
void DestroyFindDialog( HWND );

static LPSTRING StringPresent( LPSTRINGLIST lpList, LPCTSTR lpszString )
{
	LPSTRING	lps;

	/*
	 *	Iterate strings.
	 */
	for ( lps = lpList->lpFirst; lps->lpNext; lps = lps->lpNext )
	{
		/*
		 *	The same?
		 */
		if ( ! _tcsicmp( lps->lpszString, lpszString ))
			return lps;
	}
	return NULL;
}

LPSTRING AddStringNode( LPCLASSDATA lpcd, LPSTRINGLIST lpList, LPCTSTR lpszString, BOOL bTail )
{
	LPSTRING	lps, lpr;

	/*
	 *	Do nothing if the string is already present.
	 */
	if (( lps = StringPresent( lpList, lpszString )) != NULL )
	{
		/*
		 *	Move to the top of the list...
		 */
		Remove(( LPNODE )lps );
		AddHead(( LPLIST )lpList, ( LPNODE )lps );
		return lps;
	}

	/*
	 *	Allocate node?
	 */
	if (( lps = Brainchild_AllocPooled( lpcd->pMemPool, sizeof( STRING ))) != NULL )
	{
		/*
		 *	Copy the string.
		 */
		if (( lps->lpszString = CopyString( lpcd, lpszString )) != NULL )
		{
			/*
			 *	Reached the limit?
			 */
			if ( lpList->nNumNodes >= lpcd->nMaxTrack )
			{
				/*
				 *	Remove the last node.
				 */
				if (( lpr = ( LPSTRING )RemTail(( LPLIST )lpList )) != NULL )
				{
					/*
					 *	Free it's string.
					 */
					if ( lpr->lpszString )
						Brainchild_FreePooled( lpcd->pMemPool, ( LPVOID )lpr->lpszString );

					/*
					 *	And itself.
					 */
					Brainchild_FreePooled( lpcd->pMemPool, lpr );

					/*
					 *	One less.
					 */
					lpList->nNumNodes--;
				}
			}

			/*
			 *	Add it...
			 */
			if ( ! bTail ) AddHead(( LPLIST )lpList, ( LPNODE )lps );
			else	       AddTail(( LPLIST )lpList, ( LPNODE )lps );

			/*
			 *	One more...
			 */
			lpList->nNumNodes++;
		}
		else
		{
			/*
			 *	Free the node.
			 */
			Brainchild_FreePooled( lpcd->pMemPool, lps );
			lps = NULL;
		}
	}
	return lps;
}

void FreeStringNodes( LPCLASSDATA lpcd, LPSTRINGLIST lpList )
{
	LPSTRING	lps;

	/*
	 *	Remove nodes and free them.
	 */
	while (( lps = ( LPSTRING )RemHead(( LPLIST )lpList )) != NULL )
	{
		/*
		 *	Free the string.
		 */
		if ( lps->lpszString )
			Brainchild_FreePooled( lpcd->pMemPool, ( LPVOID )lps->lpszString );

		/*
		 *	And the node.
		 */
		Brainchild_FreePooled( lpcd->pMemPool, lps );
	}

	/*
	 *	Reset counter...
	 */
	lpList->nNumNodes = 0;
}

void SetSearchString( LPCLASSDATA lpcd, LPCTSTR lpszString )
{
	LPSTRING	lps;
	LPCTSTR		lpszPtr;
	int		nLength, i;

	/*
	 *	Add the string.
	 */
	if (( lps = AddStringNode( lpcd, &lpcd->slSearchList, lpszString, FALSE )) != NULL )
	{
		/*
		 *	Setup Boyer/Moore structure.
		 */
		lpcd->bmSkipTable.lpszSearchString = lps->lpszString;
		lpcd->bmSkipTable.nLength = nLength = _tcslen( lps->lpszString );

		/*
		 *	Preset skip table values.
		 */
		for ( i = 0; i <= UCHAR_MAX; i++ )
		{
			lpcd->bmSkipTable.anDeltas[ i ] = nLength;
			lpcd->bmSkipTable.anBackDeltas[ i ] = nLength;
		}

		/*
		 *	Setup skip values for
		 *	forward searches.
		 */
		for ( lpszPtr = lps->lpszString, i = nLength; --i > 0; )
		{
			lpcd->bmSkipTable.anDeltas[ ( UCHAR )_totlower( *lpszPtr ) ] = i;
			lpszPtr++;
		}

		/*
		 *	Setup skip values for
		 *	backward searches.
		 */
		for ( lpszPtr = lps->lpszString + ( nLength - 1 ), i = nLength; --i > 0; )
		{
			lpcd->bmSkipTable.anBackDeltas[ ( UCHAR )_totlower( *lpszPtr ) ] = i;
			lpszPtr--;
		}
	}
}

int FindForward( LPCLASSDATA lpcd, TCHAR *pcText, int nLength )
{
	TCHAR		*pcEndString, *pcTextPtr, *pcString = ( TCHAR * )lpcd->bmSkipTable.lpszSearchString;
	int		 nIndex, nSkip, nStrLen = lpcd->bmSkipTable.nLength - 1;
	int		*lpTable = &lpcd->bmSkipTable.anDeltas[ 0 ];

	/*
	 *	Setup data.
	 */
	pcEndString = ( TCHAR * )lpcd->bmSkipTable.lpszSearchString + nStrLen;
	pcTextPtr   = pcText + nStrLen;
	nIndex	    = nLength - nStrLen;

	/*
	 *	Enough text for a match?
	 */
	if ( nIndex >= 0 )
	{
		/*
		 *	Case-sensitive?
		 */
		if ( Parser->bSearchCase )
		{
			/*
			 *	Search...
			 */
			for ( ;; )
			{
				/*
				 *	Found string?
				 */
				if (( *pcEndString == *pcTextPtr ) && ( ! _tcsncmp( pcString, pcTextPtr - nStrLen, nStrLen )))
					/*
					 *	Yes. Return the offset.
					 */
					return ( int )(( pcTextPtr - pcText ) - nStrLen );

				/*
				 *	Find out how many characters
				 *	we can skip.
				 */
				nSkip = lpTable[ ( UCHAR )_totlower( *pcTextPtr ) ];

				/*
				 *	Enough text left?
				 */
				if ( nIndex < nSkip )
					break;

				/*
				 *	Skip the characters.
				 */
				nIndex -= nSkip;
				pcTextPtr += nSkip;
			}
		}
		else
		{
			/*
			 *	Search...
			 */
			for ( ;; )
			{
				/*
				 *	Found string?
				 */
				if (( _totlower( *pcEndString ) == _totlower( *pcTextPtr )) && ( ! _tcsnicmp( pcString, pcTextPtr - nStrLen, nStrLen )))
					/*
					 *	Yes. Return the offset.
					 */
					return ( int )(( pcTextPtr - pcText ) - nStrLen );
					
				/*
				 *	Find out how many characters
				 *	we can skip.
				 */
				nSkip = lpTable[ ( UCHAR )_totlower( *pcTextPtr ) ];

				/*
				 *	Enough text left?
				 */
				if ( nIndex < nSkip )
					break;

				/*
				 *	Skip the characters.
				 */
				nIndex -= nSkip;
				pcTextPtr += nSkip;
			}
		}
	}
	return -1;
}

/*
 *	Boyer Moore search backward.
 *
 *	This is basically the same as a forward
 *	search only the search is done in the
 *	other direction and the string is compared left
 *	to right instead of right to left with a forward
 *	search.
 */
int FindBackward( LPCLASSDATA lpcd, TCHAR *pcText, int nLength )
{
	TCHAR			*pcString, *pcTextPtr;
	int			 nIndex, nSkip, nStrLen = lpcd->bmSkipTable.nLength - 1;
	int			*lpTable = &lpcd->bmSkipTable.anBackDeltas[ 0 ];

	pcString  = ( TCHAR * )lpcd->bmSkipTable.lpszSearchString;
	pcTextPtr = pcText;
	nIndex    = nLength;

	if ( nIndex >= nStrLen )
	{
		if ( Parser->bSearchCase ) 
		{
			for ( ;; ) 
			{
				if (( *pcString == *pcTextPtr ) && ( ! _tcsncmp( pcString + 1, pcTextPtr + 1, nStrLen )))
					return ( int )( pcText - pcTextPtr );
				nSkip = lpTable[ ( UCHAR )_totlower( *pcTextPtr ) ];
				if ( nIndex < nSkip )
					break;
				nIndex -= nSkip;
				pcTextPtr -= nSkip;
			}
		} 
		else 
		{
			for ( ;; ) 
			{
				if (( _totlower( *pcString ) == _totlower( *pcTextPtr )) && ( ! _tcsnicmp( pcString + 1, pcTextPtr + 1, nStrLen )))
					return ( int )( pcText - pcTextPtr );
				nSkip = lpTable[ ( UCHAR )_totlower( *pcTextPtr ) ];
				if ( nIndex < nSkip ) 
					break;
				nIndex -= nSkip;
				pcTextPtr -= nSkip;
			}
		}
	}
	return -1;
}

static BOOL IsWordOnly( LPCLASSDATA lpcd, LPLINE lpLine, LPPOINT lpPos )
{
	TCHAR		tc;

	/*
	 *	Start of the line?
	 */
	if ( lpPos->x == 0 )
	{
		/*
		 *	Is the line the same length as
		 *	the search string?
		 */
		if ( lpLine->nLength == lpcd->bmSkipTable.nLength )
			return TRUE;

		/*
		 *	Is the character after the
		 *	found word a space or a delimiter?
		 */
		tc = lpLine->pcText[ lpPos->x + lpcd->bmSkipTable.nLength ];
		if ( _istspace( tc ) || IsDelimiter( lpcd, tc ))
			return TRUE;

		return FALSE;
	}
	else
	{
		/*
		 *	Is the character before the
		 *	found word a space or a delimiter?
		 */
		tc = lpLine->pcText[ lpPos->x - 1 ];
		if ( ! _istspace( tc ) && ! IsDelimiter( lpcd, tc ))
			return FALSE;
		
		/*
		 *	Found string reaches
		 *	the end of the line?
		 */
		if ( lpPos->x + lpcd->bmSkipTable.nLength == lpLine->nLength )
			return TRUE;

		/*
		 *	Is the character after the
		 *	found word a space or a delimiter?
		 */
		tc = lpLine->pcText[ lpPos->x + lpcd->bmSkipTable.nLength ];
		if ( _istspace( tc ) || IsDelimiter( lpcd, tc ))
			return TRUE;

		return FALSE;
	}
}

static BOOL CheckSelection( LPCLASSDATA lpcd )
{
	/*
	 *	Do we have a valid selection?
	 */
	if ( HasMark( lpcd ) && ( lpcd->ptSelStart.y == lpcd->ptSelEnd.y ) && (( lpcd->ptSelEnd.x - lpcd->ptSelStart.x ) == lpcd->bmSkipTable.nLength ))
	{
		/*
		 *	Get the line on which the
		 *	selection is located.
		 */
		LPLINE	lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelStart.y );

		/*
		 *	Does the contents match the
		 *	search string?
		 */
		if ( ! ( Parser->bSearchCase ? _tcsncmp : _tcsnicmp )( &lpLine->pcText[ lpcd->ptSelStart.x ], lpcd->bmSkipTable.lpszSearchString, lpcd->bmSkipTable.nLength ))
			return TRUE;
	}
	return FALSE;
}

static BOOL FindString( LPCLASSDATA lpcd, HWND hDlg, int nDirection, BOOL bQuiet, BOOL bSkipMark )
{
	LPLINE		lpLine = GETLINE( lpcd );
	TCHAR	       *pcText;
	POINT		ptSearch = lpcd->ptCaretPos;
	BOOL		bFound = FALSE;
	int		nStrLen = lpcd->bmSkipTable.nLength, nFoundAt;
	int		nLines = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;

	/*
	 *	Valid?
	 */
	if ( lpcd->bmSkipTable.lpszSearchString == NULL )
	{
		/*
		 *	Put up a dialog.
		 */
		if ( ! bQuiet )
			OkInformation( lpcd, hDlg, GetString( IDS_INVALID_STRING ));
		return FALSE;
	}

	/*
	 *	Forward?
	 */
	if ( nDirection > 0 )
	{
		/*
		 *	Enough text left to get a match?
		 */
		if ( lpLine->nLength - ptSearch.x < nStrLen )
		{
			/*
			 *	Are we on the last line?
			 */
			if ( ptSearch.y < nLines )
			{
				/*
				 *	Go down one line.
				 */
				ptSearch.y++;
				ptSearch.x = 0;
			}
		}

		/*
		 *	Search...
		 */
		while ( ptSearch.y <= nLines && bFound == FALSE )
		{
			/*
			 *	Get line pointer.
			 */
			lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, ptSearch.y );

			/*
			 *	Valid text?
			 */
			if ( lpLine->pcText && ( lpLine->nLength - ptSearch.x ) >= nStrLen )
			{
				/*
				 *	Get text pointer.
				 */
				pcText = &lpLine->pcText[ ptSearch.x ];
	
				/*
				 *	Find...
				 */
				if (( nFoundAt = FindForward( lpcd, pcText, lpLine->nLength - ptSearch.x )) != -1 )
				{
					/*
					 *	Adjust column.
					 */
					ptSearch.x += nFoundAt;

					/*
					 *	Word-only?
					 */
					if ( Parser->bWholeWords )
					{
						/*
						 *	See if it is a whole word.
						 */
						if (( bFound = IsWordOnly( lpcd, lpLine, &ptSearch )) == FALSE )
						{
							/*
							 *	Go to the next one.
							 */
							if ( ptSearch.x < lpLine->nLength )
								/*
								 *	Next column...
								 */
								ptSearch.x++;
							else
							{
								/*
								 *	Are we on the last line?
								 */
								if ( ptSearch.y == nLines )
									break;

								/*
								 *	Next line...
								 */
								ptSearch.y++;
								ptSearch.x = 0;
							}
						}
					}
					else
						bFound = TRUE;
					continue;
				}
			}
			/*
			 *	Next...
			 */
			ptSearch.y++;
			ptSearch.x = 0;
		}
	}
	else
	{
		/*
		 *	Enough bytes left of the caret
		 *	to get a match?
		 */
		if ( ptSearch.x < nStrLen )
		{
			/*
			 *	Are we at the beginning of the file?
			 */
			if ( ptSearch.y > 0 )
			{
				/*
				 *	Go up a line.
				 */
				ptSearch.y--;
				ptSearch.x = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, ptSearch.y ))->nLength;
			}
		}
		else
			ptSearch.x--;

		/*
		 *	Get line pointer.
		 */
		lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, ptSearch.y );

		/*
		 *	Search...
		 */
		while ( ptSearch.y >= 0 && bFound == FALSE )
		{
			/*
			 *	Valid text?
			 */
			if ( lpLine->pcText )
			{
				/*
				 *	Get text pointer.
				 */
				pcText = &lpLine->pcText[ ptSearch.x ];

				/*
				 *	Find...
				 */
				if (( nFoundAt = FindBackward( lpcd, pcText, ptSearch.x )) != -1 )
				{
					/*
					 *	Adjust column.
					 */
					ptSearch.x -= nFoundAt;

					/*
					 *	Word-only?
					 */
					if ( Parser->bWholeWords )
					{
						/*
						 *	Is it a whole word?
						 */
						if (( bFound = IsWordOnly( lpcd, lpLine, &ptSearch )) == FALSE )
						{
							/*
							 *	Go to the next one.
							 */
							if ( ptSearch.x > 0 )
								/*
								 *	Previous column...
								 */
								ptSearch.x--;
							else
							{
								/*
								 *	Are we on the first line?
								 */
								if ( ptSearch.y == 0 )
									break;


								/*
								 *	Previous line...
								 */
								ptSearch.y--;
								lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, ptSearch.y );
								ptSearch.x = lpLine->nLength;
							}
						}
					}
					else
						bFound = TRUE;
					continue;
				}
			}

			/*
			 *	Are we done?
			 */
			if ( ptSearch.y == 0 )
				break;

			/*
			 *	Previous...
			 */
			ptSearch.y--;
			lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, ptSearch.y );
			ptSearch.x = lpLine->nLength;
		}
	}

	/*
	 *	Found?
	 */
	if ( bFound == FALSE )
	{
		/*
		 *	Pop up a dialog.
		 */
		if ( ! bQuiet )
			OkInformation( lpcd, hDlg, GetString( IDS_STRING_NOT_FOUND ), lpcd->bmSkipTable.lpszSearchString );
		return FALSE;
	}

	/*
	 *	Clear current marker.
	 */
	if ( ! bSkipMark ) 
		ClearMark( lpcd );

	/*
	 *	Move caret.
	 */
	lpcd->ptCaretPos = ptSearch;

	/*
	 *	Move to the end of the
	 *	search if we searched
	 *	forwards.
	 */
	if ( nDirection > 0 ) 
		lpcd->ptCaretPos.x += nStrLen;

	/*
	 *	Mark the column.
	 */
	lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

	/*
	 *	Setup new selection.
	 */
	if ( ! bSkipMark )
	{
		lpcd->ptSelStart.y = lpcd->ptSelEnd.y = ptSearch.y;
		lpcd->ptSelStart.x = ptSearch.x;
		lpcd->ptSelEnd.x   = ptSearch.x + nStrLen;
	}

	/*
	 *	Quiet mode?
	 */
	if ( ! bQuiet )
	{
		/*
		 *	Make the caret visible or
		 *	re-render the line.
		 */
		if ( CaretInView( lpcd ) == FALSE )
			MakeCaretVisible( lpcd );
		else
			RenderLine( lpcd, ptSearch.y );
	}

	return TRUE;
}

void FindNext( LPCLASSDATA lpcd )
{
	/*
	 *	Save current caret position.
	 */
	POINT	ptCaretPos = lpcd->ptCaretPos;

	/*
	 *	Do we have a search string?
	 */
	if ( lpcd->bmSkipTable.lpszSearchString == NULL )
	{
		/*
		 *	Any strings in the buffer?
		 */
		if ( lpcd->slSearchList.lpFirst->lpNext )
			/*
			 *	Set it up.
			 */
			SetSearchString( lpcd, lpcd->slSearchList.lpFirst->lpszString );
		else
		{
			/*
			 *	Beep speaker and return.
			 */
			MessageBeep( 0xFFFFFFFF );
			return;
		}
	}

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Do we have a selection in which
	 *	the search string is located?
	 */
	if ( CheckSelection( lpcd ))
		/*
		 *	Yes. Make sure the caret is
		 *	positioned after the
		 *	current selection.
		 */
		lpcd->ptCaretPos = lpcd->ptSelEnd;

	/*
	 *	Find it.
	 */
	if ( FindString( lpcd, NULL, 1, FALSE, FALSE ) == FALSE )
		/*
		 *	Reset the caret.
		 */
		lpcd->ptCaretPos = ptCaretPos;

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
	SendCaretMessage( lpcd );
}

void FindPrevious( LPCLASSDATA lpcd )
{
	/*
	 *	Save current caret position.
	 */
	POINT	ptCaretPos = lpcd->ptCaretPos;

	/*
	 *	Do we have a search string?
	 */
	if ( lpcd->bmSkipTable.lpszSearchString == NULL )
	{
		/*
		 *	Beep speaker and return.
		 */
		MessageBeep( 0xFFFFFFFF );
		return;
	}

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Do we have a selection in which
	 *	the search string is located?
	 */
	if ( CheckSelection( lpcd ))
		/*
		 *	Yes. Make sure the caret is
		 *	positioned before the
		 *	current selection.
		 */
		lpcd->ptCaretPos = lpcd->ptSelStart;

	/*
	 *	Find it.
	 */
	if ( FindString( lpcd, NULL, -1, FALSE, FALSE ) == FALSE )
		/*
		 *	Reset the caret.
		 */
		lpcd->ptCaretPos = ptCaretPos;

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
	SendCaretMessage( lpcd );
}

static void AddReplaceString( LPCLASSDATA lpcd, HWND hCombo )
{
	LPCTSTR		lpszString;
	LPSTRING	lps;	
	int		nTextLength;

	/*
	 *	Get text length.
	 */
	if (( nTextLength = SendMessage( hCombo, WM_GETTEXTLENGTH, 0, 0 )) > 0 )
	{
		/*
		 *	Allocate buffer.
		 */
		if (( lpszString = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( nTextLength + 1 ))) != NULL )
		{
			/*
			 *	Get the string.
			 */
			SendMessage( hCombo, WM_GETTEXT, nTextLength + 1, ( LPARAM )lpszString );

			/*
			 *	Set it up.
			 */
			if (( lps = AddStringNode( lpcd, &lpcd->slReplaceList, lpszString, FALSE )) != NULL )
			{
				/*
				 *	Setup the string.
				 */
				lpcd->lpszReplaceText = lps->lpszString;

				/*
				 *	Clear the combo box contents.
				 */
				SendMessage( hCombo, CB_RESETCONTENT, 0, 0 );

				/*
				 *	Add current string list
				 *	contents.
				 */
				for ( lps = lpcd->slReplaceList.lpFirst; lps->lpNext; lps = lps->lpNext )
					SendMessage( hCombo, CB_INSERTSTRING, ( WPARAM )-1, ( LPARAM )lps->lpszString );

				/*
				 *	Select the first one.
				 */
				SendMessage( hCombo, CB_SETCURSEL, 0, 0 );
			}
			else
				lpcd->lpszReplaceText = NULL;

			/*
			 *	Release the buffer.
			 */
			Brainchild_FreePooled( lpcd->pMemPool, ( LPVOID )lpszString );
			return;
		}
	}
	lpcd->lpszReplaceText = NULL;
}

static void AddSearchString( LPCLASSDATA lpcd, HWND hCombo )
{
	LPCTSTR		lpszString;
	LPSTRING	lps;	
	int		nTextLength;

	/*
	 *	Get text length.
	 */
	if (( nTextLength = SendMessage( hCombo, WM_GETTEXTLENGTH, 0, 0 )) > 0 )
	{
		/*
		 *	Allocate buffer.
		 */
		if (( lpszString = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( nTextLength + 1 ))) != NULL )
		{
			/*
			 *	Get the string.
			 */
			SendMessage( hCombo, WM_GETTEXT, nTextLength + 1, ( LPARAM )lpszString );

			/*
			 *	Set it up.
			 */
			SetSearchString( lpcd, lpszString );

			/*
			 *	Clear the combo box contents.
			 */
			SendMessage( hCombo, CB_RESETCONTENT, 0, 0 );

			/*
			 *	Add current string list
			 *	contents.
			 */
			for ( lps = lpcd->slSearchList.lpFirst; lps->lpNext; lps = lps->lpNext )
				SendMessage( hCombo, CB_INSERTSTRING, ( WPARAM )-1, ( LPARAM )lps->lpszString );

			/*
			 *	Select the first one.
			 */
			SendMessage( hCombo, CB_SETCURSEL, 0, 0 );

			/*
			 *	Release the buffer.
			 */
			Brainchild_FreePooled( lpcd->pMemPool, ( LPVOID )lpszString );
			return;
		}
	}

	/*
	 *	Failure...
	 */
	lpcd->bmSkipTable.lpszSearchString = NULL;
}

static BOOL AddWord( LPCLASSDATA lpcd )
{
	POINT		ptStart, ptEnd;
	LPLINE		lpLine = GETLINE( lpcd );
	LPTSTR		lpszString;
	int		nTextLength;

	/*
	 *	Is we have a selection which is 
	 *	equal to the current search pattern
	 *	we do not need to add the word.
	 */
	if ( HasMark( lpcd ) && lpcd->bmSkipTable.lpszSearchString )
	{
		/*
		 *	Length of the selection.
		 */
		nTextLength = lpcd->ptSelEnd.x - lpcd->ptSelStart.x;

		/*
		 *	Do they match?
		 */
		if (( Parser->bSearchCase ? _tcsncmp : _tcsnicmp )( &lpLine->pcText[ lpcd->ptSelStart.x ],
								    lpcd->bmSkipTable.lpszSearchString,
								    nTextLength ) == 0 )
			return TRUE;
	}

	/*
	 *	Get word positions.
	 */
	if ( GetWordPoints( lpcd, &ptStart, &ptEnd ))
	{
		/*
		 *	Compute word length.
		 */
		nTextLength = ptEnd.x - ptStart.x + 1;

		/*
		 *	Allocate buffer.
		 */
		if (( lpszString = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( nTextLength + 1 ))) != NULL )
		{
			/*
			 *	Get the string.
			 */
			_tcsncpy( lpszString, &lpLine->pcText[ ptStart.x ], nTextLength - 1 );

			/*
			 *	Set it up.
			 */
			LoadSearchReplaceStrings( lpcd, &lpcd->slSearchList, FALSE );
			SetSearchString( lpcd, lpszString );
			SaveSearchReplaceStrings( lpcd, &lpcd->slSearchList, FALSE );

			/*
			 *	Release the buffer.
			 */
			Brainchild_FreePooled( lpcd->pMemPool, ( LPVOID )lpszString );
			return TRUE;
		}
	}
	return FALSE;
}

void FindNextWord( LPCLASSDATA lpcd )
{
	/*
	 *	Find the next occurence of the word
	 *	under the caret.
	 */
	if ( AddWord( lpcd ))
		FindNext( lpcd );
}

void FindPrevWord( LPCLASSDATA lpcd )
{
	/*
	 *	Find the previous occurence of the word
	 *	under the caret.
	 */
	if ( AddWord( lpcd ))
		FindPrevious( lpcd );
}

void DestroyFindDialog( HWND hDlg )
{
	/*
	 *	Get the data pointer.
	 */
	LPCLASSDATA lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );
	if ( lpcd )
	{
		/*
		 *	Save list contents to the registry.
		 */
		SaveSearchReplaceStrings( lpcd, &lpcd->slSearchList, FALSE );

		/*
		 *	Destroy the window.
		 */
		DestroyWindow( hDlg );
		lpcd->hFindWnd = NULL;
	}
}

static BOOL CALLBACK FindProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_INITDIALOG:
		{
			LPCLASSDATA	lpcd = ( LPCLASSDATA )lParam;
			HWND		hCombo = GetDlgItem( hDlg, IDC_FIND_LIST );
			POINT		ptStart = { -1, -1 }, ptEnd = { -1, -1 };
			LPLINE		lpLine;
			LPSTRING	lps;

			/*
			 *	Setup class data pointer as
			 *	the dialog user data.
			 */
			SetWindowLong( hDlg, DWL_USER, ( LONG )lpcd );

			/*
			 *	Load search strings from the registry.
			 */
			LoadSearchReplaceStrings( lpcd, &lpcd->slSearchList, FALSE );

			/*
			 *	Add search strings...
			 */
			for ( lps = lpcd->slSearchList.lpFirst; lps->lpNext; lps = lps->lpNext )
				SendMessage( hCombo, CB_INSERTSTRING, ( WPARAM )-1, ( LPARAM )lps->lpszString );
			
			/*
			 *	Setup checkboxes.
			 */
			CheckDlgButton( hDlg, IDC_FIND_MATCHCASE,  Parser->bSearchCase ? BST_CHECKED : BST_UNCHECKED );
			CheckDlgButton( hDlg, IDC_FIND_WHOLEWORDS, Parser->bWholeWords ? BST_CHECKED : BST_UNCHECKED );

			/*
			 *	Is we have a selection on one line we use it as
			 *	the search string.
			 */
			if ( HasMark( lpcd ) && lpcd->ptSelStart.y == lpcd->ptSelEnd.y )
			{
				/*
				 *	Copy the points.
				 */
				lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelStart.y );
				ptStart = lpcd->ptSelStart;
				ptEnd   = lpcd->ptSelEnd;
			}
			else
			{
				/*
				 *	Use the word under the caret.
				 */
				lpLine = GETLINE( lpcd );
				GetWordPoints( lpcd, &ptStart, &ptEnd );
			}

			/*
			 *	Valid points?
			 */
			if ( ptStart.x != -1 && ptStart.y != -1 && ptEnd.x != -1 && ptEnd.y != -1 )
			{
				TCHAR *pcTemp = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( ptEnd.x - ptStart.x + 1 ));

				/*
				 *	Allocation ok?
				 */
				if ( pcTemp )
				{
					/*
					 *	Copy the word.
					 */
					memcpy( pcTemp, &lpLine->pcText[ ptStart.x ], REAL_SIZE( ptEnd.x - ptStart.x ));

					/*
					 *	Set it up.
					 */
					SetWindowText( hCombo, pcTemp );
					AddSearchString( lpcd, hCombo );

					/*
					 *	Free the string.
					 */
					Brainchild_FreePooled( lpcd->pMemPool, pcTemp );
				}
			}
			else
			{
				/*
				 *	Any in the list?
				 */
				if ( lpcd->slSearchList.lpFirst->lpNext )
				{
					/*
					 *	Set it up.
					 */
					SetWindowText( hCombo, lpcd->slSearchList.lpFirst->lpszString );
					AddSearchString( lpcd, hCombo );
				}
			}

			/*
			 *	Setup the first string.
			 */
			SendMessage( hCombo, CB_SETCURSEL, 0, 0 );

			/*
			 *	Setup GUI.
			 */
			{
				int nLength = GetWindowTextLength( hCombo );
				EnableWindow( GetDlgItem( hDlg, IDC_FIND_NEXT     ), ( BOOL )nLength );
				EnableWindow( GetDlgItem( hDlg, IDC_FIND_PREVIOUS ), ( BOOL )nLength );
				EnableWindow( GetDlgItem( hDlg, IDC_FIND_BOOKMARK ), ( BOOL )nLength );
				SendMessage( hDlg, DM_SETDEFID, nLength ? IDC_FIND_NEXT : IDCANCEL, 0 );
			}

			/*
			 *	Focus the edit control.
			 */
			return TRUE;
		}

		case WM_ENABLE:
			// Make sure our parent inherits our state.
			EnableWindow( GetParent( hDlg ), wParam );
			break;

		case WM_ACTIVATE:
			/*
			 *	Are we being deactivated?
			 */
			if ( LOWORD( wParam ) == WA_INACTIVE )
			{
				/*
				 *	If the window being activated is not our
				 *	parent we hide ourselves unless we are
				 *	disabled.
				 */
				if (( HWND )lParam != GetParent( hDlg ) && IsWindowEnabled( hDlg ))
					ShowWindow( hDlg, SW_HIDE );
			}
			break;

		case	WM_CLOSE:
			/*
			 *	Bye...
			 */
			DestroyFindDialog( hDlg );
			break;

		case	WM_COMMAND:
		{
			/*
			 *	Get data.
			 */
			LPCLASSDATA	lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );
			HWND		hCombo = GetDlgItem( hDlg, IDC_FIND_LIST );

			switch ( LOWORD( wParam ))
			{
				case	IDC_FIND_LIST:
					/*
					 *	Focus killed?
					 */
					if ( HIWORD( wParam ) == CBN_KILLFOCUS )
						AddSearchString( lpcd, hCombo );
					else if ( HIWORD( wParam ) == CBN_EDITCHANGE )
					{
						/*
						 *	Setup control buttons.
						 */
						int nLength = GetWindowTextLength( hCombo );
						EnableWindow( GetDlgItem( hDlg, IDC_FIND_NEXT     ), ( BOOL )nLength );
						EnableWindow( GetDlgItem( hDlg, IDC_FIND_PREVIOUS ), ( BOOL )nLength );
						EnableWindow( GetDlgItem( hDlg, IDC_FIND_BOOKMARK ), ( BOOL )nLength );
						SendMessage( hDlg, DM_SETDEFID, nLength ? IDC_FIND_NEXT : IDCANCEL, 0 );
					}
					break;

				case	IDC_FIND_MATCHCASE:
					/*
					 *	Get match case boolean.
					 */
					Parser->bSearchCase = IsDlgButtonChecked( hDlg, IDC_FIND_MATCHCASE ) == BST_CHECKED ? TRUE : FALSE;
					break;

				case	IDC_FIND_WHOLEWORDS:
					/*
					 *	Get whole words boolean.
					 */
					Parser->bWholeWords = IsDlgButtonChecked( hDlg, IDC_FIND_WHOLEWORDS ) == BST_CHECKED ? TRUE : FALSE;
					break;

				case	IDC_FIND_BOOKMARK:
				{
					/*
					 *	Store current caret position.
					 */
					POINT	ptPosition = lpcd->ptCaretPos;
					BOOL	bFound = FALSE;

					/*
					 *	Go to the first line.
					 */
					lpcd->ptCaretPos.x = lpcd->ptCaretPos.y = 0;

					/*
					 *	Setup edit control contents.
					 */
					AddSearchString( lpcd, hCombo );

					/*
					 *	Bookmark lines.
					 */
					while ( FindString( lpcd, hDlg, 1, TRUE, TRUE ))
					{
						/*
						 *	Get a pointer to the line.
						 */
						LPLINE lpLine = GETLINE( lpcd );

						/*
						 *	Bookmark it.
						 */
						lpLine->dwFlags |= LNF_BOOKMARK;

						/*
						 *	Increase counter.
						 */
						lpcd->nBookmarks++;

						/*
						 *	Found one.
						 */
						bFound = TRUE;

						/*
						 *	Is it on the view?
						 */
						if ( lpcd->ptCaretPos.y >= lpcd->ptViewPos.y && lpcd->ptCaretPos.y < lpcd->ptViewPos.y + lpcd->szViewSize.cy )
							/*
							 *	Render the line.
							 */
							RenderLine( lpcd, lpcd->ptCaretPos.y );

						/*
						 *	Put the caret at the end of the
						 *	current line.
						 */
						lpcd->ptCaretPos.x = lpLine->nLength;
					}

					/*
					 *	Send a status message when
					 *	there were bookmarks set.
					 */
					if ( bFound )
						SendStatusMessage( lpcd );

					/*
					 *	Clear markers.
					 */
					ClearMark( lpcd );

					/*
					 *	Put back the caret.
					 */
					lpcd->ptCaretPos = ptPosition;
					break;
				}

				case	IDC_FIND_NEXT:
				case	IDC_FIND_PREVIOUS:
				{
					/*
					 *	Save caret position.
					 */
					POINT	ptCaretPos = lpcd->ptCaretPos;

					/*
					 *	Setup edit control contents.
					 */
					AddSearchString( lpcd, hCombo );
					
					/*
					 *	Do we have a selection which
					 *	holds the entire search string?
					 */
					if ( CheckSelection( lpcd ))
					{
						/*
						 *	Put us at the end of the selection
						 *	when we find the next occurence and
						 *	at the start if we find the previous
						 *	occurence.
						 */
						if ( LOWORD( wParam ) == IDC_FIND_NEXT ) lpcd->ptCaretPos = lpcd->ptSelEnd;
						else					 lpcd->ptCaretPos = lpcd->ptSelStart;
					}

					/*
					 *	Find it forwards.
					 */
					if ( FindString( lpcd, hDlg, LOWORD( wParam ) == IDC_FIND_NEXT ? 1 : -1, FALSE, FALSE ) == FALSE )
						/*
						 *	Reset caret.
						 */
						 lpcd->ptCaretPos = ptCaretPos;
					SendCaretMessage( lpcd );
					break;
				}

				case	IDCANCEL:
					/*
					 *	Bye...
					 */
					DestroyFindDialog( hDlg );
					break;
			}
			break;
		}

	}
	return FALSE;
}

void FindDialog( LPCLASSDATA lpcd )
{
	RECT rcReplace = { 0 };
	BOOL bPositionAtReplace = FALSE;

	/*
	 *	Clear qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Is the replace dialog open?
	 */
	if ( lpcd->hReplaceWnd )
	{
		/*
		 *	Note it's position.
		 */
		GetWindowRect( lpcd->hReplaceWnd, &rcReplace );
		bPositionAtReplace = TRUE;
	}

	/*
	 *	Create the dialog.
	 */
	lpcd->hFindWnd = CreateDialogParam( hDLL, MAKEINTRESOURCE( IDD_FIND ), lpcd->hWnd, &FindProc, ( LPARAM )lpcd );
	if ( lpcd->hFindWnd )
	{
		/*
		 *	Do we need to position it at the replace window?
		 */
		if ( bPositionAtReplace ) 
			/*
			 *	Yes. Do so and destroy the replace window.
			 */
			SetWindowPos( lpcd->hFindWnd, NULL, rcReplace.left, rcReplace.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

		/*
		 *	Show the dialog.
		 */
		ShowWindow( lpcd->hFindWnd, SW_SHOW );

		/*
		 *	Destroy the replace window (if open).
		 */
		if ( lpcd->hReplaceWnd )
			DestroyReplaceDialog( lpcd->hReplaceWnd );
	}
}

LRESULT OnFindDialog( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Popup the find dialog.
	 */
	FindDialog( lpcd );
	return 0;
}		

void Replace( LPCLASSDATA lpcd, HWND hParent )
{
	/*
	 *	Read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Valid selection?
	 */
	if ( CheckSelection( lpcd ))
	{
		POINT	ptStart = lpcd->ptSelStart, ptEnd = lpcd->ptSelEnd;

		/*
		 *	Clear marker.
		 */
		ClearMark( lpcd );

		/*
		 *	Delete the current selection.
		 */
		if ( DeleteText( lpcd, &ptStart, &ptEnd, TRUE ) == FALSE )
			return;

		/*
		 *	Move the caret to the selection start.
		 */
		lpcd->ptCaretPos = ptStart;

		/*
		 *	Insert the replacement text.
		 */
		if ( lpcd->lpszReplaceText )
			InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, lpcd->lpszReplaceText, &lpcd->ptCaretPos, FALSE );

		/*
		 *	Update column position.
		 */
		lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

		/*
		 *	Caret still inside the view?
		 */
		if ( CaretInView( lpcd ))
			/*
			 *	Yes, simply re-render the line.
			 */
			RenderLine( lpcd, lpcd->ptCaretPos.y );
		else
			/*
			 *	Move the view.
			 */
			MakeCaretVisible( lpcd );

		/*
		 *	We are modified.
		 */
		SetModified( lpcd, TRUE );
	}

	/*
	 *	Find the string.
	 */
	FindString( lpcd, hParent, 1, FALSE, FALSE );
	SendCaretMessage( lpcd );
}

void ReplaceAll( LPCLASSDATA lpcd, HWND hParent, BOOL bSelection )
{
	POINT	ptCaretPos = lpcd->ptCaretPos;
	POINT	ptStart, ptEnd;
	BOOL	bDidReplace = FALSE;
	int	nCounter = 0;

	/*
	 *	Read-only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Selection?
	 */
	if ( bSelection )
	{
		/*
		 *	Get start and end position
		 *	of the selection
		 */
		ptStart = lpcd->ptSelStart;
		ptEnd   = lpcd->ptSelEnd;
	}
	else
	{
		/*
		 *	Get start and end position
		 *	of the whole file.
		 */
		ptStart.x = ptStart.y = 0;
		ptEnd.y = Brainchild_ArrayGetSize( lpcd->lpLines ) - 1;
		ptEnd.x = (( LPLINE )Brainchild_ArrayGetAt( lpcd->lpLines, ptEnd.y ))->nLength;
	}

	/*
	 *	Move the caret to the start position.
	 */
	lpcd->ptCaretPos = ptStart;

	/*
	 *	Clear the marker.
	 */
	ClearMark( lpcd );

	/*
	 *	Continue replacing until we reached
	 *	the end...
	 */
	for ( ;; )
	{
		/*
		 *	Find the next occurence.
		 */
		if ( FindString( lpcd, hParent, 1, TRUE, FALSE ) == FALSE )
			break;

		/*
		 *	Is the end position of the
		 *	found string passed the given range?
		 */
		if ( lpcd->ptSelEnd.y == ptEnd.y && lpcd->ptSelEnd.x > ptEnd.x )
			/*
			 *	Out of range.
			 */
			break;
		else if ( lpcd->ptSelEnd.y > ptEnd.y )
			/*
			 *	Out of original range...
			 */
			break;
		else
		{
			/*
			 *	Copy selection points.
			 */
			POINT	ptSelStart = lpcd->ptSelStart, ptSelEnd = lpcd->ptSelEnd;

			/*
			 *	Delete the current selection.
			 */
			if ( DeleteText( lpcd, &ptSelStart, &ptSelEnd, ! bDidReplace ) == FALSE )
				return;

			/*
			 *	Clear the selection.
			 */
			lpcd->ptSelStart.x = lpcd->ptSelStart.y = -1;
			lpcd->ptSelEnd.x   = lpcd->ptSelEnd.y   = -1;
			lpcd->ptStartPos.x = lpcd->ptStartPos.y = -1;

			/*
			 *	Move the caret to the selection start.
			 */
			lpcd->ptCaretPos = ptSelStart;

			/*
			 *	Insert the replacement text.
			 */
			if ( lpcd->lpszReplaceText )
				InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, lpcd->lpszReplaceText, &lpcd->ptCaretPos, FALSE );

			/*
			 *	Increase counter.
			 */
			nCounter++;

			/*
			 *	Update column position.
			 */
			lpcd->nLastColumnPos = GetCaretOffset( lpcd, lpcd->ptCaretPos.x );

			/*
			 *	We did replace text.
			 */
			bDidReplace = TRUE;
		}
	}

	/*
	 *	Did we replace some text?
	 */
	if ( bDidReplace == FALSE )
	{
		/*
		 *	Pop a dialog.
		 */
		OkInformation( lpcd, hParent, GetString( IDS_NOTHING_REPLACED ), lpcd->bmSkipTable.lpszSearchString ? lpcd->bmSkipTable.lpszSearchString : _T(""), lpcd->lpszReplaceText ? lpcd->lpszReplaceText : _T("") );
		
		/*
		 *	Reset caret position.
		 */
		lpcd->ptCaretPos = ptCaretPos;
		return;
	}

	/*
	 *	Reset caret position.
	 */
	lpcd->ptCaretPos = ptCaretPos;

	/*
	 *	Make sure the caret is
	 *	visible.
	 */
	MakeCaretVisibleNoRedraw( lpcd );

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );

	/*
	 *	We are modified.
	 */
	SetModified( lpcd, TRUE );
	SendCaretMessage( lpcd );

	/*
	 *	Tell 'm how many ocurences of the string where replaced.
	 */
	OkInformation( lpcd, hParent, GetString( IDS_REPLACED ), nCounter, lpcd->bmSkipTable.lpszSearchString ? lpcd->bmSkipTable.lpszSearchString : _T(""), lpcd->lpszReplaceText ? lpcd->lpszReplaceText : _T("") );
}

void DestroyReplaceDialog( HWND hDlg )
{
	/*
	 *	Get the data pointer.
	 */
	LPCLASSDATA lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );
	if ( lpcd )
	{
		/*
		 *	Save list contents to the registry.
		 */
		SaveSearchReplaceStrings( lpcd, &lpcd->slSearchList, FALSE );
		SaveSearchReplaceStrings( lpcd, &lpcd->slReplaceList, TRUE );

		/*
		 *	Destroy the window.
		 */
		DestroyWindow( hDlg );
		lpcd->hReplaceWnd = NULL;
	}
}

static BOOL CALLBACK ReplaceProc( HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch ( uMsg )
	{
		case	WM_INITDIALOG:
		{
			LPCLASSDATA	lpcd = ( LPCLASSDATA )lParam;
			POINT		ptStart = { -1, -1 }, ptEnd = { -1, -1 };
			HWND		hFind = GetDlgItem( hDlg, IDC_REPLACE_FIND ), hReplace = GetDlgItem( hDlg, IDC_REPLACE_REPLACE );
			WORD		wFindStyle, wRepStyle;
			DWORD		dwDefID;
			LPLINE		lpLine;
			LPSTRING	lps;

			/*
			 *	Setup class data pointer as
			 *	the dialog user data.
			 */
			SetWindowLong( hDlg, DWL_USER, ( LONG )lpcd );

			/*
			 *	Load strings from the registry.
			 */
			LoadSearchReplaceStrings( lpcd, &lpcd->slSearchList, FALSE );
			LoadSearchReplaceStrings( lpcd, &lpcd->slReplaceList, TRUE );
			
			/*
			 *	Add search strings...
			 */
			for ( lps = lpcd->slSearchList.lpFirst; lps->lpNext; lps = lps->lpNext )
				SendMessage( hFind, CB_INSERTSTRING, ( WPARAM )-1, ( LPARAM )lps->lpszString );
			
			/*
			 *	And the replace strings.
			 */
			for ( lps = lpcd->slReplaceList.lpFirst; lps->lpNext; lps = lps->lpNext )
				SendMessage( hReplace, CB_INSERTSTRING, ( WPARAM )-1, ( LPARAM )lps->lpszString );

			/*
			 *	Setup checkboxes.
			 */
			CheckDlgButton( hDlg, IDC_REPLACE_MATCHCASE,  Parser->bSearchCase ? BST_CHECKED : BST_UNCHECKED );
			CheckDlgButton( hDlg, IDC_REPLACE_WHOLEWORDS, Parser->bWholeWords ? BST_CHECKED : BST_UNCHECKED );

			/*
			 *	Selection?
			 */
			if ( ! HasMark( lpcd ))
			{
				/*
				 *	Disable selection button and check
				 *	the file button.
				 */
				EnableWindow( GetDlgItem( hDlg, IDC_REPLACE_SELECTION ), FALSE );
				CheckDlgButton( hDlg, IDC_REPLACE_FILE, BST_CHECKED );

				/*
				 *	Setup default flags.
				 */
				wFindStyle = BS_DEFPUSHBUTTON;
				wRepStyle  = BS_PUSHBUTTON;

				/*
				 *	Setup default ID.
				 */
				dwDefID = IDC_FIND_NEXT;
			}
			else
			{
				/*
				 *	Disable the find next and replace 
				 *	buttons and select the file radio button.
				 */
				EnableWindow( GetDlgItem( hDlg, IDC_FIND_NEXT ), FALSE );
				EnableWindow( GetDlgItem( hDlg, IDC_REPLACE ),   FALSE );
				CheckDlgButton( hDlg, IDC_REPLACE_SELECTION, BST_CHECKED );
				
				/*
				 *	Setup default flags.
				 */
				wFindStyle = BS_PUSHBUTTON;
				wRepStyle  = BS_DEFPUSHBUTTON;

				/*
				 *	Setup default ID.
				 */
				dwDefID = IDC_REPLACE_ALL;
			}

			/*
			 *	Setup the default button.
			 */
			SendDlgItemMessage( hDlg, IDC_FIND_NEXT,   BM_SETSTYLE, MAKEWPARAM( wFindStyle, 0 ), MAKELPARAM( TRUE, 0 ));
			SendDlgItemMessage( hDlg, IDC_REPLACE_ALL, BM_SETSTYLE, MAKEWPARAM( wRepStyle,  0 ), MAKELPARAM( TRUE, 0 ));
			SendMessage( hDlg, DM_SETDEFID, dwDefID, 0 );

			/*
			 *	Is we have a selection on one line we use it as
			 *	the search string.
			 */
			if ( HasMark( lpcd ) && lpcd->ptSelStart.y == lpcd->ptSelEnd.y )
			{
				/*
				 *	Use the selection.
				 */
				lpLine = Brainchild_ArrayGetAt( lpcd->lpLines, lpcd->ptSelStart.y );
				ptStart = lpcd->ptSelStart;
				ptEnd   = lpcd->ptSelEnd;
			}
			else
			{
				/*
				 *	Use the word on which the cursor
				 *	is located.
				 */
				lpLine = GETLINE( lpcd );
				GetWordPoints( lpcd, &ptStart, &ptEnd );
			}

			/*
			 *	Was there as selection or word?
			 */
			if ( ptStart.x != -1 && ptStart.y != -1 && ptEnd.x != -1 && ptEnd.y != -1 )
			{
				TCHAR *pcTemp = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( ptEnd.x - ptStart.x + 1 ));

				/*
				 *	Allocation ok?
				 */
				if ( pcTemp )
				{
					/*
					 *	Copy the text.
					 */
					memcpy( pcTemp, &lpLine->pcText[ ptStart.x ], REAL_SIZE( ptEnd.x - ptStart.x ));

					/*
					 *	Set it up.
					 */
					SetWindowText( hFind, pcTemp );
					AddSearchString( lpcd, hFind );

					/*
					 *	Free the string.
					 */
					Brainchild_FreePooled( lpcd->pMemPool, pcTemp );
				}
			}
			else
			{
				/*
				 *	Any in the list?
				 */
				if ( lpcd->slSearchList.lpFirst->lpNext )
				{
					/*
					 *	Set it up.
					 */
					SetWindowText( hFind, lpcd->slSearchList.lpFirst->lpszString );
					AddSearchString( lpcd, hFind );
				}
			}

			/*
			 *	Setup the first strings.
			 */
			SendMessage( hFind,    CB_SETCURSEL, 0, 0 );
			SendMessage( hReplace, CB_SETCURSEL, 0, 0 );

			/*
			 *	Setup control buttons.
			 */
			{
				int nLength = GetWindowTextLength( hFind );
				EnableWindow( GetDlgItem( hDlg, IDC_FIND_NEXT   ), ( BOOL )nLength );
				EnableWindow( GetDlgItem( hDlg, IDC_REPLACE     ), ( BOOL )nLength );
				EnableWindow( GetDlgItem( hDlg, IDC_REPLACE_ALL ), ( BOOL )nLength );
				SendMessage( hDlg, DM_SETDEFID, nLength ? IDC_FIND_NEXT : IDCANCEL, 0 );
			}

			/*
			 *	Focus the edit control.
			 */
			return TRUE;
		}

		case WM_ENABLE:
			// Make sure our parent inherits our state.
			EnableWindow( GetParent( hDlg ), wParam );
			break;

		case WM_ACTIVATE:
			/*
			 *	Are we being deactivated?
			 */
			if ( LOWORD( wParam ) == WA_INACTIVE )
			{
				/*
				 *	If the window being activated is not our
				 *	parent we hide ourselves unless we are
				 *	disabled.
				 */
				if (( HWND )lParam != GetParent( hDlg ) && IsWindowEnabled( hDlg ))
					ShowWindow( hDlg, SW_HIDE );
			}
			break;

		case	WM_CLOSE:
			/*
			 *	Bye...
			 */
			DestroyReplaceDialog( hDlg );
			break;

		case	WM_COMMAND:
		{
			LPCLASSDATA	lpcd = ( LPCLASSDATA )GetWindowLong( hDlg, DWL_USER );
			HWND		hFind = GetDlgItem( hDlg, IDC_REPLACE_FIND );
			HWND		hReplace = GetDlgItem( hDlg, IDC_REPLACE_REPLACE );

			switch ( LOWORD( wParam ))
			{
				case	IDC_REPLACE_SELECTION:
				case	IDC_REPLACE_FILE:
				{
					/*
					 *	Whole file checked?
					 */
					BOOL	bFile = ( BOOL )( IsDlgButtonChecked( hDlg, IDC_REPLACE_FILE ) == BST_CHECKED );
					WORD	wFindStyle = BS_DEFPUSHBUTTON, wRepStyle = BS_PUSHBUTTON;
					DWORD	dwDefID = IDC_FIND_NEXT;

					/*
					 *	Enable/disable find next and
					 *	replace buttons.
					 */
					EnableWindow( GetDlgItem( hDlg, IDC_FIND_NEXT ), bFile );
					EnableWindow( GetDlgItem( hDlg, IDC_REPLACE ),   bFile );

					/*
					 *	Setup default button.
					 */
					if ( bFile == FALSE )
					{
						/*
						 *	Make the Replace All button default.
						 */
						wFindStyle = BS_PUSHBUTTON;
						wRepStyle  = BS_DEFPUSHBUTTON;
						dwDefID    = IDC_REPLACE_ALL;
					}

					/*
					 *	Setup the default button.
					 */
					SendDlgItemMessage( hDlg, IDC_FIND_NEXT,   BM_SETSTYLE, MAKEWPARAM( wFindStyle, 0 ), MAKELPARAM( TRUE, 0 ));
					SendDlgItemMessage( hDlg, IDC_REPLACE_ALL, BM_SETSTYLE, MAKEWPARAM( wRepStyle,  0 ), MAKELPARAM( TRUE, 0 ));
					SendMessage( hDlg, DM_SETDEFID, dwDefID, 0 );
					break;
				}

				case	IDC_REPLACE_FIND:
					/*
					 *	Focus killed?
					 */
					if ( HIWORD( wParam ) == CBN_KILLFOCUS )
						AddSearchString( lpcd, hFind );
					else if ( HIWORD( wParam ) == CBN_EDITCHANGE )
					{
						int nLength = GetWindowTextLength( hFind );
						EnableWindow( GetDlgItem( hDlg, IDC_FIND_NEXT   ), ( BOOL )nLength );
						EnableWindow( GetDlgItem( hDlg, IDC_REPLACE     ), ( BOOL )nLength );
						EnableWindow( GetDlgItem( hDlg, IDC_REPLACE_ALL ), ( BOOL )nLength );
						SendMessage( hDlg, DM_SETDEFID, nLength ? IDC_FIND_NEXT : IDCANCEL, 0 );
					}
					break;

				case	IDC_REPLACE_REPLACE:
					/*
					 *	Focus killed?
					 */
					if ( HIWORD( wParam ) == CBN_KILLFOCUS )
						AddReplaceString( lpcd, hReplace );
					break;

				case	IDC_REPLACE_MATCHCASE:
					/*
					 *	Get match case boolean.
					 */
					Parser->bSearchCase = IsDlgButtonChecked( hDlg, IDC_REPLACE_MATCHCASE ) == BST_CHECKED ? TRUE : FALSE;
					break;

				case	IDC_REPLACE_WHOLEWORDS:
					/*
					 *	Get whole words boolean.
					 */
					Parser->bWholeWords = IsDlgButtonChecked( hDlg, IDC_REPLACE_WHOLEWORDS ) == BST_CHECKED ? TRUE : FALSE;
					break;

				case	IDC_REPLACE:
					/*
					 *	Make sure we are up to date,
					 */
					AddSearchString( lpcd, hFind );
					AddReplaceString( lpcd, hReplace );

					/*
					 *	Replace the current selection
					 *	or find the search string.
					 */
					Replace( lpcd, hDlg );
					break;

				case	IDC_REPLACE_ALL:
					/*
					 *	Make sure we are up to date,
					 */
					AddSearchString( lpcd, hFind );
					AddReplaceString( lpcd, hReplace );

					/*
					 *	Replace all occurences of the
					 *	search string in the given
					 *	range.
					 */
					ReplaceAll( lpcd, hDlg, IsDlgButtonChecked( hDlg, IDC_REPLACE_SELECTION ));
					break;

				case	IDC_FIND_NEXT:
				{
					/*
					 *	Setup edit control contents.
					 */
					AddSearchString( lpcd, hFind );
					
					/*
					 *	Find it forwards.
					 */
					FindString( lpcd, hDlg, 1, FALSE, FALSE );
					break;
				}

				case	IDCANCEL:
					/*
					 *	Bye...
					 */
					DestroyReplaceDialog( hDlg );
					break;
			}
			break;
		}
	}
	return FALSE;
}

void ReplaceDialog( LPCLASSDATA lpcd )
{
	RECT rcFind = { 0 };
	BOOL bPositionAtFind = FALSE;

	/*
	 *	Clear qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Is the find dialog open?
	 */
	if ( lpcd->hFindWnd )
	{
		/*
		 *	Note it's position.
		 */
		GetWindowRect( lpcd->hFindWnd, &rcFind );
		bPositionAtFind = TRUE;
	}

	/*
	 *	Create the dialog.
	 */
	lpcd->hReplaceWnd = CreateDialogParam( hDLL, MAKEINTRESOURCE( IDD_REPLACE ), lpcd->hWnd, &ReplaceProc, ( LPARAM )lpcd );
	if ( lpcd->hReplaceWnd )
	{
		/*
		 *	Do we need to position it at the find window?
		 */
		if ( bPositionAtFind )
			/*
			 *	Yes. Do so and destroy the find window.
			 */
			SetWindowPos( lpcd->hReplaceWnd, NULL, rcFind.left, rcFind.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );

		/*
		 *	Show the dialog.
		 */
		ShowWindow( lpcd->hReplaceWnd, SW_SHOW );

		/*
		 *	Destroy the find dialog (if open).
		 */
		if ( lpcd->hFindWnd )
			DestroyFindDialog( lpcd->hFindWnd );
	}
}

LRESULT OnReplaceDialog( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Popup the replace dialog.
	 */
	ReplaceDialog( lpcd );
	return 0;
}
