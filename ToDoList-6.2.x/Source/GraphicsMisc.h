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
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-cases
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
// - merged with ToDoList version 6.1.2 sources
//*****************************************************************************

// GraphicsMisc.h: interface for the GraphicsMisc class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GRAPHICSMISC_H__A3408501_A44D_407B_A8C3_B6AB31370CD2__INCLUDED_)
#define AFX_GRAPHICSMISC_H__A3408501_A44D_407B_A8C3_B6AB31370CD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

enum { MFS_BOLD = 0x01, MFS_ITALIC = 0x02, MFS_UNDERLINED = 0x04, MFS_STRIKETHRU = 0x08, MFS_SYMBOL = 0x10};

namespace GraphicsMisc
{
void DrawGradient(HDC hdc, LPRECT pRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz);

HFONT CreateFont(HFONT hFont, DWORD dwFlags = 0);
HFONT CreateFont(LPCTSTR szFaceName, int nPoint = -1, DWORD dwFlags = 0);
BOOL CreateFont(CFont& font, LPCTSTR szFaceName, int nPoint = -1, DWORD dwFlags = 0);
BOOL CreateFont(CFont& fontOut, HFONT fontIn, DWORD dwFlags = 0);

HCURSOR HandCursor();

DWORD GetFontFlags(HFONT hFont);
int GetFontNameSize(HFONT hFont, CString& sFaceName);
BOOL SameFont(HFONT hFont, LPCTSTR szFaceName, int nPoint);
BOOL SameFontNameSize(HFONT hFont1, HFONT hFont2);
CFont& WingDings();
CFont& Marlett();

int AFX_CDECL GetTextWidth(CDC* pDC, LPCTSTR lpszFormat, ...);
float GetAverageCharWidth(CDC* pDC);

COLORREF Lighter(COLORREF color, double dAmount);
COLORREF Darker(COLORREF color, double dAmount);

BOOL EnableAeroPeek(HWND hWnd, BOOL bEnable = TRUE);
BOOL EnableFlip3D(HWND hWnd, BOOL bEnable = TRUE);

template <class TYPE>
BOOL DwmSetWindowAttribute(HWND hWnd, DWORD dwAttrib, TYPE* type)
{
	HMODULE hMod = ::LoadLibrary(_T("Dwmapi.dll"));

	if (hMod)
	{
		typedef HRESULT(*PFNDWMSETWINDOWATTRIBUTE)(HWND, DWORD, LPCVOID, DWORD);
		PFNDWMSETWINDOWATTRIBUTE pFn = (PFNDWMSETWINDOWATTRIBUTE)::GetProcAddress(hMod, "DwmSetWindowAttribute");

		if (pFn)
		{
			HRESULT hr = pFn(hWnd, dwAttrib, type, sizeof(*type));
			return SUCCEEDED(hr);
		}
	}

	return FALSE;
}
};

#endif // !defined(AFX_GRAPHICSMISC_H__A3408501_A44D_407B_A8C3_B6AB31370CD2__INCLUDED_)
