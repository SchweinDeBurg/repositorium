/*
 *	keyboard.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	All those special keys.
 */
struct SpecialKey
{
	LPCTSTR		lpszKey;
	int		cCode;
} Keys[] = {
	{ _T("LButton"),    0x01 },
	{ _T("RButton"),    0x02 },
	{ _T("Cancel"),	    0x03 },
	{ _T("MButton"),    0x04 },
	{ _T("Back"),       0x08 },
	{ _T("Tab"),        0x09 },
	{ _T("Clear"),      0x0C },
	{ _T("Return"),     0x0D },
	{ _T("Pause"),      0x13 },
	{ _T("Escape"),     0x1B },
	{ _T("Space"),      0x20 },
	{ _T("PageUp"),     0x21 },
	{ _T("PageDown"),   0x22 },
	{ _T("End"),        0x23 },
	{ _T("Home"),       0x24 },
	{ _T("Left"),       0x25 },
	{ _T("Up"),         0x26 },
	{ _T("Right"),      0x27 },
	{ _T("Down"),       0x28 },
	{ _T("Select"),     0x29 },
	{ _T("Execute"),    0x2B },
	{ _T("PrintScreen"),0x2C },
	{ _T("Insert"),     0x2D },
	{ _T("Delete"),     0x2E },
	{ _T("Help"),       0x2F },

	{ _T("LeftWindows"),	0x5B },
	{ _T("RightWindows"),	0x5C },
	{ _T("PopMenuKey"),	0x5D },

	{ _T("NP0"),        0x60 },
	{ _T("NP1"),        0x61 },
	{ _T("NP2"),        0x62 },
	{ _T("NP3"),        0x63 },
	{ _T("NP4"),        0x64 },
	{ _T("NP5"),        0x65 },
	{ _T("NP6"),        0x66 },
	{ _T("NP7"),        0x67 },
	{ _T("NP8"),        0x68 },
	{ _T("NP9"),        0x69 },
	{ _T("NP*"),        0x6A },
	{ _T("NP+"),        0x6B },
	{ _T("Separator"),  0x6C },
	{ _T("NP-"),        0x6D },
	{ _T("NP."),        0x6E },
	{ _T("NP/"),        0x6F },
	{ _T("F1"),         0x70 },
	{ _T("F2"),         0x71 },
	{ _T("F3"),         0x72 },
	{ _T("F4"),         0x73 },
	{ _T("F5"),         0x74 },
	{ _T("F6"),         0x75 },
	{ _T("F7"),         0x76 },
	{ _T("F8"),         0x77 },
	{ _T("F9"),         0x78 },
	{ _T("F10"),        0x79 },
	{ _T("F11"),        0x7A },
	{ _T("F12"),        0x7B },
	{ _T("F13"),        0x7C },
	{ _T("F14"),        0x7D },
	{ _T("F15"),        0x7E },
	{ _T("F16"),        0x7F },
	{ _T("F17"),        0x80 },
	{ _T("F18"),        0x81 },
	{ _T("F19"),        0x82 },
	{ _T("F20"),        0x83 },
	{ _T("F21"),        0x84 },
	{ _T("F22"),        0x85 },
	{ _T("F23"),        0x86 },
	{ _T("F24"),        0x87 },
	{ NULL,             0x00 }
};

/*
 *	Does the code belong to any of the
 *	keys in the special keys array above?
 */
BOOL IsSpecialKey( int cCode )
{
	struct SpecialKey	*sk = Keys;

	/*
	 *	Iterate keys.
	 */
	while ( sk->lpszKey )
	{
		/*
		 *	Is this it?
		 */
		if ( sk->cCode == cCode )
			return TRUE;

		/*
		 *	Next...
		 */
		sk++;
	}
	return FALSE;
}

/*
 *	Key up handler.
 */
static BOOL DoKeyUp( LPCLASSDATA lpcd, UINT uKeyCode )
{
        /*
         *	A qualifier key released?
         */
        if ( uKeyCode == VK_SHIFT )
	{
                lpcd->cQualifier &= ~QUAL_SHIFT;
		return TRUE;
        }
	else if ( uKeyCode == VK_CONTROL )
	{
                lpcd->cQualifier &= ~QUAL_CTRL;
		MouseOnHyperLink( lpcd );
		return TRUE;
        }
	else if ( uKeyCode == VK_MENU )
	{
                lpcd->cQualifier &= ~QUAL_ALT;
		return TRUE;
        }
	return FALSE;
}

