// AfxGadgets library.
// Copyright (c) 2004-2010 by Elijah Zarezky,
// All rights reserved.
// Portions copyright (c) 2000 by Martin Ziacek.

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

// SeException.cpp - implementation of the CSeException class

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "SeException.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// avoid unwanted ICL warnings

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
#endif	// __INTEL_COMPILER

//////////////////////////////////////////////////////////////////////////////////////////////
// debugging support

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////
// object model

IMPLEMENT_DYNAMIC(CSeException, CException)

CSeException::CSeException(UINT uCode, _EXCEPTION_POINTERS* pXcptPtrs):
m_uCode(uCode),
m_xcptPtrs(*pXcptPtrs)
{
}

CSeException::~CSeException(void)
{
}

CSeException::CSeException(const CSeException& src):
m_uCode(src.m_uCode),
m_xcptPtrs(src.m_xcptPtrs)
{
}

CSeException& CSeException::operator =(const CSeException& src)
{
	m_uCode = src.m_uCode;
	m_xcptPtrs = src.m_xcptPtrs;
	return (*this);
}

void CSeException::Delete(void)
{
#if defined(_DEBUG)
	m_bReadyForDelete = TRUE;
#endif	// _DEBUG
	delete this;
}

BOOL CSeException::GetErrorMessage(LPTSTR pszDest, UINT uMaxLen, UINT* puHelpCtx)
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

int CSeException::ReportError(UINT fuType, UINT uHelpID)
{
	CString strError;

	FormatErrorMessage(strError);
	return (AfxMessageBox(strError, fuType, uHelpID));
}

void CSeException::Translator(UINT uCode, _EXCEPTION_POINTERS* pXcptPtrs)
{
	CSeException* pXcpt = new CSeException(uCode, pXcptPtrs);
	throw pXcpt;
}

UINT CSeException::GetCode(void)
{
	return (m_uCode);
}

LPCTSTR CSeException::GetDescription(void)
{
	switch (m_uCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		return (_T("Access violation"));
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		return (_T("EXCEPTION_DATATYPE_MISALIGNMENT"));
	case EXCEPTION_BREAKPOINT:
		return (_T("EXCEPTION_BREAKPOINT"));
	case EXCEPTION_SINGLE_STEP:
		return (_T("EXCEPTION_SINGLE_STEP"));
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		return (_T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED"));
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		return (_T("EXCEPTION_FLT_DENORMAL_OPERAND"));
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		return (_T("Floating point divide by zero"));
	case EXCEPTION_FLT_INEXACT_RESULT:
		return (_T("Floating point inexact result"));
	case EXCEPTION_FLT_INVALID_OPERATION:
		return (_T("EXCEPTION_FLT_INVALID_OPERATION"));
	case EXCEPTION_FLT_OVERFLOW:
		return (_T("Floating point overflow"));
	case EXCEPTION_FLT_STACK_CHECK:
		return (_T("EXCEPTION_FLT_STACK_CHECK"));
	case EXCEPTION_FLT_UNDERFLOW:
		return (_T("Floating point underflow"));
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		return (_T("Integer divide by zero"));
	case EXCEPTION_INT_OVERFLOW:
		return (_T("Integer overflow"));
	case EXCEPTION_PRIV_INSTRUCTION:
		return (_T("EXCEPTION_PRIV_INSTRUCTION"));
	case EXCEPTION_IN_PAGE_ERROR:
		return (_T("EXCEPTION_IN_PAGE_ERROR"));
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		return (_T("EXCEPTION_ILLEGAL_INSTRUCTION"));
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		return (_T("EXCEPTION_NONCONTINUABLE_EXCEPTION"));
	case EXCEPTION_STACK_OVERFLOW:
		return (_T("Stack overflow"));
	case EXCEPTION_INVALID_DISPOSITION:
		return (_T("EXCEPTION_INVALID_DISPOSITION"));
	case EXCEPTION_GUARD_PAGE:
		return (_T("EXCEPTION_GUARD_PAGE"));
	case EXCEPTION_INVALID_HANDLE:
		return (_T("EXCEPTION_INVALID_HANDLE"));
	default:
		return (_T(">>> unrecognized <<<"));
	}
}

void* CSeException::GetAddress(void)
{
	return (m_xcptPtrs.ExceptionRecord->ExceptionAddress);
}

BOOL CSeException::FormatErrorMessage(CString& strDest)
{
	static TCHAR szFormat[] = _T("Exception 0x%08lX at address 0x%08lX:\n%s.");

	strDest.Empty();
	if (m_uCode == EXCEPTION_ACCESS_VIOLATION)
	{
		// special case
		CString strDescr(GetDescription());
		ULONG_PTR* xcptInfo = m_xcptPtrs.ExceptionRecord->ExceptionInformation;
		LPCTSTR pszMemOp = xcptInfo[0] != 0 ? _T("write") : _T("read");
		CString strDetails;
		strDetails.Format(_T("\x20(unable to %s at address 0x%08lX)"), pszMemOp, xcptInfo[1]);
		strDescr += strDetails;
		strDest.Format(szFormat, m_uCode, GetAddress(), static_cast<LPCTSTR>(strDescr));
	}
	else {
		// common case
		strDest.Format(szFormat, m_uCode, GetAddress(), GetDescription());
	}
	return (!strDest.IsEmpty());
}

#if defined(_DEBUG)

void CSeException::AssertValid(void) const
{
	// first perform inherited validity check...
	CException::AssertValid();

	// ...and then verify own state as well
}

void CSeException::Dump(CDumpContext& dumpCtx) const
{
	try
	{
		// first invoke inherited dumper...
		CException::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_uCode = " << m_uCode;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
