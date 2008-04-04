/*
 *	defs.h
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 *
 *	General include file.
 */

#include "../../Brainchild/Source/brainchild.h"

#include "Resource.h"
#include "list.h"
#include "nofrag.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *	Just in case they are not defined.
 */
#ifndef GET_X_LPARAM
#define GET_X_LPARAM( lParam )	(( int )( short )LOWORD( lParam ))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM( lParam )	(( int )( short )HIWORD( lParam ))
#endif

/*
 *	Version/Revision info.
 */
#define BCVERSION	2
#define BCREVISION	4

/*
 *	Internal messages.
 */
#define BCM_DEFAULTPARSER	( BCM_BASE - 1 )
#define BCM_RESETPARSER		( BCM_BASE - 2 )
#define BCM_RESETDEFAULT	( BCM_BASE - 3 )
#define BCM_GETEXTENTIONS	( BCM_BASE - 5 )

/*
 *	Timer IDs.
 */
#define IDT_DRAGTIMER	900
#define IDT_PARSETIMER	901

/*
 *	IPC window class.
 */
#define IPCCLASS	_T( "BCCIPCClass" )

/*
 *	Offset to the window extra data.
 */
#define GWL_INSTANCEDATA	0

/*
 *	Rectangle width and height.
 */
#define RCWIDTH(rc)	( rc.right - rc.left )
#define RCPWIDTH(rc)	( rc->right - rc->left )
#define RCHEIGHT(rc)	( rc.bottom - rc.top )
#define RCPHEIGHT(rc)	( rc->bottom - rc->top )

/*
 *	Some other usefull macros.
 */
#define ROUND_SIZE(s)	((s+15)&-16)
#define REAL_SIZE(s)	((s)*sizeof(TCHAR))
#define GETLINE(l)	((LPLINE)ArrayGetAt( l->lpLines,l->ptCaretPos.y))
#define ISREADONLY	((lpcd->dwFileAttr & FILE_ATTRIBUTE_READONLY)==FILE_ATTRIBUTE_READONLY)

/*
 *	Case conversion types.
 */
#define NTYPE_TOUPPER	0
#define NTYPE_TOLOWER	1
#define NTYPE_SWAP	2

/*
 *	Dynamic arrays.
 */
typedef struct
{
	int	nSize;		/* Current array size. */
	int	nElemSize;	/* Size of a single element. */
	int	nMax;		/* Maximum number of elements. */
	int	nGrowBy;	/* Number of elements the arrow grows if necessary. */
	POOL	mpPool;		/* Array memory pool. */
	LPBYTE	lpElements;	/* The elements. */
} ARRAY, FAR *LPARRAY;

/*
 *	Colors used by this class.
 */
#define CARR_TEXT			0
#define CARR_BACKGROUND			1
#define CARR_BACKGROUND_READONLY	2
#define CARR_SELECTION_MARGIN		3
#define CARR_BOOKMARK			4
#define CARR_NUMBER			5
#define CARR_DELIMITER			6
#define CARR_SELECTED_TEXT		7
#define CARR_SELECTED_BKGND		8
#define CARR_LINE_NUMBERS		9
#define CARR_BACKGROUND_LINE_NUMBERS	10
#define CARR_BACKGROUND_NUMBER		11
#define CARR_BACKGROUND_DELIMITER	12
#define CARR_BRACKET_MATCH		13
#define CARR_LINE_HIGHLIGHT		14
#define CARR_HYPERLINK			15
#define CARR_BACKGROUND_HYPERLINK	16

#define CARR_SIZEOF			17

/*
 *	Key word hashing structure.
 */
typedef struct KeyHash
{
	struct KeyHash	       *lpNext;
	LPTSTR			pszKeyword;
	COLORREF		crColor;
	COLORREF		crBkColor;
	int			nLength;
} KEYHASH, FAR *LPKEYHASH;

/*
 *	String node to track
 *	strings.
 */
typedef struct string
{	struct string	       *lpNext;
	struct string	       *lpPrev;
	LPCTSTR			lpszString;
} STRING, FAR *LPSTRING;

/*
 *	And a list to track'm.
 */
typedef struct
{
	LPSTRING		lpFirst;
	LPSTRING		lpEnd;
	LPSTRING		lpLast;
	int			nNumNodes;
} STRINGLIST, FAR *LPSTRINGLIST;


/*
 *	String node to track
 *	smart indention strings.
 */
typedef struct smart
{	struct smart	       *lpNext;
	struct smart	       *lpPrev;
	LPTSTR			lpszSmartString;
	int			nDirection;
} SMART, FAR *LPSMART;

#define INDENT_LEFT		0
#define INDENT_RIGHT		1

/*
 *	And a list to track'm.
 */
typedef struct
{
	LPSMART			lpFirst;
	LPSMART			lpEnd;
	LPSMART			lpLast;
} SMARTLIST, FAR *LPSMARTLIST;

/*
 *	Text lines are stored in an array of
 *	the following structures.
 */
