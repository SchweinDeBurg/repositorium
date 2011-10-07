// Copyright (C) 2003-2011 AbstractSpoon Software.
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
// - restyled using ProFactor StyleManager v1.17:
//      * removed unnecessary spaces and empty lines
//      * wrapped extremely long lines
//      * reformatted all the ctors to be more readable
//      * eliminated dead commented code
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// IContentControl.h: IContentControl interface.
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ICONTENTCONTROL_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
#define AFX_ICONTENTCONTROL_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

// function to be exported from dll to create instance of interface
#ifdef _EXPORTING // declare this in project settings for dll _only_
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

#define ICONTENTCTRL_VERSION 0x0002

const UINT WM_ICC_WANTSPELLCHECK = ::RegisterWindowMessage(_T("WM_ICC_WANTSPELLCHECK"));

class IContent;
class IContentControl;
class IPreferences;

typedef IContent* (*PFNCREATECONTENT)(); // function prototype

extern "C" DLL_DECLSPEC IContent* CreateContentInterface();

typedef int (*PFNGETVERSION)(); // function prototype
extern "C" DLL_DECLSPEC int GetInterfaceVersion();

#pragma warning(disable:4505)

// helper method
static IContent* CreateContentInterface(const TCHAR* szDllPath, int* pVer = 0)
{
	IContent* pInterface = NULL;
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATECONTENT pCreate = (PFNCREATECONTENT)GetProcAddress(hDll, "CreateContentInterface");

		if (pCreate)
		{
			// check version
			PFNGETVERSION pVersion = (PFNGETVERSION)GetProcAddress(hDll, "GetInterfaceVersion");
			int nInterfaceVer = ICONTENTCTRL_VERSION;

			// pass version back to caller
			if (pVer)
			{
				*pVer = pVersion ? pVersion() : 0;
			}

			if (!nInterfaceVer || (pVersion && pVersion() >= nInterfaceVer))
			{
				pInterface = pCreate();
			}
		}

		if (hDll && !pInterface)
		{
			FreeLibrary(hDll);
		}
	}

	return pInterface;
}

static BOOL IsContentDll(const TCHAR* szDllPath)
{
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATECONTENT pCreate = (PFNCREATECONTENT)GetProcAddress(hDll, "CreateContentInterface");
		FreeLibrary(hDll);

		return (pCreate != NULL);
	}

	return FALSE;
}

class IContent
{
public:
	virtual const char* GetTypeID() const = 0;
	virtual const char* GetTypeDescription() const = 0;

	virtual IContentControl* CreateCtrl(unsigned short nCtrlID, unsigned long nStyle, long nLeft, long nTop,
		long nWidth, long nHeight, HWND hwndParent) = 0;

	virtual void Release() = 0;

	// returns the length of the html or zero if not supported
	virtual int ConvertToHtml(const unsigned char* pContent, int nLength, const TCHAR* szCharSet, char*& pHtml) = 0;
};

class ISpellCheck;
struct UITHEME;

class IContentControl
{
public:
	// custom/binary data format
	virtual int GetContent(unsigned char* pContent) const = 0;
	virtual bool SetContent(unsigned char* pContent, int nLength, BOOL bResetSelection) = 0;
	virtual const char* GetTypeID() const = 0;

	// text content if supported. return false if not supported
	virtual int GetTextContent(TCHAR* szContent, int nLength = -1) const = 0;
	virtual bool SetTextContent(const TCHAR* szContent, BOOL bResetSelection) = 0;

	virtual void SetReadOnly(bool bReadOnly) = 0;
	virtual HWND GetHwnd() const = 0;

	virtual void Release() = 0;
	virtual bool ProcessMessage(MSG* pMsg) = 0;

	virtual ISpellCheck* GetSpellCheckInterface() = 0;

	virtual bool Undo() = 0;
	virtual bool Redo() = 0;

	virtual void SetUITheme(const UITHEME* pTheme) = 0;

	virtual void SavePreferences(IPreferences* pPrefs, LPCTSTR szKey) const = 0;
	virtual void LoadPreferences(const IPreferences* pPrefs, LPCTSTR szKey) = 0;
};

#endif // AFX_ICONTENTCONTROL_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_
