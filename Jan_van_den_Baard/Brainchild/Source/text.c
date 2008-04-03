/*
 *	text.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Parts of this code (C) Copyright 2000 Andrei Stcherbatchenko.
 *
 *	All text related routines.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

#define READ_BUF_SIZE	32768
#define UNICOOKIE	( WORD )0xFEFFU

/*
 *	Insert text at the caret position.
 */
LRESULT OnInsertText( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPCTSTR		lpszText = ( LPCTSTR )lParam;
	BOOL		bDeleted = FALSE, bRC;

	/*
	 *	Are we read-only?
	 */
	if ( ISREADONLY )
		return FALSE;

	/*
	 *	Any marks set?
	 */
	if ( HasMark( lpcd ))
		bDeleted = Delete( lpcd );

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Insert the text.
	 */
	bRC = InsertText( lpcd, lpcd->ptCaretPos.y, lpcd->ptCaretPos.x, lpszText, &lpcd->ptCaretPos, ! bDeleted );

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
	return bRC;
}

/*
 *	Return the unicode flag.
 */
LRESULT OnGetUnicodeFlag( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return lpcd->bUnicode;
}

/*
 *	Set the unicode flag.
 */
LRESULT OnSetUnicodeFlag( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	BOOL bOldVal = lpcd->bUnicode;
	lpcd->bUnicode = ( BOOL )( wParam ? TRUE : FALSE );
	return bOldVal;
}

/*
 *	Return the number of lines.
 */
LRESULT OnNumberOfLines( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	return ArrayGetSize( lpcd->lpLines );
}

/*
 *	Get the whole text.
 */
LRESULT OnGetText( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	POINT		ptStart, ptEnd;
	LPTSTR		pszResult;
	HGLOBAL		hGlobal;

	/*
	 *	Any text?
	 */
	if ( AnyText( lpcd ))
	{
		/*
		 *	Get start location.
		 */
		ptStart.x = ptStart.y = 0;
		ptEnd.y	  = ArrayGetSize( lpcd->lpLines ) - 1;;
		ptEnd.x   = (( LPLINE )ArrayGetAt( lpcd->lpLines, ptEnd.y ))->nLength;

		/*
		 *	Buffer supplied?
		 */
		if ( lParam )
		{
			/*
			 *	Get the text.
			 */
			if (( pszResult = GetTextBuffer( lpcd, &ptStart, &ptEnd, ( LPTSTR )lParam )) != NULL )
				/*
				 *	Return a pointer to the
				 *	input buffer.
				 */
				return ( LRESULT )pszResult;
		}
		else
		{
			/*
			 *	Get the text.
			 */
			if (( hGlobal = GetTextGlobal( lpcd, &ptStart, &ptEnd )) != NULL )
				/*
				 *	Return the handle.
				 */
				return ( LRESULT )hGlobal;
		}
	}
	return 0;
}

LRESULT OnGetTextLength( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	int		i, nBytes = 0, nLines = ArrayGetSize( lpcd->lpLines );

	/*
	 *	Any text?
	 */
	if ( AnyText( lpcd ) == FALSE )
		return 0;

	/*
	 *	Compute the file size
	 *	in bytes.
	 */
	for ( i = 0; i < nLines; i++ )
	{
		/*
		 *	Add line length.
		 */
		nBytes += (( LPLINE )ArrayGetAt( lpcd->lpLines, i ))->nLength;

		/*
		 *	Add line terminator bytes
		 *	if this is not the last line.
		 */
		if ( i < nLines - 1 )
			nBytes += 2;
	}

	return ( LRESULT )REAL_SIZE( nBytes + 1 );
}

LRESULT OnGetWord( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	POINT		ptStart, ptEnd;
	LPTSTR		pszResult;
	HGLOBAL		hGlobal;

	/*
	 *	Get the word points.
	 */
	if ( GetWordPoints( lpcd, &ptStart, &ptEnd ))
	{
		/*
		 *	Buffer supplied?
		 */
		if ( lParam )
		{
			/*
			 *	Get the text.
			 */
			if (( pszResult = GetTextBuffer( lpcd, &ptStart, &ptEnd, ( LPTSTR )lParam )) != NULL )
				/*
				 *	Return a pointer to the
				 *	input buffer.
				 */
				return ( LRESULT )pszResult;
		}
		else
		{
			/*
			 *	Get the text...
			 */
			if (( hGlobal = GetTextGlobal( lpcd, &ptStart, &ptEnd )) != NULL )
				/*
				 *	Return the handle.
				 */
				return ( LRESULT )hGlobal;
		}
	}
	return 0;
}

LRESULT OnGetWordLength( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	POINT		ptStart, ptEnd;

	/*
	 *	Get the word points.
	 */
	if ( GetWordPoints( lpcd, &ptStart, &ptEnd ))
		/*
		 *	Return the length in bytes
		 *	including a terminating 0
		 *	byte.
		 */
		return ( LRESULT)( REAL_SIZE( ptEnd.x - ptStart.x + 1 ));

	return 0;
}

LRESULT OnGetLine( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine;
	POINT		ptStart, ptEnd;
	LPTSTR		pszResult;
	HGLOBAL		hGlobal;

	/*
	 *	Get the requested line.
	 */
	if ( wParam != -1 ) lpLine = ArrayGetAt( lpcd->lpLines, wParam );
	else		    lpLine = GETLINE( lpcd );

	/*
	 *	Any text?
	 */
	if ( lpLine && lpLine->pcText && lpLine->nLength )
	{
		/*
		 *	Set the points.
		 */
		ptStart.x = 0;
		ptStart.y = ptEnd.y = wParam == -1 ? lpcd->ptCaretPos.y : wParam;
		ptEnd.x   = lpLine->nLength;

		/*
		 *	Buffer supplied?
		 */
		if ( lParam )
		{
			/*
			 *	Get the text.
			 */
			if (( pszResult = GetTextBuffer( lpcd, &ptStart, &ptEnd, ( LPTSTR )lParam )) != NULL )
				/*
				 *	Return a pointer to the
				 *	input buffer.
				 */
				return ( LRESULT )pszResult;
		}
		else
		{
			/*
			 *	Get the text...
			 */
			if (( hGlobal = GetTextGlobal( lpcd, &ptStart, &ptEnd )) != NULL )
				/*
				 *	Return the handle.
				 */
				return ( LRESULT )hGlobal;
		}
	}
	return 0;
}

LRESULT OnGetLineLength( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPLINE		lpLine;

	/*
	 *	Get the requested line.
	 */
	if ( wParam != -1 ) lpLine = ArrayGetAt( lpcd->lpLines, wParam );
	else		    lpLine = GETLINE( lpcd );

	/*
	 *	Return the line length
	 *	in bytes including a
	 *	terminating 0-byte.
	 */
	return ( LRESULT )REAL_SIZE( lpLine ? lpLine->nLength + 1 : 0 );
}

LRESULT OnGetSelection( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPTSTR		pszResult;
	HGLOBAL		hGlobal;

	/*
	 *	Valid selection?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Buffer supplied?
		 */
		if ( lParam )
		{
			/*
			 *	Get the text.
			 */
			if (( pszResult = GetTextBuffer( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd, ( LPTSTR )lParam )) != NULL )
				/*
				 *	Return a pointer to the
				 *	input buffer.
				 */
				return ( LRESULT )pszResult;
		}
		else
		{
			/*
			 *	Get the text...
			 */
			if (( hGlobal = GetTextGlobal( lpcd, &lpcd->ptSelStart, &lpcd->ptSelEnd )) != NULL )
				/*
				 *	Return the handle.
				 */
				return ( LRESULT )hGlobal;
		}
	}
	return 0;
}

