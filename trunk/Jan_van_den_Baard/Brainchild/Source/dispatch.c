/*
 *	dispatch.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Window class message dispatcher.
 */

#include "stdafx.h"

#include "defs.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL 0x020A
#endif

/*
 *	Message handler table.
 */
static MSGENTRY	msgTable[] =
{
	{ WM_PAINT,			OnPaint		},
	{ WM_PRINTCLIENT,		OnPrintClient	},
	{ WM_KEYDOWN,			OnKeyDown	},
	{ WM_SYSKEYDOWN,		OnSysKeyDown	},
	{ WM_KEYUP,			OnKeyUp		},
	{ WM_SYSKEYUP,			OnSysKeyUp	},
	{ WM_SYSCHAR,			OnSysChar	},
	{ WM_CHAR,			OnChar		},
	{ WM_ERASEBKGND,		OnEraseBkgnd	},
	{ WM_SIZE,			OnSize		},
	{ WM_CREATE,			OnCreate	},
	{ WM_DESTROY,			OnDestroy	},
	{ WM_SETFONT,			OnSetFont	},
	{ WM_SETCURSOR,			OnSetCursor	},
	{ WM_HSCROLL,			OnHScroll	},
	{ WM_VSCROLL,			OnVScroll	},
	{ WM_SETFOCUS,			OnSetFocus	},
	{ WM_KILLFOCUS,			OnKillFocus	},
	{ WM_GETDLGCODE,		OnGetDlgCode	},
	{ WM_LBUTTONDOWN,		OnLButtonDown	},
	{ WM_LBUTTONUP,			OnLButtonUp	},
	{ WM_RBUTTONDOWN,		OnLButtonUp	},
	{ WM_MOUSEMOVE,			OnMouseMove	},
	{ WM_MOUSEWHEEL,		OnMouseWheel	},
	{ WM_TIMER,			OnTimer		},
	{ WM_LBUTTONDBLCLK,		OnLButtonDblClk	},
	{ WM_SYSCOLORCHANGE,		OnSysColorChange},
	{ WM_SHOWWINDOW,		OnShowWindow	},
	{ WM_CUT,			OnCut		},
	{ WM_COPY,			OnCopy		},
	{ WM_PASTE,			OnPaste		},
	{ WM_CLEAR,			OnClear		},
	{ WM_MOUSEHOVER,		OnMouseHover	},
	{ WM_MOUSELEAVE,		OnMouseLeave	},

	{ BCM_LOADFILE,			OnLoadFile	},
	{ BCM_SAVEFILE,			OnSaveFile	},
	{ BCM_SETLOGFONT,		OnSetLogFont	},
	{ BCM_GOTOLINE,			OnGotoLine	},
	{ BCM_SETFILENAME,		OnSetFileName	},
	{ BCM_EXECUTECLIST,		OnExecuteCList	},
	{ BCM_EXECUTECOMMAND,		OnExecuteCommand},
	{ BCM_GETTEXT,			OnGetText	},
	{ BCM_GETTEXTLENGTH,		OnGetTextLength	},
	{ BCM_GETWORD,			OnGetWord	},
	{ BCM_GETWORDLENGTH,		OnGetWordLength	},
	{ BCM_GETLINE,			OnGetLine	},
	{ BCM_GETLINELENGTH,		OnGetLineLength	},
	{ BCM_CHECKMODIFIED,		OnCheckModified },
	{ BCM_SETTEXT,			OnSetText	},
	{ BCM_SYNTAXCOLORING,		OnSyntaxColoring},
	{ BCM_GETSELECTION,		OnGetSelection	},
	{ BCM_GETSELECTIONLENGTH,	OnGetSelectionLength},
	{ BCM_GETFILEINFO,		OnGetFileInfo	},
	{ BCM_SETREADONLY,		OnSetReadOnly	},
	{ BCM_GETCOLOR,			OnGetColor	},
	{ BCM_SETCOLOR,			OnSetColor	},
	{ BCM_INSERTLINE,		OnInsertLine	},
	{ BCM_LOADPARSER,		OnLoadParser	},
	{ BCM_GETUNICODEFLAG,		OnGetUnicodeFlag},
	{ BCM_SETUNICODEFLAG,		OnSetUnicodeFlag},
	{ BCM_REPLACESELECTION,		OnReplaceSelection},
	{ BCM_GOTO,			OnGoto		},
	{ BCM_SETSELECTIONPOINTS,	OnSetSelectionPoints},
	{ BCM_GETSELECTIONPOINTS,	OnGetSelectionPoints},
	{ BCM_INSERTTEXT,		OnInsertText	},

	{ BCM_CANUNDO,			OnCanUndo	},
	{ BCM_CANREDO,			OnCanRedo	},
	{ BCM_CANCUT,			OnCanCutDelete	},
	{ BCM_CANCOPY,			OnCanCopy	},
	{ BCM_CANPASTE,			OnCanPaste	},
	{ BCM_CANDELETE,		OnCanCutDelete	},
	{ BCM_MODIFIED,			OnModified	},
	{ BCM_OVERWRITE,		OnOverwrite	},
	{ BCM_READONLY,			OnReadOnly	},
	{ BCM_NUMBEROFLINES,		OnNumberOfLines },
	{ BCM_ANYTEXT,			OnAnyText	},

	{ BCM_GOTO_DIALOG,		OnGotoDialog	},
	{ BCM_FIND_DIALOG,		OnFindDialog	},
	{ BCM_REPLACE_DIALOG,		OnReplaceDialog	},
	{ BCM_PROPERTIES_DIALOG,	OnPropDialog	},
	{ BCM_SETTINGS_DIALOG,		OnSettingsDialog},
	{ BCM_PAGESETUP_DIALOG,		OnPageSetupDialog},

	{ BCM_DEFAULTPARSER,		OnDefaultParser	},
	{ BCM_RESETPARSER,		OnResetParser	},
	{ BCM_RESETDEFAULT,		OnResetDefault	},
	{ BCM_GETEXTENTIONS,		OnGetExtentions },
	{ MSG_END,			NULL		}
};

static BOOL bGotDestroy = FALSE;

LRESULT WINAPI Dispatch( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LPCLASSDATA	lpcd = ( LPCLASSDATA )GetWindowLong( hWnd, GWL_INSTANCEDATA );
	int	i = 0;

	/*
	 *	Valid table?
	 */
	while ( msgTable[ i ].uMsgCode != MSG_END )
	{
		/*
		 *	The message?
		 */
		if ( uMsg == msgTable[ i ].uMsgCode )
		{
			/*
			 *	Call the defined message handler. We only call the
			 *	message handlers when the window data is valid or when
			 *	the message is WM_CREATE. This prevents the message 
			 *	handlers being called after a WM_DESTROY.
			 *
			 *	During WM_DESTROY the data of the window is set to zero.
			 *	
			 */
			if ( lpcd || uMsg == WM_CREATE ) return ( msgTable[ i ].lpMsgFunc )( hWnd, wParam, lParam, lpcd );
			else				 return DefWindowProc( hWnd, uMsg, wParam, lParam );
		}
		/*
		 *	Next...
		 */
		i++;
	}

	/*
	 *	Call the default message handler.
	 */
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}
