//
// string.cpp
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include <comutil.h>

#include "string.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Default constructor.
ClsString::ClsString()
{
	// Clear pointers.
	m_pszString = NULL;
	m_pszConverted = NULL;
	m_NilChar = _T( '\0' );
}

// Copy constructor.
ClsString::ClsString( const ClsString& csSource )
{
	// Set the source buffer in this object.
	m_pszString = csSource.m_pszString;
	m_pszConverted = NULL;
	m_NilChar = _T( '\0' );

	// If the source was valid we add a reference to it.
	if ( m_pszString ) GetBuf()->AddRef();
}

// Construct a string of "nRepeat" "ch" characters.
ClsString::ClsString( TCHAR ch, int nRepeat /* = 1 */ )
{
	_ASSERT( nRepeat >= 1 );    // nRepeat must be 1 or higher.
	_ASSERT( ch != _T( '\0' )); // We do not set to nil's.

	// Clear pointers.
	m_pszString = NULL;
	m_pszConverted = NULL;
	m_NilChar = _T( '\0' );

	// Allocate the required buffer.
	AllocateBuffer( nRepeat );

#ifdef _UNICODE
	// On Unicode builds we copy the characters one by
	// one.
	for ( int i = 0; i < nRepeat; i++ )
		m_pszString[ i ] = ch;
#else
	// Otherwise we do a simple memset on the
	// buffer.
	memset( m_pszString, ch, nRepeat );
#endif
}

// Constructor. Initializes to the window text.
ClsString::ClsString( HWND hWnd )
{
	// Clear pointers.
	m_pszString   = NULL;
	m_pszConverted = NULL;
	m_NilChar = _T( '\0' );

	// Setup the text.
	GetWindowText( hWnd );
}

// Construct a string of the input string.
ClsString::ClsString( LPCTSTR pszSource, int nChars /* = -1 */ )
{
	_ASSERT_VALID( pszSource ); // A valid string is required.

	// Clear pointers.
	m_pszString   = NULL;
	m_pszConverted = NULL;
	m_NilChar = _T( '\0' );

	// If the hi-order word of "pszSource" is 0
	// we load the string from the resources.
	if ( pszSource && HIWORD( pszSource ) == 0 )
	{
		// Load the string.
		LoadString( LOWORD( pszSource ));
		return;
	}

	// Allocate a new buffer if necessary.
	AllocateBuffer( nChars == -1 ? ( int )_tcslen( pszSource ) : nChars );

	// Copy the string.
	if ( nChars == -1 ) _tcscpy( m_pszString, pszSource );
	else		    
	{
		memcpy( m_pszString, pszSource, nChars * sizeof( TCHAR ));
		m_pszString[ nChars ] = _T( '\0' );
	}
}

#ifndef _UNICODE
// Constructor. Sets the contents to the string pointed
// to by the wide character string "pszSource".
ClsString::ClsString( LPCWSTR pszSource, int nChars /* = -1 */ )
{
	_ASSERT_VALID( pszSource );	// A valid string is required.

	// Clear pointers.
	m_pszString   = NULL;
	m_pszConverted = NULL;
	m_NilChar = _T( '\0' );

	// Allocate a new buffer if necessary.
	int nLength = nChars == -1 ? ( int )wcslen( pszSource ) : nChars;
	AllocateBuffer( nLength );

	// Copy the string.
	wcstombs( m_pszString, pszSource, nLength * sizeof( TCHAR ));
}
#else
// Constructor. Sets the contents to the string pointed
// to by the character string "pszSource".
ClsString::ClsString( LPCSTR pszSource, int nChars /* = -1 */ )
{
	_ASSERT_VALID( pszSource );	// A valid string is required.

	// Clear pointers.
	m_pszString   = NULL;
	m_pszConverted = NULL;
	m_NilChar = _T( '\0' );

	// Allocate a new buffer if necessary.
	int nLength = nChars == -1 ? strlen( pszSource ) : nChars;
	AllocateBuffer( nLength );

	// Copy the string.
	mbstowcs( m_pszString, pszSource, nLength );
}
#endif // _UNICODE

// Destructor. Releases the string buffer.
ClsString::~ClsString()
{
	// Release buffer.
	Release();
}

// Empty the string contents.
void ClsString::Empty()
{
	// Do not modify referenced data!
	CopyBeforeWrite();
	
	// Do we have a string?
	if ( m_pszString )
		GetBuf()->SetLength( 0 );
}

