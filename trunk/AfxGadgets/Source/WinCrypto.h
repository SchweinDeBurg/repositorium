// AfxGadgets library.
// Copyright (c) 2003-2005 by Elijah Zarezky,
// All rights reserved.

// WinCrypto.h - interface of the CWinCrypto class

#if !defined(__WinCrypto_h)
#define __WinCrypto_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400)
#error this code uses features only available under Windows 98/NT4 or later
#endif	// _WIN32_WINNT

#if !defined(__AFXTEMPL_H__)
#pragma message(__FILE__ " : put <afxtempl.h> in your PCH to speed up compilation")
#include <afxtempl.h>
#endif	// __AFXTEMPL_H__

#if !defined(__WINCRYPT_H__)
#pragma message(__FILE__ " : put <wincrypt.h> in your PCH to speed up compilation")
#include <wincrypt.h>
#endif	// __WINCRYPT_H__

class CWinCrypto: public CObject
{
	DECLARE_DYNAMIC(CWinCrypto)

// construction/destruction
public:
	CWinCrypto(LPCTSTR pszContainer, ALG_ID algID = CALG_DES, LPCTSTR pszPassword = NULL);
	virtual ~CWinCrypto(void);

// copying/assignment - not allowed and thus not implemented
private:
	CWinCrypto(const CWinCrypto& src);
	CWinCrypto& operator =(const CWinCrypto& src);

// operations
public:
	void GenerateKey(ALG_ID algID = CALG_DES, LPCTSTR pszPassword = NULL);
	DWORD EncryptString(BSTR bstrSrc, CArray<BYTE, BYTE>& arrDest);
	DWORD DecryptString(CArray<BYTE, BYTE>& arrSrc, BSTR* pbstrDest);
	
// attributes
public:
	HCRYPTPROV m_hContext;
	HCRYPTKEY m_hKey;

// implementation helpers
private:
	DWORD AcquireContext(LPCTSTR pszContainer);

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __WinCrypto_h

// end of file
