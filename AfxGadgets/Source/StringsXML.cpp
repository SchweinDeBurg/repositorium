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

// StringsXML.cpp - implementation of the CStringsXML class

#include "stdafx.h"
#include "StringsXML.h"
#include "PugXMLplus.h"
#include "StringConv.h"

#if !defined(_INC_SHLWAPI)
#pragma message(__FILE__ " : put <shlwapi.h> in your PCH to speed up compilation")
#include <shlwapi.h>
#endif	// _INC_SHLWAPI

// PSDK header for Win95/98/NT4 builds
#if (_WIN32_WINDOWS < 0x0490) && !defined(_DBGHELP_)
#pragma message(__FILE__ " : put <dbghelp.h> in your PCH to speed up compilation")
#include <dbghelp.h>
#endif	// _WIN32_WINDOWS && _DBGHELP_

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// object model
IMPLEMENT_DYNAMIC(CStringsXML, CObject)

CStringsXML::CStringsXML(LPCTSTR pszStringsName):
m_strStringsName(pszStringsName)
{
	ASSERT(!m_strStringsName.IsEmpty());
}

CStringsXML::~CStringsXML(void)
{
}

int CStringsXML::PutStringsToCache(void)
{
	// create the parser...
	std::auto_ptr<CPugXmlParser> pParser(new CPugXmlParser());

	// ...and try to parse the source XML file
	if (ParseFile(pParser.get()))
	{
		// obtain needed XML nodes
		CPugXmlBranch branchRoot = pParser->GetRoot();
		ASSERT(!branchRoot.IsNull());
		CPugXmlBranch branchStrings = branchRoot.FindByPath(_T("./Strings"));
		ASSERT(!branchStrings.IsNull());

		// iterate through the strings
		int cNumStrings = branchStrings.GetChildrenCount();
		for (int i = 0; i < cNumStrings; ++i)
		{
			// obtain string's node
			CPugXmlBranch branchString = branchStrings.GetChildAt(i);
			ASSERT(!branchString.IsNull());

			// if there is a "known" node...
			if (::StrCmp(branchString.GetName(), _T("String")) == 0)
			{
				// ...then cache it
				DWORD dwID = GetAttribute_DWORD(branchString, _T("ID"));
				LPCTSTR pszText = branchString.GetAttribute(_T("Text"));
				m_mapCache.SetAt(dwID, pszText);
			}
		}
	}

	return (m_mapCache.GetCount());
}

void CStringsXML::CleanupCache(void)
{
	m_mapCache.RemoveAll();
}

CString CStringsXML::operator [](DWORD dwID)
{
	CString strText;
	TCHAR szTemp[32];

	// if desired string is already in the cache...
	if (m_mapCache.Lookup(dwID, strText))
	{
		// ...then quickly return it
		return (strText);
	}

	// otherwise create the parser...
	std::auto_ptr<CPugXmlParser> pParser(new CPugXmlParser());

	// ...and try to parse the source XML file
	if (ParseFile(pParser.get()))
	{
		// obtain needed XML nodes
		CPugXmlBranch branchRoot = pParser->GetRoot();
		ASSERT(!branchRoot.IsNull());
		CPugXmlBranch branchStrings = branchRoot.FindByPath(_T("./Strings"));
		ASSERT(!branchStrings.IsNull());

		// search the desired string
		_ultot(dwID, szTemp, 10);
		CPugXmlBranch branchString = branchStrings.FindFirstElemAttr(_T("String"), _T("ID"), szTemp);
		if (!branchString.IsNull())
		{
			// gotcha!!
			strText = branchString.GetAttribute(_T("Text"));

			// put found string into the cache
			m_mapCache.SetAt(dwID, strText);
		}
	}

	// return the resulting string
	return (strText);
}

CString CStringsXML::Format(DWORD dwID, ...)
{
	va_list argList;
	CString strText;

	va_start(argList, dwID);
	strText.FormatV((*this)[dwID], argList);
	va_end(argList);
	return (strText);
}

