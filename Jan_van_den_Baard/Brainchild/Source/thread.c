/*
 *	thread.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	Controls are tracked on a per-process
 *	basis.
 */
typedef struct hwnd
{
	struct hwnd	*lpNext;
	struct hwnd	*lpPrev;
	HWND		 hWnd;
} WINDOW, FAR *LPWINDOW;

typedef struct
{
	LPWINDOW	lpFirst;
	LPWINDOW	lpEndMark;
	LPWINDOW	lpLast;
} WINLIST, FAR *LPWINLIST;

/*
 *	Each process attached to this DLL will get
 *	a memory pool and parser list in which the
 *	settings for the languages are loaded.
 */
POOL		pMainPool;
PARSERLIST	plParsers;
LPPARSER	lpDefault;

/*
 *	Printer page setup.
 */
PAGESETUPDLG	pdPage = { 0 };

/*
 *	Access mutex.
 */
HANDLE		hAccessMutex, hParserMutex;

/*
 *	Default file icons.
 */
//HICON		hDefIconSmall, hDefIconLarge;

/*
 *	Create a invisible top-level window so we
 *	can simply catch the registered message we
 *	use to signal that the settings are changed.
 */
HWND		hComBox;

/*
 *	Tracked windows.
 */
WINLIST		wlWindows;

/*
 *	Handle the settings update message.
 */
LRESULT WINAPI ComProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	/*
	 *	System color change?
	 */
	if ( uMsg == WM_SYSCOLORCHANGE )
	{
		/*
		 *	Relay the message to the controls in our list.
		 */
		LPWINDOW	pWin;

		/*
		 *	Iterate tracked windows.
		 */
		for ( pWin = wlWindows.lpFirst; pWin->lpNext; pWin = pWin->lpNext )
			/*
			 *	Send the message...
			 */
			SendMessage( pWin->hWnd, WM_SYSCOLORCHANGE, 0, 0 );
	}
	else
	/*
	 *	The registered message?
	 */
	if ( uMsg == uRegisteredMsg )
	{
		/*
		 *	Update the settings.
		 */
		UpdateSettings();
		return 0;
	}

	/*
	 *	Call the default message handler.
	 */
	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}

/*
 *	Return the registered settings
 *	update message.
 */
UINT GetUpdateMessage( void )
{
	return uRegisteredMsg;
}

/*
 *	Check if the message is intended for one of
 *	the controls it's find or replace dialogs.
 */
BOOL IsNonModalDialogMessage( const LPMSG lpMsg )
{
	LPWINDOW	pWin;

	/*
	 *	Obtain the mutex.
	 */
	WaitForSingleObject( hAccessMutex, INFINITE );

	/*
	 *	Iterate the windows.
	 */
	for ( pWin = wlWindows.lpFirst; pWin->lpNext; pWin = pWin->lpNext )
	{
		/*
		 *	Obtain it's data pointer.
		 */
		LPCLASSDATA lpcd = ( LPCLASSDATA )GetWindowLong( pWin->hWnd, GWL_INSTANCEDATA );

		/*
		 *	Check if it belongs here...
		 */
		if (( lpcd->hFindWnd    && IsDialogMessage( lpcd->hFindWnd,    lpMsg )) ||
		    ( lpcd->hReplaceWnd && IsDialogMessage( lpcd->hReplaceWnd, lpMsg )))
		{
			/*
			 *	Yes. Stop here.
			 */
			ReleaseMutex( hAccessMutex );
			return TRUE;
		}
	}

	/*
	 *	Does not belong here.
	 */
	ReleaseMutex( hAccessMutex );
	return FALSE;
}

/*
 *	Find the window.
 */
static LPWINDOW FindWindowNode( HWND hWnd )
{
	LPWINDOW	pWin;

	/*
	 *	Iterate tracked windows.
	 */
	for ( pWin = wlWindows.lpFirst; pWin->lpNext; pWin = pWin->lpNext )
	{
		/*
		 *	Is this the one?
		 */
		if ( pWin->hWnd == hWnd )
			/*
			 *	Yes. Return a pointer to it's node.
			 */
			 return pWin;
	}
	return NULL;
}

/*
 *	Free a window.
 */
void FreeWindow( HWND hWnd )
{
	LPWINDOW	pWin;

	/*
	 *	Obtain the mutex.
	 */
	WaitForSingleObject( hAccessMutex, INFINITE );

	/*
	 *	Find the node.
	 */
	if (( pWin = FindWindowNode( hWnd )) != NULL )
	{
		/*
		 *	Remove and free it.
		 */
		Remove(( LPNODE )pWin );
		FreePooled( pMainPool, pWin );
	}

	/*
	 *	Release the mutex.
	 */
	ReleaseMutex( hAccessMutex );
}

/*
 *	Add a window.
 */