typedef struct Line
{
	DWORD		dwFlags;	/* Flags. See below. */
	int		nBlock;		/* Line color block. */
	int		nLength;	/* Length of the line. */
	int		nMax;		/* Maximum length of text buffer. */
	TCHAR*		pcText;		/* The text buffer. */
} LINE, FAR *LPLINE;

/*
 *	Line flags.
 */
#define LNF_BOOKMARK	0x00000001
#define LNF_FIXCASING	0x00000002

/*
 *	Undo/redo records are stored in an
 *	array of the following structures.
 */
typedef struct UndoRedo
{
	DWORD		dwFlags;	/* Undo/Redo flags. */
	POINT		ptStartPos;	/* Start position. */
	POINT		ptEndPos;	/* End position. */
	TCHAR*		pcText;		/* Record text. */
} UNDOREDO, FAR *LPUNDOREDO;

/*
 *	Undo-Redo flags.
 */
#define URF_GROUP	0x00000001
#define URF_INSERT	0x00000002
#define URF_DELETE	0x00000004

/*
 *	Structure for the Boyer/Moore
 *	skip table (Find/Replace).
 */
typedef struct {
	LPCTSTR			lpszSearchString;
	int			nLength;
	int			anDeltas[ UCHAR_MAX + 1 ];
	int			anBackDeltas[ UCHAR_MAX + 1 ];
} BOYERMOORE, FAR *LPBOYERMOORE;

typedef void			(*LPFUNC)( LPVOID );

/*
 *	Command definition node.
 */
typedef struct commNode
{
	struct commNode		*lpNext;
	struct commNode		*lpPrev;
	int			 nType;
	union
	{
		struct
		{
			TCHAR	*pcText;
		} sText;
		struct
		{
			LPFUNC   lpFunc;
		} sFunc;
	} uCommand;
} CNODE, FAR *LPCNODE;

/*
 *	Easy access to the union fields.
 */
#define pcStr	uCommand.sText.pcText
#define lpFunc	uCommand.sFunc.lpFunc

/*
 *	A list to chain them.
 */
typedef struct
{
	LPCNODE			lpFirst;
	LPCNODE			lpEnd;
	LPCNODE			lpLast;
} CLIST, FAR *LPCLIST;

/*
 *	Command table structure.
 */
typedef struct data *LPCLASSDATA;
typedef struct tagCommand
{
	int			nCommandID;
	void		       (*lpfnFunc)( LPCLASSDATA );
	LPCTSTR			pszCommandDesc;
} COMMAND, FAR *LPCOMMAND;

/*
 *	Keyboard handling.
 */
#define QUAL_SHIFT		0x01
#define QUAL_CTRL		0x02
#define QUAL_ALT		0x04

/*
 *	Hash table size and mask.
 */
#define HASHSIZE		64
#define HASHMASK		63

/*
 *	Hash structure for all bound key
 *	combinations.
 */
typedef struct Hash
{
	struct Hash	       *lpNext;
	TCHAR			cCode;
	TCHAR			cQual;
	LPCLIST			lpCommands;
} HASH, FAR *LPHASH;

/*
 *	Syntax coloring.
 */
typedef struct 
{
	COLORREF	crColor;
	COLORREF	crBgColor;
	int		nColumn;
	LPKEYHASH	pHash;
	WORD		wFlags;
} SYNTAX_COLOR;

#define SCF_HYPERLINK	0x0001

/*
 *	Block definitions...
 */
typedef struct block
{
	LPTSTR		pszName;
	COLORREF	crColor;
	COLORREF	crBgColor;
	BOOL		bStartOfLine;
	LPTSTR		pszStart;
	int		nStart;
	LPTSTR		pszEnd;
	int		nEnd;
	BOOL		bInLine;
} BLOCK, FAR *LPBLOCK;

/*
 *	End of block is end of line.
 */
#define END_OF_LINE		( LPTSTR )0xFFFFFFFF

#ifndef __cplusplus

/*
 *	OLE Drag-and-Drop.
 */
typedef struct
{
	IUnknownVtbl	       *lpVtbl;
	LPVOID			lpcd;
	int			nRefCount;
} UINTERFACE, FAR* LPUINTERFACE;

typedef struct
{
	IDataObjectVtbl	       *lpVtbl;
	LPVOID			lpcd;
	int			nRefCount;
} DOINTERFACE, FAR* LPDOINTERFACE;

typedef struct
{
	IDropSourceVtbl	       *lpVtbl;
	LPVOID			lpcd;
	int			nRefCount;
} DSINTERFACE, FAR* LPDSINTERFACE;

typedef struct
{
	IDropTargetVtbl	       *lpVtbl;
	LPVOID			lpcd;
	int			nRefCount;
} DTINTERFACE, FAR* LPDTINTERFACE;

#endif

/*
 *	File backup modes.
 */
#define BMODE_FILE_BAK		0
#define BMODE_FILE_BAK_EXT	1
#define BMODE_FILE_EXT_BAK	2
#define BMODE_FILE_EXT_PATH	3

