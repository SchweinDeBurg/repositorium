// AfxGadgets library.
// Copyright (c) 2004-2008 by Elijah Zarezky,
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

// HotKeysXML.cpp - implementation of the CHotKeysXML class

#include "stdafx.h"
#include "HotKeysXML.h"
#include "PugXMLplus.h"
#include "StringConv.h"

#if !defined(_INC_SHLWAPI)
#pragma message(__FILE__ " : put <shlwapi.h> in your PCH to speed up compilation")
#include <shlwapi.h>
#endif	// _INC_SHLWAPI

#if !defined(_INC_COMMCTRL)
#pragma message(__FILE__ " : put <commctrl.h> in your PCH to speed up compilation")
#include <commctrl.h>
#endif	// _INC_COMMCTRL

// PSDK header for Win95/98/NT4 builds
#if (_WIN32_WINDOWS < 0x0490) && !defined(_DBGHELP_)
#pragma message(__FILE__ " : put <dbghelp.h> in your PCH to speed up compilation")
#include <dbghelp.h>
#endif	// _WIN32_WINDOWS && _DBGHELP_

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
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

// object model
IMPLEMENT_DYNAMIC(CHotKeysXML, CObject)

CHotKeysXML::CHotKeysXML(void):
m_hAccTable(NULL)
{
}

CHotKeysXML::~CHotKeysXML(void)
{
	if (m_hAccTable != NULL)
	{
		DestroyTable();
	}
}

BOOL CHotKeysXML::CreateTable(LPCTSTR pszTableName)
{
	using CFile::modeCreate;
	using CFile::modeWrite;
	using CFile::shareExclusive;

	CString strFileXML;
	ACCEL_ARRAY arrAccel;

	ASSERT(m_hAccTable == NULL);

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
	strFileXML += pszTableName;
	strFileXML += _T(".xml");

	// ...and if this file doesn't exists...
	if (!::PathFileExists(strFileXML))
	{
		// ...then try to create it from the corresonding resource
		TRACE(_T("Warning: file %s doesn\'t exists, trying to create.\n"), strFileXML);
		static const TCHAR szResType[] = _T("HOTKEYS_XML");
		HINSTANCE hInstRes = AfxFindResourceHandle(pszTableName, szResType);
		if (hInstRes != NULL)
		{
			HRSRC hResInfo = ::FindResource(hInstRes, pszTableName, szResType);
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
			TRACE(_T("Error: resource \"%s\" of type \"%s\" not found.\n"), pszTableName, szResType);
			return (FALSE);
		}
	}

	// try to create accelerator data array...
	if (CreateAccelArray(strFileXML, arrAccel))
	{
		// ...and accelerator table itself
		m_hAccTable = ::CreateAcceleratorTable(arrAccel.GetData(), arrAccel.GetSize());
	}
	return (m_hAccTable != NULL);
}

BOOL CHotKeysXML::DestroyTable(void)
{
	ASSERT(m_hAccTable != NULL);

	if (::DestroyAcceleratorTable(m_hAccTable))
	{
		m_hAccTable = NULL;
		m_mapNames.RemoveAll();
		return (TRUE);
	}
	else {
		TRACE(_T("Warning: failed to destroy accelerator table 0x%08X.\n"), m_hAccTable);
		return (FALSE);
	}
}

void CHotKeysXML::AttachToFrame(CFrameWnd* pFrameWnd, BOOL fShowInMenu)
{
	CMenu menuFrame;
	CString strMenuText;
	WORD wID;
	CString strKeyText;

	ASSERT(m_hAccTable != NULL);
	ASSERT_VALID(pFrameWnd);

	pFrameWnd->m_hAccelTable = m_hAccTable;

	if (fShowInMenu)
	{
		menuFrame.Attach(::GetMenu(pFrameWnd->GetSafeHwnd()));
		POSITION pos = m_mapNames.GetStartPosition();
		while (pos != NULL)
		{
			m_mapNames.GetNextAssoc(pos, wID, strKeyText);
			menuFrame.GetMenuString(wID, strMenuText, MF_BYCOMMAND);
			strMenuText += _T('\t');
			strMenuText += strKeyText;
			menuFrame.ModifyMenu(wID, MF_BYCOMMAND, wID, strMenuText);
		}
		menuFrame.Detach();
	}
}

