#ifndef _THEMING_H_
#define _THEMING_H_
//
// theming.h
//
// (C) Copright 2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../application.h"
#include <uxtheme.h>
#include <tmschema.h>

// Just in case...
#ifndef WM_THEMECHANGED
#define WM_THEMECHANGED                 0x031A
#endif

// This class wraps the entire theming API from the
// uxtheme.dll. Instead of linking with the "UxTheme.lib" this
// approach allows you to code for the XP platform and the
// other platforms.
class ClsUxTheme
{
	_NO_COPY( ClsUxTheme );
public:
	ClsUxTheme()
	{
		// Clear function table.
		::ZeroMemory(( LPVOID )m_FuncTable, sizeof( m_FuncTable ));

		// Load the theming library.
		m_hUxLib = ::LoadLibrary( _T( "uxtheme.dll" ));
	}

	virtual ~ClsUxTheme()
	{
		// Close the library is it was opened.
		if ( m_hUxLib ) ::FreeLibrary( m_hUxLib );
	}

	// Determine if theming is enabled.
	inline BOOL IsThemingEnabled() { return ( BOOL )( ClsGetApp()->GetComCtlVersion() >= 6 && m_hUxLib && IsAppThemed()); }

	HRESULT CloseThemeData( HTHEME hTheme )
	{
		typedef HRESULT ( CALLBACK* CLOSETHEMEDATA )( HTHEME );
		CLOSETHEMEDATA fnc = ( CLOSETHEMEDATA )ProcAddress( "CloseThemeData", 0 );
		return ( *fnc )( hTheme );
	}

	HRESULT DrawThemeBackground( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, LPCRECT pClipRect )
	{
		typedef HRESULT ( CALLBACK* DRAWTHEMEBACKGROUND )( HTHEME, HDC, int, int, LPCRECT, LPCRECT );
		DRAWTHEMEBACKGROUND fnc = ( DRAWTHEMEBACKGROUND )ProcAddress( "DrawThemeBackground", 1 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pRect, pClipRect );
	}

	HRESULT DrawThemeEdge( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pDestRect, UINT uEdge, UINT uFlags, LPRECT pContentRect )
	{
		typedef HRESULT ( CALLBACK* DRAWTHEMEEDGE )( HTHEME, HDC, int, int, LPCRECT, UINT, UINT, LPRECT );
		DRAWTHEMEEDGE fnc = ( DRAWTHEMEEDGE )ProcAddress( "DrawThemeEdge", 2 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pDestRect, uEdge, uFlags, pContentRect );
	}

	HRESULT DrawThemeIcon( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HIMAGELIST himl, int iImageIndex )
	{
		typedef HRESULT ( CALLBACK* DRAWTHEMEICON )( HTHEME, HDC, int, int, LPCRECT, HIMAGELIST, int );
		DRAWTHEMEICON fnc = ( DRAWTHEMEICON )ProcAddress( "DrawThemeIcon", 3 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pRect, himl, iImageIndex );
	}

	HRESULT DrawThemeParentBackground( HWND hwnd, HDC hdc, LPRECT pRect )
	{
		typedef HRESULT ( CALLBACK* DRAWTHEMEPARENTBACKGROUND )( HWND, HDC, LPRECT );
		DRAWTHEMEPARENTBACKGROUND fnc = ( DRAWTHEMEPARENTBACKGROUND )ProcAddress( "DrawThemeParentBackground", 4 );
		return ( *fnc )( hwnd, hdc, pRect );
	}

	HRESULT DrawThemeText( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, DWORD dwTextFlags2, LPCRECT pRect )
	{
		typedef HRESULT ( CALLBACK* DRAWTHEMETEXT )( HTHEME, HDC, int, int, LPCWSTR, int, DWORD, DWORD, LPCRECT );
		DRAWTHEMETEXT fnc = ( DRAWTHEMETEXT )ProcAddress( "DrawThemeText", 5 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, dwTextFlags2, pRect );
	}

	HRESULT EnableThemeDialogTexture( HWND hwnd, DWORD dwFlags )
	{
		typedef HRESULT ( CALLBACK* ENABLETHEMEDIALOGTEXTURE )( HWND, DWORD );
		ENABLETHEMEDIALOGTEXTURE fnc = ( ENABLETHEMEDIALOGTEXTURE )ProcAddress( "EnableThemeDialogTexture", 6 );
		return ( *fnc )( hwnd, dwFlags );
	}

