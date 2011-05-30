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
// - taken out from the original ToDoList package for better sharing
// - merged with ToDoList version 6.1 sources
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
// - merged with ToDoList version 6.1.7 sources
//*****************************************************************************

// GraphicsMisc.cpp: implementation of the GraphicsMisc class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GraphicsMisc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void GraphicsMisc::DrawGradient(HDC hdc, LPRECT pRect, COLORREF crFrom, COLORREF crTo, BOOL bHorz)
{
	TRIVERTEX        vert[2] ;
	GRADIENT_RECT    gRect;
	vert [0] .x      = pRect->left;
	vert [0] .y      = pRect->top;
	vert [0] .Red    = MAKEWORD(0, GetRValue(crFrom));
	vert [0] .Green  = MAKEWORD(0, GetGValue(crFrom));
	vert [0] .Blue   = MAKEWORD(0, GetBValue(crFrom));
	vert [0] .Alpha  = 0x0000;
	vert [1] .x      = pRect->right;
	vert [1] .y      = pRect->bottom;
	vert [1] .Red    = MAKEWORD(0, GetRValue(crTo));
	vert [1] .Green  = MAKEWORD(0, GetGValue(crTo));
	vert [1] .Blue   = MAKEWORD(0, GetBValue(crTo));
	vert [1] .Alpha  = 0x0000;
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	GradientFill(hdc, vert, 2, &gRect, 1, bHorz ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V);
}

HFONT GraphicsMisc::CreateFont(HFONT hFont, DWORD dwFlags)
{
	LOGFONT lf;
	::GetObject(hFont, sizeof(lf), &lf);

	lf.lfUnderline = (BYTE)(dwFlags & MFS_UNDERLINED);
	lf.lfItalic = (BYTE)(dwFlags & MFS_ITALIC);
	lf.lfStrikeOut = (BYTE)(dwFlags & MFS_STRIKETHRU);
	lf.lfWeight = (dwFlags & MFS_BOLD) ? FW_BOLD : FW_NORMAL;

	HFONT hFontOut = CreateFontIndirect(&lf);

	// verify the font creation
	if (!SameFontNameSize(hFont, hFontOut))
	{
		AfxMessageBox(_T("failed to create font"));
		DeleteObject(hFontOut);
		hFont = NULL;
	}

	return hFontOut;
}

BOOL GraphicsMisc::CreateFont(CFont& fontOut, HFONT fontIn, DWORD dwFlags)
{
	fontOut.DeleteObject();

	return fontOut.Attach(CreateFont(fontIn, dwFlags));
}

HFONT GraphicsMisc::CreateFont(LPCTSTR szFaceName, int nPoint, DWORD dwFlags)
{
	HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	ASSERT(hDefFont);

	LOGFONT lf;
	::GetObject(hDefFont, sizeof(lf), &lf);

	// set the charset
	if (dwFlags & MFS_SYMBOL)
	{
		lf.lfCharSet = SYMBOL_CHARSET;
	}

	else if (!lf.lfCharSet)
	{
		lf.lfCharSet = DEFAULT_CHARSET;
	}

	if (szFaceName && *szFaceName)
	{
		lstrcpy(lf.lfFaceName, szFaceName);
	}

	if (nPoint > 0)
	{
		HDC hDC = ::GetDC(NULL);
		lf.lfHeight = -MulDiv(abs(nPoint), GetDeviceCaps(hDC, LOGPIXELSY), 72);
		::ReleaseDC(NULL, hDC);
	}
	else if (dwFlags & MFS_SYMBOL)
	{
		lf.lfHeight = MulDiv(lf.lfHeight, 12, 10);
	}

	lf.lfWidth = 0;
	lf.lfUnderline = (BYTE)(dwFlags & MFS_UNDERLINED);
	lf.lfItalic = (BYTE)(dwFlags & MFS_ITALIC);
	lf.lfStrikeOut = (BYTE)(dwFlags & MFS_STRIKETHRU);
	lf.lfWeight = (dwFlags & MFS_BOLD) ? FW_BOLD : FW_NORMAL;

	HFONT hFont = CreateFontIndirect(&lf);

	// verify the font creation
	if (!SameFont(hFont, szFaceName, nPoint))
	{
		AfxMessageBox(_T("failed to create font"));
		DeleteObject(hFont);
		hFont = NULL;
	}

	return hFont;
}

BOOL GraphicsMisc::CreateFont(CFont& font, LPCTSTR szFaceName, int nPoint, DWORD dwFlags)
{
	font.DeleteObject();

	return font.Attach(CreateFont(szFaceName, nPoint, dwFlags));
}

DWORD GraphicsMisc::GetFontFlags(HFONT hFont)
{
	if (!hFont)
	{
		return 0;
	}

	LOGFONT lf;
	::GetObject(hFont, sizeof(lf), &lf);

	DWORD dwFlags = 0;

	dwFlags |= (lf.lfItalic ? MFS_ITALIC : 0);
	dwFlags |= (lf.lfUnderline ? MFS_UNDERLINED : 0);
	dwFlags |= (lf.lfStrikeOut ? MFS_STRIKETHRU : 0);
	dwFlags |= (lf.lfWeight >= FW_BOLD ? MFS_BOLD : 0);

	return dwFlags;
}

