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

};

#endif // !defined(AFX_GRAPHICSMISC_H__A3408501_A44D_407B_A8C3_B6AB31370CD2__INCLUDED_)