LRESULT OnGetSelectionLength( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	int		nNumChars = 0, i = 0;

	/*
	 *	Valid selection?
	 */
	if ( HasMark( lpcd ))
	{
		/*
		 *	Count the number of bytes required. This will
		 *	result in a buffer which can hold _atleast_ the
		 *	number of bytes required.
		 */
		for ( i = lpcd->ptSelStart.y; i <= lpcd->ptSelEnd.y; i++ )
			/*
			 *	Simply compute the number of bytes
			 *	required to store the whole line.
			 */
			nNumChars += (( LPLINE )ArrayGetAt( lpcd->lpLines, i ))->nLength + 2;

		/*
		 *	Adjust for the start and
		 *	end columns.
		 */
		nNumChars -= lpcd->ptSelStart.x;
		nNumChars -= (( LPLINE )ArrayGetAt( lpcd->lpLines, lpcd->ptSelEnd.y ))->nLength - ( lpcd->ptSelEnd.x - 1 );

		/*
		 *	Return the required buffer size.
		 */
		return REAL_SIZE( nNumChars );
	}
	return 0;
}

LRESULT OnSetText( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	TCHAR		szName[ MAX_PATH ], *pszTmp;
	BOOL		bRC;

	/*
	 *	Check modified?
	 */
	if ( wParam == FALSE )
	{
		if ( OnCheckModified( hWnd, 0, 0, lpcd ) == FALSE )
			return FALSE;
	}

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Get the file name.
	 */
	_tcscpy( szName, GetString( IDS_NO_NAME ));

	/*
	 *	Steal the extention of the
	 *	current filename.
	 */
	if (( pszTmp = _tcsrchr( lpcd->szFileName, _T( '.' ))) != NULL )
		_tcscat( szName, pszTmp );

	/*
	 *	Setup the file name.
	 */
	SetFileName( lpcd, szName );

	/*
	 *	Delete the contents.
	 */
	DeleteContents( lpcd );

	/*
	 *	No longer modified.
	 */
	SetModified( lpcd, FALSE );

	/*
	 *	Reset view.
	 */
	lpcd->cQualifier	= 0;
	lpcd->nLastColumnPos	= 0;
	lpcd->ptCaretPos.x	= lpcd->ptCaretPos.y = 0;
	lpcd->ptViewPos.x	= lpcd->ptViewPos.y  = 0;
	lpcd->ptSelStart.x	= lpcd->ptSelStart.y = -1;
	lpcd->ptSelEnd.x	= lpcd->ptSelEnd.y   = -1;
	lpcd->dwFileAttr	= FILE_ATTRIBUTE_ARCHIVE;

	/*
	 *	We _must_ have atleast one
	 *	empty line.
	 */
	if ( ArrayGetSize( lpcd->lpLines ) == 0 )
	{
		if ( InsertLine( lpcd, NULL, 0, -1 ) == FALSE )
			return FALSE;
	}

	/*
	 *	Insert the text.
	 */
	if ( lParam )
		bRC = InsertText( lpcd, 0, 0, ( LPCTSTR )lParam, &lpcd->ptCaretPos, TRUE );
	else
		bRC = TRUE;

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );

	return bRC;
}

/*
 *	Insert a line at the given position.
 */
LRESULT OnInsertLine( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	int nPos = ( int )wParam;

	/*
	 *	Read only? Valid pointer?
	 */
	if ( ISREADONLY && lParam )
		return FALSE;

	/*
	 *	Hide the caret.
	 */
	DisplayCaret( lpcd, FALSE );

	/*
	 *	Clamp position.
	 */
	if ( nPos < 0 || nPos > ArrayGetSize( lpcd->lpLines )) nPos = -1;

	/*
	 *	Insert the text line.
	 */
	if ( InsertLine( lpcd, ( LPCTSTR )lParam, -1, nPos ) == FALSE )
	{
		DisplayCaret( lpcd, TRUE );
		return FALSE;
	}

	/*
	 *	Re-render.
	 */
	InvalidateRect( lpcd->hWnd, NULL, FALSE );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );

	/*
	 *	Show the caret.
	 */
	DisplayCaret( lpcd, TRUE );
	return TRUE;
}

/*
 *	Flush undo buffers.
 */
void FlushUndoBuffers( LPCLASSDATA lpcd )
{
	/*
	 *	Free the memory allcated in the array
	 *	and all it's elements.
	 */
	ArrayReset( lpcd->lpUndoRedo );

	/*
	 *	No more entries.
	 */
	lpcd->nUndoPosition = lpcd->nSavePosition = 0;
}

/*
 *	Delete text.
 */
void DeleteContents( LPCLASSDATA lpcd )
{
	/*
	 *	Stop parsing timer.
	 */
	if ( lpcd->nParseTimerID ) 
	{
		KillTimer( lpcd->hWnd, lpcd->nParseTimerID );
		lpcd->nParseTimerID = 0;
	}

	/*
	 *	Free the memory allcated in the array
	 *	and all it's elements.
	 */
	ArrayReset( lpcd->lpLines );

	/*
	 *	Flush undo buffers.
	 */
	FlushUndoBuffers( lpcd );

	/*
	 *	Reset longest line.
	 */
	lpcd->nLongestLine = 0;

	/*
	 *	No bookmarks.
	 */
	lpcd->nBookmarks = 0;

	/*
	 *	Reset scrollers.
	 */
	SetupVScroller( lpcd );
	SetupHScroller( lpcd );
}

/*
 *	Delete contents (command)
 */
