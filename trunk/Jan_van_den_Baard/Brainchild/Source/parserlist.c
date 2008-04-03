/*
 *	parserlist.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"
#include "LangRes.rh"

void FreeParserNode( POOL pMemPool, LPPARSER lpp )
{
	LPKEYHASH	lpHash, lpNext;
	LPSMART		lpSmart;
	int		i;

	/*
	 *	Free the strings.
	 */
	if ( lpp->pszTitle	  ) FreePooled( pMemPool, lpp->pszTitle );
	if ( lpp->pszExtentions   ) FreePooled( pMemPool, lpp->pszExtentions );
	if ( lpp->pszDelimiters   ) FreePooled( pMemPool, lpp->pszDelimiters );
	if ( lpp->pszFileName     ) FreePooled( pMemPool, lpp->pszFileName );
	if ( lpp->pszDateFormat   ) FreePooled( pMemPool, lpp->pszDateFormat );
	if ( lpp->pszTimeFormat   ) FreePooled( pMemPool, lpp->pszTimeFormat );
	if ( lpp->pszBackupPath   ) FreePooled( pMemPool, lpp->pszBackupPath );
	if ( lpp->pszHeaderLeft   ) FreePooled( pMemPool, lpp->pszHeaderLeft );
	if ( lpp->pszHeaderCenter ) FreePooled( pMemPool, lpp->pszHeaderCenter );
	if ( lpp->pszHeaderRight  ) FreePooled( pMemPool, lpp->pszHeaderRight );
	if ( lpp->pszFooterLeft   ) FreePooled( pMemPool, lpp->pszFooterLeft );
	if ( lpp->pszFooterCenter ) FreePooled( pMemPool, lpp->pszFooterCenter );
	if ( lpp->pszFooterRight  ) FreePooled( pMemPool, lpp->pszFooterRight );

	/*
	 *	Free the smart indention nodes.
	 */
	while (( lpSmart = ( LPSMART )RemHead(( LPLIST )&lpp->lSmartIndention )) != NULL )
	{
		if ( lpSmart->lpszSmartString ) FreePooled( pMemPool, lpSmart->lpszSmartString );
		FreePooled( pMemPool, lpSmart );
	}

	/*
	 *	Free the blocks array. This will 
	 *	automatically free the strings in
	 *	the block aswell.
	 */
	if ( lpp->lpaBlocks ) ArrayDelete( lpp->lpaBlocks );

	/*
	 *	Free the keyboard hashes.
	 */
	FreeHashTable( pMemPool, &lpp->aHash[ 0 ] );

	/*
	 *	Delete the fonts.
	 */
	if ( lpp->hPrinterFont  ) DeleteObject( lpp->hPrinterFont );
	if ( lpp->hScreenFont   ) DeleteObject( lpp->hScreenFont );
	if ( lpp->hScreenFontUL ) DeleteObject( lpp->hScreenFontUL );

	/*
	 *	Free the keyword hashes.
	 */
	for ( i = 0; i < 256; i++ )
	{
		for ( lpHash = lpp->aKeywords[ i ]; lpHash; lpHash = lpNext )
		{
			/*
			 *	Save next pointer.
			 */
			lpNext = lpHash->lpNext;

			/*
			 *	Delete keyword string.
			 */
			if ( lpHash->pszKeyword ) FreePooled( pMemPool, lpHash->pszKeyword );

			/*
			 *	And the hash entry itself.
			 */
			FreePooled( pMemPool, lpHash );
		}
	}

	/*
	 *	Free the parser structure itself.
	 */
	FreePooled( pMemPool, lpp );
}

/*
 *	Deallocate the parser list.
 */
void FreeParserList( POOL pMemPool, LPPARSERLIST lppl )
{
	LPPARSER	lpp;

	/*
	 *	Remove parser nodes.
	 */
	while (( lpp = ( LPPARSER )RemHead(( LPLIST )lppl )) != NULL )
		FreeParserNode( pMemPool, lpp );
}

/*
 *	Remove trailing blanks.
 */
void RemoveTrailingBlanks( LPTSTR lpszString )
{
	int	nLength = _tcslen( lpszString ) - 1;

	/*
	 *	Replace all trailing white spaces
	 *	by a 0 character.
	 */
	while ( _istspace( lpszString[ nLength ] ))
		lpszString[ nLength-- ] = _T( '\0' );
}

/*
 *	Get COLORREF from the string.
 */
static BOOL GetColorRef( LPTSTR lpszColorString, COLORREF *lpcrColor )
{
	int	r, g, b;

	/*
	 *	Scan for the values.
	 */
	if ( _stscanf( lpszColorString, _T("%d,%d,%d"), &r, &g, &b ) != 3 )
		return FALSE;

	/*
	 *	Store the color.
	 */
	*lpcrColor = RGB( r, g, b );
	return TRUE;
}

/*
 *	Copy string into array memory.
 */
static LPTSTR CopyArrayString( LPARRAY lpa, LPCTSTR lpszString )
{
	LPTSTR		lpszCopy;

	/*
	 *	Allocate memory.
	 */
	if (( lpszCopy = ArrayAllocMem( lpa, REAL_SIZE( _tcslen( lpszString ) + 1 ))) != NULL )
		_tcscpy( lpszCopy, lpszString );

	return lpszCopy;
}

/*
 *	Add a keyword hash to the
 *	hash table.
 */
BOOL AddKeyword( POOL pMemPool, LPPARSER lpp, LPCTSTR lpszKeyword, COLORREF crColor, COLORREF crBkColor )
{
	LPKEYHASH	*p, lpEntry, lpHash;
	WORD		 wIndex;

	/*
	 *	Allocate hash structure.
	 */
	if (( lpHash = AllocPooled( pMemPool, sizeof( KEYHASH ))) != NULL )
	{
		/*
		 *	Allocate a copy of the keyword.
		 */
		if (( lpHash->pszKeyword = CopyStringPool( pMemPool, lpszKeyword )) != NULL )
		{
			/*
			 *	Remove trailing blanks.
			 */
			RemoveTrailingBlanks( lpHash->pszKeyword );

			/*
			 *	Setup the rest.
			 */
			lpHash->nLength   = _tcslen( lpHash->pszKeyword );
			lpHash->crColor   = crColor;
			lpHash->crBkColor = crBkColor;

			/*
			 *	Hash the keyword.
			 */
			 wIndex = HashKeyword( lpHash->pszKeyword, lpHash->nLength, lpp->bCaseOn );

			 /*
			  *	Look up the entry.
			  */
			lpEntry = *( p = &lpp->aKeywords[ wIndex & 255 ] );

			/*
			 *	Scan to the end of the chain.
			 */
			while ( lpEntry )
			{
				/*
				 *	Does this hash already exist?
				 */
				if ( _tcscmp( lpEntry->pszKeyword, lpHash->pszKeyword ) == 0 )
				{
					/*
					 *	Yes. Kill this hash structure and
					 *	return success.
					 */
					FreePooled( pMemPool, lpHash->pszKeyword );
					FreePooled( pMemPool, lpHash );
					return TRUE;
				}
				/*
				 *	Next...
				 */
				lpEntry = *( p = &lpEntry->lpNext );
			}

			/*
			 *	Setup the new hash.
			 */
			*p = lpHash;

			return TRUE;
		}
		FreePooled( pMemPool, lpHash );
	}
	return FALSE;
}

/*
 *	Setup the parser for the
 *	file type.
 */