BOOL AddWindow( HWND hWnd )
{
	LPWINDOW	pWin;
	BOOL		bRC = FALSE;

	/*
	 *	Obtain the mutex.
	 */
	WaitForSingleObject( hAccessMutex, INFINITE );

	/*
	 *	Allocate node.
	 */
	if (( pWin = AllocPooled( pMainPool, sizeof( WINDOW ))) != NULL )
	{
		/*
		 *	Setup the node and add it.
		 */
		pWin->hWnd = hWnd;
		AddTail(( LPLIST )&wlWindows, ( LPNODE )pWin );
		bRC = TRUE;
	}

	/*
	 *	Release the mutex.
	 */
	ReleaseMutex( hAccessMutex );
	return bRC;
}


/*
 *	Free all window nodes.
 */
static void FreeWindows( void )
{
	LPWINDOW	pWin;

	/*
	 *	Free 'm all.
	 */
	while (( pWin = ( LPWINDOW )RemHead(( LPLIST )&wlWindows )) != NULL )
		FreePooled( pMainPool, pWin );
}

/*
 *	Add a default parser node when one
 *	was not loaded.
 */
static LPPARSER CreateDefaultParser( void )
{
	LPPARSER	lpp;

	/*
	 *	Allocate the node.
	 */
	if (( lpp = AllocPooled( pMainPool, sizeof( PARSER ))) != NULL )
	{
		/*
		 *	Create the block array.
		 */
		if (( lpp->lpaBlocks = ArrayCreate( 0, 5, sizeof( BLOCK ))) != NULL )
		{
			/*
			 *	Setup default stuff.
			 */
			DefaultParser( lpp );

			/*
			 *	Setup default keyboard
			 *	mappings.
			 */
			SetupHash( pMainPool, &lpp->aHash[ 0 ] );

			/*
			 *	Make us default.
			 */
			lpp->bDefault = TRUE;

			/*
			*	Make sure the fonts are
			*	created.
			*/
			if ( lpp->hScreenFont == NULL )
			{
				if (( lpp->hScreenFont = CreateFontIndirect( &lpp->lfScreenFont )) == NULL )
				{
					FreePooled( pMainPool, lpp );
					return FALSE;
				}
			}

			return lpp;
		}
		FreePooled( pMainPool, lpp );
	}
	return NULL;
}

/*
 *	Setup the current thread.
 */
BOOL SetupProcess( void )
{
	LPPARSER	lpp;
	BOOL		bDefAdded = FALSE;

	/*
	 *	Create the access mutex's.
	 */
	if (( hAccessMutex = CreateMutex( NULL, FALSE, NULL )) == NULL )
		return FALSE;
	if (( hParserMutex = CreateMutex( NULL, FALSE, NULL )) == NULL )
		return FALSE;

	/*
	 *	Initialize the lists.
	 */
	NewList(( LPLIST )&plParsers );
	NewList(( LPLIST )&wlWindows );

	/*
	 *	Create the combox window.
	 */
	if (( hComBox = CreateWindow( IPCCLASS, NULL, 0, 0, 0, 0, 0, NULL, NULL, hDLL, NULL )) == NULL )
		return FALSE;

	/*
	 *	Allocate memory pool.
	 */
	if (( pMainPool = GetMemoryPool( 4096L )) != NULL )
	{
		/*
		 *	Create the default parser.
		 */
		if (( lpDefault = CreateDefaultParser()) != NULL )
		{
			/*
			 *	Only load the parsers if the location
			 *	is known.
			 */
			if ( GetSettingsLocation())
			{
				/*
				*	Load the parser files.
				*/
				if ( LoadParserFiles( pMainPool, &plParsers ))
				{
					/*
					 *	Did we load a default parser?
					 */
					for ( lpp = plParsers.lpFirst; lpp->lpNext; lpp = lpp->lpNext )
					{
						/*
						 *	Default?
						 */
						if (( bDefAdded = lpp->bDefault ) == TRUE )
							break;
					}

					/*
 					 *	Add the create default if one was
					 *	not loaded.
					 */
					if ( bDefAdded == FALSE )
						AddHead(( LPLIST )&plParsers, ( LPNODE )lpDefault );
					return TRUE;
				}
				else
				{
					/*
					 *	Remove the first parser from the list
					 *	if it is the default one.
					 */
					if ( plParsers.lpFirst == lpDefault )
						Remove(( LPNODE )lpDefault );

					/*
					 *	Remove and free the nodes which did
					 *	load.
					 */
					FreeParserList( pMainPool, &plParsers );

					/*
					 *	Add the default parser which will be
					 *	used for all file types.
					 */
					AddHead(( LPLIST )&plParsers, ( LPNODE )lpDefault );
				}
			}
			else
			{
				/*
				 *	In case the settings location is not
				 *	known we use the default parser.
				 */
				AddHead(( LPLIST )&plParsers, ( LPNODE )lpDefault );
				return TRUE;
			}
			FreeParserNode( pMainPool, lpDefault );
		}
		/*
		 *	Deallocate the memory pool.
		 */
		FreeMemoryPool( pMainPool, TRUE );
	}
	return FALSE;
}

/*
 *	Kill the thread data.
 */