BOOL DeleteTheContents( LPCLASSDATA lpcd )
{
	TCHAR	szName[ MAX_PATH ] = { 0 }, *pszTmp;

	/*
	 *	Any text?
	 */
	if ( AnyText( lpcd ))
	{
		/*
		 *	Are we modified?
		 */
		if ( OnCheckModified( lpcd->hWnd, 0, 0, lpcd ) == FALSE )
			return FALSE;

		/*
		 *	Get the file name.
		 */
		_tcscpy( szName, GetString( IDS_NO_NAME ));

		/*
		 *	Steal the extention of the
		 *	current filename.
		 */
		if (( pszTmp = _tcsrchr( lpcd->szFileName, _T( '.' ))) != NULL )
			_tcscat( szName, pszTmp );

		/*
		 *	Setup the file name.
		 */
		lpcd->bSilentMode = TRUE;
		SetFileName( lpcd, szName );
		lpcd->bSilentMode = FALSE;

		/*
		 *	Delete the contents.
		 */
		DeleteContents( lpcd );

		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	No longer modified.
		 */
		SetModified( lpcd, FALSE );

		/*
		 *	Reset view.
		 */
		lpcd->cQualifier	= 0;
		lpcd->nLastColumnPos	= 0;
		lpcd->ptCaretPos.x	= lpcd->ptCaretPos.y = 0;
		lpcd->ptViewPos.x	= lpcd->ptViewPos.y  = 0;
		lpcd->ptSelStart.x	= lpcd->ptSelStart.y = -1;
		lpcd->ptSelEnd.x	= lpcd->ptSelEnd.y   = -1;
		lpcd->dwFileAttr	= FILE_ATTRIBUTE_ARCHIVE;

		/*
		 *	We _must_ have atleast one
		 *	empty line.
		 */
		if ( ArrayGetSize( lpcd->lpLines ) == 0 )
		{
			if ( InsertLine( lpcd, NULL, 0, -1 ) == FALSE )
				return FALSE;
		}

		/*
		 *	Re-render.
		 */
		InvalidateRect( lpcd->hWnd, NULL, FALSE );

		/*
		 *	Send status message.
		 */
		SendStatusMessage( lpcd );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
	return TRUE;
}

static void RecomputeLongestLine( LPCLASSDATA lpcd )
{
	int		nLine, nLength;

	/*
	 *	Reset line length.
	 */
	lpcd->nLongestLine = 0;

	/*
	 *	Iterate lines.
	 */
	for ( nLine = 0; nLine < ArrayGetSize( lpcd->lpLines ); nLine++ )
	{
		/*
		 *	Compute visual length.
		 */
		nLength = CaretOffsetLine( lpcd, nLine, (( LPLINE )ArrayGetAt( lpcd->lpLines, nLine ))->nLength );

		/*
		 *	Larger?
		 */
		if ( nLength > lpcd->nLongestLine )

			/*
			 *	Set it up.
			 */
			lpcd->nLongestLine = nLength;
	}

	/*
	 *	Setup the scroller.
	 */
	SetupHScroller( lpcd );
}

BOOL InsertLine( LPCLASSDATA lpcd, LPCTSTR lpszText, int nLength, int nPosition )
{
	LINE		line = { 0 };
	BOOL		bAdd;

	/*
	 *	Any length passed?
	 */
	if ( nLength == -1 )
	{
		/*
		 *	If there was a valid text pointer we
		 *	compute the length ourselves.
		 */
		if ( lpszText != NULL )
			nLength = _tcslen( lpszText );
	}

	/*
	 *	Setup length and allocation
	 *	size.
	 */
	line.nLength = nLength;
	line.nMax    = nLength != 0 ? ROUND_SIZE( REAL_SIZE( nLength )) : 0;
	line.nBlock  = -1;

	/*
	 *	Fix casing if turned on.
	 */
	if ( Parser->bCaseFixing )
		line.dwFlags = LNF_FIXCASING;

	/*
	 *	Any text to allocate and copy?
	 */
	if ( nLength != 0 )
	{
		/*
		 *	Allocate line storage,
		 */
		if (( line.pcText = ArrayAllocMem( lpcd->lpLines, line.nMax )) != NULL )
			/*
			 *	Copy the text.
			 */
			memcpy( line.pcText, lpszText, REAL_SIZE( nLength ));
		else
			return FALSE;
	}

	/*
	 *	Position given?
	 */
	if ( nPosition == -1 ) bAdd = ArrayAdd( lpcd->lpLines, &line, 1 );
	else		       bAdd = ArrayInsertAt( lpcd->lpLines, nPosition, &line, 1 );

	/*
	 *	OK?
	 */
	if ( bAdd == FALSE )
	{
		/*
		 *	Deallocate text.
		 */
		if ( line.pcText )
			ArrayFreeMem( lpcd->lpLines, line.pcText );
		return FALSE;
	}

	/*
	 *	Compute it's length.
	 */
	nLength = CaretOffsetLine( lpcd, nPosition == -1 ? ArrayGetSize( lpcd->lpLines ) - 1 : nPosition, nLength );

	/*
	 *	Is it the longest?
	 */
	if ( nLength > lpcd->nLongestLine )
	{
		/*
		 *	Yes. Store new length.
		 */
		lpcd->nLongestLine = nLength;

		/*
		 *	Setup scroller.
		 */
		SetupHScroller( lpcd );
	}

	return TRUE;
}

static BOOL AppendText( LPCLASSDATA lpcd, LPCTSTR lpszText, int nLine, int nLength )
{
	LPLINE		lpLine;

	/*
	 *	Length passed?
	 */
	if ( nLength == -1 )
	{
		/*
		 *	Any text to append?
		 */
		if ( lpszText != NULL )
			nLength = _tcslen( lpszText );
		else
			return TRUE;
	}

	/*
	 *	Get a pointer to the line.
	 */
	if (( lpLine = ArrayGetAt( lpcd->lpLines, nLine )) != NULL )
	{
		/*
		 *	Compute the required storage space.
		 */
		int nRequired = ROUND_SIZE( REAL_SIZE( lpLine->nLength + nLength ));

		/*
		 *	Fix casing if turned on.
		 */
		if ( Parser->bCaseFixing )
			lpLine->dwFlags |= LNF_FIXCASING;

		/*
		 *	Will it fit in the current buffer?
		 */
		if ( nRequired > lpLine->nMax )
		{
			/*
			 *	Allocate a new buffer.
			 */
			TCHAR*	pcReAlloc = ArrayAllocMem( lpcd->lpLines, nRequired );
			if ( pcReAlloc == NULL )
				return FALSE;

			/*
			 *	If there was any text we copy it to the
			 *	new buffer and free the old one.
			 */
			if ( lpLine->pcText )
			{
				memcpy( pcReAlloc, lpLine->pcText, REAL_SIZE( lpLine->nLength ));
				ArrayFreeMem( lpcd->lpLines, lpLine->pcText );
			}

			/*
			 *	Setup new buffer.
			 */
			lpLine->pcText = pcReAlloc;
			lpLine->nMax   = nRequired;
		}

		/*
		 *	Append text.
		 */
		memcpy( lpLine->pcText + lpLine->nLength, lpszText, REAL_SIZE( nLength ));

		/*
		 *	Adjust length.
		 */
		lpLine->nLength += nLength;

		/*
		 *	Get the new length.
		 */
		nLength = CaretOffsetLine( lpcd, nLine, lpLine->nLength );

		/*
		 *	Is it the longest line?
		 */
		if ( nLength > lpcd->nLongestLine )
		{
			/*
			 *	Yes. Store new length.
			 */
			lpcd->nLongestLine = nLength;

			/*
			 *	Adjust scroller.
			 */
			SetupHScroller( lpcd );
		}
		return TRUE;
	}
	return FALSE;
}

/*
 *	Read Unicode data from the file.
 */
BOOL __stdcall ReadUnicodeFile( HANDLE hFile, LPVOID pcReadBuf, DWORD dwSize, LPDWORD pdwBytesRead, LPOVERLAPPED lpOverlapped )
{
	/*
	 *	Anything to read?
	 */
	if ( dwSize == 0 )
	{
		*pdwBytesRead = 0L;
		return TRUE;
	}
	else
	{
		/*
		 *	Allocate a buffer which can hold sizeof( WCHAR ) * dwSize
		 *	bytes.
		 */
		WCHAR *pcUnicodeBuf = malloc( dwSize * sizeof( WCHAR ) );

		/*
		 * 	OK?
		 */
		if ( pcUnicodeBuf )
		{
			/*
			 *	Read dwSize * sizeof( WCHAR ) bytes into the
			 *	allocated buffer.
			 */
			if ( ReadFile( hFile, pcUnicodeBuf, dwSize * sizeof( WCHAR ), pdwBytesRead, lpOverlapped ))
			{
				/*
				 *	The amount of characters is
				 *	the amount of bytes actually read
				 *	devided by the size of a single wide
				 *	character.
				 */
				*( pdwBytesRead) /= sizeof( WCHAR );

				/*
				 *	Perform the conversion from wide characters
				 *	to multi-byte characters.
				 */
				if (( *pdwBytesRead = ( DWORD )WideCharToMultiByte( CP_ACP, 0, pcUnicodeBuf, ( int )( *pdwBytesRead ), pcReadBuf, ( int )( *pdwBytesRead ), NULL, NULL )) != 0) 
				{
					/*
					 *	Free the buffer and return
					 *	OK.
					 */
					free( pcUnicodeBuf );
					return TRUE;
				}
			}
			/*
			 *	Free the allocated buffer.
			 */
			free( pcUnicodeBuf );
		}
		else
			/*
			 *	Buffer allocation failed.
			 */
			SetLastError( ERROR_NOT_ENOUGH_MEMORY );
	}
	return FALSE;
}

BOOL LoadFile( LPCLASSDATA lpcd, LPCTSTR lpszFileName )
{
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	int		nLineBufSize = 256, nLineLen = 0;
	TCHAR		*pcLineBuf = NULL, *pcReadBuf = NULL;
	BOOL		bError = FALSE, bCaseFix = Parser->bCaseFixing;
	DWORD		dwBytesRead, dwBufferIndex = 0, i;
	WORD		wCookie = 0;

	/*
	 *	Turn off case-fixing.
	 */
	Parser->bCaseFixing = FALSE;

	/*
	 *	Simple loop. This allows me to issue a break
	 *	for easier error handling...
	 */
	for ( ;; )
	{
		/*
		 *	Allocate read buffer.
		 */
		if (( pcReadBuf = malloc( READ_BUF_SIZE )) == NULL )
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Allocate line buffer.
		 */
		if (( pcLineBuf = malloc( REAL_SIZE( nLineBufSize ))) == NULL )
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Determine file attributes.
		 */
		if (( lpcd->dwFileAttr = GetFileAttributes( lpszFileName )) == ( DWORD )-1 )
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Open file.
		 */
		hFile = CreateFile( lpszFileName,
				    GENERIC_READ,
				    FILE_SHARE_READ,
				    NULL,
				    OPEN_EXISTING,
				    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
				    NULL );

		/*
		 *	OK?
		 */
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Get file time.
		 */
		if ( GetFileTime( hFile, NULL, NULL, &lpcd->ftFileTime ) == FALSE )
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Is it a Unicode file?
		 */
		if ( ! ReadFile( hFile, &wCookie, sizeof( WORD ), &dwBytesRead, NULL ))
		{
			bError = TRUE;
			break;
		}

		/* 
		 *	Well. Is it?
		 */
		if ( wCookie != UNICOOKIE )
		{
			/* 
			 *	No. Seek back to the
			 *	start of the file.
			 */
			if ( SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER )
			{
				bError = TRUE;
				break;
			}

			/*
			 *	Not a Unicode file.
			 */
			lpcd->bUnicode = FALSE;
		}
		else
			/*
			 *	A Unicode file.
			 */
			lpcd->bUnicode = TRUE;

		/*
		 *	Fill read buffer. Uses the Unicode
		 *	version when necessary
		 */
		if ( ! ( wCookie == UNICOOKIE ? ReadUnicodeFile : ReadFile)( hFile, pcReadBuf, READ_BUF_SIZE, &dwBytesRead, NULL ))
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Try to determine the file mode.
		 */
		for ( i = 0; i < dwBytesRead; i++ )
		{
			/*
			 *	Find a carriage return or a line feed.
			 */
			if ( pcReadBuf[ i ] == _T( '\r' ))
			{
				/*
				 *	Is it followed by a line feed?
				 */
				if ( i < dwBytesRead - 1 && pcReadBuf[ i + 1 ] == _T( '\n' ))
				{
					/*
					 *	MSDOS.
					 */
					Parser->nFileMode = FMODE_MSDOS;
					break;
				}
				else
				{
					/*
					 *	Macintosh.
					 */
					Parser->nFileMode = FMODE_MACINTOSH;
					break;
				}
			}
			else if ( pcReadBuf[ i ] == _T( '\n' ))
			{
				/*
				 *	Unix.
				 */
				Parser->nFileMode = FMODE_UNIX;
				break;
			}
		}

		/*
		 *	Iterate the buffer.
		 */
		while ( dwBufferIndex < dwBytesRead && bError == FALSE )
		{
			/*
			 *	Get character.
			 */
			TCHAR	c = pcReadBuf[ dwBufferIndex++ ];

			/*
			 *	Load buffer empty?
			 */
			if ( dwBufferIndex == dwBytesRead && dwBytesRead == READ_BUF_SIZE )
			{
				/*
				 *	Re-fill the read buffer. Uses the Unicode
				 *	version when necessary
				 */
				if ( ! ( wCookie == UNICOOKIE ? ReadUnicodeFile : ReadFile)( hFile, pcReadBuf, READ_BUF_SIZE, &dwBytesRead, NULL ))
				{
					/*
					 *	Error...
					 */
					bError = TRUE;
					continue;
				}

				/*
				 *	Reset buffer index.
				 */
				dwBufferIndex = 0;
			}

			/*
			 *	What was read...
			 */
			switch ( c )
			{
				case	_T( '\r' ):
					/*
					 *	Macintosh or dos file mode?
					 */
					if ( Parser->nFileMode == FMODE_MACINTOSH || Parser->nFileMode == FMODE_MSDOS )
					{
						/*
						 *	0-terminate.
						 */
						pcLineBuf[ nLineLen ] = 0;

						/*
						 *	Add the line.
						 */
						if ( InsertLine( lpcd, pcLineBuf, nLineLen, -1 ) ==  FALSE )
						{
							bError = TRUE;
							break;
						}

						/*
						 *	Reset line length.
						 */
						nLineLen = 0;
					}
					break;

				case	_T( '\n' ):
					/*
					 *	Unix or Amiga file mode?
					 */
					if ( Parser->nFileMode == FMODE_UNIX )
					{
						/*
						 *	0-terminate.
						 */
						pcLineBuf[ nLineLen ] = 0;

						/*
						 *	Add the line.
						 */
						if ( InsertLine( lpcd, pcLineBuf, nLineLen, -1 ) == FALSE )
						{
							bError = TRUE;
							break;
						}

						/*
						 *	Reset line length.
						 */
						nLineLen = 0;
					}
					break;

				default:
					/*
					 *	Store the character in the line buffer.
					 */
					pcLineBuf[ nLineLen++ ] = c;

					/*
					 *	Line buffer full?
					 */
					if ( nLineLen == nLineBufSize )
					{
						TCHAR	*pcReAlloc;

						/*
						 *	Adjust buffer size.
						 */
						nLineBufSize += 256;

						/*
						 *	Re-allocate buffer.
						 */
						if (( pcReAlloc = malloc( REAL_SIZE( nLineBufSize ))) == NULL )
						{
							bError = TRUE;
							break;
						}

						/*
						 *	Setup new buffer.
						 */
						memcpy( pcReAlloc, pcLineBuf, REAL_SIZE( nLineBufSize - 256 ));
						free( pcLineBuf );
						pcLineBuf = pcReAlloc;
					}
					break;
			}
		}

		/*
		 *	Any text still in the buffer?
		 */
		if ( nLineLen )
		{
			/*
			 *	0-terminate.
			 */
			pcLineBuf[ nLineLen ] = 0;

			/*
			 *	Add it.
			 */
			if ( InsertLine( lpcd, pcLineBuf, nLineLen, -1 ) == FALSE )
			{
				bError = TRUE;
				break;
			}
		}

		/*
		 *	No longer modified.
		 */
		SetModified( lpcd, FALSE );
		break;
	}

	/*
	 *	We _must_ have atleast one
	 *	empty line.
	 */
	if ( ArrayGetSize( lpcd->lpLines ) == 0 )
	{
		if ( InsertLine( lpcd, NULL, 0, -1 ) == FALSE )
			bError = TRUE;
		lpcd->dwFileAttr = FILE_ATTRIBUTE_ARCHIVE;
	}

	/*
	 *	Cleanup.
	 */
	if ( hFile != INVALID_HANDLE_VALUE ) CloseHandle( hFile );
	if ( pcReadBuf ) free( pcReadBuf );
	if ( pcLineBuf ) free( pcLineBuf );

	/*
	 *	Restore case-fixing.
	 */
	Parser->bCaseFixing = bCaseFix;

	/*
	 *	Errors?
	 */
	return bError ? FALSE : TRUE;
}

/*
 *	Line terminators.
 */
static TCHAR *lpszCrLf[] =
{
	_T("\r\n"),		/* FMODE_MSDOS */
	_T("\n"),		/* FMODE_UNIX */
	_T("\r"),		/* FMODE_MACINTOSH */
};

/*
 *	Write Unicode data to the file.
 */

BOOL __stdcall WriteUnicodeFile( HANDLE hFile, LPVOID pcWriteBuf, DWORD dwSize, LPDWORD pdwBytesWritten, LPOVERLAPPED lpOverlapped )
{
	/*
	 *	Anything to convert?
	 */
	if ( dwSize == 0 )
	{
		*pdwBytesWritten = 0L;
		return TRUE;
	}
	else
	{
		/*
		 *	Allocate a buffer which can hold sizeof( WCHAR ) * dwSize
		 *	bytes.
		 */
		WCHAR *pcUnicodeBuf = malloc( dwSize * sizeof( WCHAR ) );

		/*
		 *	OK?
		 */
		if ( pcUnicodeBuf )
		{
			/*
			 *	Convert the input string to uicode.
			 */
			DWORD dwConverted;
			if (( dwConverted = MultiByteToWideChar( CP_ACP, 0, pcWriteBuf, dwSize, pcUnicodeBuf, dwSize )) != 0)
			{
				/*
				 *	Write dwConverted * sizeof( WCHAR ) bytes to the
				 *	output file.
				 */
				if ( WriteFile( hFile, pcUnicodeBuf, dwConverted * sizeof( WCHAR ), pdwBytesWritten, lpOverlapped ))
				{
					/*
					 *	Adjust to "characters".
					 */
					*pdwBytesWritten /= sizeof( WCHAR );

					/*
					 *	Free and return success.
					 */
					free( pcUnicodeBuf );
					return TRUE;
				}
			}
			/*
			 *	Free the conversion buffer.
			 */
			free( pcUnicodeBuf );
		}
		else
			/*
			 *	Buffer allocation failed.
			 */
			SetLastError( ERROR_NOT_ENOUGH_MEMORY );
	}
	return FALSE;
}


BOOL SaveFile( LPCLASSDATA lpcd, LPCTSTR lpszFileName )
{
	HANDLE		hFile = INVALID_HANDLE_VALUE;
	int		nLines, nCrSize, i;
	DWORD		dwBytesWritten, dwFileAttr = lpcd->dwFileAttr;
	BOOL		bError = FALSE;

	/*
	 *	Simple loop. This allows me to issue a break
	 *	for easier error handling...
	 */
	for ( ;; )
	{
		/*
		 *	Open the output file.
		 */
		hFile = CreateFile( lpszFileName,
				    GENERIC_WRITE,
				    0,
				    NULL,
				    CREATE_ALWAYS,
				    FILE_ATTRIBUTE_NORMAL,
				    NULL );

		/*
		 *	OK?
		 */
		if ( hFile == INVALID_HANDLE_VALUE )
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Get the number of lines in the text
		 *	and the number of bytes in the line
		 *	terminator.
		 */
		nLines = ArrayGetSize( lpcd->lpLines );

		if ( Parser->nFileMode == FMODE_MSDOS ) nCrSize = 2;
		else				      nCrSize = 1;

		/*
		 *	Write Unicode cookie if this
		 *	is a Unicode file.
		 */
		if ( lpcd->bUnicode )
		{
			WORD wCookie = UNICOOKIE;
			if ( ! WriteFile( hFile, &wCookie, sizeof( WORD ), &dwBytesWritten, NULL ))
			{
				bError = TRUE;
				break;
			}
			if (( int )dwBytesWritten != sizeof( WORD ))
			{
				bError = TRUE;
				break;
			}
		}

		/*
		 *	Iterate...
		 */
		for ( i = 0; i < nLines; i++ )
		{
			/*
			 *	Get line structure pointer.
			 */
			LPLINE	lpLine = ArrayGetAt( lpcd->lpLines, i );

			/*
			 *	Write the line.
			 */
			if ( lpLine->pcText )
			{
				/*
				 *	Clear trailing spaces?
				 */
				if ( Parser->bClearTrailingSpaces )
				{
					while ( _istspace( lpLine->pcText[ lpLine->nLength - 1 ] ) && lpLine->nLength )
						lpLine->nLength--;
				}

				if ( ! ( lpcd->bUnicode ? WriteUnicodeFile : WriteFile )( hFile, lpLine->pcText, lpLine->nLength, &dwBytesWritten, NULL ))
				{
					bError = TRUE;
					break;
				}
				if (( int )dwBytesWritten != lpLine->nLength )
				{
					bError = TRUE;
					break;
				}
			}

			/*
			 *	Terminate the line if it is not
			 *	the last one...
			 */
			if ( i < nLines - 1 )
			{
				if ( ! ( lpcd->bUnicode ? WriteUnicodeFile : WriteFile )( hFile, lpszCrLf[ Parser->nFileMode ], nCrSize, &dwBytesWritten, NULL ))
				{
					bError = TRUE;
					break;
				}
				if (( int )dwBytesWritten != nCrSize )
				{
					bError = TRUE;
					break;
				}
			}
		}
		break;
	}

	/*
	 *	Get file time.
	 */
	if ( GetFileTime( hFile, NULL, NULL, &lpcd->ftFileTime ) == FALSE )
		bError = TRUE;

	/*
	 *	Close the handle.
	 */
	if ( hFile != INVALID_HANDLE_VALUE ) CloseHandle( hFile );

	/*
	 *	Clear undo buffer after save?
	 */
	if ( Parser->bClearUndoSave )
		FlushUndoBuffers( lpcd );

	/*
	 *	Error?
	 */
	if ( bError )
	{
		DeleteFile( lpszFileName );
		return FALSE;
	}
	else
	{
		/*
		 *	Open as read-only?
		 */
		if ( lpcd->bOpenAsReadOnly )
		{
			/*
			 *	Query if the user want's to save
			 *	the file as read-only or not.
			 */
			if ( MessageBox( lpcd->hWnd, GetString( IDS_OPEN_AS_READONLY ), lpcd->szFileName, MB_YESNO | MB_ICONWARNING | MB_APPLMODAL ) == IDNO )
				/*
				 *	Do not save as read only.
				 */
				dwFileAttr &= ~FILE_ATTRIBUTE_READONLY;
		}
	}

	/*
	 *	Set file attributes.
	 */
	return SetFileAttributes( lpszFileName, dwFileAttr );
}

static LPTSTR StaticGetText( LPCLASSDATA lpcd, LPPOINT ptStart, LPPOINT ptEnd, BOOL bGlobal, TCHAR *pcBuffer )
{
	HGLOBAL	hGlobal = NULL;
	TCHAR  *pcText = NULL;
	int	nNumChars = 0, i;

	/*
	 *	Input buffer supplied?
	 */
	if ( pcBuffer == NULL )
	{
		/*
		 *	Compute the number of characters necessary
		 *	to store the text.
		 */
		for ( i = ptStart->y; i <= ptEnd->y; i++ )
			/*
			 *	Add the length of the line including
			 *	2 for the line terminator.
			 */
			nNumChars += (( LPLINE )ArrayGetAt( lpcd->lpLines, i ))->nLength + 2;

		/*
		 *	Allocate the buffer.
		 */
		if ( bGlobal == FALSE )
		{
			/*
			 *	Allocate the required memory
			 *	from the Undo/Redo array.
			 */
			pcText = ArrayAllocMem( lpcd->lpUndoRedo, REAL_SIZE( nNumChars + 1 ));
		}
		else
		{
			/*
			 *	Allocate movable memory.
			 */
			if (( hGlobal = GlobalAlloc( GMEM_ZEROINIT | GMEM_MOVEABLE | GMEM_DDESHARE, REAL_SIZE( nNumChars + 1 ))) != NULL )
			{
				/*
				 *	Lock it...
				 */
				pcText = GlobalLock( hGlobal );
			}
		}
	}
	else
	{
		/*
		 *	Use input buffer.
		 */
		pcText = pcBuffer;
		memset( pcBuffer, 0, REAL_SIZE( nNumChars + 1 ));
	}

	/*
	 *	OK?
	 */
	if ( pcText != NULL )
	{
		/*
		 *	All characters on the same line?
		 */
		if ( ptStart->y == ptEnd->y )
		{
			LPLINE	lpLine = ArrayGetAt( lpcd->lpLines, ptStart->y );

			/*
			 *	Yes. Do a simple copy. The text is automatically 0-terminated
			 *	since we allocated one extra character and all allocations
			 *	in the pool are initialized with zeros.
			 */
			if ( lpLine->pcText )
				memcpy( pcText, &lpLine->pcText[ ptStart->x ], REAL_SIZE( ptEnd->x - ptStart->x ));
		}
		else
		{
			LPLINE	lpLine = ArrayGetAt( lpcd->lpLines, ptStart->y );
			TCHAR  *pcPtr = pcText;
			int	nCount, i;

			/*
			 *	First we copy the remaining characters
			 *	in the first line.
			 */
			if (( nCount = lpLine->nLength - ptStart->x ) > 0 )
			{
				/*
				 *	Copy the characters.
				 */
				if ( lpLine->pcText )
				{
					memcpy( pcPtr, &lpLine->pcText[ ptStart->x ], REAL_SIZE( nCount ));
					pcPtr += nCount;
				}
			}

			/*
			 *	Terminate line..
			 */
			memcpy( pcPtr, _T("\r\n"), REAL_SIZE( 2 ));
			pcPtr += 2;

			/*
			 *	Iterate lines upto the last one.
			 */
			for ( i = ptStart->y + 1; i < ptEnd->y; i++ )
			{
				lpLine = ArrayGetAt( lpcd->lpLines, i );

				/*
				 *	Copy the line.
				 */
				if ( lpLine->pcText && lpLine->nLength )
				{
					/*
					 *	Copy text.
					 */
					memcpy( pcPtr, lpLine->pcText, REAL_SIZE( lpLine->nLength ));
					pcPtr += lpLine->nLength;
				}

				/*
				 *	Terminate line..
				 */
				memcpy( pcPtr, _T("\r\n"), REAL_SIZE( 2 ));
				pcPtr += 2;
			}

			/*
			 *	Add the last characters.
			 */
			if ( ptEnd->x > 0 )
			{
				memcpy( pcPtr, (( LPLINE )ArrayGetAt( lpcd->lpLines, ptEnd->y ))->pcText, REAL_SIZE( ptEnd->x ));
				pcPtr += ptEnd->x;
			}
		}
		/*
		 *	Text is automatically 0-terminated.
		 */
	}
	else if ( bGlobal == TRUE )
	{
		/*
		 *	Release handle.
		 */
		GlobalFree( hGlobal );
		hGlobal = NULL;
	}

	/*
	 *	Was it global?
	 */
	if ( bGlobal == TRUE )
	{
		/*
		 *	Success?
		 */
		if ( hGlobal )
			/*
			 *	Release it.
			 */
			GlobalUnlock( hGlobal );

		return ( LPTSTR )hGlobal;
	}

	return pcText;
}

LPTSTR GetText( LPCLASSDATA lpcd, LPPOINT ptStart, LPPOINT ptEnd )
{
	return StaticGetText( lpcd, ptStart, ptEnd, FALSE, NULL );
}

HGLOBAL GetTextGlobal( LPCLASSDATA lpcd, LPPOINT ptStart, LPPOINT ptEnd )
{
	return ( HGLOBAL )StaticGetText( lpcd, ptStart, ptEnd, TRUE, NULL );
}

LPTSTR GetTextBuffer( LPCLASSDATA lpcd, LPPOINT ptStart, LPPOINT ptEnd, LPTSTR pszBuffer )
{
	return StaticGetText( lpcd, ptStart, ptEnd, FALSE, pszBuffer );
}

static BOOL StaticInsertText( LPCLASSDATA lpcd, int nLine, int nColumn, LPCTSTR lpszText, LPPOINT ptNewPos )
{
	LPLINE	lpLine;
	TCHAR  *pcRemChars = NULL;
	int	nRemaining = 0, nCurrent = nLine, nIndex;
	BOOL	bError = FALSE;

	/*
	 *	First we save the characters right
	 *	of the caret on the line we start
	 *	the insert on.
	 */
	if (( lpLine = ArrayGetAt( lpcd->lpLines, nLine )) != NULL )
	{
		/*
		 *	Any text?
		 */
		if ( lpLine->pcText )
		{
			/*
			 *	Any characters right of the caret?
			 */
			if (( nRemaining = lpLine->nLength - nColumn ) > 0 )
			{
				/*
				 *	Save the characters.
				 */
				if (( pcRemChars = AllocPooled( lpcd->pMemPool, REAL_SIZE( nRemaining ))) != NULL )
				{
					/*
					 *	Copy the characters and
					 *	truncate the line.
					 */
					memcpy( pcRemChars, &lpLine->pcText[ nColumn ], REAL_SIZE( nRemaining ));
					lpLine->nLength = nColumn;

					/*
					 *	Fix casing if turned on.
					 */
					if ( Parser->bCaseFixing ) 
						lpLine->dwFlags |= LNF_FIXCASING;
				}
				else
					return FALSE;
			}
		}
	} 
	else
		return FALSE;

	for ( ;; )
	{
		/*
		 *	Reset index.
		 */
		nIndex = 0;

		/*
		 *	Iterate the text until we reach a line
		 *	break or the end of the text.
		 */
		while ( lpszText[ nIndex ] != _T( '\0' ) && lpszText[ nIndex ] != _T( '\r' ) && lpszText[ nIndex ] != _T( '\n' ))
			nIndex++;

		/*
		 *	Are we still on the current line?
		 */
		if ( nCurrent == nLine )
		{
			/*
			 *	Yes. Append the characters.
			 */
			bError = ! AppendText( lpcd, lpszText, nCurrent, nIndex );
		}
		else
		{
			/*
			 *	Insert a new line.
			 */
			bError = ! InsertLine( lpcd, lpszText, nIndex, nCurrent );
		}

		/*
		 *	Did we reach the end of the string?
		 */
		if ( lpszText[ nIndex ] == _T( '\0' ))
		{
			/*
			 *	Setup the new position.
			 */
			if ( ptNewPos != NULL )
			{
				ptNewPos->x = (( LPLINE )( ArrayGetAt( lpcd->lpLines, nCurrent )))->nLength;
				ptNewPos->y = nCurrent;
			}

			/*
			 *	Any remaining characters to add?
			 */
			if ( pcRemChars )
				bError = ! AppendText( lpcd, pcRemChars, nCurrent, nRemaining  );
			break;
		}

		/*
		 *	Next line...
		 */
		nCurrent++;
		nIndex++;

		/*
		 *	Newline?
		 */
		if ( lpszText[ nIndex ] == _T( '\n' ) || lpszText[ nIndex ] == _T( '\r' ))
			nIndex++;

		/*
		 *	Adjust pointer.
		 */
		lpszText += nIndex;
	}

	/*
	 *	Free the remaining characters.
	 */
	if ( pcRemChars )
		FreePooled( lpcd->pMemPool, pcRemChars );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );
	SetupHScroller( lpcd );

	return ! bError;
}

