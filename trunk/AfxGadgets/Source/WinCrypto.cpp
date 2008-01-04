// AfxGadgets library.
// Copyright (c) 2004-2008 by Elijah Zarezky,
// All rights reserved.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// WinCrypto.cpp - implementation of the CWinCrypto class

#include "stdafx.h"
#include "WinCrypto.h"
#include "Win32Error.h"

#if defined(__INTEL_COMPILER)
// warning #68: integer conversion resulted in a change of sign
#pragma warning(disable: 68)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
#endif	// __INTEL_COMPILER

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// object model
IMPLEMENT_DYNAMIC(CWinCrypto, CObject)

CWinCrypto::CWinCrypto(LPCTSTR pszContainer, ALG_ID algID, LPCTSTR pszPassword):
m_hContext(NULL),
m_hKey(NULL)
{
	DWORD dwErrCode;

	// precondition
	ASSERT(AfxIsValidString(pszContainer));

	if ((dwErrCode = AcquireContext(pszContainer)) != ERROR_SUCCESS)
	{
		throw new CWin32Error(dwErrCode);
	}
	else try
	{
		GenerateKey(algID, pszPassword);
	}
	catch (CWin32Error*)
	{
		if (m_hKey != NULL)
		{
			::CryptDestroyKey(m_hKey);
			m_hKey = NULL;
		}
		::CryptReleaseContext(m_hContext, 0);
		m_hContext = NULL;
		THROW_LAST();
	}
}

CWinCrypto::~CWinCrypto(void)
{
	// simple clean-up
	::CryptDestroyKey(m_hKey);
	::CryptReleaseContext(m_hContext, 0);
}

void CWinCrypto::GenerateKey(ALG_ID algID, LPCTSTR pszPassword)
{
	HW_PROFILE_INFO hwProfileInfo;
	DWORD dwErrCode;

	// precondition
	ASSERT(m_hContext != NULL);

	// destroy current key (if any)
	if (m_hKey != NULL) {
		::CryptDestroyKey(m_hKey);
		m_hKey = NULL;
	}

	if (pszPassword == NULL || *pszPassword == 0)
	{
		// obtain default password
		::GetCurrentHwProfile(&hwProfileInfo);
		pszPassword = hwProfileInfo.szHwProfileGuid;
	}
	ASSERT(AfxIsValidString(pszPassword));

	// try to create a hash object
	HCRYPTHASH hHash = NULL;
	if (!::CryptCreateHash(m_hContext, CALG_MD5, NULL, 0, &hHash))
	{
		dwErrCode = ::GetLastError();
		throw new CWin32Error(dwErrCode);
	}
	ASSERT(hHash != NULL);

	// hash the password
	const BYTE* pbKeyData = reinterpret_cast<const BYTE*>(pszPassword);
	int nDataLen = ::lstrlen(pszPassword) * sizeof(TCHAR);
	if (!::CryptHashData(hHash, pbKeyData, nDataLen, 0))
	{
		dwErrCode = ::GetLastError();
		::CryptDestroyHash(hHash);
		throw new CWin32Error(dwErrCode);
	}

	// derive a session key from the hash object
	if (!::CryptDeriveKey(m_hContext, algID, hHash, 0, &m_hKey))
	{
		dwErrCode = ::GetLastError();
		::CryptDestroyHash(hHash);
		throw new CWin32Error(dwErrCode);
	}
	ASSERT(m_hKey != NULL);

	// clean-up
	::CryptDestroyHash(hHash);
}

DWORD CWinCrypto::EncryptString(BSTR bstrSrc, CArray<BYTE, BYTE>& arrDest)
{
	DWORD cbBufSize;

	// precondition
	ASSERT(m_hContext != NULL);
	ASSERT(m_hKey != NULL);
	ASSERT(bstrSrc != NULL);
	ASSERT(::SysStringLen(bstrSrc) > 0);

	DWORD cbDataSize = cbBufSize = ::SysStringByteLen(bstrSrc);
	if (::CryptEncrypt(m_hKey, NULL, TRUE, 0, NULL, &cbBufSize, 0))
	{
		arrDest.SetSize(cbBufSize);
		BYTE* pbData = arrDest.GetData();
		memcpy(pbData, bstrSrc, cbDataSize);
		if (::CryptEncrypt(m_hKey, NULL, TRUE, 0, pbData, &cbDataSize, cbBufSize))
		{
			ASSERT(cbDataSize == cbBufSize);
			return (cbDataSize);
		}
	}
	throw new CWin32Error(::GetLastError());
}

DWORD CWinCrypto::DecryptString(CArray<BYTE, BYTE>& arrSrc, BSTR* pbstrDest)
{
	// precondition
	ASSERT(m_hContext != NULL);
	ASSERT(m_hKey != NULL);
	ASSERT(arrSrc.GetSize() > 0);
	ASSERT(pbstrDest != NULL);

	DWORD cbBufSize = arrSrc.GetSize();
	BYTE* pbData = arrSrc.GetData();
	if (::CryptDecrypt(m_hKey, NULL, TRUE, 0, pbData, &cbBufSize))
	{
		ASSERT(cbBufSize > 0);
		cbBufSize /= sizeof(OLECHAR);
		*pbstrDest = ::SysAllocStringLen(reinterpret_cast<OLECHAR*>(pbData), cbBufSize);
		return (cbBufSize);
	}
	else {
		throw new CWin32Error(::GetLastError());
	}
}

DWORD CWinCrypto::AcquireContext(LPCTSTR pszContainer)
{
	DWORD dwErrCode;

	// precondition
	ASSERT(AfxIsValidString(pszContainer));

	if (::CryptAcquireContext(&m_hContext, pszContainer, NULL, PROV_RSA_FULL, 0))
	{
		// successfully acquired
		dwErrCode = ERROR_SUCCESS;
	}
	// if the key container doesn't exist...
	else if ((dwErrCode = ::GetLastError()) == NTE_BAD_KEYSET)
	{
		// ...then try to create it for the first time
		if (::CryptAcquireContext(&m_hContext, pszContainer, NULL, PROV_RSA_FULL, CRYPT_NEWKEYSET))
		{
			// successfully created
			dwErrCode = ERROR_SUCCESS;
		}
		else {
			dwErrCode = ::GetLastError();
		}
	}
	return (dwErrCode);
}

#if defined(_DEBUG)

void CWinCrypto::AssertValid(void) const
{
	// first perform inherited validity check...
	CObject::AssertValid();

	// ...and then verify own state as well
	ASSERT(m_hContext != NULL);
	ASSERT(m_hKey != NULL);
}

void CWinCrypto::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CObject::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_hContext = " << m_hContext << "\n";
		dumpCtx << "m_hKey = " << m_hKey;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// import libraries
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "oleaut32.lib")

// end of file