void KillProcess( void )
{
	/*
	 *	Cleanup printer data.
	 */
	if ( pdPage.hDevMode  ) GlobalFree( pdPage.hDevMode  );
	if ( pdPage.hDevNames ) GlobalFree( pdPage.hDevNames );

	/*
	 *	Destroy combox window.
	 */
	DestroyWindow( hComBox );

	/*
	 *	Memory pool allocated?
	 */
	if ( pMainPool != NULL )
	{
		/*
		 *	Is the default parser added
		 *	to the list?
		 */
		if ( plParsers.lpFirst != lpDefault )
		{
			/*
			 *	No, we must free it manually.
			 */
			 FreeParserNode( pMainPool, lpDefault );
		}
		/*
		 *	Free the parser files.
		 */
		FreeParserList( pMainPool, &plParsers );

		/*
		 *	Free tracked windows.
		 */
		FreeWindows();

		/*
		 *	Free the memory pool.
		 */
		FreeMemoryPool( pMainPool, TRUE );
	}

	/*
	 *	Destroy the access mutex's.
	 */
	if ( hAccessMutex ) CloseHandle( hAccessMutex );
	if ( hParserMutex ) CloseHandle( hParserMutex );
}

LRESULT OnDefaultParser( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Set the parser pointer
	 *	to the default parser.
	 */
	Parser = lpDefault;
	return 0;
}

LRESULT OnResetParser( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Reset the file name.
	 */
	SetFileName( lpcd, lpcd->szFileName );
	return 0;
}

LRESULT OnResetDefault( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	LPPARSER	lpp;

	/*
	 *	Find default parser.
	 */
	for ( lpp = plParsers.lpFirst; lpp->lpNext; lpp = lpp->lpNext )
	{
		if ( lpp->bDefault == TRUE )
			break;
	}

	/*
	 *	Reset defaults..
	 */
	lpcd->nUndoSize       = lpp->nUndoSize;
	lpcd->bSyntaxColoring = lpp->bGlobalSyntaxColoring;
	lpcd->nMaxTrack	      = lpp->nMaxTrack;
	return 0;
}

static void SetDefParser( void )
{
	LPWINDOW	pWin;

	/*
	 *	Iterate windows.
	 */
	for ( pWin = wlWindows.lpFirst; pWin->lpNext; pWin = pWin->lpNext )
		SendMessage( pWin->hWnd, BCM_DEFAULTPARSER, 0, 0 );
}


static void ResetParsers( void )
{
	LPWINDOW	pWin;

	/*
	 *	Iterate windows.
	 */
	for ( pWin = wlWindows.lpFirst; pWin->lpNext; pWin = pWin->lpNext )
	{
		SendMessage( pWin->hWnd, BCM_RESETPARSER, 0, 0 );
		SendMessage( pWin->hWnd, BCM_RESETDEFAULT, 0, 0 );
	}
}

/*
 *	Update the settings.
 */
void UpdateSettings( void )
{
	LPPARSER	lpp;
	PARSERLIST	pll;
	BOOL		bDefAdded = FALSE;

	/*
	 *	Initialize buffer lists.
	 */
	NewList(( LPLIST )&pll );

	/*
	 *	Load parser files into the new list.
	 */
	if ( LoadParserFiles( pMainPool, &pll ))
	{
		/*
		 *	Lock the list.
		 */
		LockParserList();

		/*
		 *	Set the default parser.
		 */
		SetDefParser();

		/*
		 *	Did we load a default parser?
		 */
		for ( lpp = pll.lpFirst; lpp->lpNext; lpp = lpp->lpNext )
		{
			/*
			 *	Default?
			 */
			if (( bDefAdded = lpp->bDefault ) == TRUE )
				break;
		}

		/*
		 *	Add the created default if one was
		 *	not loaded.
		 */
		if ( bDefAdded == FALSE )
			AddHead(( LPLIST )&pll, ( LPNODE )lpDefault );

		/*
		 *	Free all parsers except those which were
		 *	added after initial loading of the settings files.
		 */
		while (( lpp = ( LPPARSER )RemHead(( LPLIST )&plParsers )) != NULL )
		{
			/*
			 *	Was it added after settings loading?
			 */
			if ( lpp->bHasBeenAdded == TRUE )
				/*
				 *	Add it to the buffer.
				 */
				AddTail(( LPLIST )&pll, ( LPNODE )lpp );
			else
				/*
				 *	Free it...
				 */
				FreeParserNode( pMainPool, lpp );
		}

		/*
		 *	Copy the new situation.
		 */
		while (( lpp = ( LPPARSER )RemHead(( LPLIST )&pll )) != NULL )
			AddTail(( LPLIST )&plParsers, ( LPNODE )lpp );

		/*
		 *	Reset the parsers.
		 */
		ResetParsers();

		/*
		 *	Unlock list.
		 */
		UnlockParserList();
	}
}

/*
 *	Lock the parser list.
 */
void LockParserList( void )
{
	WaitForSingleObject( hParserMutex, INFINITE );
}

/*
 *	Unlock the parserlist.
 */
void UnlockParserList( void )
{
	ReleaseMutex( hParserMutex );
}
