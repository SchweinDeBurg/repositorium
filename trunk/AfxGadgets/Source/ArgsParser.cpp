// AfxGadgets library.
// Copyright (c) 2004-2006 by Elijah Zarezky,
// All rights reserved.

// ArgsParser.cpp - implementation of the CArgsParser class

// Based on the code by Pavel Antonov and changes contributed by dB (www.dblock.org).
// Visit http://www.codeproject.com/cpp/cmdlineparser.asp for more info.

#include "stdafx.h"

#include "ArgsParser.h"

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// object model
IMPLEMENT_DYNAMIC(CArgsParser, CObject)

// construction/destruction

CArgsParser::CArgsParser(LPCTSTR pszArguments, bool fCaseSensitive):
m_strArguments(pszArguments),
m_fCaseSensitive(fCaseSensitive)
{
	if (pszArguments != NULL && *pszArguments != 0)
	{
		Parse(pszArguments, fCaseSensitive);
	}
}

CArgsParser::~CArgsParser(void)
{
}

// operations

void CArgsParser::Parse(LPCTSTR pszArguments, bool fCaseSensitive)
{
	if (! pszArguments)
		return;

	m_strArguments = pszArguments;
	m_fCaseSensitive = fCaseSensitive;

	m_mapKeys.RemoveAll();

	LPCTSTR pszCurrent = pszArguments;
	while (true)
	{
		// /Key:"arg"
		if (_tcslen(pszCurrent) == 0)
		{
			// No data left
			break;
		}

		LPCTSTR pszArg = _tcspbrk(pszCurrent, m_szDelimeters);
		if (! pszArg)
		{
			// No delimeters found
			break;
		}

		LPCTSTR pszInArg;
		do
		{
			pszArg = _tcsinc(pszArg);
			pszInArg = _tcspbrk(pszArg, m_szDelimeters);
		}
		while (pszInArg == pszArg);

		// Key:"arg"
		if (_tcslen(pszArg) == 0)
		{
			// String ends with delimeter
			break;
		}

		LPCTSTR pszVal = _tcspbrk(pszArg, m_szValueSep);
		if (pszVal == NULL)
		{
			// Key ends	command	line
			CString strKey(pszArg);
			if(! m_fCaseSensitive)
			{
				strKey.MakeLower();
			}
			m_mapKeys.SetAt(strKey, _T(""));
			break;
		}
		else if (pszVal[0] == _T(' ') || _tcslen(pszVal) == 1)
		{
			// Key with no value or cmdline ends with /Key:
			CString strKey(pszArg, (int)(pszVal - pszArg));
			if(! strKey.IsEmpty())
			{
				// Prevent /: case
				if(!m_fCaseSensitive)
				{
					strKey.MakeLower();
				}
				m_mapKeys.SetAt(strKey, _T(""));
			}
			pszCurrent = _tcsinc(pszVal);
			continue;
		}
		else
		{
			// Key with value
			CString strKey(pszArg, (int)(pszVal - pszArg));
			if (! m_fCaseSensitive)
			{
				strKey.MakeLower();
			}

			pszVal = _tcsinc(pszVal);
			// "arg"
			LPCTSTR pszQuote = _tcspbrk(pszVal, m_szQuotes), pszEndQuote(NULL);

			if (pszQuote == pszVal)
			{
				// Quoted String
				pszQuote = _tcsinc(pszVal);
				pszEndQuote = _tcspbrk(pszQuote, m_szQuotes);
			}
			else
			{
				pszQuote = pszVal;
				pszEndQuote = _tcschr(pszQuote, _T('\x20'));
			}

			if (pszEndQuote == NULL)
			{
				// No end quotes or terminating space, take rest of string
				CString strValue(pszQuote);
				if(! strKey.IsEmpty())
				{
					// Prevent /:val case
					m_mapKeys.SetAt(strKey, strValue);
				}
				break;
			}
			else
			{
				// End quote or	space present
				if(! strKey.IsEmpty())
				{
					// Prevent /:"val" case
					CString strValue(pszQuote, (int)(pszEndQuote - pszQuote));
					m_mapKeys.SetAt(strKey, strValue);
				}
				pszCurrent = _tcsinc(pszEndQuote);
				continue;
			}
		}
	}
}

