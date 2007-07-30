// AfxGadgets library.
// Copyright (c) 2004-2007 by Elijah Zarezky,
// All rights reserved.
// Portions copyright (c) 2001 by Ajit Jadhav.

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

// Win32Error.cpp - implementation of the CWin32Error class

#include "stdafx.h"
#include "Win32Error.h"

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
#endif	// __INTEL_COMPILER

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// object model
IMPLEMENT_DYNAMIC(CWin32Error, CException)

CWin32Error::CWin32Error(void):
m_dwErrCode(::GetLastError())
{
}

CWin32Error::CWin32Error(DWORD dwErrCode):
m_dwErrCode(dwErrCode)
{
}

CWin32Error::~CWin32Error(void)
{
}

void CWin32Error::Delete(void)
{
#if defined(_DEBUG)
	m_bReadyForDelete = TRUE;
#endif	// _DEBUG
	delete this;
}

BOOL CWin32Error::GetErrorMessage(LPTSTR pszDest, UINT uMaxLen, UINT* puHelpCtx)
{
	CString strTemp;

	ASSERT(pszDest != NULL);
	ASSERT(AfxIsValidString(pszDest, uMaxLen));

	if (puHelpCtx != NULL)
	{
		*puHelpCtx = 0;
	}
	FormatErrorMessage(strTemp);
	if (!strTemp.IsEmpty())
	{
		::lstrcpyn(pszDest, strTemp, uMaxLen);
		return (TRUE);
	}
	else {
		return (FALSE);
	}
}

int CWin32Error::ReportError(UINT fuType, UINT uHelpID)
{
	CString strError;

	FormatErrorMessage(strError);
	return (AfxMessageBox(strError, fuType, uHelpID));
}

BOOL CWin32Error::FormatErrorMessage(CString& strDest)
{
	LPTSTR pszBuffer;
	int cchMsg;

	strDest.Empty();
	cchMsg = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, m_dwErrCode, ::GetUserDefaultLangID(),
		reinterpret_cast<LPTSTR>(&pszBuffer), 1, NULL);
	if (cchMsg > 0)
	{
		strDest = pszBuffer;
	}
	else if (::GetLastError() == ERROR_RESOURCE_LANG_NOT_FOUND)
	{
		cchMsg = ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS, NULL, m_dwErrCode, 0, reinterpret_cast<LPTSTR>(&pszBuffer),
			1, NULL);
		ASSERT(cchMsg > 0);
		strDest = pszBuffer;
	}
	::LocalFree(pszBuffer);
	return (!strDest.IsEmpty());
}

#if defined(_DEBUG)

void CWin32Error::AssertValid(void) const
{
	// first perform inherited validity check...
	CException::AssertValid();

	// ...and then verify own state as well
}

void CWin32Error::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CException::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_dwErrCode = " << m_dwErrCode;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