/*
 *	Delete some text.
 */
static BOOL StaticDeleteText( LPCLASSDATA lpcd, LPPOINT ptFrom, LPPOINT ptTo )
{
	BOOL	bError = FALSE, bRecompute = FALSE;

	/*
	 *	All text on one line?
	 */
	if ( ptFrom->y == ptTo->y )
	{
		LPLINE	lpLine = ArrayGetAt( lpcd->lpLines, ptFrom->y );

		/*
		 *	Is this the longest line?
		 */
		if ( GetCaretOffset( lpcd, lpLine->nLength ) == lpcd->nLongestLine )
			bRecompute = TRUE;

		/*
		 *	Are there characters remaining?
		 */
		if ( ptTo->x < lpLine->nLength )
		{
			/*
			 *	Move the remaining characters.
			 */
			memmove( &lpLine->pcText[ ptFrom->x ], &lpLine->pcText[ ptTo->x ], REAL_SIZE( lpLine->nLength - ptTo->x ));
		}

		/*
		 *	Adjust the line length.
		 */
		lpLine->nLength -= ( ptTo->x - ptFrom->x );

		/*
		 *	Setup longest line.
		 */
		if ( bRecompute )
			RecomputeLongestLine( lpcd );

		/*
		 *	Send status message.
		 */
		SendStatusMessage( lpcd );

		return TRUE;
	}
	else
	{
		LPLINE	lpLine;
		TCHAR  *pcRemChars = NULL;
		int	nRemaining = 0, nLine;

		/*
		 *	First we save the characters right
		 *	of the caret on the line we start
		 *	the insert on.
		 */
		if (( lpLine = ArrayGetAt( lpcd->lpLines, ptTo->y )) != NULL )
		{
			/*
			 *	Any text?
			 */
			if ( lpLine->pcText )
			{
				/*
				 *	Any characters right of the caret?
				 */
				if (( nRemaining = lpLine->nLength - ptTo->x ) > 0 )
				{
					/*
					 *	Save the characters.
					 */
					if (( pcRemChars = AllocPooled( lpcd->pMemPool, REAL_SIZE( nRemaining ))) != NULL )
					{
						/*
						 *	Copy the characters and
						 *	truncate the line.
						 */
						memcpy( pcRemChars, &lpLine->pcText[ ptTo->x ], REAL_SIZE( nRemaining ));
						lpLine->nLength = ptTo->x;
					}
					else
						return FALSE;
				}
			}
		} else
			return FALSE;

		/*
		 *	Iterate lines.
		 */
		for ( nLine = ptFrom->y + 1; nLine <= ptTo->y; nLine++ )
		{
			/*
			 *	Get the line.
			 */
			lpLine = ArrayGetAt( lpcd->lpLines, nLine );

			/*
			 *	Remove any available text.
			 */
			if ( lpLine->pcText )
				ArrayFreeMem( lpcd->lpLines, lpLine->pcText );
		}

		/*
		 *	Remove the lines.
		 */
		ArrayRemoveAt( lpcd->lpLines, ptFrom->y + 1, ptTo->y - ptFrom->y );

		/*
		 *	Setup the length of the first line.
		 */
		lpLine = ArrayGetAt( lpcd->lpLines, ptFrom->y );
		lpLine->nLength = ptFrom->x;

		/*
		 *	Append remaining characters.
		 */
		if ( pcRemChars ) {
			bError = ! AppendText( lpcd, pcRemChars, ptFrom->y, nRemaining );
			FreePooled( lpcd->pMemPool, pcRemChars );
		}

	}

	/*
	 *	We _must_ have at least
	 *	one empty line.
	 */
	if ( ArrayGetSize( lpcd->lpLines ) == 0 )
		InsertLine( lpcd, NULL, 0, -1 );

	/*
	 *	Re-compute longest line.
	 */
	RecomputeLongestLine( lpcd );

	/*
	 *	Send status message.
	 */
	SendStatusMessage( lpcd );

	return ! bError;
}

