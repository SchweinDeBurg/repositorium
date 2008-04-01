// 
// file.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "file.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsFile::ClsFile()
{
	// Initialize data.
	m_hFile = INVALID_HANDLE_VALUE;
}

// Destructor.
ClsFile::~ClsFile()
{
	// Valid handle?
	if ( m_hFile != INVALID_HANDLE_VALUE )
	{
		// Close it.
		if ( ! ::CloseHandle( m_hFile ))
			throw ClsFileException();

		// Reset handle value.
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

// Open a file.
void ClsFile::Open( LPCTSTR pszFileName, int nFileMode )
{
	_ASSERT( m_hFile == INVALID_HANDLE_VALUE ); // May not be valid.

	// Determine the access mode.
	DWORD dwAccess = 0L;
	if ( nFileMode & fileRead      ) dwAccess |= GENERIC_READ;
	if ( nFileMode & fileWrite     ) dwAccess |= GENERIC_WRITE;
	if ( nFileMode & fileReadWrite ) dwAccess |= GENERIC_READ | GENERIC_WRITE;

	// And the file sharing mode.
	DWORD dwShareMode = 0L;
	if ( nFileMode & fileShareRead	    ) dwShareMode |= FILE_SHARE_READ;
	if ( nFileMode & fileShareWrite	    ) dwShareMode |= FILE_SHARE_WRITE;
	if ( nFileMode & fileShareExclusive ) dwShareMode = 0L;

	// Now we determine the creation of the file.
	DWORD dwCreate = 0;
	if ( nFileMode & fileCreateAlways )
	{
		_ASSERT( dwAccess & GENERIC_WRITE );
		_ASSERT(( nFileMode & ( fileCreate | fileAppend )) == 0 );
		dwCreate = CREATE_ALWAYS;
	}
	else if ( nFileMode & fileCreate )
	{
		_ASSERT( dwAccess & GENERIC_WRITE );
		_ASSERT(( nFileMode & ( fileCreateAlways | fileAppend )) == 0 );
		dwCreate = CREATE_NEW;
	}
	else if ( nFileMode & fileAppend )
	{
		_ASSERT( dwAccess & GENERIC_WRITE );
		_ASSERT(( nFileMode & ( fileCreate | fileCreateAlways )) == 0 );
		dwCreate = OPEN_ALWAYS;
	}
	else
	{
		_ASSERT( dwAccess & GENERIC_READ );
		dwCreate = OPEN_EXISTING;
	}

	// Create the handle.
	m_hFile = ::CreateFile( pszFileName, dwAccess, dwShareMode, NULL, dwCreate, dwCreate == OPEN_EXISTING ? FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN : FILE_ATTRIBUTE_NORMAL, NULL );
	if ( m_hFile == INVALID_HANDLE_VALUE )
		throw ClsFileException();

	// Are we in append mode?
	if ( nFileMode & fileAppend )
		// Yes. Set the file pointer at the end of the file.
		SeekToEnd();
}

// Close the existing handle.
void ClsFile::Close()
{
	// Close the handle.
	if ( m_hFile != INVALID_HANDLE_VALUE )
	{
		if ( m_hFile != INVALID_HANDLE_VALUE && ! ::CloseHandle( m_hFile ))
			throw ClsFileException();
	}

	// Reset handle value.
	m_hFile = INVALID_HANDLE_VALUE;
}

// Close the existing handle ignoring
// all error conditions.
void ClsFile::Abort()
{
	// Is the handle valid?
	if ( m_hFile != INVALID_HANDLE_VALUE )
	{
		// Close the handle.
		::CloseHandle( m_hFile );

		// Reset handle value.
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

// Read a number of bytes from the file.
DWORD ClsFile::Read( LPVOID lpBuffer, DWORD dwNumBytes )
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.
	_ASSERT_VALID( lpBuffer ); // Should be valid.

	// Any bytes to read?
	DWORD dwBytesRead = 0L;
	if ( dwNumBytes )
	{
		// Read the bytes from the file.
		if ( ! ::ReadFile( m_hFile, lpBuffer, dwNumBytes, &dwBytesRead, NULL ))
			throw ClsFileException();
	}
	return dwBytesRead;
}

// Write a number of bytes to a file.
DWORD ClsFile::Write( LPCVOID lpBuffer, DWORD dwNumBytes )
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.
	_ASSERT_VALID( lpBuffer ); // Should be valid.

	// Any bytes to write?
	DWORD dwBytesWritten = 0L;
	if ( dwNumBytes )
	{
		// Write the bytes to the file.
		if ( ! ::WriteFile( m_hFile, lpBuffer, dwNumBytes, &dwBytesWritten, NULL ))
			throw ClsFileException();
	}
	return dwBytesWritten;
}

// Flush the file buffers.
void ClsFile::Flush()
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// Flush the buffers.
	if ( ! ::FlushFileBuffers( m_hFile ))
		throw ClsFileException();
}

// Seek to a specific position.
void ClsFile::Seek( LONG lOffset, UINT nFrom )
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// Set the new file position.
	if ( ::SetFilePointer( m_hFile, lOffset, NULL, nFrom ) == 0xFFFFFFFF )
		throw ClsFileException();
}

// Set the file pointer.
DWORD ClsFile::SetFilePointer( LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod )
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// Set the new file position.
	DWORD dwPos = ::SetFilePointer( m_hFile, lDistanceToMove, lpDistanceToMoveHigh, dwMoveMethod );
	if ( dwPos == 0xFFFFFFFF )
	{
		// Error?
		if ( lpDistanceToMoveHigh == NULL || ::GetLastError() != NO_ERROR )
			throw ClsFileException();
	}
	return dwPos;
}

// Set the file pointer to the
// beginning of the file.
void ClsFile::SeekToBegin()
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// Set the file pointer to the start of the file.
	Seek( 0, seekBegin );
}

// Set the file pointer to the
// end of the file.
void ClsFile::SeekToEnd()
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// Set the file pointer to the end of the file.
	Seek( 0, seekEnd );
}

// Get the file size.
DWORD ClsFile::GetFileSize( LPDWORD lpdwFileSizeHigh /* = NULL */ ) const
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// Get the file size.
	DWORD dwSize = ::GetFileSize( m_hFile, lpdwFileSizeHigh );
	if ( dwSize == 0xFFFFFFFF )
	{
		// Error?
		if ( lpdwFileSizeHigh == NULL || ::GetLastError() != NO_ERROR )
			throw ClsFileException();
	}
	return dwSize;
}

// Get file information.
void ClsFile::GetFileInformation( BY_HANDLE_FILE_INFORMATION& fileInfo ) const
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// Get the information.
	if ( ! ::GetFileInformationByHandle( m_hFile, &fileInfo ))
		throw ClsFileException();
}

// Get the file type.
DWORD ClsFile::GetFileType() const
{
	_ASSERT( m_hFile != INVALID_HANDLE_VALUE ); // Must be valid.

	// return the file type.
	return ::GetFileType( m_hFile );
}