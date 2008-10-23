// AfxGadgets library.
// Copyright (c) 2004-2008 by Elijah Zarezky,
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

// CustomResource.cpp - implementation of the CCustomResource class

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "CustomResource.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// debugging support

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////
// object model

IMPLEMENT_DYNAMIC(CCustomResourceBase, CObject)

//////////////////////////////////////////////////////////////////////////////////////////////
// construction/destruction

CCustomResourceBase::CCustomResourceBase(HINSTANCE hInstRes, LPCTSTR pszType, LPCTSTR pszName):
m_hInstRes(hInstRes),
m_strType(pszType),
m_pszName(IS_INTRESOURCE(pszName) ? pszName : _tcsdup(pszName)),
m_hrsrcInfo(NULL),
m_hMemBlock(NULL),
m_pvDataPtr(NULL)
{
	ASSERT(!m_strType.IsEmpty());
	ASSERT(m_pszName != NULL);

	if ((m_hrsrcInfo = ::FindResource(m_hInstRes, m_pszName, m_strType)) != NULL)
	{
		if ((m_hMemBlock = ::LoadResource(m_hInstRes, m_hrsrcInfo)) != NULL)
		{
			m_pvDataPtr = ::LockResource(m_hMemBlock);
		}
	}
}

CCustomResourceBase::~CCustomResourceBase(void)
{
	if (m_hMemBlock != NULL)
	{
		UnlockResource(m_hMemBlock);
		::FreeResource(m_hMemBlock);
	}

	if (!IS_INTRESOURCE(m_pszName))
	{
		free(const_cast<LPTSTR>(m_pszName));
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
// diagnostic services

#if defined(_DEBUG)

void CCustomResourceBase::AssertValid(void) const
{
	// first perform inherited validity check...
	CObject::AssertValid();

	// ...and then verify own state as well
	ASSERT(!m_strType.IsEmpty());
	ASSERT(m_pszName != NULL);
	ASSERT(m_hrsrcInfo != NULL);
	ASSERT(m_hMemBlock != NULL);
	ASSERT(m_pvDataPtr != NULL);
}

void CCustomResourceBase::Dump(CDumpContext& dumpCtx) const
{
	try
	{
		// first invoke inherited dumper...
		CObject::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_hInstRes = " << m_hInstRes << "\n";
		dumpCtx << "m_strType = " << m_strType << "\n";
		if (IS_INTRESOURCE(m_pszName))
		{
			dumpCtx << "m_pszName = " << static_cast<const void*>(m_pszName) << "\n";
		}
		else {
			dumpCtx << "m_pszName = " << m_pszName << "\n";
		}
		dumpCtx << "m_hrsrcInfo = " << m_hrsrcInfo << "\n";
		dumpCtx << "m_hMemBlock = " << m_hMemBlock << "\n";
		dumpCtx << "m_pvDataPtr = " << m_pvDataPtr;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