// Setup string length.
void ClsString::SetStringLength( int nLength )
{
	// String buffer present?
	if ( m_pszString && GetBuf()->GetLength() != nLength )
	{
		// Do not modify referenced data!
		CopyBeforeWrite();

		// Setup new string length.
		GetBuf()->SetLength( nLength );
	}
}

// Obtain window text.
int ClsString::GetWindowText( HWND hWnd )
{
	_ASSERT_VALID( hWnd ); // Must be valid.
	_ASSERT( ::IsWindow( hWnd )); // Must be valid.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Get the length necessary to store the text.
	int nLength = ::GetWindowTextLength( hWnd );

	// Empty the string when there is not text
	// in the window.
	if ( nLength == 0 )
		Empty();

	// Allocate memory to store the text.
	AllocateBuffer( nLength );

	// Get the window text.
	return ::GetWindowText( hWnd, m_pszString, nLength + 1 );
}

// Equality test with another ClsString object
// to compare with.
BOOL ClsString::IsEqual( const ClsString& csSource ) const
{
	// Compare the two strings
	if (( &csSource == this ) || ( m_pszString && csSource.m_pszString && _tcscmp( m_pszString, csSource.m_pszString ) == 0 ))
		return TRUE;
	return FALSE;
}

// Equality test with a string pointer to
// compare with.
BOOL ClsString::IsEqual( LPCTSTR pszSource ) const
{
	// Compare the two strings
	if ( m_pszString && pszSource && _tcscmp( m_pszString, pszSource ) == 0 )
		return TRUE;
	return FALSE;
}

// Return the character at "nIndex".
TCHAR ClsString::GetAt( int nIndex ) const
{
	_ASSERT( nIndex >= 0 && nIndex < GetStringLength()); // Stay in range.
	_ASSERT_VALID( m_pszString );

	// Return the character.
	return m_pszString[ nIndex ];
}

// Return the character reference at "nIndex".
TCHAR& ClsString::GetAt( int nIndex )
{
	_ASSERT( nIndex >= 0 && nIndex < GetStringLength()); // Stay in range.
	_ASSERT_VALID( m_pszString );

	// Return character reference.
	return ( TCHAR& )m_pszString[ nIndex ];
}

// Replace the character at "nIndex".
void ClsString::SetAt( int nIndex, TCHAR ch )
{
	_ASSERT( nIndex >= 0 && nIndex < GetStringLength()); // Stay in range.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Set the character.
	if ( m_pszString )
		m_pszString[ nIndex ] = ch;
}

// Compare this object with another string.
// <0 This object is smaller.
//  0 This object matches the string.
// >0 This object is larger.
int ClsString::Compare( LPCTSTR pszString ) const
{
	_ASSERT_VALID( pszString ); // Must be valid.
	_ASSERT_VALID( m_pszString ); // Can't compare with a NULL string.

	// Return result.
	return _tcscmp( m_pszString, pszString );
}

// Compare this object with another string (no case).
// <0 This object is smaller.
//  0 This object matches the string.
// >0 This object is larger.
int ClsString::CompareNoCase( LPCTSTR pszString ) const
{
	_ASSERT_VALID( pszString ); // Must be valid.
	_ASSERT_VALID( m_pszString ); // Can't compare with a NULL string.

	// Return result.
	return _tcsicmp( m_pszString, pszString );
}

// Compare this object with another string.
// Uses locale specific information.
// <0 This object is smaller.
//  0 This object matches the string.
// >0 This object is larger.
int ClsString::Collate( LPCTSTR pszString ) const
{
	_ASSERT_VALID( pszString ); // Must be valid.
	_ASSERT_VALID( m_pszString ); // Can't compare with a NULL string.

	// Return result.
	return _tcscoll( m_pszString, pszString );
}

// Compare this object with another string.
// Uses locale specific information.
// <0 This object is smaller.
//  0 This object matches the string.
// >0 This object is larger.
int ClsString::CollateNoCase( LPCTSTR pszString ) const
{
	_ASSERT_VALID( pszString ); // Must be valid.
	_ASSERT_VALID( m_pszString ); // Can't compare with a NULL string.

	// Return result.
	return _tcsicoll( m_pszString, pszString );
}

// Trims leading and trailing spaces from the
// contents.
ClsString& ClsString::Trim()
{
	// Anything to trim?
	if ( m_pszString )
	{
		// First remove the trailing spaces.
		int nPos = GetStringLength() - 1;
		while ( _istspace( m_pszString[ nPos ] )) nPos--;

		// Anything stipped?
		if ( nPos < GetStringLength() - 1 )
			SetStringLength( nPos + 1 );

		// Now count the leading spaces.
		nPos = 0;
		while ( _istspace( m_pszString[ nPos ] )) nPos++;

		// Anything to strip?
		if ( nPos )
		{
			// Move remaining characters in the buffer
			// and adjust the string length to match.
			memmove( &m_pszString[ 0 ], &m_pszString[ nPos ], ( GetStringLength() - nPos ) * sizeof( TCHAR ));
			SetStringLength( GetStringLength() - nPos );
		}
	}
	return *this;
}
			