/*
 *	Parser node. A list of these
 *	nodes is build for each
 *	supported language.
 */
typedef struct parser
{
	/*
	 *	Common node.
	 */
	struct parser	       *lpNext;
	struct parser	       *lpPrev;

	/*
	 *	File name.
	 */
	LPTSTR			pszFileName;

	/*
	 *	Language strings.
	 */
	LPTSTR			pszTitle;
	LPTSTR			pszExtentions;

	/*
	 *	Date format strings.
	 */
	LPTSTR			pszDateFormat;
	LPTSTR			pszTimeFormat;

	/*
	 *	Delimiters.
	 */
	LPTSTR			pszDelimiters;

	/*
	 *	Smart indention list.
	 */
	SMARTLIST		lSmartIndention;

	/*
	 *	Keyboard hashes.
	 */
	LPHASH			aHash[ HASHSIZE ];

	/*
	 *	Fonts.
	 */
	HFONT			hScreenFont;
	HFONT			hScreenFontUL;
	HFONT			hPrinterFont;
	LOGFONT			lfScreenFont;
	LOGFONT			lfScreenFontUL;
	LOGFONT			lfPrinterFont;
	SIZE			szCharSize;

	/*
	 *	Printer header and
	 *	footer stuff.
	 */
	LPTSTR			pszHeaderLeft;
	LPTSTR			pszHeaderCenter;
	LPTSTR			pszHeaderRight;
	LPTSTR			pszFooterLeft;
	LPTSTR			pszFooterCenter;
	LPTSTR			pszFooterRight;
	BOOL			bPrintHeader;
	BOOL			bPrintFooter;

	/*
	 *	Column marker.
	 */
	int			nMarkerOffset;
	BOOL			bColumnMarker;

	/*
	 *	Language options and
	 *	settings.
	 */
	BOOL			bAutoIndent;
	BOOL			bSelectionMargin;
	BOOL			bLineNumbers;
	BOOL			bClearTrailingSpaces;
	BOOL			bSearchCase;
	BOOL			bWholeWords;
	BOOL			bAutoMatch;
	BOOL			bVisibleMatchError;
	BOOL			bSmartIndent;
	BOOL			bDefault;
	BOOL			bDnDEnabled;
	BOOL			bPrintSyntaxColoring;
	BOOL			bSyntaxColoring;
	BOOL			bGlobalSyntaxColoring;
	BOOL			bPrefsChanged;
	BOOL			bPrintScreenFont;
	BOOL			bFastScroll;
	BOOL			bClearUndoSave;
	BOOL			bVisibleSpaces;
	BOOL			bHasBeenAdded;
	BOOL			bCaseFixing;
	BOOL			bSolidMargin;
	BOOL			bColorBracketMatches;
	BOOL			bHighlightCurrentLine;
	BOOL			bPrintLineNumbers;
	BOOL			bParseHyperLinks;
	BOOL			bShowHyperTips;

	int			nMaxTrack;
	int			nTabSize;
	int			nHighlighted;

	/*
	 *	Standard colors.
	 */
	COLORREF		crColors[ CARR_SIZEOF ];
	DWORD			dwColorFlags;

	/*
	 *	Syntax coloring...
	 */
	LPARRAY			lpaBlocks;
	TCHAR			cEscape;
	BOOL			bCaseOn;
	LPKEYHASH		aKeywords[ 256 ];

	/*
	 *	Misc. settings.
	 */
	int			nFileMode;
	int			nCaretType;
	int			nUndoSize;
	int			nFilterIndex;

	/*
	 *	File backups.
	 */
	BOOL			bBackup;
	LPTSTR			pszBackupPath;
	int			nBackupMode;
} PARSER, FAR *LPPARSER;

/*
 *	Color flags.
 */
#define CF_DEFAULT_TEXT			0x00000001
#define CF_DEFAULT_BACKGROUND		0x00000002
#define CF_DEFAULT_BACKGROUND_RO	0x00000004
#define CF_DEFAULT_MARGIN		0x00000008
#define CF_DEFAULT_BOOKMARK		0x00000010
#define CF_DEFAULT_NUMBER		0x00000020
#define CF_DEFAULT_DELIMITER		0x00000040
#define CF_DEFAULT_SELECTED_TEXT	0x00000080
#define CF_DEFAULT_SELECTED_BKGND	0x00000100
#define CF_DEFAULT_LINE_NUMBERS		0x00000200
#define CF_DEFAULT_LINE_NUMBERS_BKGND	0x00000400
#define CF_DEFAULT_BACKGROUND_NUMBER	0x00000800
#define CF_DEFAULT_BACKGROUND_DELIMITER 0x00001000
#define CF_DEFAULT_BRACKET_MATCH	0x00002000
#define CF_DEFAULT_LINE_HIGHLIGHT	0x00004000
#define CF_DEFAULT_HYPERLINK		0x00008000
#define CF_DEFAULT_BACKGROUND_HYPERLINK 0x00010000
#define CF_DEFAULT_COLORS		CF_DEFAULT_TEXT | CF_DEFAULT_BACKGROUND | CF_DEFAULT_BACKGROUND_RO | CF_DEFAULT_MARGIN | CF_DEFAULT_BOOKMARK | CF_DEFAULT_NUMBER | CF_DEFAULT_DELIMITER | CF_DEFAULT_SELECTED_TEXT | CF_DEFAULT_SELECTED_BKGND | CF_DEFAULT_LINE_NUMBERS | CF_DEFAULT_LINE_NUMBERS_BKGND | CF_DEFAULT_BACKGROUND_NUMBER | CF_DEFAULT_BACKGROUND_DELIMITER | CF_DEFAULT_BRACKET_MATCH | CF_DEFAULT_LINE_HIGHLIGHT | CF_DEFAULT_HYPERLINK | CF_DEFAULT_BACKGROUND_HYPERLINK

