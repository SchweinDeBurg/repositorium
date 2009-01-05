// AfxGadgets library.
// Copyright (c) 2004-2009 by Elijah Zarezky,
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

// XML_NodeList.cpp - implementation of the XML::CNodeList class

// Based on the code by Jerry.Wang.
// Visit http://www.codeproject.com/useritems/CXml.asp for more info.

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "XML_NodeList.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// avoid unwanted warnings

#if !defined(_DEBUG)
#pragma warning(disable: 4101)
#endif	// _DEBUG

namespace XML
{

//////////////////////////////////////////////////////////////////////////////////////////////
// construction/destruction

CNodeList::CNodeList(void):
m_ipNodeList(NULL)
{
}

CNodeList::CNodeList(const CNodeList& nodeListSrc):
m_ipNodeList(nodeListSrc.m_ipNodeList)
{
}

CNodeList::~CNodeList(void)
{
	Release();
}

CNodeList::CNodeList(IDomNodeListPtr ipNodeList):
m_ipNodeList(ipNodeList)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////
// assignment

CNodeListPtr CNodeList::operator =(CNodeListPtr nodeListPtrRhs)
{
	ReleaseInterface(m_ipNodeList);

	m_ipNodeList = nodeListPtrRhs->m_ipNodeList;
	return (nodeListPtrRhs);
}

CNodeList& CNodeList::operator =(CNodeList& nodeListRhs)
{
	ReleaseInterface(m_ipNodeList);

	m_ipNodeList = nodeListRhs.m_ipNodeList;
	return (*this);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// indexing/items access

CNodePtr CNodeList::operator[](LONG nIndex)
{
	return (GetItem(nIndex));
}

CNodePtr CNodeList::operator[](LPCTSTR lpszName)
{
	return (GetItem(lpszName));
}

CNodePtr CNodeList::GetItem(LONG nIndex)
{
	XML_ASSERT(m_ipNodeList != NULL);
	XML_ASSERT(nIndex >= 0 && nIndex < GetCount());

	CNodePtr nodePtrItem;

	try
	{
		nodePtrItem.reset(new CNode(m_ipNodeList->item[nIndex]));
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNodeList::GetItem() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtrItem);
}

CNodePtr CNodeList::GetItem(LPCTSTR lpszName)
{
	XML_ASSERT(m_ipNodeList != NULL);
	XML_ASSERT(lpszName != NULL);

	CNodePtr nodePtrItem(new CNode());

	try
	{
		HRESULT hr = S_OK;
		CString strName;
		BSTR bstrTemp = NULL;
		IDomNodePtr ipDomNode = NULL;

		for (int i = 0; i < m_ipNodeList->length; i++)
		{
			ipDomNode = m_ipNodeList->item[i];

			hr = ipDomNode->get_nodeName(&bstrTemp);

			strName = static_cast<LPCTSTR>(_bstr_t(bstrTemp, true));
			if (bstrTemp != NULL)
			{
				::SysFreeString(bstrTemp);
				bstrTemp = NULL;
			}

			// get the first element
			if (strName.Compare(lpszName) == 0)
			{
				nodePtrItem->m_ipDomNode = ipDomNode;
				ReleaseInterface(ipDomNode);

				return (nodePtrItem);
			}

			ReleaseInterface(ipDomNode);
		}
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNodeList::GetItem() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtrItem);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// misc operations

LONG CNodeList::GetCount(void)
{
	XML_ASSERT(m_ipNodeList != NULL);

	try
	{
		return (m_ipNodeList->length);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNodeList::GetCount() failed: %s\n"), err.ErrorMessage());
		throw;
	}
}

void CNodeList::Release(void)
{
	ReleaseInterface(m_ipNodeList);
}

}	// namespace XML

// end of file
