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
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// IEncryption.h: interface for the IEncryption class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IENCRYPTION_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
#define AFX_IENCRYPTION_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4189)
#pragma warning(disable: 4127)

// function to be exported from dll to create instance of interface
#ifdef _EXPORTING // declare this in project settings for dll _only_
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

#define IENCRYPTION_VERSION 0x0000

class IEncryption;

typedef IEncryption* (*PFNCREATE)(); // function prototype
extern "C" DLL_DECLSPEC IEncryption* CreateEncryptionInterface();

typedef int (*PFNGETVERSION)(); // function prototype
extern "C" DLL_DECLSPEC int GetInterfaceVersion();

#pragma warning(disable:4505)

// helper method
static IEncryption* CreateEncryptionInterface(const TCHAR* szDllPath)
{
	IEncryption* pInterface = NULL;
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATE pCreate = (PFNCREATE)GetProcAddress(hDll, "CreateEncryptionInterface");

		if (pCreate)
		{
			// check version
			PFNGETVERSION pVersion = (PFNGETVERSION)GetProcAddress(hDll, "GetInterfaceVersion");

			if (!IENCRYPTION_VERSION || (pVersion && pVersion() >= IENCRYPTION_VERSION))
			{
				pInterface = pCreate();
			}
		}
	}

	return pInterface;
}

static BOOL IsEncryptionDll(const TCHAR* szDllPath)
{
	HMODULE hDll = LoadLibrary(szDllPath);

	if (hDll)
	{
		PFNCREATE pCreate = (PFNCREATE)GetProcAddress(hDll, "CreateEncryptionInterface");
		FreeLibrary(hDll);

		return (NULL != pCreate);
	}

	return FALSE;
}

class IEncryption
{
public:
	virtual void Release() = 0; // releases the interface

	// returns a dynamically allocated buffer to the encrypted text
	// caller responsible for calling FreeBuffer on the returned buffer
	virtual bool Encrypt(const unsigned char* pInput, int nLenInput, const char* szPassword,
		unsigned char*& pOutput, int& nLenOutput) = 0;

	// returns a dynamically allocated buffer to the decrypted text
	// caller responsible for calling FreeBuffer on the returned buffer
	virtual bool Decrypt(const unsigned char* pInput, int nLenInput, const char* szPassword,
		unsigned char*& pOutput, int& nLenOutput) = 0;

	// frees a previously returned buffer and sets the ptr to NULL
	// eg for buffer allocated with 'new' use 'delete []'
	// eg for buffer allocated with 'strdup' use 'free'
	virtual void FreeBuffer(unsigned char*& pBuffer) = 0;
};

static void ReleaseEncryptionInterface(IEncryption*& pInterface)
{
	if (pInterface)
	{
		pInterface->Release();
		pInterface = NULL;
	}
}

#endif // !defined(AFX_IENCRYPTION_H__7741547B_BA15_4851_A41B_2B4EC1DC12D5__INCLUDED_)
