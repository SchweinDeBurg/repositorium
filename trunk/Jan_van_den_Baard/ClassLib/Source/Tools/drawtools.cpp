//
// drawtools.cpp
//
// (C) Copyright 2000-2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "drawtools.h"
#include "theming.h"
#include "color.h"
#include "xpcolors.h"
#include "../gdi/brush.h"
#include "../gdi/pen.h"
#include "../gdi/dc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Just in case...
#ifndef DT_HIDEPREFIX
#define DT_HIDEPREFIX               0x00100000
#endif

// Data structure used by the
// DrawState() API callbacks.
struct StrData
{
	// Contains either a pointer to the
	// text to render or a image list handle.
	union
	{
		LPCTSTR		m_pszText;
		HIMAGELIST	m_hImageList;;
	} m_Data;

	// Bitmap index.
	int	m_iBitmap;

	// Rendering flags (See above)
	DWORD	m_dwFlags;
};

// Renders a text.
BOOL ClsDrawTools::RenderText( HDC hDC, LPCTSTR pszText, ClsRect& cRect, DWORD dwFlags )
{
	StrData	sData;

	// Setup the data structure.
	sData.m_Data.m_pszText = pszText;
	sData.m_dwFlags	       = dwFlags;

	// Render the text.
	return ::DrawState( hDC, 
			    NULL, 
			    ( DRAWSTATEPROC )ClsDrawTools::RenderTextHelper, 
			    ( LPARAM )&sData, 
			    ( WPARAM )_tcslen( pszText ), 
			    cRect.Left(), 
			    cRect.Top(), 
			    cRect.Width(), 
			    cRect.Height(), 
			    DST_COMPLEX | ( dwFlags & CDSF_DISABLED ? DSS_DISABLED : 0 ));
}

// Renders an image from an image list.
BOOL ClsDrawTools::RenderBitmap( HDC hDC, HIMAGELIST hImageList, int iBitmap, ClsRect& cRect, DWORD dwFlags )
{
	StrData		sData;
	BOOL		bRC = FALSE;

	// Setup the data structure.
	sData.m_Data.m_hImageList = hImageList;
	sData.m_iBitmap		  = iBitmap;
	sData.m_dwFlags		  = dwFlags;

	// Render the image.
	bRC = ::DrawState( hDC, 
			   NULL, 
			   ( DRAWSTATEPROC )ClsDrawTools::RenderBitmapHelper, 
			   ( LPARAM )&sData, 
			   ( WPARAM )0, 
			   cRect.Left(), 
			   cRect.Top(), 
			   cRect.Width(), 
			   cRect.Height(), 
			   DST_COMPLEX | ( dwFlags & CDSF_DISABLED ? DSS_DISABLED : 0 ));

	return bRC;
}

// Renders a monochrome image from an image list.
BOOL ClsDrawTools::RenderMonoBitmap( HDC hDC, HIMAGELIST hImageList, int iBitmap, ClsRect& cRect, COLORREF crColor, DWORD dwFlags /* = 0 */ )
{
	StrData		sData;
	BOOL		bRC = FALSE;

	// Setup the data structure.
	sData.m_Data.m_hImageList = hImageList;
	sData.m_iBitmap		  = iBitmap;
	sData.m_dwFlags		  = dwFlags;

	// Create color brush.
	ClsBrush brush( crColor );

	// Render the image.
	bRC = ::DrawState( hDC, 
			   brush, 
			   ( DRAWSTATEPROC )ClsDrawTools::RenderBitmapHelper, 
			   ( LPARAM )&sData, 
			   ( WPARAM )0, 
			   cRect.Left(), 
			   cRect.Top(), 
			   cRect.Width(), 
			   cRect.Height(), 
			   DST_COMPLEX | DSS_MONO );

	return bRC;
}

// Renders a XP style image from an image list.
BOOL ClsDrawTools::RenderXPBitmap( HDC hDC, HIMAGELIST hImageList, int iBitmap, ClsRect& cRect, DWORD dwFlags )
{
	BOOL bRC = TRUE;

	// Hot?
	if ( dwFlags & CDSF_HOT )
	{
		// Offset the image position and render the
		// shadow bitmap. This does not exactly match
		// the Office XP style but it's close enough...
		cRect.Offset( 1, 1 );
		bRC = RenderMonoBitmap( hDC, hImageList, iBitmap, cRect, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_SHADOW ), dwFlags );
		cRect.Offset( -2, -2 );
	}

	// Disabled or normal?
	if ( bRC )
	{
		if ( dwFlags & CDSF_DISABLED ) 
			bRC = ClsDrawTools::RenderMonoBitmap( hDC, hImageList, iBitmap, cRect, XPColors.GetXPColor( ClsXPColors::XPC_IMAGE_DISABLED ), dwFlags );
		else
			bRC = ClsDrawTools::RenderBitmap( hDC, hImageList, iBitmap, cRect, dwFlags );
	}
	return bRC;
}

