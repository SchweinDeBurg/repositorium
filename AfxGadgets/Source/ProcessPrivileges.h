// AfxGadgets library.
// Copyright (c) 2004-2005 by Elijah Zarezky,
// All rights reserved.

// ProcessPrivileges.h - interface of the CProcessPrivileges class

#if !defined(__ProcessPrivileges_h)
#define __ProcessPrivileges_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

class CProcessPrivileges
{
// contruction/destruction
public:
	CProcessPrivileges(void);
	~CProcessPrivileges(void);

// attributes
private:
	struct ITEM {
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
