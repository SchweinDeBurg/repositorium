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

// MenuXML.cpp - implementation of the CMenuXML class

#include "stdafx.h"
#include "MenuXML.h"
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

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #797: a class-qualified name is not allowed
#pragma warning(disable: 797)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
#endif	// __INTEL_COMPILER

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

// extended menu templates (new in Win95)

#pragma pack(push, 1)

typedef struct
{
	WORD wVersion;
	WORD wOffset;
	DWORD dwHelpId;
}
MENUEX_TEMPLATE_HEADER;

typedef struct
{
	DWORD dwType;
	DWORD dwState;
	DWORD menuID;
	WORD bResInfo;
}
MENUEX_TEMPLATE_ITEM;

#pragma pack(pop)

// object model
IMPLEMENT_DYNCREATE(CMenuXML, CMenu)

CMenuXML::CMenuXML(void)
{
}

CMenuXML::~CMenuXML(void)
{
}

const MENUTEMPLATE* CMenuXML::CreateMenuExTemplate(LPCTSTR pszFileXML)
{
	CMemFile fileMem;
	MENUEX_TEMPLATE_HEADER menuExHeader;

	ASSERT(AfxIsValidString(pszFileXML));
	ASSERT(::PathFileExists(pszFileXML));

	std::auto_ptr<CPugXmlParser> pParser(new CPugXmlParser());

	// try to parse source XML file
	if (pParser->ParseFile(pszFileXML))
	{
		// let's go ahead and fuck this iceberg!
		fileMem.Attach(NULL, 0, 1024);

		// obtain needed XML nodes
		CPugXmlBranch branchRoot = pParser->GetRoot();
		ASSERT(!branchRoot.IsNull());
		CPugXmlBranch branchMenu = branchRoot.FindByPath(_T("./Menu"));
		ASSERT(!branchMenu.IsNull());

		// build and write menu header
		menuExHeader.wVersion = 1;
		menuExHeader.wOffset = sizeof(menuExHeader.dwHelpId);
		menuExHeader.dwHelpId = GetAttribute_DWORD(branchMenu, _T("HelpID"));
		fileMem.Write(&menuExHeader, sizeof(menuExHeader));

		// recursively traverse XML-tree
		RecurseMenuTree(fileMem, branchMenu);

		// return memory address of the created template
		return (reinterpret_cast<DLGTEMPLATE*>(fileMem.Detach()));
	}
	else {
		// error parsing source XML file
		return (NULL);
	}
}

BOOL CMenuXML::CreateMenuXML(LPCTSTR pszMenuName)
{
	using CFile::modeCreate;
	using CFile::modeWrite;
	using CFile::shareExclusive;

	CString strFileXML;

	// precondition
	ASSERT(AfxIsValidString(pszMenuName));
	ASSERT(*pszMenuName != 0);

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
	strFileXML += pszMenuName;
	strFileXML += _T(".xml");

	// ...and if this file doesn't exists...
	if (!::PathFileExists(strFileXML))
	{
		// ...then try to create it from the corresonding resource
		TRACE(_T("Warning: file %s doesn\'t exists, trying to create.\n"), strFileXML);
		static const TCHAR szResType[] = _T("MENU_XML");
		HINSTANCE hInstRes = AfxFindResourceHandle(pszMenuName, szResType);
		if (hInstRes != NULL)
		{
			HRSRC hResInfo = ::FindResource(hInstRes, pszMenuName, szResType);
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
			TRACE(_T("Error: resource \"%s\" of type \"%s\" not found.\n"), pszMenuName, szResType);
			return (FALSE);
		}
	}

	// try to build menu template
	const MENUTEMPLATE* pMenuTemplate = CreateMenuExTemplate(strFileXML);
	if (pMenuTemplate != NULL)
	{
		BOOL fSuccess = LoadMenuIndirect(pMenuTemplate);
		free(const_cast<MENUTEMPLATE*>(pMenuTemplate));
		return (fSuccess);
	}
	else {
		TRACE(_T("Error: unable to create menu template -\n"));
		TRACE(_T("\tprobably %s is not a valid XML file.\n"), strFileXML);
		return (FALSE);
	}
}