// Extracts "nCount" characters from the left of the
// string and places them in a new object.
ClsString ClsString::Left( int nCount ) const
{
	_ASSERT( nCount <= GetStringLength()); // Can't extract more than there is.
	_ASSERT_VALID( m_pszString ); // Can't extract from a NULL string.

	// Create an object.
	ClsString	csTemp;

	// Allocate a buffer in it which will hold
	// the extracted part.
	csTemp.AllocateBuffer( nCount );

	// Copy the string.
	_tcsncpy( csTemp.m_pszString, m_pszString, nCount );

	// Return the new object.
	return csTemp;
}

// Extracts "nCount" characters from offset "nIndex" of the
// string and places them in a new object.
ClsString ClsString::Mid( int nIndex, int nCount /* == -1 */ ) const
{
	_ASSERT( nIndex >= 0 && nIndex < GetStringLength()); // Make sure we are in range.
	_ASSERT_VALID( m_pszString ); // Can't extract from a NULL string.

	// Create an object.
	ClsString	csTemp;
	int		nNumChars = nCount == -1 ? GetStringLength() - nIndex : nCount;
	_ASSERT( nNumChars + nIndex <= GetStringLength()); // Extracting to much?

	// Allocate a buffer in it which will hold
	// the extracted part.
	csTemp.AllocateBuffer( nNumChars );

	// Copy the string.
	_tcsncpy( csTemp.m_pszString, &m_pszString[ nIndex ], nNumChars );

	// Return the new object.
	return csTemp;
}

// Extracts "nCount" characters from the right of the
// string and places them in a new object.
ClsString ClsString::Right( int nCount ) const
{
	_ASSERT( nCount <= GetStringLength()); // Can't extract more than there is.
	_ASSERT_VALID( m_pszString ); // Can't extract from a NULL string.

	// Create an object.
	ClsString	csTemp;

	// Allocate a buffer in it which will hold
	// the extracted part.
	csTemp.AllocateBuffer( nCount );

	// Copy the string.
	_tcsncpy( csTemp.m_pszString, &m_pszString[ GetStringLength() - nCount ], nCount );

	// Return the new object.
	return csTemp;
}

// Convert the contents to upper case.
void ClsString::UpperCase()
{
	// Do not modify referenced data!
	CopyBeforeWrite();

	// Convert casing.
	if ( m_pszString )
	{
		// Iterate the contents.
		for ( int i = 0; i < GetStringLength(); i++ )
			// Convert the character.
			m_pszString[ i ] = ( TCHAR )_totupper( m_pszString[ i ] );
	}
}

// Convert the contents to lower case.
void ClsString::LowerCase()
{
	// Do not modify referenced data!
	CopyBeforeWrite();

	// Convert casing.
	if ( m_pszString )
	{
		// Iterate the contents.
		for ( int i = 0; i < GetStringLength(); i++ )
			// Convert the chracater.
			m_pszString[ i ] = ( TCHAR )_totlower( m_pszString[ i ] );
	}
}

// Swap the contents to lower case.
void ClsString::SwapCase()
{
	// Do not modify referenced data!
	CopyBeforeWrite();

	// Convert casing.
	if ( m_pszString )
	{
		// Iterate the contents.
		for ( int i = 0; i < GetStringLength(); i++ )
		{
			// Swap the letter casing.
			if (	  _istupper( m_pszString[ i ] )) m_pszString[ i ] = ( TCHAR )_totlower( m_pszString[ i ] );
			else if ( _istlower( m_pszString[ i ] )) m_pszString[ i ] = ( TCHAR )_totupper( m_pszString[ i ] );
		}
	}
}

// Replace all instances of "chOld" with
// "chNew".
int ClsString::Replace( TCHAR chOld, TCHAR chNew )
{
	_ASSERT( chOld != _T( '\0' ) && chNew != _T( '\0' )); // No 0 characters.

	// Are the characters different?
	if ( chOld == chNew )
		// No. Simply return 0;
		return 0;

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Initialize replacement counter.
	int	nCount = 0;

	// Iterate characters.
	for ( int i = 0; i < GetStringLength(); i++ )
	{
		// Match?
		if ( m_pszString[ i ] == chOld )
		{
			// Replace it and increase the replacement
			// counter.
			m_pszString[ i ] = chNew;
			nCount++;
		}
	}

	// Return the replacement counter.
	return nCount;
}

