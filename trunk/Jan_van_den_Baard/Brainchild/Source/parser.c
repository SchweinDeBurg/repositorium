/*
 *	parser.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Syntax coloring parser.
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	16-Bit CRC table.
 */
static WORD Crc16[ 256 ] = 
{
	0x0000,  0x1021,  0x2042,  0x3063,  0x4084,  0x50a5,  0x60c6,  0x70e7,
	0x8108,  0x9129,  0xa14a,  0xb16b,  0xc18c,  0xd1ad,  0xe1ce,  0xf1ef,
	0x1231,  0x0210,  0x3273,  0x2252,  0x52b5,  0x4294,  0x72f7,  0x62d6,
	0x9339,  0x8318,  0xb37b,  0xa35a,  0xd3bd,  0xc39c,  0xf3ff,  0xe3de,
	0x2462,  0x3443,  0x0420,  0x1401,  0x64e6,  0x74c7,  0x44a4,  0x5485,
	0xa56a,  0xb54b,  0x8528,  0x9509,  0xe5ee,  0xf5cf,  0xc5ac,  0xd58d,
	0x3653,  0x2672,  0x1611,  0x0630,  0x76d7,  0x66f6,  0x5695,  0x46b4,
	0xb75b,  0xa77a,  0x9719,  0x8738,  0xf7df,  0xe7fe,  0xd79d,  0xc7bc,
	0x48c4,  0x58e5,  0x6886,  0x78a7,  0x0840,  0x1861,  0x2802,  0x3823,
	0xc9cc,  0xd9ed,  0xe98e,  0xf9af,  0x8948,  0x9969,  0xa90a,  0xb92b,
	0x5af5,  0x4ad4,  0x7ab7,  0x6a96,  0x1a71,  0x0a50,  0x3a33,  0x2a12,
	0xdbfd,  0xcbdc,  0xfbbf,  0xeb9e,  0x9b79,  0x8b58,  0xbb3b,  0xab1a,
	0x6ca6,  0x7c87,  0x4ce4,  0x5cc5,  0x2c22,  0x3c03,  0x0c60,  0x1c41,
	0xedae,  0xfd8f,  0xcdec,  0xddcd,  0xad2a,  0xbd0b,  0x8d68,  0x9d49,
	0x7e97,  0x6eb6,  0x5ed5,  0x4ef4,  0x3e13,  0x2e32,  0x1e51,  0x0e70,
	0xff9f,  0xefbe,  0xdfdd,  0xcffc,  0xbf1b,  0xaf3a,  0x9f59,  0x8f78,
	0x9188,  0x81a9,  0xb1ca,  0xa1eb,  0xd10c,  0xc12d,  0xf14e,  0xe16f,
	0x1080,  0x00a1,  0x30c2,  0x20e3,  0x5004,  0x4025,  0x7046,  0x6067,
	0x83b9,  0x9398,  0xa3fb,  0xb3da,  0xc33d,  0xd31c,  0xe37f,  0xf35e,
	0x02b1,  0x1290,  0x22f3,  0x32d2,  0x4235,  0x5214,  0x6277,  0x7256,
	0xb5ea,  0xa5cb,  0x95a8,  0x8589,  0xf56e,  0xe54f,  0xd52c,  0xc50d,
	0x34e2,  0x24c3,  0x14a0,  0x0481,  0x7466,  0x6447,  0x5424,  0x4405,
	0xa7db,  0xb7fa,  0x8799,  0x97b8,  0xe75f,  0xf77e,  0xc71d,  0xd73c,
	0x26d3,  0x36f2,  0x0691,  0x16b0,  0x6657,  0x7676,  0x4615,  0x5634,
	0xd94c,  0xc96d,  0xf90e,  0xe92f,  0x99c8,  0x89e9,  0xb98a,  0xa9ab,
	0x5844,  0x4865,  0x7806,  0x6827,  0x18c0,  0x08e1,  0x3882,  0x28a3,
	0xcb7d,  0xdb5c,  0xeb3f,  0xfb1e,  0x8bf9,  0x9bd8,  0xabbb,  0xbb9a,
	0x4a75,  0x5a54,  0x6a37,  0x7a16,  0x0af1,  0x1ad0,  0x2ab3,  0x3a92,
	0xfd2e,  0xed0f,  0xdd6c,  0xcd4d,  0xbdaa,  0xad8b,  0x9de8,  0x8dc9,
	0x7c26,  0x6c07,  0x5c64,  0x4c45,  0x3ca2,  0x2c83,  0x1ce0,  0x0cc1,
	0xef1f,  0xff3e,  0xcf5d,  0xdf7c,  0xaf9b,  0xbfba,  0x8fd9,  0x9ff8,
	0x6e17,  0x7e36,  0x4e55,  0x5e74,  0x2e93,  0x3eb2,  0x0ed1,  0x1ef0,
};

/*
 *	Compute 16-bit CRC checksum.
 */
