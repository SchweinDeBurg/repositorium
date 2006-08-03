// AfxGadgets library.
// Copyright (c) 2005-2006 by Elijah Zarezky,
// All rights reserved.

// RemoteInvoke.hxx - interface and implementation of the CRemoteInvoke<> template class

#if !defined(__RemoteInvoke_hxx)
#define __RemoteInvoke_hxx

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#include "RemoteProc.hxx"

// interface

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
class CRemoteInvoke
{
// construction/destruction
public:
	CRemoteInvoke(_Data_t& dataRef);

// copying/assignment - disabled and thus not implemented
private:
	CRemoteInvoke(const CRemoteInvoke& rhs);
	CRemoteInvoke& operator=(const CRemoteInvoke& rhs);

// operations
public:
	BOOL operator()(HANDLE hProcess, DWORD dwTimeOut = INFINITE);
	BOOL operator()(DWORD dwProcessID, DWORD dwTimeOut = INFINITE);

// attributes
private:
	_Data_t& m_dataRef;
};

// implementation

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
inline CRemoteInvoke<_Data_t, _Proc>::CRemoteInvoke(_Data_t& dataRef):
m_dataRef(dataRef)
{
}

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
BOOL CRemoteInvoke<_Data_t, _Proc>::operator()(HANDLE hProcess, DWORD dwTimeOut)
{
	DWORD dwThreadID;
	BOOL fSuccess;

	CRemoteParam<_Data_t> param(hProcess, m_dataRef);
	CRemoteProc<_Data_t, _Proc> proc(hProcess);
	HANDLE hThread = ::CreateRemoteThread(hProcess, NULL, 0, proc, param, 0, &dwThreadID);
	switch (::WaitForSingleObject(hThread, dwTimeOut))
	{
	case WAIT_OBJECT_0:
		fSuccess = param.GetRemoteData(m_dataRef);
		break;
	case WAIT_TIMEOUT:
		// time-out interval expired
		fSuccess = FALSE;
		break;
	case WAIT_ABANDONED:
		// shouldn't be reached
		fSuccess = FALSE;
		break;
	default:
		fSuccess = FALSE;
	}
	return (fSuccess);
}

template <typename _Data_t, DWORD(WINAPI* _Proc)(CRemoteParam<_Data_t>*)>
inline BOOL CRemoteInvoke<_Data_t, _Proc>::operator()(DWORD dwProcessID, DWORD dwTimeOut)
{
	enum { fdwAccess = PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ };
	HANDLE hProcess = ::OpenProcess(fdwAccess, FALSE, dwProcessID);
	BOOL fSuccess = (*this)(hProcess, dwTimeOut);
	::CloseHandle(hProcess);
	return (fSuccess);
}

#endif	// __RemoteInvoke_hxx

// end of file
