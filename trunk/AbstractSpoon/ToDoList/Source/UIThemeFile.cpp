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

// UIThemeFile.cpp: implementation of the CUIThemeFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ToDoListApp.h"
#include "UIThemeFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIThemeFile::CUIThemeFile() : CXmlFile("TODOLIST")
{

}

CUIThemeFile::~CUIThemeFile()
{

}

BOOL CUIThemeFile::LoadThemeFile(LPCTSTR szThemeFile)
{
	if (!CXmlFile::Load(szThemeFile))
		return FALSE;

	if (!GetItem("UITHEME"))
		return FALSE;

	// else
	return TRUE;
}

void CUIThemeFile::GetTheme(UITHEME& theme) const
{
	theme.nStyle = GetStyle();

	theme.crAppBackDark = GetColor("APPBACKDARK");
	theme.crAppBackLight = GetColor("APPBACKLIGHT");
	theme.crAppLines = GetColor("APPLINES");
	//theme.crAppText = GetColor("APPTEXT");
	theme.crMenuBack = GetColor("MENUBACK");
	theme.crToolbarDark = GetColor("TOOLBARDARK");
	theme.crToolbarLight = GetColor("TOOLBARLIGHT");
	theme.crStatusBarDark = GetColor("STATUSBARDARK");
	theme.crStatusBarLight = GetColor("STATUSBARLIGHT");
}

COLORREF CUIThemeFile::GetColor(LPCTSTR szName) const
{
	const CXmlItem* pXIName = FindItem("NAME", szName);

	if (!pXIName)
		return UIT_NOCOLOR;

	const CXmlItem* pXIColor = pXIName->GetParent();
	ASSERT(pXIColor);

	BYTE bRed = (BYTE)pXIColor->GetItemValueI("R");
	BYTE bGreen = (BYTE)pXIColor->GetItemValueI("G");
	BYTE bBlue = (BYTE)pXIColor->GetItemValueI("B");

	return RGB(bRed, bGreen, bBlue);
}

UI_STYLE CUIThemeFile::GetStyle() const
{
	const CXmlItem* pXITheme = GetItem("UITHEME");
	ASSERT (pXITheme);

	if (!pXITheme)
		return UIS_GRADIENT;

	CString sStyle = pXITheme->GetItemValue("STYLE");

	if (sStyle == "GLASS")
		return UIS_GLASS;

	// else
	return UIS_GRADIENT;
}