/*
 *	Key down handler.
 */
static BOOL DoKeyDown( LPCLASSDATA lpcd, UINT uKeyCode, DWORD dwKeyData )
{
	LPHASH		lpHash;
        BYTE		bStates[ 256 ], bBuf[ 4 ] = { 0 };
	int		nLen, i;

	/*
	 *	Hide the tooltip.
	 */
	HideTooltip( lpcd );

	/*
	 *	Key in repeat mode?
	 */
	if ( dwKeyData & ( 1 << 30 )) lpcd->bRepeatMode = TRUE;
	else			      lpcd->bRepeatMode = FALSE;

	/*
	 *	A qualifier key pressed?
	 */
        if ( uKeyCode == VK_SHIFT )
	{
                lpcd->cQualifier |= QUAL_SHIFT;
                return TRUE;
        }
	else if ( uKeyCode == VK_CONTROL )
	{
                lpcd->cQualifier |= QUAL_CTRL;
		MouseOnHyperLink( lpcd );
                return TRUE;
        }
	else if ( uKeyCode == VK_MENU )
	{
                lpcd->cQualifier |= QUAL_ALT;
                return TRUE;
        }
	else if ( uKeyCode == VK_ESCAPE && lpcd->nDragTimerID != 0 )
	{
		/*
		 *	Clear current markers.
		 */
		ClearMark( lpcd );

		/*
		 *	Setup new markers.
		 */
		lpcd->ptStartPos = lpcd->ptSelStart = lpcd->ptSelEnd = lpcd->ptCaretPos;

		/*
		 *	Re-render.
		 */
		InvalidateRect( lpcd->hWnd, NULL, FALSE );

		/*
		 *	Swallow key...
		 */
		return TRUE;
	}

        /*
         *      Get keyboard state.
         */
        if ( GetKeyboardState( &bStates[ 0 ] ) == FALSE )
                return FALSE;

        /*
         *      Find the hash entry for this key.
         */
        for ( lpHash = Parser->aHash[ uKeyCode & HASHMASK ]; lpHash; lpHash = lpHash->lpNext )
	{
                if ( lpHash->cCode == ( TCHAR )uKeyCode && lpcd->cQualifier == lpHash->cQual )
                break;
        }

        /*
         *      If there is a hash entry
         *      we call it.
         */
        if ( lpHash )
	{
                /*
		 *	Execute all commands bound
                 *      to this key.
                 */
                ExecuteCList( lpHash->lpCommands, lpcd );
                return TRUE;
        }

	/*
	 *	We do _not_ try to translate the key-code
	 *	to ASCII when either the Control or Alt
	 *	qualifiers are set.
	 */
	if ( lpcd->cQualifier & ( QUAL_CTRL | QUAL_ALT ))
		return FALSE;

        /*
         *      Else show the ToAscii translation.
         */
        if (( nLen = ToAscii( uKeyCode, ( UINT )dwKeyData, &bStates[ 0 ], ( LPWORD )&bBuf[ 0 ], 0 )) > 0 )
	{
		for ( i = 0; i < nLen; i++ )
		{
			/*
			 *	We do not insert carriage returns or newlines.
			 */
			if ( bBuf[ i ] != _T( '\n' ) && bBuf[ i ] != _T( '\r' ))
			{
				lpcd->bKeyHandled = TRUE;
				InsertChar( lpcd, bBuf[ i ] );
			}
		}
                return TRUE;
        }
        return FALSE;
}

LRESULT OnKeyDown( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if (( lpcd->bKeyHandled = DoKeyDown( lpcd, ( UINT )wParam, ( DWORD )lParam )) == FALSE )
		return DefWindowProc( hWnd, WM_KEYDOWN, wParam, lParam );
	return 0;
}

