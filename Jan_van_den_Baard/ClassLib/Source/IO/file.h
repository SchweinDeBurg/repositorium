#ifndef _FILE_H_
#define _FILE_H_
//
// file.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../exceptions/fileexception.h"

// A simple wrapper for the Win32 file
// functions.
class ClsFile
{
	_NO_COPY( ClsFile );
public:
	// File types.
	enum
	{
		fileCreate	   = 0x00000001,
		fileCreateAlways   = 0x00000002,
		fileAppend	   = 0x00000004,
		fileRead	   = 0x00000008,
		fileWrite	   = 0x00000010,
		fileReadWrite	   = 0x00000020,
		fileShareRead	   = 0x00000040,
		fileShareWrite	   = 0x00000080,
		fileShareExclusive = 0x00000100
	};

	// Seek modes.
	enum
	{
		seekBegin   = FILE_BEGIN,
		seekCurrent = FILE_CURRENT,
		seekEnd	    = FILE_END
	};

	// Constructor.
	ClsFile();

	// Destructor.
	virtual ~ClsFile();

	// Implementation.
	void Open( LPCTSTR pszFileName, int nFileMode );
	void Close();
	void Abort();
	DWORD Read( LPVOID lpBuffer, DWORD dwNumBytes );
	DWORD Write( LPCVOID lpBuffer, DWORD dwNumBytes );
	void Flush();
	void Seek( LONG lOffset, UINT nFrom );
	DWORD SetFilePointer( LONG lDistanceToMove, PLONG lpDistanceToMoveHigh, DWORD dwMoveMethod );
	void SeekToBegin();
	void SeekToEnd();
	DWORD GetFileSize( LPDWORD lpdwFileSizeHigh = NULL ) const;
	void GetFileInformation( BY_HANDLE_FILE_INFORMATION& fileInfo ) const;
	DWORD GetFileType() const;

	// operators.
	inline operator HANDLE()
	{ return m_hFile; }

protected:
	// Data.
	HANDLE	m_hFile;
};

#endif // _FILE_H_
