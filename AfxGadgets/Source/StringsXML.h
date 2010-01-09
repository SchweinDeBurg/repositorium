// AfxGadgets library.
// Copyright (c) 2004-2010 by Elijah Zarezky,
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

// StringsXML.h - interface of the CStringsXML class

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__StringsXML_h)
#define __StringsXML_h

#if !defined(__AFXTEMPL_H__)
#pragma message(__FILE__ " : put <afxtempl.h> in your PCH to speed up compilation")
#include <afxtempl.h>
#endif	// __AFXTEMPL_H__

class CStringsXML: public CObject
{
	DECLARE_DYNAMIC(CStringsXML)

// construction/destruction
public:
	CStringsXML(LPCTSTR pszStringsName);
	virtual ~CStringsXML(void);

// initialization/cleanup
public:
	int PutStringsToCache(void);
	void CleanupCache(void);

// operations
public:
	CString operator [](DWORD dwID);
	CString Format(DWORD dwID, ...);

// overridables
protected:
	virtual void GetXMLpath(CString& strDest);

// attributes
public:
	CString m_strStringsName;
	CMap<DWORD, DWORD, CString, LPCTSTR> m_mapCache;

// implementation helpers
private:
	BOOL ParseFile(class CPugXmlParser* pParser);

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __StringsXML_h

// end of file