	HRESULT EnableTheming( BOOL fEnable )
	{
		typedef HRESULT ( CALLBACK* ENABLETHEMING )( BOOL );
		ENABLETHEMING fnc = ( ENABLETHEMING )ProcAddress( "EnableTheming", 7 );
		return ( *fnc )( fEnable );
	}

	HRESULT GetCurrentThemeName( LPWSTR pszThemeFileName, int cchMaxNameChars, LPWSTR pszColorBuff, int cchMaxColorChars, LPWSTR pszSizeBuff, int cchMaxSizeChars )
	{
		typedef HRESULT ( CALLBACK* GETCURRENTTHEMENAME )( LPWSTR, int, LPWSTR, int, LPWSTR, int );
		GETCURRENTTHEMENAME fnc = ( GETCURRENTTHEMENAME )ProcAddress( "GetCurrentThemeName", 8 );
		return ( *fnc )( pszThemeFileName, cchMaxNameChars, pszColorBuff, cchMaxColorChars, pszSizeBuff, cchMaxSizeChars );
	}
	
	DWORD GetThemeAppProperties()
	{
		typedef DWORD   ( CALLBACK* GETTHEMEAPPPROPERTIES )();
		GETTHEMEAPPPROPERTIES fnc = ( GETTHEMEAPPPROPERTIES )ProcAddress( "GetThemeAppProperties", 9 );
		return ( *fnc )();
	}