#define CRC16( index, crc ) ( Crc16[ (( crc ) ^ ( index )) & 0xff ] ^ (( crc ) >> 8 ))

/*
 *	Compute string hash code.
 */
WORD HashKeyword( LPCTSTR pszKeyword, int nLength, BOOL bCaseOn )
{
	register WORD	wHash = ( WORD )-1;
	register int	i;

	/*
	 *	Compute the hash, or 16 bit CRC,
	 *	of the given string.
	 */
	if ( bCaseOn )
	{
		/*
		 *	Case sensitive.
		 */
		for ( i = 0; i < nLength; i++ )
			wHash = ( WORD )CRC16( *pszKeyword++, wHash );
	}
	else
	{
		/*
		 *	Case insensitive.
		 */
		for ( i = 0; i < nLength; i++ )
			wHash = ( WORD )CRC16( _totlower( *pszKeyword++ ), wHash );
	}

	/*
	 *	Return hash code.
	 */
	return wHash;
}

/*
 *	String comparisson function.
 */
typedef int ( *CMPFUNC )( const TCHAR *, const TCHAR *, unsigned int );

/*
 *	Find a keyword from the
 *	Keyword hash table.
 */
static LPKEYHASH IsKeyword( LPCLASSDATA lpcd, LPCTSTR pszKeyword, int nLength )
{
	WORD			wHashCode = HashKeyword( pszKeyword, nLength, Parser->bCaseOn );
	register LPKEYHASH	lpHash;
	CMPFUNC			lpComp = ( Parser->bCaseOn  ? _tcsncmp : _tcsnicmp );

	/*
	 *	Get a pointer to the first hash
	 *	entry with this hash code.
	 */
	lpHash = Parser->aKeywords[ wHashCode & 255 ];

	/*
	 *	Find the correct entry.
	 */
	while ( lpHash )
	{
		/*
		 *	Check keyword.
		 */
		if ( lpHash->nLength == nLength && 
		     ! ( lpComp )( pszKeyword, lpHash->pszKeyword, nLength ))
				return lpHash;

		/*
		 *	Next please...
		 */
		lpHash = lpHash->lpNext;
	}

	/*
	 *	Not found.
	 */
	return NULL;
}

/*
 *	Is the character a delimiter?
 */
BOOL IsDelimiter( LPCLASSDATA lpcd, TCHAR cChar )
{
	int		i;

	/*
	 *	Any delimiters?
	 */
	if ( Parser == NULL || Parser->pszDelimiters == NULL )
		return FALSE;

	/*
	 *	Scan delimiters.
	 */
	for ( i = 0; ; i++ )
	{
		/*
		 *	Valid delimiter?
		 */
		if ( Parser->pszDelimiters[ i ] )
		{
			/*
			 *	Is this the one?
			 */
			if ( cChar == Parser->pszDelimiters[ i ] )
				return TRUE;
		} 
		else
			break;
	}
	return FALSE;
}

/*
 *	Is this the start of the block?
 */
static BOOL BlockStart( LPCLASSDATA lpcd, LPBLOCK lpb, TCHAR *pcText, int nIndex, int nLength, BOOL bStartOfLine )
{
	/*
	 *	Start string valid?
	 */
	if ( lpb->nStart > 0 )
	{
		/*
		 *	Is this the block initiator?
		 */
		if ( ! ( Parser->bCaseOn ? _tcsncmp : _tcsnicmp )( lpb->pszStart, &pcText[ nIndex ], lpb->nStart ))
		{
			/*
			 *	Must it be located at the
			 *	start of the line?
			 */
			if ( lpb->bStartOfLine && ! bStartOfLine )
				return FALSE;

			/*
			 *	Match!
			 */
			return TRUE;
		}
	}
	return FALSE;
}

/*
 *	Is this the end of a block?
 */