/*
 *	List to track parser nodes.
 */
typedef struct
{
	LPPARSER		lpFirst;
	LPPARSER		lpEnd;
	LPPARSER		lpLast;
} PARSERLIST, FAR *LPPARSERLIST;

#ifndef __cplusplus

/*
 *	Instance data for windows created
 *	by this class.
 */
typedef struct data
{
	/*
	 *	Owner window handle.
	 */
	HWND		hWnd;

	/*
	 *	Memory pool for general allocations.
	 */
	POOL		pMemPool;

	/*
	 *	Cursors.
	 */
	HCURSOR		hIBeam;
	HCURSOR		hArrow;
	HCURSOR		hSelection;
	HCURSOR		hWait;
	HCURSOR		hHand;

	/*
	 *	Pattern bitmap for the
	 *	selection margin and the
	 *	line-number margin.
	 */
	HBITMAP		hPattern;
	HBITMAP		hPatternLine;

	/*
	 *	File.
	 */
	TCHAR		szFileName[ MAX_PATH ];
	DWORD		dwFileAttr;
	FILETIME	ftFileTime;

	/*
	 *	File icons.
	 */
	HICON			hIcon;
	HICON			hIconLarge;

	/*
	 *	Array of text lines.
	 */
	LPARRAY		lpLines;
	DWORD		dwParsedUpTo;

	/*
	 *	Array of undo/redo records.
	 */
	LPARRAY		lpUndoRedo;
	int		nUndoPosition;
	int		nSavePosition;
	int		nUndoSize;

	/*
	 *	Brushes used.
	 */
	HBRUSH		hbBackground;
	HBRUSH		hbBackgroundRO;
	HBRUSH		hbSelectionMargin;
	HBRUSH		hbSolidSelectionMargin;
	HBRUSH		hbBookmark;
	HBRUSH		hbSelBkgnd;
	HBRUSH		hbLineMargin;
	HBRUSH		hbLineMarginBkgnd;
	HBRUSH		hbLineHighlight;

	/*
	 *	Pens.
	 */
	HPEN		hpMargin;
	HPEN		hpColumnMarker;

	/*
	 *	Bitmap and device context
	 *	for offscreen rendering.
	 */
	HDC		hdcCompat;
	HBITMAP		hbmCompat;
	HBITMAP		hbmOld;

	/*
	 *	Caret and view.
	 */
	POINT		ptCaretPos;
	POINT		ptViewPos;
	SIZE		szViewSize;

	/*
	 *	Block markers
	 */
	POINT		ptSelStart;
	POINT		ptSelEnd;
	POINT		ptStartPos;
	BOOL		bLineSelect;
	BOOL		bWordSelect;
	BOOL		bAllSelect;
	int		nDragTimerID;

	/*
	 *	Bracket positions.
	 */
	POINT		ptBracket1;
	POINT		ptBracket2;

	/*
	 *	Parsing timer.
	 */
	int		nParseTimerID;

	/*
	 *	Find/Replace.
	 */
	BOYERMOORE	bmSkipTable;
	STRINGLIST	slSearchList;
	STRINGLIST	slReplaceList;
	LPCTSTR		lpszReplaceText;
	int		nMaxTrack;
	HWND		hFindWnd;
	HWND		hReplaceWnd;

	/*
	 *	Keyboard stuff.
	 */
	LPHASH		aHash[ HASHSIZE ];
	TCHAR		cQualifier;

	/*
	 *	Tooltips.
	 */
	HWND		hTooltip;
	BOOL		bHoverTracking;
	BOOL		bTipShowing;
	DWORD		dwPrevPos;

	/*
	 *	Misc.
	 */
	int		nLastColumnPos;
	int		nLongestLine;
	int		nBookmarkAnchor;
	int		nBookmarks;
	int		nWheelDelta;

	/*
	 *	Syntax coloring parsers.
	 */
	LPPARSER	lpCurrParser;
	SYNTAX_COLOR   *lpscColorArray;
	int		nColorBlocks;

	/*
	 *	Conversion buffer.
	 */
	TCHAR	       *pszBuffer;
	int		nBufLen;

	/*
	 *	OLE Drag-and-Drop
	 */
	IDataObjectVtbl	doVtbl;
	IDropSourceVtbl dsVtbl;
	IDropTargetVtbl dtVtbl;
	DOINTERFACE	ifDataObject;
	DSINTERFACE	ifDropSource;
	DTINTERFACE	ifDropTarget;
	BOOL		bTargetRegistered;
	BOOL		bDragSource;
	BOOL		bDragOver;

	/*
	 *	Notifications sent.
	 */
	POINT		ptLastPositionSent;
	NMSTATUSUPDATE	suLastSent;

	/*
	 *	Flags.
	 */
	BOOL		bCaretVisible;
	BOOL		bHasFocus;
	BOOL		bModified;
	BOOL		bBreakExec;
	BOOL		bRepeatMode;
	BOOL		bOpenAsReadOnly;
	BOOL		bOverwrite;
	BOOL		bRefreshBelow;
	BOOL		bSilentMode;
	BOOL		bActive;
	BOOL		bLocked;
	BOOL		bSyntaxColoring;
	BOOL		bKeyHandled;
	BOOL		bUnicode;
} CLASSDATA, FAR *LPCLASSDATA;