/*
 *	Remove last undo record.
 */
static void RemoveLastUndoRecord( LPCLASSDATA lpcd )
{
	LPUNDOREDO	lpUndoRedo;

	/*
	 *	Can we remove it?
	 */
	if ( lpcd->nUndoPosition )
	{
		/*
		 *	One less.
		 */
		lpcd->nUndoPosition--;

		/*
		 *	Get the record.
		 */
		lpUndoRedo = ArrayGetAt( lpcd->lpUndoRedo, lpcd->nUndoPosition );

		/*
		 *	Free it's text.
		 */
		if ( lpUndoRedo->pcText )
			ArrayFreeMem( lpcd->lpUndoRedo, lpUndoRedo->pcText );

		/*
		 *	Remove the entry.
		 */
		ArrayRemoveAt( lpcd->lpUndoRedo, lpcd->nUndoPosition, 1 );
	}
}

BOOL InsertText( LPCLASSDATA lpcd, int nLine, int nColumn, LPCTSTR lpszText, LPPOINT ptNewPos, BOOL bGroupStart )
{
	POINT	ptStart = { nColumn, nLine };

	/*
	 *	Insert the text...
	 */
	if ( StaticInsertText( lpcd, nLine, nColumn, lpszText, ptNewPos ) == FALSE )
		return FALSE;

	/*
	 *	Add undo record.
	 */
	if ( AddUndoRecord( lpcd, bGroupStart ? URF_GROUP | URF_INSERT : URF_INSERT, &ptStart, ptNewPos, lpszText ) == FALSE )
	{
		/*
		 *	Delete inserted text.
		 */
		StaticDeleteText( lpcd, &ptStart, ptNewPos );
		return FALSE;
	}
	return TRUE;
}

