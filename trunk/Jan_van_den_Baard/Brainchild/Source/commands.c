/*
 *	commands.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

COMMAND	ComTable[] = {
	{ CID_CARET_RIGHT,		CaretRight,		_T("CaretRight")		},
	{ CID_CARET_RIGHTEXTEND,	CaretRightExtend,	_T("CaretRightExtend")		},
	{ CID_CARET_LEFT,		CaretLeft,		_T("CaretLeft")			},
	{ CID_CARET_LEFTEXTEND,		CaretLeftExtend,	_T("CaretLeftExtend")		},
	{ CID_CARET_UP,			CaretUp,		_T("CaretUp")			},
	{ CID_CARET_UPEXTEND,		CaretUpExtend,		_T("CaretUpExtend")		},
	{ CID_CARET_DOWN,		CaretDown,		_T("CaretDown")			},
	{ CID_CARET_DOWNEXTEND,		CaretDownExtend,	_T("CaretDownExtend")		},
	{ CID_CARET_EOL,		CaretEol,		_T("CaretEndOfLine")		},
	{ CID_CARET_EOLEXTEND,		CaretEolExtend,		_T("CaretEndOfLineExtend")	},
	{ CID_CARET_SOL,		CaretSol,		_T("CaretStartOfLine")		},
	{ CID_CARET_SOLEXTEND,		CaretSolExtend,		_T("CaretStartOfLineExtend")	},
	{ CID_CARET_NEXTWORD,		CaretNextWord,		_T("CaretNextWord")		},
	{ CID_CARET_NEXTWORDEXTEND,	CaretNextWordExtend,	_T("CaretNextWordExtend")	},
	{ CID_CARET_PREVWORD,		CaretPrevWord,		_T("CaretPreviousWord")		},
	{ CID_CARET_PREVWORDEXTEND,	CaretPrevWordExtend,	_T("CaretPreviousWordExtend")	},
	{ CID_CARET_HOME,		CaretHome,		_T("CaretHome")			},
	{ CID_CARET_HOMEEXTEND,		CaretHomeExtend,	_T("CaretHomeExtend")		},
	{ CID_CARET_END,		CaretEnd,		_T("CaretEnd")			},
	{ CID_CARET_ENDEXTEND,		CaretEndExtend,		_T("CaretEndExtend")		},
	{ CID_CARET_TABBACK,		CaretTabBack,		_T("CaretTabBack")		},
	{ CID_CARET_TABBACKEXTEND,	CaretTabBackExtend,	_T("CaretTabBackExtend")	},
	{ CID_CARET_VIEWUP,		CaretViewUp,		_T("CaretViewUp")		},
	{ CID_CARET_VIEWUPEXTEND,	CaretViewUpExtend,	_T("CaretViewUpExtend")		},
	{ CID_CARET_VIEWDOWN,		CaretViewDown,		_T("CaretViewDown")		},
	{ CID_CARET_VIEWDOWNEXTEND,	CaretViewDownExtend,	_T("CaretViewDownExtend")	},

	{ CID_MISC_CLEARUNDOBUFFERS,	FlushUndoBuffers,	_T("MiscClearUndoBuffers")	},

	{ CID_SCROLLVIEW_UP,		ScrollViewUp,		_T("ScrollViewUp")		},
	{ CID_SCROLLVIEW_DOWN,		ScrollViewDown,		_T("ScrollViewDown")		},
	{ CID_SCROLLVIEW_LEFT,		ScrollViewLeft,		_T("ScrollViewLeft")		},
	{ CID_SCROLLVIEW_RIGHT,		ScrollViewRight,	_T("ScrollViewRight")		},

	{ CID_EDIT_UNDO,		Undo,			_T("EditUndo")			},
	{ CID_EDIT_REDO,		Redo,			_T("EditRedo")			},
	{ CID_EDIT_PULL,		Pull,			_T("EditPull")			},
	{ CID_EDIT_DELETEEOL,		DeleteEol,		_T("EditDeleteEndOfLine")	},
	{ CID_EDIT_BACKSPACE,		BackSpace,		_T("EditBackSpace")		},
	{ CID_EDIT_DELETECHAR,		DeleteChar,		_T("EditDeleteChar")		},
	{ CID_EDIT_DELETELINE,		DeleteLine,		_T("EditDeleteLine")		},
	{ CID_EDIT_SPLITLINE,		SplitLine,		_T("EditSplitLine")		},
	{ CID_EDIT_WORDTOUPPER,		WordToUpper,		_T("EditWordToUpper")		},
	{ CID_EDIT_WORDTOLOWER,		WordToLower,		_T("EditWordToLower")		},
	{ CID_EDIT_WORDSWAPCASE,	WordSwapCase,		_T("EditWordSwapCase")		},
	{ CID_EDIT_COPYLINE,		CopyLine,		_T("EditCopyLine")		},
	{ CID_EDIT_DELETEWORD,		DeleteWord,		_T("EditDeleteWord")		},
	{ CID_EDIT_DELETEEOW,		DeleteEow,		_T("EditDeleteEndOfWord")	},
	{ CID_EDIT_DELETESOW,		DeleteSow,		_T("EditDeleteStartOfWord")	},
	{ CID_EDIT_TAB,			DoTab,			_T("EditDoTab")			},
	{ CID_EDIT_BACKTAB,		DoBackTab,		_T("EditDoBackTab")		},
	{ CID_EDIT_SWAPLINES,		SwapLines,		_T("EditSwapLines")		},
	{ CID_EDIT_UPPERCASESELECTION,	UpperCaseSelection,	_T("EditUpperCaseSelection")	},
	{ CID_EDIT_LOWERCASESELECTION,	LowerCaseSelection,	_T("EditLowerCaseSelection")	},
	{ CID_EDIT_SWAPCASESELECTION,	SwapCaseSelection,	_T("EditSwapCaseSelection")	},
	{ CID_EDIT_INSERTDATE,		InsertDate,		_T("EditInsertDate")		},
	{ CID_EDIT_INSERTTIME,		InsertTime,		_T("EditInsertTime")		},
	{ CID_EDIT_TOGGLEOVERWRITE,	ToggleOverwrite,	_T("EditToggleOverwrite")	},
	{ CID_EDIT_UNDOALL,		UndoAll,		_T("EditUndoAll")		},
	{ CID_EDIT_REDOALL,		RedoAll,		_T("EditRedoAll")		},

	{ CID_MARK_WORD,		MarkWord,		_T("MarkWord")			},
	{ CID_MARK_ALL,			MarkAll,		_T("MarkAll")			},
	{ CID_MARK_CLEAR,		ClearMark,		_T("MarkClear")			},
	{ CID_MARK_LINE,		MarkLine,		_T("MarkLine")			},

	{ CID_CLIP_PASTE,		Paste,			_T("ClipPaste")			},
	{ CID_CLIP_COPY,		Copy,			_T("ClipCopy")			},
	{ CID_CLIP_CUT,			Cut,			_T("ClipCut")			},
	{ CID_CLIP_DELETE,		Delete,			_T("ClipDelete")		},
	{ CID_CLIP_COPYLINE,		CopyClipLine,		_T("ClipCopyClipLine")		},
	{ CID_CLIP_CUTLINE,		CutLine,		_T("ClipCutLine")		},
	{ CID_CLIP_CLEARCLIPBOARD,	ClearClipboard,		_T("ClipClearClipboard")	},
	{ CID_CLIP_COPYAPPEND,		CopyAppend,		_T("ClipCopyAppend")		},
	{ CID_CLIP_CUTAPPEND,		CutAppend,		_T("ClipCutAppend")		},

	{ CID_BOOKMARK_SET,		SetBookmark,		_T("BookmarkSet")		},
	{ CID_BOOKMARK_CLEAR,		ClearBookmark,		_T("BookmarkClear")		},
	{ CID_BOOKMARK_TOGGLE,		ToggleBookmark,		_T("BookmarkToggle")		},
	{ CID_BOOKMARK_CLEARALL,	ClearAllBookmarks,	_T("BookmarkClearAll")		},
	{ CID_BOOKMARK_GOTONEXT,	NextBookmark,		_T("BookmarkGotoNext")		},
	{ CID_BOOKMARK_GOTOPREV,	PrevBookmark,		_T("BookmarkGotoPrevious")	},
	{ CID_BOOKMARK_GOTOFIRST,	FirstBookmark,		_T("BookmarkGotoFirst")		},
	{ CID_BOOKMARK_GOTOLAST,	LastBookmark,		_T("BookmarkGotoLast")		},

	{ CID_FIND_NEXTWORD,		FindNextWord,		_T("FindNextWord")		},
	{ CID_FIND_PREVWORD,		FindPrevWord,		_T("FindPrevWord")		},
	{ CID_FIND_NEXT,		FindNext,		_T("FindNext")			},
	{ CID_FIND_PREV,		FindPrevious,		_T("FindPrevious")		},
	{ CID_FIND_MATCHBRACKET,	MatchBracket,		_T("FindMatchingBracket")	},
	{ CID_FIND_MATCHBRACKET_SELECT,	MatchBracketSelect,	_T("FindMatchingBracketSelect")	},

	{ CID_DIALOG_GOTO,		GotoDialog,		_T("DialogGoto")		},
	{ CID_DIALOG_FIND,		FindDialog,		_T("DialogFind")		},
	{ CID_DIALOG_REPLACE,		ReplaceDialog,		_T("DialogReplace")		},
	{ CID_DIALOG_PROPERTIES,	PropertiesDialog,	_T("DialogProperties")		},
	{ CID_DIALOG_ABOUTCONTROL,	AboutControl,		_T("DialogAbout")		},
	{ CID_DIALOG_PAGESETUP,		PageSetup,		_T("DialogPageSetup")		},

	{ CID_FILE_SAVE,		SaveFileAsIs,		_T("FileSave")			},
	{ CID_FILE_DELETECONTENTS,	DeleteTheContents,	_T("FileClear")			},
	{ CID_FILE_PRINT,		Print,			_T("FilePrint")			},

	{ -1,				NULL,			NULL				}
};

/*
 *	Return a pointer to the command table.
 */
