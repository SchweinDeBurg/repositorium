// AfxGadgets library.
// Copyright (c) 2004-2005 by Elijah Zarezky,
// All rights reserved.

// MemMapFile.h - interface of the CMemMapFile class

#if !defined(__MemMapFile_h)
#define __MemMapFile_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

class CMemMapFile: public CObject
{
	DECLARE_DYNAMIC(CMemMapFile)

// construction/destruction
public:
	CMemMapFile(void);
	virtual ~CMemMapFile(void);

// operations
public:
	void* Create(LPCTSTR pszFileName, BOOL fWritable, DWORD cbMaxSize = 0, LPCTSTR pszObjName = NULL);
	void* Open(BOOL fWritable, LPCTSTR pszObjName);
	void Close(void);
	void Flush(void);
	void* GetDataPtr(void);
	DWORD GetLength(void);

// attributes
public:
	CString m_strFileName;
	HANDLE m_hFile;
	CString m_strObjName;
	HANDLE m_hMapping;
	void* m_dataPtr;

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __MemMapFile_h

// end of file
