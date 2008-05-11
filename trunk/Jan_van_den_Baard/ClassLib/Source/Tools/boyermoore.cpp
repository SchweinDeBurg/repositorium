//
// boyermoore.cpp
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#ifndef _UNICODE
#include "boyermoore.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Default constructor.
ClsBoyerMoore::ClsBoyerMoore()
{
	// Set default.
	m_bCaseOn = FALSE;
}

// Constructor. Set's search string and case mode.
ClsBoyerMoore::ClsBoyerMoore( LPCSTR pszSearchString, BOOL bCaseOn /* = FALSE  */ )
{
	_ASSERT_VALID( pszSearchString );

	// Setup data.
	m_strSearchString = pszSearchString;
	m_bCaseOn = bCaseOn;
	SetSearchString();
}

// Copy constructor.
ClsBoyerMoore::ClsBoyerMoore( const ClsBoyerMoore& bm )
{
	// Copy data from source.
	operator=( bm );
}

// Destructor...
ClsBoyerMoore::~ClsBoyerMoore()
{
}

// Find forward.
#pragma warning( push )
#pragma warning( disable: 4127 )
int ClsBoyerMoore::FindForward( char *pData, int nLength )
{
	_ASSERT_VALID( pData );
	_ASSERT( nLength > 0 );

	char		*pcEndString, *pcTextPtr, *pcString = ( char * )m_strSearchString.GetMBS();
	int		 nIndex, nSkip = 0, nStrLen = m_strSearchString.GetStringLength() - 1;
	int		*lpTable = &m_aDeltas[ 0 ];

	// Setup data.
	pcEndString = ( char * )pcString + nStrLen;
	pcTextPtr   = pData + nStrLen;
	nIndex	    = nLength - nStrLen;

	// Enough text for a match?
	if ( nIndex >= 0 )
	{
		// Case-sensitive?
		if ( m_bCaseOn )
		{
			// Search...
			while ( 1 )
			{
				// Found string?
				if (( *pcEndString == *pcTextPtr ) && ( ! strncmp( pcString, pcTextPtr - nStrLen, nStrLen )))
					// Yes. Return the offset.
					return ( int )(( pcTextPtr - pData ) - nStrLen );

				// Find out how many characters
				// we can skip.
				nSkip = lpTable[ ( UCHAR )tolower( *pcTextPtr ) ];

				// Enough left?
				if ( nIndex < nSkip )
					break;

				// Skip the characters.
				nIndex -= nSkip;
				pcTextPtr += nSkip;
			}
		}
		else
		{
			// Search...
			while ( 1 )
			{
				// Found string?
#if (_MSC_VER < 1500)
				if (( tolower( *pcEndString ) == tolower( *pcTextPtr )) && ( ! strnicmp( pcString, pcTextPtr - nStrLen, nStrLen )))
#else
				if (( tolower( *pcEndString ) == tolower( *pcTextPtr )) && ( ! _strnicmp( pcString, pcTextPtr - nStrLen, nStrLen )))
#endif   // _MSC_VER
					// Yes. Return the offset.
					return ( int )(( pcTextPtr - pData ) - nStrLen );

				// Find out how many characters
				// we can skip.
				nSkip = lpTable[ ( UCHAR )tolower( *pcTextPtr ) ];

				// Enough left?
				if ( nIndex < nSkip )
					break;

				// Skip the characters.
				nIndex -= nSkip;
				pcTextPtr += nSkip;
			}
		}
	}
	return -1;
}

int ClsBoyerMoore::FindBackward( char *pData, int nLength )
{
	_ASSERT_VALID( pData );
	_ASSERT( nLength > 0 );

	// Setup data.
	char			*pcString, *pcTextPtr;
	int			 nIndex, nSkip = 0, nStrLen = m_strSearchString.GetStringLength() - 1;
	int			*lpTable = &m_aBackDeltas[ 0 ];

	pcString  = ( char * )m_strSearchString.GetMBS();
	pcTextPtr = pData;
	nIndex    = nLength;

	// Enough data to find the string in?
	if ( nIndex >= nStrLen )
	{
		// Case-sensitive?
		if ( m_bCaseOn )
		{
			// Search...
			while ( 1 )
			{
				// Found the string?
				if (( *pcString == *pcTextPtr ) && ( ! strncmp( pcString + 1, pcTextPtr + 1, nStrLen )))
					// Return the index.
					return ( int )( pData - pcTextPtr );

				// Find out how many characters
				// we can skip.
				nSkip = lpTable[ ( UCHAR )tolower( *pcTextPtr ) ];

				// Enough left?
				if ( nIndex < nSkip )
					break;

				// Skip the characters.
				nIndex -= nSkip;
				pcTextPtr -= nSkip;
			}
		}
		else
		{
			// Search...
			while ( 1 )
			{
				// Found the string?
#if (_MSC_VER < 1500)
				if (( tolower( *pcString ) == tolower( *pcTextPtr )) && ( ! strnicmp( pcString + 1, pcTextPtr + 1, nStrLen )))
#else
				if (( tolower( *pcString ) == tolower( *pcTextPtr )) && ( ! _strnicmp( pcString + 1, pcTextPtr + 1, nStrLen )))
#endif   // _MSC_VER
					// Return the index.
					return ( int )( pData - pcTextPtr );

				// Find out how many characters
				// we can skip.
				nSkip = lpTable[ ( UCHAR )tolower( *pcTextPtr ) ];

				// Enough left?
				if ( nIndex < nSkip )
					break;

				// Skip the characters.
				nIndex -= nSkip;
				pcTextPtr -= nSkip;
			}
		}
	}
	return -1;
}
#pragma warning( pop )

// Operator overloads.
const ClsBoyerMoore& ClsBoyerMoore::operator=( const ClsBoyerMoore& bm )
{
	_ASSERT( &bm != this );

	// Copy data from source.
	m_strSearchString = bm.m_strSearchString;
	m_bCaseOn = bm.m_bCaseOn;
	memcpy( m_aDeltas, bm.m_aDeltas, ( 0xFF + 1 ) * sizeof( int ));
	memcpy( m_aBackDeltas, bm.m_aBackDeltas, ( 0xFF + 1 ) * sizeof( int ));
	return *this;
}
const ClsBoyerMoore& ClsBoyerMoore::operator=( LPCSTR pszSearchString )
{
	_ASSERT_VALID( pszSearchString );

	// Setup data.
	m_strSearchString = pszSearchString;
	SetSearchString();
	return *this;
}

void ClsBoyerMoore::SetSearchString()
{
	// Get search string length.
	int nLength = m_strSearchString.GetStringLength();

	// Preset skip table values.
	int i = 0;
	for ( ; i <= 0xFF; i++ )
	{
		m_aDeltas[ i ] = nLength;
		m_aBackDeltas[ i ] = nLength;
	}

	// Setup skip values for
	// forward searches.
	LPCSTR pszPtr;
	for ( pszPtr = m_strSearchString.GetMBS(), i = nLength; --i > 0; )
	{
		m_aDeltas[ ( UCHAR )tolower( *pszPtr ) ] = i;
		pszPtr++;
	}

	// Setup skip values for
	// backward searches.
	for ( pszPtr = (( LPCSTR )m_strSearchString.GetMBS()) + ( nLength - 1 ), i = nLength; --i > 0; )
	{
		m_aBackDeltas[ ( UCHAR )tolower( *pszPtr ) ] = i;
		pszPtr--;
	}
}

#endif // _UNICODE