bool CArgsParser::GetIntValue(LPCTSTR pszKeyName, int& nDest, int nRadix)
{
	ASSERT(AfxIsValidString(pszKeyName));

	if (HasKey(pszKeyName) && HasValue(pszKeyName))
	{
		LPTSTR pszStop = 0;
		nDest = _tcstol(GetStringValue(pszKeyName), &pszStop, nRadix);
		return (*pszStop == 0);
	}
	else {
		return (false);
	}
}

int CArgsParser::GetIntValue(LPCTSTR pszKeyName, int nRadix)
{
	ASSERT(AfxIsValidString(pszKeyName));

	int nValue;
	if (!GetIntValue(pszKeyName, nValue, nRadix))
	{
		AfxThrowInvalidArgException();
	}
	return (nValue);
}

bool CArgsParser::GetUIntValue(LPCTSTR pszKeyName, UINT& uDest, int nRadix)
{
	ASSERT(AfxIsValidString(pszKeyName));

	if (HasKey(pszKeyName) && HasValue(pszKeyName))
	{
		LPTSTR pszStop = 0;
		uDest = _tcstoul(GetStringValue(pszKeyName), &pszStop, nRadix);
		return (*pszStop == 0);
	}
	else {
		return (false);
	}
}

UINT CArgsParser::GetUIntValue(LPCTSTR pszKeyName, int nRadix)
{
	ASSERT(AfxIsValidString(pszKeyName));

	UINT uValue;
	if (!GetUIntValue(pszKeyName, uValue, nRadix))
	{
		AfxThrowInvalidArgException();
	}
	return (uValue);
}

#if !defined(ARGS_PARSER_NO_TIME)

bool CArgsParser::GetTimeValue(LPCTSTR pszKeyName, CTime& timeDest, DWORD fdwFlags, LCID Locale)
{
	ASSERT(AfxIsValidString(pszKeyName));

	COleDateTime odtTemp;
	if (GetOleTimeValue(pszKeyName, odtTemp, fdwFlags, Locale))
	{
		SYSTEMTIME st = { 0 };
		odtTemp.GetAsSystemTime(st);
		timeDest = CTime(st);
		return (true);
	}
	else {
		return (false);
	}
}

CTime CArgsParser::GetTimeValue(LPCTSTR pszKeyName, DWORD fdwFlags, LCID Locale)
{
	ASSERT(AfxIsValidString(pszKeyName));

	CTime timeValue;
	if (!GetTimeValue(pszKeyName, timeValue, fdwFlags, Locale))
	{
		AfxThrowInvalidArgException();
	}
	return (timeValue);
}

bool CArgsParser::GetOleTimeValue(LPCTSTR pszKeyName, COleDateTime& odtDest, DWORD fdwFlags, LCID Locale)
{
	ASSERT(AfxIsValidString(pszKeyName));

	if (HasKey(pszKeyName) && HasValue(pszKeyName))
	{
		odtDest.ParseDateTime(GetStringValue(pszKeyName), fdwFlags, Locale);
		return (odtDest.GetStatus() == COleDateTime::valid);
	}
	else {
		return (false);
	}
}

COleDateTime CArgsParser::GetOleTimeValue(LPCTSTR pszKeyName, DWORD fdwFlags, LCID Locale)
{
	ASSERT(AfxIsValidString(pszKeyName));

	COleDateTime odtValue;
	if (!GetOleTimeValue(pszKeyName, odtValue, fdwFlags, Locale))
	{
		AfxThrowInvalidArgException();
	}
	return (odtValue);
}

#endif	// ARGS_PARSER_NO_TIME

// attributes

const TCHAR CArgsParser::m_szDelimeters[] = _T("-/");
const TCHAR CArgsParser::m_szQuotes[] = _T("\"\'");
// space MUST be in set, but is not a name/value delimiter
const TCHAR CArgsParser::m_szValueSep[] = _T("\x20:=");

// diagnostic services

#if defined(_DEBUG)

void CArgsParser::AssertValid(void) const
{
	// first perform inherited validity check...
	CObject::AssertValid();

	// ...and then verify own state as well
	ASSERT_VALID(&m_mapKeys);
}

void CArgsParser::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CObject::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_strArguments = " << m_strArguments << "\n";
		dumpCtx << "m_fCaseSensitive = " << m_fCaseSensitive << "\n";
		m_mapKeys.Dump(dumpCtx);
		dumpCtx << "m_szDelimeters = " << m_szDelimeters << "\n";
		dumpCtx << "m_szQuotes = " << m_szQuotes << "\n";
		dumpCtx << "m_szValueSep = " << m_szValueSep;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
