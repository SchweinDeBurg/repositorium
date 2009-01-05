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

// XML_Defs.h - generic defines, types and aliases

// Based on the code by Jerry.Wang.
// Visit http://www.codeproject.com/useritems/CXml.asp for more info.

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__XML_Defs_h_)
#define __XML_Defs_h_

#if defined(_ATL_VER) && (_ATL_VER < 0x0700)
#error required ATL version 7.0 or later
#endif   // _ATL_VER

#if !defined(_MSXML_VER)
#define _MSXML_VER 0x0300
#endif   // _MSXML_VER

#if (_MSXML_VER == 0x0200)

#import <msxml2.dll> rename_namespace("MSXML")

namespace XML
{

typedef MSXML::IXMLDOMDocument2Ptr IDomDocumentPtr;
typedef MSXML::IXMLDOMNodePtr IDomNodePtr;
typedef MSXML::IXMLDOMNamedNodeMapPtr IDomNamedNodeMapPtr;
typedef MSXML::IXMLDOMElementPtr IDomElementPtr;
typedef MSXML::IXMLDOMNodeListPtr IDomNodeListPtr;

typedef MSXML::DOMDocument CoDomDocument;

}	// namespace XML

#elif (_MSXML_VER == 0x0260)

#import <msxml2.dll> rename_namespace("MSXML")

namespace XML
{

typedef MSXML::IXMLDOMDocument2Ptr IDomDocumentPtr;
typedef MSXML::IXMLDOMNodePtr IDomNodePtr;
typedef MSXML::IXMLDOMNamedNodeMapPtr IDomNamedNodeMapPtr;
typedef MSXML::IXMLDOMElementPtr IDomElementPtr;
typedef MSXML::IXMLDOMNodeListPtr IDomNodeListPtr;

typedef MSXML::DOMDocument26 CoDomDocument;

}	// namespace XML

#elif (_MSXML_VER == 0x0300)

#import <msxml3.dll> rename_namespace("MSXML")

namespace XML
{

typedef MSXML::IXMLDOMDocument2Ptr IDomDocumentPtr;
typedef MSXML::IXMLDOMNodePtr IDomNodePtr;
typedef MSXML::IXMLDOMNamedNodeMapPtr IDomNamedNodeMapPtr;
typedef MSXML::IXMLDOMElementPtr IDomElementPtr;
typedef MSXML::IXMLDOMNodeListPtr IDomNodeListPtr;

typedef MSXML::DOMDocument30 CoDomDocument;

}	// namespace XML

#elif (_MSXML_VER == 0x0400)

#import <msxml4.dll> rename_namespace("MSXML")

namespace XML
{

typedef MSXML::IXMLDOMDocument2Ptr IDomDocumentPtr;
typedef MSXML::IXMLDOMNodePtr IDomNodePtr;
typedef MSXML::IXMLDOMNamedNodeMapPtr IDomNamedNodeMapPtr;
typedef MSXML::IXMLDOMElementPtr IDomElementPtr;
typedef MSXML::IXMLDOMNodeListPtr IDomNodeListPtr;

typedef MSXML::DOMDocument40 CoDomDocument;

}	// namespace XML

#elif (_MSXML_VER == 0x0600)

#import <msxml6.dll> rename_namespace("MSXML")

namespace XML
{

typedef MSXML::IXMLDOMDocument3Ptr IDomDocumentPtr;
typedef MSXML::IXMLDOMNodePtr IDomNodePtr;
typedef MSXML::IXMLDOMNamedNodeMapPtr IDomNamedNodeMapPtr;
typedef MSXML::IXMLDOMElementPtr IDomElementPtr;
typedef MSXML::IXMLDOMNodeListPtr IDomNodeListPtr;

typedef MSXML::DOMDocument60 CoDomDocument;

}	// namespace XML

#else

#error unrecognized or unsupported version of MSXML specified

#endif   // _MSXML_VER

// CString namspace definition (ATL/WTL/MFC compatibility)

#if defined(_MFC_VER)
   #define _CSTRING_NS
#elif defined(__ATLMISC_H__) && !defined(_WTL_NO_CSTRING)
   #define _CSTRING_NS WTL
#elif defined(_ATL_VER)
   #if !defined(__ATLSTR_H__)
      #include <atlstr.h>
   #endif
   #define _CSTRING_NS ATL
#else
   #include "../../CodeProject/Source/StdString.h"
   typedef CStdString CString;
   #define _CSTRING_NS
#endif

#if !defined(_MEMORY_) && !defined(__SGI_STL_MEMORY) && !defined(_STLP_MEMORY)
#pragma message(__FILE__ " : put <memory> in your PCH to speed up compilation")
#include <memory>
#endif	// !_MEMORY_ && !__SGI_STL_MEMORY && !_STLP_MEMORY

// assertions and traces

#if defined(_MFC_VER)
   #define XML_ASSERT ASSERT
   #define XML_TRACE TRACE
#elif defined(_ATL_VER)
   #define XML_ASSERT ATLASSERT
   #define XML_TRACE ATLTRACE
#elif defined(_DEBUG) && !defined(_INC_CRTDBG)
   #include <crtdbg.h>
   #define XML_ASSERT _ASSERTE
   #define XML_TRACE_BASE _CrtDbgReport args ;
   #define XML_TRACE1(fmt, arg) XML_TRACE_BASE((_CRT_WARN, __FILE__, __LINE__, NULL, fmt, arg))
   #define XML_TRACE XML_TRACE1
#endif

// forward declarations and shorten aliases

namespace XML
{

typedef const _CSTRING_NS::CString& CStringIn;
typedef _CSTRING_NS::CString& CStringOut;
typedef _CSTRING_NS::CString& CStringInOut;

typedef _CSTRING_NS::CString CStringRet;

class CDocument;
class CNode;
class CNodeList;

typedef std::auto_ptr<CNode> CNodePtr;
typedef std::auto_ptr<CNodeList> CNodeListPtr;

}	// namespace XML

// commonly used utility routines

namespace XML
{

template <typename _Interface, const IID* _IID>
inline void ReleaseInterface(_com_ptr_t< _com_IIID<_Interface, _IID> > ip)
{
	if (ip != NULL)
	{
		ip.Release();
		ip = NULL;
	}
}

}	// namespace XML

#endif	__XML_Defs_h_

// end of file
