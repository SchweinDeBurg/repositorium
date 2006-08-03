// AfxGadgets library.
// Copyright (c) 2005-2006 by Elijah Zarezky,
// All rights reserved.

// RemoteProc.hxx - interface and implementation of the CRemoteProc<> template class

#if !defined(__RemoteProc_hxx)
#define __RemoteProc_hxx

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#include "RemoteParam.hxx"

// interface

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
class CRemoteProc
{
// construction/destruction
public:
	CRemoteProc(HANDLE hProcess, int cbMaxCode = 4096);
	~CRemoteProc(void);

// copying/assignment - disabled and thus not implemented
private:
	CRemoteProc(const CRemoteProc& rhs);
	CRemoteProc& operator=(const CRemoteProc& rhs);

// type conversion
public:
	operator LPTHREAD_START_ROUTINE(void) const;

// attributes
private:
	HANDLE m_hProcess;
	void* m_codePtr;
};

// implementation

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
inline CRemoteProc<_Data_t, _Proc>::CRemoteProc(HANDLE hProcess, int cbMaxCode):
m_hProcess(hProcess),
m_codePtr(NULL)
{
	m_codePtr = ::VirtualAllocEx(hProcess, NULL, cbMaxCode, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	::WriteProcessMemory(m_hProcess, m_codePtr, _Proc, cbMaxCode, NULL);
}

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
inline CRemoteProc<_Data_t, _Proc>::~CRemoteProc(void)
{
	::VirtualFreeEx(m_hProcess, m_codePtr, 0, MEM_RELEASE);
	m_codePtr = NULL;
}

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
inline CRemoteProc<_Data_t, _Proc>::operator LPTHREAD_START_ROUTINE(void) const
{
	return (reinterpret_cast<LPTHREAD_START_ROUTINE>(m_codePtr));
}

#endif	// __RemoteProc_hxx

// end of file
