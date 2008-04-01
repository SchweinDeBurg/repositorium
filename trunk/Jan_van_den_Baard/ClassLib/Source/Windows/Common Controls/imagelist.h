#ifndef _IMAGELIST_H_
#define _IMAGELIST_H_
//
// imagelist.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../application.h"
#include "../window.h"
#include "../../coords/size.h"
#include "../../coords/point.h"

// A wrapper for HIMAGELIST.
class ClsImageList
{
public:
	// Default construction.
	ClsImageList() 
	{
		// Defailt.
		m_hImages = NULL;
	}

	// Construction from a handle.
	ClsImageList( HIMAGELIST hImages, BOOL bAttach = FALSE )
	{ 
		// Default.
		m_hImages = NULL;

		// Attach or copy?
		if ( bAttach ) m_hImages = hImages; 
		else if ( hImages ) m_hImages = ImageList_Duplicate( hImages ); 
	}

	// Copy constructor.
	ClsImageList( const ClsImageList& ims ) 
	{ 
		// default and duplicate.
		m_hImages = NULL;
		if ( ims.m_hImages ) m_hImages = ims.Duplicate(); 
	}
	
	// Destruction.
	virtual ~ClsImageList()
	{
		// Destroy?
		if ( m_hImages ) Destroy();
	}

	// Attach a handle, return the previous handle.
	HIMAGELIST Attach( HIMAGELIST hImages )
	{
		// Save old list.
		HIMAGELIST hRC = m_hImages;
		m_hImages = hImages;
		return hRC;
	}

	// Detach image list.
	HIMAGELIST Detach()
	{
		// Save list handle.
		HIMAGELIST hRC = m_hImages;
		m_hImages = NULL;
		return hRC;
	}