void CHotKeysXML::DetachFromFrame(CFrameWnd* pFrameWnd)
{
	CMenu menuFrame;
	CString strMenuText;
	WORD wID;
	CString strKeyText;

	ASSERT_VALID(pFrameWnd);
	ASSERT(pFrameWnd->m_hAccelTable == m_hAccTable);

	menuFrame.Attach(::GetMenu(pFrameWnd->GetSafeHwnd()));
	POSITION pos = m_mapNames.GetStartPosition();
	while (pos != NULL)
	{
		m_mapNames.GetNextAssoc(pos, wID, strKeyText);
		menuFrame.GetMenuString(wID, strMenuText, MF_BYCOMMAND);
		int cchLeft = strMenuText.Find(_T('\t'));
		if (cchLeft > 0)
		{
			menuFrame.ModifyMenu(wID, MF_BYCOMMAND, wID, strMenuText.Left(cchLeft));
		}
	}
	menuFrame.Detach();

	pFrameWnd->m_hAccelTable = NULL;
}

void CHotKeysXML::GetXMLpath(CString& strDest)
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

BOOL CHotKeysXML::CreateAccelArray(LPCTSTR pszFileXML, ACCEL_ARRAY& arrAccel)
{
	ACCEL accData;

	ASSERT(AfxIsValidString(pszFileXML));
	ASSERT(::PathFileExists(pszFileXML));

	std::auto_ptr<CPugXmlParser> pParser(new CPugXmlParser());

	// try to parse source XML file
	if (pParser->ParseFile(pszFileXML))
	{
		// obtain needed XML nodes
		CPugXmlBranch branchRoot = pParser->GetRoot();
		ASSERT(!branchRoot.IsNull());
		CPugXmlBranch branchHotKeys = branchRoot.FindByPath(_T("./HotKeys"));
		ASSERT(!branchHotKeys.IsNull());

		// iterate though the keys
		int cHotKeys = branchHotKeys.GetChildrenCount();
		for (int i = 0; i < cHotKeys; ++i)
		{
			CPugXmlBranch branchHotKey = branchHotKeys.GetChildAt(i);
			ASSERT(!branchHotKey.IsNull());
			LPCTSTR pszText = branchHotKey.GetAttribute(_T("Text"));
			DWORD fdwCode = GetAttribute_DWORD(branchHotKey, _T("Code"));
			if (fdwCode != NULL)
			{
				// assume that the low-order word contains VK_* value...
				WORD wVKeyCode = LOWORD(fdwCode);
				ASSERT(wVKeyCode != 0);

				// ...and the high-order word contains HOTKEYF_* flags
				WORD wModifiers = HIWORD(fdwCode);

				// accelerator flags
				accData.fVirt = FVIRTKEY;
				accData.fVirt |= (wModifiers & HOTKEYF_SHIFT) != 0 ? FSHIFT : 0;
				accData.fVirt |= (wModifiers & HOTKEYF_CONTROL) != 0 ? FCONTROL : 0;
				accData.fVirt |= (wModifiers & HOTKEYF_ALT) != 0 ? FALT : 0;

				// virtual key code
				accData.key = wVKeyCode;
			}
			else {
				// we need to parse hot-key text :-(
				accData.fVirt = FVIRTKEY;
				ParseHotKeyText(pszText, accData);
				ASSERT(accData.key != 0);
			}

			// accelerator command
			accData.cmd = GetAttribute_WORD(branchHotKey, _T("ID"));

			// add prepared ACCEL to the destination array
			arrAccel.Add(accData);

			// remember hot-key text for later use
			m_mapNames.SetAt(accData.cmd, pszText);
		}

		// succeeded
		return (TRUE);
	}
	else {
		// error parsing source XML file
		return (FALSE);
	}
}