int GraphicsMisc::GetFontNameSize(HFONT hFont, CString& sFaceName)
{
	if (!hFont)
	{
		sFaceName.Empty();
		return 0;
	}

	LOGFONT lf;
	::GetObject(hFont, sizeof(lf), &lf);

	sFaceName = lf.lfFaceName;

	HDC hDC = ::GetDC(NULL);
	int nPoint = MulDiv(abs(lf.lfHeight), 72, GetDeviceCaps(hDC, LOGPIXELSY));
	::ReleaseDC(NULL, hDC);

	return nPoint;
}

BOOL GraphicsMisc::SameFont(HFONT hFont, LPCTSTR szFaceName, int nPoint)
{
	CString sFontName;
	int nFontSize = GetFontNameSize(hFont, sFontName);

	return ((nPoint <= 0 || nPoint == nFontSize) &&
			(!szFaceName || sFontName.CompareNoCase(szFaceName) == 0));
}

BOOL GraphicsMisc::SameFontNameSize(HFONT hFont1, HFONT hFont2)
{
	CString sName1;
	int nSize1 = GetFontNameSize(hFont1, sName1);

	return SameFont(hFont2, sName1, nSize1);
}

HCURSOR GraphicsMisc::HandCursor()
{
#ifndef IDC_HAND
#define IDC_HAND  MAKEINTRESOURCE(32649) // from winuser.h
#endif
	static HCURSOR cursor = NULL;

	if (!cursor)
	{
		cursor = ::LoadCursor(NULL, IDC_HAND);

		// fallback hack for win9x
		if (!cursor)
		{
			CString sWinHlp32;

			GetWindowsDirectory(sWinHlp32.GetBuffer(MAX_PATH), MAX_PATH);
			sWinHlp32.ReleaseBuffer();
			sWinHlp32 += _T("\\winhlp32.exe");

			HMODULE hMod = LoadLibrary(sWinHlp32);

			if (hMod)
			{
				cursor = ::LoadCursor(hMod, MAKEINTRESOURCE(106));
			}
		}
	}

	return cursor;
}

CFont& GraphicsMisc::WingDings()
{
	static CFont font;

	if (!font.GetSafeHandle())
	{
		font.Attach(CreateFont(_T("Wingdings"), -1, MFS_SYMBOL));
	}

	return font;
}

CFont& GraphicsMisc::Marlett()
{
	static CFont font;

	if (!font.GetSafeHandle())
	{
		font.Attach(CreateFont(_T("Marlett"), -1, MFS_SYMBOL));
	}

	return font;
}

int AFX_CDECL GraphicsMisc::GetTextWidth(CDC* pDC, LPCTSTR lpszFormat, ...)
{
	static TCHAR BUFFER[2048];

	ASSERT(AfxIsValidString(lpszFormat));

	va_list argList;
	va_start(argList, lpszFormat);
	//fabio_2005
#if _MSC_VER >= 1400
	_vstprintf_s(BUFFER, sizeof(BUFFER), lpszFormat, argList);
#else
	_vstprintf(BUFFER, lpszFormat, argList);
#endif
	va_end(argList);

	return pDC->GetTextExtent(BUFFER).cx;
}

float GraphicsMisc::GetAverageCharWidth(CDC* pDC)
{
	ASSERT(pDC);

	int nExtent = pDC->GetTextExtent(_T("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz")).cx;

	return (nExtent / 52.0f);
}

COLORREF GraphicsMisc::Lighter(COLORREF color, double dAmount)
{
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);

	red += (int)((255 - red) * dAmount);
	green += (int)((255 - green) * dAmount);
	blue += (int)((255 - blue) * dAmount);

	red = min(255, red);
	green = min(255, green);
	blue = min(255, blue);

	return RGB(red, green, blue);
}

COLORREF GraphicsMisc::Darker(COLORREF color, double dAmount)
{
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);

	red -= (int)((255 - red) * dAmount);
	green -= (int)((255 - green) * dAmount);
	blue -= (int)((255 - blue) * dAmount);

	red = max(0, red);
	green = max(0, green);
	blue = max(0, blue);

	return RGB(red, green, blue);
}

BOOL GraphicsMisc::EnableAeroPeek(HWND hWnd, BOOL bEnable)
{
#ifndef DWMWA_DISALLOW_PEEK
#define DWMWA_DISALLOW_PEEK 11
#endif

	BOOL bDisallow = !bEnable;

	return DwmSetWindowAttribute(hWnd, DWMWA_DISALLOW_PEEK, &bDisallow);
}

BOOL GraphicsMisc::EnableFlip3D(HWND hWnd, BOOL bEnable)
{
#ifndef DWMWA_FLIP3D_POLICY
#define DWMWA_FLIP3D_POLICY 8
#define DWMFLIP3D_DEFAULT      0
#define DWMFLIP3D_EXCLUDEBELOW 1
#define DWMFLIP3D_EXCLUDEABOVE 2
#endif

	int nPolicy = bEnable ? DWMFLIP3D_DEFAULT : DWMFLIP3D_EXCLUDEBELOW;

	return DwmSetWindowAttribute(hWnd, DWMWA_FLIP3D_POLICY, &nPolicy);
}