LRESULT OnSysKeyDown( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if (( lpcd->bKeyHandled = DoKeyDown( lpcd, ( UINT )wParam, ( DWORD )lParam )) == FALSE )
		return DefWindowProc( hWnd, WM_SYSKEYDOWN, wParam, lParam );
	return 0;
}

LRESULT OnChar( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if ( ! lpcd->bKeyHandled && ! ( lpcd->cQualifier & ( QUAL_CTRL | QUAL_ALT )))
	{
		if ( _istprint(( TCHAR )wParam ))	
			InsertChar( lpcd, ( TCHAR )wParam );
	}
	
	lpcd->bKeyHandled = FALSE;
	return 0;
}

LRESULT OnSysChar( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPHASH		lpHash;
	UINT		uCode = wParam;

        /*
         *      Find the hash entry for this key.
         */
        for ( lpHash = Parser->aHash[ uCode & HASHMASK ]; lpHash; lpHash = lpHash->lpNext )
	{
                if ( lpHash->cCode == ( TCHAR )uCode && lpcd->cQualifier == lpHash->cQual )
                break;
        }

	/*
	 *	If we have mapped this key we will
	 *	not let the system process the message.
	 */
	if ( lpHash )
		return 0;

	/*
	 *	Clear the qualifiers.
	 */
	lpcd->cQualifier = 0;

	/*
	 *	Call the default window
	 *	procedure.
	 */
	return DefWindowProc( hWnd, WM_SYSCHAR, wParam, lParam );
}

LRESULT OnKeyUp( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if ( DoKeyUp( lpcd, ( UINT )wParam ) == FALSE )
		return DefWindowProc( hWnd, WM_KEYUP, wParam, lParam );
	return 0;
}

LRESULT OnSysKeyUp( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	if ( DoKeyUp( lpcd, ( UINT )wParam ) == FALSE )
		return DefWindowProc( hWnd, WM_SYSKEYUP, wParam, lParam );
	return 0;
}

/*
 *      De-allocate the hash table.
 */
void FreeHashTable( POOL pMemPool, LPHASH *lpTable )
{
        LPHASH		lpHash, lpNext;
	int		i;

        for ( i = 0; i < HASHSIZE; i++ )
	{
                for ( lpHash = lpTable[ i ]; lpHash; lpHash = lpNext )
		{
                        /*
                         *      Save off next hash pointer.
                         */
                        lpNext = lpHash->lpNext;

                        /*
                         *      Delete it's command list.
                         */
                        KillCList( lpHash->lpCommands, pMemPool );

                        /*
                         *      Free the hash entry itself.
                         */
                        FreePooled( pMemPool, lpHash );
                }
		lpTable[ i ] = NULL;
        }
}

/*
 *      Setup default key map.
 */