static BOOL BlockEnd( LPCLASSDATA lpcd, LPBLOCK lpb, TCHAR *pcText, int nIndex, int nLength )
{
	/*
	 *	End String valid?
	 */
	if ( lpb->nEnd || lpb->pszEnd == END_OF_LINE )
	{
		/*
		 *	Block terminator at eol?
		 */
		if ( lpb->pszEnd == END_OF_LINE )
		{
			if ( nIndex == nLength )
			{
				/*
				 *	Match. Is it preceeded by an
				 *	escape character?
				 */
				if ( nIndex > 0 && Parser->cEscape && pcText[ nIndex - 1 ] == Parser->cEscape )
				{
					/*
					 *	Escape character escaped?
					 */
					if ( nIndex > 1 && Parser->cEscape && pcText[ nIndex - 2 ] == Parser->cEscape )
						/*
						 *	Still a match.
						 */
						 return TRUE;
					/*
					 *	This means no match.
					 */
					return FALSE;
				}

				/*
				 *	OK.
				 */
				return TRUE;
			}
		}
		/*
		 *	Is this the block terminator?
		 */
		else if ( ! ( Parser->bCaseOn ? _tcsncmp : _tcsnicmp )( lpb->pszEnd, &pcText[ nIndex ], lpb->nEnd ))
		{
			/*
			 *	Match. Is it preceeded by an
			 *	escape character?
			 */
			if ( nIndex > 0 && Parser->cEscape && pcText[ nIndex - 1 ] == Parser->cEscape )
			{
				/*
				 *	Escape character escaped?
				 */
				if ( nIndex > 1 && Parser->cEscape && pcText[ nIndex - 2 ] == Parser->cEscape )
					/*
					 *	Still a match.
					 */
					 return TRUE;

				/*
				 *	This means no match.
				 */
				return FALSE;
			}

			/*
			 *	Match!
			 */
			return TRUE;
		}
	}
	/*
	 *	If we do not have a block terminator we
	 *	terminate the block at the first white
	 *	space/delimiter or at the end of the line.
	 */
	else if ( nIndex == nLength )
		/*
		 *	End the block.
		 */
		return TRUE;
	else if ( nIndex < nLength && ( _istspace( pcText[ nIndex ] ) || IsDelimiter( lpcd, pcText[ nIndex ] )))
		/*
		 *	End the block.
		 */
		return TRUE;

	return FALSE;
}

/* 
 *	Pre-parse until we reach the passed line.
 */
void PreParseTo( LPCLASSDATA lpcd, int nTo )
{
	register LPLINE		lpLine;
	register int		i, nBlock = -1, nLines = ArrayGetSize( lpcd->lpLines );
	int			nDummy;

	/* 
	 *	Did the previous line end with an open block
	 *	marker?
	 */
	if ( lpcd->dwParsedUpTo == 0 ) nBlock = -1;
	else			       nBlock = (( LPLINE )ArrayGetAt( lpcd->lpLines, lpcd->dwParsedUpTo ))->nBlock;

	/*
	 *	Make sure we do not go past the end
	 *	of the file.
	 */
	nTo = min( nTo, nLines - 1 );

	/*
	 *	Iterate the lines.
	 */
	for ( i = lpcd->dwParsedUpTo; i <= nTo; i++ )
	{
		/*
		 *	Get line pointer.
		 */
		lpLine = ArrayGetAt( lpcd->lpLines, i );

		/*
		 *	Pre-parse the line. This is done so that 
		 *	the blocks found in the text are parsed
		 *	before the file is displayed the first
		 *	time.
		 */
		lpLine->nBlock = nBlock = ParseLine( lpcd, nBlock, lpLine->pcText, lpLine->nLength, NULL, &nDummy, 0, i );
	}

	/*
	 *	Are all lines parsed?
	 */
	if ( nTo >= nLines - 1 )
	{
		/*
		 *	Kill the timer.
		 */
		if ( lpcd->nParseTimerID ) 
		{
			KillTimer( lpcd->hWnd, lpcd->nParseTimerID );
			lpcd->nParseTimerID = 0;
		}

		/*
		 *	Set the counter to the maximum.
		 */
		lpcd->dwParsedUpTo = 0xFFFFFFFF;
	}
	else
	{
		/*
		 *	Is the parsing timer up?
		 */
		if ( lpcd->nParseTimerID == 0 ) lpcd->nParseTimerID = SetTimer( lpcd->hWnd, IDT_PARSETIMER, 100, NULL );

		/*
		 *	Update the counter.
		 */
		lpcd->dwParsedUpTo = nTo;
	}
}

/*
 *	Macro which checks if the number of
 *	text blocks is sufficient.
 */
#define CHECK_BLOCKS\
	if ( nBlock == nNumBlocks - 1 )\
	{\
		*lpnBlocks = 1;\
		scColors[ 0 ].nColumn = 0;\
		scColors[ 0 ].crColor = Parser->crColors[ CARR_TEXT ];\
		scColors[ 0 ].crBgColor = CLR_DEFAULT;\
		scColors[ 0 ].wFlags = 0;\
	}

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
 *	Check if the text at 'nIndex' is a
 *	hyperlink.
 */
static BOOL IsHyperlink( TCHAR *pszText, int nIndex, int nLength )
{
	/*
	 *	Iterate all hyperlink initiators.
	 */
	int i = 0;
	while ( Hyper[ i ].pszURL )
	{
		/*
		 *	Is it this initiator?
		 */
		if (( nLength - nIndex >= Hyper[ i ].nLength ) && _tcsnicmp( &pszText[ nIndex ], Hyper[ i ].pszURL, Hyper[ i ].nLength ) == 0 )
			return TRUE;
		i++;
	}
	return FALSE;
}

/*
 *	Look for a hyperlink starting at position
 *	'nIndex' up to position 'nLength'.
 */
