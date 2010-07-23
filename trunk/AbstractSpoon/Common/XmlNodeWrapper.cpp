// Copyright (C) 2003-2005 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

// XmlNodeWrapper.cpp: implementation of the CXmlNodeWrapper class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "XmlNodeWrapper.h"
#include <comutil.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// fix for bad typedef in <wtypes.h>
#ifdef VARIANT_TRUE
#	undef VARIANT_TRUE
#	define VARIANT_TRUE ((VARIANT_BOOL)(-1))
#endif

const LPCTSTR DEFAULT_HEADER = _T("version=\"1.0\" encoding=\"windows-1252\"");

CXmlNodeWrapper::CXmlNodeWrapper()
{
}

CXmlNodeWrapper::CXmlNodeWrapper(MSXML2::IXMLDOMNodePtr pNode, BOOL bAutoRelease)
{
	m_xmlnode = pNode;
	m_bAutoRelease = bAutoRelease;
}
void CXmlNodeWrapper::operator=(MSXML2::IXMLDOMNodePtr pNode)
{
	if (IsValid())
	{
		m_xmlnode.Release();
	}
	m_xmlnode = pNode;
}

CXmlNodeWrapper::~CXmlNodeWrapper()
{
	if (!m_bAutoRelease)
	{
		m_xmlnode.Detach();
	}
}


CString CXmlNodeWrapper::GetValue(LPCTSTR valueName)
{
	if (!IsValid())
	{
		return _T("");
	}

	MSXML2::IXMLDOMNodePtr attribute = m_xmlnode->Getattributes()->getNamedItem(valueName);
	if (attribute)
	{
		return (LPCSTR)attribute->Gettext();
	}
	return _T("");
}

