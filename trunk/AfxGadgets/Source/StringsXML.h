// AfxGadgets library.
// Copyright (c) 2005-2006 by Elijah Zarezky,
// All rights reserved.

// StringsXML.h - interface of the CStringsXML class

#if !defined(__StringsXML_h)
#define __StringsXML_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

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
