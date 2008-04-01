//
// findfile.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "findfile.h"
#include "../exceptions/memoryexception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Initializes data.
ClsFindFile::ClsFindFile()
{
	// Initialize data.
	m_bFoundFile = FALSE;
	m_hHandle   = NULL;
	memset(( void * )&m_findData, 0, sizeof( WIN32_FIND_DATA ));
}

// Destructor. Closes the handle when it
// is still opened.
ClsFindFile::~ClsFindFile()
{
	// Close the handle is it is still
	// open.
	// Close the handle.
	if ( m_hHandle )
		::FindClose( m_hHandle );
}

// Get the length (only the lower DWORD).
DWORD ClsFindFile::GetLength() const
{
	_ASSERT( m_bFoundFile );
	return m_findData.nFileSizeLow;
}

// Get the length.
DWORD ClsFindFile::GetLength( DWORD& nFileSizeHigh ) const
{
	_ASSERT( m_bFoundFile );

	// Store the high DWORD and return the
	// low DWORD.
	nFileSizeHigh = m_findData.nFileSizeHigh;
	return m_findData.nFileSizeLow;
}

// Get the file name including extention.
BOOL ClsFindFile::GetFileName( ClsString& strName ) const
{
	_ASSERT( m_bFoundFile );

	try
	{
		// Find the last slash.
		LPCTSTR pszSlash = _tcsrchr( &m_findData.cFileName[ 0 ], _T( '\\' ));
		if ( pszSlash )
		{
			// Get a pointer to the next character which is
			// either a 0-character or the first character
			// of the file name.
			pszSlash = _tcsinc( pszSlash );

			// Set the string to the result.
			strName = pszSlash;
		}
		else
			// Use the whole name.
			strName = &m_findData.cFileName[ 0 ];
		return TRUE;
	}
	catch( ClsMemoryException& me )
	{
		// Failure...
		me;
	}
	return FALSE;
}

// Get the file path.
BOOL ClsFindFile::GetFilePath( ClsString& strName ) const
{
	_ASSERT( m_bFoundFile );

	try
	{
		// Set the base string.
		strName = m_strBase;

		// Add the file name.
		strName += m_findData.cFileName;
		return TRUE;
	}
	catch( ClsMemoryException& me )
	{
		// Failure...
		me;
	}
	return FALSE;
}

// Get the file name excluding extention.
BOOL ClsFindFile::GetFileTitle( ClsString& strName ) const
{
	_ASSERT( m_bFoundFile );

	try
	{
		// First get the file name.
		if ( GetFileName( strName ))
		{
			// Find the offset of the extention.
			int nPos = strName.Find( _T( '.' ));
			if ( nPos != -1 )
				// Truncate the name here.
				strName.SetStringLength( nPos );
			return TRUE;
		}
	}
	catch( ClsMemoryException& me )
	{
		// Failure...
		me;
	}
	return FALSE;
}

// Get the file URL.
BOOL ClsFindFile::GetFileURL( ClsString& strName ) const
{
	_ASSERT( m_bFoundFile );

	try
	{
		// First we set the file:// string.
		strName = _T( "file://" );
		
		// Add the base/
		strName += m_strBase;

		// Than we add the path.
		strName += &m_findData.cFileName[ 0 ];
		return TRUE;
	}
	catch( ClsMemoryException& me )
	{
		// Failure...
		me;
	}
	return FALSE;
}

// Get root directory of the file.
BOOL ClsFindFile::GetRoot( ClsString& strName ) const
{
	_ASSERT( m_bFoundFile );

	try
	{
		// Set the name to the base directory.
		strName = m_strBase;
		return TRUE;
	}
	catch( ClsMemoryException& me )
	{
		// Failure...
		me;
	}
	return FALSE;
}

// See if the file name is "." or ".."
BOOL ClsFindFile::IsDots() const
{
	_ASSERT( m_bFoundFile );

	// Get the file name.
	ClsString str;
	if ( GetFileName( str ))
	{
		// Check the file name.
		if ( str.IsEqual( _T( "." )) || str.IsEqual( _T( ".." )))
			return 1;
		return 0;
	}
	// Failure...
	return -1;
}

// Close the handle.
void ClsFindFile::Close()
{
	// Close the handle.
	if ( m_hHandle )
	{
		::FindClose( m_hHandle );
		m_hHandle = NULL;
	}

	// Reset data.
	m_strBase.Empty();
	m_bFoundFile = FALSE;
	memset(( void * )&m_findData, 0, sizeof( WIN32_FIND_DATA ));
}

// Open the handle.
BOOL ClsFindFile::FindFile( LPCTSTR pszName )
{
	_ASSERT( m_hHandle == NULL || m_hHandle == INVALID_HANDLE_VALUE ); // Should not be is use!

	// Save the base.
	m_strBase = pszName;

	// Make sure we only have the directory.
	int nPos = m_strBase.ReverseFind( _T( '\\' ));
	if ( nPos != -1 ) m_strBase.SetStringLength( nPos + 1 );
	
	// Open the handle.
	if (( m_hHandle = ::FindFirstFile( pszName, &m_findData )) != INVALID_HANDLE_VALUE )
		m_bFoundFile = TRUE;
	else
		m_bFoundFile = FALSE;

	return m_bFoundFile;
}

// Get the next name.
BOOL ClsFindFile::FindNextFile()
{
	_ASSERT( m_bFoundFile );

	// Get the next file name.
	m_bFoundFile = ::FindNextFile( m_hHandle, &m_findData );
	return m_bFoundFile;
}