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

// XML_Document.cpp - implementation of the XML::CDocument class

// Based on the code by Jerry.Wang.
// Visit http://www.codeproject.com/useritems/CXml.asp for more info.

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "XML_Document.h"

#if !defined(_INC_SHLWAPI)
#pragma message(__FILE__ " : put <shlwapi.h> in your PCH to speed up compilation")
#include <shlwapi.h>
#endif   // _INC_SHLWAPI
#pragma comment(lib, "shlwapi.lib")

#if defined(_ATL_VER) && !defined(_MFC_VER) && !defined(__ATLFILE_H__)
#pragma message(__FILE__ " : put <atlfile.h> in your PCH to speed up compilation")
#include <atlfile.h>
#endif   // _ATL_VER && !__ATLFILE_H__

//////////////////////////////////////////////////////////////////////////////////////////////
// avoid unwanted warnings

#if !defined(_DEBUG)
#pragma warning(disable: 4101)
#endif   // _DEBUG

namespace XML
{

//////////////////////////////////////////////////////////////////////////////////////////////
// construction/destruction

CDocument::CDocument(void):
m_ipDocument(NULL),
m_dwVersion(0)
{
}

CDocument::~CDocument(void)
{
	Close();
}

//////////////////////////////////////////////////////////////////////////////////////////////
// operations

CStringRet CDocument::GetFilePath(void) const
{
	return (m_strFilePath);
}

BOOL CDocument::Base64Decode(CStringIn strSrc, LPBYTE& pbBuffer, LONG& cbSize)
{
	XML_ASSERT(m_ipDocument != NULL);

	try
	{
		IDomElementPtr ipDomElement = NULL;
		ipDomElement = m_ipDocument->createElement(_bstr_t(_T("Base64")));
		XML_ASSERT(ipDomElement != NULL);

		HRESULT hr = S_OK;
		hr = ipDomElement->put_dataType(_bstr_t(_T("bin.base64")));
		XML_ASSERT(SUCCEEDED(hr));

		hr = ipDomElement->put_text(_bstr_t(strSrc));
		XML_ASSERT(SUCCEEDED(hr));

		hr = ::SafeArrayGetUBound(ipDomElement->nodeTypedValue.parray, 1, &cbSize);
		XML_ASSERT(SUCCEEDED(hr));
		++cbSize;

		pbBuffer = new BYTE[cbSize];
		memset(pbBuffer, 0, cbSize);

		memmove(pbBuffer, LPVOID(ipDomElement->nodeTypedValue.parray->pvData), cbSize);

		ReleaseInterface(ipDomElement);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CDocument::Base64Decode() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (TRUE);
}

CStringRet CDocument::Base64Encode(LPBYTE pbBuffer, ULONG cbSize)
{
	XML_ASSERT(m_ipDocument != NULL);
	XML_ASSERT(pbBuffer != NULL);

	_CSTRING_NS::CString strText = _T("");

	try
	{
		IDomElementPtr ipDomElement = NULL;
		ipDomElement = m_ipDocument->createElement(_bstr_t(_T("Base64")));
		XML_ASSERT(ipDomElement != NULL);

		HRESULT hr = S_OK;
		hr = ipDomElement->put_dataType(_bstr_t(_T("bin.base64")));
		XML_ASSERT(SUCCEEDED(hr));

		SAFEARRAY* pSafeArr = ::SafeArrayCreateVector(VT_UI1, 0L, cbSize);
		memmove(pSafeArr->pvData, pbBuffer, cbSize);

		VARIANT var;
		::VariantInit(&var);
		var.vt = VT_ARRAY | VT_UI1;
		var.parray = pSafeArr;
		ipDomElement->nodeTypedValue = var;

		BSTR bstrTemp = NULL;
		hr = ipDomElement->get_text(&bstrTemp);
		XML_ASSERT(SUCCEEDED(hr));
		strText = static_cast<LPCTSTR>(_bstr_t(bstrTemp, true));

		if (bstrTemp != NULL )
		{
			::SysFreeString(bstrTemp);
			bstrTemp = NULL;
		}

		ReleaseInterface(ipDomElement);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CDocument::Base64Encode() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (strText);
}

BOOL CDocument::Open(LPCTSTR lpszFilePath)
{
	HRESULT hr = S_FALSE;

	if (!CreateParser())
	{
		return (FALSE);
	}

	m_strFilePath = lpszFilePath;
	VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

	try
	{
		m_ipDocument->preserveWhiteSpace = VARIANT_FALSE;
		m_ipDocument->validateOnParse = VARIANT_FALSE;

		if (::PathFileExists(lpszFilePath))
		{
			vbSuccessful = m_ipDocument->load(_variant_t(lpszFilePath));
		}
		else
		{
			MSXML::IXMLDOMProcessingInstructionPtr ipDomInstr = NULL;
			ipDomInstr = m_ipDocument->createProcessingInstruction(_bstr_t(_T("xml")), _bstr_t(_T("version=\"1.0\"")));
			XML_ASSERT(ipDomInstr != NULL);

			hr = m_ipDocument->appendChild(ipDomInstr);
			XML_ASSERT(SUCCEEDED(hr));

			// create the root element
			IDomElementPtr ipDomElementRoot = NULL;
			ipDomElementRoot = m_ipDocument->createElement(_bstr_t(_T("xmlRoot")));
			m_ipDocument->appendChild(ipDomElementRoot);
			ReleaseInterface(ipDomElementRoot);

			vbSuccessful = SUCCEEDED(hr) ? VARIANT_TRUE : VARIANT_FALSE;
		}

		m_ipDocument->setProperty(_bstr_t(_T("SelectionLanguage")), _variant_t(_T("XPath")));
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CDocument::Open() failed: %s\n"), err.ErrorMessage());
		vbSuccessful = FALSE;
		throw;
	}
	catch (...)
	{
		vbSuccessful = FALSE;
		XML_ASSERT(FALSE);
	}

	return (vbSuccessful == VARIANT_TRUE);
}

BOOL CDocument::LoadContent(LPCTSTR lpszContent)
{
	if (!CreateParser())
	{
		return (FALSE);
	}

	VARIANT_BOOL vbSuccessful = VARIANT_TRUE;

	try
	{
		m_ipDocument->preserveWhiteSpace = VARIANT_FALSE;
		m_ipDocument->validateOnParse = VARIANT_FALSE;

		vbSuccessful = m_ipDocument->loadXML(_bstr_t(lpszContent));

		m_ipDocument->setProperty(_bstr_t(_T("SelectionLanguage")), _variant_t(_T("XPath")));
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CDocument::LoadContent() failed:%s\n"), err.ErrorMessage());
		vbSuccessful = FALSE;
		throw;
	}
	catch (...)
	{
		vbSuccessful = FALSE;
		XML_ASSERT(FALSE);
	}

	return (vbSuccessful == VARIANT_TRUE);
}

BOOL CDocument::Save(LPCTSTR lpszFilePath)
{
	if (m_ipDocument == NULL)
	{
		return (FALSE);
	}

	XML_ASSERT(m_ipDocument != NULL);

	HRESULT hr = S_OK;
	if (lpszFilePath == NULL)
	{
		hr = m_ipDocument->save(_variant_t(static_cast<LPCTSTR>(m_strFilePath)));
	}
	else {
		hr = m_ipDocument->save(_variant_t(lpszFilePath));
	}

	return (SUCCEEDED(hr));
}

void CDocument::Close(void)
{
	m_strFilePath = _T("");
	m_dwVersion = 0;
	ReleaseInterface(m_ipDocument);
}

CStringRet CDocument::GetLastError(IDomDocumentPtr ipDocument)
{
	if (ipDocument == NULL )
	{
		XML_ASSERT(m_ipDocument != NULL);
		ipDocument = m_ipDocument;
	}

	HRESULT hr = S_OK;
	MSXML::IXMLDOMParseErrorPtr ipParseError = NULL;

	hr = ipDocument->get_parseError(&ipParseError);
	XML_ASSERT(SUCCEEDED(hr));

	_CSTRING_NS::CString strLastErr;
	if (ipParseError != NULL)
	{
		BSTR bstrTemp = NULL;
		hr = ipParseError->get_reason(&bstrTemp);
		XML_ASSERT(SUCCEEDED(hr));

		strLastErr = static_cast<LPCTSTR>(_bstr_t(bstrTemp, true));

		if (bstrTemp != NULL)
		{
			::SysFreeString(bstrTemp);
			bstrTemp = NULL;
		}

		ReleaseInterface(ipParseError);
	}

	return (strLastErr);
}

CNodePtr CDocument::GetRoot(void)
{
	XML_ASSERT(m_ipDocument != NULL);

	CNodePtr nodePtrRoot(new CNode());

	try
	{
		IDomElementPtr ipDomElement = NULL;
		HRESULT hr = m_ipDocument->get_documentElement(&ipDomElement);
		XML_ASSERT(SUCCEEDED(hr));
		XML_ASSERT(ipDomElement != NULL);

		nodePtrRoot->m_ipDomNode = ipDomElement;
		ReleaseInterface(ipDomElement);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CDocument::GetRoot() failed:%s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtrRoot);
}

CNodePtr CDocument::SelectSingleNode(LPCTSTR lpszPath)
{
	XML_ASSERT(m_ipDocument != NULL);

	CNodePtr nodePtr(new CNode());

	CNodePtr nodePtrRoot = GetRoot();
	if (!nodePtrRoot->IsNull())
	{
		*nodePtr = nodePtrRoot->SelectSingleNode(lpszPath);
	}

	return (nodePtr);
}

CNodeListPtr CDocument::SelectNodes(LPCTSTR lpszPath)
{
	XML_ASSERT(m_ipDocument != NULL);

	CNodeListPtr nodeListPtr(new CNodeList());

	CNodePtr nodePtrRoot = GetRoot();
	if (!nodePtrRoot->IsNull())
	{
		*nodeListPtr = nodePtrRoot->SelectNodes(lpszPath);
	}

	return (nodeListPtr);
}

CNodePtr CDocument::CreateNode(CStringIn strName)
{
	XML_ASSERT(m_ipDocument != NULL);

	CNodePtr nodePtrNew(new CNode());

	try
	{
		IDomNodePtr ipChildNode = NULL;
		ipChildNode = m_ipDocument->createElement(_bstr_t(strName));
		XML_ASSERT(ipChildNode != NULL);
		nodePtrNew->m_ipDomNode = ipChildNode;
		ReleaseInterface(ipChildNode);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CDocument::CreateNode() failed: %s\n"), err.ErrorMessage());
		throw;
	}

	return (nodePtrNew);
}

DWORD CDocument::GetVersion(void) const
{
	return (m_dwVersion);
}

BOOL CDocument::Transform(CStringIn strXslFile, CStringIn strDestFile)
{
	XML_ASSERT(m_ipDocument != NULL);
	HRESULT hr = S_OK;

	IDomDocumentPtr ipStyleSheet;
	MSXML::IXSLTemplatePtr ipXslTemplate;
	MSXML::IXSLProcessorPtr ipXslProcessor;

	IStream * ipOutStream = NULL;
	LPVOID pvOutput = NULL;

	try
	{
		hr = ipXslTemplate.CreateInstance(__uuidof(MSXML::XSLTemplate));
		XML_ASSERT(SUCCEEDED(hr));

		hr = ipStyleSheet.CreateInstance(__uuidof(MSXML::FreeThreadedDOMDocument));
		XML_ASSERT(SUCCEEDED(hr));

		VARIANT_BOOL vbSuccessful = VARIANT_TRUE;
		vbSuccessful = ipStyleSheet->load(_variant_t(static_cast<LPCTSTR>(strXslFile)));
		XML_ASSERT(vbSuccessful == VARIANT_TRUE);

		hr = ipXslTemplate->putref_stylesheet(ipStyleSheet);
		if (!SUCCEEDED(hr))
		{
			XML_TRACE(_T("Error occurs when transform XSL file %s\n"), static_cast<LPCTSTR>(strXslFile));
			return (FALSE);
		}

		ipXslProcessor = ipXslTemplate->createProcessor();
		XML_ASSERT(ipXslProcessor != NULL);

		::CreateStreamOnHGlobal(NULL, TRUE, &ipOutStream);
		ipXslProcessor->put_output(_variant_t(ipOutStream));

		hr = ipXslProcessor->put_input(_variant_t((IUnknown*)m_ipDocument));
		XML_ASSERT(SUCCEEDED(hr));

		vbSuccessful = ipXslProcessor->transform();
		XML_ASSERT(vbSuccessful == VARIANT_TRUE);

		HGLOBAL hg = NULL;
		hr = ipOutStream->Write(_T("\0"), 1, 0);
		::GetHGlobalFromStream(ipOutStream, &hg);
		::GlobalUnlock(hg);

		LARGE_INTEGER qnStart = {0, 0};
		ULARGE_INTEGER cbSize = {0, 0};
		ipOutStream->Seek(qnStart, STREAM_SEEK_CUR, &cbSize);

		pvOutput = ::GlobalLock(hg);
#if defined(_MFC_VER)
		CFile fileOut;
		fileOut.Open(strDestFile, CFile::modeCreate | CFile::modeWrite, NULL);
		fileOut.Write(pvOutput, cbSize.LowPart);
		fileOut.Flush();
		fileOut.Close();
#elif defined(_ATL_VER)
		ATL::CAtlFile fileOut;
		fileOut.Create(strDestFile, GENERIC_WRITE, 0, CREATE_ALWAYS);
		fileOut.Write(pvOutput, cbSize.LowPart);
		fileOut.Flush();
		fileOut.Close();
#else
		HANDLE hFileOut = INVALID_HANDLE_VALUE;
		hFileOut = ::CreateFile(strDestFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		DWORD cbWritten = 0;
		::WriteFile(hFileOut, pvOutput, cbSize.LowPart, &cbWritten, NULL);
		::FlushFileBuffers(hFileOut);
		::CloseHandle(hFileOut);
#endif
		::GlobalUnlock(hg);

		ReleaseInterface(ipXslProcessor);
		ReleaseInterface(ipStyleSheet);
		ReleaseInterface(ipXslTemplate);

		return (TRUE);
	}
	catch (const _com_error& err)
	{
		XML_TRACE(_T("CDocument::Transform() failed: %s\n"), err.ErrorMessage());
		throw;
	}
}

BOOL CDocument::CreateParser(void)
{
	Close();

	HRESULT hr =  m_ipDocument.CreateInstance(__uuidof(CoDomDocument));
	if (SUCCEEDED(hr))
	{
		m_dwVersion = _MSXML_VER;
		return (TRUE);
	}
	else {
		return (FALSE);
	}
}

}	// namespace XML

// end of file