BOOL DeleteText( LPCLASSDATA lpcd, LPPOINT ptFrom, LPPOINT ptTo, BOOL bGroupStart )
{
	/*
	 *	Get text to delete.
	 */
	LPTSTR		lpszUndoText = GetText( lpcd, ptFrom, ptTo );
	BOOL		bRC = FALSE;

	/*
	 *	OK?
	 */
	if ( lpszUndoText )
	{
		/*
		 *	Add undo record.
		 */
		if ( AddUndoRecord( lpcd, bGroupStart ? URF_GROUP | URF_DELETE : URF_DELETE, ptFrom, ptTo, lpszUndoText ) == TRUE )
		{
			/*
			 *	Delete the text.
			 */
			if ( StaticDeleteText( lpcd, ptFrom, ptTo ) == TRUE )
				/*
				 *	OK.
				 */
				bRC = TRUE;
			else
				/*
				 *	Remove undo record.
				 */
				RemoveLastUndoRecord( lpcd );
		}

		/*
		 *	Free the text.
		 */
		ArrayFreeMem( lpcd->lpUndoRedo, lpszUndoText );
	}
	return bRC;
}

BOOL AddUndoRecord( LPCLASSDATA lpcd, DWORD dwFlags, LPPOINT ptStart, LPPOINT ptEnd, LPCTSTR lpszText )
{
	LPUNDOREDO	lpRecord;
	int		nCount;
	BOOL		bRC = TRUE;

	/*
	 *	First we remove any re-done
	 *	records.
	 */
	nCount = ArrayGetSize( lpcd->lpUndoRedo );
	if ( lpcd->nUndoPosition < nCount )
		/*
		 *	Re-size the array.
		 */
		ArraySetSize( lpcd->lpUndoRedo, lpcd->nUndoPosition, -1 );

	/*
	 *	Are we close to critical?
	 */
	if ( ArrayGetSize( lpcd->lpUndoRedo ) >= lpcd->nUndoSize )
	{
		nCount = 0;

		for (;;)
		{
			/*
			 *	Get record.
			 */
			LPUNDOREDO lpUndoRedo = ArrayGetAt( lpcd->lpUndoRedo, nCount++ );
			LPUNDOREDO lpNext = ArrayGetAt( lpcd->lpUndoRedo, nCount );

			/*
			 *	Delete text.
			 */
			if ( lpUndoRedo->pcText )
				ArrayFreeMem( lpcd->lpUndoRedo, lpUndoRedo->pcText );

			/*
			 *	Is the buffer empty or are we
			 *	at a new group?
			 */
			if ( ArrayGetSize( lpcd->lpUndoRedo ) == 0 || (( lpNext->dwFlags & URF_GROUP ) == URF_GROUP ))
				break;
		}

		/*
		 *	Remove records.
		 */
		ArrayRemoveAt( lpcd->lpUndoRedo, 0, nCount );

		/*
		 *	Adjust the save position.
		 */
		lpcd->nSavePosition -= nCount;
	}

	/*
	 *	Allocate record.
	 */
	if (( lpRecord = ArrayAllocMem( lpcd->lpUndoRedo, sizeof( UNDOREDO ))) != NULL )
	{
		/*
		 *	Initalize it.
		 */
		lpRecord->ptStartPos = *ptStart;
		lpRecord->ptEndPos   = *ptEnd;
		lpRecord->dwFlags    = dwFlags;

		/*
		 *	Any text?
		 */
		if ( lpszText && *lpszText )
		{
			/*
			 *	Allocate buffer.
			 */
			if (( lpRecord->pcText = ArrayAllocMem( lpcd->lpUndoRedo, REAL_SIZE( _tcslen( lpszText ) + 1 ))) != NULL )
				/*
				 *	Copy the input text.
				 */
				_tcscpy( lpRecord->pcText, lpszText );
			else
			{
				/*
				 *	Free record.
				 */
				ArrayFreeMem( lpcd->lpUndoRedo, lpRecord );
				bRC = FALSE;
			}
		}

		/*
		 *	OK to add?
		 */
		if ( bRC )
		{
			/*
			 *	Add the record.
			 */
			if ( ArrayAdd( lpcd->lpUndoRedo, lpRecord, 1 ) == TRUE )
			{
				/*
				 *	Set new index.
				 */
				lpcd->nUndoPosition = ArrayGetSize( lpcd->lpUndoRedo );
				return TRUE;
			}
		}
	}

	return FALSE;
}