LPCOMMAND Brainchild_GetCommandTable( void )
{
	return ComTable;
}

/*
 *	Find a command by it's identifier.
 */
LPFUNC Brainchild_FindCommand( int nCommandID )
{
	int		i = 0;

	/*
	 *	Iterate the table.
	 */
	while ( ComTable[ i ].lpfnFunc )
	{
		/*
		 *	Is this the one?
		 */
		if ( ComTable[ i ].nCommandID == nCommandID )
			/*
			 *	Yes. Return a pointer
			 *	to the function.
			 */
			return ( LPFUNC )ComTable[ i ].lpfnFunc;
		/*
		 *	Next...
		 */
		i++;
	}
	/*
	 *	Not found.
	 */
	return ( LPFUNC )NULL;
}

/*
 *	Find a command identifier by
 *	it's function pointer.
 */
int Brainchild_FindCommandID( LPFUNC lpfnFunc )
{
	int		i = 0;

	/*
	 *	Iterate the table.
	 */
	while ( ComTable[ i ].lpfnFunc )
	{
		/*
		 *	Is this the one?
		 */
		if ( ComTable[ i ].lpfnFunc == lpfnFunc )
			/*
			 *	Yes. Return the command ID.
			 */
			return ComTable[ i ].nCommandID;
		/*
		 *	Next...
		 */
		i++;
	}
	return -1;
}

/*
 *	Find a command descroption by
 *	it's function pointer.
 */
LPCTSTR Brainchild_FindCommandDesc( LPFUNC lpfnFunc )
{
	int		i = 0;

	/*
	 *	Iterate the table.
	 */
	while ( ComTable[ i ].lpfnFunc )
	{
		/*
		 *	Is this the one?
		 */
		if ( ComTable[ i ].lpfnFunc == lpfnFunc )
			/*
			 *	Yes. Return the command description.
			 */
			return ComTable[ i ].pszCommandDesc;
		/*
		 *	Next...
		 */
		i++;
	}
	return NULL;
}

LRESULT OnExecuteCommand( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPFUNC		lpfnFunction;

	/*
	 *	Find the command.
	 */
	if (( lpfnFunction = Brainchild_FindCommand( wParam )) != NULL )
		/*
		 *	Execute it.
		 */
		 ( lpfnFunction )( lpcd );
	return 0;
}
