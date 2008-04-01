#ifndef _EDIT_H_
#define _EDIT_H_
//
// edit.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "../../coords/rect.h"

// A ClsWindow derived class handling edit controls.
class ClsEdit : public ClsWindow
{
	_NO_COPY( ClsEdit );
public:
	// Default constructor. Does nothing.
	ClsEdit()
	{;}

	// Constructor. Initializes to the passed
	// handle.
	ClsEdit( HWND hWnd )
	{
		// Attach the handle so that it will not
		// get destroyed when the object is destroyed.
		Attach( hWnd );
	}

	// Destructor. Does nothing.
	virtual ~ClsEdit()
	{;}

	// Create an edit control.
	inline BOOL Create( ClsWindow *parent, const ClsRect& rBounds, UINT nID, LPCTSTR pszText = NULL, UINT nStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE |	ES_AUTOHSCROLL | ES_LEFT )
	{
		_ASSERT( m_hWnd == NULL ); // May not exist already.

		// Create the control.
		return ClsWindow::Create( 0, _T( "EDIT" ), pszText, nStyle, rBounds, parent ? parent->GetSafeHWND() : NULL, ( HMENU )( UINT_PTR )nID );
	}

	// Can we undo?
	inline BOOL CanUndo() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get undo status.
		return ( BOOL )::SendMessage( m_hWnd, EM_CANUNDO, 0, 0 );
	}

	// Get the character index at the given position.
	inline DWORD CharFromPos( int xPos, int yPos ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get character index.
		return ( DWORD )::SendMessage( m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM( xPos, yPos ));
	}

	// Empty the undo buffer.
	inline void EmptyUndoBuffer()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Empty the buffer.
		SendMessage( EM_EMPTYUNDOBUFFER );
	}

	// Format the lines.
	inline void FmtLines( BOOL bAddEOL = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Format...
		SendMessage( EM_FMTLINES, bAddEOL );
	}

	// Get the index of the first visible line.
	inline UINT GetFirstVisibleLine() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( UINT )::SendMessage( m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0 );
	}

	// Get the handle of the text memory.
	inline HLOCAL GetHandle() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the handle.
		return ( HLOCAL )::SendMessage( m_hWnd, EM_GETHANDLE, 0, 0 );
	}

	// Get the text limit.
	inline UINT GetLimitText() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the limit.
		return ( UINT )::SendMessage( m_hWnd, EM_GETLIMITTEXT, 0, 0 );
	}

	// Get a line of the text.
	inline UINT GetLine( UINT nLine, LPCSTR pszLine ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the line.
		return ( UINT )::SendMessage( m_hWnd, EM_GETLINE, nLine, ( LPARAM )pszLine );
	}

	// Get the number of lines in the text.
	inline UINT GetLineCount() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Return the line count.
		return ( UINT )::SendMessage( m_hWnd, EM_GETLINECOUNT, 0, 0 );
	}

	// Get the margins.
	inline DWORD GetMargins() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the margins.
		return ( DWORD )::SendMessage( m_hWnd, EM_GETMARGINS, 0, 0 );
	}

	// Modified?
	inline BOOL GetModify() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get modified state.
		return ( BOOL )::SendMessage( m_hWnd, EM_GETMODIFY, 0, 0 );
	}

	// Get the password character.
	inline TCHAR GetPasswordChar() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the password character.
		return ( TCHAR )::SendMessage( m_hWnd, EM_GETPASSWORDCHAR, 0, 0 );
	}

	// Get the formatting rectangle.
	inline void GetRect( LPRECT pRect ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the rectangle.
		::SendMessage( m_hWnd, EM_GETRECT, 0, ( LPARAM )pRect );
	}

	// Get the selection.
	inline DWORD GetSel( DWORD& dwStart, DWORD& dwEnd ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the selection.
		return ( DWORD )::SendMessage( m_hWnd, EM_GETSEL, ( WPARAM )&dwStart, ( LPARAM )&dwEnd );
	}

	// Get the scroller (thumb) position.
	inline UINT GetThumb() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the position.
		return ( UINT )::SendMessage( m_hWnd, EM_GETTHUMB, 0, 0 );
	}

	// Get the wordbreak function.
	inline EDITWORDBREAKPROC GetWordBreakProc() const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the procedure address.
		return ( EDITWORDBREAKPROC )::SendMessage( m_hWnd, EM_GETWORDBREAKPROC, 0, 0 );
	}

	// Set the text limit.
	inline void LimitText( UINT nLimit )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the text limit.
		SendMessage( EM_LIMITTEXT, nLimit );
	}

	// Get the line index from the character index.
	inline UINT LineFromChar( int ich ) const 
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( UINT )::SendMessage( m_hWnd, EM_LINEFROMCHAR, ich, 0 );
	}

	// Get the character index from the line index.
	inline UINT LineIndex( UINT nIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( UINT )::SendMessage( m_hWnd, EM_LINEINDEX, nIndex, 0 );
	}

	// Get the line length from the character index.
	inline UINT LineLength( int ich ) const 
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the index.
		return ( UINT )::SendMessage( m_hWnd, EM_LINELENGTH, ich, 0 );
	}

	// Scroll the text.
	inline BOOL LineScroll( int cxScroll, int cyScroll )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Scroll the text.
		return ( BOOL )SendMessage( EM_LINESCROLL, cxScroll, cyScroll );
	}

	// Get the position of the character.
	inline DWORD PosFromChar( UINT nCharIndex ) const
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Get the position.
		return ( DWORD )::SendMessage( m_hWnd, EM_POSFROMCHAR, nCharIndex, 0 );
	}

	// Replace the selection.
	inline void ReplaceSel( LPCTSTR pszReplace, BOOL bCanUndo = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Replace the selection.
		SendMessage( EM_REPLACESEL, bCanUndo, ( LPARAM )pszReplace );
	}

	// Scroll the text.
	inline DWORD Scroll( int nScroll )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Scroll text.
		return ( DWORD )SendMessage( EM_SCROLL, nScroll );
	}

	// Scroll the caret.
	inline DWORD ScrollCaret()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Scroll the caret into the view.
		return ( DWORD )SendMessage( EM_SCROLLCARET );
	}

	// Set the text memory handle.
	inline void SetHandle( HLOCAL hText )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the handle.
		SendMessage( EM_SETHANDLE, ( WPARAM )hText );
	}

	// Set the text limite.
	inline void SetLimitText( int nChars )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Limit the text.
		SendMessage( EM_SETLIMITTEXT, nChars );
	}

	// Set the margins.
	inline void SetMargins( UINT nFlags, UINT nLeft, UINT nRight )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the margins.
		SendMessage( EM_SETMARGINS, nFlags, MAKELONG( nLeft, nRight ));
	}

	// Set modified flag.
	inline void SetModify( BOOL bModified = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the flag.
		SendMessage( EM_SETMODIFY, bModified );
	}

	// Set the password character.
	inline void SetPasswordChar( TCHAR ch )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the character.
		SendMessage( EM_SETPASSWORDCHAR, ch );
	}

	// Set readonly flag.
	inline BOOL SetReadOnly( BOOL bReadOnly = TRUE )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the flag.
		return ( BOOL )SendMessage( EM_SETREADONLY, bReadOnly );
	}

	// Set the  formatting rectangle.
	inline void SetRect( LPRECT pRect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the rectangle.
		SendMessage( EM_SETRECT, 0, ( LPARAM )pRect );
	}

	// Set the formatting rectangle.
	inline void SetRectNP( LPRECT pRect )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the rectangle.
		SendMessage( EM_SETRECTNP, 0, ( LPARAM )pRect );
	}

	// Set the selection.
	inline void SetSel( UINT nStart, UINT nEnd )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the selection.
		SendMessage( EM_SETSEL, nStart, nEnd );
	}

	// Set tabstops.
	inline BOOL SetTabStops( int cTabs, LPDWORD lpdwTabs )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the tab-stops.
		return ( BOOL )SendMessage( EM_SETTABSTOPS, cTabs, ( LPARAM )lpdwTabs );
	}

	// Set the word-break procedure.
	inline void SetWordBreakProc( EDITWORDBREAKPROC lpfnProc )
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Set the procedure.
		SendMessage( EM_SETWORDBREAKPROC, 0, ( LPARAM )lpfnProc );
	}

	// Undo last operation.
	inline BOOL Undo()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Undo the operation.
		return ( BOOL )SendMessage( EM_UNDO );
	}

	// Copy the selection.
	inline void Copy()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Copy the selection.
		SendMessage( WM_COPY );
	}

	// Cut the selection.
	inline void Cut()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Cut the selection.
		SendMessage( WM_CUT );
	}
	
	// Paste the clipboard contents.
	inline void Paste()
	{
		_ASSERT_VALID( GetSafeHWND() ); // Must be valid.

		// Copy the selection.
		SendMessage( WM_PASTE );
	}

