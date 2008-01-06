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

// XML_Node.cpp - implementation of the XML::CNode class

// Based on the code by Jerry.Wang.
// Visit http://www.codeproject.com/useritems/CXml.asp for more info.

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "XML_Node.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// avoid unwanted warnings

#if !defined(_DEBUG)
#pragma warning(disable: 4101)
#endif	// _DEBUG

namespace XML
{

//////////////////////////////////////////////////////////////////////////////////////////////
// construction/destruction

CNode::CNode(void):
m_ipDomNode(NULL)
{
}

CNode::CNode(const CNode& nodeSrc):
m_ipDomNode(nodeSrc.m_ipDomNode)
{
}

CNode::~CNode(void)
{
	ReleaseInterface(m_ipDomNode);
}

CNode::CNode(IDomNodePtr ipDomNode)
{
	m_ipDomNode = ipDomNode;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// assignment

CNodePtr CNode::operator =(CNodePtr nodePtr)
{
	ReleaseInterface(m_ipDomNode);

	m_ipDomNode = nodePtr->m_ipDomNode;
	return (nodePtr);
}

CNode& CNode::operator =(CNode& nodeRhs)
{
	ReleaseInterface(m_ipDomNode);

	m_ipDomNode = nodeRhs.m_ipDomNode;
	return (*this);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations: parent <-> child

CNodePtr CNode::GetChild(CStringIn strName, BOOL fCreate)
{
	XML_ASSERT(m_ipDomNode != NULL);

	CNodePtr nodePtrChild(new CNode());

	try
	{
		IDomNodePtr ipChildNode = NULL;
		ipChildNode = m_ipDomNode->selectSingleNode(_bstr_t(strName));

		// create if not exist
		if (ipChildNode == NULL && fCreate)
		{
			MSXML::IXMLDOMDocumentPtr ipDomDoc = NULL;
			HRESULT hr = m_ipDomNode->get_ownerDocument(&ipDomDoc);
			XML_ASSERT(SUCCEEDED(hr));

			ipChildNode = ipDomDoc->createElement(_bstr_t(strName));
			XML_ASSERT(ipChildNode != NULL);

			m_ipDomNode->appendChild(ipChildNode);

			ReleaseInterface(ipDomDoc);
		}

		nodePtrChild->m_ipDomNode = ipChildNode;
		ReleaseInterface(ipChildNode);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::GetChild() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtrChild);
}

CNodePtr CNode::NewChild(CStringIn strName)
{
	XML_ASSERT(m_ipDomNode != NULL);

	CNodePtr nodePtrChild(new CNode());

	try
	{
		MSXML::IXMLDOMDocumentPtr ipDomDoc = NULL;
		HRESULT hr = m_ipDomNode->get_ownerDocument(&ipDomDoc);
		XML_ASSERT(SUCCEEDED(hr));

		IDomNodePtr ipChildNode = NULL;
		ipChildNode = ipDomDoc->createElement(_bstr_t(strName));
		XML_ASSERT(ipChildNode != NULL);
		ReleaseInterface(ipDomDoc);

		m_ipDomNode->appendChild(ipChildNode);
		nodePtrChild->m_ipDomNode = ipChildNode;
		ReleaseInterface(ipChildNode);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::NewChild() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtrChild);
}

CNodePtr CNode::GetParent(void)
{
	XML_ASSERT(m_ipDomNode != NULL);

	CNodePtr nodePtrParent(new CNode());

	try
	{
		nodePtrParent->m_ipDomNode = m_ipDomNode->GetparentNode();
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::GetParent() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtrParent);
}

CNodeListPtr CNode::GetChildren(void)
{
	XML_ASSERT(m_ipDomNode != NULL);

	CNodeListPtr nodeListPtr(new CNodeList());

	try
	{
		IDomNodeListPtr ipNodeList = NULL;
		ipNodeList = m_ipDomNode->selectNodes(_bstr_t(_T("child::*")));

		nodeListPtr->m_ipNodeList = ipNodeList;
		ReleaseInterface(ipNodeList);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::GetChildren() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodeListPtr);
}

void CNode::AttachChild(CNodePtr& nodePtrChild)
{
	XML_ASSERT(m_ipDomNode != NULL);

	try
	{
		m_ipDomNode->appendChild(nodePtrChild->m_ipDomNode);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::AttachChild() failed: %s\n"), err.ErrorMessage());
		throw;
	}
}

void CNode::AttachChild(CNode& nodeChild)
{
	try
	{
		m_ipDomNode->appendChild(nodeChild.m_ipDomNode);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::AttachChild() failed: %s\n"), err.ErrorMessage());
		throw;
	}
}

BOOL CNode::HasChildren(void)
{
	XML_ASSERT(m_ipDomNode != NULL);

	BOOL fHasChildren = FALSE;

	try
	{
		IDomNodeListPtr ipNodeList = NULL;
		ipNodeList = m_ipDomNode->selectNodes(_bstr_t(_T("child::*")));
		XML_ASSERT(ipNodeList != NULL);

		fHasChildren = ipNodeList->length > 0;
		ReleaseInterface(ipNodeList);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::HasChildren() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (fHasChildren);
}

BOOL CNode::RemoveChildren(void)
{
	XML_ASSERT(m_ipDomNode != NULL);

	try
	{
		IDomNodeListPtr ipNodeList = m_ipDomNode->childNodes;

		for (int i = ipNodeList->length - 1; i >= 0; i--)
		{
			m_ipDomNode->removeChild(ipNodeList->item[i]);
		}

		ReleaseInterface(ipNodeList);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::RemoveChildren() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (TRUE);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations: get value

// get CString value
CStringRet CNode::GetValue(LPCTSTR lpszDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.IsEmpty() && lpszDefault != NULL)
	{
		strValue = lpszDefault;
	}

	return (strValue);
}

// get bool value
bool CNode::GetValue(bool fDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.CompareNoCase(_T("1")) == 0)
	{
		return (true);
	}
	else if (strValue.CompareNoCase(_T("0")) == 0)
	{
		return (false);
	}
	else {
		strValue = fDefault ? "1" : "0";
		return (fDefault);
	}
}

// get int value
int CNode::GetValue(int nDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%d"), nDefault);
	}

	return (_ttoi(strValue));
}

// get long value
long CNode::GetValue(long nDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%ld"), nDefault);
	}

	return (_ttol(strValue));
}

// get __int64 value
__int64 CNode::GetValue(__int64 qnDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%I64d"), qnDefault);
	}

	return (_ttoi64(strValue));
}

// get float value
float CNode::GetValue(float fDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%f"), fDefault);
	}