/*
 *	Macro to access the current parser.
 */
#define Parser		lpcd->lpCurrParser

#endif
 
/*
 *	The dispatcher uses an array of these
 *	structures.
 */
typedef struct {
	UINT		uMsgCode;
	LRESULT	     ( *lpMsgFunc )( HWND, WPARAM, LPARAM, LPCLASSDATA );
} MSGENTRY, FAR *LPMSGENTRY;

/*
 *	Table end marker.
 */
#define MSG_END		0xFFFFFFFF

/* dll.c */
extern HMODULE hDLL;
extern UINT uRegisteredMsg;

/* array.c */

typedef int ( __cdecl *COMPFUNC )( const void *, const void * );

extern void ArraySort( LPARRAY, COMPFUNC );
extern BOOL ArraySetSize( LPARRAY, int, int );
extern LPVOID ArrayGetAt( LPARRAY, int );
extern int ArrayGetSize( LPARRAY );
extern BOOL ArrayAdd( LPARRAY, LPVOID, int );
extern BOOL ArrayInsertAt( LPARRAY, int, LPVOID, int );
extern void ArrayRemoveAt( LPARRAY, int, int );
extern void ArrayRemoveAll( LPARRAY );
extern LPARRAY ArrayCreate( int, int, int );
extern void ArrayDelete( LPARRAY );
extern LPVOID ArrayAllocMem( LPARRAY, int );
extern void ArrayFreeMem( LPARRAY, LPVOID );
extern void ArrayReset( LPARRAY );

