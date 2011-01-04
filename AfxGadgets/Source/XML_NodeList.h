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

// XML_NodeList.h - interface of the XML::CNodeList class

// Based on the code by Jerry.Wang.
// Visit http://www.codeproject.com/useritems/CXml.asp for more info.

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__XML_NodeList_h_)
#define __XML_NodeList_h_

#include "XML_Defs.h"
#include "XML_Node.h"

namespace XML
{

class CNodeList
{
// our friends
	friend class CDocument;
	friend class CNode;

// construction/destruction
public:
	CNodeList(void);
	CNodeList(const CNodeList& nodeListSrc);
	~CNodeList(void);
protected:
	CNodeList(IDomNodeListPtr ipNodeList);

// assignment
public:
	CNodeListPtr operator =(CNodeListPtr nodeListPtrRhs);
	CNodeList& operator =(CNodeList& nodeListRhs);

// indexing/items access
public:
	CNodePtr operator[](LONG nIndex);
	CNodePtr operator[](LPCTSTR lpszName);
	CNodePtr GetItem(LONG nIndex);
	CNodePtr GetItem(LPCTSTR lpszName);

// misc operations
public:
	LONG GetCount(void);
	void Release(void);

// data members
protected:
	IDomNodeListPtr m_ipNodeList;
};

}	// namespace XML

#endif	// __XML_NodeList_h_

// end of file