// Remove all instances of "ch" from
// the string
int ClsString::Remove( TCHAR ch )
{
	_ASSERT( ch != _T( '\0' )); // No 0 characters.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Initialize removement counter.
	int	nCount = 0;

	// Iterate characters.
	for ( int i = 0; i < GetStringLength(); i++ )
	{
		int nLength = GetStringLength();

		// Match?
		if ( m_pszString[ i ] == ch )
		{
			// Remove the character by moving all characters
			// right of it one place to the left.
			memmove(( void * )&m_pszString[ i ], ( void * )&m_pszString[ i + 1 ], ( nLength - i ) * sizeof( TCHAR ));
			SetStringLength( nLength - 1 );
			i--;
			nCount++;
		}
	}
	// Return the replacement counter.
	return nCount;
}

// Insert a character at the given index.
int ClsString::Insert( int nIndex, TCHAR ch )
{
	_ASSERT( nIndex >= 0 && nIndex <= GetStringLength()); // Make sure the index is valid.
	_ASSERT( ch != _T( '\0' )); // No 0 characters.

	// Do we have contents?
	if ( m_pszString == NULL )
		// No. Add the character.
		operator+=( ch );
	else
	{
		// Do not modify referenced data!
		CopyBeforeWrite();

		// How many characters must we move?
		int nChars = GetStringLength() - nIndex;

		// Make sure we have enough room to
		// add the character.
		AllocateBuffer( GetStringLength() + 1, TRUE );

		// Make room.
		memmove(( void * )&m_pszString[ nIndex + 1 ], ( void * )&m_pszString[ nIndex ], nChars * sizeof( TCHAR ));

		// Insert character.
		m_pszString[ nIndex ] = ch;
	}

	// Return the new length.
	return GetStringLength();
}

// Insert a string at the given index.
int ClsString::Insert( int nIndex, LPCTSTR pszString )
{
	_ASSERT( nIndex >= 0 && nIndex <= GetStringLength()); // Make sure the index is valid.
	_ASSERT_VALID( pszString ); // No NULL pointers.

	// Do we have contents?
	if ( m_pszString == NULL )
		// No. Add the string.
		operator+=( pszString );
	else
	{
		// Do not modify referenced data!
		CopyBeforeWrite();

		// How many characters must we move?
		int nChars = GetStringLength() - nIndex;

		// Make sure we have enough room to
		// add the string.
		int nLength = ( int )_tcslen( pszString );
		AllocateBuffer( GetStringLength() + nLength, TRUE );

		// Make room.
		memmove(( void * )&m_pszString[ nIndex + nLength ], ( void * )&m_pszString[ nIndex ], nChars * sizeof( TCHAR ));

		// Insert string.
		memcpy(( void * )&m_pszString[ nIndex ], ( void * )pszString, nLength * sizeof( TCHAR ));
	}

	// Return the new length.
	return GetStringLength();
}

// Insert a string at the given index.
int ClsString::Insert( int nIndex, ClsString& csString )
{
	_ASSERT( nIndex >= 0 && nIndex <= GetStringLength()); // Make sure the index is valid.
	_ASSERT( &csString != this ); // Do not insert into ourselves.

	// Do we have contents?
	if ( m_pszString == NULL )
		// No. Add the string.
		operator+=( csString );
	else
		// Insert the string.
		Insert( nIndex, csString.m_pszString );

	// Return the new length.
	return GetStringLength();
}

// Delete one or more characters from the string.
int ClsString::Delete( int nIndex, int nCount /* = 1 */ )
{
	_ASSERT( nIndex >= 0 && nIndex < GetStringLength()); // Make sure the index is valid.
	_ASSERT( nCount + nIndex <= GetStringLength()); // Do not delete to much.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Move the contents right of the removed area.
	memmove(( void * )&m_pszString[ nIndex ], ( void * )&m_pszString[ nIndex + nCount ], ( GetStringLength() - ( nIndex + nCount - 1 )) * sizeof( TCHAR ));
	SetStringLength( GetStringLength() - nCount );

	// Return the new length.
	return GetStringLength();
}

// Find the first occurence of the character "ch"
// from "nIndex" on.
int ClsString::Find( TCHAR ch, int nIndex /* = 0 */ ) const
{
	_ASSERT( nIndex >= 0 && nIndex < GetStringLength()); // Make sure the index is valid.
	_ASSERT( ch != _T( '\0' )); // No 0 characters.

	// Iterate string.
	for ( int i = nIndex; i < GetStringLength(); i++ )
	{
		// Character match?
		if ( m_pszString[ i ] == ch )
			return i;
	}
	return -1;
}

