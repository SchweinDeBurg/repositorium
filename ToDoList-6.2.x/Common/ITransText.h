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
// - added AbstractSpoon Software copyright notice and licenese information
//*****************************************************************************

// ITransText.h: interface for the ITransText class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITRANSTEXT_H__B685332D_C395_46B1_8DDB_ED8196B3A63F__INCLUDED_)
#define AFX_ITRANSTEXT_H__B685332D_C395_46B1_8DDB_ED8196B3A63F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef BOOL (*PFNTRANSTEXTINIT)(LPCTSTR, BOOL);
typedef BOOL (*PFNTRANSTEXTTRANSLATE)(CString&);
typedef void (*PFNTRANSTEXTUPDATEMENU)(HWND);
typedef void (*PFNTRANSTEXTRELEASE)();

#define ITRANSTEXTAPI __declspec(dllexport)

extern "C" ITRANSTEXTAPI BOOL Initialize(LPCTSTR szTransFile, BOOL bAddToDictionary);
extern "C" ITRANSTEXTAPI void Release();
extern "C" ITRANSTEXTAPI BOOL TranslateText(CString& sText);
extern "C" ITRANSTEXTAPI void UpdateMenu(HWND hWnd);

static HINSTANCE g_hTransTextDll = NULL;

static BOOL TTInitialize(LPCTSTR szTransFile = NULL, BOOL bAddToDictionary = TRUE)
{
	if (g_hTransTextDll)
	{
		return TRUE;
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	HINSTANCE hInst = LoadLibrary("transtext.dll");

	if (hInst)
	{
		PFNTRANSTEXTINIT pfnInit = (PFNTRANSTEXTINIT)GetProcAddress(hInst, "Initialize");

		if (pfnInit)
		{
			if (pfnInit(szTransFile, bAddToDictionary))
			{
				g_hTransTextDll = hInst;
				return TRUE;
			}
		}
		else
		{
			FreeLibrary(hInst);
		}
	}

	return FALSE;
}

static BOOL TTTranslateText(CString& sText)
{
	if (!g_hTransTextDll)
	{
		return FALSE;
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!sText.GetLength())
	{
		return FALSE;
	}

	PFNTRANSTEXTTRANSLATE pfnTranslate = (PFNTRANSTEXTTRANSLATE)GetProcAddress(g_hTransTextDll, "TranslateText");

	if (pfnTranslate)
	{
		return pfnTranslate(sText);
	}

	return FALSE;
}

static BOOL TTRelease()
{
	if (!g_hTransTextDll)
	{
		return TRUE;
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	PFNTRANSTEXTRELEASE pfnRelease = (PFNTRANSTEXTRELEASE)GetProcAddress(g_hTransTextDll, "Release");

	if (pfnRelease)
	{
		pfnRelease();
		FreeLibrary(g_hTransTextDll);
		g_hTransTextDll = NULL;

		return TRUE;
	}

	return FALSE;
}

static BOOL TTUpdateMenu(HWND hWnd)
{
	if (!g_hTransTextDll)
	{
		return FALSE;
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	if (!hWnd)
	{
		return FALSE;
	}

	PFNTRANSTEXTUPDATEMENU pfnUpdate = (PFNTRANSTEXTUPDATEMENU)GetProcAddress(g_hTransTextDll, "UpdateMenu");

	if (pfnUpdate)
	{
		pfnUpdate(hWnd);
		return TRUE;
	}

	return FALSE;
}

#endif // !defined(AFX_ITRANSTEXT_H__B685332D_C395_46B1_8DDB_ED8196B3A63F__INCLUDED_)
