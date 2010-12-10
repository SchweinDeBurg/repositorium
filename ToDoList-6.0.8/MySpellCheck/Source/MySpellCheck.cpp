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
//*****************************************************************************

// MySpellCheck.cpp : Defines the entry point for the DLL application.
//

#include "StdAfx.h"

#include "../../Common/ISpellCheck.h"
#include "../../Common/SpellChecker.h"

BOOL APIENTRY DllMain( HANDLE /*hModule*/, 
                       DWORD  /*ul_reason_for_call*/, 
                       LPVOID /*lpReserved*/
					 )
{
    return TRUE;
}

DLL_DECLSPEC ISpellChecker* CreateSpellCheckerInterface(const TCHAR* szAffPath, const TCHAR* szDicPath)
{
	// some simple checks first
	if (GetFileAttributes(szDicPath) == 0xffffffff)
		return NULL;

	CSpellChecker* pSC = new CSpellChecker(szAffPath, szDicPath);

	if (!pSC->IsValid())
	{
		delete pSC;
		pSC = NULL;
	}

	return pSC;
}