void CStringsXML::GetXMLpath(CString& strDest)
{
	ASSERT(*(AfxGetApp()->m_pszRegistryKey) != 0);

	LPTSTR pszPath = strDest.GetBuffer(_MAX_PATH);
	::SHGetSpecialFolderPath(NULL, pszPath, CSIDL_APPDATA, TRUE);
	::PathAddBackslash(pszPath);
	::lstrcat(pszPath, AfxGetApp()->m_pszRegistryKey);
	::PathAddBackslash(pszPath);
	::lstrcat(pszPath, AfxGetAppName());
	::PathAddBackslash(pszPath);
	strDest.ReleaseBuffer();
}

BOOL CStringsXML::ParseFile(class CPugXmlParser* pParser)
{
	using CFile::modeCreate;
	using CFile::modeWrite;
	using CFile::shareExclusive;

	CString strFileXML;

	ASSERT(pParser != NULL);

	// get the path and ensure that it exists
	GetXMLpath(strFileXML);
	::PathAddBackslash(strFileXML.GetBuffer(_MAX_PATH));
	strFileXML.ReleaseBuffer();
	if (!::PathFileExists(strFileXML))
	{
		TRACE(_T("Warning: folder %s doesn\'t exists, trying to create.\n"), strFileXML);
#if (_WIN32_WINDOWS < 0x0490)
		if (!::MakeSureDirectoryPathExists(_T2A(strFileXML)))
#else
		if (::SHCreateDirectoryEx(NULL, strFileXML, NULL) != ERROR_SUCCESS)
#endif	// _WIN32_WINDOWS
		{
			TRACE(_T("Error: unable to create folder %s\n"), strFileXML);
			return (FALSE);
		}
	}

	// construct full name of the XML-file...
	strFileXML += m_strStringsName;
	strFileXML += _T(".xml");

	// ...and if this file doesn't exists...
	if (!::PathFileExists(strFileXML))
	{
		// ...then try to create it from the corresonding resource
		TRACE(_T("Warning: file %s doesn\'t exists, trying to create.\n"), strFileXML);
		static const TCHAR szResType[] = _T("STRINGS_XML");
		HINSTANCE hInstRes = AfxFindResourceHandle(m_strStringsName, szResType);
		if (hInstRes != NULL)
		{
			HRSRC hResInfo = ::FindResource(hInstRes, m_strStringsName, szResType);
			ASSERT(hResInfo != NULL);
			HGLOBAL hResData = ::LoadResource(hInstRes, hResInfo);
			ASSERT(hResData != NULL);
			void* pvResData = ::LockResource(hResData);
			ASSERT(pvResData != NULL);
			DWORD cbSize = ::SizeofResource(hInstRes, hResInfo);
			try
			{
				CFile fileXML(strFileXML, modeCreate | modeWrite | shareExclusive);
				fileXML.Write(pvResData, cbSize);
				fileXML.Flush();
				fileXML.Close();
			}
			catch (CFileException* pXcpt)
			{
				// oops!
				::UnlockResource(hResData);
				::FreeResource(hResData);
				pXcpt->ReportError(MB_ICONSTOP | MB_OK);
				pXcpt->Delete();
				return (FALSE);
			}
			::UnlockResource(hResData);
			::FreeResource(hResData);
			TRACE(_T("File %s was successfully created.\n"), strFileXML);
		}
		else {
			TRACE(_T("Error: resource \"%s\" of type \"%s\" not found.\n"), m_strStringsName, szResType);
			return (FALSE);
		}
	}

	return (pParser->ParseFile(strFileXML));
}

#if defined(_DEBUG)

void CStringsXML::AssertValid(void) const
{
	// first perform inherited validity check...
	CObject::AssertValid();

	// ...and then verify our own state as well
}

void CStringsXML::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CObject::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_strStringsName = " << m_strStringsName << "\n";
		dumpCtx.SetDepth(1);
		dumpCtx << "m_mapCache = " << m_mapCache;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// import libraries
#pragma comment(lib, "shlwapi.lib")

// import library for Win95/98/NT4 builds
#if (_WIN32_WINDOWS < 0x0490)
#pragma comment(lib, "dbghelp.lib")
#endif	// _WIN32_WINDOWS

// end of file
