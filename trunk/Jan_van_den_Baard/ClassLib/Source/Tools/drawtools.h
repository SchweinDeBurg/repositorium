#ifndef _DRAWTOOLS_H_
#define _DRAWTOOLS_H_
//
// drawtools.h
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../Coords/rect.h"
#include "../Coords/size.h"

// Simple function only class.
class ClsDrawTools
{
public:
	// Rendering flags.
	enum
	{
		CDSF_DISABLED	   = 0x00000001,
		CDSF_DOWNPRESSED   = 0x00000002,
		CDSF_LEFTALIGN	   = 0x00000004,
		CDSF_RIGHTALIGN	   = 0x00000008,
		CDSF_HIDEACCEL     = 0x00000010,
		CDSF_HOT	   = 0x00000020,
		CDSF_TRANSPARENT   = 0x00000040,
		CDSF_PATH_ELLIPSIS = 0x00000080,
		CDSF_END_ELLIPSIS  = 0x00000100
	};

	// Implementation
	static BOOL RenderText( HDC hDC, LPCTSTR pszText, ClsRect& cRect, DWORD dwFlags );
	static BOOL RenderBitmap( HDC hDC, HIMAGELIST hImageList, int iBitmap, ClsRect& cRect, DWORD dwFlags );
	static BOOL RenderMonoBitmap( HDC hDC, HIMAGELIST hImageList, int iBitmap, ClsRect& cRect, COLORREF crColor, DWORD dwFlags = 0 );
	static BOOL RenderXPBitmap( HDC hDC, HIMAGELIST hImageList, int iBitmap, ClsRect& cRect, DWORD dwFlags );
	static void DrawShadow( HDC hDC, LPCRECT pRect );
	static void DrawRectangle( HDC hDC, LPCRECT pRect, COLORREF crColor, int nPenSize = 1 );
	static void DrawGradient( HDC hDC, LPCRECT pRect, COLORREF crColor1, COLORREF crColor2, BOOL bHoriz = TRUE );

protected:
	// Callbacks for the DrawState API.
	static BOOL CALLBACK RenderTextHelper( HDC hDC, LPARAM lParam, WPARAM wParam, int cx, int cy );
	static BOOL CALLBACK RenderBitmapHelper( HDC hDC, LPARAM lParam, WPARAM wParam, int cx, int cy );
};
#endif // _DRAWTOOLS_H_