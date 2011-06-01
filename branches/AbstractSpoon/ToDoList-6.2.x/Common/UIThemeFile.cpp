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
//*****************************************************************************

// UIThemeFile.cpp: implementation of the CUIThemeFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UIThemeFile.h"
#include "XmlFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CUIThemeFile::CUIThemeFile()
{
	Reset();
}

CUIThemeFile::~CUIThemeFile()
{

}

BOOL CUIThemeFile::LoadThemeFile(LPCTSTR szThemeFile)
{
	CXmlFile xiFile;

	if (!xiFile.Load(szThemeFile, "TODOLIST"))
		return FALSE;

	if (!xiFile.GetItem("UITHEME"))
		return FALSE;

	// else
	nStyle = GetStyle(xiFile);

	crAppBackDark = GetColor(xiFile, "APPBACKDARK");
	crAppBackLight = GetColor(xiFile, "APPBACKLIGHT");
	crAppLinesDark = GetColor(xiFile, "APPLINESDARK", COLOR_3DSHADOW);
	crAppLinesLight = GetColor(xiFile, "APPLINESLIGHT", COLOR_3DHIGHLIGHT);
	crAppText = GetColor(xiFile, "APPTEXT", COLOR_WINDOWTEXT);
	crMenuBack = GetColor(xiFile, "MENUBACK", COLOR_3DFACE);
	crToolbarDark = GetColor(xiFile, "TOOLBARDARK");
	crToolbarLight = GetColor(xiFile, "TOOLBARLIGHT");
	crStatusBarDark = GetColor(xiFile, "STATUSBARDARK");
	crStatusBarLight = GetColor(xiFile, "STATUSBARLIGHT");
	crStatusBarText = GetColor(xiFile, "STATUSBARTEXT", COLOR_WINDOWTEXT);

	return TRUE;
}

void CUIThemeFile::Reset()
{
	nStyle = UIS_GRADIENT;

	crAppBackDark = GetSysColor(COLOR_3DFACE);
	crAppBackLight = GetSysColor(COLOR_3DFACE);
	crAppLinesDark = GetSysColor(COLOR_3DSHADOW);
	crAppLinesLight = GetSysColor(COLOR_3DHIGHLIGHT);
	crAppText = GetSysColor(COLOR_WINDOWTEXT);
	crMenuBack = GetSysColor(COLOR_3DFACE);
	crToolbarDark = GetSysColor(COLOR_3DFACE);
	crToolbarLight = GetSysColor(COLOR_3DFACE);
	crStatusBarDark = GetSysColor(COLOR_3DFACE);
	crStatusBarLight = GetSysColor(COLOR_3DFACE);
	crStatusBarText = GetSysColor(COLOR_WINDOWTEXT);
}

COLORREF CUIThemeFile::GetColor(const CXmlFile& xiFile, LPCTSTR szName, int nColorID)
{
	const CXmlItem* pXIName = xiFile.FindItem("NAME", szName);

	if (!pXIName)
	{
		if (nColorID != -1)
			return GetSysColor(nColorID);
		else
			return UIT_NOCOLOR;
	}

	const CXmlItem* pXIColor = pXIName->GetParent();
	ASSERT(pXIColor);

	BYTE bRed = (BYTE)pXIColor->GetItemValueI("R");
	BYTE bGreen = (BYTE)pXIColor->GetItemValueI("G");
	BYTE bBlue = (BYTE)pXIColor->GetItemValueI("B");

	return RGB(bRed, bGreen, bBlue);
}

UI_STYLE CUIThemeFile::GetStyle(const CXmlFile& xiFile)
{
	const CXmlItem* pXITheme = xiFile.GetItem("UITHEME");
	ASSERT (pXITheme);

	if (!pXITheme)
		return UIS_GRADIENT;

	CString sStyle = pXITheme->GetItemValue("STYLE");

	if (sStyle == "GLASS")
		return UIS_GLASS;

	// else
	return UIS_GRADIENT;
}