BOOL SetupParser( LPCLASSDATA lpcd, LPCTSTR lpszFileName )
{
	LPPARSER		lpp, lpd = NULL;
	LPTSTR			lpszExtention, lpszPtr;
	BOOL			bRC = FALSE;
	int			nLength;

	/*
	 *	Clear parser value.
	 */
	Parser = NULL;

	/*
	 *	Lock the list.
	 */
	LockParserList();

	/*
	 *	Find the default parser.
	 */
	for ( lpp = plParsers.lpFirst; lpp->lpNext; lpp = lpp->lpNext )
	{
		/*
		 *	Default?
		 */
		if ( lpp->bDefault == TRUE )
		{
			/*
			 *	Mark it.
			 */
			lpd = lpp;
			break;
		}
	}

	/*
	 *	Valid name?
	 */
	if ( lpszFileName )
	{
		/*
		 *	Find the file extention.
		 */
		if (( lpszExtention = _tcsrchr( lpszFileName, _T( '.' ))) != NULL )
		{
			/*
			 *	Get it's length;
			 */
			nLength = _tcslen( lpszExtention );

			/*
			 *	Iterate the parsers.
			 */
			for ( lpp = plParsers.lpFirst; lpp->lpNext; lpp = lpp->lpNext )
			{
				/*
				 *	Is this the one?
				 */
				if (( lpszPtr = lpp->pszExtentions ) != NULL )
				{
					while ( *lpszPtr )
					{
						if ( ! _tcsnicmp( lpszPtr, lpszExtention, nLength ))
						{
							/*
							 *	Is it really OK?
							 */
							if ( lpszPtr[ nLength ] == _T( ';' ) || lpszPtr[ nLength ] == _T( '\0' ))
							{
								/*
								 *	Setup this parser.
								 */
								Parser = lpp;
								bRC = TRUE;
								break;
							}
						}
						/*
						 *	Next...
						 */
						lpszPtr++;
					}
				}
			}
		}
	} else
		bRC = TRUE;

	/*
	 *	Unlock the list.
	 */
	UnlockParserList();

	/*
	 *	No parser. Use the default.
	 */
	if ( Parser == NULL )
		Parser = lpd;

	/*
	 *	Recompute view.
	 */
	SetupViewSize( lpcd );

	/*
	 *	Send messages.
	 */
	SendStatusMessage( lpcd );

	/*
	 *	Re-create caret.
	 */
	if ( lpcd->bHasFocus )
	{
		DisplayCaret( lpcd, FALSE );
		CreateTheCaret( lpcd );
	}

	/*
	 *	Pre-parse the file.
	 */
	if ( lpcd->bSilentMode == FALSE )
		/*
		 *	Re-start pre-parsing...
		 */
		lpcd->dwParsedUpTo = 0;

	/*
	 *	Simulate system colors change.
	 */
	if ( lpcd->bSilentMode == FALSE )
		SendMessage( lpcd->hWnd, WM_SYSCOLORCHANGE, 0, 0 );
	return bRC;
}

BOOL LoadParserFiles( POOL pMemPool, LPPARSERLIST lppl )
{
	HANDLE		hFile;
	WIN32_FIND_DATA	wfd;
	TCHAR		szCurrDir[ MAX_PATH ], szUser[ MAX_PATH ];
	BOOL		bRC = FALSE;

	/*
	 *	Save current directory.
	 */
	if ( GetCurrentDirectory( MAX_PATH, szCurrDir ) > 0 )
	{
		/*
		 *	Setup settings directory.
		 */
		if ( CheckUser( szUser ))
		{
			/*
			 *	Setup the directory.
			 */
			if ( SetCurrentDirectory( szUser ))
			{
				/*
				 *	Find parser files.
				 */
				if (( hFile = FindFirstFile( _T("*.bcp"), &wfd )) != INVALID_HANDLE_VALUE )
				{
					/*
					 *	Get all parser files.
					 */
					do
					{
						if(( bRC = LoadParserFile( pMemPool, lppl, wfd.cFileName, INVALID_HANDLE_VALUE, FALSE )) == FALSE )
							break;
					} while ( FindNextFile( hFile, &wfd ));

					/*
					 *	Close up the handle.
					 */
					FindClose( hFile );
				}
				/*
				 *	Reset directory.
				 */
				SetCurrentDirectory( szCurrDir );
			}
		}
	}
	return bRC;
}

/*
 *	Static parser data.
 */
static COLORREF	crColor, crBkColor;
static BOOL	bError = FALSE, bInBlock = FALSE, bInKeyword = FALSE, bKey = FALSE, bCList = FALSE;
static TCHAR	cCode, cQual;
static BLOCK	bBlock;
static LPCLIST	lpCList = NULL;
static BOOL	bErrorMsg = FALSE;

/*
 *	Skip blanks.
 */
#define SKIP_BLANKS(x)		while ( _istspace( *x )) x++;

/*
 *	Skip blanks and check for
 *	arguments.
 */
#define CHECK_ARGS(x)\
	while ( _istspace( *x )) x++;\
	if ( *x == _T( '\0' ))\
	{\
		ErrorMsg( GetString( IDS_ARGUMENT_REQUIRED ), nLineNum, lpszFileName );\
		bErrorMsg = TRUE;\
		return FALSE;\
	}

/*
 *	Boolean argument.
 */
#define CHECK_BOOLEAN(v,x)\
	if ( *x == _T( 'y' ) || *x == _T( 'Y' )) v = TRUE;\
	else					 v = FALSE;

/*
 *	Get RGB arguments.
 */
#define CHECK_RGB(c)\
	if ( GetColorRef( pszArgs, &c ) == FALSE )\
	{\
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );\
		bErrorMsg = TRUE;\
		return FALSE;\
	}

/*
 *	Read a string...
 */
BOOL ReadString( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs, LPTSTR *ppDest )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Already present?
	 */
	if ( *ppDest )
	{
		FreePooled( pMemPool, *ppDest );
		*ppDest = NULL;
	}

	/*
	 *	Copy the name string.
	 */
	if (( *ppDest = CopyStringPool( pMemPool, pszArgs )) == NULL )
	{
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	remove spaces from the end.
	 */
	RemoveTrailingBlanks( *ppDest );

	return TRUE;
}

/*
 *	Parse the name command.
 */
BOOL Name( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszTitle );
}

/*
 *	Parse the extentions command.
 */
BOOL Extentions( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszExtentions );
}

/*
 *	Parse the delimiters command.
 */
BOOL Delimiters( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszDelimiters );
}

/*
 *	Parse the block command.
 */
BOOL Block( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	In keywords mode?
	 */
	if ( bInKeyword )
		return FALSE;

	/*
	 *	Put us in block mode.
	 */
	bInBlock = TRUE;

	/*
	 *	Clear structure.
	 */
	memset( &bBlock, 0, sizeof( BLOCK ));
	bBlock.crBgColor = CLR_DEFAULT;
	return TRUE;
}

/*
 *	Parse the blockis command.
 */
