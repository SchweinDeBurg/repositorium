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

// UIThemeFile.h: interface for the CUIThemeFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UITHEMEFILE_H__A652A09A_AB3F_4B65_A3D9_B4B975A36195__INCLUDED_)
#define AFX_UITHEMEFILE_H__A652A09A_AB3F_4B65_A3D9_B4B975A36195__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "UITheme.h"

class CXmlFile;

class CUIThemeFile : public UITHEME  
{
public:
	CUIThemeFile();
	virtual ~CUIThemeFile();

	BOOL LoadThemeFile(LPCTSTR szThemeFile);
	void Reset();

protected:
	static COLORREF GetColor(const CXmlFile& xiFile, LPCTSTR szName, int nColorID = -1);
	static UI_STYLE GetStyle(const CXmlFile& xiFile);

};

#endif // !defined(AFX_UITHEMEFILE_H__A652A09A_AB3F_4B65_A3D9_B4B975A36195__INCLUDED_)
