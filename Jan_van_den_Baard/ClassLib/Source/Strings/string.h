#ifndef _STRING_H_
#define _STRING_H_
//
// string.h
//
// (C) Copyright 2000-2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

class ClsStringBuf
{
	_NO_COPY( ClsStringBuf );
public:
	// Reference counting.
	inline LONG AddRef()		{ ::InterlockedIncrement( &m_nRefCount ); return m_nRefCount; }
	inline LONG Release()		{ ::InterlockedDecrement( &m_nRefCount ); return m_nRefCount; }

	// Properties.
	inline int GetLength() const	{ return m_nLength; }
	inline int GetMax() const	{ return m_nMax; }
	inline TCHAR *GetString() const	{ return ( TCHAR * )( this + 1 ); }
	inline LONG GetRefs() const	{ return m_nRefCount; }
	inline BOOL IsShared() const	{ return ( BOOL )( m_nRefCount > 1 ); }

	// Set string length. Values must lie between
	// 0 and the current length or buffer maximum.
	inline void SetLength( int nNewLen, BOOL OnMax = FALSE )
	{
		// Assert when the new length is larger than the old length
		// or when the length is larger than the maximum.
		#ifdef _DEBUG
			if ( OnMax ) _ASSERT( nNewLen >= 0 && nNewLen <  m_nMax  );
			else	     _ASSERT( nNewLen >= 0 && nNewLen <= m_nLength  );
		#endif
		m_nLength = nNewLen;
		GetString()[ nNewLen ] = _T( '\0' );
	}

	// Allocate a new string buffer.
	static ClsStringBuf *AllocBuffer( int nChars )
	{
		// This should avoid heap-fragmentation a bit.
		int	nMax = 0;
		if (	  nChars < 64  ) nMax = 64;
		else if ( nChars < 128 ) nMax = 128;
		else if ( nChars < 256 ) nMax = 256;
		else if ( nChars < 320 ) nMax = 320;
		else if ( nChars < 384 ) nMax = 384;
		else if ( nChars < 448 ) nMax = 448;
		else if ( nChars < 512 ) nMax = 512;
		else			 nMax = (( nChars + 7 ) & -8 ) + 8;

		// Allocate the memory required to hold the object and
		// the string including a termination 0-character.
		ClsStringBuf *pStr = ( ClsStringBuf * )new BYTE[ sizeof( ClsStringBuf ) + (( nMax + 1 ) * sizeof( TCHAR )) ];

		// Setup the object.
		pStr->m_nRefCount = 1;
		pStr->m_nLength	  = nChars;
		pStr->m_nMax	  = nMax;
		pStr->GetString()[ nChars ] = _T( '\0' );

		// Return a pointer to it.
		return pStr;
	}

	// Free a string buffer.
	static void FreeBuffer( ClsStringBuf *pStr )
	{
		// Free the occupied memory.
		_ASSERT_VALID( pStr );
		delete [] ( BYTE * )pStr;
	}

private:
	// Data.
	LONG	m_nRefCount;
	int	m_nLength;
	int	m_nMax;
};

class ClsString
{
public:
	// Construction.
	ClsString();
	ClsString( const ClsString& csSource );
	ClsString( TCHAR ch, int nRepeat = 1 );
	ClsString( HWND hWnd );
	ClsString( LPCTSTR pszSource, int nChars = -1 );
	#ifndef _UNICODE
	ClsString( LPCWSTR pszSource, int nChars = -1 );
	#else
	ClsString( LPCSTR pszSource, int nChars = -1 );
	#endif

	// Destruction.
	virtual ~ClsString();
	
	// Implementation.
	void Empty();
	void SetStringLength( int nLength );
	int GetWindowText( HWND );
	BOOL IsEqual( const ClsString& csSource ) const;
	BOOL IsEqual( LPCTSTR pszSource ) const;
	LPTSTR GetBuffer( int nChars );
	void AllocateBuffer( int nChars, BOOL bCopy = FALSE );
	TCHAR GetAt( int nIndex ) const;
	TCHAR& GetAt( int nIndex );
	void SetAt( int nIndex, TCHAR ch );
	int Compare( LPCTSTR pszString ) const;
	int CompareNoCase( LPCTSTR pszString ) const;
	int Collate( LPCTSTR pszString ) const;
	int CollateNoCase( LPCTSTR pszString ) const;
	ClsString Left( int nCount ) const;
	ClsString Mid( int nIndex, int nCount = -1 ) const;
	ClsString Right( int nCount ) const;
	void UpperCase();
	void LowerCase();
	void SwapCase();
	int Replace( TCHAR chOld, TCHAR chNew );
	int Remove( TCHAR ch );
	int Insert( int nIndex, TCHAR ch );
	int Insert( int nIndex, LPCTSTR pszString );
	int Insert( int nIndex, ClsString& csString );
	int Delete( int nIndex, int nCount = 1 );
	int Find( TCHAR ch, int nIndex = 0 ) const;
	int Find( LPCTSTR pszString, int nIndex = 0 ) const;
	int ReverseFind( TCHAR ch ) const;
	int FindOneOf( LPCTSTR pszCharSet, int nIndex = 0 ) const;
	void FormatMessage( DWORD dwErrorID = 0 );
	void Format( LPCTSTR pszFormatString, ... );
	int GetDelimitedPart( TCHAR ch, int nIndex, ClsString& str ) const;
	BOOL ClsString::LoadString( UINT uID );
	inline int GetStringLength() const	{ return m_pszString ? GetBuf()->GetLength() : 0; }
	inline BOOL IsEmpty() const		{ return ( BOOL )(( m_pszString == NULL ) || ( GetBuf()->GetLength() == 0 )); }
	ClsString& Trim();

