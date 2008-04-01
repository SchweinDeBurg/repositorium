#ifndef _DC_H_
#define _DC_H_
//
// dc.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../collectors/linkedlist.h"
#include "../windows/window.h"
#include "bitmap.h"
#include "brush.h"
#include "font.h"
#include "palette.h"
#include "pen.h"
#include "rgn.h"
#include "../coords/point.h"
#include "../coords/size.h"
#include "../strings/string.h"

// A simple HDC wrapper class.
class ClsDC : public ClsNode
{
public:
	// Constructors.
	ClsDC();
	ClsDC( HDC hDC );

	// Destructor.
	virtual ~ClsDC();

	// Implementation.
	BOOL Attach( HDC hDC );
	HDC Detach();
	BOOL CreateDC( LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const DEVMODE *lpInitData );
	BOOL CreateIC( LPCTSTR lpszDriverName, LPCTSTR lpszDeviceName, LPCTSTR lpszOutput, const DEVMODE *lpInitData );
	BOOL CreateCompatibleDC( ClsDC* pDC );
	BOOL DeleteDC();
	static ClsDC *FromHandle( HDC hDC );
	static void DeleteTempObjects();
	ClsBitmap *GetCurrentBitmap() const;
	ClsBrush *GetCurrentBrush() const;
	ClsFont *GetCurrentFont() const;
	ClsPalette *GetCurrentPalette() const;
	ClsPen *GetCurrentPen() const;
	ClsGdiObject *SelectObject( ClsGdiObject *pGdiObject );
	ClsPalette *SelectPalette( ClsPalette *pPalette, BOOL bForceBackground );
	
