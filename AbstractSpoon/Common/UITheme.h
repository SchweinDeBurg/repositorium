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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// UITheme.h: interface for the UITHEME class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_UITHEME_H__7747061B_7841_47E8_BB36_89C11D6DC934__INCLUDED_)
#define AFX_UITHEME_H__7747061B_7841_47E8_BB36_89C11D6DC934__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum UI_STYLE
{
	UIS_GRADIENT,
	UIS_GLASS,
};

const COLORREF UIT_NOCOLOR = (COLORREF)-1;

struct UITHEME
{
	UI_STYLE nStyle;

	COLORREF crAppBackDark;
	COLORREF crAppBackLight;
	COLORREF crAppLinesDark;
	COLORREF crAppLinesLight;
	COLORREF crAppText;
	COLORREF crMenuBack;
	COLORREF crToolbarDark;
	COLORREF crToolbarLight;
	COLORREF crStatusBarDark;
	COLORREF crStatusBarLight;
	COLORREF crStatusBarText;
};

#endif // !defined(AFX_UITHEME_H__7747061B_7841_47E8_BB36_89C11D6DC934__INCLUDED_)