// Find the first occurence of the string "pszString"
// from "nIndex" on.
int ClsString::Find( LPCTSTR pszString, int nIndex /* = 0 */ ) const
{
	_ASSERT_VALID( pszString ); // No NULL pointers.
	_ASSERT( nIndex >= 0 && nIndex <= GetStringLength()); // Make sure the index is valid.

	// Iterate string.
	int nLength = ( int )_tcslen( pszString );
	for ( int i = nIndex; i <= ( int )( GetStringLength() - nLength ); i++ )
	{
		// Match?
		if ( _tcsncmp( &m_pszString[ i ], pszString, nLength ) == 0 )
			return i;
	}
	return -1;
}

// Find the first occurence of the character "ch"
// starting from the end of the string.
int ClsString::ReverseFind( TCHAR ch ) const
{
	_ASSERT( ch != _T( '\0' )); // No 0 characters.

	// Iterate string.
	for ( int i = GetStringLength() - 1; i >= 0; i-- )
	{
		// Character match?
		if ( m_pszString[ i ] == ch )
			return i;
	}
	return -1;
}

// Find the first occurence of any of the characters in
// "pszCharSet" from "nIndex" on.
int ClsString::FindOneOf( LPCTSTR pszCharSet, int nIndex /* = 0 */ ) const
{
	_ASSERT( nIndex >= 0 && nIndex <= GetStringLength()); // Make sure the index is valid.
	_ASSERT_VALID( pszCharSet ); // No NULL pointers.

	// Iterate string.
	for ( int i = nIndex; i < GetStringLength(); i++ )
	{
		// Iterate character set.
		for ( int a = 0; a < ( int )_tcslen( pszCharSet ); a++ )
		{
			// Is it a match?
			if ( m_pszString[ i ] == pszCharSet[ a ] )
				return i;
		}
	}
	return -1;
}

// Format a system error.
void ClsString::FormatMessage( DWORD dwErrorID /* = 0 */ )
{
	// Format the error specified or the
	// current error.
	LPVOID	pMsgBuffer;
	DWORD dwSize = ::FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					dwErrorID ? dwErrorID : ::GetLastError(),
					MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
					( LPTSTR )&pMsgBuffer,
					0,
					NULL );

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Error formatted OK?
	if ( dwSize )
	{
		// Allocate buffer.
		AllocateBuffer( dwSize );

		// Copy the result.
		_tcsncpy( m_pszString, ( LPCTSTR )pMsgBuffer, dwSize );

		// Free the result from the FormatMessage() API.
		::LocalFree( pMsgBuffer );
	}
	else
		// Use a default output. Not exactly language independant...
		Format( _T( "Error %ld occured." ), dwErrorID ? dwErrorID : ::GetLastError());
}

// Format a string.
void ClsString::Format( LPCTSTR pszFormatString, ... )
{
	_ASSERT_VALID( pszFormatString ); // Must be valid...
	TCHAR		szBuffer[ 4096 ]; // 4Kbyte chars is max.
	int		nChars;

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Format the output.
	va_list		args;
	va_start( args, pszFormatString );
	nChars = ::wvsprintf( szBuffer, pszFormatString, args );
	va_end( args );

	// Allocate a buffer to fit.
	AllocateBuffer( nChars );
	
	// Copy contents.
	_tcscpy( m_pszString, szBuffer );
}

// Obtain a part of the string which is delimited by the
// specified character.
int ClsString::GetDelimitedPart( TCHAR ch, int nIndex, ClsString& str ) const
{
	_ASSERT( ch != _T( '\0' )); // Must be a valid character.
	_ASSERT( nIndex >= 0 ); // Index must be positive.

	// Empty?
	int nLength = GetStringLength();
	if ( nLength == 0 )
		return 0;

	// Get the string base.
	LPCTSTR psz = m_pszString;

	// Skip through to the index.
	int nCount = 0;
	for ( ; nCount < nIndex; nCount++, psz = _tcsinc( psz ));

	// Do not pass the end of the string.
	if ( nCount >= nLength )
		return 0;

	// Set the string to the remainder.
	str = psz;

	// Iterate string to find the next delimited
	// part.
	for ( ; nCount <= nLength; nCount++, psz = _tcsinc( psz ))
	{
		// Is it the delimiter or a 0-character?
		if ( *psz == ch || *psz == _T( '\0' ))
		{
			// Truncate the string.
			str.SetStringLength( nCount - nIndex );

			// Free the extra memory.
			//str.FreeExtra();

			// Return the index of the next entry.
			return nCount + 1;
		}
	}
	return 0;
}

