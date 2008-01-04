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

// ProcessPrivileges.cpp - implementation of the CProcessPrivileges class

#include "stdafx.h"
#include "ProcessPrivileges.h"

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
#endif	// __INTEL_COMPILER

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

DWORD GetProcessPrivilege(LPCTSTR pszName)
{
	OSVERSIONINFO osVerInfo;
	HANDLE hToken;
	DWORD cbSize;
	TOKEN_PRIVILEGES* ptpState;
	TCHAR szTemp[64];

	DWORD fdwAttributes = 0;

	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	VERIFY(::GetVersionEx(&osVerInfo));
	if (osVerInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		fdwAttributes = SE_PRIVILEGE_ENABLED;
	}
	else if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &hToken))
	{
		::GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &cbSize);
		ptpState = reinterpret_cast<TOKEN_PRIVILEGES*>(malloc(cbSize));
		::GetTokenInformation(hToken, TokenPrivileges, ptpState, cbSize, &cbSize);
		for (DWORD i = 0; i < ptpState->PrivilegeCount; ++i)
		{
			DWORD cchLength = 64;
			::LookupPrivilegeName(NULL, &ptpState->Privileges[i].Luid, szTemp, &cchLength);
			if (::lstrcmpi(pszName, szTemp) == 0)
			{
				fdwAttributes = ptpState->Privileges[i].Attributes;
				break;
			}
		}
		free(ptpState);
		::CloseHandle(hToken);
	}

	return (fdwAttributes);
}

BOOL SetProcessPrivilege(LPCTSTR pszName, DWORD fdwAttributes)
{
	OSVERSIONINFO osVerInfo;
	HANDLE hToken;
	TOKEN_PRIVILEGES* ptpNewState;

	BOOL fSuccess = FALSE;

	osVerInfo.dwOSVersionInfoSize = sizeof(osVerInfo);
	VERIFY(::GetVersionEx(&osVerInfo));
	if (osVerInfo.dwPlatformId != VER_PLATFORM_WIN32_NT)
	{
		fSuccess = TRUE;
	}
	else if (::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
	{
		enum { cbSize = sizeof(TOKEN_PRIVILEGES) + sizeof(LUID_AND_ATTRIBUTES) };
		ptpNewState = reinterpret_cast<TOKEN_PRIVILEGES*>(malloc(cbSize));
		ptpNewState->PrivilegeCount = 1;
		if (::LookupPrivilegeValue(NULL, pszName, &ptpNewState->Privileges[0].Luid))
		{
			ptpNewState->Privileges[0].Attributes = fdwAttributes;
			fSuccess = ::AdjustTokenPrivileges(hToken, FALSE, ptpNewState, 0, NULL, NULL);
		}
		free(ptpNewState);
		::CloseHandle(hToken);
	}

	return (fSuccess);
}


#if !defined(_AFX_ENABLE_INLINES)
// inline functions expanded out-of-line

static char _szProcessPrivileges_inl[] = "ProcessPrivileges.inl";
#undef THIS_FILE
#define THIS_FILE _szProcessPrivileges_inl
#define _ProcessPrivileges_INLINE

#include "ProcessPrivileges.inl"

#endif	// _AFX_ENABLE_INLINES

// import library
#pragma comment(lib, "advapi32.lib")

// end of file
