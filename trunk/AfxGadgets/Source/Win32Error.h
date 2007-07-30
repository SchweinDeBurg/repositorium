// AfxGadgets library.
// Copyright (c) 2004-2006 by Elijah Zarezky,
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

// Win32Error.h - interface of the CWin32Error class

#if !defined(__Win32Error_h)
#define __Win32Error_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

class CWin32Error: public CException
{
	DECLARE_DYNAMIC(CWin32Error)

// construction/destruction
public:
	CWin32Error(void);
	CWin32Error(DWORD dwErrCode);
	virtual ~CWin32Error(void);

// operations
public:
	void Delete(void);
	virtual BOOL GetErrorMessage(LPTSTR pszDest, UINT uMaxLen, UINT* puHelpCtx = NULL);
	virtual int ReportError(UINT fuType = MB_OK, UINT uHelpID = 0);

// attributes
protected:
	DWORD m_dwErrCode;

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

#endif	// __Win32Error_h

// end of file
