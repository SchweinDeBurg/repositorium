// AfxGadgets library.
// Copyright (c) 2003-2005 by Elijah Zarezky,
// All rights reserved.
// Portions copyright (c) 2001 by Ajit Jadhav.

// Win32Error.h - interface of the CWin32Error class

#if !defined(__Win32Error_h)
#define __Win32Error_h

class CWin32Error: public CException
{
	DECLARE_DYNAMIC(CWin32Error)

// construction/destruction
public:
	CWin32Error(void);
	CWin32Error(DWORD dwErrCode);
	virtual ~CWin32Error(void);

// operations
public:
	void Delete(void);
	virtual BOOL GetErrorMessage(LPTSTR pszDest, UINT uMaxLen, UINT* puHelpCtx = NULL);
	virtual int ReportError(UINT fuType = MB_OK, UINT uHelpID = 0);

// attributes
protected:
	DWORD m_dwErrCode;

// implementation helpers
private:
	BOOL FormatErrorMessage(CString& strDest);

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __Win32Error_h

// end of file