// Callback for the RenderText() function.
BOOL CALLBACK ClsDrawTools::RenderTextHelper( HDC hDC, LPARAM lParam, WPARAM wParam, int cx, int cy )
{
	StrData  *pData = ( StrData * )lParam;
	ClsRect	cRect, cText;
	DWORD dwFlags = DT_EXPANDTABS;
	BOOL bRC = FALSE;

	// Get the text size.
	::DrawText( hDC, pData->m_Data.m_pszText, ( int )wParam, cText, dwFlags | DT_CALCRECT );

	// Setup the rectangle cx, cy holds the
	// width and height of the area.
	cRect.Right()  = cx;
	cRect.Bottom() = cy;

	// Center the text vertically in the area.
	cRect.Top()  = max( 0, ( cy - cText.Height()) / 2 );

	// Setup alignment.
	if (      pData->m_dwFlags & CDSF_LEFTALIGN  ) cRect.Left() = 0;
	else if ( pData->m_dwFlags & CDSF_RIGHTALIGN ) cRect.Left() = cRect.Right() - cText.Width();
	else					       cRect.Left() = max( 0, ( cx - cText.Width()) / 2 );

	// Are we to appear down-pressed?
	if ( pData->m_dwFlags & CDSF_DOWNPRESSED )
	{
		// Move the left and top edge.
		cRect.Left()++;
		cRect.Top()++;
	}

	// Transparent?
	int oldBkMode = 0;
	if ( pData->m_dwFlags & CDSF_TRANSPARENT )
		oldBkMode = ::SetBkMode( hDC, TRANSPARENT );

	// Render the text.
	// Setup flags.
	if ( pData->m_dwFlags & CDSF_HIDEACCEL     ) dwFlags |= DT_HIDEPREFIX;
	if ( pData->m_dwFlags & CDSF_END_ELLIPSIS  ) dwFlags |= DT_END_ELLIPSIS;
	if ( pData->m_dwFlags & CDSF_PATH_ELLIPSIS ) dwFlags |= DT_PATH_ELLIPSIS;
	bRC = ( BOOL )( ::DrawText( hDC, pData->m_Data.m_pszText, ( int )wParam, cRect, dwFlags ) ? TRUE : FALSE );

	// Restore background mode.
	if ( pData->m_dwFlags & CDSF_TRANSPARENT )
		oldBkMode = ::SetBkMode( hDC, oldBkMode );

	return bRC;
}

// Callback for the RenderBitmap() function.
BOOL CALLBACK ClsDrawTools::RenderBitmapHelper( HDC hDC, LPARAM lParam, WPARAM wParam, int cx, int cy )
{
	StrData  *pData = ( StrData * )lParam;
	int	cbx, cby;
	COLORREF crOldBkColor = CLR_NONE;

	// Transparent?
	if ( pData->m_dwFlags & CDSF_TRANSPARENT ) 
		crOldBkColor = ImageList_SetBkColor( pData->m_Data.m_hImageList, CLR_NONE );

	// Prevent a compiler warning when compiling at
	// warning level 4 (Visual C++ 6.0)
	UNREFERENCED_PARAMETER( wParam );

	// Get the image width and height.
	::ImageList_GetIconSize( pData->m_Data.m_hImageList, &cbx, &cby );

	// Center the image vertically inside the area.
	int nTop  = ( cy - cby ) >> 1;// 2;

	// Setup alignment.
	int nLeft;
	if (      pData->m_dwFlags & CDSF_LEFTALIGN  ) nLeft = 0;
	else if ( pData->m_dwFlags & CDSF_RIGHTALIGN ) nLeft = cx - cbx;
	else					       nLeft = ( cx - cbx  ) >> 1;// 2;

	// Are we to appear down-pressed?
	if ( pData->m_dwFlags & CDSF_DOWNPRESSED )
	{
		// Move the left and top edge.
		nLeft++;
		nTop++;
	}

	// Draw the image.
	::ImageList_Draw( pData->m_Data.m_hImageList, pData->m_iBitmap, hDC, nLeft, nTop, ILD_NORMAL );

	// Restore color?
	if ( pData->m_dwFlags & CDSF_TRANSPARENT )
		ImageList_SetBkColor( pData->m_Data.m_hImageList, crOldBkColor );

	return TRUE;
}

