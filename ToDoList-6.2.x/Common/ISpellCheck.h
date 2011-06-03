// Copyright (C) 2003-2005 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and license information
// - adjusted #include's paths
// - reformatted using Artistic Style 2.02 with the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --style=allman
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// ISpellChecker.h: interface for the ISpellChecker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ISPELLCHECK_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
#define AFX_ISPELLCHECK_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4189)
#pragma warning(disable: 4127)

#include <windows.h>

// function to be exported from dll to create instance of interface
#ifdef _EXPORTING // declare this in project settings for dll _only_
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

#define ISPELLCHECK_VERSION 0x0000

class ISpellChecker;

typedef ISpellChecker* (*PFNCREATE)(const TCHAR*, const TCHAR*); // function prototype
extern "C" DLL_DECLSPEC ISpellChecker* CreateSpellCheckerInterface(const TCHAR* szAffPath, const TCHAR* szDicPath); // single exported function

typedef int (*PFNGETVERSION)(); // function prototype
extern "C" DLL_DECLSPEC int GetInterfaceVersion();

#pragma warning(disable:4505)

// helper method
static ISpellChecker* CreateSpellCheckerInterface(const TCHAR* szDllPath, const TCHAR* szAffPath, const TCHAR* szDicPath)
{
	ISpellChecker* pInterface = NULL;
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATE pCreate = (PFNCREATE)GetProcAddress(hDll, "CreateSpellCheckerInterface");

		if (pCreate)
		{
			// check version
			PFNGETVERSION pVersion = (PFNGETVERSION)GetProcAddress(hDll, "GetInterfaceVersion");

			if (!ISPELLCHECK_VERSION || (pVersion && pVersion() >= ISPELLCHECK_VERSION))
			{
				pInterface = pCreate(szAffPath, szDicPath);
			}
		}
	}

	return pInterface;
}

static BOOL IsSpellCheckDll(const TCHAR* szDllPath)
{
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATE pCreate = (PFNCREATE)GetProcAddress(hDll, "CreateSpellCheckerInterface");
		FreeLibrary(hDll);

		return (NULL != pCreate);
	}

	return FALSE;
}

class ISpellChecker
{
public:
	virtual void Release() = 0; // releases the interface

	//
	virtual bool CheckSpelling(const char* szWord) = 0;
	virtual bool CheckSpelling(const char* szWord, char** & pSuggestions, int& nNumSuggestions) = 0;

	// frees a previously returned buffer and sets the ptr to NULL
	virtual void FreeSuggestions(char** & pSuggestions) = 0;
};

static void ReleaseSpellCheckerInterface(ISpellChecker*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

class ISpellCheck
{
public:
	virtual const char* GetFirstWord() const = 0;
	virtual const char* GetNextWord() const = 0;
	virtual const char* GetCurrentWord() const = 0;

	virtual void SelectCurrentWord() = 0;
	virtual void ReplaceCurrentWord(const char* szWord) = 0;

	virtual void ClearSelection() = 0;
};

#endif // !defined(AFX_ISpellCheck_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
