// CJMemFile.h: interface for the CCJMemFile class.
//
// Copyright © 1999 Maxallion
// mailto:XFile@maxallion.8m.com
// ICQ# 32304418
//
// CCJMemFile - Extended Memory File - Beta - w/o a lot of error checking
// - Is used like a normal CFile or CStdioFile or CMemFile object
// - String Functions : ReadString, WriteString
// - Loads physical files into memory on creation and saves them back to disk on destruction
// - Can duplicate itself to other CFile derived objects
// - Has a Search function
// - can be read-accessed like an array
//
// OVERLOADED OPERATORS:
// = Imports from another file or sets file Position
// += Appends another file
// [] reads a byte like an array
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 24/05/02 13:52 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/Include/CJMemFile.h $
 *
 * $History: CJMemFile.h $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 24/05/02   Time: 13:52
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/24/99   Time: 3:01a
 * Created in $/CodeJock/Include
 * Initial creation.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJMEMFILE_H__
#define __CJMEMFILE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class _CJX_EXT_CLASS CCJMemFile : public CMemFile  
{
private:
	CFileException* m_pError;
	CFile m_File;
	UINT m_uiOpenFlags;
	bool m_bOpen;

public:
	CCJMemFile( UINT nGrowBytes = 1024 );
	CCJMemFile( BYTE* lpBuffer, UINT nBufferSize, UINT nGrowBytes = 0 );
	CCJMemFile(LPCTSTR lpstFilename, UINT uiOpenFlags);
	virtual ~CCJMemFile();

protected:
	// Load() loads the file into Memory
	virtual bool Load();  
	
	// Save() saves the file to Disk
	virtual bool Save();
	
	// Import() imports the data of a CFile derived object (operator = )
	virtual bool Import(CFile *fImp);

	// Appends a CFile derived object to the file (operator += )
	virtual bool Append(CFile *fApp);

public:
	// Flush() saves the memory to disk 
	virtual void Flush();
	
	// Open(...) opens and loads a physical File into memory
	virtual bool Open(CString strFilename, UINT uiOpenFlags, CFileException *pError = NULL);
	
	// Close() saves the contents of the memory to the disk and closes it.
	virtual void Close();

	// ReadString(...) reads a string
	virtual BOOL ReadString(CString& rString); 

	// WriteString(...) appends a string
	virtual void WriteString( LPCTSTR lpsz );

	// Duplicate(...) Duplicates the CCJMemFile to a CFile derived class
	virtual bool Duplicate(CFile *fDuplicate);
	virtual bool Duplicate(CString strDup);

	// Discard() discards all changes to file since Open() or last Flush()
	virtual bool Discard();

	// Insert(...) inserts any File and returns the length of actual copied bytes
#if _MSC_VER >= 1300		// VC 7.0 (.NET)
	virtual ULONGLONG Insert(CFile* fSrc, LONGLONG dwSourcePos, LONGLONG dwDestPos, LONGLONG dwBytes);
	virtual ULONGLONG Insert(CString strSrc, LONGLONG dwSourcePos, LONGLONG dwDestPos, LONGLONG dwBytes);

	// Extract(...) Extracts bytes to a file and returns the length of actual copied bytes
	virtual ULONGLONG Extract(CFile *fDest,LONGLONG dwStartPos, LONGLONG dwBytes);
	virtual ULONGLONG Extract(CString strDest, LONGLONG dwStartPos, LONGLONG dwBytes);
#else						// VC 5.0/6.0
	virtual DWORD Insert(CFile* fSrc, DWORD dwSourcePos, DWORD dwDestPos, DWORD dwBytes);
	virtual DWORD Insert(CString strSrc, DWORD dwSourcePos, DWORD dwDestPos, DWORD dwBytes);

	// Extract(...) Extracts bytes to a file and returns the length of actual copied bytes
	virtual DWORD Extract(CFile *fDest,DWORD dwStartPos, DWORD dwBytes);
	virtual DWORD Extract(CString strDest, DWORD dwStartPos, DWORD dwBytes);
#endif

	// Find(...) Finds data in the file
#if (_MFC_VER < 0x0700)
	LONG FindData(void* pData, DWORD dwDataLen, LONG lStartPos);
#else
	LONGLONG FindData(void* pData, DWORD dwDataLen, LONG lStartPos);
#endif   // _MFC_VER

// Overloaded Operators
public:
	// Importing Operators
	void operator =( CFile *fDup);
	void operator =( CString strDup);

	// Positioning
	void operator =( DWORD dwFilePos);
	
	// Appending operators
	void operator +=( CFile *fApp);
	void operator +=( CString strApp);

	// Indexing Operators
	BYTE operator [](DWORD dwFilePos);


};

#endif // !defined(__CJMEMFILE_H__)