	// Conversions.
	LPWSTR GetWS();
	LPSTR GetMBS();
	void ReleaseConversion();

	// Conversion operator overloads.
	inline operator LPCTSTR() const		{ return m_pszString ? m_pszString : _T( "" ); }
	inline operator LPTSTR() 		{ return m_pszString ? m_pszString : _T( "" ); }

	// Index operator overload.
	inline TCHAR operator[]( int nIndex ) const	{ return GetAt( nIndex ); }
	inline TCHAR& operator[]( int nIndex )		{ return GetAt( nIndex ); }

	// Assignement operator overloads.
	const ClsString& operator=( const ClsString& csSource );
	const ClsString& operator=( LPCTSTR pszSource );
	const ClsString& operator=( TCHAR ch );
	const ClsString& operator=( HWND hWnd );
	#ifndef _UNICODE
	const ClsString& operator=( LPCWSTR pszSource );
	#else
	const ClsString& operator=( LPCSTR pszSource );
	#endif
	const ClsString& operator=( INT nNumber );
	const ClsString& operator=( LONG nNumber );
	const ClsString& operator=( UINT nNumber );
	const ClsString& operator=( ULONG nNumber );

	// Addition operator overloads.
	const ClsString& operator+=( const ClsString& csSource );
	const ClsString& operator+=( LPCTSTR csSource );
	#ifndef _UNICODE
	const ClsString& operator+=( LPCWSTR pszSource );
	#else
	const ClsString& operator+=( LPCSTR pszSource );
	#endif
	const ClsString& operator+=( TCHAR ch );
	const ClsString& operator+=( INT nNumber );
	const ClsString& operator+=( LONG nNumber );
	const ClsString& operator+=( UINT nNumber );
	const ClsString& operator+=( ULONG nNumber );

	// Comparison operator overloads.
	inline BOOL operator==( const ClsString& csSource ) const	{ return IsEqual( csSource ); }
	inline BOOL operator==( LPCTSTR pszSource ) const		{ return IsEqual( pszSource ); }
	inline BOOL operator!=( const ClsString& csSource ) const	{ return !IsEqual( csSource ); }
	inline BOOL operator!=( LPCTSTR pszSource ) const		{ return !IsEqual( pszSource ); }
	BOOL operator>=( const ClsString& csSource ) const;
	BOOL operator>=( LPCTSTR pszSource ) const;
	BOOL operator>( const ClsString& csSource ) const;
	BOOL operator>( LPCTSTR pszSource ) const;
	BOOL operator<( const ClsString& csSource ) const;
	BOOL operator<( LPCTSTR pszSource ) const;
	BOOL operator<=( const ClsString& csSource ) const;
	BOOL operator<=( LPCTSTR pszSource ) const;

protected:
	// Helpers.
	void CopyBeforeWrite();
	void Release();
	void DoConversion();
	inline const ClsStringBuf *GetBuf() const { return (( ClsStringBuf * )m_pszString ) - 1; }
	inline ClsStringBuf *GetBuf() { return (( ClsStringBuf * )m_pszString ) - 1; }

	// Data.
	TCHAR		*m_pszString;
	TCHAR		 m_NilChar;

#ifndef _UNICODE
	WCHAR		*m_pszConverted;
#else
	CHAR		*m_pszConverted;
#endif
};

// Addition operators.
inline ClsString operator+( const ClsString& str1, const ClsString str2 )
{ ClsString s( str1 ); s += str2; return s; }
inline ClsString operator+( const ClsString& str1, TCHAR ch )
{ ClsString s( str1 ); s += ch; return s; }
inline ClsString operator+( const ClsString str1, LPCTSTR str2 )
{ ClsString s( str1 ); s += str2; return s; }
inline ClsString operator+( TCHAR ch, const ClsString str2 )
{ ClsString s( ch ); s += str2; return s; } 
inline ClsString operator+( LPCTSTR str1, const ClsString str2 )
{ ClsString s( str1 ); s += str2; return s; }

#endif // _STRING_H_