// Load a string from the resources.
BOOL ClsString::LoadString( UINT uID )
{
	// First we try 256 character buffer from
	// the stack.
	TCHAR	szBuffer[ 256 ];
	int	nSize = 256;

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Load the string. When the number of loaded characters
	// returned exactly equals the buffer size minus one we
	// can assume that LoadString() trunctated the string.
	int nChars = ::LoadString( ClsGetResourceHandle(), uID, szBuffer, nSize );
	if ( nChars == nSize - 1 )
	{
		do {
			// Increase buffer size until we found
			// a size which will hold the string.
			nSize += 256;

			// Allocate buffer.
			AllocateBuffer( nSize );

			// Load the string.
			nChars = ::LoadString( ClsGetResourceHandle(), uID, m_pszString, nSize );
		} while ( nChars == nSize - 1 );

	}
	else
	{
		// Allocate buffer and copy the contents of
		// the loaded string.
		AllocateBuffer( nChars );
		_tcscpy( m_pszString, szBuffer );
	}
	// Return success/failure.
	return ( BOOL )( nChars > 0 );
}

// Assignment operator.
const ClsString& ClsString::operator=( const ClsString& csSource )
{
	_ASSERT( &csSource != this ); // We can't assign us to ourself.

	// Release current data.
	Release();

	// Set the source buffer in this object.
	m_pszString = csSource.m_pszString;

	// If the source was valid we add a reference to it.
	if ( m_pszString ) GetBuf()->AddRef();
	return *this;
}

// Assignment operator.
const ClsString& ClsString::operator=( LPCTSTR pszSource )
{
	_ASSERT_VALID( pszSource ); // We can't assign to a NULL pointer.

	// If the hi-order word of "pszSource" is 0
	// we load the string from the resources.
	if ( HIWORD( pszSource ) == 0 )
	{
		// Load the string.
		LoadString( LOWORD( pszSource ));
		return *this;
	}

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer.
	AllocateBuffer(( int )_tcslen( pszSource ));

	// Copy the contents of the source to this
	// object.
	_tcscpy( m_pszString, pszSource );
	return *this;
}

// Assignment operator.
const ClsString& ClsString::operator=( TCHAR ch )
{
	_ASSERT( ch != _T( '\0' )); // We can't add a 0 character.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer.
	AllocateBuffer( 2 );

	// Copy the character.
	m_pszString[ 0 ] = ch;
	return *this;
}

// Assignment operator.
const ClsString& ClsString::operator=( HWND hWnd )
{
	// Assign to the window text.
	GetWindowText( hWnd );
	return *this;
}

// Assignment operator.
#ifndef _UNICODE
const ClsString& ClsString::operator=( LPCWSTR pszSource )
{
	_ASSERT_VALID( pszSource ); // We can't assign to a NULL pointer.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer.
	int nLength = ( int )wcslen( pszSource );
	AllocateBuffer( nLength );

	// Copy the string.
	wcstombs( m_pszString, pszSource, nLength * sizeof( TCHAR ));
	return *this;
}
#else
const ClsString& ClsString::operator=( LPCSTR pszSource )
{
	_ASSERT_VALID( pszSource ); // We can't assign to a NULL pointer.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer.
	int nLength = strlen( pszSource );
	AllocateBuffer( nLength );

	// Copy the string.
	mbstowcs( m_pszString, pszSource, nLength );
	return *this;
}
#endif // _UNICODE

// Assignment operator
const ClsString& ClsString::operator=( INT nNumber )
{
	// Set number.
	return operator=(( LONG )nNumber );
}
	
// Assignment operator
const ClsString& ClsString::operator=( LONG nNumber )
{
	TCHAR	szBuffer[ 64 ];

	// Set number.
	return operator=( _ltot( nNumber, szBuffer, 10 ));
}

// Assignment operator
const ClsString& ClsString::operator=( UINT nNumber )
{
	// Set number.
	return operator=(( ULONG )nNumber );
}
	
// Assignment operator
const ClsString& ClsString::operator=( ULONG nNumber )
{
	TCHAR	szBuffer[ 64 ];

	// Set number.
	return operator=( _ultot( nNumber, szBuffer, 10 ));
}

// Addition operator.
const ClsString& ClsString::operator+=( const ClsString& csSource )
{
	_ASSERT( &csSource != this ); // We can't add us to ourself.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer copying the current
	// contents.
	AllocateBuffer( GetStringLength() + csSource.GetStringLength(), TRUE );

	// Append string.
	_tcscat( m_pszString, csSource.m_pszString );
	return *this;
}