	HGDIOBJ SelectObject( HGDIOBJ hObject );
	int SaveDC();
	BOOL RestoreDC( int nSavedDC );
	BOOL ResetDC( const DEVMODE *lpDevMode );
	int GetDeviceCaps( int nIndex ) const;
	COLORREF GetNearestColor( COLORREF crColor ) const;
	UINT RealizePalette();
	void UpdateColors();
	COLORREF GetBkColor() const;
	COLORREF SetBkColor( COLORREF crColor );
	int GetBkMode() const;
	int SetBkMode( int nBkMode );
	int GetPolyFillMode() const;
	int SetPolyFillMode( int nPolyFillMode );
	int GetROP2() const;
	int SetROP2( int nDrawMode );
	int GetStretchBltMode() const;
	int SetStretchBltMode( int nStretchMode );
	COLORREF GetTextColor() const;
	COLORREF SetTextColor( COLORREF crColor );
	BOOL GetColorAdjustment( LPCOLORADJUSTMENT lpColorAdjust ) const;
	BOOL SetColorAdjustment( const COLORADJUSTMENT *lpColorAdjust );
	int GetMapMode() const;
	int SetMapMode( int nMapMode );
	ClsPoint GetViewportOrg() const;
	ClsPoint SetViewportOrg( int x, int y );
	ClsPoint SetViewportOrg( POINT point );
	ClsPoint OffsetViewportOrg( int nWidth, int nHeight );
	ClsSize GetViewportExt() const;
	ClsSize SetViewportExt( int cx, int cy );
	ClsSize SetViewportExt( SIZE size );
	ClsSize ScaleViewportExt( int xNum, int xDenom, int yNum, int yDenom );
	ClsPoint GetWindowOrg() const;
	ClsPoint SetWindowOrg( int x, int y );
	ClsPoint SetWindowOrg( POINT point );
	ClsPoint OffsetWindowOrg( int nWidth, int nHeight );
	ClsSize GetWindowExt() const;
	ClsSize SetWindowExt( int cx, int cy );
	ClsSize SetWindowExt( SIZE size );
	ClsSize ScaleWindowExt( int xNum, int xDenom, int yNum, int yDenom );
	BOOL FillRgn( ClsRgn *pRgn, ClsBrush *pBrush );
	BOOL FrameRgn( ClsRgn *pRgn, ClsBrush *pBrush, int nWidth, int nHeight );
	BOOL InvertRgn( ClsRgn *pRgn );
	BOOL PaintRgn( ClsRgn *pRgn );
	UINT SetBoundsRect( LPCRECT lpRectBounds, UINT flags );
	UINT GetBoundsRect( LPRECT lpRectBounds, UINT flags );
	int GetClipBox( LPRECT lpRect ) const;
	int SelectClipRgn( ClsRgn *pRgn );
	int SelectClipRgn( ClsRgn *pRgn, int nMode );
	int ExcludeClipRect( int x1, int y1, int x2, int y2 );
	int ExcludeClipRect( LPCRECT lpRect );
	int ExcludeUpdateRgn( ClsWindow *pWnd );
	int IntersectClipRect( int x1, int y1, int x2, int y2 );
	int IntersectClipRect( LPCRECT lpRect );
	int OffsetClipRgn( int x, int y );
	int OffsetClipRgn( SIZE size );
	BOOL PtVisible( int x, int y ) const;
	BOOL PtVisible( POINT point ) const;
	BOOL RectVisible( LPCRECT lpRect ) const;
	ClsPoint GetCurrentPosition() const;
	ClsPoint MoveTo( int x, int y );
	ClsPoint MoveTo( POINT point );
	BOOL LineTo( int x, int y );
	BOOL LineTo( POINT point );
	BOOL Arc( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 );
	BOOL Arc( LPCRECT lpRect, POINT ptStart, POINT ptEnd );
	BOOL ArcTo( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 );
	BOOL ArcTo( LPCRECT lpRect, POINT ptStart, POINT ptEnd );
	BOOL AngleArc( int x, int y, DWORD dwRadius, FLOAT fStartAngle, FLOAT fSweepAngle );
	int GetArcDirection() const;
	int SetArcDirection( int nArcDirection );
	BOOL PolyDraw( const POINT *lpPoints, const BYTE *lpTypes, int nCount );
	BOOL Polyline( LPPOINT lpPoints, int nCount );
	BOOL PolyPolyline( const POINT *lpPoints, const DWORD *lpPolyPoints, int nCount );
	BOOL PolylineTo( const POINT *lpPoints, int nCount );
	BOOL PolyBezier( const POINT *lpPoints, int nCount );
	BOOL PolyBezierTo( const POINT *lpPoints, int nCount );
	void FillSolidRect( LPCRECT lpRect, COLORREF crColor );
	void FillSolidRect( int x, int y, int cx, int cy, COLORREF crColor );
	void FillRect( LPCRECT lpRect, HBRUSH hBrush );
	void FillRect( LPCRECT lpRect, ClsBrush *pBrush );
	void FrameRect( LPCRECT lpRect, HBRUSH hBrush );
	void FrameRect( LPCRECT lpRect, ClsBrush *pBrush );
	void InvertRect( LPCRECT lpRect );
	BOOL DrawIcon( int x, int y, HICON hIcon );
	BOOL DrawIcon( POINT point, HICON hIcon );
	BOOL DrawIconEx( int xLeft, int yTop, HICON hIcon, int cxWidth, int cyWidth, UINT istepIfAniCur, HBRUSH hbrFlickerFreeDraw, UINT diFlags );
	BOOL DrawIconEx( ClsPoint point, HICON hIcon, ClsSize size, UINT istepIfAniCur, ClsBrush *pFlickerFreeDraw, UINT diFlags );
	BOOL DrawEdge( LPRECT lpRect, UINT nEdge, UINT nFlags );
	BOOL DrawFrameControl( LPRECT lpRect, UINT nType, UINT nState );
	BOOL DrawState( ClsPoint pt, ClsSize size, HBITMAP hBitmap, UINT nFlags, HBRUSH hBrush = NULL );
	BOOL DrawState( ClsPoint pt, ClsSize size, ClsBitmap *pBitmap, UINT nFlags, ClsBrush *pBrush = NULL );
	BOOL DrawState( ClsPoint pt, ClsSize size, HICON hIcon, UINT nFlags, HBRUSH hBrush = NULL );
	BOOL DrawState( ClsPoint pt, ClsSize size, HICON hIcon, UINT nFlags, ClsBrush *pBrush = NULL );
	BOOL DrawState( ClsPoint pt, ClsSize size, LPCTSTR lpszText, UINT nFlags, BOOL bPrefixText = TRUE, int nTextLen = 0, HBRUSH hBrush = NULL );
	BOOL DrawState( ClsPoint pt, ClsSize size, LPCTSTR lpszText, UINT nFlags, BOOL bPrefixText = TRUE, int nTextLen = 0, ClsBrush *pBrush = NULL );
	BOOL DrawState( ClsPoint pt, ClsSize size, DRAWSTATEPROC lpDrawProc, LPARAM lData, UINT nFlags, HBRUSH hBrush = NULL );
	BOOL DrawState( ClsPoint pt, ClsSize size, DRAWSTATEPROC lpDrawProc, LPARAM lData, UINT nFlags, ClsBrush *pBrush = NULL );
	BOOL Chord( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 );
	BOOL Chord( LPCRECT lpRect, POINT ptStart, POINT ptEnd );
	BOOL DrawFocusRect( LPCRECT lpRect );
	BOOL Ellipse( int x1, int y1, int x2, int y2 );
	BOOL Ellipse( LPCRECT lpRect );
	BOOL Pie( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 );
	BOOL Pie( LPCRECT lpRect, POINT ptStart, POINT ptEnd );
	BOOL Polygon( LPPOINT lpPoints, int nCount );
	BOOL PolyPolygon( LPPOINT lpPoints, LPINT lpPolyCounts, int nCount );
	BOOL Rectangle( int x1, int y1, int x2, int y2 );
	BOOL Rectangle( LPCRECT lpRect );
	BOOL RoundRect( int x1, int y1, int x2, int y2, int x3, int y3 );
	BOOL RoundRect( LPCRECT lpRect, POINT point );
	BOOL PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop );
	BOOL BitBlt( int x, int y, int nWidth, int nHeight, ClsDC *pSrcDC, int xSrc, int ySrc, DWORD dwRop );
	BOOL StretchBlt( int x, int y, int nWidth, int nHeight, ClsDC *pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop );
	COLORREF GetPixel( int x, int y ) const;
	COLORREF GetPixel( POINT point ) const;
	COLORREF SetPixel( int x, int y, COLORREF crColor );
	COLORREF SetPixel( POINT point, COLORREF crColor );
	BOOL SetPixelV(int x, int y, COLORREF crColor);
	BOOL SetPixelV( POINT point, COLORREF crColor );
	BOOL FloodFill( int x, int y, COLORREF crColor );
	BOOL ExtFloodFill( int x, int y, COLORREF crColor, UINT nFillType );
	BOOL MaskBlt( int x, int y, int nWidth, int nHeight, ClsDC *pSrcDC, int xSrc, int ySrc, ClsBitmap& maskBitmap, int xMask, int yMask, DWORD dwRop );
	BOOL PlgBlt( const LPPOINT lpPoint, ClsDC *pSrcDC, int xSrc, int ySrc, int nWidth, int nHeight, ClsBitmap& maskBitmap, int xMask, int yMask );
	BOOL TextOut( int x, int y, LPCTSTR lpszString, int nCount );
	BOOL TextOut( int x, int y, const ClsString& str );
	BOOL ExtTextOut( int x, int y, UINT nOptions, LPCRECT lpRect, LPCTSTR lpszString, UINT nCount, LPINT lpDxWidths );
	BOOL ExtTextOut( int x, int y, UINT nOptions, LPCRECT lpRect, const ClsString& str, LPINT lpDxWidths );
	ClsSize TabbedTextOut( int x, int y, LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin );
	ClsSize TabbedTextOut( int x, int y, const ClsString& str, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin );
	int DrawText( LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat );
	int DrawText( const ClsString& str, LPRECT lpRect, UINT nFormat );
	ClsSize GetTextExtent( LPCTSTR lpszString, int nCount ) const;
	ClsSize GetTextExtent( const ClsString& str ) const;
	ClsSize GetTextExtentExPoint( LPCTSTR lpszString, int nCount, int nMaxExtent, LPINT lpnFit, LPINT alpDx = NULL ) const;
	ClsSize GetTextExtentExPoint( const ClsString& str, int nMaxExtent, LPINT lpnFit, LPINT alpDx = NULL ) const;
	ClsSize GetTabbedTextExtent( LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions ) const;
	ClsSize GetTabbedTextExtent( const ClsString& str, int nTabPositions, LPINT lpnTabStopPositions ) const;
	BOOL GrayString( ClsBrush *pBrush, GRAYSTRINGPROC lpfnGrayProc, LPARAM lpData, int nCount, int x, int y, int nWidth, int nHeight );
	UINT GetTextAlign() const;
	UINT SetTextAlign( UINT nFlags );
	int GetTextFace( int nCount, LPTSTR lpszFacename ) const;
	int GetTextFace( ClsString& rString ) const;
	BOOL GetTextMetrics( LPTEXTMETRIC lpMetrics ) const;
	int SetTextJustification( int nBreakExtra, int nBreakCount );
	int GetTextCharacterExtra() const;
	int SetTextCharacterExtra( int nCharExtra );
	DWORD GetFontData( DWORD dwTable, DWORD dwOffset, LPVOID lpData, DWORD cbData ) const;
	int GetKerningPairs( int nPairs, LPKERNINGPAIR lpkrnpair ) const;
	UINT GetOutlineTextMetrics( UINT cbData, LPOUTLINETEXTMETRIC lpotm ) const;
	DWORD GetGlyphOutline( UINT nChar, UINT nFormat, LPGLYPHMETRICS lpgm, DWORD cbBuffer, LPVOID lpBuffer, const MAT2 FAR* lpmat2 ) const;
	BOOL GetCharABCWidths( UINT nFirstChar, UINT nLastChar, LPABC lpabc ) const;
	BOOL GetCharABCWidths( UINT nFirstChar, UINT nLastChar, LPABCFLOAT lpABCF ) const;
	BOOL GetCharWidth( UINT nFirstChar, UINT nLastChar, LPINT lpBuffer ) const;
	BOOL GetCharWidth( UINT nFirstChar, UINT nLastChar, float *lpFloatBuffer ) const;
	DWORD SetMapperFlags( DWORD dwFlag );
	int Escape( int nEscape, int nCount, LPCSTR lpszInData, LPVOID lpOutData );
	int ExtEscape( int nEscape, int nInputSize, LPCSTR lpszInputData, int nOutputSize, LPSTR lpszOutputData );
	int DrawEscape( int nEscape, int nInputSize, LPCSTR lpszInputData );
	int StartDoc( LPDOCINFO lpDocInfo );
	int StartPage();
	int EndPage();
	int SetAbortProc( ABORTPROC lpfn );
	int AbortDoc();
	int EndDoc();
	BOOL ScrollDC( int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip, ClsRgn *pRgnUpdate, LPRECT lpRectUpdate );
	BOOL PlayMetaFile( HMETAFILE hMF );
	BOOL PlayMetaFile( HENHMETAFILE hEnhMetaFile, LPCRECT lpBounds );
	BOOL AddMetaFileComment( UINT nDataSize, const BYTE *pCommentData );
	BOOL AbortPath();
	BOOL BeginPath();
	BOOL CloseFigure();
	BOOL EndPath();
	BOOL FillPath();
	BOOL FlattenPath();
	float GetMiterLimit() const;
	int GetPath( LPPOINT lpPoints, LPBYTE lpTypes, int nCount ) const;
	BOOL SelectClipPath( int nMode );
	float SetMiterLimit( float fMiterLimit );
	BOOL StrokeAndFillPath();
	BOOL StrokePath();
	BOOL WidenPath();
        BOOL GradientFill( TRIVERTEX* pVertices, ULONG nVertices, void* pMesh, ULONG nMeshElements,  DWORD dwMode );
	void OutlinedRectangle( LPCRECT pRect, COLORREF crOuter, COLORREF crInner );
	BOOL IsValid() const;

	// Operator overloads.
	inline operator HDC() const { return m_hDC; }