BOOL BlockIs( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Put us in block mode.
	 */
	if ( ! Block( pMemPool, lpp, nLineNum, lpszFileName, pszArgs ))
		return FALSE;

	/*
	 *	Copy the block name string.
	 */
	if (( bBlock.pszName = CopyArrayString( lpp->lpaBlocks, pszArgs )) == NULL )
	{
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	remove spaces from the end.
	 */
	RemoveTrailingBlanks( bBlock.pszName );

	return TRUE;
}

/*
 *	Parse the start command.
 */
BOOL Start( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Block mode?
	 */
	if ( bInBlock == FALSE )
	{
		ErrorMsg( GetString( IDS_NO_BLOCK ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Copy the start string.
	 */
	if (( bBlock.pszStart = CopyArrayString( lpp->lpaBlocks, pszArgs )) == NULL )
	{
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	remove spaces from the end.
	 */
	RemoveTrailingBlanks( bBlock.pszStart );

	return TRUE;
}

/*
 *	Parse the startofline command.
 */
BOOL StartOfLine( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Block mode?
	 */
	if ( bInBlock == FALSE )
	{
		ErrorMsg( GetString( IDS_NO_BLOCK ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( bBlock.bStartOfLine, pszArgs );
	return TRUE;
}

/*
 *	Parse the End command.
 */
BOOL End( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Block mode?
	 */
	if ( bInBlock == FALSE )
	{
		ErrorMsg( GetString( IDS_NO_BLOCK ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	End of line?
	 */
	if ( _tcsnicmp( pszArgs, _T("\\n"), 2 ) == 0 )
		bBlock.pszEnd = END_OF_LINE;
	else if (( bBlock.pszEnd = CopyArrayString( lpp->lpaBlocks, pszArgs )) == NULL )
	{
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	remove spaces from the end.
	 */
	if ( bBlock.pszEnd != END_OF_LINE )
		RemoveTrailingBlanks( bBlock.pszEnd );

	return TRUE;
}

/*
 *	Parse the endblock command.
 */
BOOL EndBlock( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Are we in block mode?
	 */
	if ( bInBlock == FALSE )
	{
		ErrorMsg( GetString( IDS_ENDBLOCK_NO_BLOCK ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Store lengths.
	 */
	if ( bBlock.pszStart ) 	bBlock.nStart = _tcslen( bBlock.pszStart  );
				
	if ( bBlock.pszEnd ) 
	{
		if ( bBlock.pszEnd == END_OF_LINE ) bBlock.nEnd = 0;
		else				    bBlock.nEnd = _tcslen( bBlock.pszEnd );
	}

	/*
	 *	Save the block in the array.
	 */
	if ( ArrayAdd( lpp->lpaBlocks, &bBlock, 1 ) == FALSE )
	{
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	No longer in block mode.
	 */
	bInBlock = FALSE;

	/*
	 *	Reset color value.
	 */
	crColor = GetSysColor( COLOR_WINDOWTEXT );;

	return TRUE;
}


/*
 *	Parse the case command.
 */
BOOL Case( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bCaseOn, pszArgs );
	return TRUE;
}

/*
 *	Parse the autoindent command.
 */
BOOL AutoIndent( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bAutoIndent, pszArgs );
	return TRUE;
}

/*
 *	Parse the smartindent command.
 */
BOOL SmartIndent( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bSmartIndent, pszArgs );
	return TRUE;
}

/*
 *	Parse the selectionmargin command.
 */
BOOL SelectionMargin( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bSelectionMargin, pszArgs );
	return TRUE;
}

/*
 *	Parse the clearspaces command.
 */
BOOL ClearSpaces( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bClearTrailingSpaces, pszArgs );
	return TRUE;
}

/*
 *	Parse the automatch command.
 */
BOOL AutoMatch( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bAutoMatch, pszArgs );
	return TRUE;
}

/*
 *	Parse the wholewords command.
 */
BOOL WholeWords( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bWholeWords, pszArgs );
	return TRUE;
}

/*
 *	Parse the searchcase command.
 */
BOOL SearchCase( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bSearchCase, pszArgs );
	return TRUE;
}

/*
 *	Parse the visibleautomatcherror command.
 */
BOOL VisibleMatch( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bVisibleMatchError, pszArgs );
	return TRUE;
}

/*
 *	Parse the dndenabled command.
 */
BOOL DnDEnabled( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bDnDEnabled, pszArgs );
	return TRUE;
}

/*
 *	Parse the RGB command.
 */
BOOL RGBCom( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get arguments.
	 */
	CHECK_RGB( crColor );
	
	/*
	 *	Store it...
	 */
	if ( bInBlock ) bBlock.crColor = crColor;
	return TRUE;
}

/*
 *	Parse the BkRGB command.
 */
BOOL BkRGBCom( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default or RGB?
	 */
	if ( *pszArgs == _T( '*' )) crBkColor = CLR_DEFAULT;
	else CHECK_RGB( crBkColor );
	
	/*
	 *	Store it...
	 */
	if ( bInBlock ) bBlock.crBgColor = crBkColor;
	return TRUE;
}

/*
 *	Parse the TextRGB command.
 */
BOOL TextRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_TEXT;
		lpp->crColors[ CARR_TEXT ] = GetSysColor( COLOR_WINDOWTEXT );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_TEXT;
		CHECK_RGB( lpp->crColors[ CARR_TEXT ] );
	}
	return TRUE;
}

/*
 *	Parse the BackgroundRGB command.
 */
BOOL BackgroundRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_BACKGROUND;
		lpp->crColors[ CARR_BACKGROUND ] = GetSysColor( COLOR_WINDOW );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_BACKGROUND;
		CHECK_RGB( lpp->crColors[ CARR_BACKGROUND ] );
	}
	return TRUE;
}

/*
 *	Parse the BackgroundReadOnlyRGB command.
 */
BOOL BackgroundReadOnlyRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_BACKGROUND_RO;
		lpp->crColors[ CARR_BACKGROUND_READONLY ] = GetSysColor( COLOR_3DLIGHT );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_BACKGROUND_RO;
		CHECK_RGB( lpp->crColors[ CARR_BACKGROUND_READONLY ] );
	}
	return TRUE;
}

/*
 *	Parse the MarginRGB command.
 */
BOOL MarginRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_MARGIN;
		lpp->crColors[ CARR_SELECTION_MARGIN ] = GetSysColor( COLOR_SCROLLBAR );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_MARGIN;
		CHECK_RGB( lpp->crColors[ CARR_SELECTION_MARGIN ] );
	}
	return TRUE;
}

/*
 *	Parse the BookmarkRGB command.
 */
BOOL BookmarkRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_BOOKMARK;
		lpp->crColors[ CARR_BOOKMARK ] = RGB( 0, 255, 255 );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_BOOKMARK;
		CHECK_RGB( lpp->crColors[ CARR_BOOKMARK ] );
	}
	return TRUE;
}

/*
 *	Parse the DelimiterRGB command.
 */
BOOL DelimiterRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_DELIMITER;
		lpp->crColors[ CARR_DELIMITER ] = GetSysColor( COLOR_WINDOWTEXT );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_DELIMITER;
		CHECK_RGB( lpp->crColors[ CARR_DELIMITER ] );
	}

	return TRUE;
}

/*
 *	Parse the NumberRGB command.
 */
BOOL NumberRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_NUMBER;
		lpp->crColors[ CARR_NUMBER ] = GetSysColor( COLOR_WINDOWTEXT );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_NUMBER;
		CHECK_RGB( lpp->crColors[ CARR_NUMBER ] );
	}
	return TRUE;
}

/*
 *	Parse the SelectedTextRGB command.
 */
BOOL SelTextRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_SELECTED_TEXT;
		lpp->crColors[ CARR_SELECTED_TEXT ] = GetSysColor( COLOR_HIGHLIGHTTEXT );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_SELECTED_TEXT;
		CHECK_RGB( lpp->crColors[ CARR_SELECTED_TEXT ] );
	}
	return TRUE;
}

/*
 *	Parse the SelectedBackgroundRGB command.
 */
BOOL SelBackgroundRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_SELECTED_BKGND;
		lpp->crColors[ CARR_SELECTED_BKGND ] = GetSysColor( COLOR_HIGHLIGHT );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_SELECTED_BKGND;
		CHECK_RGB( lpp->crColors[ CARR_SELECTED_BKGND ] );
	}
	return TRUE;
}

/*
 *	Parse the LineNumberRGB command.
 */
BOOL LineNumberRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_LINE_NUMBERS;
		lpp->crColors[ CARR_LINE_NUMBERS ] = RGB( 0, 128, 128 );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_LINE_NUMBERS;
		CHECK_RGB( lpp->crColors[ CARR_LINE_NUMBERS ] );
	}
	return TRUE;
}

/*
 *	Parse the BkgndLineNumberRGB command.
 */
BOOL BkgndLineNumberRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_LINE_NUMBERS_BKGND;
		lpp->crColors[ CARR_BACKGROUND_LINE_NUMBERS ] = RGB( 0, 128, 128 );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_LINE_NUMBERS_BKGND;
		CHECK_RGB( lpp->crColors[ CARR_BACKGROUND_LINE_NUMBERS ] );
	}
	return TRUE;
}

/*
 *	Parse the BkgndNumberRGB command.
 */
BOOL BkgndNumberRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_BACKGROUND_NUMBER;
		lpp->crColors[ CARR_BACKGROUND_NUMBER ] = CLR_DEFAULT;
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_BACKGROUND_NUMBER;
		CHECK_RGB( lpp->crColors[ CARR_BACKGROUND_NUMBER ] );
	}
	return TRUE;
}

/*
 *	Parse the BkgndDelimiterRGB command.
 */
BOOL BkgndDelimiterRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_BACKGROUND_DELIMITER;
		lpp->crColors[ CARR_BACKGROUND_DELIMITER ] = CLR_DEFAULT;
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_BACKGROUND_DELIMITER;
		CHECK_RGB( lpp->crColors[ CARR_BACKGROUND_DELIMITER ] );
	}
	return TRUE;
}

/*
 *	Parse the bracketmatchrgb command.
 */
BOOL BracketMatchRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_BRACKET_MATCH;
		lpp->crColors[ CARR_BRACKET_MATCH ] = LightenColor( GetSysColor( COLOR_HIGHLIGHT ), 0.70, 0.70, 0.70 );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_BRACKET_MATCH;
		CHECK_RGB( lpp->crColors[ CARR_BRACKET_MATCH ] );
	}
	return TRUE;
}

/*
 *	Parse the linehighlightrgb command.
 */
BOOL LineHighlightRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_LINE_HIGHLIGHT;
		lpp->crColors[ CARR_LINE_HIGHLIGHT ] = DarkenColor( lpp->crColors[ CARR_BACKGROUND ], 0.10, 0.10, 0.10 );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_LINE_HIGHLIGHT;
		CHECK_RGB( lpp->crColors[ CARR_LINE_HIGHLIGHT ] );
	}
	return TRUE;
}

/*
 *	Parse the hyperlinkrgb command.
 */
BOOL HyperlinkRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_HYPERLINK;
		lpp->crColors[ CARR_HYPERLINK ] = RGB( 0, 0, 255 );
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_HYPERLINK;
		CHECK_RGB( lpp->crColors[ CARR_HYPERLINK ] );
	}
	return TRUE;
}