BOOL SetupHash( POOL pMemPool, LPHASH *lpTable )
{
	int			i;
	TCHAR			cCode, cQual;
        /*
         *	Default key setup.
         */
        struct
	{
                TCHAR	       *pcKeyStr;
		int		nFuncID;
		int		nType;
        } defmap[] = {
		{ _T("Up"),			CID_CARET_UP,			CTYPE_HARDCODED	},
		{ _T("Shift Up"),		CID_CARET_UPEXTEND,		CTYPE_HARDCODED },
		{ _T("Down"),			CID_CARET_DOWN,			CTYPE_HARDCODED },
		{ _T("Shift Down"),		CID_CARET_DOWNEXTEND,		CTYPE_HARDCODED },
		{ _T("Left"),			CID_CARET_LEFT,			CTYPE_HARDCODED },
		{ _T("Shift Left"),		CID_CARET_LEFTEXTEND,		CTYPE_HARDCODED },
		{ _T("Right"),			CID_CARET_RIGHT,		CTYPE_HARDCODED },
		{ _T("Shift Right"),		CID_CARET_RIGHTEXTEND,		CTYPE_HARDCODED },
		{ _T("Home"),			CID_CARET_SOL,			CTYPE_HARDCODED },
		{ _T("Shift Home"),		CID_CARET_SOLEXTEND,		CTYPE_HARDCODED },
		{ _T("End"),			CID_CARET_EOL,			CTYPE_HARDCODED },
		{ _T("Shift End"),		CID_CARET_EOLEXTEND,		CTYPE_HARDCODED },
		{ _T("Ctrl Home"),		CID_CARET_HOME,			CTYPE_HARDCODED },
		{ _T("Shift Ctrl Home"),	CID_CARET_HOMEEXTEND,		CTYPE_HARDCODED },
		{ _T("Ctrl End"),		CID_CARET_END,			CTYPE_HARDCODED },
		{ _T("Shift Ctrl End"),		CID_CARET_ENDEXTEND,		CTYPE_HARDCODED },
		{ _T("Ctrl Right"),		CID_CARET_NEXTWORD,		CTYPE_HARDCODED },
		{ _T("Shift Ctrl Right"),	CID_CARET_NEXTWORDEXTEND,	CTYPE_HARDCODED },
		{ _T("Ctrl Left"),		CID_CARET_PREVWORD,		CTYPE_HARDCODED },
		{ _T("Shift Ctrl Left"),	CID_CARET_PREVWORDEXTEND,	CTYPE_HARDCODED },
		{ _T("Shift Ctrl Tab"),		CID_CARET_TABBACKEXTEND,	CTYPE_HARDCODED },
		{ _T("PageUp"),			CID_CARET_VIEWUP,		CTYPE_HARDCODED },
		{ _T("Shift PageUp"),		CID_CARET_VIEWUPEXTEND,		CTYPE_HARDCODED },
		{ _T("PageDown"),		CID_CARET_VIEWDOWN,		CTYPE_HARDCODED },
		{ _T("Shift PageDown"),		CID_CARET_VIEWDOWNEXTEND,	CTYPE_HARDCODED },
		{ _T("Ctrl Up"),		CID_SCROLLVIEW_DOWN,		CTYPE_HARDCODED },
		{ _T("Ctrl Down"),		CID_SCROLLVIEW_UP,		CTYPE_HARDCODED },
		{ _T("Ctrl PageUp"),		CID_SCROLLVIEW_RIGHT,		CTYPE_HARDCODED },
		{ _T("Ctrl PageDown"),		CID_SCROLLVIEW_LEFT,		CTYPE_HARDCODED },

		{ _T("Ctrl d"),			CID_EDIT_INSERTDATE,		CTYPE_HARDCODED },
		{ _T("Ctrl Shift d"),		CID_EDIT_INSERTTIME,		CTYPE_HARDCODED },


		{ _T("Return"),			CID_EDIT_SPLITLINE,		CTYPE_HARDCODED },
		{ _T("Delete"),			CID_EDIT_DELETECHAR,		CTYPE_HARDCODED },
		{ _T("Ctrl z"),			CID_EDIT_UNDO,			CTYPE_HARDCODED },
		{ _T("Ctrl y"),			CID_EDIT_REDO,			CTYPE_HARDCODED },
		{ _T("Back"),			CID_EDIT_BACKSPACE,		CTYPE_HARDCODED },
		{ _T("Alt Delete"),		CID_EDIT_DELETELINE,		CTYPE_HARDCODED },
		{ _T("Shift Delete"),		CID_EDIT_DELETEEOL,		CTYPE_HARDCODED },
		{ _T("Ctrl p"),			CID_EDIT_PULL,			CTYPE_HARDCODED },
		{ _T("Alt u"),			CID_EDIT_WORDTOUPPER,		CTYPE_HARDCODED },
		{ _T("Alt l"),			CID_EDIT_WORDTOLOWER,		CTYPE_HARDCODED },
		{ _T("Alt s"),			CID_EDIT_WORDSWAPCASE,		CTYPE_HARDCODED },
		{ _T("Alt c"),			CID_EDIT_COPYLINE,		CTYPE_HARDCODED },
		{ _T("Insert"),			CID_EDIT_TOGGLEOVERWRITE,	CTYPE_HARDCODED },
		{ _T("Ctrl Delete"),		CID_EDIT_DELETEEOW,		CTYPE_HARDCODED },
		{ _T("Ctrl Back"),		CID_EDIT_DELETESOW,		CTYPE_HARDCODED },
		{ _T("Ctrl Shift Delete"),	CID_EDIT_DELETEWORD,		CTYPE_HARDCODED },
		{ _T("Tab"),			CID_EDIT_TAB,			CTYPE_HARDCODED },
		{ _T("Shift Tab"),		CID_EDIT_BACKTAB,		CTYPE_HARDCODED },
		{ _T("Alt Shift t" ),		CID_EDIT_SWAPLINES,		CTYPE_HARDCODED },
		{ _T("Ctrl u"),			CID_EDIT_LOWERCASESELECTION,	CTYPE_HARDCODED },
		{ _T("Ctrl Shift u"),		CID_EDIT_UPPERCASESELECTION,	CTYPE_HARDCODED },
		{ _T("Ctrl Alt u"),		CID_EDIT_SWAPCASESELECTION,	CTYPE_HARDCODED },

		{ _T("Ctrl v"),			CID_CLIP_PASTE,			CTYPE_HARDCODED },
		{ _T("Shift Insert"),		CID_CLIP_PASTE,			CTYPE_HARDCODED },
		{ _T("Ctrl c"),			CID_CLIP_COPY,			CTYPE_HARDCODED },
		{ _T("Alt c"),			CID_CLIP_COPYAPPEND,		CTYPE_HARDCODED	},
		{ _T("Ctrl x"),			CID_CLIP_CUT,			CTYPE_HARDCODED },
		{ _T("Alt x"),			CID_CLIP_CUTAPPEND,		CTYPE_HARDCODED },
		{ _T("Ctrl Shift c"),		CID_CLIP_COPYLINE,		CTYPE_HARDCODED },
		{ _T("Ctrl Shift x"),		CID_CLIP_CUTLINE,		CTYPE_HARDCODED },

		{ _T("Ctrl s"),			CID_FILE_SAVE,			CTYPE_HARDCODED },
		{ _T("Ctrl p" ),		CID_FILE_PRINT,			CTYPE_HARDCODED },

		{ _T("Ctrl F2"),		CID_BOOKMARK_TOGGLE,		CTYPE_HARDCODED },
		{ _T("Shift Ctrl F2"),		CID_BOOKMARK_CLEARALL,		CTYPE_HARDCODED },
		{ _T("F2"),			CID_BOOKMARK_GOTONEXT,		CTYPE_HARDCODED },
		{ _T("Shift F2"),		CID_BOOKMARK_GOTOPREV,		CTYPE_HARDCODED },

		{ _T("F3"),			CID_FIND_NEXT,			CTYPE_HARDCODED },
		{ _T("Shift F3"),		CID_FIND_PREV,			CTYPE_HARDCODED },
		{ _T("Ctrl F3"),		CID_FIND_NEXTWORD,		CTYPE_HARDCODED },
		{ _T("Shift Ctrl F3"),		CID_FIND_PREVWORD,		CTYPE_HARDCODED },
		{ _T("Ctrl ]"),			CID_FIND_MATCHBRACKET,		CTYPE_HARDCODED },
		{ _T("Shift Ctrl ]"),		CID_FIND_MATCHBRACKET_SELECT,	CTYPE_HARDCODED },

		{ _T("Ctrl g"),			CID_DIALOG_GOTO,		CTYPE_HARDCODED },
		{ _T("Ctrl f"),			CID_DIALOG_FIND,		CTYPE_HARDCODED },
		{ _T("Ctrl h"),			CID_DIALOG_REPLACE,		CTYPE_HARDCODED },
		{ _T("Alt Return"),		CID_DIALOG_PROPERTIES,		CTYPE_HARDCODED },
		{ _T("Ctrl F1"),		CID_DIALOG_ABOUTCONTROL,	CTYPE_HARDCODED },

		{ _T("Ctrl a"),			CID_MARK_ALL,			CTYPE_HARDCODED },
		{ _T("Ctrl Shift w"),		CID_MARK_WORD,			CTYPE_HARDCODED },
		{ _T("Ctrl Shift l"),		CID_MARK_LINE,			CTYPE_HARDCODED },
		{ _T("Escape"),			CID_MARK_CLEAR,			CTYPE_HARDCODED },

                { NULL, 0, 0 }
        };

	/*
	 *      Setup hardcoded key configuration.
	 */
	for ( i = 0; defmap[ i ].pcKeyStr; i++ )
	{
		if ( Str2CodeQual( defmap[ i ].pcKeyStr, &cCode, &cQual ))
		{
			/*
			 *      By default a simple
			 *      set of built-in commands
			 *      is bound.
			 */
			AddHash( pMemPool, lpTable, cCode, cQual, BuildCList( pMemPool, defmap[ i ].nType, defmap[ i ].nFuncID, CTYPE_END ));
                }
        }
        return TRUE;
}