void CMenuXML::GetXMLpath(CString& strDest)
{
	ASSERT(AfxIsValidString(AfxGetApp()->m_pszRegistryKey));
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

void CMenuXML::RecurseMenuTree(CMemFile& fileMem, CPugXmlBranch& branchMenu)
{
	MENUEX_TEMPLATE_ITEM menuExItem;
	UINT cbRemains;

	BYTE abAligner[4] = { 0 };

	// interate through the menu items
	int iLast = branchMenu.GetChildrenCount() - 1;
	for (int i = 0; i <= iLast; ++i)
	{
		// obtain item's XML node
		CPugXmlBranch branchItem = branchMenu.GetChildAt(i);
		ASSERT(!branchItem.IsNull());

		// determine whether the item is a popup menu
		BOOL fPopup = ::lstrcmpi(branchItem.GetName(), _T("Popup")) == 0;

		// item's type
		LPCTSTR pszTypeStr = branchItem.GetAttribute(_T("Type"), _T("String"));
		menuExItem.dwType = ParseMenuItemType(pszTypeStr);

		// initial item's state
		LPCTSTR pszStateStr = branchItem.GetAttribute(_T("State"), _T("Enabled"));
		menuExItem.dwState = ParseMenuItemState(pszStateStr);

		// item's ID
		menuExItem.menuID = GetAttribute_DWORD(branchItem, _T("ID"));

		// item's resource info
		menuExItem.bResInfo = 0;
		menuExItem.bResInfo |= i == iLast ? MF_END : 0;
		menuExItem.bResInfo |= fPopup ? 0x01 : 0;

		// write MENUEX_TEMPLATE_ITEM into the memory
		fileMem.Write(&menuExItem, sizeof(menuExItem));

		// item's text
		_T2W pwszText(branchItem.GetAttribute(_T("Text")));
		fileMem.Write(pwszText, (::lstrlenW(pwszText) + 1) * sizeof(WCHAR));

		// align to the DWORD boundary
		ASSERT(sizeof(DWORD) == 4);
#if (_MFC_VER < 0x0700)
		if ((cbRemains = (4 - fileMem.GetLength() % 4) & 3) > 0)
#else
		if ((cbRemains = (4 - static_cast<UINT>(fileMem.GetLength() % 4)) & 3) > 0)
#endif
		{
			fileMem.Write(abAligner, cbRemains);
		}

		// if there's a popup menu...
		if (fPopup)
		{
			// ...first obtain and write its help identifier...
			DWORD dwHelpId = GetAttribute_DWORD(branchItem, _T("HelpID"));
			fileMem.Write(&dwHelpId, sizeof(dwHelpId));

			// ...and then recurse its items
			RecurseMenuTree(fileMem, branchItem);
		}
	}
}

DWORD CMenuXML::ParseMenuFlags(MENU_FLAG amfDict[], LPCTSTR pszFlagsStr)
{
	ASSERT(AfxIsValidString(pszFlagsStr));

	DWORD fdwFlags = 0;
	LPTSTR pszTemp = ::StrDup(pszFlagsStr);
	static const TCHAR szSeps[] = _T("\x20\t,;");
	LPTSTR pszCurFlag = _tcstok(pszTemp, szSeps);
	while (pszCurFlag != NULL)
	{
		for (int i = 0; amfDict[i].pszName != NULL; ++i)
		{
			if (::lstrcmpi(pszCurFlag, amfDict[i].pszName) == 0)
			{
				fdwFlags |= amfDict[i].fdwValue;
			}
		}
		pszCurFlag = _tcstok(NULL, szSeps);
	}
	::LocalFree(pszTemp);
	return (fdwFlags);
}

DWORD CMenuXML::ParseMenuItemType(LPCTSTR pszTypeStr)
{
	MENU_FLAG amfDict[] =
	{
		{ _T("String"), MFT_STRING },
		{ _T("Bitmap"), MFT_BITMAP },
		{ _T("MenuBarBreak"), MFT_MENUBARBREAK },
		{ _T("MenuBreak"), MFT_MENUBREAK },
		{ _T("OwnerDraw"), MFT_OWNERDRAW },
		{ _T("RadioCheck"), MFT_RADIOCHECK },
		{ _T("Separator"), MFT_SEPARATOR },
		{ _T("RightOrder"), MFT_RIGHTORDER },
		{ _T("RightJustify"), MFT_RIGHTJUSTIFY },
		{ NULL, 0 }
	};
	return (ParseMenuFlags(amfDict, pszTypeStr));
}

DWORD CMenuXML::ParseMenuItemState(LPCTSTR pszStateStr)
{
	MENU_FLAG amfDict[] =
	{
		{ _T("Grayed"), MFS_GRAYED },
		{ _T("Disabled"), MFS_DISABLED },
		{ _T("Checked"), MFS_CHECKED },
		{ _T("Hilite"), MFS_HILITE },
		{ _T("Enabled"), MFS_ENABLED },
		{ _T("Unchecked"), MFS_UNCHECKED },
		{ _T("Unhilite"), MFS_UNHILITE },
		{ _T("Default"), MFS_DEFAULT },
		{ NULL, 0 }
	};
	return (ParseMenuFlags(amfDict, pszStateStr));
}

#if defined(_DEBUG)

void CMenuXML::AssertValid(void) const
{
	// first perform inherited validity check...
	CMenu::AssertValid();

	// ...and then verify our own state as well
}

void CMenuXML::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CMenu::Dump(dumpCtx);

		// ...and then dump own unique members
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