	// Implementation.
	inline int Add( HBITMAP hbmImage, HBITMAP hbmMask )
	{ _ASSERT_VALID( m_hImages ); return ImageList_Add( m_hImages, hbmImage, hbmMask );	}
	inline int AddIcon( HICON hicon )
	{ _ASSERT_VALID( m_hImages ); return ImageList_AddIcon( m_hImages, hicon ); }
	inline int AddMasked( HBITMAP hbmImage, COLORREF crMask )
	{ _ASSERT_VALID( m_hImages ); return ImageList_AddMasked( m_hImages, hbmImage, crMask ); }
	inline BOOL BeginDrag( int iTrack, int dxHotspot, int dyHotspot ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_BeginDrag( m_hImages, iTrack, dxHotspot, dyHotspot ); }	
	inline BOOL Copy( int iDst, int iSrc, UINT uFlags )
	{ _ASSERT_VALID( m_hImages ); return ImageList_Copy( m_hImages, iDst, m_hImages, iSrc, uFlags ); }
	inline BOOL Create( int cx, int cy, UINT flags, int cInitial, int cGrow	)
	{ _ASSERT( m_hImages == NULL ); m_hImages = ImageList_Create( cx, cy, flags, cInitial, cGrow ); return ( BOOL )( m_hImages ? TRUE : FALSE ); }
	inline void Destroy()
	{ _ASSERT_VALID( m_hImages ); ImageList_Destroy( m_hImages ); m_hImages = NULL; };
	inline BOOL Draw( int i, HDC hdcDst, int x, int y, UINT fStyle ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_Draw( m_hImages, i, hdcDst, x, y, fStyle ); }	
	inline BOOL DrawEx( int i, HDC hdcDst, int x, int y, int dx, int dy, COLORREF rgbBk, COLORREF rgbFg, UINT fStyle ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_DrawEx( m_hImages, i, hdcDst, x, y, dx, dy, rgbBk, rgbFg, fStyle );	}
	inline BOOL DrawIndirect( IMAGELISTDRAWPARAMS* pimldp ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_DrawIndirect( pimldp); }
	inline HIMAGELIST Duplicate() const
	{ _ASSERT_VALID( m_hImages ); return ImageList_Duplicate( m_hImages ); }
	inline HICON ExtractIcon( int i, HINSTANCE hinst = NULL ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_ExtractIcon( hinst ? hinst : ClsGetApp()->GetResourceHandle(), m_hImages, i ); }
	inline COLORREF GetBkColor() const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetBkColor( m_hImages ); }
	inline HICON GetIcon( int i, UINT flags ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetIcon( m_hImages, i, flags ); }
	inline BOOL GetIconSize( int FAR *cx, int FAR *cy ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetIconSize( m_hImages, cx, cy ); }
	inline BOOL GetIconSize( int& cx, int& cy ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetIconSize( m_hImages, &cx, &cy ); }
	inline BOOL GetIconSize( LPSIZE pSize ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetIconSize( m_hImages, ( LPINT )&pSize->cx, ( LPINT )&pSize->cy ); }
	inline BOOL GetIconSize( ClsSize& sz ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetIconSize( m_hImages, &sz.CX(), &sz.CY()); }
	inline int GetImageCount() const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetImageCount( m_hImages ); }
	inline BOOL GetImageInfo( int i, IMAGEINFO FAR *pImageInfo ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_GetImageInfo( m_hImages, i, pImageInfo ); }
	inline BOOL LoadBitmap( LPCTSTR lpbmp, int cx, int cGrow, COLORREF crMask, HINSTANCE hinst = NULL )
	{ _ASSERT( m_hImages == NULL ); m_hImages = ImageList_LoadBitmap( hinst ? hinst : ClsGetApp()->GetResourceHandle(), lpbmp, cx, cGrow, crMask ); return ( BOOL )( m_hImages ? TRUE : FALSE ); }
	inline BOOL LoadImage( LPCTSTR lpbmp, int cx, int cGrow, COLORREF crMask, UINT uType, UINT uFlags, HINSTANCE hinst = NULL )
	{ _ASSERT( m_hImages == NULL ); m_hImages = ImageList_LoadImage( hinst ? hinst : ClsGetApp()->GetResourceHandle(), lpbmp, cx, cGrow, crMask, uType, uFlags ); return ( BOOL )( m_hImages ? TRUE : FALSE ); }
	inline BOOL Merge( HIMAGELIST himl1, int i1, HIMAGELIST himl2, int i2, int dx, int dy )
	{ _ASSERT( m_hImages == NULL ); m_hImages = ImageList_Merge( himl1, i1, himl2, i2, dx, dy ); return ( BOOL )( m_hImages ? TRUE : FALSE ); }
	inline BOOL Read( LPSTREAM pstm )
	{ _ASSERT( m_hImages == NULL ); m_hImages = ImageList_Read( pstm ); return ( BOOL )( m_hImages ? TRUE : FALSE ); }
	inline BOOL Remove( int i )
	{ _ASSERT_VALID( m_hImages ); return ImageList_Remove( m_hImages, i ); }
	inline BOOL RemoveAll()
	{ _ASSERT_VALID( m_hImages ); return ImageList_RemoveAll( m_hImages ); }
	inline BOOL Replace( int i, HBITMAP hbmImage, HBITMAP hbmMask )
	{ _ASSERT_VALID( m_hImages ); return ImageList_Replace( m_hImages, i, hbmImage, hbmMask ); }
	inline int ReplaceIcon( int i, HICON hicon )
	{ _ASSERT_VALID( m_hImages ); return ImageList_ReplaceIcon( m_hImages, i, hicon ); }
	inline COLORREF SetBkColor( COLORREF clrBk )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetBkColor( m_hImages, clrBk ); }
	inline BOOL SetDragCursorImage( int iDrag, int dxHotspot, int dyHotspot )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetDragCursorImage( m_hImages, iDrag, dxHotspot, dyHotspot ); }
	inline BOOL SetDragCursorImage( int iDrag, const LPPOINT pt )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetDragCursorImage( m_hImages, iDrag, pt->x, pt->y ); }
	inline BOOL SetIconSize( int cx, int cy )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetIconSize( m_hImages, cx, cy ); }
	inline BOOL SetIconSize( const LPSIZE sz )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetIconSize( m_hImages, sz->cx, sz->cy ); }
	inline BOOL SetIconSize( const ClsSize& sz )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetIconSize( m_hImages, sz.CX(), sz.CY()); }
	inline BOOL SetImageCount( UINT uNewCount )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetImageCount( m_hImages, uNewCount ); }
	inline BOOL SetOverlayImage( int iImage, int iOverlay )
	{ _ASSERT_VALID( m_hImages ); return ImageList_SetOverlayImage( m_hImages, iImage, iOverlay ); }
	inline BOOL Write( LPSTREAM pstm ) const
	{ _ASSERT_VALID( m_hImages ); return ImageList_Write( m_hImages, pstm ); }

	// Static members.
	static BOOL DragEnter( HWND hwndLock, int x, int y )
	{ return ImageList_DragEnter( hwndLock, x, y ); }
	static BOOL DragEnter( ClsWindow *pLock, int x, int y )
	{ _ASSERT_VALID( pLock ); return ImageList_DragEnter( pLock->GetSafeHWND(), x, y ); }
	static BOOL DragLeave( HWND hwndLock )
	{ return ImageList_DragLeave( hwndLock ); }
	static BOOL DragLeave( ClsWindow *pLock )
	{ _ASSERT_VALID( pLock ); return ImageList_DragLeave( pLock->GetSafeHWND()); }
	static BOOL DragMove( int x, int y )
	{ return ImageList_DragMove( x, y ); }
	static BOOL DragShowNolock( BOOL fShow )
	{ return ImageList_DragShowNolock( fShow ); }
	static void EndDrag()
	{ ImageList_EndDrag(); }
	static HIMAGELIST GetDragImage( POINT FAR *ppt, POINT FAR *pptHotspot )
	{ return ImageList_GetDragImage( ppt, pptHotspot ); }

	// Operators.
	const ClsImageList& operator=( const ClsImageList& ims )
	{ _ASSERT( m_hImages == NULL ); if ( ims.m_hImages ) m_hImages = ims.Duplicate(); return *this; }
	const ClsImageList& operator=( const HIMAGELIST himl )
	{ _ASSERT( m_hImages == NULL ); if ( himl ) m_hImages = ImageList_Duplicate( himl ); return *this; }
	inline operator HIMAGELIST() { return m_hImages; }

protected:
	// Data.
	HIMAGELIST	m_hImages;
};

#endif // _IMAGELIST_H_