protected:
	// Data
	HDC	m_hDC;
};

inline ClsGdiObject *ClsDC::SelectObject( ClsGdiObject *pGdiObject )	{ _ASSERT_VALID( m_hDC ); return ClsGdiObject::FromHandle( ::SelectObject( m_hDC, pGdiObject ? ( HGDIOBJ )*pGdiObject : NULL )); }
inline HGDIOBJ ClsDC::SelectObject( HGDIOBJ hGdiObject )		{ _ASSERT_VALID( m_hDC ); return ::SelectObject( m_hDC, hGdiObject );}
inline int ClsDC::SaveDC()						{ _ASSERT_VALID( m_hDC ); return ::SaveDC( m_hDC ); }
inline BOOL ClsDC::RestoreDC( int nSavedDC )				{ _ASSERT_VALID( m_hDC ); return ::RestoreDC( m_hDC, nSavedDC ); }
inline BOOL ClsDC::ResetDC( const DEVMODE *lpDevMode )			{ _ASSERT_VALID( m_hDC ); return ( BOOL )( ::ResetDC( m_hDC, lpDevMode ) ? TRUE : FALSE ); }
inline int ClsDC::GetDeviceCaps( int nIndex ) const			{ _ASSERT_VALID( m_hDC ); return ::GetDeviceCaps( m_hDC, nIndex ); }
inline COLORREF ClsDC::GetNearestColor( COLORREF crColor ) const	{ _ASSERT_VALID( m_hDC ); return ::GetNearestColor( m_hDC, crColor ); }
inline UINT ClsDC::RealizePalette()					{ _ASSERT_VALID( m_hDC ); return ::RealizePalette( m_hDC ); }
inline void ClsDC::UpdateColors()					{ _ASSERT_VALID( m_hDC ); ::UpdateColors( m_hDC ); }
inline COLORREF ClsDC::GetBkColor() const				{ _ASSERT_VALID( m_hDC ); return ::GetBkColor( m_hDC ); }
inline COLORREF ClsDC::SetBkColor( COLORREF crColor )			{ _ASSERT_VALID( m_hDC ); return ::SetBkColor( m_hDC, crColor ); }
inline int ClsDC::GetBkMode() const					{ _ASSERT_VALID( m_hDC ); return ::GetBkMode( m_hDC ); }
inline int ClsDC::SetBkMode( int nBkMode )				{ _ASSERT_VALID( m_hDC ); return ::SetBkMode( m_hDC, nBkMode ); }
inline int ClsDC::GetPolyFillMode() const				{ _ASSERT_VALID( m_hDC ); return ::GetPolyFillMode( m_hDC ); }
inline int ClsDC::SetPolyFillMode( int nPolyFillMode )			{ _ASSERT_VALID( m_hDC ); return ::SetPolyFillMode( m_hDC, nPolyFillMode ); }
inline int ClsDC::GetROP2() const					{ _ASSERT_VALID( m_hDC ); return ::GetROP2( m_hDC ); }
inline int ClsDC::SetROP2( int nDrawMode )				{ _ASSERT_VALID( m_hDC ); return ::SetROP2( m_hDC, nDrawMode ); }
inline int ClsDC::GetStretchBltMode() const				{ _ASSERT_VALID( m_hDC ); return ::GetStretchBltMode( m_hDC ); }
inline int ClsDC::SetStretchBltMode( int nStretchMode )			{ _ASSERT_VALID( m_hDC ); return ::SetStretchBltMode( m_hDC, nStretchMode ); }
inline COLORREF ClsDC::GetTextColor() const				{ _ASSERT_VALID( m_hDC ); return ::GetTextColor( m_hDC ); }
inline COLORREF ClsDC::SetTextColor( COLORREF crColor )			{ _ASSERT_VALID( m_hDC ); return ::SetTextColor( m_hDC, crColor ); }
inline BOOL ClsDC::GetColorAdjustment( LPCOLORADJUSTMENT lpColorAdjust ) const	{ _ASSERT_VALID( m_hDC ); return ::GetColorAdjustment( m_hDC, lpColorAdjust ); }
inline BOOL ClsDC::SetColorAdjustment( const COLORADJUSTMENT *lpColorAdjust )	{ _ASSERT_VALID( m_hDC ); return ::SetColorAdjustment( m_hDC, lpColorAdjust ); }
inline int ClsDC::GetMapMode() const					{ _ASSERT_VALID( m_hDC ); return ::GetMapMode( m_hDC ); }
inline int ClsDC::SetMapMode( int nMapMode )				{ _ASSERT_VALID( m_hDC ); return ::SetMapMode( m_hDC, nMapMode ); }
inline ClsPoint ClsDC::GetViewportOrg() const				{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::GetViewportOrgEx( m_hDC, pt );	return pt; }
inline ClsPoint ClsDC::SetViewportOrg( int x, int y )			{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::SetViewportOrgEx( m_hDC, x, y, pt ); return pt; }
inline ClsPoint ClsDC::SetViewportOrg( POINT point )			{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::SetViewportOrgEx( m_hDC, point.x, point.y, pt ); return pt; }
inline ClsPoint ClsDC::OffsetViewportOrg( int nWidth, int nHeight )	{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::OffsetViewportOrgEx( m_hDC, nWidth, nHeight, pt ); return pt; }
inline ClsSize ClsDC::GetViewportExt() const				{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetViewportExtEx( m_hDC, sz ); return sz; }
inline ClsSize ClsDC::SetViewportExt( int cx, int cy )			{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::SetViewportExtEx( m_hDC, cx, cy, sz ); return sz; }
inline ClsSize ClsDC::SetViewportExt( SIZE size )			{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::SetViewportExtEx( m_hDC, size.cx, size.cy, sz ); return sz; }
inline ClsSize ClsDC::ScaleViewportExt( int xNum, int xDenom, int yNum, int yDenom )	{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::ScaleViewportExtEx( m_hDC, xNum, xDenom, yNum, yDenom, sz ); return sz; }
inline ClsPoint ClsDC::GetWindowOrg() const				{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::GetWindowOrgEx( m_hDC, pt ); return pt; }
inline ClsPoint ClsDC::SetWindowOrg( int x, int y )			{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::SetWindowOrgEx( m_hDC, x, y, pt ); return pt; }
inline ClsPoint ClsDC::SetWindowOrg( POINT point )			{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::SetWindowOrgEx( m_hDC, point.x, point.y, pt ); return pt; }
inline ClsPoint ClsDC::OffsetWindowOrg( int nWidth, int nHeight )	{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::OffsetWindowOrgEx( m_hDC, nWidth, nHeight, pt ); return pt; }
inline ClsSize ClsDC::GetWindowExt() const				{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetWindowExtEx( m_hDC, sz ); return sz; }
inline ClsSize ClsDC::SetWindowExt( int cx, int cy )			{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::SetWindowExtEx( m_hDC, cx, cy, sz ); return sz; }
inline ClsSize ClsDC::SetWindowExt( SIZE size )				{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::SetWindowExtEx( m_hDC, size.cx, size.cy, sz ); return sz; }
inline ClsSize ClsDC::ScaleWindowExt( int xNum, int xDenom, int yNum, int yDenom )	{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::ScaleWindowExtEx( m_hDC, xNum, xDenom, yNum, yDenom, sz ); return sz; }
inline BOOL ClsDC::FillRgn( ClsRgn *pRgn, ClsBrush *pBrush )		{ _ASSERT_VALID( m_hDC ); return ::FillRgn( m_hDC, *pRgn, *pBrush ); }
inline BOOL ClsDC::FrameRgn( ClsRgn *pRgn, ClsBrush *pBrush, int nWidth, int nHeight )	{ _ASSERT_VALID( m_hDC ); return ::FrameRgn( m_hDC, *pRgn, *pBrush, nWidth, nHeight ); }
inline BOOL ClsDC::InvertRgn( ClsRgn *pRgn )				{ _ASSERT_VALID( m_hDC ); return ::InvertRgn( m_hDC, *pRgn ); }
inline BOOL ClsDC::PaintRgn( ClsRgn *pRgn )				{ _ASSERT_VALID( m_hDC ); return ::PaintRgn( m_hDC, *pRgn ); }
inline UINT ClsDC::SetBoundsRect( LPCRECT lpRectBounds, UINT flags )	{ _ASSERT_VALID( m_hDC ); return ::SetBoundsRect( m_hDC, lpRectBounds, flags ); }
inline UINT ClsDC::GetBoundsRect( LPRECT lpRectBounds, UINT flags )	{ _ASSERT_VALID( m_hDC ); return ::GetBoundsRect( m_hDC, lpRectBounds, flags ); }
inline int ClsDC::GetClipBox( LPRECT lpRect ) const			{ _ASSERT_VALID( m_hDC ); return ::GetClipBox( m_hDC, lpRect ); }
inline int ClsDC::SelectClipRgn( ClsRgn *pRgn )				{ _ASSERT_VALID( m_hDC ); return ::SelectClipRgn( m_hDC, *pRgn ); }
inline int ClsDC::SelectClipRgn( ClsRgn *pRgn, int nMode )		{ _ASSERT_VALID( m_hDC ); return ::ExtSelectClipRgn( m_hDC, *pRgn, nMode ); }
inline int ClsDC::ExcludeClipRect( int x1, int y1, int x2, int y2 )	{ _ASSERT_VALID( m_hDC ); return ::ExcludeClipRect( m_hDC, x1, y1, x2, y2 ); }
inline int ClsDC::ExcludeClipRect( LPCRECT lpRect )			{ _ASSERT_VALID( m_hDC ); return ::ExcludeClipRect( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom ); }
inline int ClsDC::ExcludeUpdateRgn( ClsWindow *pWnd )			{ _ASSERT_VALID( m_hDC ); return ::ExcludeUpdateRgn( m_hDC, *pWnd ); }
inline int ClsDC::IntersectClipRect( int x1, int y1, int x2, int y2 )	{ _ASSERT_VALID( m_hDC ); return ::IntersectClipRect( m_hDC, x1, y1, x2, y2 ); }
inline int ClsDC::IntersectClipRect( LPCRECT lpRect )			{ _ASSERT_VALID( m_hDC ); return ::IntersectClipRect( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom ); }
inline int ClsDC::OffsetClipRgn( int x, int y )				{ _ASSERT_VALID( m_hDC ); return ::OffsetClipRgn( m_hDC, x, y ); }
inline int ClsDC::OffsetClipRgn( SIZE size )				{ _ASSERT_VALID( m_hDC ); return ::OffsetClipRgn( m_hDC, size.cx, size.cy ); }
inline BOOL ClsDC::PtVisible( int x, int y ) const			{ _ASSERT_VALID( m_hDC ); return ::PtVisible( m_hDC, x, y ); }
inline BOOL ClsDC::PtVisible( POINT point ) const			{ _ASSERT_VALID( m_hDC ); return ::PtVisible( m_hDC, point.x, point.y ); }
inline BOOL ClsDC::RectVisible( LPCRECT lpRect ) const			{ _ASSERT_VALID( m_hDC ); return ::RectVisible( m_hDC, lpRect ); }
inline ClsPoint ClsDC::GetCurrentPosition() const			{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::GetCurrentPositionEx( m_hDC, pt ); return pt; }
inline ClsPoint ClsDC::MoveTo( int x, int y )				{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::MoveToEx( m_hDC, x, y, pt ); return pt; }
inline ClsPoint ClsDC::MoveTo( POINT point )				{ _ASSERT_VALID( m_hDC ); ClsPoint pt; ::MoveToEx( m_hDC, point.x, point.y, pt ); return pt; }
inline BOOL ClsDC::LineTo( int x, int y )				{ _ASSERT_VALID( m_hDC ); return ::LineTo( m_hDC, x, y ); }
inline BOOL ClsDC::LineTo( POINT point )				{ _ASSERT_VALID( m_hDC ); return ::LineTo( m_hDC, point.x, point.y ); }
inline BOOL ClsDC::Arc( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )	{ _ASSERT_VALID( m_hDC ); return ::Arc( m_hDC, x1, y1, x2, y2, x3, y3, x4, y4 ); }
inline BOOL ClsDC::Arc( LPCRECT lpRect, POINT ptStart, POINT ptEnd )	{ _ASSERT_VALID( m_hDC ); return ::Arc( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y ); }
inline BOOL ClsDC::ArcTo( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )	{ _ASSERT_VALID( m_hDC ); return ::ArcTo( m_hDC, x1, y1, x2, y2, x3, y3, x4, y4 ); }
inline BOOL ClsDC::ArcTo( LPCRECT lpRect, POINT ptStart, POINT ptEnd )	{ _ASSERT_VALID( m_hDC ); return ::ArcTo( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y ); }
inline BOOL ClsDC::AngleArc( int x, int y, DWORD dwRadius, FLOAT fStartAngle, FLOAT fSweepAngle )	{ _ASSERT_VALID( m_hDC ); return ::AngleArc( m_hDC, x, y, dwRadius, fStartAngle, fSweepAngle ); }
inline int ClsDC::GetArcDirection() const				{ _ASSERT_VALID( m_hDC ); return ::GetArcDirection( m_hDC ); }
inline int ClsDC::SetArcDirection( int nArcDirection )			{ _ASSERT_VALID( m_hDC ); return ::SetArcDirection( m_hDC, nArcDirection ); }
inline BOOL ClsDC::PolyDraw( const POINT *lpPoints, const BYTE *lpTypes, int nCount )	{ _ASSERT_VALID( m_hDC ); return ::PolyDraw( m_hDC, lpPoints, lpTypes, nCount ); }
inline BOOL ClsDC::Polyline( LPPOINT lpPoints, int nCount )		{ _ASSERT_VALID( m_hDC ); return ::Polyline( m_hDC, lpPoints, nCount ); }
inline BOOL ClsDC::PolyPolyline( const POINT *lpPoints, const DWORD *lpPolyPoints, int nCount )		{ _ASSERT_VALID( m_hDC ); return ::PolyPolyline( m_hDC, lpPoints, lpPolyPoints, nCount ); }
inline BOOL ClsDC::PolylineTo( const POINT *lpPoints, int nCount )	{ _ASSERT_VALID( m_hDC ); return ::PolylineTo( m_hDC, lpPoints, nCount ); }
inline BOOL ClsDC::PolyBezier( const POINT *lpPoints, int nCount )	{ _ASSERT_VALID( m_hDC ); return ::PolyBezier( m_hDC, lpPoints, nCount ); }
inline BOOL ClsDC::PolyBezierTo( const POINT *lpPoints, int nCount )	{ _ASSERT_VALID( m_hDC ); return ::PolyBezierTo( m_hDC, lpPoints, nCount ); }
inline void ClsDC::FillRect( LPCRECT lpRect, HBRUSH hBrush )		{ _ASSERT_VALID( m_hDC ); ::FillRect( m_hDC, lpRect, hBrush ); }
inline void ClsDC::FillRect( LPCRECT lpRect, ClsBrush *pBrush )		{ _ASSERT_VALID( m_hDC ); ::FillRect( m_hDC, lpRect, *pBrush ); }
inline void ClsDC::FrameRect( LPCRECT lpRect, HBRUSH hBrush )		{ _ASSERT_VALID( m_hDC ); ::FrameRect( m_hDC, lpRect, hBrush ); }
inline void ClsDC::FrameRect( LPCRECT lpRect, ClsBrush *pBrush )	{ _ASSERT_VALID( m_hDC ); ::FrameRect( m_hDC, lpRect, *pBrush ); }
inline void ClsDC::InvertRect( LPCRECT lpRect )				{ _ASSERT_VALID( m_hDC ); ::InvertRect( m_hDC, lpRect ); }
inline BOOL ClsDC::DrawIcon( int x, int y, HICON hIcon )		{ _ASSERT_VALID( m_hDC ); return ::DrawIcon( m_hDC, x, y, hIcon ); }
inline BOOL ClsDC::DrawIcon( POINT point, HICON hIcon )			{ _ASSERT_VALID( m_hDC ); return ::DrawIcon( m_hDC, point.x, point.y, hIcon ); }
inline BOOL ClsDC::DrawIconEx( int xLeft, int yTop, HICON hIcon, int cxWidth, int cyWidth, UINT istepIfAniCur, HBRUSH hbrFlickerFreeDraw, UINT diFlags )	{ _ASSERT_VALID( m_hDC ); return ::DrawIconEx( m_hDC, xLeft, yTop, hIcon, cxWidth, cyWidth, istepIfAniCur, hbrFlickerFreeDraw, diFlags ); }
inline BOOL ClsDC::DrawIconEx( ClsPoint point, HICON hIcon, ClsSize size, UINT istepIfAniCur, ClsBrush *pFlickerFreeDraw, UINT diFlags )	{ _ASSERT_VALID( m_hDC ); return ::DrawIconEx( m_hDC, point.X(), point.Y(), hIcon, size.CX(), size.CY(), istepIfAniCur, pFlickerFreeDraw ? ( HBRUSH )*pFlickerFreeDraw : NULL, diFlags ); }
inline BOOL ClsDC::DrawEdge( LPRECT lpRect, UINT nEdge, UINT nFlags )	{ _ASSERT_VALID( m_hDC ); return ::DrawEdge( m_hDC, lpRect, nEdge, nFlags ); }
inline BOOL ClsDC::DrawFrameControl( LPRECT lpRect, UINT nType, UINT nState )	{ _ASSERT_VALID( m_hDC ); return ::DrawFrameControl( m_hDC, lpRect, nType, nState ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, HBITMAP hBitmap, UINT nFlags, HBRUSH hBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, hBrush, NULL, ( LPARAM )hBitmap, 0, pt.X(), pt.Y(), size.CX(), size.CY(), DST_BITMAP | nFlags ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, ClsBitmap *pBitmap, UINT nFlags, ClsBrush *pBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, pBrush ? ( HBRUSH )*pBrush : NULL, NULL, ( LPARAM )( HBITMAP )*pBitmap, 0, pt.X(), pt.Y(), size.CX(), size.CY(), DST_BITMAP | nFlags ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, HICON hIcon, UINT nFlags, HBRUSH hBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, hBrush, NULL, ( LPARAM )hIcon, 0, pt.X(), pt.Y(), size.CX(), size.CY(), DST_ICON | nFlags ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, HICON hIcon, UINT nFlags, ClsBrush *pBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, pBrush ? ( HBRUSH )*pBrush : NULL, NULL, ( LPARAM )hIcon, 0, pt.X(), pt.Y(), size.CX(), size.CY(), DST_ICON | nFlags ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, LPCTSTR lpszText, UINT nFlags, BOOL bPrefixText /* = TRUE */, int nTextLen /* = 0 */, HBRUSH hBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, hBrush, NULL, ( LPARAM )lpszText, nTextLen, pt.X(), pt.Y(), size.CX(), size.CY(), ( bPrefixText ? DST_PREFIXTEXT : DST_TEXT ) | nFlags ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, LPCTSTR lpszText, UINT nFlags, BOOL bPrefixText /* = TRUE */, int nTextLen /* = 0 */, ClsBrush *pBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, pBrush ? ( HBRUSH )*pBrush : NULL, NULL, ( LPARAM )lpszText, nTextLen, pt.X(), pt.Y(), size.CX(), size.CY(), ( bPrefixText ? DST_PREFIXTEXT : DST_TEXT ) | nFlags ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, DRAWSTATEPROC lpDrawProc, LPARAM lData, UINT nFlags, HBRUSH hBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, hBrush, lpDrawProc, lData, 0, pt.X(), pt.Y(), size.CX(), size.CY(), DST_COMPLEX | nFlags ); }
inline BOOL ClsDC::DrawState( ClsPoint pt, ClsSize size, DRAWSTATEPROC lpDrawProc, LPARAM lData, UINT nFlags, ClsBrush *pBrush /* = NULL */ )	{ _ASSERT_VALID( m_hDC ); return ::DrawState( m_hDC, pBrush ? ( HBRUSH )*pBrush : NULL, lpDrawProc, lData, 0, pt.X(), pt.Y(), size.CX(), size.CY(), DST_COMPLEX | nFlags ); }
inline BOOL ClsDC::Chord( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )	{ _ASSERT_VALID( m_hDC ); return ::Chord( m_hDC, x1, y1, x2, y2, x3, y3, x4, y4 ); }
inline BOOL ClsDC::Chord( LPCRECT lpRect, POINT ptStart, POINT ptEnd )	{ _ASSERT_VALID( m_hDC ); return ::Chord( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y ); }
inline BOOL ClsDC::DrawFocusRect( LPCRECT lpRect )			{ _ASSERT_VALID( m_hDC ); return ::DrawFocusRect( m_hDC, lpRect ); }
inline BOOL ClsDC::Ellipse( int x1, int y1, int x2, int y2 )		{ _ASSERT_VALID( m_hDC ); return ::Ellipse( m_hDC, x1, y1, x2, y2 ); }
inline BOOL ClsDC::Ellipse( LPCRECT lpRect )				{ _ASSERT_VALID( m_hDC ); return ::Ellipse( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom ); }
inline BOOL ClsDC::Pie( int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4 )	{ _ASSERT_VALID( m_hDC ); return ::Pie( m_hDC, x1, y1, x2, y2, x3, y3, x4, y4 ); }
inline BOOL ClsDC::Pie( LPCRECT lpRect, POINT ptStart, POINT ptEnd )	{ _ASSERT_VALID( m_hDC ); return ::Pie( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, ptStart.x, ptStart.y, ptEnd.x, ptEnd.y ); }
inline BOOL ClsDC::Polygon( LPPOINT lpPoints, int nCount )		{ _ASSERT_VALID( m_hDC ); return ::Polygon( m_hDC, lpPoints, nCount ); }
inline BOOL ClsDC::PolyPolygon( LPPOINT lpPoints, LPINT lpPolyCounts, int nCount )	{ _ASSERT_VALID( m_hDC ); return ::PolyPolygon( m_hDC, lpPoints, lpPolyCounts, nCount ); }
inline BOOL ClsDC::Rectangle( int x1, int y1, int x2, int y2 )		{ _ASSERT_VALID( m_hDC ); return ::Rectangle( m_hDC, x1, y1, x2, y2 ); }
inline BOOL ClsDC::Rectangle( LPCRECT lpRect )				{ _ASSERT_VALID( m_hDC ); return ::Rectangle( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom ); }
inline BOOL ClsDC::RoundRect( int x1, int y1, int x2, int y2, int x3, int y3 )	{ _ASSERT_VALID( m_hDC ); return ::RoundRect( m_hDC, x1, y1, x2, y2, x3, y3 ); }
inline BOOL ClsDC::RoundRect( LPCRECT lpRect, POINT point )		{ _ASSERT_VALID( m_hDC ); return ::RoundRect( m_hDC, lpRect->left, lpRect->top, lpRect->right, lpRect->bottom, point.x, point.y ); }
inline BOOL ClsDC::PatBlt( int x, int y, int nWidth, int nHeight, DWORD dwRop )	{ _ASSERT_VALID( m_hDC ); return ::PatBlt( m_hDC, x, y, nWidth, nHeight, dwRop ); }
inline BOOL ClsDC::BitBlt( int x, int y, int nWidth, int nHeight, ClsDC *pSrcDC, int xSrc, int ySrc, DWORD dwRop )	{ _ASSERT_VALID( m_hDC ); return ::BitBlt( m_hDC, x, y, nWidth, nHeight, *pSrcDC, xSrc, ySrc, dwRop ); }
inline BOOL ClsDC::StretchBlt( int x, int y, int nWidth, int nHeight, ClsDC *pSrcDC, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight, DWORD dwRop )	{ _ASSERT_VALID( m_hDC ); return ::StretchBlt( m_hDC, x, y, nWidth, nHeight, *pSrcDC, xSrc, ySrc, nSrcWidth, nSrcHeight, dwRop ); }
inline COLORREF ClsDC::GetPixel( int x, int y ) const			{ _ASSERT_VALID( m_hDC ); return ::GetPixel( m_hDC, x, y ); }
inline COLORREF ClsDC::GetPixel( POINT point ) const			{ _ASSERT_VALID( m_hDC ); return ::GetPixel( m_hDC, point.x, point.y ); }
inline COLORREF ClsDC::SetPixel( int x, int y, COLORREF crColor )	{ _ASSERT_VALID( m_hDC ); return ::SetPixel( m_hDC, x, y, crColor ); }
inline COLORREF ClsDC::SetPixel( POINT point, COLORREF crColor )	{ _ASSERT_VALID( m_hDC ); return ::SetPixel( m_hDC, point.x, point.y, crColor ); }
inline BOOL ClsDC::SetPixelV( int x, int y, COLORREF crColor)		{ _ASSERT_VALID( m_hDC ); return ::SetPixelV( m_hDC, x, y, crColor ); }
inline BOOL ClsDC::SetPixelV( POINT point, COLORREF crColor )		{ _ASSERT_VALID( m_hDC ); return ::SetPixelV( m_hDC, point.x, point.y, crColor ); }
inline BOOL ClsDC::FloodFill( int x, int y, COLORREF crColor )		{ _ASSERT_VALID( m_hDC ); return ::FloodFill( m_hDC, x, y, crColor ); }
inline BOOL ClsDC::ExtFloodFill( int x, int y, COLORREF crColor, UINT nFillType )	{ _ASSERT_VALID( m_hDC ); return ::ExtFloodFill( m_hDC, x, y, crColor, nFillType ); }
inline BOOL ClsDC::MaskBlt( int x, int y, int nWidth, int nHeight, ClsDC *pSrcDC, int xSrc, int ySrc, ClsBitmap& maskBitmap, int xMask, int yMask, DWORD dwRop )	{ _ASSERT_VALID( m_hDC ); return ::MaskBlt( m_hDC, x, y, nWidth, nHeight, *pSrcDC, xSrc, ySrc, maskBitmap, xMask, yMask, dwRop ); }
inline BOOL ClsDC::PlgBlt( const LPPOINT lpPoint, ClsDC *pSrcDC, int xSrc, int ySrc, int nWidth, int nHeight, ClsBitmap& maskBitmap, int xMask, int yMask )	{ _ASSERT_VALID( m_hDC ); return ::PlgBlt( m_hDC, lpPoint, *pSrcDC, xSrc, ySrc, nWidth, nHeight, maskBitmap, xMask, yMask ); }
inline BOOL ClsDC::TextOut( int x, int y, LPCTSTR lpszString, int nCount )	{ _ASSERT_VALID( m_hDC ); return ::TextOut( m_hDC, x, y, lpszString, nCount ); }
inline BOOL ClsDC::TextOut( int x, int y, const ClsString& str )		{ _ASSERT_VALID( m_hDC ); return ::TextOut( m_hDC, x, y, str, str.GetStringLength()); }
inline BOOL ClsDC::ExtTextOut( int x, int y, UINT nOptions, LPCRECT lpRect, LPCTSTR lpszString, UINT nCount, LPINT lpDxWidths )	{ _ASSERT_VALID( m_hDC ); return ::ExtTextOut( m_hDC, x, y, nOptions, lpRect, lpszString, nCount, lpDxWidths ); }
inline BOOL ClsDC::ExtTextOut( int x, int y, UINT nOptions, LPCRECT lpRect, const ClsString& str, LPINT lpDxWidths )	{ _ASSERT_VALID( m_hDC ); return ::ExtTextOut( m_hDC, x, y, nOptions, lpRect, str, str.GetStringLength(), lpDxWidths ); }
inline ClsSize ClsDC::TabbedTextOut( int x, int y, LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin )	{ _ASSERT_VALID( m_hDC ); LONG size = ::TabbedTextOut( m_hDC, x, y, lpszString, nCount, nTabPositions, lpnTabStopPositions, nTabOrigin ); return ClsSize( LOWORD( size ), HIWORD( size )); }
inline ClsSize ClsDC::TabbedTextOut( int x, int y, const ClsString& str, int nTabPositions, LPINT lpnTabStopPositions, int nTabOrigin )	{ _ASSERT_VALID( m_hDC ); LONG size = ::TabbedTextOut( m_hDC, x, y, str, str.GetStringLength(), nTabPositions, lpnTabStopPositions, nTabOrigin ); return ClsSize( LOWORD( size ), HIWORD( size )); }
inline int ClsDC::DrawText( LPCTSTR lpszString, int nCount, LPRECT lpRect, UINT nFormat )	{ _ASSERT_VALID( m_hDC ); return ::DrawText( m_hDC, lpszString, nCount, lpRect, nFormat ); }
inline int ClsDC::DrawText( const ClsString& str, LPRECT lpRect, UINT nFormat )	{ _ASSERT_VALID( m_hDC ); return ::DrawText( m_hDC, str, str.GetStringLength(), lpRect, nFormat ); }
inline ClsSize ClsDC::GetTextExtent( LPCTSTR lpszString, int nCount ) const	{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetTextExtentPoint32( m_hDC, lpszString, nCount, sz ); return sz; }
inline ClsSize ClsDC::GetTextExtent( const ClsString& str ) const	{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetTextExtentPoint32( m_hDC, str, str.GetStringLength(), sz ); return sz; }
inline ClsSize ClsDC::GetTextExtentExPoint( LPCTSTR lpszString, int nCount, int nMaxExtent, LPINT lpnFit, LPINT alpDx /* = NULL */ ) const	{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetTextExtentExPoint( m_hDC, lpszString, nCount, nMaxExtent, lpnFit, alpDx, sz ); return sz; }
inline ClsSize ClsDC::GetTextExtentExPoint( const ClsString& str, int nMaxExtent, LPINT lpnFit, LPINT alpDx /* = NULL */ ) const		{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetTextExtentExPoint( m_hDC, str, str.GetStringLength() * sizeof( TCHAR ), nMaxExtent, lpnFit, alpDx, sz ); return sz; }
inline ClsSize ClsDC::GetTabbedTextExtent( LPCTSTR lpszString, int nCount, int nTabPositions, LPINT lpnTabStopPositions ) const	{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetTabbedTextExtent( m_hDC, lpszString, nCount, nTabPositions, lpnTabStopPositions ); return sz; }
inline ClsSize ClsDC::GetTabbedTextExtent( const ClsString& str, int nTabPositions, LPINT lpnTabStopPositions ) const	{ _ASSERT_VALID( m_hDC ); ClsSize sz; ::GetTabbedTextExtent( m_hDC, str, str.GetStringLength(), nTabPositions, lpnTabStopPositions ); return sz; }
inline BOOL ClsDC::GrayString( ClsBrush *pBrush, GRAYSTRINGPROC lpfnGrayProc, LPARAM lpData, int nCount, int x, int y, int nWidth, int nHeight )	{ _ASSERT_VALID( m_hDC ); return ::GrayString( m_hDC, pBrush ? ( HBRUSH )*pBrush : NULL, lpfnGrayProc, lpData, nCount, x, y, nWidth, nHeight ); }
inline UINT ClsDC::GetTextAlign() const					{ _ASSERT_VALID( m_hDC ); return ::GetTextAlign( m_hDC ); }
inline UINT ClsDC::SetTextAlign( UINT nFlags )				{ _ASSERT_VALID( m_hDC ); return ::SetTextAlign( m_hDC, nFlags ); }
inline int ClsDC::GetTextFace( int nCount, LPTSTR lpszFacename ) const	{ _ASSERT_VALID( m_hDC ); return ::GetTextFace( m_hDC, nCount, lpszFacename ); }
inline int ClsDC::GetTextFace( ClsString& rString ) const		{ _ASSERT_VALID( m_hDC ); int nLen = ::GetTextFace( m_hDC, 0, NULL ); rString.AllocateBuffer( nLen ); return ::GetTextFace( m_hDC, nLen, rString ); }
inline BOOL ClsDC::GetTextMetrics( LPTEXTMETRIC lpMetrics ) const	{ _ASSERT_VALID( m_hDC ); return ::GetTextMetrics( m_hDC, lpMetrics ); }
inline int ClsDC::SetTextJustification( int nBreakExtra, int nBreakCount )	{ _ASSERT_VALID( m_hDC ); return ::SetTextJustification( m_hDC, nBreakExtra, nBreakCount ); }
inline int ClsDC::GetTextCharacterExtra() const				{ _ASSERT_VALID( m_hDC ); return ::GetTextCharacterExtra( m_hDC ); }
inline int ClsDC::SetTextCharacterExtra( int nCharExtra )		{ _ASSERT_VALID( m_hDC ); return ::SetTextCharacterExtra( m_hDC, nCharExtra ); }
inline DWORD ClsDC::GetFontData( DWORD dwTable, DWORD dwOffset, LPVOID lpData, DWORD cbData ) const	{ _ASSERT_VALID( m_hDC ); return ::GetFontData( m_hDC, dwTable, dwOffset, lpData, cbData ); }
inline int ClsDC::GetKerningPairs( int nPairs, LPKERNINGPAIR lpkrnpair ) const	{ _ASSERT_VALID( m_hDC ); return ::GetKerningPairs( m_hDC, nPairs, lpkrnpair ); }
inline UINT ClsDC::GetOutlineTextMetrics( UINT cbData, LPOUTLINETEXTMETRIC lpotm ) const	{ _ASSERT_VALID( m_hDC ); return ::GetOutlineTextMetrics( m_hDC, cbData, lpotm ); }
inline DWORD ClsDC::GetGlyphOutline( UINT nChar, UINT nFormat, LPGLYPHMETRICS lpgm, DWORD cbBuffer, LPVOID lpBuffer, const MAT2 FAR *lpmat2 ) const	{ _ASSERT_VALID( m_hDC ); return ::GetGlyphOutline( m_hDC, nChar, nFormat, lpgm, cbBuffer, lpBuffer, lpmat2 ); }
inline BOOL ClsDC::GetCharABCWidths( UINT nFirstChar, UINT nLastChar, LPABC lpabc ) const	{ _ASSERT_VALID( m_hDC ); return ::GetCharABCWidths( m_hDC, nFirstChar, nLastChar, lpabc ); }
inline BOOL ClsDC::GetCharABCWidths( UINT nFirstChar, UINT nLastChar, LPABCFLOAT lpABCF ) const	{ _ASSERT_VALID( m_hDC ); return ::GetCharABCWidthsFloat( m_hDC, nFirstChar, nLastChar, lpABCF ); }
inline BOOL ClsDC::GetCharWidth( UINT nFirstChar, UINT nLastChar, LPINT lpBuffer ) const	{ _ASSERT_VALID( m_hDC ); return ::GetCharWidth( m_hDC, nFirstChar, nLastChar, lpBuffer ); }
inline BOOL ClsDC::GetCharWidth( UINT nFirstChar, UINT nLastChar, float *lpFloatBuffer ) const	{ _ASSERT_VALID( m_hDC ); return ::GetCharWidthFloat( m_hDC, nFirstChar, nLastChar, lpFloatBuffer ); }
inline DWORD ClsDC::SetMapperFlags( DWORD dwFlag )			{ _ASSERT_VALID( m_hDC ); return ::SetMapperFlags( m_hDC, dwFlag ); }
inline int ClsDC::Escape( int nEscape, int nCount, LPCSTR lpszInData, LPVOID lpOutData )	{ _ASSERT_VALID( m_hDC ); return ::Escape( m_hDC, nEscape, nCount, lpszInData, lpOutData ); }
inline int ClsDC::ExtEscape( int nEscape, int nInputSize, LPCSTR lpszInputData, int nOutputSize, LPSTR lpszOutputData )	{ _ASSERT_VALID( m_hDC ); return ::ExtEscape( m_hDC, nEscape, nInputSize, lpszInputData, nOutputSize, lpszOutputData ); }
inline int ClsDC::DrawEscape( int nEscape, int nInputSize, LPCSTR lpszInputData )	{ _ASSERT_VALID( m_hDC ); return ::DrawEscape( m_hDC, nEscape, nInputSize, lpszInputData ); }
inline int ClsDC::StartDoc( LPDOCINFO lpDocInfo )			{ _ASSERT_VALID( m_hDC ); return ::StartDoc( m_hDC, lpDocInfo ); }
inline int ClsDC::StartPage()						{ _ASSERT_VALID( m_hDC ); return ::StartPage( m_hDC ); }
inline int ClsDC::EndPage()						{ _ASSERT_VALID( m_hDC ); return ::EndPage( m_hDC ); }
inline int ClsDC::SetAbortProc( ABORTPROC lpfn )			{ _ASSERT_VALID( m_hDC ); return ::SetAbortProc( m_hDC, lpfn ); }
inline int ClsDC::AbortDoc()						{ _ASSERT_VALID( m_hDC ); return ::AbortDoc( m_hDC ); }
inline int ClsDC::EndDoc()						{ _ASSERT_VALID( m_hDC ); return ::EndDoc( m_hDC ); }
inline BOOL ClsDC::ScrollDC( int dx, int dy, LPCRECT lpRectScroll, LPCRECT lpRectClip, ClsRgn *pRgnUpdate, LPRECT lpRectUpdate )	{ _ASSERT_VALID( m_hDC ); return ::ScrollDC( m_hDC, dx, dy, lpRectScroll, lpRectClip, *pRgnUpdate, lpRectUpdate ); }
inline BOOL ClsDC::PlayMetaFile( HMETAFILE hMF )			{ _ASSERT_VALID( m_hDC ); return ::PlayMetaFile( m_hDC, hMF ); }
inline BOOL ClsDC::PlayMetaFile( HENHMETAFILE hEnhMetaFile, LPCRECT lpBounds )	{ _ASSERT_VALID( m_hDC ); return ::PlayEnhMetaFile( m_hDC, hEnhMetaFile, lpBounds ); }
inline BOOL ClsDC::AddMetaFileComment( UINT nDataSize, const BYTE *pCommentData )	{ _ASSERT_VALID( m_hDC ); return ::GdiComment( m_hDC, nDataSize, pCommentData ); }
inline BOOL ClsDC::AbortPath()						{ _ASSERT_VALID( m_hDC ); return ::AbortPath( m_hDC ); }
inline BOOL ClsDC::BeginPath()						{ _ASSERT_VALID( m_hDC ); return ::BeginPath( m_hDC ); }
inline BOOL ClsDC::CloseFigure()					{ _ASSERT_VALID( m_hDC ); return ::CloseFigure( m_hDC ); }
inline BOOL ClsDC::EndPath()						{ _ASSERT_VALID( m_hDC ); return ::EndPath( m_hDC ); }
inline BOOL ClsDC::FillPath()						{ _ASSERT_VALID( m_hDC ); return ::FillPath( m_hDC ); }
inline BOOL ClsDC::FlattenPath()					{ _ASSERT_VALID( m_hDC ); return ::FlattenPath( m_hDC ); }
inline float ClsDC::GetMiterLimit() const				{ _ASSERT_VALID( m_hDC ); float fLimit; ::GetMiterLimit( m_hDC, &fLimit ); return fLimit; }
inline int ClsDC::GetPath( LPPOINT lpPoints, LPBYTE lpTypes, int nCount ) const	{ _ASSERT_VALID( m_hDC ); return ::GetPath( m_hDC, lpPoints, lpTypes, nCount ); }
inline BOOL ClsDC::SelectClipPath( int nMode )				{ _ASSERT_VALID( m_hDC ); return ::SelectClipPath( m_hDC, nMode ); }
inline float ClsDC::SetMiterLimit( float fMiterLimit )			{ _ASSERT_VALID( m_hDC ); float fLimit; ::SetMiterLimit( m_hDC, fMiterLimit, &fLimit ); return fLimit; }
inline BOOL ClsDC::StrokeAndFillPath()					{ _ASSERT_VALID( m_hDC ); return ::StrokeAndFillPath( m_hDC ); }
inline BOOL ClsDC::StrokePath()						{ _ASSERT_VALID( m_hDC ); return ::StrokePath( m_hDC ); }
inline BOOL ClsDC::WidenPath()						{ _ASSERT_VALID( m_hDC ); return ::WidenPath( m_hDC ); }
inline BOOL ClsDC::IsValid() const					{ return ( m_hDC != NULL ) ? TRUE : FALSE; }
#endif // _DC_H_