WORD CHotKeysXML::GetModifierFlag(LPCTSTR pszText)
{
	struct MODIFIER { LPCTSTR pszName; WORD wValue; };

	ASSERT(AfxIsValidString(pszText));

	static MODIFIER amDict[] =
	{
		{ _T("Shift"), FSHIFT },
		{ _T("Ctrl"), FCONTROL },
		{ _T("Control"), FCONTROL },
		{ _T("Alt"), FALT },
		{ NULL, 0 }
	};

	// iterate through the dictionary...
	for (int i = 0; amDict[i].pszName != NULL; ++i)
	{
		// ...and compare given text with the known key names
		if (::lstrcmpi(pszText, amDict[i].pszName) == 0)
		{
			// gotcha!!
			return (amDict[i].wValue);
		}
	}

	// doesn't found
	return (0);
}

WORD CHotKeysXML::GetVKeyCode(LPCTSTR pszText)
{
	struct VIRT_KEY { LPCTSTR pszName; WORD wVKeyCode; };

	ASSERT(AfxIsValidString(pszText));

	static VIRT_KEY avkDict[] =
	{
		{ _T("Backspace"), VK_BACK },
		{ _T("Tab"), VK_TAB },
		{ _T("Enter"), VK_RETURN },
		{ _T("Esc"), VK_ESCAPE },
		{ _T("Space"), VK_SPACE },
		{ _T("PageUp"), VK_PRIOR },
		{ _T("PageDown"), VK_NEXT },
		{ _T("End"), VK_END },
		{ _T("Home"), VK_HOME },
		{ _T("Left"), VK_LEFT },
		{ _T("Up"), VK_UP },
		{ _T("Right"), VK_RIGHT },
		{ _T("Down"), VK_DOWN },
		{ _T("Ins"), VK_INSERT },
		{ _T("Del"), VK_DELETE },
		{ _T("Gray*"), VK_MULTIPLY },
		{ _T("Gray+"), VK_ADD },
		{ _T("Gray-"), VK_SUBTRACT },
		{ _T("Gray/"), VK_DIVIDE },
		{ _T("F1"), VK_F1 },
		{ _T("F2"), VK_F2 },
		{ _T("F3"), VK_F3 },
		{ _T("F4"), VK_F4 },
		{ _T("F5"), VK_F5 },
		{ _T("F6"), VK_F6 },
		{ _T("F7"), VK_F7 },
		{ _T("F8"), VK_F8 },
		{ _T("F9"), VK_F9 },
		{ _T("F10"), VK_F10 },
		{ _T("F11"), VK_F11 },
		{ _T("F12"), VK_F12 },
		{ NULL, 0 }
	};

	if (::lstrlen(pszText) > 1)
	{
		// iterate through the dictionary...
		for (int i = 0; avkDict[i].pszName != NULL; ++i)
		{
			// ...and compare given text with the known key names
			if (::lstrcmpi(pszText, avkDict[i].pszName) == 0)
			{
				// gotcha!!
				return (avkDict[i].wVKeyCode);
			}
		}
	}
	else {
		// assume '0'..'9' or 'A'..'Z'
		return (LOWORD(::CharUpper(reinterpret_cast<LPTSTR>(MAKELONG(pszText[0], 0)))));
	}

	// doesn't found
	return (0);
}

void CHotKeysXML::ParseHotKeyText(LPCTSTR pszText, ACCEL& accData)
{
	WORD wModifier;

	// precondiion
	ASSERT(AfxIsValidString(pszText));
	ASSERT(*pszText != 0);

	LPTSTR pszTemp = ::StrDup(pszText);
	static const TCHAR szSeps[] = _T("+\x20,");
	LPTSTR pszKey = _tcstok(pszTemp, szSeps);
	while (pszKey != NULL)
	{
		if ((wModifier = GetModifierFlag(pszKey)) != 0)
		{
			accData.fVirt |= wModifier;
		}
		else {
			accData.key = GetVKeyCode(pszKey);
		}
		pszKey = _tcstok(NULL, szSeps);
	}
	::LocalFree(pszTemp);
}

#if defined(_DEBUG)

void CHotKeysXML::AssertValid(void) const
{
	// first perform inherited validity check...
	CObject::AssertValid();

	// ...and then verify our own state as well
}

void CHotKeysXML::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CObject::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_hAccTable = " << m_hAccTable << "\n";
		dumpCtx << "m_mapNames = " << m_mapNames;
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
