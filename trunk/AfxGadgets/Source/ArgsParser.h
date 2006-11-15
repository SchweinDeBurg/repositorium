// AfxGadgets library.
// Copyright (c) 2004-2006 by Elijah Zarezky,
// All rights reserved.

// ArgsParser.h - interface of the CArgsParser class

// Based on the code by Pavel Antonov and changes contributed by dB (www.dblock.org).
// Visit http://www.codeproject.com/cpp/cmdlineparser.asp for more info.

#if !defined(__ArgsParser_h)
#define __ArgsParser_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif	// _MSC_VER

#if !defined(__AFXTEMPL_H__)
#pragma message(__FILE__ " : put <afxtempl.h> in your PCH to speed up compilation")
#include <afxtempl.h>
#endif	// __AFXTEMPL_H__

#if !defined(ARGS_PARSER_NO_TIME) && !defined(__ATLCOMTIME_H__)
#pragma message(__FILE__ " : put <atlbase.h> in your PCH to speed up compilation")
#include <atlbase.h>
#pragma message(__FILE__ " : put <ATLComTime.h> in your PCH to speed up compilation")
#include <ATLComTime.h>
#endif	// ARGS_PARSER_NO_TIME && __ATLCOMTIME_H__

// interface

class CArgsParser: public CObject
{
	DECLARE_DYNAMIC(CArgsParser)

// construction/destruction
public:
	CArgsParser(LPCTSTR pszArguments = ::GetCommandLine(), bool fCaseSensitive = false);
	virtual ~CArgsParser(void);

// operations
public:
	void Parse(LPCTSTR pszArguments, bool fCaseSensitive = false);
	CString GetStringValue(LPCTSTR pszKeyName);
	bool GetIntValue(LPCTSTR pszKeyName, int& nDest, int nRadix = 10);
	int GetIntValue(LPCTSTR pszKeyName, int nRadix = 10);
	bool GetUIntValue(LPCTSTR pszKeyName, UINT& uDest, int nRadix = 10);
	UINT GetUIntValue(LPCTSTR pszKeyName, int nRadix = 10);

#if !defined(ARGS_PARSER_NO_TIME)
	bool GetTimeValue(LPCTSTR pszKeyName, CTime& timeDest, DWORD fdwFlags = 0,
		LCID Locale = LANG_USER_DEFAULT);
	CTime GetTimeValue(LPCTSTR pszKeyName, DWORD fdwFlags = 0, LCID Locale = LANG_USER_DEFAULT);
	bool GetOleTimeValue(LPCTSTR pszKeyName, COleDateTime& odtDest, DWORD fdwFlags = 0,
		LCID Locale = LANG_USER_DEFAULT);
	COleDateTime GetOleTimeValue(LPCTSTR pszKeyName, DWORD fdwFlags = 0, LCID Locale = LANG_USER_DEFAULT);
#endif	// ARGS_PARSER_NO_TIME

	bool HasKey(LPCTSTR pszKeyName);
	bool HasValue(LPCTSTR pszKeyName);

// attributes
public:
	CString m_strArguments;
	bool m_fCaseSensitive;
	CMap<CString, LPCTSTR, CString, LPCTSTR> m_mapKeys;
	static const TCHAR m_szDelimeters[];
	static const TCHAR m_szQuotes[];
	static const TCHAR m_szValueSep[];

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

// inlines

inline CString CArgsParser::GetStringValue(LPCTSTR pszKeyName)
{
	ASSERT(AfxIsValidString(pszKeyName));

	return (m_mapKeys[pszKeyName]);
}

inline bool CArgsParser::HasKey(LPCTSTR pszKeyName)
{
	ASSERT(AfxIsValidString(pszKeyName));

	CString strValue;
	return (m_mapKeys.Lookup(pszKeyName, strValue) != 0);
}

inline bool CArgsParser::HasValue(LPCTSTR pszKeyName)
{
	ASSERT(AfxIsValidString(pszKeyName));

	CString strValue;
	return (m_mapKeys.Lookup(pszKeyName, strValue) != 0 && !strValue.IsEmpty());
}

#endif	// __ArgsParser_h

// end of file