/*
 *	Parse the BkgndHyperlinkRGB command.
 */
BOOL BkgndHyperlinkRGB( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default?
	 */
	if ( *pszArgs == _T( '*' ))
	{
		lpp->dwColorFlags |= CF_DEFAULT_BACKGROUND_HYPERLINK;
		lpp->crColors[ CARR_BACKGROUND_HYPERLINK ] = CLR_DEFAULT;
	}
	else
	{
		lpp->dwColorFlags &= ~CF_DEFAULT_BACKGROUND_HYPERLINK;
		CHECK_RGB( lpp->crColors[ CARR_BACKGROUND_HYPERLINK ] );
	}
	return TRUE;
}

/*
 *	Parse the Keywords command.
 */
BOOL Keywords( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Put us in keywords mode.
	 */
	bInKeyword = TRUE;
	crBkColor = CLR_DEFAULT;
	return TRUE;
}

/*
 *	Parse the EndKeywords command.
 */
BOOL EndKeywords( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	In keyword mode?
	 */
	if ( bInKeyword == FALSE )
	{
		ErrorMsg( GetString( IDS_ENDKEYWORDS_NO_KEYWORDS ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	No longer in keyword mode.
	 */
	bInKeyword = FALSE;

	/*
	 *	Reset color value.
	 */
	crColor = GetSysColor( COLOR_WINDOWTEXT );
	return TRUE;
}

/*
 *	Parse the Tab command.
 */
BOOL Tab( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	int		nTabSize;

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get tab size.
	 */
	if ( _stscanf( pszArgs, _T("%ld"), &nTabSize ) != 1 )
	{
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	In range?
	 */
	if ( nTabSize < 2 || nTabSize > 24 )
	{
		ErrorMsg( GetString( IDS_TAB_OUT_OF_RANGE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
 		return FALSE;
	}

	/*
	 *	Save tab size.
	 */
	lpp->nTabSize = nTabSize;

	return TRUE;
}

/*
 *	Parse the MaxTrack command.
 */
BOOL MaxTrack( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	int		nMaxTrack;

	/*
	 *	Default?
	 */
	if ( lpp->bDefault == FALSE )
	{
		ErrorMsg( GetString( IDS_ONLY_IN_DEFAULT ), nLineNum, lpszFileName );
		return FALSE;
	}

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get tab size.
	 */
	if ( _stscanf( pszArgs, _T("%ld"), &nMaxTrack ) != 1 )
	{
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	In range?
	 */
	if ( nMaxTrack < 10 || nMaxTrack > 30 )
	{
		ErrorMsg( GetString( IDS_TRACK_OUT_OF_RANGE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
 		return FALSE;
	}

	/*
	 *	Save track size.
	 */
	lpp->nMaxTrack = nMaxTrack;

	return TRUE;
}

/*
 *	Parse the Escape command.
 */
BOOL EscapeCom( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get escape character.
	 */
	lpp->cEscape = *pszArgs;
	return TRUE;
}

/*
 *	Parse the smartindentleft/right command.
 */
BOOL Indent( BOOL bLeft, POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	LPSMART		lps;

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Allocate node.
	 */
	if (( lps = AllocPooled( pMemPool, sizeof( SMART ))) != NULL )
	{
		/*
		 *	Copy the name string.
		 */
		if (( lps->lpszSmartString = CopyStringPool( pMemPool, pszArgs )) == NULL )
		{
			FreePooled( pMemPool, lps );
			ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
			bErrorMsg = TRUE;
			return FALSE;
		}

		/*
		 *	remove spaces from the end.
		 */
		RemoveTrailingBlanks( lps->lpszSmartString );
		
		/*
		 *	Setup direction.
		 */
		lps->nDirection = bLeft ? INDENT_LEFT : INDENT_RIGHT;

		/*
		 *	Add it to the list.
		 */
		AddTail(( LPLIST )&lpp->lSmartIndention, ( LPNODE )lps );
	}
	else
	{
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	return TRUE;
}

BOOL IndentLeft( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return Indent( TRUE, pMemPool, lpp, nLineNum, lpszFileName, pszArgs );
}

BOOL IndentRight( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return Indent( FALSE, pMemPool, lpp, nLineNum, lpszFileName, pszArgs );
}

/*
 *	Parse the default command.
 */
BOOL Default( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bDefault, pszArgs );
	return TRUE;
}

/*
 *	Parse the filemode command.
 */
BOOL FileMode( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	int		nFileMode;

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get tab size.
	 */
	if ( _stscanf( pszArgs, _T("%ld"), &nFileMode ) != 1 )
	{
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	In range?
	 */
	if ( nFileMode < FMODE_MSDOS || nFileMode > FMODE_MACINTOSH )
	{
		ErrorMsg( GetString( IDS_FILEMODE_OUT_OF_RANGE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
 		return FALSE;
	}

	/*
	 *	Save file mode.
	 */
	lpp->nFileMode = nFileMode;

	return TRUE;
}

/*
 *	Parse the carettype command.
 */
BOOL CaretType( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	int		nCaretType;

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get tab size.
	 */
	if ( _stscanf( pszArgs, _T("%ld"), &nCaretType ) != 1 )
	{
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	In range?
	 */
	if ( nCaretType < CARET_NORMAL || nCaretType > CARET_HORIZONTAL )
	{
		ErrorMsg( GetString( IDS_CARET_OUT_OF_RANGE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
 		return FALSE;
	}

	/*
	 *	Save caret type.
	 */
	lpp->nCaretType = nCaretType;

	return TRUE;
}

/*
 *	Parse the undosize command.
 */
BOOL UndoSize( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	int		nUndoSize;

	/*
	 *	Default?
	 */
	if ( lpp->bDefault == FALSE )
	{
		ErrorMsg( GetString( IDS_ONLY_IN_DEFAULT ), nLineNum, lpszFileName );
		return FALSE;
	}

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get tab size.
	 */
	if ( _stscanf( pszArgs, _T("%ld"), &nUndoSize ) != 1 )
	{
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	In range?
	 */
	if ( nUndoSize < 100 )
	{
		ErrorMsg( GetString( IDS_UNDO_OUT_OF_RANGE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
 		return FALSE;
	}

	/*
	 *	Save undo size.
	 */
	lpp->nUndoSize = nUndoSize;

	return TRUE;
}

/*
 *	Parse the dateformat command.
 */
BOOL DateFormat( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszDateFormat );
}

/*
 *	Parse the timeformat command.
 */
BOOL TimeFormat( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszTimeFormat );
}

/*
 *	Parse the key command.
 */
BOOL Key( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	LPTSTR		pszKeyDesc;
	BOOL		bRC = FALSE;

	/*
	 *	Already in key mopde?
	 */
	if ( bKey == TRUE )
	{
		ErrorMsg( GetString( IDS_ALREADY_IN_KEY ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Copy the key string.
	 */
	if (( pszKeyDesc = CopyStringPool( pMemPool, pszArgs )) != NULL )
	{
		/*
		 *	remove spaces from the end.
		 */
		RemoveTrailingBlanks( pszKeyDesc );

		/*
		 *	Create the command list.
		 */
		if (( lpCList = AllocPooled( pMemPool, sizeof( CLIST ))) != NULL )
		{
			/*
			 *	Initialize.
			 */
			NewList(( LPLIST )lpCList );

			/*
			 *	Convert the key string to a code
			 *	and qualifier.
			 */
			if ( Str2CodeQual( pszKeyDesc, &cCode, &cQual ) == TRUE )
				/*
				 *	We are in key mode.
				 */
				bRC = bKey = TRUE;
			else {
				/*
				 *	Can't convert...
				 */
				ErrorMsg( GetString( IDS_INVALID_KEYDESC ), nLineNum, lpszFileName );
				bErrorMsg = TRUE;
				FreePooled( pMemPool, lpCList );
				lpCList = NULL;
			}
		}
		FreePooled( pMemPool, pszKeyDesc );
	}
	return bRC;
}

/*
 *	Parse the endkey command.
 */
BOOL EndKey( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	In key mode?
	 */
	if ( bKey == FALSE )
	{
		ErrorMsg( GetString( IDS_ENDKEY_NO_KEY ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	No longer in key mode.
	 */
	bKey = FALSE;

	/*
	 *	Add the hash to the hash table.
	 */
	if ( AddHash( pMemPool, &lpp->aHash[ 0 ], cCode, cQual, lpCList ) == NULL )
	{
		KillCList( lpCList, pMemPool );
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Clear pointer.
	 */
	lpCList = NULL;

	return TRUE;
}

/*
 *	Parse the hardcoded command.
 */
BOOL Hardcoded( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	LPCNODE		lpCNode;
	int		nCode;

	/*
	 *	In key mode?
	 */
	if ( bKey == FALSE )
	{
		ErrorMsg( GetString( IDS_ONLY_IN_KEYMODE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Allocate a node.
	 */
	if (( lpCNode = AllocPooled( pMemPool, sizeof( CNODE ))) != NULL )
	{
		/*
		 *	Read the command code.
		 */
		if ( _stscanf( pszArgs, _T("%d"), &nCode ) == 1 )
		{
			/*
			 *	Command valid?
			 */
			if (( lpCNode->lpFunc = FindCommand( nCode )) != NULL )
			{
				/*
				 *	Yes. Add the node.
				 */
				AddTail(( LPLIST )lpCList, ( LPNODE )lpCNode );
				return TRUE;
			}
			else
			{
				ErrorMsg( GetString( IDS_UNKNOWN_CODE ), nLineNum, lpszFileName );
				bErrorMsg = TRUE;
			}
		}
		else
		{
			ErrorMsg( GetString( IDS_INVALIDARG ), nLineNum, lpszFileName );
			bErrorMsg = TRUE;
		}
	}
	else
	{
		ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
		bErrorMsg = TRUE;
	}

	return FALSE;
}

/*
 *	Parse the run/inserttext/open command.
 */
BOOL RunInsert( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs, UINT uType )
{
	LPCNODE		lpCNode;

	/*
	 *	In key mode?
	 */
	if ( bKey == FALSE )
	{
		ErrorMsg( GetString( IDS_ONLY_IN_KEYMODE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Allocate a node.
	 */
	if (( lpCNode = AllocPooled( pMemPool, sizeof( CNODE ))) != NULL )
	{
		/*
		 *	Copy arguuments.
		 */
		if (( lpCNode->pcStr = CopyStringPool( pMemPool, pszArgs )) != NULL )
		{
			/*
			 *	Clear balnks.
			 */
			RemoveTrailingBlanks( lpCNode->pcStr );

			/*
			 *	Setup type.
			 */
			lpCNode->nType = uType;

			/*
			 *	Add the node.
			 */
			AddTail(( LPLIST )lpCList, ( LPNODE )lpCNode );
			return TRUE;
		}
		FreePooled( pMemPool, lpCNode );
	}
	ErrorMsg( GetString( IDS_OUTOFMEM ), lpszFileName );
	bErrorMsg = TRUE;
	return FALSE;
}

/*
 *	Parse the open command.
 */
BOOL Open( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return RunInsert( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, CTYPE_SHELLOPEN );
}

/*
 *	Parse the run command.
 */
BOOL Run( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return RunInsert( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, CTYPE_RUN );
}

/*
 *	Parse the inserttext command.
 */
BOOL IText( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return RunInsert( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, CTYPE_TEXT );
}

/*
 *	Parse the syntaxcoloring command.
 */
BOOL SyntaxColoring( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bSyntaxColoring, pszArgs );
	return TRUE;
}

/*
 *	Parse the globalsyntaxcoloring command.
 */
BOOL GlobalSyntax( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Default?
	 */
	if ( lpp->bDefault == FALSE )
	{
		ErrorMsg( GetString( IDS_ONLY_IN_DEFAULT ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bGlobalSyntaxColoring, pszArgs );
	return TRUE;
}

/*
 *	Parse the printsyntaxcoloring command.
 */
BOOL PrintSyntax( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bPrintSyntaxColoring, pszArgs );
	return TRUE;
}

/*
 *	Parse the printheader command.
 */
BOOL PrintHeader( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bPrintHeader, pszArgs );
	return TRUE;
}

/*
 *	Parse the printfooter command.
 */
BOOL PrintFooter( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bPrintFooter, pszArgs );
	return TRUE;
}

/*
 *	Parse the printscreenfont command.
 */
BOOL PrintScreenFont( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bPrintScreenFont, pszArgs );
	return TRUE;
}

/*
 *	Parse the fastscroll command.
 */
BOOL FastScroll( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bFastScroll, pszArgs );
	return TRUE;
}

/*
 *	Parse the clearundoaftersave command.
 */
BOOL ClearUndoSave( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bClearUndoSave, pszArgs );
	return TRUE;
}

/*
 *	Parse the visiblespaces command.
 */
BOOL VisibleSpaces( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bVisibleSpaces, pszArgs );
	return TRUE;
}

/*
 *	Parse the showlinenumbers command.
 */
BOOL ShowLineNumbers( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bLineNumbers, pszArgs );
	return TRUE;
}

/*
 *	Parse the casefixing command.
 */
BOOL CaseFixing( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bCaseFixing, pszArgs );
	return TRUE;
}

/*
 *	Parse the solidmargin command.
 */
BOOL SolidMargin( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bSolidMargin, pszArgs );
	return TRUE;
}

/*
 *	Parse the colorbracketmatches command.
 */
BOOL ColorBrackets( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bColorBracketMatches, pszArgs );
	return TRUE;
}

/*
 *	Parse the highlightline command.
 */
BOOL HighlightLine( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bHighlightCurrentLine, pszArgs );
	return TRUE;
}

/*
 *	Parse the printlinenumbers command.
 */
BOOL PrintLineNumbers( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bPrintLineNumbers, pszArgs );
	return TRUE;
}

/*
 *	Parse the parsehyperlinks command.
 */
BOOL ParseHyperlinks( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bParseHyperLinks, pszArgs );
	return TRUE;
}

/*
 *	Parse the showhypertips command.
 */
BOOL ShowHyperTips( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bShowHyperTips, pszArgs );
	return TRUE;
}

/*
 *	Parse the backupfiles command.
 */
BOOL BackupFiles( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bBackup, pszArgs );
	return TRUE;
}

/*
 *	Parse the backupdirectory command.
 */
BOOL BackupDirectory( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszBackupPath );
}

/*
 *	Parse the backupmode command.
 */
BOOL BackupMode( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	int		nBackupMode;

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get backup mode.
	 */
	if ( _stscanf( pszArgs, _T("%ld"), &nBackupMode ) != 1 )
	{
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	In range?
	 */
	if ( nBackupMode < BMODE_FILE_BAK || nBackupMode > BMODE_FILE_EXT_PATH )
	{
		ErrorMsg( GetString( IDS_BACKUPMODE_OUT_OF_RANGE ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
 		return FALSE;
	}

	/*
	 *	Save backup mode.
	 */
	lpp->nBackupMode = nBackupMode;

	return TRUE;
}

/*
 *	Parse the screenfont or printerfont command.
 */
BOOL Font( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs, BOOL bPrinter )
{
	LOGFONT		lf = { 0 }, lfUL = { 0 };
	HFONT		hFont, hUnderlined;
	int		i, nCharSet, nItalic, nUnderline;

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Default the structure.
	 */
	lf.lfHeight	    = 0;
	lf.lfWeight	    = FW_NORMAL;
	lf.lfCharSet	    = ANSI_CHARSET;
	lf.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lf.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lf.lfQuality	    = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;

	/*
	 *	Extract the font name.
	 */
	for ( i = 0; ; i++ )
	{
		/*
		 *	Done?
		 */
		if ( pszArgs[ i ] == _T( ',' ) || pszArgs[ i ] == 0 )
			break;

		/*
		 *	Store character.
		 */
		lf.lfFaceName[ i ] = pszArgs[ i ];
	}

	/*
	 *	More args?
	 */
	if ( pszArgs[ i ] == _T( ',' ))
	{
		/*
		 *	Skip to next character.
		 */
		i++;

		/*
		 *	Read the font height, weight, charset, italic and underline.
		 */
		if ( _stscanf( &pszArgs[ i ], _T("%ld,%ld,%ld,%ld,%ld"), &lf.lfHeight, &lf.lfWeight, &nCharSet, &nItalic, &nUnderline ) != 5 )
		{
			ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
			bErrorMsg = TRUE;
			return FALSE;
		}

		/*
		 *	Save values.
		 */
		lf.lfCharSet   = ( BYTE )nCharSet;
		lf.lfItalic    = ( BYTE )nItalic;
		lf.lfUnderline = ( BYTE )nUnderline;
	}

	/*
	 *	Create underlined version.
	 */
	lfUL = lf;
	lfUL.lfUnderline = TRUE;

	/*
	 *	Create new font.
	 */
	if (( hFont = CreateFontIndirect( &lf )) == NULL )
	{
		ErrorMsg( GetString( IDS_UNKNOWN_FONT ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	Screen or printer?
	 */
	if ( bPrinter )
	{
		/*
		 *	Destroy old font and
		 *	setup new.
		 */
		if ( lpp->hPrinterFont ) DeleteObject( lpp->hPrinterFont );
		lpp->hPrinterFont = hFont;
		memcpy( &lpp->lfPrinterFont, &lf, sizeof( LOGFONT ));
	}
	else
	{
		/*
		 *	Create underlined version
		 *	of the font.
		 */
		hUnderlined = CreateFontIndirect( &lfUL );
		if ( hUnderlined )
		{
			/*
			 *	Destroy old fonts and
			 *	setup new.
			 */
			if ( lpp->hScreenFont   ) DeleteObject( lpp->hScreenFont   );
			if ( lpp->hScreenFontUL ) DeleteObject( lpp->hScreenFontUL );
			lpp->hScreenFont = hFont;
			lpp->hScreenFontUL = hUnderlined;
			memcpy( &lpp->lfScreenFont, &lf, sizeof( LOGFONT ));
			memcpy( &lpp->lfScreenFontUL, &lfUL, sizeof( LOGFONT ));
		}
		else
		{
			/*
			 *	Font creation failed.
			 */
			DeleteObject( hFont );
			ErrorMsg( GetString( IDS_UNKNOWN_FONT ), nLineNum, lpszFileName );
			bErrorMsg = TRUE;
			return FALSE;
		}

		/*
		 *	Unknown character size.
		 */
		lpp->szCharSize.cx = lpp->szCharSize.cy = -1;
	}
	return TRUE;
}

BOOL ScreenFont( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return Font( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, FALSE );
}

BOOL PrinterFont( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return Font( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, TRUE );
}

/*
 *	Parser the header commands.
 */
BOOL HeaderLeft( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszHeaderLeft );
}

BOOL HeaderCenter( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszHeaderCenter );
}

BOOL HeaderRight( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszHeaderRight );
}

/*
 *	Parser the footer commands.
 */
BOOL FooterLeft( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszFooterLeft );
}

BOOL FooterCenter( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszFooterCenter );
}

BOOL FooterRight( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	return ReadString( pMemPool, lpp, nLineNum, lpszFileName, pszArgs, &lpp->pszFooterRight );
}

/*
 *	Parse the columnmarker command.
 */
BOOL ColMarker( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get argument.
	 */
	CHECK_BOOLEAN( lpp->bColumnMarker, pszArgs );
	return TRUE;
}

/*
 *	Parse the columnmarkeroffset command.
 */
BOOL ColMarkerOffset( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszArgs )
{
	int		nColOffset;

	/*
	 *	Check for arguments.
	 */
	CHECK_ARGS( pszArgs );

	/*
	 *	Get tab offset.
	 */
	if ( _stscanf( pszArgs, _T("%ld"), &nColOffset ) != 1 )
	{
		ErrorMsg( GetString( IDS_PARSING_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
		return FALSE;
	}

	/*
	 *	In range?
	 */
	if ( nColOffset < 0 || nColOffset > 256 )
	{
		ErrorMsg( GetString( IDS_COLOFFSET_ERROR ), nLineNum, lpszFileName );
		bErrorMsg = TRUE;
 		return FALSE;
	}

	/*
	 *	Save the offset.
	 */
	lpp->nMarkerOffset = nColOffset;

	return TRUE;
}

typedef BOOL ( *LPCOMMFUNC )( POOL, LPPARSER, int, LPCTSTR, TCHAR * );
typedef struct {
	LPCTSTR		lpszCommand;
	int		nLength;
	LPCOMMFUNC	lpfnFunc;
} COMM, FAR *LPCOMM;

/*
 *	Configuration file commands.
 */
COMM CommandTable[] = {
	/*
	 *	Name strings.
	 */
	{ _T("Name="),			5,		Name			},
	{ _T("Extentions="),		11,		Extentions		},
	{ _T("Delimiters="),		11,		Delimiters		},
	
	/*
	 *	Blocks.
	 */
	{ _T("Block="),			6,		BlockIs			},
	{ _T("Block"),			5,		Block			},
	{ _T("EndBlock"),		8,		EndBlock		},
	{ _T("Start="),			6,		Start			},
	{ _T("StartOfLine="),		12,		StartOfLine		},
	{ _T("End="),			4,		End			},
	
	/*
	 *	Keywords.
	 */
	{ _T("Keywords"),		8,		Keywords		},
	{ _T("EndKeywords"),		11,		EndKeywords		},

	/*
	 *	Booleans.
	 */
	{ _T("Case="),			5,		Case			},
	{ _T("AutoIndent="),		11,		AutoIndent		},
	{ _T("SmartIndent="),		12,		SmartIndent		},	
	{ _T("SelectionMargin="),	16,		SelectionMargin		},
	{ _T("ClearSpaces="),		12,		ClearSpaces		},
	{ _T("AutoMatch="),		10,		AutoMatch		},
	{ _T("WholeWords="),		11,		WholeWords		},
	{ _T("SearchCase="),		11,		SearchCase		},
	{ _T("VisibleAutoMatchError="),	22,		VisibleMatch		},
	{ _T("Default="),		8,		Default			},
	{ _T("DnDEnabled="),		11,		DnDEnabled		},
	{ _T("SyntaxColoring="),	15,		SyntaxColoring		},
	{ _T("GlobalSyntaxColoring="),	21,		GlobalSyntax		},
	{ _T("PrintSyntaxColoring="),	20,		PrintSyntax		},
	{ _T("PrintHeader="),		12,		PrintHeader		},
	{ _T("PrintFooter="),		12,		PrintFooter		},
	{ _T("PrintScreenFont="),	16,		PrintScreenFont		},
	{ _T("FastScroll="),		11,		FastScroll		},
	{ _T("ClearUndoAfterSave="),	19,		ClearUndoSave		},
	{ _T("VisibleSpaces="),		14,		VisibleSpaces		},
	{ _T("ShowLineNumbers="),	16,		ShowLineNumbers		},
	{ _T("CaseFixing="),		11,		CaseFixing		},
	{ _T("SolidMargin="),		12,		SolidMargin		},
	{ _T("ColorBracketMatches="),	20,		ColorBrackets		},
	{ _T("HighlightLine="),		14,		HighlightLine		},
	{ _T("PrintLineNumbers="),	17,		PrintLineNumbers	},
	{ _T("ParseHyperlinks="),	16,		ParseHyperlinks		},
	{ _T("ShowHyperTips="),		14,		ShowHyperTips		},

	/*
	 *	RGB color values.
	 */
	{ _T("RGB="),			4,		RGBCom			},
	{ _T("BkRGB="),			6,		BkRGBCom		},
	{ _T("TextRGB="),		8,		TextRGB			},
	{ _T("BackgroundRGB="),		14,		BackgroundRGB		},
	{ _T("BackgroundReadOnlyRGB="),	22,		BackgroundReadOnlyRGB	},
	{ _T("MarginRGB="),		10,		MarginRGB		},
	{ _T("BookmarkRGB="),		12,		BookmarkRGB		},
	{ _T("DelimiterRGB="),		13,		DelimiterRGB		},
	{ _T("NumberRGB="),		10,		NumberRGB		},
	{ _T("SelectedTextRGB="),	16,		SelTextRGB		},
	{ _T("SelectedBackgroundRGB="),	22,		SelBackgroundRGB	},
	{ _T("LineNumberRGB="),		14,		LineNumberRGB		},
	{ _T("BkgndLineNumberRGB="),	19,		BkgndLineNumberRGB	},
	{ _T("BkgndNumberRGB="),	15,		BkgndNumberRGB		},
	{ _T("BkgndDelimiterRGB="),	18,		BkgndDelimiterRGB	},
	{ _T("BracketMatchRGB="),	16,		BracketMatchRGB		},
	{ _T("LineHighlightRGB="),	17,		LineHighlightRGB	},
	{ _T("HyperlinkRGB="),		13,		HyperlinkRGB		},
	{ _T("BkgndHyperlinkRGB="),	18,		BkgndHyperlinkRGB	},

	/*
	 *	Miscellanious.
	 */
	{ _T("Tab="),			4,		Tab			},
	{ _T("Escape="),		7,		EscapeCom		},
	{ _T("MaxTrack="),		9,		MaxTrack		},
	{ _T("IndentLeft="),		11,		IndentLeft		},
	{ _T("IndentRight="),		12,		IndentRight		},
	{ _T("CaretType="),		10,		CaretType		},
	{ _T("FileMode="),		9,		FileMode		},
	{ _T("DateFormat="),		11,		DateFormat		},
	{ _T("TimeFormat="),		11,		TimeFormat		},
	{ _T("ScreenFont="),		11,		ScreenFont		},
	{ _T("PrinterFont="),		12,		PrinterFont		},
	{ _T("HeaderLeft="),		11,		HeaderLeft		},
	{ _T("HeaderCenter="),		13,		HeaderCenter		},
	{ _T("HeaderRight="),		12,		HeaderRight		},
	{ _T("FooterLeft="),		11,		FooterLeft		},
	{ _T("FooterCenter="),		13,		FooterCenter		},
	{ _T("FooterRight="),		12,		FooterRight		},

	/*
	 *	Backup.
	 */
	{ _T("BackupFiles="),		12,		BackupFiles		},
	{ _T("BackupDirectory="),	16,		BackupDirectory		},
	{ _T("BackupMode="),		11,		BackupMode		},

	/*
	 *	Column marker.
	 */
	{ _T("ColumnMarkerOffset="),	19,		ColMarkerOffset		},
	{ _T("ColumnMarker="),		13,		ColMarker		},

	/*
	 *	Keyboard.
	 */
	{ _T("Key="),			4,		Key			},
	{ _T("EndKey"),			6,		EndKey			},
	{ _T("Hardcoded="),		10,		Hardcoded		},
	{ _T("Run="),			4,		Run			},
	{ _T("Open="),			5,		Open			},
	{ _T("InsertText="),		11,		IText			},

	/*
	 *	Only allowed in the default
	 *	parser file.
	 */
	{ _T("UndoSize="),		9,		UndoSize		},

	{ NULL,				0,		NULL			}
};

BOOL ParseCommand( POOL pMemPool, LPPARSER lpp, int nLineNum, LPCTSTR lpszFileName, TCHAR *pszCommand )
{
	LPCOMM		lpc = &CommandTable[ 0 ];

	/*
	 *	Scan commands.
	 */
	while ( lpc->lpszCommand )
	{
		/*
		 *	Is this the one?
		 */
		if ( _tcsnicmp( pszCommand, lpc->lpszCommand, lpc->nLength ) == 0 )
			/*
			 *	Execute it's parser.
			 */
			return ( lpc->lpfnFunc )( pMemPool, lpp, nLineNum, lpszFileName, pszCommand + lpc->nLength );
		/*
		 *	Next...
		 */
		lpc++;
	}

	/*
	 *	Not found...
	 */
	return FALSE;
}

void DefaultParser( LPPARSER lpp )
{
	int		i;

	/*
	 *	Setup defaults.
	 */
	lpp->bAutoIndent	  = TRUE;
	lpp->bSmartIndent	  = FALSE;
	lpp->bVisibleMatchError	  = FALSE;
	lpp->bAutoMatch		  = FALSE;
	lpp->bCaseOn		  = FALSE;
	lpp->bSelectionMargin	  = TRUE;
	lpp->bClearTrailingSpaces = TRUE;
	lpp->bSearchCase	  = FALSE;
	lpp->bWholeWords	  = FALSE;
	lpp->bDefault		  = FALSE;
	lpp->bDnDEnabled	  = TRUE;
	lpp->bSyntaxColoring	  = TRUE;
	lpp->bPrintSyntaxColoring = TRUE;
	lpp->bPrintHeader	  = FALSE;
	lpp->bPrintFooter	  = FALSE;
	lpp->bPrintScreenFont	  = TRUE;
	lpp->bFastScroll	  = TRUE;
	lpp->bClearUndoSave	  = FALSE;
	lpp->bVisibleSpaces	  = FALSE;
	lpp->bLineNumbers	  = FALSE;
	lpp->nTabSize		  = 8;
	lpp->cEscape		  = 0;
	lpp->nFileMode		  = FMODE_MSDOS;
	lpp->nCaretType		  = CARET_WIDE;
	lpp->nFilterIndex	  = 1;
	lpp->bHasBeenAdded	  = FALSE;
	lpp->bCaseFixing	  = FALSE;
	lpp->bSolidMargin	  = FALSE;
	lpp->nMarkerOffset	  = 80;
	lpp->bColumnMarker	  = FALSE;
	lpp->bColorBracketMatches = FALSE;
	lpp->bHighlightCurrentLine= FALSE;
	lpp->nHighlighted	  = 0;
	lpp->bPrintLineNumbers	  = FALSE;
	lpp->bParseHyperLinks	  = FALSE;
	lpp->bShowHyperTips	  = FALSE;

	/*
	 *	Setup default colors.
	 */
	lpp->crColors[ CARR_TEXT ]		      = GetSysColor( COLOR_WINDOWTEXT );
	lpp->crColors[ CARR_BACKGROUND ]	      = GetSysColor( COLOR_WINDOW );
	lpp->crColors[ CARR_BACKGROUND_READONLY ]     = GetSysColor( COLOR_3DLIGHT );
	lpp->crColors[ CARR_SELECTION_MARGIN ]	      = GetSysColor( COLOR_SCROLLBAR );
	lpp->crColors[ CARR_BOOKMARK ]		      = RGB( 0, 255, 255 );
	lpp->crColors[ CARR_NUMBER ]		      = GetSysColor( COLOR_WINDOWTEXT );
	lpp->crColors[ CARR_DELIMITER ]		      = GetSysColor( COLOR_WINDOWTEXT );
	lpp->crColors[ CARR_SELECTED_TEXT ]	      = GetSysColor( COLOR_HIGHLIGHTTEXT );
	lpp->crColors[ CARR_SELECTED_BKGND ]	      = GetSysColor( COLOR_HIGHLIGHT );
	lpp->crColors[ CARR_LINE_NUMBERS ]	      = RGB( 0, 128, 128 );
	lpp->crColors[ CARR_BACKGROUND_LINE_NUMBERS ] = GetSysColor( COLOR_WINDOW );
	lpp->crColors[ CARR_BACKGROUND_NUMBER ]	      = CLR_DEFAULT;
	lpp->crColors[ CARR_BACKGROUND_DELIMITER ]    = CLR_DEFAULT;
	lpp->crColors[ CARR_BRACKET_MATCH ]	      = LightenColor( GetSysColor( COLOR_HIGHLIGHT ), 0.70, 0.70, 0.70 );
	lpp->crColors[ CARR_LINE_HIGHLIGHT ]	      = DarkenColor( lpp->crColors[ CARR_BACKGROUND ], 0.10, 0.10, 0.10 );
	lpp->crColors[ CARR_HYPERLINK ]		      = RGB( 0, 0, 255 );
	lpp->crColors[ CARR_BACKGROUND_HYPERLINK ]    = CLR_DEFAULT;
	
	/*
	 *	Clear hashes.
	 */
	for ( i = 0; i < HASHSIZE; i++ )
		lpp->aHash[ i ] = NULL;
	for ( i = 0; i < 256; i++ )
		lpp->aKeywords[ i ] = NULL;

	/*
	 *	Setup backup defaults.
	 */
	lpp->bBackup       = FALSE;
	lpp->pszBackupPath = NULL;
	lpp->nBackupMode   = BMODE_FILE_BAK;

	/*
	 *	Use defaults.
	 */
	lpp->dwColorFlags = CF_DEFAULT_COLORS;

	/*
	 *	Setup fonts.
	 */
	lpp->lfScreenFont.lfHeight	   = 0;
	lpp->lfScreenFont.lfWeight	   = FW_NORMAL;
	lpp->lfScreenFont.lfCharSet	   = ANSI_CHARSET;
	lpp->lfScreenFont.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lpp->lfScreenFont.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lpp->lfScreenFont.lfQuality	   = DEFAULT_QUALITY;
	lpp->lfScreenFont.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	_tcscpy( lpp->lfScreenFont.lfFaceName, _T("Courier New"));

	lpp->lfScreenFontUL.lfHeight	     = 0;
	lpp->lfScreenFontUL.lfUnderline	     = TRUE;
	lpp->lfScreenFontUL.lfWeight	     = FW_NORMAL;
	lpp->lfScreenFontUL.lfCharSet	     = ANSI_CHARSET;
	lpp->lfScreenFontUL.lfOutPrecision   = OUT_DEFAULT_PRECIS;
	lpp->lfScreenFontUL.lfClipPrecision  = CLIP_DEFAULT_PRECIS;
	lpp->lfScreenFontUL.lfQuality	     = DEFAULT_QUALITY;
	lpp->lfScreenFontUL.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	_tcscpy( lpp->lfScreenFontUL.lfFaceName, _T("Courier New"));


	/*
	 *	Character size unknown.
	 */
	lpp->szCharSize.cx = lpp->szCharSize.cy = -1;

	/*
	 *	Make the printer font the
	 *	same.
	 */
	memcpy( &lpp->lfPrinterFont, &lpp->lfScreenFont, sizeof( LOGFONT ));

	/*
	 *	Only supported when bDefault
	 *	is set to TRUE.
	 */
	lpp->nUndoSize		   = 1000;
	lpp->bGlobalSyntaxColoring = TRUE;
	lpp->nMaxTrack		   = 20;

	/*
	 *	Initialize smart indention list.
	 */
	NewList(( LPLIST )&lpp->lSmartIndention );
}

/* 
 *	Load a parser file from the given file name.
 */
BOOL LoadParserFromFile( LPCTSTR lpszFileName )
{
	return LoadParserFile( pMainPool, &plParsers, lpszFileName, INVALID_HANDLE_VALUE, TRUE );
}

/*
 *	Load a parser from the memory handle.
 */
BOOL LoadParserFromMemory( HGLOBAL hGlobal )
{
	return LoadParserFile( pMainPool, &plParsers, NULL, hGlobal, TRUE );
}

/*
 *	Load parser file.
 */
LRESULT OnLoadParser( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Load the file either from memory
	 *	or form a file.
	 */
	if ( wParam ) 
		return LoadParserFile( pMainPool, &plParsers, NULL, ( HGLOBAL )lParam, TRUE );
	return LoadParserFile( pMainPool, &plParsers, ( LPCTSTR )lParam, INVALID_HANDLE_VALUE, TRUE );
}

/*
 *	Load the parser file either from the file
 *	specified by the file name or from the
 *	global memory handle hGlobal.
 *
 *	The code which parses the file contained in
 *	the global memory handle was originally written
 *	by Ernest Laurentin.
 */
BOOL LoadParserFile( POOL pMemPool, LPPARSERLIST lppl, LPCTSTR lpszFileName, HGLOBAL hGlobal, BOOL bAdded )
{
	LPPARSER	 lpp = NULL;
	FILE		*file = NULL;
	TCHAR		 szBuffer[ 1024 ], *pszPtr, *pszResData = NULL;
	int		 nLineNum = 0, nIndex = 0;
	DWORD		 dwResPos = 0;
	DWORD		 dwResSize = 0;

	/*
	 *	Load from the file or the
	 *	global memory handle?
	 */
	if ( hGlobal && hGlobal != INVALID_HANDLE_VALUE )
	{
		/*
		 *	Obtain the size of the memorychunk
		 *	the handle represents.
		 */
		if (( dwResSize = GlobalSize( hGlobal )) == 0L )
			return FALSE;

		/*
		 *	Lock the memory.
		 */
		if (( pszResData = ( TCHAR * )GlobalLock( hGlobal )) == NULL )
			return FALSE;
	}
	else
	{
		/*
		 *	Filename passed?
		 */
		if ( ! lpszFileName || ! _tcslen( lpszFileName ))
			return FALSE;
	}

	/*
	 *	Preset values.
	 */
	crColor = GetSysColor( COLOR_WINDOWTEXT );
	bError = bInBlock = bInKeyword = FALSE;
	memset( &bBlock, 0, sizeof( BLOCK ));

	/*
	 *	Simple loop. This allows me to issue a break
	 *	for easier error handling...
	 */
	for ( ;; )
	{
		/*
		 *	Open the file when we are loading
		 *	from a file.
		 */
		if ( pszResData == NULL )
		{
			if (( file = _tfopen( lpszFileName, _T( "ra" ))) == NULL ) {
				bError = TRUE;
				break;
			}
		}

		/*
		 *	Allocate parser node.
		 */
		if (( lpp = AllocPooled( pMemPool, sizeof( PARSER ))) == NULL ) 
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Allocate block array.
		 */
		if (( lpp->lpaBlocks = ArrayCreate( 0, 5, sizeof( BLOCK ))) == NULL )
		{
			bError = TRUE;
			break;
		}

		/*
		 *	Setup defaults.
		 */
		DefaultParser( lpp );

		/*
		 *	Added parser?
		 */
		lpp->bHasBeenAdded = bAdded;

		/*
		 *	Save it's file name.
		 */
		if ( lpszFileName )
		{
			if (( lpp->pszFileName = CopyStringPool( pMemPool, lpszFileName )) == NULL )
			{
				bError = TRUE;
				break;
			}
		}
 
		/*
		 *	Read and parse the file.
		 */
		while ( bError == FALSE )
		{
			/*
			 *	Are we loading from a file?
			 */
			if ( file )
			{
				/*
				 *	Obtain the next line from the
				 *	file.
				 */
				if (( pszPtr = _fgetts( szBuffer, 1024, file )) == NULL )
					break;
			}
			else
			{
				/*
				 *	Did we get it all?
				 */
				if ( dwResPos >= dwResSize )
					break;

				/*
				 *	This simulates _fgetts(...) but with string buffer
				 */
				nIndex = 0;
				while( dwResPos < dwResSize )
				{
					/*
					 *	Copy the contents of the buffer
					 *	until we reach a newline.
					 */
					szBuffer[ nIndex++ ] = pszResData[ dwResPos++ ];

					/*
					 *	Was this the newline?
					 */
					if ( pszResData[ dwResPos ] == _T( '\n' ))
					{
						/*
						 *	Position index on the next line.
						 */
						dwResPos++;
						break;
					}
				}

				/*
				 *	0-terminate buffer and setup the pointer.
				 */
				szBuffer[ nIndex ] = 0;
				pszPtr = szBuffer;
			}

			/*
			 *	Increase line number.
			 */
			nLineNum++;

			/*
			 *	Skip spaces.
			 */
			SKIP_BLANKS( pszPtr );

			/*
			 *	Skip this line if it is a comment 
			 *	or when it is empty.
			 */
			if ( *pszPtr == _T( ';' ) || *pszPtr == _T( '\0' )) 
				continue;

			/*
			 *	Reset error boolean.
			 */
			bErrorMsg = FALSE;

			/*
			 *	Parse this line.
			 */
			if ( ParseCommand( pMemPool, lpp, nLineNum, lpszFileName ? lpszFileName : _T( "MemoryHandle" ), pszPtr ) == TRUE )
				/*
				 *	Continue parsing...
				 */
				continue;
			else
			{
				/*
				 *	Here we must be in keyword mode.
				 */
				if ( bInKeyword == FALSE && bErrorMsg == FALSE )
				{
					ErrorMsg( GetString( IDS_UNKNOWN_COMMAND ), nLineNum, lpszFileName );
					bError = TRUE;
					continue;
				}

				/*
				 *	Add it as a hash.
				 */
				if ( AddKeyword( pMemPool, lpp, pszPtr, crColor, crBkColor ) == FALSE )
				{
					bError = TRUE;
					continue;
				}
			}
		}
		break;
	}

	/*
	 *	Close the file or unlock
	 *	the memory.
	 */
	if ( file ) fclose( file );
	else if ( pszResData ) GlobalUnlock( hGlobal );

	/*
	 *	Make sure the fonts are
	 *	created.
	 */
	if ( lpp->hScreenFont == NULL )
	{
		if (( lpp->hScreenFont = CreateFontIndirect( &lpp->lfScreenFont )) == NULL )
			bError = TRUE;
	}
	if ( lpp->hScreenFontUL == NULL )
	{
		if (( lpp->hScreenFontUL = CreateFontIndirect( &lpp->lfScreenFontUL )) == NULL )
			bError = TRUE;
	}
	if ( lpp->hPrinterFont == NULL )
	{
		if (( lpp->hPrinterFont = CreateFontIndirect( &lpp->lfPrinterFont )) == NULL )
			bError = TRUE;
	}

	/*
	 *	Errors?
	 */
	if ( bError )
	{
		/*
		 *	Cleanup any strings that might not
		 *	be added to the node yet.
		 */
		if ( lpp->lpaBlocks )
		{
			if ( bBlock.pszName   ) ArrayFreeMem( lpp->lpaBlocks, bBlock.pszName   );
			if ( bBlock.pszStart  ) ArrayFreeMem( lpp->lpaBlocks, bBlock.pszStart  );
			if ( bBlock.pszEnd    ) ArrayFreeMem( lpp->lpaBlocks, bBlock.pszEnd    );
		}

		/*
		 *	De-allocate the parser node.
		 */
		if ( lpp )
			FreeParserNode( pMemPool, lpp );

		/*
		 *	Failure.
		 */
		return FALSE;
	}

	/*
	 *	Add the parser node to the list.
	 */
	LockParserList();
	AddTail(( LPLIST )lppl, ( LPNODE )lpp );
	UnlockParserList();

	return TRUE;
}