/*
 *	Find a specific hash entry.
 */
LPHASH FindHash( LPHASH *lpTable, TCHAR cCode, TCHAR cQual )
{
	LPHASH		*p, lpHash;

	lpHash = *( p = &lpTable[ cCode & HASHMASK ] );

	while ( lpHash )
	{
		if ( lpHash->cCode == cCode && lpHash->cQual == cQual )
			return lpHash;
		lpHash = *( p = &lpHash->lpNext );
	}
	return NULL;
}

/*
 *      Add a key to the hash table.
 */
LPHASH AddHash( POOL pMemPool, LPHASH *lpTable, TCHAR cCode, TCHAR cQual, LPCLIST lpCList )
{
        LPHASH		*p, lpHash;

        lpHash = *( p = &lpTable[ cCode & HASHMASK ] );

        while ( lpHash )
	{
                /*
                 *      When this key hash already
                 *      exists we set it up for the
                 *      new configuration.
                 */
                if ( lpHash->cCode == cCode && lpHash->cQual == cQual )
		{
                        /*
                         *      Kill the current command list.
                         */
                        KillCList( lpHash->lpCommands, pMemPool );

			/*
			 *	Setup new commands.
			 */
			lpHash->lpCommands = lpCList;

			return lpHash;
                }
		/*
		 *	Next...
		 */
                lpHash = *( p = &lpHash->lpNext );
        }

        /*
         *      Allocate a new hash entry.
         */
        if (( *p = lpHash = ( LPHASH )AllocPooled( pMemPool, sizeof( HASH ))) != NULL )
	{
                lpHash->lpNext     = NULL;
                lpHash->cCode      = cCode;
                lpHash->cQual	   = cQual;
                lpHash->lpCommands = lpCList;

                return lpHash;
        }

        return NULL;
}

