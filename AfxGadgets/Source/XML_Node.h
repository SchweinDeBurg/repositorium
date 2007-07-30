// AfxGadgets library.
// Copyright (c) 2004-2007 by Elijah Zarezky,
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

// XML_Node.h - interface of the XML::CNode class

// Based on the code by Jerry.Wang.
// Visit http://www.codeproject.com/useritems/CXml.asp for more info.

#if !defined(__XML_Node_h_)
#define __XML_Node_h_

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#include "XML_Defs.h"
#include "XML_NodeList.h"

namespace XML
{

class CNode
{
// our friends
	friend class CDocument;
	friend class CNodeList;

// construction/destruction
public:
	CNode(void);
	CNode(const CNode& nodeSrc);
	~CNode(void);
protected:
	CNode(IDomNodePtr ipDomNode);

// assignment
public:
	CNodePtr operator =(CNodePtr nodePtr);
	CNode& operator =(CNode& nodeRhs);

// operations
public:

	// parent <-> child
	CNodePtr GetChild(CStringIn strName, BOOL fCreate = TRUE);
	CNodePtr NewChild(CStringIn strName);
	CNodePtr GetParent(void);
	CNodeListPtr GetChildren(void);
	void AttachChild(CNodePtr& nodePtrChild);
	void AttachChild(CNode& nodeChild);
	BOOL HasChildren(void);
	BOOL RemoveChildren(void);

	// get value
	CStringRet GetValue(LPCTSTR lpszDefault = NULL) const;
	bool GetValue(bool fDefault) const;
	int GetValue(int nDefault) const;
	long GetValue(long nDefault) const;
	__int64 GetValue(__int64 qnDefault) const;
	float GetValue(float fDefault) const;
	double GetValue(double dDefault) const;
	DWORD GetValue(DWORD dwDefault) const;

	// set value
	BOOL SetValue(LPCTSTR lpszValue);
	BOOL SetValue(bool fValue);
	BOOL SetValue(int nValue);
	BOOL SetValue(long nValue);
	BOOL SetValue(__int64 qnValue);
	BOOL SetValue(float fValue);
	BOOL SetValue(double dValue);
	BOOL SetValue(DWORD dwValue);

	// get attribute
	CStringRet GetAttribute(CStringIn strName, LPCTSTR lpszDefault = NULL) const;
	bool GetAttribute(CStringIn strName, bool fDefault) const;
	int GetAttribute(CStringIn strName, int nDefault) const;
	long GetAttribute(CStringIn strName, long nDefault) const;
	__int64 GetAttribute(CStringIn strName, __int64 qnDefault) const;
	float GetAttribute(CStringIn strName, float fDefault) const;
	double GetAttribute(CStringIn strName, double dDefault) const;
	DWORD GetAttribute(CStringIn strName, DWORD dwDefault) const;

	// set attribute
	BOOL SetAttribute(CStringIn strName, LPCTSTR lpszValue);
	BOOL SetAttribute(CStringIn strName, bool bValue);
	BOOL SetAttribute(CStringIn strName, int nValue);
	BOOL SetAttribute(CStringIn strName, long nValue);
	BOOL SetAttribute(CStringIn strName, __int64 qnValue);
	BOOL SetAttribute(CStringIn strName, float fValue);
	BOOL SetAttribute(CStringIn strName, double dValue);
	BOOL SetAttribute(CStringIn strName, DWORD dwValue);

	// remove attribute
	BOOL RemoveAttribute(CStringIn strName);

	// misc
	BOOL IsNull(void);
	CStringRet GetName(void) const;
	CNode& Detach(void);
	void Release(void);

	// XPath
	CNodePtr SelectSingleNode(LPCTSTR lpszPath);
	CNodeListPtr SelectNodes(LPCTSTR lpszPath);

// data members
protected:
	IDomNodePtr m_ipDomNode;

// implementation helpers
protected:
	BOOL _GetValue(CStringOut strValue) const;
	BOOL _SetValue(CStringIn strValue);
	BOOL _GetAttribute(CStringIn strName, CStringOut strValue) const;
	BOOL _SetAttribute(CStringIn strName, CStringInOut strValue);
};

}	// namespace XML

#endif	// __XML_Node_h_

// end of file
