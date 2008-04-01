#ifndef _BITMAP_H_
#define _BITMAP_H_
//
// bitmap.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "gdiobject.h"

// A wrapper class for the HBITMAP handle.
class ClsBitmap : public ClsGdiObject
{
	// No copy constructor.
	_NO_COPY( ClsBitmap );
public:
	// Constructor.
	ClsBitmap();

	// Destructor.
	virtual ~ClsBitmap();

	// Interface.
	BOOL LoadBitmap( LPCTSTR pszBitmap );
	BOOL LoadOEMBitmap( UINT nBitmap );
	BOOL LoadMappedBitmap( LPCTSTR pszBitmap, UINT nFlags, LPCOLORMAP pColorMap = NULL, int nMapSize = 0 );
	BOOL CreateBitmap( int nWidth, int nHeight, UINT nPlanes, UINT nBitCount, const void *lpBits );
	BOOL CreateBitmapIndirect( const BITMAP *pBitmap );
	BOOL CreateCompatibleBitmap( HDC hDC, int nWidth, int nHeight );
	BOOL CreateMappedBitmap( int idBitmap, UINT nFlags, LPCOLORMAP pColorMap = NULL, int nMapSize = 0 );
	BOOL GetBitmap( BITMAP *pBitmap ) const;
	BOOL GetDIBSection( DIBSECTION *pDIBSection ) const;
	HBITMAP GetGrayscaledBitmap( COLORREF crException = CLR_NONE, int nXPos = 0, int nYPos = NULL ) const;
	BOOL GetGrayscaledBitmap( ClsBitmap& bmDst, COLORREF crException = CLR_NONE, int nXPos = 0, int nYPos = 0 ) const;
	HBITMAP GetMaskBitmap( COLORREF crColor, int nXPos = 0, int nYPos = NULL ) const;
	BOOL GetMaskBitmap( ClsBitmap& bmDst, COLORREF crColor, int nXPos = 0, int nYPos = 0 ) const;
	HBITMAP GetMappedBitmap( LPCOLORMAP pColorMap, int nMapSize ) const;
	BOOL GetMappedBitmap( ClsBitmap& bmDst, LPCOLORMAP pColorMap, int nMapSize ) const;
	static ClsBitmap *FromHandle( HBITMAP hBitmap );

	// Operator overloads.
	operator HBITMAP() const;

protected:
	// Helpers.
	RGBTRIPLE *GetBitmapBits( HDC hDC, BITMAPINFO& bmi, int nWidth, int nHeight ) const;
};

#endif // _BITMAP_H_