/*
 *      Remove a key from the hash table.
 */
BOOL RemHash( POOL pMemPool, LPHASH *lpTable, TCHAR cCode, TCHAR cQual )
{
        LPHASH            *p, lpHash;

        lpHash = *( p = &lpTable[ cCode & HASHMASK ] );

        while ( lpHash )
	{
                if ( lpHash->cCode == cCode && lpHash->cQual == cQual )
		{
                        /*
                         *	Save of next hash pointer.
                         */
                        *p = lpHash->lpNext;

                        /*
                         *      Kill it's command list.
                         */
                        KillCList( lpHash->lpCommands, pMemPool );

                        /*
                         *      Free the hash entry.
                         */
                        FreePooled( pMemPool, lpHash );
                        return TRUE;
                }
                lpHash = *( p = &lpHash->lpNext );
        }

        return FALSE;
}

/*
 *      Convert a key string into
 *      a code and qualifier value.
 */
BOOL Str2CodeQual( LPCTSTR lpszKeyString, TCHAR *pcCode, TCHAR *pcQual )
{
	LPCTSTR		lpszBase = lpszKeyString;
	TCHAR		cQual = 0;
        SHORT           sScan;
        BOOL            bQualifier;
        int		i;

        if ( _tcslen( lpszKeyString ))
	{
                while ( *lpszKeyString )
		{
                        /*
                         *      Default to no qualifier added.
                         */
                        bQualifier = FALSE;

                        /*
                         *      Skip leading spaces.
                         */
                        while ( _istspace( *lpszKeyString ))
			{
                                if ( *lpszKeyString == 0 )
                                        break;
                                lpszKeyString++;
                        }

                        /*
                         *      Check for qualifier strings.
                         */
                        if ( ! _tcsnicmp( lpszKeyString, _T("Shift "), 6 ))
			{
				lpszKeyString += 6;
				cQual |= QUAL_SHIFT;
				bQualifier = TRUE;
			}
			else if ( ! _tcsnicmp( lpszKeyString, _T("Ctrl "), 5 ))
			{
				lpszKeyString += 5;
				cQual |= QUAL_CTRL;
				bQualifier = TRUE;
			}
			else if ( ! _tcsnicmp( lpszKeyString, _T("Alt "), 4 ))
			{
				lpszKeyString += 4;
				cQual |= QUAL_ALT;
				bQualifier = TRUE;
			}

                        /*
                         *      If no qualifier was found we stop looking
                         *      for the qualifier strings. This means that
                         *      all wanted qualifiers _must_ precede the
                         *      key that activates the routine.
                         *
                         *      Example:        Alt Ctrl a  ; correct
                         *                      Alt a Ctrl  ; wrong
                         */
                        if ( bQualifier == FALSE )
                                break;
                }

                /*
                 *      End-of-string? Reset the pointer.
                 */
                if ( *lpszKeyString == 0 )
                        lpszKeyString = lpszBase;

                /*
                 *      Skip leading spaces.
                 */
                while ( _istspace( *lpszKeyString )) lpszKeyString++;

                /*
                 *      When the rest of the string
                 *      is longer than 1 it's one of the
                 *      special keys.
                 */
                if ( _tcslen( lpszKeyString ) > 1 )
		{
                        /*
                         *      Setup qualifier.
                         */
                        *pcQual = cQual;
                        i = 0;

                        /*
                         *      Loop through the special string identifiers
                         *      to find the correct one.
                         */
                        while ( Keys[ i ].lpszKey )
			{
                                /*
                                 *      Is this the one?
                                 */
                                if ( ! _tcsnicmp( Keys[ i ].lpszKey, lpszKeyString, _tcslen( lpszKeyString )))
				{
                                        /*
                                         *      Setup key code and return ok.
                                         */
                                        *pcCode = ( TCHAR )Keys[ i ].cCode;
                                        return TRUE;
                                }
                                /*
                                 *      Next...
                                 */
                                i++;
                        }
                }
		else
		{
                        /*
                         *      Otherwise it is a single key character.
                         */
                        sScan = VkKeyScan( *lpszKeyString );

                        /*
                         *      Store the virtual key code
                         *      and qualifier.
                         */
                        *pcCode = ( TCHAR )( sScan & 0xFF );
                        *pcQual = ( TCHAR )((( sScan >> 8 ) & 0xFF ) | cQual );
                        return TRUE;
                }
        }

        /*
         *      Failure.
         */
        return FALSE;
}