// Addition operator.
const ClsString& ClsString::operator+=( LPCTSTR pszSource )
{
	_ASSERT_VALID( pszSource ); // We can't add a NULL pointer.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer copying the current
	// contents.
	AllocateBuffer( GetStringLength() + ( int )_tcslen( pszSource ), TRUE );

	// Append string.
	_tcscat( m_pszString, pszSource );
	return *this;
}

// Addition operator.
const ClsString& ClsString::operator+=( TCHAR ch )
{
	_ASSERT( ch != _T( '\0' )); // We can't assign to 0 character.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer copying the current
	// contents.
	int nLength = GetStringLength();
	AllocateBuffer( nLength + 1, TRUE );

	// Append character.
	m_pszString[ nLength ] = ch;
	return *this;
}

#ifndef _UNICODE
// Addition operator.
const ClsString& ClsString::operator+=( LPCWSTR pszSource )
{
	_ASSERT_VALID( pszSource ); // We can't add a NULL pointer.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer copying the current
	// contents.
	int nLength = GetStringLength(), nNew = ( int )wcslen( pszSource );
	AllocateBuffer( nLength + nNew, TRUE );

	// Append string.
	wcstombs( &m_pszString[ nLength ], pszSource, ( nNew * sizeof( TCHAR )));
	return *this;
}
#else
// Addition operator.
const ClsString& ClsString::operator+=( LPCSTR pszSource )
{
	_ASSERT_VALID( pszSource ); // We can't add a NULL pointer.

	// Do not modify referenced data!
	CopyBeforeWrite();

	// Allocate a new buffer copying the current
	// contents.
	int nLength = GetStringLength(), nNew = strlen( pszSource );
	AllocateBuffer( nLength + nNew, TRUE );

	// Append string.
	mbstowcs( &m_pszString[ nLength ], pszSource, nNew );
	return *this;
}
#endif

// Addition operator
const ClsString& ClsString::operator+=( INT nNumber )
{
	// Append number.
	return operator+=(( LONG )nNumber );
}
	
// Addition operator
const ClsString& ClsString::operator+=( LONG nNumber )
{
	TCHAR	szBuffer[ 64 ];

	// Append number.
	return operator+=( _ltot( nNumber, szBuffer, 10 ));
}

// Addition operator
const ClsString& ClsString::operator+=( UINT nNumber )
{
	// Append number.
	return operator+=(( ULONG )nNumber );
}
	
// Addition operator
const ClsString& ClsString::operator+=( ULONG nNumber )
{
	TCHAR	szBuffer[ 64 ];

	// Append number.
	return operator+=( _ultot( nNumber, szBuffer, 10 ));
}

// Comparison operator.
BOOL ClsString::operator>=( const ClsString& csSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( csSource.m_pszString );
	return ( BOOL )( _tcscmp( m_pszString, csSource ) >= 0 );
}

// Comparison operator.
BOOL ClsString::operator>=( LPCTSTR pszSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( pszSource );
	return ( BOOL )( _tcscmp( m_pszString, pszSource ) >= 0 );
}

// Comparison operator.
BOOL ClsString::operator>( const ClsString& csSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( csSource.m_pszString );
	return ( BOOL )( _tcscmp( m_pszString, csSource ) > 0 );
}

// Comparison operator.
BOOL ClsString::operator>( LPCTSTR pszSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( pszSource );
	return ( BOOL )( _tcscmp( m_pszString, pszSource ) > 0 );
}

// Comparison operator.
BOOL ClsString::operator<( const ClsString& csSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( csSource.m_pszString );
	return ( BOOL )( _tcscmp( m_pszString, csSource ) < 0 );
}

// Comparison operator.
BOOL ClsString::operator<( LPCTSTR pszSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( pszSource );
	return ( BOOL )( _tcscmp( m_pszString, pszSource ) < 0 );
}

// Comparison operator.
BOOL ClsString::operator<=( const ClsString& csSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( csSource.m_pszString );
	return ( BOOL )( _tcscmp( m_pszString, csSource ) <= 0 );
}

// Comparison operator.
BOOL ClsString::operator<=( LPCTSTR pszSource ) const
{
	_ASSERT_VALID( m_pszString );
	_ASSERT_VALID( pszSource );
	return ( BOOL )( _tcscmp( m_pszString, pszSource ) <= 0 );
}

// Get a buffer (non-referenced).
LPTSTR ClsString::GetBuffer( int nChars )
{
	// Release current buffer.
	Release();

	// Allocate new buffer.
	AllocateBuffer( nChars );

	// Return pointer to it.
	return m_pszString;
}

