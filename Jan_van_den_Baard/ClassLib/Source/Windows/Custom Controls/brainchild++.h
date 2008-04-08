#ifndef _BRAINCHILDPLUSPLUS_H_
#define _BRAINCHILDPLUSPLUS_H_
//
// brainchild++.h
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "../window.h"
#include "brainchild.h"

class ClsBrainchild : public ClsWindow
{
	_NO_COPY( ClsBrainchild );
public:
	// Construction/destruction.
	ClsBrainchild() {;}
	virtual ~ClsBrainchild() {;}

	// Implementation...
	BOOL Create( ClsWindow *pParent, ClsRect& rc, UINT nID = 0, DWORD dwStyle = WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_VISIBLE, DWORD dwExStyle = 0 )
	{
		_ASSERT( GetSafeHWND() == NULL );
		return ClsWindow::Create( dwExStyle, BRAINCHILD_CLASS, NULL, dwStyle, rc, pParent->GetSafeHWND(), ( HMENU )( UINT_PTR )nID );
	}

	inline int LoadFile( LPCTSTR pszFilename, BOOL bForce = FALSE )
	{
		_ASSERT( GetSafeHWND() );
		return ( int )SendMessage( BCM_LOADFILE, bForce, ( LPARAM )pszFilename );
	}

	inline int SaveFile( LPCTSTR pszFilename )
	{
		_ASSERT( GetSafeHWND() );
		return ( int )SendMessage( BCM_SAVEFILE, 0, ( LPARAM )pszFilename );
	}

