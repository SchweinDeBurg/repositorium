//
// bitmap.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "bitmap.h"
#include "dc.h"
#include "../io/file.h"
#include "../exceptions/memoryexception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsBitmap::ClsBitmap()
{
}

// Destructor. Does nothing.
ClsBitmap::~ClsBitmap()
{
}

// Load a bitmap.
BOOL ClsBitmap::LoadBitmap( LPCTSTR pszBitmap )
{
	// Attach the loaded bitmap.
	UINT uFlags = ( pszBitmap && HIWORD( pszBitmap ) == 0 ) ? 0 : LR_LOADFROMFILE;
	Attach( ::LoadImage( ClsGetResourceHandle(), pszBitmap, IMAGE_BITMAP, 0, 0, uFlags ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Load a OEM bitmap.
BOOL ClsBitmap::LoadOEMBitmap( UINT nBitmap )
{
	// Attach the loaded bitmap.
	Attach( ::LoadImage( NULL, MAKEINTRESOURCE( nBitmap ), IMAGE_BITMAP, 0, 0, 0 ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Load a mapped bitmap.
BOOL ClsBitmap::LoadMappedBitmap( LPCTSTR pszBitmap, UINT nFlags, LPCOLORMAP pColorMap /* = NULL */, int nMapSize /* = 0 */ )
{
	// Valid name?
	if ( pszBitmap )
	{
		// From the resources?
		if ( HIWORD( pszBitmap ) == 0 )
			// Attach the mapped bitmap.
			Attach( ::CreateMappedBitmap( ClsGetResourceHandle(), ( UINT )pszBitmap, nFlags, pColorMap, nMapSize ));
		else
		{
			// Load the bitmap from the file. This code is based on the code
			// from "Creating a bitmap object from a BMP file - Zafir Anjum (1998/08/05)"
			// at www.codeguru.com.
			ClsFile	file;
			HBITMAP	hBitmap = NULL;
			HGLOBAL hDIB = NULL;
			int nColors = 0;

			try
			{
				// Open the file.
				file.Open( pszBitmap, ClsFile::fileRead | ClsFile::fileShareRead );

				// Read the file header.
				BITMAPFILEHEADER        bmfHeader;
				file.Read( &bmfHeader, sizeof( bmfHeader ));

				// Is the file of the right type?
				if ( bmfHeader.bfType == (( WORD )( 'M' << 8 ) | 'B' ))
				{
					// Allocate memory for the file.
					DWORD nPackedDIBLen = file.GetFileSize( NULL ) - sizeof( BITMAPFILEHEADER );
					hDIB = ::GlobalAlloc( GMEM_FIXED, nPackedDIBLen );
					if ( hDIB )
					{
						// Read the entire file.
						file.Read( hDIB, nPackedDIBLen );

						// Cast pointers.
						LPBITMAPINFOHEADER      pbmiHeader = ( LPBITMAPINFOHEADER )hDIB;
						LPBITMAPINFO            pbmInfo    = ( LPBITMAPINFO )hDIB;

						// Determine the amount of colors present.
						nColors = pbmiHeader->biClrUsed ? pbmiHeader->biClrUsed : 1 << pbmiHeader->biBitCount;
						LPVOID lpDIBBits;

						// Get a pointer to the "real" bitmap bits.
						if ( pbmInfo->bmiHeader.biBitCount > 8 )
							lpDIBBits = ( LPVOID )(( LPDWORD )( pbmInfo->bmiColors + pbmInfo->bmiHeader.biClrUsed ) +
								    (( pbmInfo->bmiHeader.biCompression == BI_BITFIELDS ) ? 3 : 0 ));
						else
							lpDIBBits = ( LPVOID )( pbmInfo->bmiColors + nColors );

						// 256 colors or less. Higher color bitmaps do not carry
						// color information. These bitmaps are mapped via the
						// GetMappedBitmap() method.
						if ( nColors <= 256 && pColorMap )
						{
							COLORREF        cr;
							int             i, a;

							// Iterate colors.
							for ( i = 0 ; i < nColors; i++ )
							{
								// Create a COLORREF from the entry.
								cr = RGB( pbmInfo->bmiColors[ i ].rgbRed, pbmInfo->bmiColors[ i ].rgbGreen, pbmInfo->bmiColors[ i ].rgbBlue );

								// Iterate the color map.
								for ( a = 0; a < nMapSize; a++ )
								{
									// Map this color?
									if ( cr == pColorMap[ a ].from )
									{
										// Map the color.
										pbmInfo->bmiColors[ i ].rgbRed   = GetRValue( pColorMap[ a ].to );
										pbmInfo->bmiColors[ i ].rgbGreen = GetGValue( pColorMap[ a ].to );
										pbmInfo->bmiColors[ i ].rgbBlue  = GetBValue( pColorMap[ a ].to );
										break;
									}
								}
							}
						}

						// Create a display DC.
						ClsDC dc;
						if ( dc.CreateDC( _T( "DISPLAY" ), NULL, NULL, NULL ))
						{
							// Create the bitmap.
							hBitmap = CreateDIBitmap( dc,
										  pbmiHeader,
										  CBM_INIT,
										  lpDIBBits,
										  pbmInfo,
										  DIB_RGB_COLORS );
							// Release the DC.
							dc.DeleteDC();
						}
					}
					// Free the memory.
					::GlobalFree( hDIB );
				}
				// Attach the bitmap.
				if ( hBitmap )
				{
					// Attach the bitmap.
					Attach( hBitmap );

					// If the DIB does not have a colormap we map all
					// the bits by calling the GetMappedBitmap() method.
					if ( nColors > 256 && pColorMap && nMapSize )
					{
						// Create the mapped version and replace the current
						// bitmap by the mapped one.
						HBITMAP hMapped = GetMappedBitmap( pColorMap, nMapSize );
						if ( hMapped )
						{
							Delete();
							Attach( hMapped );
							return TRUE;
						}
						return FALSE;
					}
					return TRUE;
				}
				return FALSE;
			}
			catch( ClsFileException& )
			{
				if ( hDIB ) ::GlobalFree( hDIB );
				return FALSE;
			}
			catch( ClsMemoryException& )
			{
				return FALSE;
			}
		}
	}
	return FALSE;
}

// Create a bitmap.
BOOL ClsBitmap::CreateBitmap( int nWidth, int nHeight, UINT nPlanes, UINT nBitCount, const void *lpBits )
{
	// Attach the bitmap.
	Attach( ::CreateBitmap( nWidth, nHeight, nPlanes, nBitCount, lpBits ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a bitmap using a BITMAP structure.
BOOL ClsBitmap::CreateBitmapIndirect( const BITMAP *pBitmap )
{
	// Attach the bitmap.
	Attach( ::CreateBitmapIndirect( pBitmap ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a HDC compatible bitmap.
BOOL ClsBitmap::CreateCompatibleBitmap( HDC hDC, int nWidth, int nHeight )
{
	// Attach the bitmap.
	Attach( ::CreateCompatibleBitmap( hDC, nWidth, nHeight ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Create a mapped bitmap.
BOOL ClsBitmap::CreateMappedBitmap( int idBitmap, UINT nFlags, LPCOLORMAP pColorMap /* = NULL */, int nMapSize /* = 0 */ )
{
	// Attach the bitmap.
	Attach( ::CreateMappedBitmap( ClsGetResourceHandle(), idBitmap, nFlags, pColorMap, nMapSize ));
	return ( BOOL )( m_hGdiObject ? TRUE : FALSE );
}

// Get the BITMAP structure.
BOOL ClsBitmap::GetBitmap( BITMAP *pBitmap ) const
{
	// Get the information.
	return GetObject( sizeof( BITMAP ), ( LPVOID )pBitmap ) == sizeof( BITMAP ) ? TRUE : FALSE;
}

// Get DIBSECTION structure.
BOOL ClsBitmap::GetDIBSection( DIBSECTION *pDIBSection ) const
{
	// Get the information.
	return GetObject( sizeof( DIBSECTION ), ( LPVOID )pDIBSection ) == sizeof( DIBSECTION ) ? TRUE : FALSE;
}

// Return the object of the handle.
ClsBitmap *ClsBitmap::FromHandle( HBITMAP hBitmap )
{
	return ( ClsBitmap * )ClsGdiObject::FromHandle( hBitmap );
}

// Operator to return the bitmap handle.
ClsBitmap::operator HBITMAP() const
{
	// Return the handle.
	return ( HBITMAP )m_hGdiObject;
}

// Obtain a grayscaled version of the bitmap.
HBITMAP ClsBitmap::GetGrayscaledBitmap( COLORREF crException /* = CLR_NONE */, int nXPos /* = 0 */, int nYPos /* = 0 */ ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Preset result.
	BOOL bResult = FALSE;
	HBITMAP hGrayBM = NULL;

	// Create a device context.
	ClsDC dcSrc;;
	if ( dcSrc.CreateCompatibleDC( NULL ))
	{
		// Get source bitmap information.
		BITMAP bm;
		if ( GetBitmap( &bm ))
		{
			// Create a bitmap of the same size and color depth.
			hGrayBM = ::CreateBitmap( bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL );
			if ( hGrayBM )
			{
				// Grayscale the bitmap. Since using GetPixel()/SetPixel() is no way
				// near fast enough for bitmaps of any size we access the bitmap data
				// directly.
				// 
				// We do this by simply requesting the bitmap data in a 24bpp form using
				// GetDIBits(). This returns an array of RGBTRIPLE structures. We grayscale 
				// this data and use SetDIBits() to put it back into the bitmap.

				// Get the bitmap bits.
				BITMAPINFO bi;
				RGBTRIPLE *pBits = GetBitmapBits( dcSrc, bi, bm.bmWidthBytes * 8, bm.bmHeight );

				// OK?
				if ( pBits )
				{
					// Select the source bitmap.
					HGDIOBJ hOldSrc = dcSrc.SelectObject( m_hGdiObject );
					_ASSERT( hOldSrc );
					
					// Exception color by getting the color at a pixel
					// position?
					if ( crException == CLR_DEFAULT ) 
						crException = dcSrc.GetPixel( nXPos, nYPos );

					// Assign a temporary pointer that points to the first
					// RGBTRIPLE of the bitmap.
					RGBTRIPLE *pPixel = ( RGBTRIPLE * )pBits;

					// Iterate through all the RGBQUADs in the bitmap.
					for ( DWORD dwSize = 0; dwSize < ( DWORD )(( bm.bmWidthBytes * 8 ) * bm.bmHeight ); dwSize++ )
					{
						// Exception color?
						if ( crException == CLR_NONE || ( RGB( pPixel->rgbtRed, pPixel->rgbtGreen, pPixel->rgbtBlue ) != crException ))
							// Grayscale this pixel.
							pPixel->rgbtBlue = pPixel->rgbtGreen = pPixel->rgbtRed = ( BYTE )(( pPixel->rgbtRed * .299 ) + ( pPixel->rgbtGreen * .589 ) + ( pPixel->rgbtBlue * .114 ));
						// Next pixel.
						pPixel++;
					}
					
					// Set the new bitmap.
					if ( ::SetDIBits( dcSrc, hGrayBM, 0, bm.bmHeight, ( LPVOID )pBits, &bi, DIB_RGB_COLORS ))
						// OK
						bResult = TRUE;

					// Delete the buffer.
					delete pBits;
					
					// Restore old bitmap.
					dcSrc.SelectObject( hOldSrc );
				}
				// Destroy it if no successful.
				if ( bResult == FALSE ) 
					::DeleteObject( hGrayBM );
			}
		}
	}
	// Destroy DC
	if ( dcSrc.IsValid())    dcSrc.DeleteDC();
	return bResult == TRUE ? hGrayBM : NULL;
}

// Obtain a grayscaled version of the bitmap.
BOOL ClsBitmap::GetGrayscaledBitmap( ClsBitmap& bmDst, COLORREF crException /* = CLR_NONE */, int nXPos /* = 0 */, int nYPos /* = 0 */ ) const
{
	// Get grayscaled bitmap.
	HBITMAP hGrayBM = GetGrayscaledBitmap( crException, nXPos, nYPos );

	// OK?
	if ( hGrayBM ) 
	{
		// Attach it to the object.
		bmDst.Attach( hGrayBM );
		return TRUE;
	}
	return FALSE;
}

// Obtain a mask bitmap. A mask bitmap is a bitmap which has
// all pixels with the color "crColor" set to white and all
// other pixels set to black.
HBITMAP ClsBitmap::GetMaskBitmap( COLORREF crColor, int nXPos /* = 0 */, int nYPos /* = 0 */ ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Preset result.
	BOOL bResult = FALSE;
	HBITMAP hMaskBM = NULL;

	// Create the necessary device contexts.
	ClsDC dcSrc, dcDst;
	if ( dcSrc.CreateCompatibleDC( NULL ) &&
	     dcDst.CreateCompatibleDC( NULL ))
	{
		// Get source bitmap information.
		BITMAP bm;
		if ( GetBitmap( &bm ))
		{
			// Create a monochrome bitmap of the same size.
			hMaskBM = ::CreateBitmap( bm.bmWidth, bm.bmHeight, 1, 1, NULL );
			if ( hMaskBM )
			{
				// Select both the source and destination bitmaps.
				HGDIOBJ hOldSrc = dcSrc.SelectObject( m_hGdiObject );
				HGDIOBJ hOldDst = dcDst.SelectObject( hMaskBM );
				_ASSERT( hOldSrc && hOldDst );

				// Obtain the color used to create the mask bitmap.
				if ( crColor == CLR_DEFAULT )
					crColor = dcSrc.GetPixel( nXPos, nYPos );

				// Change the background color to the masked color.
				COLORREF crOldBkCol = dcSrc.SetBkColor( crColor );

				// Copy the source into the destination which creates
				// the mask.
				if ( dcDst.BitBlt( 0, 0, bm.bmWidth, bm.bmHeight, &dcSrc, 0, 0, SRCCOPY ))
					// Success...
					bResult = TRUE;
				
				// Restore background color.
				dcSrc.SetBkColor( crOldBkCol );

				// Restore old bitmaps.
				dcSrc.SelectObject( hOldSrc );
				dcDst.SelectObject( hOldDst );

				// Destroy it if no successful.
				if ( bResult == FALSE ) 
					::DeleteObject( hMaskBM );
			}
		}
	}
	// Destroy DCs
	if ( dcSrc.IsValid())    dcSrc.DeleteDC();
	if ( dcDst.IsValid())    dcDst.DeleteDC();
	return bResult == TRUE ? hMaskBM : NULL;
}

// Obtain a mask bitmap.
BOOL ClsBitmap::GetMaskBitmap( ClsBitmap& bmDst, COLORREF crColor, int nXPos /* = 0 */, int nYPos /* = 0 */ ) const
{
	// Get mask bitmap.
	HBITMAP hMaskBM = GetMaskBitmap( crColor, nXPos, nYPos );

	// OK?
	if ( hMaskBM ) 
	{
		// Attach it to the object.
		bmDst.Attach( hMaskBM );
		return TRUE;
	}
	return FALSE;
}

// Obtain a color-mapped version of the bitmap.
HBITMAP ClsBitmap::GetMappedBitmap( LPCOLORMAP pColorMap, int nMapSize ) const
{
	_ASSERT_VALID( m_hGdiObject ); // Must be valid.

	// Preset result.
	BOOL bResult = FALSE;
	HBITMAP hMapBM = NULL;

	// Create a device context.
	ClsDC dcSrc;
	if ( dcSrc.CreateCompatibleDC( NULL ))
	{
		// Get source bitmap information.
		BITMAP bm;
		if ( GetBitmap( &bm ))
		{
			// Create a bitmap of the same size and color depth.
			hMapBM = ::CreateBitmap( bm.bmWidth, bm.bmHeight, bm.bmPlanes, bm.bmBitsPixel, NULL );
			if ( hMapBM )
			{
				// Anything to map?
				if ( pColorMap && nMapSize )
				{
					// Obtain bits.
					BITMAPINFO bi;
					RGBTRIPLE *pBits = GetBitmapBits( dcSrc, bi, bm.bmWidthBytes * 8, bm.bmHeight );

					// OK?
					if ( pBits )
					{
						// Assign a temporary pointer that points to the first
						// RGBTRIPLE of the bitmap.
						RGBTRIPLE *pPixel = ( RGBTRIPLE * )pBits;

						// Iterate through all the RGBQUADs in the bitmap.
						COLORREF crSrc;
						for ( DWORD dwSize = 0; dwSize < ( DWORD )(( bm.bmWidthBytes * 8 ) * bm.bmHeight ) - 1; dwSize++ )
						{
							// Build COLORREF.
							crSrc = RGB( pPixel->rgbtRed, pPixel->rgbtGreen, pPixel->rgbtBlue );

							// Iterate color map.
							for ( int a = 0; a < nMapSize; a++ )
							{
								// Is this a color in the map?
								if ( crSrc == pColorMap[ a ].from )
								{
									// Map the color to the requested color.
									pPixel->rgbtRed   = GetRValue( pColorMap[ a ].to );
									pPixel->rgbtGreen = GetGValue( pColorMap[ a ].to );
									pPixel->rgbtBlue  = GetBValue( pColorMap[ a ].to );
									break;
								}
							}
							// Next pixel.
							pPixel++;
						}

						// Set the new bitmap.
						if ( ::SetDIBits( dcSrc, hMapBM, 0, bm.bmHeight, ( LPVOID )pBits, &bi, DIB_RGB_COLORS ))
							// OK
							bResult = TRUE;

						// Delete the buffer.
						delete pBits;
					}
				}
				else
					bResult = TRUE;

				// Destroy it if no successful.
				if ( bResult == FALSE ) 
					::DeleteObject( hMapBM );
			}
		}
	}
	// Destroy DCs
	if ( dcSrc.IsValid())    dcSrc.DeleteDC();
	return bResult == TRUE ? hMapBM : NULL;
}

// Obtain a color mapped version of the bitmap.
BOOL ClsBitmap::GetMappedBitmap( ClsBitmap& bmDst, LPCOLORMAP pColorMap, int nMapSize ) const
{
	// Get mapped bitmap.
	HBITMAP hMapBM = GetMappedBitmap( pColorMap, nMapSize );

	// OK?
	if ( hMapBM ) 
	{
		// Attach it to the object.
		bmDst.Attach( hMapBM );
		return TRUE;
	}
	return FALSE;
}

// Helper which returns a 24bpp version of the bitmap bits.
RGBTRIPLE *ClsBitmap::GetBitmapBits( HDC hDC, BITMAPINFO& bmi, int nWidth, int nHeight ) const
{
	_ASSERT( nWidth > 0 && nHeight > 0 ); // Must be true.

	// Preset result.
	RGBTRIPLE *pBits = NULL;

	// Catch memory problems...
	try
	{
		// Allocate an array of RGBTRIPLE structures to hold the
		// bitmap bits.
		pBits = new RGBTRIPLE[ nWidth * nHeight ];
		
		// Setup the BITMAPINFOHEADER structure for the
		// GetDIBits() API so that it returns a 24bpp
		// bitmap.
		memset( &bmi, 0, sizeof( BITMAPINFO ));
		bmi.bmiHeader.biSize	    = sizeof( bmi.bmiHeader );
		bmi.bmiHeader.biBitCount    = 24;
		bmi.bmiHeader.biWidth	    = nWidth;
		bmi.bmiHeader.biHeight	    = nHeight;
		bmi.bmiHeader.biPlanes	    = 1;
		bmi.bmiHeader.biCompression = BI_RGB;

		// Get the bits.
		if ( ::GetDIBits( hDC, ( HBITMAP )m_hGdiObject, 0, nHeight, ( LPVOID )pBits, &bmi, DIB_RGB_COLORS ))
			return pBits;
	}
	catch( ClsMemoryException& )
	{
	}
	return NULL;
}