static int FindHyperlinksInBlock( LPCLASSDATA lpcd, TCHAR *pcText, int nIndex, int nLength, int nBlock, int nNumBlocks, int *lpnBlocks, SYNTAX_COLOR *scColors )
{
	/*
	 *	Iterate all characters from 'nIndex'
	 *	up to 'nLength'.
	 */
	for ( ; nIndex < nLength; nIndex++ )
	{
		/*
		 *	Is this a hyperlink?
		 */
		if ( IsHyperlink( pcText, nIndex, nLength ))
		{
			/*
			 *	Enough room in the color
			 *	blocks array?
			 */
			CHECK_BLOCKS;

			/*
			 *	Yes. Terminate current color.
			 */
			scColors[ nBlock++ ].nColumn = nIndex;

			/*
			 *	Start a new color. Actually this does not
			 *	start a new color but it sets the SCF_HYPERLINK
			 *	flag in a block with the same color.
			 *
			 *	The flag tell's the rendering engine to
			 *	render the text using the underlined font.
			 */
			scColors[ nBlock ].nColumn   = nLength;
			scColors[ nBlock ].crColor   = scColors[ nBlock - 1 ].crColor;
			scColors[ nBlock ].crBgColor = scColors[ nBlock - 1 ].crBgColor;
			scColors[ nBlock ].pHash     = NULL;
			scColors[ nBlock ].wFlags    = SCF_HYPERLINK;

			/*
			 *	Is it a quoted hyperlink?
			 */
			if ( nIndex && pcText[ nIndex - 1 ] == _T( '"' ))
			{
				/*
				 *	Look for the next double quote or
				 *	the end of the line.
				 */
				while ( nIndex < nLength && pcText[ nIndex ] != _T( '"' )) nIndex++;
			}
			else if ( nIndex == 0 || ( _istspace( pcText[ nIndex - 1 ] )))
			{
				/*
				 *	Look for the next white space or
				 *	the end of the line.
				 */
				while ( nIndex < nLength && ! _istspace( pcText[ nIndex ] )) nIndex++;
			}

			/*
			 *	Terminate this block.
			 */
			CHECK_BLOCKS;

			/*
			 *	Yes. Terminate current color.
			 */
			scColors[ nBlock++ ].nColumn = nIndex;

			/*
			 *	Erase the SCF_HYPERLINK flag which tells 
			 *	the rendering engine to switch back to 
			 *	the normal font.
			 */
			scColors[ nBlock ].nColumn   = nLength;
			scColors[ nBlock ].crColor   = scColors[ nBlock - 1 ].crColor;
			scColors[ nBlock ].crBgColor = scColors[ nBlock - 1 ].crBgColor;
			scColors[ nBlock ].pHash     = NULL;
			scColors[ nBlock ].wFlags    = 0;
		}
	}
	return nBlock;
}

