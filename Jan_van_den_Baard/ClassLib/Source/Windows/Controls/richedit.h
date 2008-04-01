#ifndef _RICHEDIT_H_
#define _RICHEDIT_H_
//
// richedit.h
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "edit.h"
#include "../window.h"
#include "../../coords/rect.h"

#ifndef _RICHEDIT_
#include <richedit.h>
#endif

#ifndef _RICHOLE_
#include <richole.h>
#endif

// A ClsEdit derived class handling richedit controls.
class ClsRichEdit : public ClsEdit
{
	_NO_COPY( ClsRichEdit );
public:
	// Default constructor.
	ClsRichEdit() {;}

	// Constructor. Initializes to the passed
	// handle.
	ClsRichEdit( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsRichEdit()
	{;}

	// Create an edit control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszText = NULL, UINT nStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL | ES_LEFT )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsWindow::Create( 0, RICHEDIT_CLASS, pszText, nStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	inline BOOL AutoURLDetect( BOOL bAutodetect )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )( ::SendMessage( m_hWnd, EM_AUTOURLDETECT, bAutodetect, 0 ) ? FALSE : TRUE );
	}

	inline BOOL CanPaste( int nFormat = 0 ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_CANPASTE, ( WPARAM )nFormat, 0 );
	}

	inline BOOL CanRedo() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_CANREDO, 0, 0 );
	}

	inline BOOL Displayband( LPRECT pRect )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_DISPLAYBAND, 0, ( LPARAM )pRect );
	}

	inline void ExGetSel( CHARRANGE& CharRange ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_EXGETSEL, 0, ( LPARAM )&CharRange );
	}

	inline void ExGetSel( CHARRANGE *pCharRange ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_EXGETSEL, 0, ( LPARAM )pCharRange );
	}

	inline void ExLimitText( DWORD dwLength = 0 )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_EXLIMITTEXT, 0, ( LPARAM )dwLength );
	}

	inline UINT ExLineFromChar( UINT nCharIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return( UINT )::SendMessage( m_hWnd, EM_EXLINEFROMCHAR, 0, ( LPARAM )nCharIndex );
	}

	inline void ExSetSel( CHARRANGE& CharRange )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_EXSETSEL, 0, ( LPARAM )&CharRange );
	}

	inline void ExSetSel( CHARRANGE *pCharRange )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_EXSETSEL, 0, ( LPARAM )pCharRange );
	}

	inline int FindText( DWORD dwFlags, FINDTEXT *pFindText ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
#if (_RICHEDIT_VER >= 0x0200) && defined(_UNICODE)
		return ( int )::SendMessage( m_hWnd, EM_FINDTEXTW, ( WPARAM )dwFlags, ( LPARAM )pFindText);
#else
		return ( int )::SendMessage( m_hWnd, EM_FINDTEXT, ( WPARAM )dwFlags, ( LPARAM )pFindText);
#endif
	}

	inline int FindText( DWORD dwFlags, FINDTEXTEX *pFindText ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
#if (_RICHEDIT_VER >= 0x0200) && defined(_UNICODE)
		return ( int )::SendMessage( m_hWnd, EM_FINDTEXTEXW, ( WPARAM )dwFlags, ( LPARAM )pFindText);
#else
		return ( int )::SendMessage( m_hWnd, EM_FINDTEXTEX, ( WPARAM )dwFlags, ( LPARAM )pFindText);
#endif
	}

	inline DWORD FindWordBreak( DWORD dwFlags, UINT nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_FINDWORDBREAK, ( WPARAM )dwFlags, ( LPARAM )nIndex );
	}

	inline UINT FormatRange( FORMATRANGE& Range, BOOL bRender = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_FORMATRANGE, ( WPARAM )bRender, ( LPARAM )&Range );
	}

	inline UINT FormatRange( FORMATRANGE *pRange, BOOL bRender = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_FORMATRANGE, ( WPARAM )bRender, ( LPARAM )pRange );
	}

	inline BOOL GetAutoURLDetect() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETAUTOURLDETECT, 0, 0 );
	}

	inline void GetBIDIOptions( BIDIOPTIONS& bidiOpt ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_GETBIDIOPTIONS, 0, ( LPARAM )&bidiOpt );
	}

	inline void GetBIDIOptions( BIDIOPTIONS *pbidiOpt ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_GETBIDIOPTIONS, 0, ( LPARAM )pbidiOpt );
	}

	inline DWORD GetCharFormat( DWORD dwFlags, CHARFORMAT& charFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETCHARFORMAT, dwFlags, ( LPARAM )&charFormat );
	}

	inline DWORD GetCharFormat( DWORD dwFlags, CHARFORMAT *pcharFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETCHARFORMAT, dwFlags, ( LPARAM )pcharFormat );
	}

	inline DWORD GetCharFormat( DWORD dwFlags, CHARFORMAT2& charFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETCHARFORMAT, dwFlags, ( LPARAM )&charFormat );
	}

	inline DWORD GetCharFormat( DWORD dwFlags, CHARFORMAT2 *pcharFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETCHARFORMAT, dwFlags, ( LPARAM )pcharFormat );
	}

	inline DWORD GetEditStyle() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETEDITSTYLE, 0, 0 );
	}

	inline DWORD GetEventMask() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETEVENTMASK, 0, 0 );
	}

	inline BOOL GetIMEColor( COMPCOLOR& compColor ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETIMECOLOR, 0, ( LPARAM )&compColor );
	}

	inline BOOL GetIMEColor( COMPCOLOR *pcompColor ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETIMECOLOR, 0, ( LPARAM )pcompColor );
	}

	inline DWORD GetIMECompMode() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETIMECOMPMODE, 0, 0 );
	}

	inline DWORD GetIMEOptions() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETIMEOPTIONS, 0, 0 );
	}

	inline DWORD GetLangOptions() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETLANGOPTIONS, 0, 0 );
	}

	inline BOOL GetOLEInterface( IRichEditOle& oleInterface ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETOLEINTERFACE, 0, ( LPARAM )&oleInterface );
	}

	inline BOOL GetOLEInterface( IRichEditOle *poleInterface ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETOLEINTERFACE, 0, ( LPARAM )poleInterface );
	}

	inline DWORD GetOptions() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETOPTIONS, 0, 0 );
	}

	inline DWORD GetParaFormat( PARAFORMAT& paraFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETPARAFORMAT, 0, ( LPARAM )&paraFormat );
	}

	inline DWORD GetParaFormat( PARAFORMAT *pparaFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETPARAFORMAT, 0, ( LPARAM )pparaFormat );
	}

	inline DWORD GetParaFormat( PARAFORMAT2& paraFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETPARAFORMAT, 0, ( LPARAM )&paraFormat );
	}

	inline DWORD GetParaFormat( PARAFORMAT2 *pparaFormat ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETPARAFORMAT, 0, ( LPARAM )pparaFormat );
	}

	inline BOOL GetPunctuation( DWORD dwFlags, PUNCTUATION& punctuation ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETPUNCTUATION, ( WPARAM )dwFlags, ( LPARAM )&punctuation );
	}

	inline BOOL GetPunctuation( DWORD dwFlags, PUNCTUATION *ppunctuation ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETPUNCTUATION, ( WPARAM )dwFlags, ( LPARAM )ppunctuation );
	}

	inline UINT GetRedoName() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETREDONAME, 0, 0 );
	}

	inline void GetScrollPos( POINT& pt ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_GETSCROLLPOS, 0, ( LPARAM )&pt );
	}

	inline void GetScrollPos( POINT *ppt ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_GETSCROLLPOS, 0, ( LPARAM )ppt );
	}

	inline UINT GetSelText( LPSTR pszText ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETSELTEXT, 0, ( LPARAM )pszText );
	}

	inline UINT GetTextEx( GETTEXTEX& getText, LPTSTR pszBuffer ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETTEXTEX, ( WPARAM )&getText, ( LPARAM )pszBuffer );
	}

	inline UINT GetTextEx( GETTEXTEX *pgetText, LPTSTR pszBuffer ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETTEXTEX, ( WPARAM )pgetText, ( LPARAM )pszBuffer );
	}

	inline UINT GetTextMode() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETTEXTMODE, 0, 0 );
	}

	inline UINT GetTextRange( TEXTRANGE& textRange ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETTEXTRANGE, 0, ( LPARAM )&textRange );
	}

	inline UINT GetTextRange( TEXTRANGE *ptextRange ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETTEXTRANGE, 0, ( LPARAM )ptextRange );
	}

	inline DWORD GetTypographyOptions() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETTYPOGRAPHYOPTIONS, 0, 0 );
	}

	inline UINT GetUndoName() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_GETUNDONAME, 0, 0 );
	}

	inline EDITWORDBREAKPROCEX GetWordBreakProcEx() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( EDITWORDBREAKPROCEX )::SendMessage( m_hWnd, EM_GETWORDBREAKPROCEX, 0, 0 );
	}

	inline DWORD GetWordwrapMode() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_GETWORDWRAPMODE, 0, 0 );
	}

	inline BOOL GetZoom( WORD& numerator, WORD& denominator ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETZOOM, ( WPARAM )&numerator, ( LPARAM )&denominator );
	}

	inline BOOL GetZoom( WORD *pnumerator, WORD *pdenominator ) const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_GETZOOM, ( WPARAM )pnumerator, ( LPARAM )pdenominator );
	}

	inline void HideSelecton( BOOL bHide )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_GETZOOM, ( WPARAM )bHide, 0 );
	}

	inline void PasteSpecial( UINT format, REPASTESPECIAL& pasteSpecial )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_PASTESPECIAL, ( WPARAM )format, ( LPARAM )&pasteSpecial );
	}

	inline void PasteSpecial( UINT format, REPASTESPECIAL *ppasteSpecial )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_PASTESPECIAL, ( WPARAM )format, ( LPARAM )ppasteSpecial );
	}

	inline void Reconversion()
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_RECONVERSION, 0, 0 );
	}

	inline BOOL Redo()
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_REDO, 0, 0 );
	}

	inline void RequestResize()
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_REQUESTRESIZE, 0, 0 );
	}

	inline DWORD SelectionType() const
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_SELECTIONTYPE, 0, 0 );
	}

	inline void SetBIDIOptions( BIDIOPTIONS& bidiOptions )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_SETBIDIOPTIONS, 0, ( LPARAM )&bidiOptions );
	}

	inline void SetBIDIOptions( BIDIOPTIONS *pbidiOptions )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_SETBIDIOPTIONS, 0, ( LPARAM )pbidiOptions );
	}

	inline COLORREF SetBkgndColor( COLORREF crColor, BOOL bSysColor = FALSE )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( COLORREF )::SendMessage( m_hWnd, EM_SETBKGNDCOLOR, ( WPARAM )bSysColor, ( LPARAM )crColor );
	}

	inline BOOL SetCharFormat( DWORD dwFlags, CHARFORMAT& charFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETCHARFORMAT, ( WPARAM )dwFlags, ( LPARAM )&charFormat );
	}

	inline BOOL SetCharFormat( DWORD dwFlags, CHARFORMAT *pcharFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETCHARFORMAT, ( WPARAM )dwFlags, ( LPARAM )pcharFormat );
	}

	inline BOOL SetCharFormat( DWORD dwFlags, CHARFORMAT2& charFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETCHARFORMAT, ( WPARAM )dwFlags, ( LPARAM )&charFormat );
	}

	inline BOOL SetCharFormat( DWORD dwFlags, CHARFORMAT2 *pcharFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETCHARFORMAT, ( WPARAM )dwFlags, ( LPARAM )pcharFormat );
	}

	inline DWORD SetEditStyle( DWORD dwStyle, DWORD dwMask )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_SETEDITSTYLE, ( WPARAM )dwStyle, ( LPARAM )dwMask );
	}

	inline DWORD SetEditMask( DWORD dwMask )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_SETEVENTMASK, 0, ( LPARAM )dwMask );
	}

	inline BOOL SetFontSize( int nSizeDelta )
	{
		_ASSERT_VALID( GetSafeHWND() );
		_ASSERT( nSizeDelta >= -1637 && nSizeDelta <= 1638 );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETFONTSIZE, ( WPARAM )nSizeDelta, 0 );
	}

	inline BOOL SetCompColor( COMPCOLOR& compColor )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETIMECOLOR, 0, ( LPARAM )&compColor );
	}

	inline BOOL SetCompColor( COMPCOLOR *pcompColor )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETIMECOLOR, 0, ( LPARAM )pcompColor );
	}

	inline BOOL SetIMEOptions( DWORD dwOper, DWORD dwOptions )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETIMEOPTIONS, ( WPARAM )dwOper, ( LPARAM )dwOptions );
	}

	inline void SetLangOptions( DWORD dwOptions )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_SETLANGOPTIONS, 0, ( LPARAM )dwOptions );
	}

	inline BOOL SetOleCallback( IRichEditOleCallback *pCallback )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETOLECALLBACK, 0, ( LPARAM )pCallback );
	}

	inline DWORD SetOptions( DWORD dwOper, DWORD dwOptions )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_SETOPTIONS, ( WPARAM )dwOper, ( LPARAM )dwOptions );
	}

	inline void SetPalette( HPALETTE hPalette )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_SETPALETTE, ( WPARAM )hPalette, 0 );
	}

	inline BOOL SetParaFormat( PARAFORMAT& paraFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, ( LPARAM )&paraFormat );
	}

	inline BOOL SetParaFormat( PARAFORMAT *pparaFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, ( LPARAM )pparaFormat );
	}

	inline BOOL SetParaFormat( PARAFORMAT2& paraFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, ( LPARAM )&paraFormat );
	}

	inline BOOL SetParaFormat( PARAFORMAT2 *pparaFormat )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETPARAFORMAT, 0, ( LPARAM )pparaFormat );
	}

	inline BOOL SetPunctuation( DWORD dwType, PUNCTUATION& punctuation )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETPUNCTUATION, ( WPARAM )dwType, ( LPARAM )&punctuation );
	}

	inline BOOL SetPunctuation( DWORD dwType, PUNCTUATION *ppunctuation )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETPUNCTUATION, ( WPARAM )dwType, ( LPARAM )ppunctuation );
	}

	inline void SetScrollPos( POINT &pt )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_SETSCROLLPOS, 0, ( LPARAM )&pt );
	}

	inline void SetScrollPos( POINT *ppt )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_SETSCROLLPOS, 0, ( LPARAM )ppt );
	}

	inline BOOL SetTargetDevice( HDC hDC, UINT nLineWidth )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETTARGETDEVICE, ( WPARAM )hDC, ( LPARAM )nLineWidth );
	}

	inline UINT SetTextEx( SETTEXTEX& textEx, TCHAR *pszText )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_SETTEXTEX, ( WPARAM )&textEx, ( LPARAM )pszText );
	}

	inline UINT SetTextEx( SETTEXTEX *ptextEx, TCHAR *pszText )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_SETTEXTEX, ( WPARAM )ptextEx, ( LPARAM )pszText );
	}

	inline BOOL SetTextMode( DWORD dwMode )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )( ::SendMessage( m_hWnd, EM_SETTEXTMODE, ( WPARAM )dwMode, 0 ) ? FALSE : TRUE );
	}

	inline BOOL SetTypographyOptions( DWORD dwOptions, DWORD dwMask )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETTYPOGRAPHYOPTIONS, ( WPARAM )dwOptions, ( WPARAM )dwMask );
	}

	inline UINT SetUndoLimit( UINT nLimit )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_SETUNDOLIMIT, ( WPARAM )nLimit, 0 );
	}

	inline EDITWORDBREAKPROCEX SetWordBreakProcEx( EDITWORDBREAKPROCEX pfnProc )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( EDITWORDBREAKPROCEX )::SendMessage( m_hWnd, EM_SETWORDBREAKPROCEX, 0, ( LPARAM )pfnProc );
	}

	inline DWORD SetWordwrapMode( DWORD dwMode )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( DWORD )::SendMessage( m_hWnd, EM_SETWORDWRAPMODE, ( WPARAM )dwMode, 0 );
	}

	inline BOOL SetZoom( WORD numerator, WORD denominator )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( BOOL )::SendMessage( m_hWnd, EM_SETZOOM, ( WPARAM )numerator, ( LPARAM )denominator );
	}

	inline void ShowScrollbar( DWORD dwIdent, BOOL bShow )
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_SHOWSCROLLBAR, ( WPARAM )dwIdent, ( LPARAM )bShow );
	}

	inline void StopGroupTyping()
	{
		_ASSERT_VALID( GetSafeHWND() );
		::SendMessage( m_hWnd, EM_STOPGROUPTYPING, 0, 0 );
	}

	inline UINT StreamIn( DWORD dwOptions, EDITSTREAM& editStream )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_STREAMIN, ( WPARAM )dwOptions, ( LPARAM )&editStream );
	}

	inline UINT StreamIn( DWORD dwOptions, EDITSTREAM *peditStream )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_STREAMIN, ( WPARAM )dwOptions, ( LPARAM )peditStream );
	}

	inline UINT StreamOut( DWORD dwOptions, EDITSTREAM& editStream )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_STREAMOUT, ( WPARAM )dwOptions, ( LPARAM )&editStream );
	}

	inline UINT StreamOut( DWORD dwOptions, EDITSTREAM *peditStream )
	{
		_ASSERT_VALID( GetSafeHWND() );
		return ( UINT )::SendMessage( m_hWnd, EM_STREAMOUT, ( WPARAM )dwOptions, ( LPARAM )peditStream );
	}