BOOL CXmlNodeWrapper::IsValid()
{
	if (m_xmlnode == NULL)
	{
		return FALSE;
	}
	if (m_xmlnode.GetInterfacePtr() == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetPrevSibling()
{
	if (!IsValid())
	{
		return NULL;
	}
	return m_xmlnode->GetpreviousSibling().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNextSibling()
{
	if (!IsValid())
	{
		return NULL;
	}
	return m_xmlnode->GetnextSibling().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNode(LPCTSTR nodeName)
{
	if (!IsValid())
	{
		return NULL;
	}
	try
	{
		return m_xmlnode->selectSingleNode(nodeName).Detach();
	}
	catch (_com_error e)
	{
		CString err = e.ErrorMessage();
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::GetNode(int nodeIndex)
{
	if (!IsValid())
	{
		return NULL;
	}

	return m_xmlnode->GetchildNodes()->Getitem(nodeIndex).Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::FindNode(LPCTSTR searchString)
{
	if (!IsValid())
	{
		return NULL;
	}

	try
	{
		return m_xmlnode->selectSingleNode(searchString).Detach();
	}
	catch (_com_error e)
	{
		CString err = e.ErrorMessage();
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Detach()
{
	if (IsValid())
	{
		return m_xmlnode.Detach();
	}
	else
	{
		return NULL;
	}
}

long CXmlNodeWrapper::NumNodes()
{
	if (IsValid())
	{
		return m_xmlnode->GetchildNodes()->Getlength();
	}
	else
	{
		return 0;
	}
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName, LPCTSTR value)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();

	if (xmlDocument)
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attributes = m_xmlnode->Getattributes();

		if (attributes)
		{
			MSXML2::IXMLDOMAttributePtr attribute = xmlDocument->createAttribute(valueName);

			if (attribute)
			{
				attribute->Puttext(value);
				attributes->setNamedItem(attribute);
			}
		}
	}
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName, int value)
{
	CString str;
	str.Format(_T("%ld"), value);
	SetValue(valueName, str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName, short value)
{
	CString str;
	str.Format(_T("%hd"), value);
	SetValue(valueName, str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName, double value)
{
	CString str;
	str.Format(_T("%f"), value);
	SetValue(valueName, str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName, float value)
{
	CString str;
	str.Format(_T("%f"), value);
	SetValue(valueName, str);
}

void CXmlNodeWrapper::SetValue(LPCTSTR valueName, bool value)
{
	CString str;
	if (value)
	{
		str = _T("True");
	}
	else
	{
		str = _T("False");
	}
	SetValue(valueName, str);
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::AppendChild(MSXML2::IXMLDOMNodePtr pNode)
{
	return m_xmlnode->appendChild(pNode);
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertNode(int index, LPCTSTR nodeName)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();

	if (xmlDocument)
	{
		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT), nodeName, "");
		MSXML2::IXMLDOMNode* refNode = GetNode(index);
		MSXML2::IXMLDOMNode* pNewNode = NULL;

		if (refNode)
		{
			pNewNode = m_xmlnode->insertBefore(newNode.Detach(), refNode);
		}
		else
		{
			pNewNode = m_xmlnode->appendChild(newNode.Detach());
		}

		return pNewNode;
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertNode(int index, MSXML2::IXMLDOMNodePtr pNode)
{
	MSXML2::IXMLDOMNodePtr newNode = pNode->cloneNode(VARIANT_TRUE);

	if (newNode)
	{
		MSXML2::IXMLDOMNode* refNode = GetNode(index);
		MSXML2::IXMLDOMNode* pNewNode = NULL;

		if (refNode)
		{
			pNewNode = m_xmlnode->insertBefore(newNode.Detach(), refNode);
		}
		else
		{
			pNewNode = m_xmlnode->appendChild(newNode.Detach());
		}

		return pNewNode;
	}
	else
	{
		return NULL;
	}
}

CString CXmlNodeWrapper::GetXML()
{
	if (IsValid())
	{
		return (LPCSTR)m_xmlnode->Getxml();
	}
	else
	{
		return _T("");
	}
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::RemoveNode(MSXML2::IXMLDOMNodePtr pNode)
{
	if (!IsValid())
	{
		return NULL;
	}
	return m_xmlnode->removeChild(pNode).Detach();
}

/* ********************************************************************************************************* */

BOOL CXmlDocumentWrapper::s_bVer3orGreater = -1;

CXmlDocumentWrapper::CXmlDocumentWrapper(LPCTSTR header, LPCTSTR szRootItem)
{
	try
	{
		m_xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);
		m_xmldoc->put_validateOnParse(VARIANT_FALSE);
		m_xmldoc->put_async(VARIANT_FALSE);

		if (szRootItem && *szRootItem)
		{
			CString sRoot;
			sRoot.Format(_T("<%s/>"), szRootItem);
			LoadXML(sRoot);
		}

		// set header afterwards so it not overwritten
		if (header && *header)
		{
			SetHeader(header);
		}
	}
	catch (...)
	{
	}
}

CXmlDocumentWrapper::CXmlDocumentWrapper(MSXML2::IXMLDOMDocumentPtr pDoc)
{
	m_xmldoc = pDoc;
}

CXmlDocumentWrapper::~CXmlDocumentWrapper()
{
}

BOOL CXmlDocumentWrapper::IsValid() const
{
	if (m_xmldoc == NULL)
	{
		return FALSE;
	}

	if (m_xmldoc.GetInterfacePtr() == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

CString CXmlDocumentWrapper::GetHeader(BOOL bAsXml) const
{
	CString sHeader;

	if (IsValid())
	{
		CXmlNodeWrapper nodeHdr(m_xmldoc->childNodes->item[0]);

		if (nodeHdr.IsValid())
		{
			if (nodeHdr.GetXML().Find(_T("?xml")) == 1) // <?xml
			{
				int nAttribs = nodeHdr.NumAttributes();

				for (int nAttrib = 0; nAttrib < nAttribs; nAttrib++)
				{
					CString sAttrib;
					sAttrib.Format(_T("%s=\"%s\" "), nodeHdr.GetAttribName(nAttrib),
					               nodeHdr.GetAttribVal(nAttrib));
					sHeader += sAttrib;
				}
			}
		}

		if (sHeader.IsEmpty())
		{
			sHeader = DEFAULT_HEADER;

			// get active code page
			CString sCodePage;

			GetLocaleInfo(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, sCodePage.GetBuffer(7), 6);
			sCodePage.ReleaseBuffer();

			// and replace in header
			if (_ttoi(sCodePage) > 0)
			{
				sHeader.Replace(_T("1252"), sCodePage);
			}
		}

		if (bAsXml)
		{
			sHeader = _T("<?xml ") + sHeader;
			sHeader += _T("?>");
		}
	}

	return sHeader;
}

void CXmlDocumentWrapper::SetHeader(LPCTSTR szHeader)
{
	if (IsValid())
	{
		_bstr_t name(CXmlNodeWrapper::ConvertStringToBSTR(_T("xml")), FALSE);
		_bstr_t bstr(CXmlNodeWrapper::ConvertStringToBSTR(szHeader), FALSE);

		MSXML2::IXMLDOMProcessingInstructionPtr pHdr = m_xmldoc->createProcessingInstruction(name, bstr);

		// always insert header right at the start
		MSXML2::IXMLDOMNodePtr pNode = m_xmldoc->childNodes->item[0];

		if (pNode)
		{
			CString sXml = (LPCTSTR)pNode->Getxml();
			_variant_t varFirst(pNode.GetInterfacePtr());

			m_xmldoc->insertBefore(pHdr, varFirst);
		}
		else
		{
			m_xmldoc->appendChild(pHdr);
		}
	}
}

MSXML2::IXMLDOMDocument* CXmlDocumentWrapper::Detach()
{
	if (!IsValid())
	{
		return NULL;
	}
	return m_xmldoc.Detach();
}

MSXML2::IXMLDOMDocument* CXmlDocumentWrapper::Clone()
{
	if (!IsValid())
	{
		return NULL;
	}
	MSXML2::IXMLDOMDocumentPtr xmldoc;
	xmldoc.CreateInstance(MSXML2::CLSID_DOMDocument);

	_variant_t v(xmldoc.GetInterfacePtr());
	m_xmldoc->save(v);

	return xmldoc.Detach();
}

BOOL CXmlDocumentWrapper::Load(LPCTSTR path, BOOL bPreserveWhiteSpace)
{
	if (!IsValid())
	{
		return FALSE;
	}

	m_xmldoc->put_preserveWhiteSpace(bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE);
	m_xmldoc->put_async(VARIANT_FALSE);

	_bstr_t bstr(CXmlNodeWrapper::ConvertStringToBSTR(path), FALSE);

	return (VARIANT_TRUE == m_xmldoc->load(bstr));
}

BOOL CXmlDocumentWrapper::LoadXML(LPCTSTR xml/*, BOOL bPreserveWhiteSpace*/)
{
	if (!IsValid())
	{
		return FALSE;
	}

	//m_xmldoc->put_preserveWhiteSpace(bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE);

	_bstr_t bstr(CXmlNodeWrapper::ConvertStringToBSTR(xml), FALSE);

	return (VARIANT_TRUE == m_xmldoc->loadXML(bstr));
}

BOOL CXmlDocumentWrapper::Save(LPCTSTR path, BOOL bPreserveWhiteSpace)
{
	if (!IsValid())
	{
		return FALSE;
	}

	try
	{
		_bstr_t bPath(CXmlNodeWrapper::ConvertStringToBSTR(path), FALSE);

		if (bPath.length() == 0)
		{
			bPath = m_xmldoc->Geturl();
		}

		m_xmldoc->put_preserveWhiteSpace(bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE);

		return (VARIANT_TRUE == m_xmldoc->save(bPath));
	}
	catch (...)
	{
		return FALSE;
	}
}

CString CXmlDocumentWrapper::Transform(LPCTSTR pathXSL) const
{
	CXmlDocumentWrapper xsl;

	try
	{
		if (xsl.Load(pathXSL))
		{
			return (LPSTR)m_xmldoc->transformNode(xsl.AsNode());
		}
	}
	catch (const _com_error& err)
	{
		AfxMessageBox(err.ErrorMessage(), MB_OK | MB_ICONERROR);
	}

	// else
	return _T("");
}

MSXML2::IXMLDOMNode* CXmlDocumentWrapper::AsNode()
{
	if (!IsValid())
	{
		return NULL;
	}

	return m_xmldoc->GetdocumentElement().Detach();
}

CString CXmlDocumentWrapper::GetXML(BOOL bPreserveWhiteSpace) const
{
	CString sXml;

	if (IsValid())
	{
		m_xmldoc->put_preserveWhiteSpace(bPreserveWhiteSpace ? VARIANT_TRUE : VARIANT_FALSE);
		sXml = (LPCSTR)m_xmldoc->Getxml();

		// Getxml doesn't return entire header so we need to fix it up here
		CString sHeader = GetHeader(TRUE);
		int nStart = sXml.Find(_T("?xml"));

		if (nStart > 0)
		{
			// find start brace
			while (sXml[nStart] != _T('<'))
			{
				nStart--;
			}

			// find end brace
			int nEnd = sXml.Find(_T('>')) + 1; // closing brace

			sXml = sXml.Left(nStart) + sHeader + sXml.Mid(nEnd);
		}
		else // just add header
		{
			sXml = sHeader + sXml;
		}
	}

	return sXml;
}

CString CXmlDocumentWrapper::GetUrl() const
{
	return (LPSTR)m_xmldoc->Geturl();
}

BOOL CXmlDocumentWrapper::IsVersion3orGreater()
{
	if (s_bVer3orGreater == -1)
	{
		try
		{
			// try to load some dummy xml
			CXmlDocumentWrapper doc;
			CString sXml;

			sXml.Format(_T("<?xml %s ?><NodeList/>"), DEFAULT_HEADER);
			s_bVer3orGreater = doc.LoadXML(sXml);
		}
		catch (CException* /*e*/)
		{
			s_bVer3orGreater = FALSE;
		}
	}

	return s_bVer3orGreater;
}

//////////////////////////////////////////////////////////////////////////

//------------------------//
// Convert char * to BSTR //
//------------------------//
BSTR CXmlNodeWrapper::ConvertStringToBSTR(const TCHAR* pSrc)
{
	if (!pSrc)
	{
		return NULL;
	}

	BSTR wsOut(NULL);

#if defined(UNICODE) || defined(_UNICODE)
	DWORD cwch = _tcslen(pSrc);
	wsOut = ::SysAllocStringLen(pSrc, cwch);
#else
	DWORD cwch = ::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0);//get size minus NULL terminator

	if (cwch)
	{
		cwch--;
		wsOut = ::SysAllocStringLen(NULL, cwch);

		if (wsOut)
		{
			if (!::MultiByteToWideChar(CP_ACP, 0, pSrc, -1, wsOut, cwch))
			{
				if (ERROR_INSUFFICIENT_BUFFER == ::GetLastError())
				{
					return wsOut;
				}
				::SysFreeString(wsOut);//must clean up
				wsOut = NULL;
			}
		}

	};
#endif   // UNICODE || _UNICODE

	return wsOut;
};

//////////////////////////////////////////////////////////////////////////////

MSXML2::IXMLDOMDocument* CXmlNodeWrapper::ParentDocument()
{
	return m_xmlnode->GetownerDocument().Detach();
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Interface()
{
	if (IsValid())
	{
		return m_xmlnode;
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertBefore(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT), nodeName, "");
		MSXML2::IXMLDOMNode* pNode = m_xmlnode->insertBefore(newNode.Detach(), (IUnknown*)refNode);

		return pNode;
	}
	return NULL;
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertAfter(MSXML2::IXMLDOMNode* refNode, LPCTSTR nodeName)
{
	MSXML2::IXMLDOMDocumentPtr xmlDocument = m_xmlnode->GetownerDocument();
	if (xmlDocument)
	{
		MSXML2::IXMLDOMNodePtr newNode = xmlDocument->createNode(_variant_t((short)MSXML2::NODE_ELEMENT), nodeName, "");
		MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
		MSXML2::IXMLDOMNode* pNode = NULL;

		if (nextNode.GetInterfacePtr() != NULL)
		{
			pNode = m_xmlnode->insertBefore(newNode.Detach(), (IUnknown*)nextNode);
		}
		else
		{
			pNode = m_xmlnode->appendChild(newNode.Detach());
		}

		return pNode;
	}
	return NULL;
}

void CXmlNodeWrapper::RemoveNodes(LPCTSTR searchStr)
{
	if (!IsValid())
	{
		return;
	}

	MSXML2::IXMLDOMNodeListPtr nodeList = m_xmlnode->selectNodes(searchStr);
	for (int i = 0; i < nodeList->Getlength(); i++)
	{
		try
		{
			MSXML2::IXMLDOMNode* pNode = nodeList->Getitem(i).Detach();
			pNode->GetparentNode()->removeChild(pNode);
		}
		catch (_com_error er)
		{
			AfxMessageBox(er.ErrorMessage());
		}
	}
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::Parent()
{
	if (IsValid())
	{
		return m_xmlnode->GetparentNode().Detach();
	}

	return NULL;
}

CXmlNodeListWrapper::CXmlNodeListWrapper()
{
}
CXmlNodeListWrapper::CXmlNodeListWrapper(MSXML2::IXMLDOMNodeListPtr pList)
{
	m_xmlnodelist = pList;
}

void CXmlNodeListWrapper::operator=(MSXML2::IXMLDOMNodeListPtr pList)
{
	if (IsValid())
	{
		m_xmlnodelist.Release();
	}

	m_xmlnodelist = pList;
}

CXmlNodeListWrapper::~CXmlNodeListWrapper()
{

}

int CXmlNodeListWrapper::Count()
{
	if (IsValid())
	{
		return m_xmlnodelist->Getlength();
	}
	else
	{
		return 0;
	}

}

BOOL CXmlNodeListWrapper::IsValid()
{
	if (m_xmlnodelist == NULL)
	{
		return FALSE;
	}

	if (m_xmlnodelist.GetInterfacePtr() == NULL)

	{
		return FALSE;
	}
	return TRUE;
}

MSXML2::IXMLDOMNode* CXmlNodeListWrapper::Next()
{
	if (IsValid())
	{
		return m_xmlnodelist->nextNode().Detach();
	}
	else
	{
		return NULL;
	}
}

void CXmlNodeListWrapper::Start()
{
	if (IsValid())
	{
		m_xmlnodelist->reset();
	}
}

MSXML2::IXMLDOMNode* CXmlNodeListWrapper::Node(int index)
{
	if (IsValid())
	{
		return m_xmlnodelist->Getitem(index).Detach();
	}
	else
	{
		return NULL;
	}
}

MSXML2::IXMLDOMDocument* CXmlNodeListWrapper::AsDocument()
{
	if (IsValid())
	{
		CXmlDocumentWrapper doc;
		doc.LoadXML(_T("<NodeList></NodeList>"));

		CXmlNodeWrapper root(doc.AsNode());

		for (int i = 0; i < m_xmlnodelist->Getlength(); i++)
		{
			root.InsertNode(root.NumNodes(), m_xmlnodelist->Getitem(i)->cloneNode(VARIANT_TRUE));
		}
		return doc.Detach();
	}
	else
	{
		return NULL;
	}
}

MSXML2::IXMLDOMNodeList* CXmlNodeWrapper::FindNodes(LPCTSTR searchStr)
{
	if (IsValid())
	{
		try
		{
			return m_xmlnode->selectNodes(searchStr).Detach();
		}
		catch (_com_error e)
		{
			CString err = e.ErrorMessage();
			return NULL;
		}
	}
	else
	{
		return NULL;
	}
}

CString CXmlNodeWrapper::Name()
{
	if (IsValid())
	{
		return (LPCSTR)m_xmlnode->GetbaseName();
	}
	return _T("");
}

MSXML2::IXMLDOMNode* CXmlNodeWrapper::InsertAfter(MSXML2::IXMLDOMNode* refNode, MSXML2::IXMLDOMNode* pNode)
{
	MSXML2::IXMLDOMNodePtr nextNode = refNode->GetnextSibling();
	MSXML2::IXMLDOMNodePtr newNode;
	// Valik - Change IXMLDOMNode* to IXMLDOMNodePtr to prevent an ambiguous symbol error (C2872) in VC 7.1
	MSXML2::IXMLDOMNodePtr pNewNode = NULL;

	if (nextNode.GetInterfacePtr() != NULL)
	{
		pNewNode = m_xmlnode->insertBefore(pNode, (IUnknown*)nextNode);
	}
	else
	{
		pNewNode = m_xmlnode->appendChild(pNode);
	}

	return pNewNode;
}

void CXmlNodeWrapper::SetText(LPCTSTR text)
{
	if (IsValid())
	{
		_bstr_t bstr(ConvertStringToBSTR(text), FALSE);
		m_xmlnode->Puttext(bstr);
	}
}

CString CXmlNodeWrapper::GetText()
{
	if (IsValid())
	{
		return (LPCSTR)m_xmlnode->Gettext();
	}
	else
	{
		return _T("");
	}
}

void CXmlNodeWrapper::ReplaceNode(MSXML2::IXMLDOMNode* pOldNode, MSXML2::IXMLDOMNode* pNewNode)
{
	if (IsValid())
	{
		m_xmlnode->replaceChild(pNewNode, pOldNode);
	}
}

int CXmlNodeWrapper::NumAttributes()
{
	if (IsValid())
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

		if (attribs)
		{
			return attribs->Getlength();
		}
	}

	return 0;
}

CString CXmlNodeWrapper::GetAttribName(int index)
{
	CString sName;

	if (IsValid())
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

		if (attribs)
		{
			MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);

			if (attrib)
			{
				sName = (LPCSTR)attrib->Getname();
				attrib.Release();
			}

			attribs.Release();
		}
	}

	return sName;
}

CString CXmlNodeWrapper::GetAttribVal(int index)
{
	CString sVal;

	if (IsValid())
	{
		MSXML2::IXMLDOMNamedNodeMapPtr attribs = m_xmlnode->Getattributes();

		if (attribs)
		{
			MSXML2::IXMLDOMAttributePtr attrib = attribs->Getitem(index);

			if (attrib)
			{
				sVal = (LPCSTR)attrib->Gettext();
				attrib.Release();
			}

			attribs.Release();
		}
	}


	return sVal;
}

CString CXmlNodeWrapper::NodeType()
{
	if (IsValid())
	{
		return (LPCSTR)m_xmlnode->GetnodeTypeString();
	}

	return _T("");
}

int CXmlNodeWrapper::NodeTypeVal()
{
	if (IsValid())
	{
		return (int)m_xmlnode->GetnodeType();
	}

	// Valik - Fully qualify the name of NODE_INVALID to prevent an ambiguous symbol error (C2872) in VC 7.1
	return MSXML2::NODE_INVALID;
}