int ParseLine( LPCLASSDATA lpcd, int nInBlock, TCHAR *pcText, int nLength, SYNTAX_COLOR *scColors, int *lpnBlocks, int nNumBlocks, int nLine )
{
	LPPARSER	lpp = Parser;
	LPKEYHASH	lpHash;
	LPBLOCK		lpBlock;
	COLORREF	crText = Parser->crColors[ CARR_TEXT ];
	int		nIndex = 0, nBlock = 0, nSize, i, nMatchX1 = 0, nMatchX2 = 0;
	BOOL		bContinue = FALSE, bStartOfLine = TRUE, bHasInit = FALSE;
	
	/*
	 *	Compute match columns.
	 */
	if ( lpcd->ptBracket1.y >= 0 ) nMatchX1 = CaretOffsetLine( lpcd, lpcd->ptBracket1.y, lpcd->ptBracket1.x );
	if ( lpcd->ptBracket2.y >= 0 ) nMatchX2 = CaretOffsetLine( lpcd, lpcd->ptBracket2.y, lpcd->ptBracket2.x );

	/*
	 *	Any text to parse?
	 */
	if ( nLength == 0 )
		/*
		 *	Return the block we are in.
		 */
		 return nInBlock;

	/*
	 *	Start with a normal text
	 *	color block...
	 */
	if ( scColors )
	{
		/*
		 *	Enough text blocks?
		 */
		CHECK_BLOCKS;
		scColors[ nBlock ].nColumn   = nLength;
		scColors[ nBlock ].crColor   = crText;
		scColors[ nBlock ].crBgColor = CLR_DEFAULT;
		scColors[ nBlock ].pHash     = NULL;
		scColors[ nBlock ].wFlags    = 0;
	}

	/*
	 *	Parser active?
	 */
	if ( lpp == NULL )
	{
		/*
		 *	A single text color block.
		 */
		*lpnBlocks = 1;
		return -1;
	}

	/*
	 *	Are we in a block?
	 */
	if ( nInBlock != -1 )
	{
		/*
		 *	Get the block.
		 */
		lpBlock = ArrayGetAt( lpp->lpaBlocks, nInBlock );
		
		/*
		 *	Setup the color.
		 */
		if ( scColors )
		{
			/*
			 *	Enough text blocks?
			 */
			CHECK_BLOCKS;
			scColors[ nBlock ].nColumn   = nLength;
			scColors[ nBlock ].crColor   = lpBlock->crColor;
			scColors[ nBlock ].crBgColor = lpBlock->crBgColor;
			scColors[ nBlock ].pHash     = NULL;
			scColors[ nBlock ].wFlags    = 0;
		}

		/*
		 *	End the block with a string?
		 */
		if ( lpBlock->nEnd )
		{
			/*
			 *	See if the terminator string occures
			 *	on the line.
			 */
			LPCTSTR pszStr = ( lpp->bCaseOn ? StrStr : StrStrI )( pcText, lpBlock->pszEnd );

			/*
			 *	Terminator found?
			 */
			if( pszStr == NULL )
			{
				/*
				 *	Are we parsing hyperlinks?
				 */
				if ( Parser->bParseHyperLinks && scColors && lpnBlocks )
				{
					/*
					 *	Find all hyperlinks in the line starting
					 *	at offset 'nIndex'.
					 */
					*lpnBlocks = FindHyperlinksInBlock( lpcd, pcText, nIndex, nLength, 0, nNumBlocks, lpnBlocks, scColors ) + 1;
					scColors[ *lpnBlocks - 1 ].nColumn = nLength;
				}
				else if ( lpnBlocks )
					/*
					 *	Uses a single block.
					 */
					*lpnBlocks = 1;

				/*
				 *	Remain in the text block.
				 */
				return nInBlock;
			}
			else if ( Parser->bParseHyperLinks && scColors && lpnBlocks )
				/*
				 *	Find all hyperlinks from offset 'nIndex'
				 *	up to the block end marker.
				 */
				nBlock = FindHyperlinksInBlock( lpcd, pcText, nIndex, ( int )( pszStr - pcText ), 0, nNumBlocks, lpnBlocks, scColors );

			/*
			 *	Skip to the index at which
			 *	the terminator was found.
			 */
			nIndex = ( int )( pszStr - pcText );
		}
	}
	else
	{
		/*
		 *	If we are not yet inside a block we determine
		 *	if any of the block initiators occure in this
		 *	line and, if so, at which offset.
		 */
		for ( i = 0; ; i++ )
		{
			/*
			 *	Get the block.
			 */
			lpBlock = ArrayGetAt( lpp->lpaBlocks, i );
			
			/*
			 *	Done?
			 */
			if ( lpBlock == NULL ) 
				break;

			/*
			 *	By default this block initiator is not on
			 *	this line.
			 */
			lpBlock->bInLine = FALSE;

			/*
			 *	Will the initiator fit at all?
			 */
			if ( lpBlock->nStart && lpBlock->nStart <= nLength )
			{
				/*
				 *	Look up the block initiator on the line.
				 */
				LPCTSTR pszStr = ( lpp->bCaseOn ? StrStr : StrStrI )( pcText, lpBlock->pszStart );

				/*
				 *	Found?
				 */
				if ( pszStr != NULL )
				{
					/*
					 *	We have a block initializer.
					 */
					bHasInit = TRUE;

					/*
					 *	This block initiator is located 
					 *	on this line.
					 */
					lpBlock->bInLine = TRUE;
				}
			}
		}
	}
	
	/*
	 *	First we skip the leading blanks...
	 */
	while ( _istspace( pcText[ nIndex ] ) && nIndex <= nLength ) nIndex++;

	/*
	 *	Iterate text.
	 */
	for ( /*nIndex = 0*/; nIndex <= nLength; nIndex++ )
	{
		/*
		 *	Clear continue flag.
		 */
		bContinue = FALSE;

		/*
		 *	In a block?
		 */
		if ( nInBlock != -1 )
		{
			/*
			 *	Get block.
			 */
			lpBlock = ArrayGetAt( lpp->lpaBlocks, nInBlock );

			/*
			 *	Does the block terminate with a string?
			 */
			if ( lpBlock->nEnd )
			{
				/*
				 *	Does the terminator occure in the text?
				 */
				LPCTSTR pszStr = ( lpp->bCaseOn ? StrStr : StrStrI )( &pcText[ nIndex ], lpBlock->pszEnd );
				
				/*
				 *	No. Return the block number.
				 */
				if ( pszStr == NULL )
				{
					/*
					 *	Are we parsing hyperlinks?
					 */
					if ( Parser->bParseHyperLinks && scColors && lpnBlocks )
					{
						/*
						 *	Find the hyperlinks starting at offset 'nIndex'.
						 */
						*lpnBlocks = FindHyperlinksInBlock( lpcd, pcText, nIndex, nLength, nBlock, nNumBlocks, lpnBlocks, scColors ) + 1;
						scColors[ *lpnBlocks - 1 ].nColumn = nLength;
					}
					else if ( lpnBlocks )
						/*
						 *	Store the block number. 
						 */
						*lpnBlocks = nBlock + 1;

					return nInBlock;
				}
				else if ( Parser->bParseHyperLinks && scColors && lpnBlocks )
					/*
					 *	Find all hyperlinks from offset 'nIndex'
					 *	up to the block end marker.
					 */
					nBlock = FindHyperlinksInBlock( lpcd, pcText, nIndex, ( int )( pszStr - pcText ), nBlock, nNumBlocks, lpnBlocks, scColors );

				/*
				 *	Skip through to the index at which the
				 *	terminator was found.
				 */
				nIndex = ( int )( pszStr - pcText );
			}

			/*
			 *	Will the terminator fit?
			 */
			if ( nLength - nIndex >= lpBlock->nEnd || lpBlock->pszEnd == END_OF_LINE )
			{
				/*
				 *	End a block?
				 */
				if ( BlockEnd( lpcd, lpBlock, pcText, nIndex, nLength ))
				{
					/*
					 *	Color array present?
					 */
					if ( scColors )
					{
						/*
						 *	Enough room?
						 */
						CHECK_BLOCKS;

						/*
						 *	Yes. Terminate current color.
						 */
						scColors[ nBlock++ ].nColumn = nIndex + lpBlock->nEnd;

						/*
						 *	Start a new color.
						 */
						scColors[ nBlock ].nColumn   = nLength;
						scColors[ nBlock ].crColor   = crText;
						scColors[ nBlock ].crBgColor = CLR_DEFAULT;
						scColors[ nBlock ].pHash     = NULL;
						scColors[ nBlock ].wFlags    = 0;
					}

					/*
					 *	Skip the block terminator.
					 */
					if ( lpBlock->pszEnd != END_OF_LINE ) nIndex += lpBlock->nEnd - 1;
					else				      nIndex = nLength;

					/*
					 *	No longer in the block.
					 */
					nInBlock = -1;
					
					/*
					 *	Continue parse...
					 */
					continue;
				}
			}
		}
		
		/*
		 *	Keep looking for the terminator if
		 *	we are still inside a block.
		 */
		if ( nInBlock != -1 )
		{
			if ( scColors && nIndex >= nLength )
				scColors[ nBlock ].nColumn = nIndex;
			continue;
		}

		/*
		 *	Do we have an initiator?
		 */
		if ( bHasInit )
		{
			/*
			 *	Look up the block
			 *	initiators.
			 */	
			for ( i = 0; ; i++ )
			{
				/*
				 *	Valid block?
				 */
				if (( lpBlock = ArrayGetAt( lpp->lpaBlocks, i )) == NULL )
					break;

				/*
				 *	Block initiator in this
				 *	line?
				 */
				if ( ! lpBlock->bInLine )
					continue;

				/*
				 *	Block found?
				 */
				if ( ! BlockStart( lpcd, lpBlock, pcText, nIndex, nLength, bStartOfLine ))
					continue;

				/*
				 *	Colors present?
				 */
				if ( scColors )
				{
					/*
					 *	Enough room?
					 */
					CHECK_BLOCKS;

					/*
					 *	Yes. Terminate current color.
					 */
					scColors[ nBlock++ ].nColumn = nIndex;

					/*
					 *	Start a new color.
					 */
					scColors[ nBlock ].nColumn   = nLength;
					scColors[ nBlock ].crColor   = lpBlock->crColor;
					scColors[ nBlock ].crBgColor = lpBlock->crBgColor;
					scColors[ nBlock ].pHash     = NULL;
					scColors[ nBlock ].wFlags    = 0;
				}

				/*
				 *	Mark us as being in the block.
				 */
				nInBlock = i;

				/*
				 *	Does this block terminate at eol?
				 */
				if ( lpBlock->pszEnd == END_OF_LINE )
				{
					/*
					 *	Look at the rest of the line
					 *	for hyperlinks.
					 */
					if ( Parser->bParseHyperLinks && scColors && lpnBlocks )
						nBlock = FindHyperlinksInBlock( lpcd, pcText, nIndex, nLength, nBlock, nNumBlocks, lpnBlocks, scColors );

					/*
					 *	Skip everything except
					 *	the last terminator length 
					 *	plus the escape character.
					 */
					nIndex = max( 0, nLength - 2 );
				}
				else
					/*
					 *	Skip the block initiator.
					 */
					nIndex += lpBlock->nStart - 1;

				/*
				 *	Break the loop and
				 *	continue parsing.
				 */
				bContinue = TRUE;
				break;
			}
		}

		/*
		 *	No longer at the start
		 *	of the line...
		 */
		bStartOfLine = FALSE;

		/*
		 *	Continue?
		 */
		if ( bContinue || scColors == NULL )
			continue;

		/*
		 *	Skip spaces.
		 */
		if ( _istspace( pcText[ nIndex ] ))
		{
			/*
			 *	Make a new block.
			 */
			if ( scColors[ nBlock ].crBgColor != CLR_DEFAULT ||
			     scColors[ nBlock ].crColor   != crText ||
			     scColors[ nBlock ].wFlags )
			{
				/*
				 *	Enough room?
				 */
				CHECK_BLOCKS;

				/*
				 *	Terminate current color.
				 */
				scColors[ nBlock++ ].nColumn = nIndex;

				/*
				 *	Setup new color.
				 */
				scColors[ nBlock ].crColor   = crText;
				scColors[ nBlock ].crBgColor = CLR_DEFAULT;
				scColors[ nBlock ].nColumn   = nLength;
				scColors[ nBlock ].pHash     = NULL;
				scColors[ nBlock ].wFlags    = 0;
			}
			continue;
		}
	
		if ( Parser->bParseHyperLinks && scColors && IsHyperlink( pcText, nIndex, nLength ))
		{
			/*
			 *	Enough room?
			 */
			CHECK_BLOCKS;

			/*
			 *	Yes. Terminate current color.
			 */
			scColors[ nBlock++ ].nColumn = nIndex;

			/*
			 *	Start a new color.
			 */
			scColors[ nBlock ].nColumn   = nLength;
			scColors[ nBlock ].crColor   = Parser->crColors[ CARR_HYPERLINK ];
			scColors[ nBlock ].crBgColor = Parser->crColors[ CARR_BACKGROUND_HYPERLINK ];
			scColors[ nBlock ].pHash     = NULL;
			scColors[ nBlock ].wFlags    = SCF_HYPERLINK;

			/*
			 *	Is it a quoted hyperlink?
			 */
			if ( nIndex && pcText[ nIndex - 1 ] == _T( '"' ))
			{
				/*
				 *	Look up the next quote.
				 */
				while ( nIndex < nLength && pcText[ nIndex ] != _T( '"' )) nIndex++;
				if ( pcText[ nIndex ] == _T( '"' )) nIndex--;
			}
			else
			{
				/*
				 *	Look up the next white space.
				 */
				while ( nIndex < nLength && ! _istspace( pcText[ nIndex ] )) nIndex++;
				if ( _istspace( pcText[ nIndex ] )) nIndex--;
			}
			continue;
		}

		/*
		 *	Delimiter?
		 */
		if ( IsDelimiter( lpcd, pcText[ nIndex ] ))
		{
			/*
			 *	Any change in color?
			 */
			if (( scColors[ nBlock ].crColor   != Parser->crColors[ CARR_DELIMITER ] ) ||
			    ( scColors[ nBlock ].crBgColor != Parser->crColors[ CARR_BACKGROUND_DELIMITER ] ) ||
			    ( scColors[ nBlock ].wFlags ))
			{
				/*
				 *	Enough room?
				 */
				CHECK_BLOCKS;

				/*
				 *	Terminate current color.
				 */
				scColors[ nBlock++ ].nColumn = nIndex;

				/*
				 *	Setup delimiter color.
				 */
				scColors[ nBlock ].crColor   = Parser->crColors[ CARR_DELIMITER ];
				scColors[ nBlock ].crBgColor = Parser->crColors[ CARR_BACKGROUND_DELIMITER ];
				scColors[ nBlock ].nColumn   = nLength;
				scColors[ nBlock ].pHash     = NULL;
				scColors[ nBlock ].wFlags    = 0;
			}

			/*
			 *	Do we have matched brackets?
			 */
			if (( nLine == lpcd->ptBracket1.y && nIndex == nMatchX1 ) ||
			    ( nLine == lpcd->ptBracket2.y && nIndex == nMatchX2 ))
			{
				/*
				 *	Enough room?
				 */
				CHECK_BLOCKS;

				/*
				 *	Terminate current color.
				 */
				scColors[ nBlock++ ].nColumn = nIndex;

				/*
				 *	Setup match color.
				 */
				scColors[ nBlock ].crColor   = scColors[ nBlock - 1 ].crColor;
				scColors[ nBlock ].crBgColor = lpp->crColors[ CARR_BRACKET_MATCH ];
				scColors[ nBlock ].nColumn   = nLength;
				scColors[ nBlock ].pHash     = NULL;
				scColors[ nBlock ].wFlags    = 0;
			}

			/*
			 *	Continue parsing.
			 */
			continue;
		} 
		else
		/*
		 *	Is it a number? This check is a bit
		 *	to simple but it should not pose
		 *	any problems...
		 */
		if ( _istdigit( pcText[ nIndex ] ))
		{
			/*
			 *	Color changes?
			 */
			if (( scColors[ nBlock ].crColor   != Parser->crColors[ CARR_NUMBER ] ) ||
			    ( scColors[ nBlock ].crBgColor != Parser->crColors[ CARR_BACKGROUND_NUMBER ] ) ||
			    ( scColors[ nBlock ].wFlags ))
			{
				/*
				 *	Enough room?
				 */
				CHECK_BLOCKS;

				/*
				 *	Terminate the current color.
				 */
				scColors[ nBlock++ ].nColumn = nIndex;

				/*
				 *	Setup the number colors.
				 */
				scColors[ nBlock ].nColumn   = nLength;
				scColors[ nBlock ].crColor   = Parser->crColors[ CARR_NUMBER ];
				scColors[ nBlock ].crBgColor = Parser->crColors[ CARR_BACKGROUND_NUMBER ];
				scColors[ nBlock ].pHash     = NULL;
				scColors[ nBlock ].wFlags    = 0;
			}
		}
		else
		{
			/*
			 *	Do we have bracket matches?
			 */
			if (( nLine == lpcd->ptBracket1.y && nIndex == nMatchX1 ) ||
			    ( nLine == lpcd->ptBracket2.y && nIndex == nMatchX2 ))
			{
				/*
				 *	Enough room?
				 */
				CHECK_BLOCKS;

				/*
				 *	Terminate current color.
				 */
				scColors[ nBlock++ ].nColumn = nIndex;

				/*
				 *	Setup match color.
				 */
				scColors[ nBlock ].crColor   = crText;
				scColors[ nBlock ].crBgColor = lpp->crColors[ CARR_BRACKET_MATCH ];
				scColors[ nBlock ].nColumn   = nLength;
				scColors[ nBlock ].pHash     = NULL;
				scColors[ nBlock ].wFlags    = 0;

				/*
				 *	Continue parsing.
				 */
				continue;
			}
			else if ( scColors[ nBlock ].crColor   != crText ||
				  scColors[ nBlock ].crBgColor != CLR_DEFAULT ||
				  scColors[ nBlock ].wFlags )
			{
				/*
				 *	Enough room?
				 */
				CHECK_BLOCKS;
		
				/*
				 *	Terminate current color.
				 */
				scColors[ nBlock++ ].nColumn = nIndex;

				/*
				 *	Setup text color.
				 */
				scColors[ nBlock ].crColor   = crText;
				scColors[ nBlock ].crBgColor = CLR_DEFAULT;
				scColors[ nBlock ].nColumn   = nLength;
				scColors[ nBlock ].pHash     = NULL;
				scColors[ nBlock ].wFlags    = 0;
			}
		}

		/*
		 *	Count the characters up until
		 *	the next space or delimiter.
		 */
		nSize = nIndex;
		while ( nSize < nLength && ! _istspace( pcText[ nSize ] ) && ! IsDelimiter( lpcd, pcText[ nSize ] ))
		{
			/*
			 *	Do we have bracket matches?
			 */
			if (( nLine == lpcd->ptBracket1.y && nSize == nMatchX1 ) ||
			    ( nLine == lpcd->ptBracket2.y && nSize == nMatchX2 ))
			{
				/*
				 *	Enough room?
				 */
				CHECK_BLOCKS;

				/*
				 *	Terminate current color.
				 */
				scColors[ nBlock++ ].nColumn = nSize;

				/*
				 *	Setup match colors.
				 */
				scColors[ nBlock ].crColor   = crText;
				scColors[ nBlock ].crBgColor = lpp->crColors[ CARR_BRACKET_MATCH ];
				scColors[ nBlock ].nColumn   = nLength;
				scColors[ nBlock ].pHash     = NULL;
				scColors[ nBlock ].wFlags    = 0;

				/*
				 *	Continue parsing.
				 */
				break;
			}
			nSize++;
		}

		/*
		 *	Is the previous character a space/delimiter or are
		 *	we at the start of the line?
		 */
		if ( nIndex == 0 || _istspace( pcText[ nIndex - 1 ] ) || IsDelimiter( lpcd, pcText[ nIndex - 1 ] ))
		{
			/*
			 *	Is it a keyword
			 */
			if (( lpHash = IsKeyword( lpcd, &pcText[ nIndex ], nSize - nIndex )) != NULL )
			{
				/*
				 *	Color changes?
				 *	NOTE: Removed to accomodate case-fixing.
				 */
				/*if ( scColors[ nBlock ].crColor != lpHash->crColor )*/
				{
					/*
					 *	Enough room?
					 */
					CHECK_BLOCKS;

					/*
					 *	Terminate the current color.
					 */
					scColors[ nBlock++ ].nColumn = nIndex;

					/*
					 *	Setup the keyword color and the hash. We use the hash
					 *	when a text line is edited to fix the casing when 
					 *	case-fixing is turned on.
					 */
					scColors[ nBlock ].nColumn   = nLength;
					scColors[ nBlock ].crColor   = lpHash->crColor;
					scColors[ nBlock ].crBgColor = lpHash->crBkColor;
					scColors[ nBlock ].pHash     = lpHash;
					scColors[ nBlock ].wFlags    = 0;
				}
			}
		}

		/*
		 *	Are we at the end?
		 */
		if ( nSize >= nLength )
			break;

		/*
		 *	Adjust the index.
		 */
		nIndex = nSize - 1;
	}

	/*
	 *	Store the number of syntax
	 *	color block that are valid.
	 */
	if ( lpnBlocks )
		*lpnBlocks = nBlock + 1;

	return nInBlock;
}