// Draw a shadow in the rectangle.
void ClsDrawTools::DrawShadow( HDC hDC, LPCRECT pRect  )
{
	// Copy input rectangle.
	ClsRect rc;
	rc = *pRect;

	// Render the shadow at the right side of the menu.
	ClsColor col;
	double factor;
	int x, y;
	for ( x = 0; x < 5; x++ )
	{
		for ( y = 0; y <= rc.Height() - 9; y++ )
		{
			col = GetPixel( hDC, x + ( rc.Right() - 4 ), y + rc.Top() + 4 );
			if ( y < 4 ) factor = y * 0.02;
			else factor = 0.1;
			factor = max( 0.01, min( 0.01 + (( 4 - x ) * factor ), 1 ));
			col.DarkenColor( factor, factor, factor );				
			SetPixelV( hDC, x + ( rc.Right() - 4 ), y + rc.Top() + 4 , col );
		}
	}

	// And the shadow at the right/bottom corner of the menu.
	for ( x = 0; x < 5; x++ )
	{
		for ( y = 0; y < 4; y++ )
		{
			col = GetPixel( hDC, x + ( rc.Right() - 4 ), y + ( rc.Bottom() - 4 ));
			if ( y && y < 4 ) factor = 0.1 - ( y * 0.02 );
			else factor = 0.1;
			factor = max( 0.01, min( 0.01 + (( 4 - x ) * factor ), 1 ));
			col.DarkenColor( factor, factor, factor );				
			SetPixelV( hDC, x + ( rc.Right() - 4 ), y + ( rc.Bottom() - 4 ), col );
		}
	}

	// And finally the shadow at the bottom of the menu.
	for ( y = 0; y < 5; y++ )
	{
		for ( x = 0; x <= rc.Width() - 9; x++ )
		{
			col = GetPixel( hDC, x + rc.Left() + 4, y + ( rc.Bottom() - 4 ));
			if ( x < 4 ) factor = x * 0.02;
			else factor = 0.1;
			factor = max( 0.01, min( 0.01 + (( 4 - y ) * factor ), 1 ));
			col.DarkenColor( factor, factor, factor );				
			SetPixelV( hDC, x + rc.Left() + 4, y + ( rc.Bottom() - 4 ), col );
		}
	}
}

// Render a rectangle (not-filled)
void ClsDrawTools::DrawRectangle( HDC hDC, LPCRECT pRect, COLORREF crColor, int nPenSize /* = 1 */ )
{
	// Create and setlect the pen.
	ClsPen pen( PS_SOLID | PS_INSIDEFRAME, nPenSize, crColor );
	HGDIOBJ hOldPen = ::SelectObject( hDC, pen );
	HGDIOBJ hOldBrush = ::SelectObject( hDC, ::GetStockObject( NULL_BRUSH ));

	// Draw the rectangle.
	::Rectangle( hDC, pRect->left, pRect->top, pRect->right, pRect->bottom );

	// Restore the original objects.
	::SelectObject( hDC, hOldBrush );
	::SelectObject( hDC, hOldPen );
}

// Render a gradient rectangle.
void ClsDrawTools::DrawGradient( HDC hDC, LPCRECT pRect, COLORREF crColor1, COLORREF crColor2, BOOL bHoriz /* = TRUE */ )
{
	TRIVERTEX       trVert[ 2 ];
	GRADIENT_RECT   gRect;
	COLORREF	crColor = bHoriz ? crColor1 : crColor2;

	// Setup the top (or left) side of the rectangle.
	trVert[ 0 ].x      = pRect->left;
	trVert[ 0 ].y      = pRect->top;;
	trVert[ 0 ].Red    = ( COLOR16 )( GetRValue( crColor ) << 8 );
	trVert[ 0 ].Green  = ( COLOR16 )( GetGValue( crColor ) << 8 );
	trVert[ 0 ].Blue   = ( COLOR16 )( GetBValue( crColor ) << 8 );
	trVert[ 0 ].Alpha  = 0x0000;

	// Setup the bottom (or right) size of the rectangle.
	crColor		   = bHoriz ? crColor2 : crColor1;
	trVert[ 1 ].x      = pRect->right;
	trVert[ 1 ].y      = pRect->bottom;
	trVert[ 1 ].Red    = ( COLOR16 )( GetRValue( crColor ) << 8 );
	trVert[ 1 ].Green  = ( COLOR16 )( GetGValue( crColor ) << 8 );
	trVert[ 1 ].Blue   = ( COLOR16 )( GetBValue( crColor ) << 8 );
	trVert[ 1 ].Alpha  = 0x0000;

	// Tell the function where the upper-left
	// and lower-right corners are located in the
	// TRIVERTEXT array.
	gRect.UpperLeft  = 0;
	gRect.LowerRight = 1;

	// Fill the area.
	ClsDC *pDC = ClsDC::FromHandle( hDC );
	if ( pDC ) 
		pDC->GradientFill( trVert, 2, &gRect, 1, bHoriz ? GRADIENT_FILL_RECT_H : GRADIENT_FILL_RECT_V );
}