/*
 *      Convert a code and qualifier
 *      value to a key string.
 */
void CodeQual2Str( TCHAR cCode, TCHAR cQual, TCHAR *lpszBuffer )
{
	TCHAR			bStates[ 256 ] = { 0 }, bBuf[ 4 ] = { 0 };
	int			i;

        *lpszBuffer = 0;

        /*
         *      Append qualifier strings.
         */
        if ( cQual & QUAL_SHIFT ) _tcscat( lpszBuffer, _T("Shift "));
        if ( cQual & QUAL_CTRL  ) _tcscat( lpszBuffer, _T("Ctrl "));
        if ( cQual & QUAL_ALT   ) _tcscat( lpszBuffer, _T("Alt "));

        /*
         *      Is it a special key?
         */
        for ( i = 0; i < sizeof( Keys ) / sizeof( struct SpecialKey ); i++ )
	{
                if ( Keys[ i ].lpszKey )
		{
                        if (( Keys[ i ].cCode & 0xFF ) == cCode )
			{
                                /*
                                 *      Append key string.
                                 */
                                _tcscat( lpszBuffer, Keys[ i ].lpszKey );
                                return;
                        }
                }
        }

        /*
	 *	Map key code.
         */
        if ( ToAscii( cCode, MapVirtualKey( cCode, 0 ), ( LPBYTE )&bStates[ 0 ], ( LPWORD )&bBuf[ 0 ], 0 ) > 0 )
                _tcscat( lpszBuffer, &bBuf[ 0 ] );
}
