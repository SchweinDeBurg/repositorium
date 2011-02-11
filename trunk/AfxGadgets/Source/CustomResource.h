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

// CustomResource.h - interface of the CCustomResource class

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif   // _MSC_VER

#if !defined(__CustomResource_h)
#define __CustomResource_h

//////////////////////////////////////////////////////////////////////////////////////////////
// CCustomResourceBase

class CCustomResourceBase: public CObject
{
	DECLARE_DYNAMIC(CCustomResourceBase)

// construction/destruction
public:
	CCustomResourceBase(HINSTANCE hInstRes, LPCTSTR pszType, LPCTSTR pszName);
	virtual ~CCustomResourceBase(void);

// operations
public:
	void* GetData(void) const;
	DWORD GetSize(void) const;

// attributes
protected:
	HINSTANCE m_hInstRes;
	CString m_strType;
	LPCTSTR m_pszName;
	HRSRC m_hrsrcInfo;
	HGLOBAL m_hMemBlock;
	void* m_pvDataPtr;

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////
// inlines

inline void* CCustomResourceBase::GetData(void) const
{
	return (m_pvDataPtr);
}

inline DWORD CCustomResourceBase::GetSize(void) const
{
	return (::SizeofResource(m_hInstRes, m_hrsrcInfo));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CCustomResource

class CCustomResource: public CCustomResourceBase
{
// construction
public:
	CCustomResource(HINSTANCE hInstRes, LPCTSTR pszType, UINT uID);
	CCustomResource(HINSTANCE hInstRes, LPCTSTR pszType, LPCTSTR pszName);
};

//////////////////////////////////////////////////////////////////////////////////////////////
// inlines

inline CCustomResource::CCustomResource(HINSTANCE hInstRes, LPCTSTR pszType, UINT uID):
CCustomResourceBase(hInstRes, pszType, MAKEINTRESOURCE(uID))
{
}

inline CCustomResource::CCustomResource(HINSTANCE hInstRes, LPCTSTR pszType, LPCTSTR pszName):
CCustomResourceBase(hInstRes, pszType, pszName)
{
}

#endif   // __CustomResource_h

// end of file