	inline BOOL SetLogFont( LPLOGFONT pFont )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pFont );
		return ( BOOL )SendMessage( BCM_SETLOGFONT, 0, ( LPARAM )pFont );
	}

	inline BOOL GotoLine( int nLine )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_GOTOLINE, nLine );
	}

	inline BOOL SetFilename( LPCTSTR pszFilename )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pszFilename );
		return ( BOOL )SendMessage( BCM_SETFILENAME, 0, ( LPARAM )pszFilename );
	}

	inline void ExecuteCList( LPCOMLIST pList )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pList );
		SendMessage( BCM_EXECUTECLIST, 0, ( LPARAM )pList );
	}

	inline void ExecuteCommand( int nCommand )
	{
		_ASSERT( GetSafeHWND() );
		SendMessage( BCM_EXECUTECOMMAND, nCommand );
	}

	inline LPTSTR GetText( LPTSTR pszBuffer )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pszBuffer );
		return ( LPTSTR )SendMessage( BCM_GETTEXT, 0, ( LPARAM )pszBuffer );
	}

	inline HGLOBAL GetText()
	{
		_ASSERT( GetSafeHWND() );
		return ( HGLOBAL )SendMessage( BCM_GETTEXT );
	}

	inline DWORD GetTextLength()
	{
		_ASSERT( GetSafeHWND() );
		return ( DWORD )SendMessage( BCM_GETTEXTLENGTH );
	}

	inline LPTSTR GetWord( LPTSTR pszBuffer )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pszBuffer );
		return ( LPTSTR )SendMessage( BCM_GETWORD, 0, ( LPARAM )pszBuffer );
	}

	inline HGLOBAL GetWord()
	{
		_ASSERT( GetSafeHWND() );
		return ( HGLOBAL )SendMessage( BCM_GETWORD );
	}

	inline DWORD GetWordLength()
	{
		_ASSERT( GetSafeHWND() );
		return ( DWORD )SendMessage( BCM_GETWORDLENGTH );
	}

	inline LPTSTR GetLine( LPTSTR pszBuffer )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pszBuffer );
		return ( LPTSTR )SendMessage( BCM_GETLINE, 0, ( LPARAM )pszBuffer );
	}

	inline HGLOBAL GetLine()
	{
		_ASSERT( GetSafeHWND() );
		return ( HGLOBAL )SendMessage( BCM_GETLINE );
	}

	inline DWORD GetLineLength()
	{
		_ASSERT( GetSafeHWND() );
		return ( DWORD )SendMessage( BCM_GETLINELENGTH );
	}

	inline LPTSTR GetSelection( LPTSTR pszBuffer )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pszBuffer );
		return ( LPTSTR )SendMessage( BCM_GETSELECTION, 0, ( LPARAM )pszBuffer );
	}

	inline HGLOBAL GetSelection()
	{
		_ASSERT( GetSafeHWND() );
		return ( HGLOBAL )SendMessage( BCM_GETSELECTION );
	}

	inline DWORD GetSelectionLength()
	{
		_ASSERT( GetSafeHWND() );
		return ( DWORD )SendMessage( BCM_GETSELECTIONLENGTH );
	}

	inline BOOL CheckModified()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_CHECKMODIFIED );
	}

	inline BOOL SetText( LPCTSTR pszText, BOOL bForce = FALSE )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_SETTEXT, bForce, ( LPARAM )pszText );
	}
	
	inline void SyntaxColoring( BOOL bOn = TRUE, BOOL bGlobal = TRUE )
	{
		_ASSERT( GetSafeHWND() );
		SendMessage( BCM_SYNTAXCOLORING, bOn, bGlobal );
	}

	inline BOOL CanUndo()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_CANUNDO );
	}

	inline BOOL CanRedo()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_CANREDO );
	}

	inline BOOL CanCut()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_CANCUT );
	}

	inline BOOL CanCopy()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_CANCOPY );
	}

	inline BOOL CanPaste()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_CANPASTE );
	}

	inline BOOL CanDelete()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_CANDELETE );
	}

	inline BOOL Modified()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_MODIFIED );
	}

	inline BOOL Overwrite()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_OVERWRITE );
	}

	inline BOOL Readonly()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_READONLY );
	}

	inline int NumberOfLines()
	{
		_ASSERT( GetSafeHWND() );
		return ( int )SendMessage( BCM_NUMBEROFLINES );
	}

	inline BOOL AnyText()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_ANYTEXT );
	}

	inline BOOL GetFileInfo( LPFILEINFO pFileInfo )
	{
		_ASSERT( GetSafeHWND() );
		_ASSERT_VALID( pFileInfo );
		return ( BOOL )SendMessage( BCM_GETFILEINFO, 0, ( LPARAM )pFileInfo );
	}

	inline BOOL SetReadonly( BOOL bReadonly = TRUE )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_SETREADONLY, bReadonly );
	}
	
	inline void GotoDialog( LPCDLGTEMPLATE lpDlgTemplate = NULL )
	{
		_ASSERT( GetSafeHWND() );
		SendMessage( BCM_GOTO_DIALOG, 0, ( LPARAM )lpDlgTemplate );
	}

	inline void FindDialog( LPCDLGTEMPLATE lpDlgTemplate = NULL )
	{
		_ASSERT( GetSafeHWND() );
		SendMessage( BCM_FIND_DIALOG, 0, ( LPARAM )lpDlgTemplate );
	}

	inline void ReplaceDialog( LPCDLGTEMPLATE lpDlgTemplate = NULL )
	{
		_ASSERT( GetSafeHWND() );
		SendMessage( BCM_REPLACE_DIALOG, 0, ( LPARAM )lpDlgTemplate );
	}

	inline void PropertiesDialog( LPCDLGTEMPLATE lpDlgTemplate = NULL )
	{
		_ASSERT( GetSafeHWND() );
		SendMessage( BCM_PROPERTIES_DIALOG, 0, ( LPARAM )lpDlgTemplate );
	}

	inline void SettingsDialog()
	{
		_ASSERT( GetSafeHWND() );
		SendMessage( BCM_SETTINGS_DIALOG );
	}

	inline COLORREF SetColor( int nIndex, COLORREF crColor )
	{
		_ASSERT( GetSafeHWND() );
		return ( COLORREF )SendMessage( BCM_SETCOLOR, ( WPARAM )nIndex, crColor );
	}

	inline COLORREF GetColor( int nIndex )
	{
		_ASSERT( GetSafeHWND() );
		return ( COLORREF )SendMessage( BCM_GETCOLOR, ( WPARAM )nIndex );
	}

	inline BOOL InsertLine( LPCTSTR pszText, int nLine = -1 )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_INSERTLINE, ( WPARAM )nLine, ( LPARAM )pszText );
	}

	inline BOOL LoadParserFromMemory( HGLOBAL hGlobal )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_LOADPARSER, TRUE, ( LPARAM )hGlobal );
	}

	inline BOOL LoadParserFromFile( LPCTSTR pszFilename )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_LOADPARSER, FALSE, ( LPARAM )pszFilename );
	}

	inline BOOL GetUnicodeFlag()
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_GETUNICODEFLAG, 0, 0 );
	}

	inline BOOL SetUnicodeFlag( BOOL bUnicode )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_SETUNICODEFLAG, bUnicode, 0 );
	}

	inline BOOL ReplaceSelection( LPCTSTR pszText )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_REPLACESELECTION, 0, ( LPARAM )pszText );
	}

	inline BOOL Goto( const LPPOINT ptPosition )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_GOTO, 0, ( LPARAM )ptPosition );
	}

	inline BOOL SetSelectionPoints( LPCRECT pRect )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_SETSELECTIONPOINTS, 0, ( LPARAM )pRect );
	}

	inline BOOL GetSelectionPoints( LPRECT pRect )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_GETSELECTIONPOINTS, 0, ( LPARAM )pRect );
	}

	inline BOOL InsertText( LPCTSTR pszText )
	{
		_ASSERT( GetSafeHWND() );
		return ( BOOL )SendMessage( BCM_INSERTTEXT, 0, ( LPARAM )pszText );
	}

protected:
	// Overidables.
	virtual LRESULT OnCaretPosition( LPNMCARETPOSITION pCaretPos, BOOL& bNotifyParent )		{ return 0; }
	virtual LRESULT OnStatusUpdate( LPNMSTATUSUPDATE pStatusUpd, BOOL& bNotifyParent )		{ return 0; }
	virtual LRESULT OnFilenameChanged( LPNMFILENAMECHANGED pFilenameChg, BOOL& bNotifyParent )	{ return 0; }

	// Handle reflected notification messages and call the
	// virtual notification handlers.
	virtual LRESULT OnReflectedNotify( LPNMHDR pNMHDR, BOOL& bNotifyParent )
	{
		// What's the notification code?
		switch ( pNMHDR->code )
		{
			case	NMBC_CARETPOSITION:	    return OnCaretPosition(( LPNMCARETPOSITION )pNMHDR, bNotifyParent );
			case	NMBC_STATUSUPDATE:	    return OnStatusUpdate(( LPNMSTATUSUPDATE )pNMHDR, bNotifyParent );
			case	NMBC_FILENAMECHANGED:	    return OnFilenameChanged(( LPNMFILENAMECHANGED )pNMHDR, bNotifyParent );
		}
		// Pass to the base class.
		return ClsWindow::OnReflectedNotify( pNMHDR, bNotifyParent );
	}
};

#endif // _BRAINCHILDPLUSPLUS_H_