	return (static_cast<float>(_tstof(strValue)));
}

// get double value
double CNode::GetValue(double dDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%.12f"), dDefault);
	}

	return (_tstof(strValue));
}

// get DWORD value
DWORD CNode::GetValue(DWORD dwDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetValue(strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%lu"), dwDefault);
	}

	return (_tcstoul(strValue, NULL, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations: set value

// set LPCTSTR value
BOOL CNode::SetValue(LPCTSTR lpszValue)
{
	_CSTRING_NS::CString strValue(lpszValue);
	return (_SetValue(strValue));
}

// set bool value
BOOL CNode::SetValue(bool fValue)
{
	_CSTRING_NS::CString strValue;
	strValue = fValue ? _T("1") : _T("0");

	return (_SetValue(strValue));
}

// set int value
BOOL CNode::SetValue(int nValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%d"), nValue);

	return (_SetValue(strValue));
}

// set long value
BOOL CNode::SetValue(long nValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%ld"), nValue);

	return (_SetValue(strValue));
}

// set __int64 value
BOOL CNode::SetValue(__int64 qnValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%I64d"), qnValue);

	return (_SetValue(strValue));
}

// set float value
BOOL CNode::SetValue(float fValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%f"), fValue);

	return (_SetValue(strValue));
}

// set double value
BOOL CNode::SetValue(double dValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%.12f"), dValue);

	return (_SetValue(strValue));
}

// set DWORD value
BOOL CNode::SetValue(DWORD dwValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%lu"), dwValue);

	return (_SetValue(strValue));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations: get attribute

// get LPCTSTR attribute
CStringRet CNode::GetAttribute(CStringIn strName, LPCTSTR lpszDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.IsEmpty() && lpszDefault != NULL)
	{
		strValue = lpszDefault;
	}
	return (strValue);
}

// get bool attribute
bool CNode::GetAttribute(CStringIn strName, bool fDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.CompareNoCase(_T("1")) == 0)
	{
		return (true);
	}
	else if (strValue.CompareNoCase(_T("0")) == 0)
	{
		return (false);
	}
	else {
		strValue = fDefault ? _T("1") : _T("0");
		return (fDefault);
	}
}

// get int attribute
int CNode::GetAttribute(CStringIn strName, int nDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%d"), nDefault);
	}

	return (_ttoi(strValue));
}

// get long attribute
long CNode::GetAttribute(CStringIn strName, long nDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%ld"), nDefault);
	}

	return (_ttol(strValue));
}

