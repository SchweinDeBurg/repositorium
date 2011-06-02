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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// UIThemeFile.cpp: implementation of the CUIThemeFile class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIThemeFile.h"
#include "../../Common/XmlFile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

	if (!xiFile.Load(szThemeFile, _T("TODOLIST")))
	{
		return FALSE;
	}

	if (!xiFile.GetItem(_T("UITHEME")))
	{
		return FALSE;
	}

	// else
	nStyle = GetStyle(xiFile);

	crAppBackDark = GetColor(xiFile, _T("APPBACKDARK"));
	crAppBackLight = GetColor(xiFile, _T("APPBACKLIGHT"));
	crAppLinesDark = GetColor(xiFile, _T("APPLINESDARK"), COLOR_3DSHADOW);
	crAppLinesLight = GetColor(xiFile, _T("APPLINESLIGHT"), COLOR_3DHIGHLIGHT);
	crAppText = GetColor(xiFile, _T("APPTEXT"), COLOR_WINDOWTEXT);
	crMenuBack = GetColor(xiFile, _T("MENUBACK"), COLOR_3DFACE);
	crToolbarDark = GetColor(xiFile, _T("TOOLBARDARK"));
	crToolbarLight = GetColor(xiFile, _T("TOOLBARLIGHT"));
	crStatusBarDark = GetColor(xiFile, _T("STATUSBARDARK"));
	crStatusBarLight = GetColor(xiFile, _T("STATUSBARLIGHT"));
	crStatusBarText = GetColor(xiFile, _T("STATUSBARTEXT"), COLOR_WINDOWTEXT);

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
	const CXmlItem* pXIName = xiFile.FindItem(_T("NAME"), szName);

	if (!pXIName)
	{
		if (nColorID != -1)
		{
			return GetSysColor(nColorID);
		}
		else
		{
			return UIT_NOCOLOR;
		}
	}

	const CXmlItem* pXIColor = pXIName->GetParent();
	ASSERT(pXIColor);

	BYTE bRed = (BYTE)pXIColor->GetItemValueI(_T("R"));
	BYTE bGreen = (BYTE)pXIColor->GetItemValueI(_T("G"));
	BYTE bBlue = (BYTE)pXIColor->GetItemValueI(_T("B"));

	return RGB(bRed, bGreen, bBlue);
}

UI_STYLE CUIThemeFile::GetStyle(const CXmlFile& xiFile)
{
	const CXmlItem* pXITheme = xiFile.GetItem(_T("UITHEME"));
	ASSERT(pXITheme);

	if (!pXITheme)
	{
		return UIS_GRADIENT;
	}

	CString sStyle = pXITheme->GetItemValue(_T("STYLE"));

	if (sStyle == _T("GLASS"))
	{
		return UIS_GLASS;
	}

	// else
	return UIS_GRADIENT;
}
