// AfxGadgets library.
// Copyright (c) 2004-2010 by Elijah Zarezky,
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

// RemoteParam.hxx - interface and implementation of the CRemoteParam<> template class

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__RemoteParam_hxx)
#define __RemoteParam_hxx

// interface

template <typename _Data_t>
class CRemoteParam
{
// construction/destruction
public:
	CRemoteParam(HANDLE hProcess, const _Data_t& dataSrc);
	~CRemoteParam(void);

// copying/assignment - disabled and thus not implemented
private:
	CRemoteParam(const CRemoteParam& rhs);
	CRemoteParam& operator=(const CRemoteParam& rhs);

// type conversion
public:
	operator void*(void) const;

// accessors
public:
	BOOL GetRemoteData(_Data_t& dataDest);

// attributes
public:
	HMODULE (WINAPI* m_pfnGetModuleHandle)(LPCTSTR);
	HMODULE (WINAPI* m_pfnLoadLibrary)(LPCTSTR);
	FARPROC (WINAPI* m_pfnGetProcAddress)(HMODULE, LPCSTR);
	BOOL (WINAPI* m_pfnFreeLibrary)(HMODULE);
	// user-defined data
	_Data_t m_data;
private:
	HANDLE m_hProcess;
	void* m_thisPtr;
};

// global helpers

template <class _Proc_t>
inline void GetProcAddress_t(HMODULE hModule, LPCSTR pszProcName, _Proc_t& procAddress)
{
	procAddress = reinterpret_cast<_Proc_t>(::GetProcAddress(hModule, pszProcName));
}

// implementation

template <typename _Data_t>
CRemoteParam<_Data_t>::CRemoteParam(HANDLE hProcess, const _Data_t& dataSrc):
m_hProcess(hProcess),
m_thisPtr(NULL)
{
	HMODULE hKernel32 = ::GetModuleHandle(_T("kernel32"));
#if defined(_UNICODE) || defined(UNICODE)
// Unicode build
	GetProcAddress_t(hKernel32, "GetModuleHandleW", m_pfnGetModuleHandle);
	GetProcAddress_t(hKernel32, "LoadLibraryW", m_pfnLoadLibrary);
#else
// MBCS build
	GetProcAddress_t(hKernel32, "GetModuleHandleA", m_pfnGetModuleHandle);
	GetProcAddress_t(hKernel32, "LoadLibraryA", m_pfnLoadLibrary);
#endif	// _UNICODE
	GetProcAddress_t(hKernel32, "GetProcAddress", m_pfnGetProcAddress);
	GetProcAddress_t(hKernel32, "FreeLibrary", m_pfnFreeLibrary);

	m_data = dataSrc;

	m_thisPtr = ::VirtualAllocEx(m_hProcess, NULL, sizeof(*this), MEM_COMMIT, PAGE_READWRITE);
	::WriteProcessMemory(m_hProcess, m_thisPtr, this, sizeof(*this), NULL);
}

template <typename _Data_t>
inline CRemoteParam<_Data_t>::~CRemoteParam(void)
{
	::VirtualFreeEx(m_hProcess, m_thisPtr, 0, MEM_RELEASE);
	m_thisPtr = NULL;
}

template <typename _Data_t>
inline CRemoteParam<_Data_t>::operator void*(void) const
{
	return (m_thisPtr);
}

template <typename _Data_t>
inline BOOL CRemoteParam<_Data_t>::GetRemoteData(_Data_t& dataDest)
{
	typedef CRemoteParam<_Data_t> _This_t;

	_Data_t* pRemoteData = &reinterpret_cast<_This_t*>(m_thisPtr)->m_data;
	return (::ReadProcessMemory(m_hProcess, pRemoteData, &dataDest, sizeof(_Data_t), NULL));
}

#endif	// __RemoteParam_hxx

// end of file