protected:
	// Reflected WM_COMMAND notification handlers.
	virtual LRESULT OnChange(    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnErrSpace(  BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnHScroll(   BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnKillFocus( BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnMaxText(   BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnSetFocus(  BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnUpdate(    BOOL& bNotifyParent ) { return 0; }
	virtual LRESULT OnVScroll(   BOOL& bNotifyParent ) { return 0; }

	// Handle the reflected WM_COMMAND messages.
	virtual LRESULT OnReflectedCommand( UINT nNotifyCode, BOOL& bNotifyParent )
	{
		switch ( nNotifyCode )
		{
			case	EN_CHANGE:
				return OnChange( bNotifyParent );

			case	EN_ERRSPACE:
				return OnErrSpace( bNotifyParent );

			case	EN_HSCROLL:
				return OnHScroll( bNotifyParent );

			case	EN_KILLFOCUS:
				return OnKillFocus( bNotifyParent );

			case	EN_MAXTEXT:
				return OnMaxText( bNotifyParent );

			case	EN_SETFOCUS:
				return OnSetFocus( bNotifyParent );

			case	EN_UPDATE:
				return OnUpdate( bNotifyParent );

			case	EN_VSCROLL:
				return OnVScroll( bNotifyParent );
		}
		return 1;
	}
};

#endif // _EDIT_H_