// AfxGadgets library.
// Copyright (c) 2003-2004 by Elijah Zarezky,
// All rights reserved.
// Portions copyright (c) 2000 by Martin Ziacek.

// SeException.h - interface of the CSeException class

#if !defined(__SeException_h)
#define __SeException_h

class CSeException: public CException
{
	DECLARE_DYNAMIC(CSeException)

// construction/destruction
public:
	CSeException(UINT uCode, _EXCEPTION_POINTERS* pXcptPtrs);
	virtual ~CSeException(void);

// copying/assignment
public:
	CSeException(const CSeException& src);
	CSeException& operator =(const CSeException& src);

// operations
public:
	void Delete(void);
	virtual BOOL GetErrorMessage(LPTSTR pszDest, UINT uMaxLen, UINT* puHelpCtx = NULL);
	virtual int ReportError(UINT fuType = MB_OK, UINT uHelpID = 0);

// SEH translator
public:
	static void Translator(UINT uCode, _EXCEPTION_POINTERS* pXcptPtrs);

// accessibility
public:
	UINT GetCode(void);
	LPCTSTR GetDescription(void);
	void* GetAddress(void);

// attributes
protected:
	UINT m_uCode;
	_EXCEPTION_POINTERS m_xcptPtrs;

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

#endif	// __SeException_h

// end of file