/* text.c */
extern LRESULT OnInsertText( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetUnicodeFlag( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSetUnicodeFlag( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnNumberOfLines( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCanUndo( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCanRedo( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetText( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetTextLength( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetWord( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetWordLength( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetLine( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetLineLength( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetSelection( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetSelectionLength( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSetText( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnInsertLine( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern void DeleteContents( LPCLASSDATA );
extern BOOL DeleteTheContents( LPCLASSDATA );
extern BOOL InsertLine( LPCLASSDATA, LPCTSTR, int, int );
extern BOOL LoadFile( LPCLASSDATA, LPCTSTR );
extern BOOL SaveFile( LPCLASSDATA, LPCTSTR );
extern LPTSTR GetText( LPCLASSDATA, LPPOINT, LPPOINT );
extern HGLOBAL GetTextGlobal( LPCLASSDATA, LPPOINT, LPPOINT );
extern LPTSTR GetTextBuffer( LPCLASSDATA, LPPOINT, LPPOINT, LPTSTR );
extern BOOL InsertText( LPCLASSDATA, int, int, LPCTSTR, LPPOINT, BOOL );
extern BOOL DeleteText( LPCLASSDATA, LPPOINT, LPPOINT, BOOL );
extern BOOL AddUndoRecord( LPCLASSDATA, DWORD, LPPOINT, LPPOINT, LPCTSTR );
extern void UndoAll( LPCLASSDATA );
extern void Undo( LPCLASSDATA );
extern void RedoAll( LPCLASSDATA );
extern void Redo( LPCLASSDATA );

/* dispatch.c */
extern LRESULT WINAPI Dispatch( HWND, UINT, WPARAM, LPARAM );

/* create.c */
extern LRESULT OnSysColorChange( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCreate( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnDestroy( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSetColor( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetColor( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* render.c */
extern SYNTAX_COLOR *GetColorBlocks( LPCLASSDATA, int );
extern LRESULT OnSyntaxColoring( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern void VScroll( LPCLASSDATA, int );
extern void HScroll( LPCLASSDATA, int );
extern void GetCharSize( HDC, LPCLASSDATA );
extern int GetMarginWidth( LPCLASSDATA );
extern int GetLineMarginWidth( LPCLASSDATA );
extern BOOL IsFixedWidth( HWND, HFONT );
extern void SetupViewSize( LPCLASSDATA );
extern void RenderLine( LPCLASSDATA, int );
extern void RenderLines( LPCLASSDATA, int, int );
extern COLORREF DarkenColor( COLORREF cr, double dRed, double dGreen, double dBlue );
extern COLORREF LightenColor( COLORREF cr, double dRed, double dGreen, double dBlue );

#ifdef _DEBUG
extern void ParserResult( LPCLASSDATA );
#endif

extern LRESULT OnEraseBkgnd( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnPrint( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnPrintClient( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnPaint( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSetFont( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSetLogFont( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSize( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* cursor.c */
extern LRESULT OnSetCursor( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* scrollers.c */
extern void SetupHScroller( LPCLASSDATA );
extern void SetupVScroller( LPCLASSDATA );

extern LRESULT OnHScroll( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnVScroll( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* caret.c */
extern void CreateTheCaret( LPCLASSDATA );
extern void SendCaretMessage( LPCLASSDATA );
extern int GetCaretOffset( LPCLASSDATA, int );
extern int GetTextOffset( LPCLASSDATA, int );
extern int CaretOffsetLine( LPCLASSDATA, int, int );
extern int TextOffsetLine( LPCLASSDATA, int, int, BOOL * );
extern void DisplayCaret( LPCLASSDATA, BOOL );
extern BOOL CaretInView( LPCLASSDATA );
extern void UpdateCaret( LPCLASSDATA );
extern void MakeCaretVisible( LPCLASSDATA );
extern void MakeCaretVisibleNoRedraw( LPCLASSDATA );
extern void CaretRight( LPCLASSDATA );
extern void CaretRightExtend( LPCLASSDATA );
extern void CaretLeft( LPCLASSDATA );
extern void CaretLeftExtend( LPCLASSDATA );
extern void CaretUp( LPCLASSDATA );
extern void CaretUpExtend( LPCLASSDATA );
extern void CaretDown( LPCLASSDATA );
extern void CaretDownExtend( LPCLASSDATA );
extern void CaretEol( LPCLASSDATA );
extern void CaretEolExtend( LPCLASSDATA );
extern void CaretSol( LPCLASSDATA );
extern void CaretSolExtend( LPCLASSDATA );
extern void CaretNextWord( LPCLASSDATA );
extern void CaretNextWordExtend( LPCLASSDATA );
extern void CaretPrevWord( LPCLASSDATA );
extern void CaretPrevWordExtend( LPCLASSDATA );
extern void CaretHome( LPCLASSDATA );
extern void CaretHomeExtend( LPCLASSDATA );
extern void CaretEnd( LPCLASSDATA );
extern void CaretEndExtend( LPCLASSDATA );
extern void CaretTabBack( LPCLASSDATA );
extern void CaretTabBackExtend( LPCLASSDATA );
extern void CaretViewUp( LPCLASSDATA );
extern void CaretViewUpExtend( LPCLASSDATA );
extern void CaretViewDown( LPCLASSDATA );
extern void CaretViewDownExtend( LPCLASSDATA );
extern void ScrollViewUp( LPCLASSDATA );
extern void ScrollViewDown( LPCLASSDATA );
extern void ScrollViewLeft( LPCLASSDATA );
extern void ScrollViewRight( LPCLASSDATA );

extern LRESULT OnSetFocus( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnKillFocus( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* clist.c */
extern LRESULT OnExecuteCList( HWND, WPARAM, LPARAM, LPCLASSDATA );

extern void ExecuteCList( LPCLIST, LPCLASSDATA );
extern LPCLIST BuildCList( POOL, DWORD, ... );
extern void KillCList( LPCLIST, POOL );
extern LPCLIST CopyCList( LPCLIST, POOL );

/* keyboard.c */
extern LRESULT OnKeyDown( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSysKeyDown( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnKeyUp( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSysKeyUp( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSysChar( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnChar( HWND, WPARAM, LPARAM, LPCLASSDATA );

extern BOOL IsSpecialKey( int );
extern void FreeHashTable( POOL, LPHASH * );
extern BOOL SetupHash( POOL, LPHASH * );
extern LPHASH FindHash( LPHASH *, TCHAR, TCHAR );
extern LPHASH AddHash( POOL, LPHASH *, TCHAR, TCHAR, LPCLIST );
extern BOOL RemHash( POOL, LPHASH *, TCHAR, TCHAR );
extern BOOL Str2CodeQual( LPCTSTR, TCHAR *, TCHAR * );
extern void CodeQual2Str( TCHAR, TCHAR, TCHAR * );

/* misc.c */
extern LRESULT OnModified( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnOverwrite( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnAnyText( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSettingsDialog( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetExtentions( HWND, WPARAM, LPARAM, LPCLASSDATA );

extern void ToggleOverwrite( LPCLASSDATA );
extern LPCTSTR GetString( int );
extern void SetModified( LPCLASSDATA, BOOL );
extern LPTSTR CopyString( LPCLASSDATA, LPCTSTR );
extern LPTSTR CopyStringPool( POOL, LPCTSTR );
extern BOOL AnyText( LPCLASSDATA );

extern void SendStatusMessage( LPCLASSDATA );

extern LRESULT OnGetDlgCode( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* io.c */
extern void FlushUndoBuffers( LPCLASSDATA );
extern void SaveFileAsIs( LPCLASSDATA );
extern void SetFileName( LPCLASSDATA, LPCTSTR );

extern LRESULT OnLoadFile( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSaveFile( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnReadOnly( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetFileInfo( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSetFileName( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCheckModified( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnSetReadOnly( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* edit.c */
extern void Pull( LPCLASSDATA );
extern void DeleteEol( LPCLASSDATA );
extern void BackSpace( LPCLASSDATA );
extern void DeleteChar( LPCLASSDATA );
extern void DeleteLine( LPCLASSDATA );
extern void InsertChar( LPCLASSDATA, TCHAR );
extern void InsertCharacters( LPCLASSDATA, LPCTSTR );
extern void SplitLine( LPCLASSDATA );
extern BOOL GetWordPoints( LPCLASSDATA, LPPOINT, LPPOINT );
extern void ConvertCase( LPCLASSDATA, LPPOINT, LPPOINT, int );
extern void WordToUpper( LPCLASSDATA );
extern void WordToLower( LPCLASSDATA );
extern void WordSwapCase( LPCLASSDATA );
extern void CopyLine( LPCLASSDATA );
extern void DeleteWord( LPCLASSDATA );
extern void DeleteEow( LPCLASSDATA );
extern void DeleteSow( LPCLASSDATA );
extern void SwapLines( LPCLASSDATA );

/* parser.c */
extern WORD HashKeyword( LPCTSTR, int, BOOL );
extern BOOL IsDelimiter( LPCLASSDATA, TCHAR );
extern void PreParseTo( LPCLASSDATA lpcd, int nTo );
extern int ParseLine( LPCLASSDATA lpcd, int, TCHAR *, int, SYNTAX_COLOR *, int *, int, int );

/* mouse.c */
extern BOOL MouseToCaret( LPCLASSDATA, int, int, LPPOINT );
extern BOOL MouseOnSelection( LPCLASSDATA );
extern LRESULT OnLButtonDown( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnLButtonUp( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnMouseMove( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnTimer( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnLButtonDblClk( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnMouseWheel( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnMouseHover( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnMouseLeave( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* parserlist.c */
extern void FreeParserNode( POOL, LPPARSER );
extern BOOL AddKeyword( POOL, LPPARSER, LPCTSTR, COLORREF, COLORREF );
extern BOOL SetupParser( LPCLASSDATA, LPCTSTR );
extern void FreeParserList( POOL, LPPARSERLIST );
extern BOOL LoadParserFiles( POOL, LPPARSERLIST );
extern LRESULT OnLoadParser( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern BOOL LoadParserFile( POOL, LPPARSERLIST, LPCTSTR, HGLOBAL, BOOL );
extern void DefaultParser( LPPARSER );
extern void RemoveTrailingBlanks( LPTSTR );

/* marks.c */
extern LRESULT OnSetSelectionPoints( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGetSelectionPoints( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern BOOL CheckPoint( LPCLASSDATA, LPPOINT );
extern void MarkWord( LPCLASSDATA );
extern void MarkLine( LPCLASSDATA );
extern BOOL MarkWholeLine( LPCLASSDATA, BOOL );
extern void MarkAll( LPCLASSDATA );
extern void ClearMark( LPCLASSDATA );
extern BOOL HasMark( LPCLASSDATA );
extern void UpperCaseSelection( LPCLASSDATA );
extern void LowerCaseSelection( LPCLASSDATA );
extern void SwapCaseSelection( LPCLASSDATA );

/* clip.c */
extern LRESULT OnReplaceSelection( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCanCutDelete( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCanCopy( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCanPaste( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCut( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnCopy( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnPaste( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnClear( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern void ClearClipboard( LPCLASSDATA );
extern void Paste( LPCLASSDATA );
extern BOOL Copy( LPCLASSDATA );
extern void Cut( LPCLASSDATA );
extern BOOL Delete( LPCLASSDATA );
extern void CopyClipLine( LPCLASSDATA );
extern void CutLine( LPCLASSDATA );
extern BOOL CopyAppend( LPCLASSDATA );
extern void CutAppend( LPCLASSDATA );

/* match.c */
extern BOOL MatchBracketSelect( LPCLASSDATA );
extern BOOL MatchBracket( LPCLASSDATA );
extern int FindMatches( LPCLASSDATA, LPPOINT, LPPOINT, BOOL );

/* messagebox.c */
extern int OkWarning( LPCLASSDATA, HWND, LPCTSTR, ... );
extern int OkInformation( LPCLASSDATA, HWND, LPCTSTR, ... );
extern int YesNoWarning( LPCLASSDATA, HWND, LPCTSTR, ... );
extern int YesNoCancelWarning( LPCLASSDATA, HWND, LPCTSTR, ... );
extern int OkIOError( LPCLASSDATA, HWND, LPCTSTR, ... );
extern int OkError( LPCLASSDATA, HWND, LPCTSTR, ... );
extern void ErrorMsg( LPCTSTR, ... );
extern void AboutControl( LPCLASSDATA );

/* bookmarks.c */
extern void SetBookmark( LPCLASSDATA );
extern void ClearBookmark( LPCLASSDATA );
extern void ToggleBookmark( LPCLASSDATA );
extern void ClearAllBookmarks( LPCLASSDATA lpcd );
extern void NextBookmark( LPCLASSDATA );
extern void PrevBookmark( LPCLASSDATA );
extern void FirstBookmark( LPCLASSDATA );
extern void LastBookmark( LPCLASSDATA );

/* goto.c */
extern void GotoDialog( LPCLASSDATA );

extern LRESULT OnGotoLine( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGotoDialog( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnGoto( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* search.c */
extern LPSTRING AddStringNode( LPCLASSDATA, LPSTRINGLIST, LPCTSTR, BOOL );
extern void FreeStringNodes( LPCLASSDATA, LPSTRINGLIST );
extern void SetSearchString( LPCLASSDATA, LPCTSTR );
extern void FindNextWord( LPCLASSDATA );
extern void FindPrevWord( LPCLASSDATA );
extern void FindNext( LPCLASSDATA );
extern void FindPrevious( LPCLASSDATA );
extern void FindDialog( LPCLASSDATA );
extern void ReplaceDialog( LPCLASSDATA );
extern int FindForward( LPCLASSDATA, TCHAR *, int );
extern int FindBackward( LPCLASSDATA, TCHAR *, int );

extern LRESULT OnFindDialog( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnReplaceDialog( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* registry.c */
extern TCHAR szSettingsPath[ MAX_PATH ];

extern BOOL CheckUser( LPTSTR );
extern BOOL GetSettingsLocation( void );
extern BOOL SaveSearchReplaceStrings( LPCLASSDATA, LPSTRINGLIST, BOOL );
extern BOOL LoadSearchReplaceStrings( LPCLASSDATA lpcd, LPSTRINGLIST lpList, BOOL bReplace );

/* prop.c */
extern void PropertiesDialog( LPCLASSDATA );

extern LRESULT OnPropDialog( HWND, WPARAM, LPARAM, LPCLASSDATA );

/* ole.c */
extern LRESULT OnShowWindow( HWND, WPARAM, LPARAM, LPCLASSDATA );

extern HGLOBAL GetSelectionGlobal( LPCLASSDATA );
extern void SetupInterfaces( LPCLASSDATA );

/* thread.c */
extern POOL		pMainPool;
extern PARSERLIST	plParsers;
extern PAGESETUPDLG	pdPage;

extern UINT GetUpdateMessage( void );
extern LRESULT OnDefaultParser( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnResetParser( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern LRESULT OnResetDefault( HWND, WPARAM, LPARAM, LPCLASSDATA );

extern LRESULT WINAPI ComProc( HWND, UINT, WPARAM, LPARAM );
extern void FreeWindow( HWND );
extern BOOL AddWindow( HWND );
extern BOOL SetupProcess( void );
extern void KillProcess( void );
extern void UpdateSettings( void );
extern void LockParserList( void );
extern void UnlockParserList( void );

/* indent.c */
extern TCHAR *CopyLeadingBlanks( LPCLASSDATA lpcd, int * );
extern void DoTab( LPCLASSDATA );
extern void DoBackTab( LPCLASSDATA );

/* commands.c */
extern LRESULT OnExecuteCommand( HWND, WPARAM, LPARAM, LPCLASSDATA );

extern LPCOMMAND GetCommandTable( void );
extern LPFUNC FindCommand( int );
extern int FindCommandID( LPFUNC );
extern LPCTSTR FindCommandDesc( LPFUNC );

/* date.c */
extern void InsertDate( LPCLASSDATA );
extern void InsertTime( LPCLASSDATA );

/* printer.c */
extern void Print( LPCLASSDATA );
extern LRESULT OnPageSetupDialog( HWND, WPARAM, LPARAM, LPCLASSDATA );
extern void PageSetupBody( LPCLASSDATA, BOOL );
extern void PageSetup( LPCLASSDATA );
extern void PageSetupDefault( LPCLASSDATA );

/* hyperlink.c */
extern BOOL PointOnHyperlink( LPCLASSDATA, int, int );
extern BOOL MouseOnHyperLink( LPCLASSDATA );
extern TCHAR *GetHyperlink( LPCLASSDATA );
extern BOOL RunHyperlink( LPCLASSDATA );

/* tooltip.c */
extern void SetTooltipText( LPCLASSDATA, LPCTSTR );
extern void ShowTooltip( LPCLASSDATA, LPCTSTR );
extern void HideTooltip( LPCLASSDATA );

#ifdef __cplusplus
}
#endif
