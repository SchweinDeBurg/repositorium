// AfxGadgets library.
// Copyright (c) 2004 by Elijah Zarezky,
// All rights reserved.

// MemMapFile.h - interface of the CMemMapFile class

#if !defined(__MemMapFile_h)
#define __MemMapFile_h

class CMemMapFile: public CObject
{
	DECLARE_DYNAMIC(CMemMapFile)

// construction/destruction
public:
	CMemMapFile(LPCTSTR pszObjName = NULL);
	virtual ~CMemMapFile(void);

// operations
public:
	void* Create(LPCTSTR pszFileName, DWORD fdwAccess, DWORD cbMaxSize = 0);
	void Close(void);
	void* GetDataPtr(void);

// attributes
public:
	CString m_strObjName;
	CString m_strFileName;
	HANDLE m_hFile;
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
