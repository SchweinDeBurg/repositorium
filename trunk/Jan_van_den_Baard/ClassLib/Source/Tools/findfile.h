#ifndef _FINDFILE_H_
#define _FINDFILE_H_
//
// findfile.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../strings/string.h"

// A wrapper class for the FindFirstFile(),
// FindNextFile() and FindClose() APIs.
class ClsFindFile
{
	_NO_COPY( ClsFindFile );
public:
	// Constructor.
	ClsFindFile();

	// Destructor.
	virtual ~ClsFindFile();

	// Attributes.
	DWORD GetLength() const;
	DWORD GetLength( DWORD& nFileSizeHigh ) const;
	BOOL GetFileName( ClsString& strName ) const;
	BOOL GetFilePath( ClsString& strPath ) const;
	BOOL GetFileTitle( ClsString& strTitle ) const;
	BOOL GetFileURL( ClsString& strURL ) const;
	BOOL GetRoot( ClsString& strRoot ) const;
	BOOL IsDots() const;
	inline void GetCreationTime( FILETIME& fileTime ) const { _ASSERT( m_bFoundFile ); fileTime = m_findData.ftCreationTime; }
	inline void GetLastAccessTime( FILETIME& fileTime ) const { _ASSERT( m_bFoundFile ); fileTime = m_findData.ftLastAccessTime; }
	inline void GetLastWriteTime( FILETIME& fileTime ) const { _ASSERT( m_bFoundFile ); fileTime = m_findData.ftLastWriteTime; }
	inline BOOL MatchesMask( DWORD dwMask ) const { _ASSERT( m_bFoundFile ); return ( BOOL )(( m_findData.dwFileAttributes & dwMask ) == dwMask ); }
	inline BOOL IsReadOnly()   const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY   ); }
	inline BOOL IsDirectory()  const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY  ); }
	inline BOOL IsCompressed() const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED ); }
	inline BOOL IsSystem()	   const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM     ); }
	inline BOOL IsHidden()     const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN     ); }
	inline BOOL IsTemporary()  const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY  ); }
	inline BOOL IsNormal()     const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_NORMAL     ); }
	inline BOOL IsArchived()   const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE    ); }
	inline BOOL IsEncrypted()  const { _ASSERT( m_bFoundFile ); return ( BOOL )( m_findData.dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED  ); }
	inline DWORD Attributes()  const { _ASSERT( m_bFoundFile ); return ( m_findData.dwFileAttributes ); }
	// Implementation.
	void Close();
	BOOL FindFile( LPCTSTR pszName );
	BOOL FindNextFile();

protected:
	ClsString		m_strBase;
	WIN32_FIND_DATA		m_findData;
	HANDLE			m_hHandle;
	BOOL			m_bFoundFile;
};

#endif // _FINDFILE_H_