// get __int64 attribute
__int64 CNode::GetAttribute(CStringIn strName, __int64 qnDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%I64d"), qnDefault);
	}

	return (_ttoi64(strValue));
}

// get float attribute
float CNode::GetAttribute(CStringIn strName, float fDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%f"), fDefault);
	}

	return (static_cast<float>(_tstof(strValue)));
}

// get double attribute
double CNode::GetAttribute(CStringIn strName, double dDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%.12f"), dDefault);
	}

	return (_tstof(strValue));
}

// get DWORD attribute
DWORD CNode::GetAttribute(CStringIn strName, DWORD dwDefault) const
{
	_CSTRING_NS::CString strValue;
	_GetAttribute(strName, strValue);

	if (strValue.IsEmpty())
	{
		strValue.Format(_T("%lu"), dwDefault);
	}

	return (_tcstoul(strValue, NULL, 10));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations: set attribute

// set LPCTSTR attribute
BOOL CNode::SetAttribute(CStringIn strName, LPCTSTR lpszValue)
{
	_CSTRING_NS::CString strValue;
	strValue = lpszValue;

	return (_SetAttribute(strName, strValue));
}

// set bool attribute
BOOL CNode::SetAttribute(CStringIn strName, bool fValue)
{
	_CSTRING_NS::CString strValue;
	strValue = fValue ? _T("1") : _T("0");

	return (_SetAttribute(strName, strValue));
}

// set int attribute
BOOL CNode::SetAttribute(CStringIn strName, int nValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%d"), nValue);

	return (_SetAttribute(strName, strValue));
}

// set long attribute
BOOL CNode::SetAttribute(CStringIn strName, long nValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%ld"), nValue);

	return (_SetAttribute(strName, strValue));
}

// set __int64 attribute
BOOL CNode::SetAttribute(CStringIn strName, __int64 qnValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%I64d"), qnValue);

	return (_SetAttribute(strName, strValue));
}

// set float attribute
BOOL CNode::SetAttribute(CStringIn strName, float fValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%f"), fValue);

	return (_SetAttribute(strName, strValue));
}

// set double attribute
BOOL CNode::SetAttribute(CStringIn strName, double dValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%.12f"), dValue);

	return (_SetAttribute(strName, strValue));
}

// set DWORD attribute
BOOL CNode::SetAttribute(CStringIn strName, DWORD dwValue)
{
	_CSTRING_NS::CString strValue;
	strValue.Format(_T("%lu"), dwValue);

	return (_SetAttribute(strName, strValue));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations: remove attribute

BOOL CNode::RemoveAttribute(CStringIn strName)
{
	XML_ASSERT(m_ipDomNode != NULL);

	HRESULT hr = S_OK;

	try
	{
		IDomNamedNodeMapPtr ipNamedNodeMap = NULL;
		hr = m_ipDomNode->get_attributes(&ipNamedNodeMap);
		XML_ASSERT(SUCCEEDED(hr));

		if (SUCCEEDED(hr))
		{
			ipNamedNodeMap->removeNamedItem(_bstr_t(strName));
		}
		XML_ASSERT(SUCCEEDED(hr));

		ReleaseInterface(ipNamedNodeMap);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::RemoveAttribute() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (SUCCEEDED(hr));
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations: misc

BOOL CNode::IsNull(void)
{
	return (m_ipDomNode == NULL);
}

CStringRet CNode::GetName(void) const
{
	XML_ASSERT(m_ipDomNode != NULL);

	CString strName;

	try
	{
		BSTR bstrTemp = NULL;
		HRESULT hr = m_ipDomNode->get_nodeName(&bstrTemp);
		XML_ASSERT(SUCCEEDED(hr));
		strName = static_cast<LPCTSTR>(_bstr_t(bstrTemp, true));

		if (bstrTemp != NULL)
		{
			::SysFreeString(bstrTemp);
			bstrTemp = NULL;
		}
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::GetName() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (strName);
}

CNode& CNode::Detach(void)
{
	try
	{
		IDomNodePtr ipDomNodeParent = NULL;
		HRESULT hr = m_ipDomNode->get_parentNode(&ipDomNodeParent);
		XML_ASSERT(SUCCEEDED(hr));

		ipDomNodeParent->removeChild(m_ipDomNode);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::Detach() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (*this);
}

void CNode::Release(void)
{
	ReleaseInterface(m_ipDomNode);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// XPath

CNodePtr CNode::SelectSingleNode(LPCTSTR lpszPath)
{
	XML_ASSERT(m_ipDomNode != NULL);

	CNodePtr nodePtr(new CNode());

	try
	{
		IDomNodePtr ipItem = NULL;
		ipItem = m_ipDomNode->selectSingleNode(_bstr_t(lpszPath));

		nodePtr->m_ipDomNode = ipItem;
		ReleaseInterface(ipItem);
	}
	catch (const _com_error& err)
	{
		XML_TRACE( _T("CNode::SelectSingleNode() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtr);
}

CNodeListPtr CNode::SelectNodes(LPCTSTR lpszPath)
{
	XML_ASSERT(m_ipDomNode != NULL);

	CNodeListPtr nodeListPtr(new CNodeList());

	try
	{
		IDomNodeListPtr ipNodeList = NULL;
		ipNodeList = m_ipDomNode->selectNodes(_bstr_t(lpszPath));

		nodeListPtr->m_ipNodeList = ipNodeList;
		ReleaseInterface(ipNodeList);
	}
	catch (const _com_error& err)
	{
		XML_TRACE( _T("CNode::SelectNodes() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodeListPtr);
}

//////////////////////////////////////////////////////////////////////////////////////////////
// implementation helpers

BOOL CNode::_GetValue(CStringOut strValue) const
{
	XML_ASSERT(m_ipDomNode != NULL);

	HRESULT hr = S_OK;

	try
	{
		BSTR bstrTemp = NULL;
		hr = m_ipDomNode->get_text(&bstrTemp);
		XML_ASSERT(SUCCEEDED(hr));

		strValue = static_cast<LPCTSTR>(_bstr_t(bstrTemp, true));

		if (bstrTemp != NULL)
		{
			::SysFreeString(bstrTemp);
			bstrTemp = NULL;
		}
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::_GetValue() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (SUCCEEDED(hr));
}

BOOL CNode::_SetValue(CStringIn strValue)
{
	XML_ASSERT(m_ipDomNode != NULL);

	HRESULT hr = S_OK;

	try
	{
		hr = m_ipDomNode->put_text(_bstr_t(strValue));
		XML_ASSERT(SUCCEEDED(hr));
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::_SetValue() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (SUCCEEDED(hr));
}

BOOL CNode::_GetAttribute(CStringIn strName, CStringOut strValue) const
{
	XML_ASSERT(m_ipDomNode != NULL);

	HRESULT hr = S_OK;

	try
	{
		IDomNamedNodeMapPtr ipNamedNodeMap = NULL;
		hr = m_ipDomNode->get_attributes(&ipNamedNodeMap);
		if (FAILED(hr))
		{
			XML_ASSERT(FALSE);
			return (FALSE);
		}

		IDomNodePtr ipDomNode = NULL;
		ipDomNode = ipNamedNodeMap->getNamedItem(_bstr_t(strName));

		if (ipDomNode == NULL)
		{
			strValue.Empty();
		}
		else {
			VARIANT varValue;
			hr = ipDomNode->get_nodeValue(&varValue);
			if (FAILED(hr))
			{
				XML_ASSERT(FALSE);
				return (FALSE);
			}

			strValue = static_cast<LPCTSTR>(_bstr_t(varValue));
		}

		ReleaseInterface(ipDomNode);
		ReleaseInterface(ipNamedNodeMap);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::_GetAttribute() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (TRUE);
}

BOOL CNode::_SetAttribute(CStringIn strName, CStringInOut strValue)
{
	XML_ASSERT(m_ipDomNode != NULL);

	HRESULT hr = S_OK;

	try
	{
		IDomNamedNodeMapPtr ipNamedNodeMap = NULL;
		hr = m_ipDomNode->get_attributes(&ipNamedNodeMap);
		if (!SUCCEEDED(hr))
		{
			XML_ASSERT(FALSE);
			return (FALSE);
		}

		IDomNodePtr ipDomNode = NULL;
		ipDomNode = ipNamedNodeMap->getNamedItem(_bstr_t(strName));

		if (ipDomNode == NULL)
		{
			strValue.Empty();
		}
		else {
			VARIANT varValue;
			hr = ipDomNode->get_nodeValue(&varValue);
			if (!SUCCEEDED(hr))
			{
				XML_ASSERT(FALSE);
				return (FALSE);
			}

			strValue = static_cast<LPCTSTR>(_bstr_t(varValue));
		}

		ReleaseInterface(ipDomNode);
		ReleaseInterface(ipNamedNodeMap);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CNode::_GetAttribute() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return TRUE;
}

}	// namespace XML

// end of file