	HRESULT GetThemeBackgroundContentRect( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pBoundingRect, LPRECT pContentRect )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEBACKGROUNDCONTENTRECT )( HTHEME, HDC, int, int, LPCRECT, LPRECT );
		GETTHEMEBACKGROUNDCONTENTRECT fnc = ( GETTHEMEBACKGROUNDCONTENTRECT )ProcAddress( "GetThemeBackgroundContentRect", 10 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pBoundingRect, pContentRect );
	}

	HRESULT GetThemeBackgroundExtent( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pContentRect, LPRECT pExtentRect )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEBACKGROUNDEXTENT )( HTHEME, HDC, int, int, LPCRECT, LPRECT );
		GETTHEMEBACKGROUNDEXTENT fnc = ( GETTHEMEBACKGROUNDEXTENT )ProcAddress( "GetThemeBackgroundExtent", 11 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pContentRect, pExtentRect );
	}
	
	HRESULT GetThemeBackgroundRegion( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCRECT pRect, HRGN *pRegion )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEBACKGROUNDREGION )( HTHEME, HDC, int, int, LPCRECT, HRGN * );
		GETTHEMEBACKGROUNDREGION fnc = ( GETTHEMEBACKGROUNDREGION )ProcAddress( "GetThemeBackgroundRegion", 12 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pRect, pRegion );
	}

	HRESULT GetThemeBool( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPBOOL pfVal )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEBOOL )( HTHEME, int, int, int, LPBOOL );
		GETTHEMEBOOL fnc = ( GETTHEMEBOOL )ProcAddress( "GetThemeBool", 13 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pfVal );
	}
	
	HRESULT GetThemeColor( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPCOLORREF pColor )
	{
		typedef HRESULT ( CALLBACK* GETTHEMECOLOR )( HTHEME, int, int, int, LPCOLORREF );
		GETTHEMECOLOR fnc = ( GETTHEMECOLOR )ProcAddress( "GetThemeColor", 14 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pColor );
	}
	
	HRESULT GetThemeDocumentationProperty( LPCWSTR pszThemeName, LPCWSTR pszPropertyName, LPWSTR pszValueBuff, int cchMaxValChars )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEDOCUMENTATIONPROPERTY )( LPCWSTR, LPCWSTR, LPWSTR, int );
		GETTHEMEDOCUMENTATIONPROPERTY fnc = ( GETTHEMEDOCUMENTATIONPROPERTY )ProcAddress( "GetThemeDocumentationProperty", 15 );
		return ( *fnc )( pszThemeName, pszPropertyName, pszValueBuff, cchMaxValChars );
	}
	
	HRESULT GetThemeEnumValue( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPINT piVal )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEENUMVALUE )( HTHEME, int, int, int, LPINT );
		GETTHEMEENUMVALUE fnc = ( GETTHEMEENUMVALUE )ProcAddress( "GetThemeEnumValue", 16 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, piVal );
	}
	
	HRESULT GetThemeFilename( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszThemeFilename, int cchMaxBuffChars )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEFILENAME )( HTHEME, int, int, int, LPWSTR, int );
		GETTHEMEFILENAME fnc = ( GETTHEMEFILENAME )ProcAddress( "GetThemeFilename", 17 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pszThemeFilename, cchMaxBuffChars );
	}
	
	HRESULT GetThemeFont( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPLOGFONT pFont )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEFONT )( HTHEME, HDC, int, int, int, LPLOGFONT );
		GETTHEMEFONT fnc = ( GETTHEMEFONT )ProcAddress( "GetThemeFont", 18 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, iPropId, pFont );
	}
	
	HRESULT GetThemeInt( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPINT piVal )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEINT )( HTHEME, int, int, int, LPINT );
		GETTHEMEINT fnc = ( GETTHEMEINT )ProcAddress( "GetThemeInt", 19 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, piVal );
	}
	
	HRESULT GetThemeIntList( HTHEME hTheme, int iPartId, int iStateId, int iPropId, INTLIST *pIntList )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEINTLIST )( HTHEME, int, int, int, INTLIST * );
		GETTHEMEINTLIST fnc = ( GETTHEMEINTLIST )ProcAddress( "GetThemeIntList", 20 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pIntList );
	}
	
	HRESULT GetThemeMargins( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPRECT prc, MARGINS *pMargins )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEMARGINS )( HTHEME, HDC, int, int, int, LPRECT, MARGINS * );
		GETTHEMEMARGINS fnc = ( GETTHEMEMARGINS )ProcAddress( "GetThemeMargins", 21 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, iPropId, prc, pMargins );
	}
	
	HRESULT GetThemeMetric( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, int iPropId, LPINT piVal )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEMETRIC )( HTHEME, HDC, int, int, int, LPINT );
		GETTHEMEMETRIC fnc = ( GETTHEMEMETRIC )ProcAddress( "GetThemeMetric", 22 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, iPropId, piVal );
	}
	
	HRESULT GetThemePartSize( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPRECT prc, THEMESIZE eSize, LPSIZE psz )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEPARTSIZE )( HTHEME, HDC, int, int, LPRECT, THEMESIZE, LPSIZE );
		GETTHEMEPARTSIZE fnc = ( GETTHEMEPARTSIZE )ProcAddress( "GetThemePartSize", 23 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, prc, eSize, psz );
	}
	
	HRESULT GetThemePosition( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPPOINT pPoint )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEPOSITION )( HTHEME, int, int, int, LPPOINT );
		GETTHEMEPOSITION fnc = ( GETTHEMEPOSITION )ProcAddress( "GetThemePosition", 24 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pPoint );
	}

	HRESULT GetThemePropertyOrigin( HTHEME hTheme, int iPartId, int iStateId, int iPropId, PROPERTYORIGIN *pOrigin )
	{
		typedef HRESULT ( CALLBACK* GETTHEMEPROPERTYORIGIN )( HTHEME, int, int, int, PROPERTYORIGIN * );
		GETTHEMEPROPERTYORIGIN fnc = ( GETTHEMEPROPERTYORIGIN )ProcAddress( "GetThemePropertyOrigin", 25 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pOrigin );
	}
	
	HRESULT GetThemeRect( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPRECT pRect )
	{
		typedef HRESULT ( CALLBACK* GETTHEMERECT )( HTHEME, int, int, int, LPRECT );
		GETTHEMERECT fnc = ( GETTHEMERECT )ProcAddress( "GetThemeRect", 26 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pRect );
	}
	
	HRESULT GetThemeString( HTHEME hTheme, int iPartId, int iStateId, int iPropId, LPWSTR pszBuff, int cchMaxBuffChars )
	{
		typedef HRESULT ( CALLBACK* GETTHEMESTRING )( HTHEME, int, int, int, LPWSTR, int );
		GETTHEMESTRING fnc = ( GETTHEMESTRING )ProcAddress( "GetThemeString", 27 );
		return ( *fnc )( hTheme, iPartId, iStateId, iPropId, pszBuff, cchMaxBuffChars );
	}
	
	BOOL GetThemeSysBool( HTHEME hTheme, int iBoolID )
	{
		typedef BOOL ( CALLBACK* GETTHEMESYSBOOL )( HTHEME, int );
		GETTHEMESYSBOOL fnc = ( GETTHEMESYSBOOL )ProcAddress( "GetThemeSysBool", 28 );
		return ( *fnc )( hTheme, iBoolID );
	}

	COLORREF GetThemeSysColor( HTHEME hTheme, int iColorID )
	{
		typedef COLORREF ( CALLBACK* GETTHEMESYSCOLOR )( HTHEME, int );
		GETTHEMESYSCOLOR fnc = ( GETTHEMESYSCOLOR )ProcAddress( "GetThemeSysColor", 29 );
		return ( *fnc )( hTheme, iColorID );
	}
	
	HBRUSH GetThemeSysColorBrush( HTHEME hTheme, int iColorID )
	{
		typedef HBRUSH  ( CALLBACK* GETTHEMESYSCOLORBRUSH )( HTHEME, int );
		GETTHEMESYSCOLORBRUSH fnc = ( GETTHEMESYSCOLORBRUSH )ProcAddress( "GetThemeSysColorBrush", 30 );
		return ( *fnc )( hTheme, iColorID );
	}
	
	HRESULT GetThemeSysFont( HTHEME hTheme, int iFontID, LPLOGFONT plf )
	{
		typedef HRESULT ( CALLBACK* GETTHEMESYSFONT )( HTHEME, int, LPLOGFONT );
		GETTHEMESYSFONT fnc = ( GETTHEMESYSFONT )ProcAddress( "GetThemeSysFont", 31 );
		return ( *fnc )( hTheme, iFontID, plf );
	}
	
	HRESULT GetThemeSysInt( HTHEME hTheme, int iIntID, LPINT piValue )
	{
		typedef HRESULT ( CALLBACK* GETTHEMESYSINT )( HTHEME, int, LPINT );
		GETTHEMESYSINT fnc = ( GETTHEMESYSINT )ProcAddress( "GetThemeSysInt", 32 );
		return ( *fnc )( hTheme, iIntID, piValue );
	}
	
	int GetThemeSysSize( HTHEME hTheme, int iSizeID )
	{
		typedef int ( CALLBACK* GETTHEMESYSSIZE )( HTHEME, int );
		GETTHEMESYSSIZE fnc = ( GETTHEMESYSSIZE )ProcAddress( "GetThemeSysSize", 33 );
		return ( *fnc )( hTheme, iSizeID );
	}
	
	HRESULT GetThemeSysString( HTHEME hTheme, int iStringID, LPWSTR pszStringBuff, int cchMaxStringChars )
	{
		typedef HRESULT ( CALLBACK* GETTHEMESYSSTRING )( HTHEME, int, LPWSTR, int );
		GETTHEMESYSSTRING fnc = ( GETTHEMESYSSTRING )ProcAddress( "GetThemeSysString", 34 );
		return ( *fnc )( hTheme, iStringID, pszStringBuff, cchMaxStringChars );
	}
	
	HRESULT GetThemeTextExtent( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, LPCRECT pBoundingRect, LPRECT pExtentRect )
	{
		typedef HRESULT ( CALLBACK* GETTHEMETEXTEXTENT )( HTHEME, HDC, int, int, LPCWSTR, int, DWORD, LPCRECT, LPRECT );
		GETTHEMETEXTEXTENT fnc = ( GETTHEMETEXTEXTENT )ProcAddress( "GetThemeTextExtent", 35 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, pszText, iCharCount, dwTextFlags, pBoundingRect, pExtentRect );
	}
	
	HRESULT GetThemeTextMetrics( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, LPTEXTMETRIC ptm )
	{
		typedef HRESULT ( CALLBACK* GETTHEMETEXTMETRICS )( HTHEME, HDC, int, int, LPTEXTMETRIC );
		GETTHEMETEXTMETRICS fnc = ( GETTHEMETEXTMETRICS )ProcAddress( "GetThemeTextMetrics", 36 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, ptm );
	}
	
	HTHEME GetWindowTheme( HWND hWnd )
	{
		typedef HTHEME  ( CALLBACK* GETWINDOWTHEME )( HWND );
		GETWINDOWTHEME fnc = ( GETWINDOWTHEME )ProcAddress( "GetWindowTheme", 37 );
		return ( *fnc )( hWnd );
	}
	
	HRESULT HitTestThemeBackground( HTHEME hTheme, HDC hdc, int iPartId, int iStateId, DWORD dwOptions, LPCRECT pRect, HRGN hrgn, POINT ptTest, LPWORD pwHitTestCode )
	{
		typedef HRESULT ( CALLBACK* HITTESTTHEMEBACKGROUND )( HTHEME, HDC, int, int, DWORD, LPCRECT, HRGN, POINT, LPWORD );
		HITTESTTHEMEBACKGROUND fnc = ( HITTESTTHEMEBACKGROUND )ProcAddress( "HitTestThemeBackground", 38 );
		return ( *fnc )( hTheme, hdc, iPartId, iStateId, dwOptions, pRect, hrgn, ptTest, pwHitTestCode );
	}
	
	BOOL IsAppThemed()
	{
		typedef BOOL ( CALLBACK* ISAPPTHEMED )();
		ISAPPTHEMED fnc = ( ISAPPTHEMED )ProcAddress( "IsAppThemed", 39 );
		return ( *fnc )();
	}
	
	BOOL IsThemeActive()
	{
		typedef BOOL ( CALLBACK* ISTHEMEACTIVE )();
		ISTHEMEACTIVE fnc = ( ISTHEMEACTIVE )ProcAddress( "IsThemeActive", 40 );
		return ( *fnc )();
	}

	BOOL IsThemeBackgroundPartiallyTransparent( HTHEME hTheme, int iPartId, int iStateId )
	{
		typedef BOOL ( CALLBACK* ISTHEMEBACKGROUNDPARTIALLYTRANSPARENT )( HTHEME, int, int );
		ISTHEMEBACKGROUNDPARTIALLYTRANSPARENT fnc = ( ISTHEMEBACKGROUNDPARTIALLYTRANSPARENT )ProcAddress( "IsThemeBackgroundPartiallyTransparent", 41 );
		return ( *fnc )( hTheme, iPartId, iStateId );
	}
	
	BOOL IsThemeDialogTextureEnabled()
	{
		typedef BOOL ( CALLBACK* ISTHEMEDIALOGTEXTUREENABLED )();
		ISTHEMEDIALOGTEXTUREENABLED fnc = ( ISTHEMEDIALOGTEXTUREENABLED )ProcAddress( "IsThemeDialogTextureEnabled", 42 );
		return ( *fnc )();
	}
	
	BOOL IsThemePartDefined( HTHEME hTheme, int iPartId, int iStateId )
	{
		typedef BOOL ( CALLBACK* ISTHEMEPARTDEFINED )( HTHEME, int, int );
		ISTHEMEPARTDEFINED fnc = ( ISTHEMEPARTDEFINED )ProcAddress( "IsThemePartDefined", 43 );
		return ( *fnc )( hTheme, iPartId, iStateId );
	}
	
	HTHEME OpenThemeData( HWND hwnd, LPCWSTR pszClassList )
	{
		typedef HTHEME ( CALLBACK* OPENTHEMEDATA )( HWND, LPCWSTR );
		OPENTHEMEDATA fnc = ( OPENTHEMEDATA )ProcAddress( "OpenThemeData", 44 );
		return ( *fnc )( hwnd, pszClassList );
	}
	
	void SetThemeAppProperties( DWORD dwFlags )
	{
		typedef void ( CALLBACK* SETTHEMEAPPPROPERTIES )( DWORD );
		SETTHEMEAPPPROPERTIES fnc = ( SETTHEMEAPPPROPERTIES )ProcAddress( "SetThemeAppProperties", 45 );
		( *fnc )( dwFlags );
	}
	
	HRESULT SetWindowTheme( HWND hwnd, LPCWSTR pszSubAppName, LPCWSTR pszSubIdList )
	{
		typedef HRESULT ( CALLBACK* SETWINDOWTHEME )( HWND, LPCWSTR, LPCWSTR );
		SETWINDOWTHEME fnc = ( SETWINDOWTHEME )ProcAddress( "SetWindowTheme", 46 );
		return ( *fnc )( hwnd, pszSubAppName, pszSubIdList );
	}

protected:
	// Obtain the address of the function.
	FARPROC ProcAddress( LPCSTR pszProcName, int iIndex )
	{
		// Do we have themes?
		_ASSERT_VALID( m_hUxLib );

		// Did we get it already?
		if ( m_FuncTable[ iIndex ] == NULL )
			// Load it.
			m_FuncTable[ iIndex ] = ::GetProcAddress( m_hUxLib, pszProcName );

		// Must be valid here.
		_ASSERT_TEXT( m_FuncTable[ iIndex ] != 0, pszProcName );
		return m_FuncTable[ iIndex ];
	}

	// Data.
	FARPROC		m_FuncTable[ 47 ];
	HMODULE		m_hUxLib;
};
// Defined in application.cpp.
extern ClsUxTheme ThemingAPI;
#endif // _THEMING_H_