// Allocate a new memory buffer.
void ClsString::AllocateBuffer( int nChars, BOOL bCopy /* = FALSE */ )
{
	_ASSERT( nChars >= 0 );

	// Preset to NULL.
	ClsStringBuf *pNew = NULL;

	// Do we already have a buffer?
	if ( m_pszString )
	{
		// Is the available size enough to hold
		// the requested size?
		if ( nChars > ( GetBuf()->GetMax() - 1 ))
			// We need to allocate a new buffer.
			pNew = ClsStringBuf::AllocBuffer( nChars );
	}
	else
		// We always need to allocate a new buffer
		// when there is no current buffer.
		pNew = ClsStringBuf::AllocBuffer( nChars );

	// Have we allocated a new buffer?
	if ( pNew )
	{
		// Copy the old contents?
		if ( bCopy && m_pszString )
			memcpy( pNew->GetString(), m_pszString, GetBuf()->GetMax() * sizeof( TCHAR ));

		// Free the old buffer.
		Release();

		// Setup the new buffer.
		m_pszString = pNew->GetString();
	}
	else
		// Setup the new length.
		GetBuf()->SetLength( nChars, TRUE );
}

// Make sure we do not modify referenced data.
void ClsString::CopyBeforeWrite()
{
	// Delete conversion.
	ReleaseConversion();

	// Do we have a string?
	if ( m_pszString )
	{
		// Is the current contents a referenced string?
		if ( GetBuf()->IsShared())
		{
			// Create a new string buffer which has enough room to hold
			// the original string.
			ClsStringBuf *pNew = ClsStringBuf::AllocBuffer( GetBuf()->GetMax());

			// Setup it's length.
			pNew->SetLength( GetBuf()->GetLength());

			// Copy the contents of the original to the new buffer.
			memcpy( pNew->GetString(), m_pszString, GetBuf()->GetLength() * sizeof( TCHAR ));

			// Release the reference.
			GetBuf()->Release();

			// Setup the new pointer.
			m_pszString = pNew->GetString();
		}
	}
}

// Release the string buffer and, if the reference
// count is below 1 free the memory.
void ClsString::Release()
{
	// Delete conversion.
	ReleaseConversion();

	// Do we have a string?
	if ( m_pszString )
	{
		// Release our reference.
		if ( GetBuf()->Release() <= 0 )
		{
			// Free the memory and clear the pointer.
			ClsStringBuf::FreeBuffer( GetBuf());
			m_pszString = NULL;
		}
	}
}

// Release the converted string if the
// conversion is present.
void ClsString::ReleaseConversion()
{
	// Is there a conversion?
	if ( m_pszConverted != NULL )
	{
		// Yes. Free the memory and reset the
		// pointer.
		delete[] m_pszConverted;
		m_pszConverted = NULL;
	}
}

#ifndef _UNICODE
// Convert the multibyte string to a wide-character
// string.
void ClsString::DoConversion()
{
	// Is there anything to convert?
	if ( m_pszString )
	{
		// Only perform the conversion if not
		// yet done.
		if ( m_pszConverted == NULL )
		{
			// Allocate the buffer to hold the conversion.
			m_pszConverted = new WCHAR[ GetStringLength() ];

			// Perform the conversion.
			mbstowcs( m_pszConverted, m_pszString, GetStringLength());
		}
	}
}

// We are already multibyte so simply return a pointer to the buffer.
LPSTR ClsString::GetMBS() { return m_pszString ? m_pszString : ""; }

// Return a pointer to the conversion or an
// empty string.
LPWSTR ClsString::GetWS()
{
	DoConversion();
	return m_pszConverted ? m_pszConverted : L"";
}
#else
// Convert the wide-character string to a multibyte
// string.
void ClsString::DoConversion()
{
	// Anything to convert?
	if ( m_pszString )
	{
		// Only perform the conversion if not
		// yet done.
		if ( m_pszConverted == NULL )
		{
			// Allocate a buffer to hold the conversion.
			m_pszConverted = new CHAR[ GetStringLength() ];

			// Do the actual conversion.
			wcstombs( m_pszConverted, m_pszString, GetStringLength());
		}
	}
}

// We are already wide-char so simply return a pointer to the buffer.
LPWSTR ClsString::GetWS() { return m_pszString ? m_pszString : L""; }

// Return a pointer to the conversion or an
// empty string.
LPSTR ClsString::GetMBS()
{
	DoConversion();
	return m_pszConverted ? m_pszConverted : "";
}
#endif