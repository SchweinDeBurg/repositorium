// AfxGadgets library.
// Copyright (c) 2004-2005 by Elijah Zarezky,
// All rights reserved.

// MemMapFile.cpp - implementation of the CMemMapFile class

#include "stdafx.h"
#include "MemMapFile.h"
#include "Win32Error.h"

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
#endif	// __INTEL_COMPILER

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// object model
IMPLEMENT_DYNAMIC(CMemMapFile, CObject)

CMemMapFile::CMemMapFile(void):
m_hFile(INVALID_HANDLE_VALUE),
m_hMapping(NULL),
m_dataPtr(NULL)
{
}

CMemMapFile::~CMemMapFile(void)
{
	Close();
}

void* CMemMapFile::Create(LPCTSTR pszFileName, BOOL fWritable, DWORD cbMaxSize, LPCTSTR pszObjName)
{
	ASSERT(m_hFile == INVALID_HANDLE_VALUE);
	ASSERT(m_hMapping == NULL);
	ASSERT(m_dataPtr == NULL);

	ASSERT(AfxIsValidString(pszFileName));
	ASSERT(::lstrlen(pszFileName) > 0);

	DWORD fdwMode = fWritable ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ;
	m_hFile = ::CreateFile(pszFileName, fdwMode, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE) {
		DWORD dwErrCode = ::GetLastError();
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}
	else if (cbMaxSize == 0 && ::GetFileSize(m_hFile, NULL) == 0) {
		// special case - zero-length file and no growing allowed
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		return (NULL);
	}

	DWORD fdwProtect = fWritable ? PAGE_READWRITE : PAGE_READONLY;
	m_hMapping = ::CreateFileMapping(m_hFile, NULL, fdwProtect, 0, cbMaxSize, pszObjName);
	if (m_hMapping == NULL) {
		DWORD dwErrCode = ::GetLastError();
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}

	DWORD fdwAccess = fWritable ? FILE_MAP_WRITE : FILE_MAP_READ;
	m_dataPtr = ::MapViewOfFile(m_hMapping, fdwAccess, 0, 0, 0);
	if (m_dataPtr == NULL) {
		DWORD dwErrCode = ::GetLastError();
		::CloseHandle(m_hMapping);
		m_hMapping = NULL;
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}

	m_strFileName = pszFileName;
	m_strObjName = pszObjName;

	return (m_dataPtr);
}

void* CMemMapFile::Open(BOOL fWritable, LPCTSTR pszObjName)
{
	ASSERT(m_hFile == INVALID_HANDLE_VALUE);
	ASSERT(m_hMapping == NULL);
	ASSERT(m_dataPtr == NULL);

	ASSERT(AfxIsValidString(pszObjName));
	ASSERT(::lstrlen(pszObjName) > 0);

	DWORD fdwAccess = fWritable ? FILE_MAP_WRITE : FILE_MAP_READ;

	m_hMapping = ::OpenFileMapping(fdwAccess, FALSE, pszObjName);
	if (m_hMapping == NULL) {
		DWORD dwErrCode = ::GetLastError();
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}

	m_dataPtr = ::MapViewOfFile(m_hMapping, fdwAccess, 0, 0, 0);
	if (m_dataPtr == NULL) {
		DWORD dwErrCode = ::GetLastError();
		::CloseHandle(m_hMapping);
		m_hMapping = NULL;
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}

	m_strObjName = pszObjName;

	return (m_dataPtr);
}

void CMemMapFile::Close(void)
{
	if (m_dataPtr != NULL) {
		::UnmapViewOfFile(m_dataPtr);
		m_dataPtr = NULL;
		::CloseHandle(m_hMapping);
		m_hMapping = NULL;
		if (m_hFile != INVALID_HANDLE_VALUE) {
			::CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
		m_strObjName.Empty();
		m_strFileName.Empty();
	}
}

void CMemMapFile::Flush(void)
{
	ASSERT(m_dataPtr != NULL);

	if (!::FlushViewOfFile(m_dataPtr, 0)) {
		DWORD dwErrCode = ::GetLastError();
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}
}

void* CMemMapFile::GetDataPtr(void)
{
	return (m_dataPtr);
}

DWORD CMemMapFile::GetLength(void)
{
	return (m_hFile != INVALID_HANDLE_VALUE ? ::GetFileSize(m_hFile, NULL) : 0);
}

#if defined(_DEBUG)

void CMemMapFile::AssertValid(void) const
{
	// first perform inherited validity check...
	CObject::AssertValid();
	// ...and then verify our own state as well
}

void CMemMapFile::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CObject::Dump(dumpCtx);
		// ...and then dump own unique members
		dumpCtx << "m_strFileName = " << m_strFileName;
		dumpCtx << "\nm_hFile = " << m_hFile;
		dumpCtx << "\nm_strObjName = " << m_strObjName;
		dumpCtx << "\nm_hMapping = " << m_hMapping;
		dumpCtx << "\nm_dataPtr = " << m_dataPtr;
	}
	catch (CFileException* pXcpt) {
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
