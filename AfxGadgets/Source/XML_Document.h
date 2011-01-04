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

// XML_Document.h - interface of the XML::CDocument class

// Based on the code by Jerry.Wang.
// Visit http://www.codeproject.com/useritems/CXml.asp for more info.

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__XML_Document_h_)
#define __XML_Document_h_

#include "XML_Defs.h"
#include "XML_Node.h"

namespace XML
{

class CDocument
{

// construction/destruction
public:
	CDocument(void);
	~CDocument(void);

// operations
public:
	CStringRet GetFilePath(void) const;
	BOOL Base64Decode(CStringIn strSrc, LPBYTE& pbBuffer, LONG& cbSize);
	CStringRet Base64Encode(LPBYTE pbBuffer, ULONG cbSize);
	// open XML file
	BOOL Open(LPCTSTR lpszFilePath);
	// load XML string
	BOOL LoadContent(LPCTSTR lpszContent);
	// save xml file
	BOOL Save(LPCTSTR lpszFilePath = NULL);
	// close XML file
	void Close(void);
	// get the latest error message
	CStringRet GetLastError(IDomDocumentPtr ipDocument = NULL);
	// get the root element of
	CNodePtr GetRoot(void);
	// get single node by XPath
	CNodePtr SelectSingleNode(LPCTSTR lpszPath);
	// get nodes by XPath
	CNodeListPtr SelectNodes(LPCTSTR lpszPath);
	// create node
	CNodePtr CreateNode(CStringIn strName);
	// get the current version of MSXML
	DWORD GetVersion(void) const;
	BOOL Transform(CStringIn strXslFile, CStringIn strDestFile);
protected:
	BOOL CreateParser(void);

// data members
protected:
	IDomDocumentPtr m_ipDocument;
	_CSTRING_NS::CString m_strFilePath;
	DWORD m_dwVersion;
};

}	// namespace XML

#endif	// __XML_Document_h_

// end of file