LRESULT OnCanUndo( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if ( ISREADONLY )
		return FALSE;

	return ( BOOL )( lpcd->nUndoPosition > 0 && ! ISREADONLY );
}

LRESULT OnCanRedo( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if ( ISREADONLY )
		return FALSE;

	return ( BOOL )( lpcd->nUndoPosition < ArrayGetSize( lpcd->lpUndoRedo ));
}

/*
 *	Undo all changes.
 */
void UndoAll( LPCLASSDATA lpcd )
{
	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Anything to undo?
	 */
	if ( lpcd->nUndoPosition > 0 )
	{
		/*
		 *	Quiet mode...
		 */
		lpcd->bSilentMode = TRUE;

		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Clear markers.
		 */
		ClearMark( lpcd );

		/*
		 *	Undo all of them...
		 */
		while ( lpcd->nUndoPosition )
			Undo( lpcd );

		/*
		 *	Is the caret still visible?
		 */
		if ( CaretInView( lpcd ) == FALSE )
			/*
			 *	Make it visible.
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
		 *	Send status message.
		 */
		SendStatusMessage( lpcd );

		/*
		 *	Show caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
}


/*
 *	Undo the latest change.
 */
void Undo( LPCLASSDATA lpcd )
{
	BOOL	bRedraw = FALSE;
	int	nOldPosition = lpcd->nUndoPosition;

	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Quiet mode?
	 */
	if ( ! lpcd->bSilentMode )
	{
		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Clear markers.
		 */
		ClearMark( lpcd );
	}

	/*
	 *	Are there any undo records?
	 */
	if ( lpcd->nUndoPosition > 0 ) {
		LPUNDOREDO	lpUndoRedo = NULL;
		int		nNumRecords = 0;

		for ( ;; )
		{
			/*
			 *	One less.
			 */
			lpcd->nUndoPosition--;

			/*
			 *	Add record done.
			 */
			nNumRecords++;
			/*
			 *	Get record.
			 */
			lpUndoRedo = ArrayGetAt( lpcd->lpUndoRedo, lpcd->nUndoPosition );

			/*
			 *	Undo text insert?
			 */
			if (( lpUndoRedo->dwFlags & URF_INSERT ) == URF_INSERT )
			{
				/*
				 *	Delete the inserted text.
				 */
				StaticDeleteText( lpcd, &lpUndoRedo->ptStartPos, &lpUndoRedo->ptEndPos );

				/*
				 *	Caret at the start position.
				 */
				lpcd->ptCaretPos = lpUndoRedo->ptStartPos;

			}
			else
			{
				/*
				 *	Insert the deleted text
				 */
				StaticInsertText( lpcd, lpUndoRedo->ptStartPos.y, lpUndoRedo->ptStartPos.x, lpUndoRedo->pcText, NULL );

				/*
				 *	Caret at the end position.
				 */
				lpcd->ptCaretPos = lpUndoRedo->ptEndPos;
			}

			/*
			 *	Group start or no more entries?
			 */
			if ((( lpUndoRedo->dwFlags & URF_GROUP ) == URF_GROUP ) || lpcd->nUndoPosition == 0 )
				break;
		}

		/*
		 *	Are we at the position the file was saved
		 *	last? 
		 */
		SetModified( lpcd, lpcd->nUndoPosition == lpcd->nSavePosition ? FALSE : TRUE );

		/*
		 *	Silent mode?
		 */
		if ( ! lpcd->bSilentMode )
		{
			/*
			 *	Is the caret still visible?
			 */
			if ( CaretInView( lpcd ) == FALSE )
			{
				/*
				 *	Make it visible.
				 */
				 MakeCaretVisibleNoRedraw( lpcd );
				 bRedraw = TRUE;
			}

			/*
			 *	Was the undo on a single line?
			 */
			if ( nNumRecords == 1 && lpUndoRedo->ptStartPos.y == lpUndoRedo->ptEndPos.y && ! bRedraw )
			{
				/*
				 *	Re-render a single line.
				 */
				 RenderLine( lpcd, lpUndoRedo->ptStartPos.y );
				 SetupHScroller( lpcd );
			}
			else
			{
				/*
				 *	Re-render view.
				 */
				InvalidateRect( lpcd->hWnd, NULL, TRUE );
				SetupHScroller( lpcd );
				SetupVScroller( lpcd );
			}
		}
	}

	/*
	 *	Silent mode?
	 */
	if ( ! lpcd->bSilentMode )
	{
		/*
		 *	Status change?
		 */
		if ( nOldPosition != lpcd->nUndoPosition )
			SendStatusMessage( lpcd );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
}

/*
 *	Re-do all of them...
 */
void RedoAll( LPCLASSDATA lpcd )
{
	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Anything to redo?
	 */
	if ( lpcd->nUndoPosition < ArrayGetSize( lpcd->lpUndoRedo ))
	{
		/*
		 *	Quiet mode...
		 */
		lpcd->bSilentMode = TRUE;

		/*
		 *	Hide the caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Clear markers.
		 */
		ClearMark( lpcd );

		/*
		 *	Redo all entries.
		 */
		while ( lpcd->nUndoPosition < ArrayGetSize( lpcd->lpUndoRedo ))
			Redo( lpcd );

		/*
		 *	Is the caret still visible?
		 */
		if ( CaretInView( lpcd ) == FALSE )
			/*
			 *	Make it visible.
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
		 *	Send status message.
		 */
		SendStatusMessage( lpcd );

		/*
		 *	Show caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
}

/*
 *	Redo.
 */
void Redo( LPCLASSDATA lpcd )
{
	int	nNumElem = ArrayGetSize( lpcd->lpUndoRedo );
	BOOL	bRedraw = FALSE;
	BOOL	bCanRedo = ( BOOL )( lpcd->nUndoPosition < nNumElem );

	/*
	 *	Read only?
	 */
	if ( ISREADONLY )
		return;

	/*
	 *	Silent mode?
	 */
	if ( ! lpcd->bSilentMode )
	{
		/*
		 *	Hide caret.
		 */
		DisplayCaret( lpcd, FALSE );

		/*
		 *	Clear markers.
		 */
		ClearMark( lpcd );
	}

	/*
	 *	Anything to redo?
	 */
	if ( lpcd->nUndoPosition < nNumElem )
	{
		LPUNDOREDO	lpUndoRedo = NULL, lpNext;
		int		nNumRecords = 0;

		for ( ;; )
		{
			/*
			 *	Get record.
			 */
			lpUndoRedo = ArrayGetAt( lpcd->lpUndoRedo, lpcd->nUndoPosition );

			/*
			 *	One more record done.
			 */
			nNumRecords++;

			/*
			 *	Redo text insert?
			 */
			if (( lpUndoRedo->dwFlags & URF_INSERT ) == URF_INSERT )
			{
				/*
				 *	Re-insert the text.
				 */
				StaticInsertText( lpcd, lpUndoRedo->ptStartPos.y, lpUndoRedo->ptStartPos.x, lpUndoRedo->pcText, &lpcd->ptCaretPos );

				/*
				 *	Caret at the end position.
				 */
				lpcd->ptCaretPos = lpUndoRedo->ptEndPos;
			}
			else
			{
				/*
				 *	Re-delete the text.
				 */
				StaticDeleteText( lpcd, &lpUndoRedo->ptStartPos, &lpUndoRedo->ptEndPos );

				/*
				 *	Caret at the start position.
				 */
				lpcd->ptCaretPos = lpUndoRedo->ptStartPos;
			}

			/*
			 *	One more to undo.
			 */
			lpcd->nUndoPosition++;

			/*
			 *	End of this group or end of redo?
			 */
			if (( lpNext = ArrayGetAt( lpcd->lpUndoRedo, lpcd->nUndoPosition )) == NULL )
				break;

			if ((( lpNext->dwFlags & URF_GROUP ) == URF_GROUP ) || lpcd->nUndoPosition == ArrayGetSize( lpcd->lpUndoRedo ))
				break;
		}

		/*
		 *	Are we at the position the file was saved
		 *	last?
		 */
		SetModified( lpcd, lpcd->nUndoPosition == lpcd->nSavePosition ? FALSE : TRUE );

		/*
		 *	Silent mode?
		 */
		if ( ! lpcd->bSilentMode )
		{
			/*
			 *	Is the caret still visible?
			 */
			if ( CaretInView( lpcd ) == FALSE )
			{
				/*
				 *	Make it visible.
				 */
				MakeCaretVisibleNoRedraw( lpcd );
				bRedraw = TRUE;
			}

			/*
			 *	Was the undo on a single line?
			 */
			if ( nNumRecords == 1 && lpUndoRedo->ptStartPos.y == lpUndoRedo->ptEndPos.y && ! bRedraw )
			{
				/*
				 *	Re-render a single line.
				 */
				 RenderLine( lpcd, lpUndoRedo->ptStartPos.y );
				 SetupHScroller( lpcd );
			}
			else
			{
				/*
				 *	Re-render view.
				 */
				InvalidateRect( lpcd->hWnd, NULL, TRUE );
				SetupHScroller( lpcd );
				SetupVScroller( lpcd );
			}
		}
	}

	/*
	 *	Silent mode?
	 */
	if ( ! lpcd->bSilentMode )
	{
		/*
		 *	Status changed?
		 */
		if ( bCanRedo != ( BOOL )( lpcd->nUndoPosition < nNumElem ))
			SendStatusMessage( lpcd );

		/*
		 *	Show the caret.
		 */
		DisplayCaret( lpcd, TRUE );
	}
}
