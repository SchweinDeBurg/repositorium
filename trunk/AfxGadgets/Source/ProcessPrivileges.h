// AfxGadgets library.
// Copyright (c) 2004-2011 by Elijah Zarezky,
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

// ProcessPrivileges.h - interface of the CProcessPrivileges class

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__ProcessPrivileges_h)
#define __ProcessPrivileges_h

class CProcessPrivileges
{
// contruction/destruction
public:
	CProcessPrivileges(void);
	~CProcessPrivileges(void);

// attributes
private:
	struct ITEM
	{
		friend class CProcessPrivileges;
	private:
		explicit ITEM(LPCTSTR pszName);
		// copying/assignment - disabled and thus not implemented
		ITEM(const ITEM& src);
		ITEM& operator =(const ITEM& src);
	public:
		operator DWORD(void);
		ITEM& operator =(DWORD fdwAttributes);
	private:
		CString m_strName;
	};

// operations
public:
	ITEM operator [](LPCTSTR pszName) const;
};

// global helpers - can be used separately
DWORD GetProcessPrivilege(LPCTSTR pszName);
BOOL SetProcessPrivilege(LPCTSTR pszName, DWORD fdwAttributes);

// inlines
#if defined(_AFX_ENABLE_INLINES)
#define _ProcessPrivileges_INLINE AFX_INLINE
#include "ProcessPrivileges.inl"
#endif	// _AFX_ENABLE_INLINES

#endif	// __ProcessPrivileges_h

// end of file
