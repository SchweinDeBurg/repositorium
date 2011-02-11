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

#endif   // _ProcessPrivileges_INLINE

// end of file
