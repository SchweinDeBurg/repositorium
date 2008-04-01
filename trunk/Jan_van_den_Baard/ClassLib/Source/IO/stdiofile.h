#ifndef _STDIOFILE_H_
#define _STDIOFILE_H_
//
// stdiofile.h
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../exceptions/exception.h"

// A simple wrapper for the stdio FILE handle.
class ClsStdioFile
{
	_NO_COPY( ClsStdioFile );
public:
	// Error codes used in the thrown exceptions.
	enum
	{
		IOERR_OPEN,
		IOERR_CLOSE,
		IOERR_SEEK,
		IOERR_FLUSH,
		IOERR_READ,
		IOERR_WRITE,
		IOERR_GETPOS
	};

	// Default constructor.
	ClsStdioFile()
	{
		// Reset handle.
		m_hFile = NULL;
	}

	// Constructor. Opens passed file.
	ClsStdioFile( LPCTSTR pszFilename, LPCTSTR pszMode = _T( "r" ))
	{
		// Open the file.
		m_hFile = _tfopen( pszFilename, pszMode );
		if ( m_hFile == NULL )
			throw ClsException( IOERR_OPEN );
	}

	// Destructor. Closes the file.
	virtual ~ClsStdioFile()
	{
		// File open?
		if ( m_hFile )
		{
			// Close the file.
			if ( fclose( m_hFile ) != 0 )
				throw ClsException( IOERR_CLOSE );
		}
	}

	// Open a file.
	inline void Open( LPCTSTR pszFilename, LPCTSTR pszMode )
	{
		_ASSERT( m_hFile == NULL ); // May not be valid.

		// Open the file
		m_hFile = _tfopen( pszFilename, pszMode );
		if ( m_hFile == NULL )
			throw ClsException( IOERR_OPEN );
	}

	// Open a file.
	inline void Open( LPCTSTR pszFilename, LPCTSTR pszMode, int nShareFlag )
	{
		_ASSERT( m_hFile == NULL ); // May not be valid.

		// Open the file
		m_hFile = _tfsopen( pszFilename, pszMode, nShareFlag );
		if ( m_hFile == NULL )
			throw ClsException( IOERR_OPEN );
	}

	// Close the file.
	inline void Close()
	{
		// Close the handle.
		if ( m_hFile && fclose( m_hFile ) != 0 )
			throw ClsException( IOERR_CLOSE );

		// Clear handle.
		m_hFile = NULL;
	}

	// Test for end of file.
	inline BOOL Eof()
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Clear error flag.
		return ( BOOL )( feof( m_hFile ));
	}

	// Flush the file buffer.
	inline void Flush()
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Flush the buffer.
		if ( fflush( m_hFile ) != 0 )
			throw ClsException( IOERR_FLUSH );
	}

	// Read a character from the stream.
	inline TCHAR GetC()
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Read the character
		TCHAR ch = ( TCHAR )_fgettc( m_hFile );
		if ( ferror( m_hFile ))
			throw ClsException( IOERR_READ );
		return ch;
	}

	// Get the file position.
	inline fpos_t GetPos()
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Get the file position.
		fpos_t pos;
		if ( fgetpos( m_hFile, &pos ) != 0 )
			throw ClsException( IOERR_GETPOS );
		return pos;
	}

	// Get a string from the stream.
	inline LPTSTR GetS( LPTSTR pszBuffer, size_t cchMax )
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Read the string.
		pszBuffer = _fgetts( pszBuffer, ( int )cchMax, m_hFile );
		if ( ferror( m_hFile ) != 0 )
			throw ClsException( IOERR_READ );
		return pszBuffer;
	}

	// Format output to the stream.
	inline int PrintF( LPCTSTR pszFormat, ... )
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Format the output.
		va_list		args;
		va_start( args, pszFormat );
		int nChars = _vftprintf( m_hFile, pszFormat, args );
		va_end( args );

		// Error?
		if ( nChars < 0 )
			throw ClsException( IOERR_WRITE );

		// Return characters written.
		return nChars;
	}

	// Write a character to the stream.
	inline TCHAR PutC( TCHAR ch )
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Write the character
		TCHAR ch1 = ( TCHAR )_fputtc( ch, m_hFile );
		if ( ch1 != ch )
			throw ClsException( IOERR_WRITE );
		return ch1;
	}

	// Write a string to the stream.
	inline void PutS( LPTSTR pszBuffer )
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Write the string.
		if ( _fputts( pszBuffer, m_hFile ) < 0 )
			throw ClsException( IOERR_WRITE );
	}

	// Read a number of bytes from the stream.
	inline void Read( LPVOID pszBuffer, size_t size, size_t count = 1 )
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Write to the file.
		fread( pszBuffer, size, count, m_hFile );
		if ( ferror( m_hFile ) != 0 )
			throw ClsException( IOERR_READ );
	}

	// Write a number of bytes to the stream.
	inline void Write( LPVOID pszBuffer, size_t size, size_t count = 1 )
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Write to the file.
		fwrite( pszBuffer, size, count, m_hFile );
		if ( ferror( m_hFile ) != 0 )
			throw ClsException( IOERR_WRITE );
	}

	// Seek int the stream.
	inline void Seek( long offset, int origin )
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Seek.
		if ( fseek( m_hFile, offset, origin ) != 0 )
			throw ClsException( IOERR_SEEK );
	}

	// Rewind the stream.
	inline void Rewind()
	{
		_ASSERT_VALID( m_hFile ); // Must be valid.

		// Rewind.
		rewind(m_hFile );
	}

protected:
	// Data
	FILE	*m_hFile;
};

#endif // _STDIOFILE_H