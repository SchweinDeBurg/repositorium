// AfxGadgets library.
// Copyright (c) 2004-2008 by Elijah Zarezky,
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

// SeException.h - interface of the CSeException class

#if !defined(__SeException_h)
#define __SeException_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

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