protected:
	// Reflected WM_COMMAND notification handlers.
	virtual LRESULT OnIMEChange( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnAlignLTR(  BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnAlignRTL(  BOOL& bNotifyParent ) { return 0; }

	// Handle the reflected WM_COMMAND messages.
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
	{
		switch ( nNotifyCode )
		{
			case	EN_IMECHANGE:	return OnIMEChange( bNotifyParent );
			case	EN_ALIGNLTR:	return OnAlignLTR( bNotifyParent );
			case	EN_ALIGNRTL:	return OnAlignRTL( bNotifyParent );
		}
		return ClsEdit::OnReflectedCommand( nNotifyCode, bNotifyParent );
	}

	// Reflected WM_NOTIFY notification handlers.
	virtual LRESULT OnCorrectText( ENCORRECTTEXT *pCorrectText, BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnDragDropDone( NMHDR *pNMHDR,		    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnDropFiles( ENDROPFILES *pDropFiles,	    BOOL& bNotifyParent ) { return 1; }
	virtual LRESULT OnLink( ENLINK *pLink,			    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnMsgFilter( MSGFILTER *pMsgFilter,	    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnObjectPositions( OBJECTPOSITIONS *pPos,   BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnProtected( ENPROTECTED *pProtected,	    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnRequestResize( REQRESIZE *pReq,	    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSelChange( SELCHANGE *pSel,		    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnOleOpFailed( ENOLEOPFAILED *pOleOp,	    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSaveClipboard( ENSAVECLIPBOARD *pSave,    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnStopNoUndo( NMHDR *pNMHDR,		    BOOL& bNotifyParent ) { return 0; }

	// Handle the reflected WM_NOTIFY messages.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		switch (  pNMHDR->code )
		{
			case	EN_CORRECTTEXT:		return OnCorrectText(( ENCORRECTTEXT * )pNMHDR, bNotifyParent );
			case	EN_DRAGDROPDONE:	return OnDragDropDone( pNMHDR, bNotifyParent );
			case	EN_DROPFILES:		return OnDropFiles(( ENDROPFILES * )pNMHDR, bNotifyParent );
			case	EN_LINK:		return OnLink(( ENLINK * )pNMHDR, bNotifyParent );
			case	EN_MSGFILTER:		return OnMsgFilter(( MSGFILTER * )pNMHDR, bNotifyParent );
			case	EN_OBJECTPOSITIONS:	return OnObjectPositions(( OBJECTPOSITIONS * )pNMHDR, bNotifyParent );
			case	EN_PROTECTED:		return OnProtected(( ENPROTECTED * )pNMHDR, bNotifyParent );
			case	EN_REQUESTRESIZE:	return OnRequestResize(( REQRESIZE * )pNMHDR, bNotifyParent );
			case	EN_SELCHANGE:		return OnSelChange(( SELCHANGE * )pNMHDR, bNotifyParent );
			case	EN_OLEOPFAILED:		return OnOleOpFailed(( ENOLEOPFAILED * )pNMHDR, bNotifyParent );
			case	EN_SAVECLIPBOARD:	return OnSaveClipboard(( ENSAVECLIPBOARD * )pNMHDR, bNotifyParent );
			case	EN_STOPNOUNDO:		return OnStopNoUndo( pNMHDR, bNotifyParent );
		}
		return 0;
	}
};

#endif // _RICHEDIT_H_