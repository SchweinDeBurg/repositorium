// AfxGadgets library.
// Copyright (c) 2004 by Elijah Zarezky,
// All rights reserved.

// MemMapFile.cpp - implementation of the CMemMapFile class

#include "stdafx.h"
#include "MemMapFile.h"
#include "Win32Error.h"

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// object model
IMPLEMENT_DYNAMIC(CMemMapFile, CObject)

CMemMapFile::CMemMapFile(LPCTSTR pszObjName):
m_strObjName(pszObjName),
m_hFile(INVALID_HANDLE_VALUE),
m_hMapping(NULL),
m_dataPtr(NULL)
{
}

CMemMapFile::~CMemMapFile(void)
{
	Close();
}

void* CMemMapFile::Create(LPCTSTR pszFileName, DWORD fdwAccess, DWORD cbMaxSize)
{
	ASSERT(AfxIsValidString(pszFileName));
	ASSERT((fdwAccess & GENERIC_READ) != 0);

	m_hFile = ::CreateFile(pszFileName, fdwAccess, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (m_hFile == INVALID_HANDLE_VALUE) {
		DWORD dwErrCode = ::GetLastError();
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}

	DWORD fdwProtect = (fdwAccess & GENERIC_WRITE) != 0 ? PAGE_READWRITE : PAGE_READONLY;
	m_hMapping = ::CreateFileMapping(m_hFile, NULL, fdwProtect, 0, cbMaxSize, m_strObjName);
	if (m_hMapping == NULL) {
		DWORD dwErrCode = ::GetLastError();
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}

	DWORD fdwAccView = fdwProtect == PAGE_READWRITE ? FILE_MAP_WRITE : FILE_MAP_READ;
	m_dataPtr = ::MapViewOfFile(m_hMapping, fdwAccView, 0, 0, 0);
	if (m_dataPtr == NULL) {
		DWORD dwErrCode = ::GetLastError();
		::CloseHandle(m_hMapping);
		m_hMapping = NULL;
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		CWin32Error* pXcpt = new CWin32Error(dwErrCode);
		throw pXcpt;
	}

	return (m_dataPtr);
}

void CMemMapFile::Close(void)
{
	if (m_dataPtr != NULL) {
		::UnmapViewOfFile(m_dataPtr);
		m_dataPtr = NULL;
		::CloseHandle(m_hMapping);
		m_hMapping = NULL;
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}
}

void* CMemMapFile::GetDataPtr(void)
{
	return (m_dataPtr);
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
		dumpCtx << "m_strObjName = " << m_strObjName;
		dumpCtx << "\nm_strFileName = " << m_strFileName;
		dumpCtx << "\nm_hFile = " << m_hFile;
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
