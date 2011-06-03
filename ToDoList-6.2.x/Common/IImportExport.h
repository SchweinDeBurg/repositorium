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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// IImportExport.h: interface and implementation of the IImportExport class.
//
/////////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_IIMPORTEXPORT_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
#define AFX_IIMPORTEXPORT_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_

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

#define IIMPORTEXPORT_VERSION 0x0002

class IImportTasklist;
class IExportTasklist;
class ITaskList;
class IMultiTaskList;

typedef IImportTasklist* (*PFNCREATEIMPORT)(); // function prototype
typedef IExportTasklist* (*PFNCREATEEXPORT)(); // function prototype

extern "C" DLL_DECLSPEC IImportTasklist* CreateImportInterface();
extern "C" DLL_DECLSPEC IExportTasklist* CreateExportInterface();

typedef int (*PFNGETVERSION)(); // function prototype
extern "C" DLL_DECLSPEC int GetInterfaceVersion();

#pragma warning(disable:4505)

// helper method
static IImportTasklist* CreateImportInterface(const TCHAR* szDllPath)
{
	IImportTasklist* pInterface = NULL;
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATEIMPORT pCreate = (PFNCREATEIMPORT)GetProcAddress(hDll, "CreateImportInterface");

		if (pCreate)
		{
			// check version
			PFNGETVERSION pVersion = (PFNGETVERSION)GetProcAddress(hDll, "GetInterfaceVersion");

			if (!IIMPORTEXPORT_VERSION || (pVersion && pVersion() >= IIMPORTEXPORT_VERSION))
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

static IExportTasklist* CreateExportInterface(const TCHAR* szDllPath)
{
	IExportTasklist* pInterface = NULL;
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATEEXPORT pCreate = (PFNCREATEEXPORT)GetProcAddress(hDll, "CreateExportInterface");

		if (pCreate)
		{
			// check version
			PFNGETVERSION pVersion = (PFNGETVERSION)GetProcAddress(hDll, "GetInterfaceVersion");

			if (!IIMPORTEXPORT_VERSION || (pVersion && pVersion() >= IIMPORTEXPORT_VERSION))
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

static BOOL IsImportExportDll(const TCHAR* szDllPath)
{
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATEEXPORT pCreateExp = NULL;
		PFNCREATEIMPORT pCreateImp = (PFNCREATEIMPORT)GetProcAddress(hDll, "CreateImportInterface");

		if (!pCreateImp)
		{
			pCreateExp = (PFNCREATEEXPORT)GetProcAddress(hDll, "CreateExportInterface");
		}

		FreeLibrary(hDll);

		return (pCreateImp || pCreateExp);
	}

	return FALSE;
}

class IImportTasklist
{
public:
	virtual void Release() = 0; // releases the interface

	// caller must copy result only
	virtual const char* GetMenuText() = 0;
	virtual const char* GetFileFilter() = 0;
	virtual const char* GetFileExtension() = 0;

	virtual bool Import(const TCHAR* szSrcFilePath, ITaskList* pDestTaskFile) = 0;
};

class IExportTasklist
{
public:
	virtual void Release() = 0; // releases the interface

	// caller must copy result only
	virtual const char* GetMenuText() = 0;
	virtual const char* GetFileFilter() = 0;
	virtual const char* GetFileExtension() = 0;

	virtual bool Export(const ITaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent) = 0;
	virtual bool Export(const IMultiTaskList* pSrcTaskFile, const TCHAR* szDestFilePath, BOOL bSilent) = 0;
};

static void ReleaseImportInterface(IImportTasklist*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

static void ReleaseExportInterface(IExportTasklist*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

#endif // !defined(AFX_IImportExport_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
