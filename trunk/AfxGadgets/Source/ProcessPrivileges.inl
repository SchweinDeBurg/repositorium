// AfxGadgets library.
// Copyright (c) 2004-2005 by Elijah Zarezky,
// All rights reserved.

// ProcessPrivileges.inl - inline members of the CProcessPrivileges class

#if defined(_ProcessPrivileges_INLINE)

_ProcessPrivileges_INLINE
CProcessPrivileges::CProcessPrivileges(void)
{
}

_ProcessPrivileges_INLINE
CProcessPrivileges::~CProcessPrivileges(void)
{
}

_ProcessPrivileges_INLINE
CProcessPrivileges::ITEM::ITEM(LPCTSTR pszName):
m_strName(pszName)
{
	ASSERT(AfxIsValidString(m_strName));
	ASSERT(!m_strName.IsEmpty());
}

_ProcessPrivileges_INLINE
CProcessPrivileges::ITEM::operator DWORD(void)
{
	return (GetProcessPrivilege(m_strName));
}

_ProcessPrivileges_INLINE
CProcessPrivileges::ITEM& CProcessPrivileges::ITEM::operator =(DWORD fdwAttributes)
{
	SetProcessPrivilege(m_strName, fdwAttributes);
	return (*this);
}

_ProcessPrivileges_INLINE
CProcessPrivileges::ITEM CProcessPrivileges::operator [](LPCTSTR pszName) const
{
	ASSERT(AfxIsValidString(pszName));
	ASSERT(*pszName != 0);
	return (ITEM(pszName));
}

#endif	// _ProcessPrivileges_INLINE

// end of file
