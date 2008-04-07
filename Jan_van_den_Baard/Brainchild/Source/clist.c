/*
 *	clist.c
 *
 *	(C) Copyright 1993-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Command list stuff.
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	The data format passed on the stack
 *	for the BuildCList() routine.
 */
typedef struct
{
	DWORD		dwType;
	LPVOID		lpData;
} DATA, *LPDATA;

/*
 *	Convert a CNode string to a normal string.
 */
static LPCTSTR CNode2Str( LPCLASSDATA lpcd, LPCTSTR pszString )
{
	int		nLength = _tcslen( pszString ), i, a, nNewLength;
	LPTSTR		pszRc;

	/*
	 *	Compute the length necessary to
	 *	store the conversion.
	 */
	for ( i = 0, nNewLength = 0; i < nLength; i++ )
	{
		if ( pszString[ i ] == _T( '\\' ) && i < nLength - 1 && pszString[ i + 1 ] == _T( 'n' ))
		{
			i++;
			nNewLength += 2;
		}
		else
			nNewLength++;
	}

	/*
	 *	Allocate string.
	 */
	if (( pszRc = Brainchild_AllocPooled( lpcd->pMemPool, REAL_SIZE( nNewLength + 1 ))) != NULL )
	{
		/*
		 *	Copy the string and convert
		 *	it to the string form.
		 */
		for ( i = 0, a = 0; i < nLength; i++ )
		{
			if ( pszString[ i ] == _T( '\\' ) && i < nLength - 1 && pszString[ i + 1 ] == _T( 'n' ))
			{
				i++;
				pszRc[ a++ ] = _T( '\r' );
				pszRc[ a++ ] = _T( '\n' );
			}
			else	
				pszRc[ a++ ] = pszString[ i ];
		}
		/*
		 *	Terminate.
		 */
		pszRc[ a ] = 0;
	}
	return pszRc;
}

/*
 *	Execute command list.
 */
void ExecuteCList( LPCLIST lpCList, LPCLASSDATA lpcd )
{
	LPCNODE		lpCNode;

	/*
	 *	Iterate command list.
	 */
	for ( lpCNode = lpCList->lpFirst; lpCNode->lpNext; lpCNode = lpCNode->lpNext )
	{
		/*
		 *	What's the command type?
		 */
		switch ( lpCNode->nType )
		{
			case	CTYPE_HARDCODED:
				/*
				 *	Call the function.
				 */
				( lpCNode->lpFunc )( lpcd );
				break;

			case	CTYPE_TEXT:
			{
				LPCTSTR		pszConversion;

				/*
				 *	Convert the text.
				 */
				if (( pszConversion = CNode2Str( lpcd, lpCNode->pcStr )) != NULL )
				{
					/*
					 *	Insert the converted text.
					 */
					InsertCharacters( lpcd, pszConversion );

					/*
					 *	Free conversion.
					 */
					Brainchild_FreePooled( lpcd->pMemPool, ( LPVOID )pszConversion );
				}
				break;
			}

			case	CTYPE_RUN:
			{
				STARTUPINFO		si = { 0 };
				PROCESS_INFORMATION	pi = { 0 };

				si.cb	       = sizeof( si );
				si.dwFlags     = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_SHOWDEFAULT;

				CreateProcess( NULL,
					       lpCNode->pcStr,
					       NULL,
					       NULL,
					       FALSE,
					       CREATE_DEFAULT_ERROR_MODE | NORMAL_PRIORITY_CLASS,
					       NULL,
					       NULL,
					       &si,
					       &pi );
			}
			break;

			case	CTYPE_SHELLOPEN:
				/*
				 *	Open the file.
				 */
				if (( int )ShellExecute( NULL,
					      _T( "open" ),
					      lpCNode->pcStr,
					      NULL,
					      NULL,
					      SW_SHOWNORMAL ) <= 32 )
					OkIOError( lpcd, lpcd->hWnd, _T( "%s" ), lpCNode->pcStr );
				break;

		}
		/*
		 *	If the break flag is set we stop
		 *	executing because the list has probably
		 *	changed.
		 */
		if ( lpcd->bBreakExec )
		{
			lpcd->bBreakExec = FALSE;
			break;
		}
	}
}

/*
 *	Delete command list.
 */
static void FreeCList( LPCLIST lpCList, POOL pMemPool )
{
	LPCNODE		lpCNode;

	/*
	 *	No list? return...
	 */
	if ( lpCList == NULL )
		return;

	/*
	 *	Remove all nodes.
	 */
	while (( lpCNode = ( LPCNODE )RemHead(( LPLIST )lpCList )) != NULL )
	{
		/*
		 *	All types have a string except
		 *	CTYPE_HARDCODED.
		 */
		if ( lpCNode->nType != CTYPE_HARDCODED )
		{
			/*
			 *	Free the string.
			 */
			if ( lpCNode->pcStr )
				Brainchild_FreePooled( pMemPool, lpCNode->pcStr );
		}

		/*
		 *	Free the node.
		 */
		Brainchild_FreePooled( pMemPool, lpCNode );
	}

	/*
	 *	Free the list itself.
	 */
	Brainchild_FreePooled( pMemPool, lpCList );
}

/*
 *	Construct a command list.
 */
static LPCLIST MakeCList( POOL pMemPool, LPDATA lpData )
{
	LPCNODE		lpCNode;
	LPCLIST		lpCList;

	/*
	 *	Allocate list.
	 */
	if (( lpCList = Brainchild_AllocPooled( pMemPool, sizeof( CLIST ))) != NULL )
	{
		/*
		 *	Initialize it.
		 */
		NewList(( LPLIST )lpCList );

		/*
		 *	Create and add the nodes.
		 */
		while ( lpData->dwType != CTYPE_END )
		{
			/*
			 *	Allocate node.
			 */
			if (( lpCNode = Brainchild_AllocPooled( pMemPool, sizeof( CNODE ))) != NULL )
			{
				/*
				 *	Setup command type.
				 */
				lpCNode->nType = ( int )lpData->dwType;

				/*
				 *	What type is it?
				 */
				switch ( lpCNode->nType )
				{
					case	CTYPE_HARDCODED:
						/*
						 *	Store function pointer.
						 */
						lpCNode->lpFunc = Brainchild_FindCommand(( int )lpData->lpData );

						/*
						 *	OK?
						 */
						if ( lpCNode->lpFunc == NULL )
						{
							Brainchild_FreePooled( pMemPool, lpCNode );
							FreeCList( lpCList, pMemPool );
							return NULL;
						}
						break;

					default:
						/*
						 *	Other commands get a copy of
						 *	the supplied string.
						 */
						if (( lpCNode->pcStr = Brainchild_AllocPooled( pMemPool, REAL_SIZE( _tcslen(( LPCTSTR )lpData->lpData ) + 1 ))) != NULL )
							_tcscpy( lpCNode->pcStr, ( LPCTSTR )lpData->lpData );
						else
						{
							Brainchild_FreePooled( pMemPool, lpCNode );
							FreeCList( lpCList, pMemPool );
							return NULL;
						}
						break;
				}
				/*
				 *	Add the node.
				 */
				AddTail(( LPLIST )lpCList, ( LPNODE )lpCNode );
			}
			else
			{
				FreeCList( lpCList, pMemPool );
				return NULL;
			}
			/*
			 *	Next...
			 */
			lpData++;
		}
	}
	return lpCList;
}

LRESULT OnExecuteCList( HWND hWnd, WPARAM wParam, LPARAM lParam, LPCLASSDATA lpcd )
{
	/*
	 *	Execute...
	 */
	ExecuteCList(( LPCLIST )lParam, lpcd );
	return 0;
}

/*
 *	Construct a command list (exported...)
 */
LPCOMLIST Brainchild_CreateCList( DWORD dwType, ... )
{
	return ( LPCOMLIST )MakeCList( pMainPool, ( LPDATA )&dwType );
}

/*
 *	Construct a command list.
 */
LPCLIST BuildCList( POOL pMemPool, DWORD dwType, ... )
{
	return MakeCList( pMemPool, ( LPDATA )&dwType );
}

/*
 *	Delete a command list (exported)
 */
void Brainchild_DeleteCList( LPCOMLIST lpComList )
{
	FreeCList(( LPCLIST )lpComList, pMainPool );
}

/*
 *	Delete a command list.
 */
void Brainchild_KillCList( LPCLIST lpComList, POOL pMemPool )
{
	FreeCList( lpComList, pMemPool );
}

/*
 *	Copy a command list.
 */
LPCLIST CopyCList( LPCLIST lpCList, POOL pMemPool )
{
	LPCLIST		lpCClones;
	LPCNODE		lpCNode, lpCCopy;

	/*
	 *	Allocate a command list.
	 */
	if (( lpCClones = Brainchild_AllocPooled( pMemPool, sizeof( CLIST ))) != NULL )
	{
		/*
		 *	Initialize list.
		 */
		NewList(( LPLIST )lpCClones );

		/*
		 *	Iterate the list.
		 */
		for ( lpCNode = lpCList->lpFirst; lpCNode->lpNext; lpCNode = lpCNode->lpNext )
		{
			/*
			 *	Allocate node.
			 */
			if (( lpCCopy = Brainchild_AllocPooled( pMemPool, sizeof( CNODE ))) != NULL )
			{
				/*
				 *	Copy data.
				 */
				lpCCopy->nType = lpCNode->nType;

				if ( lpCNode->nType == CTYPE_HARDCODED )
					lpCCopy->lpFunc = lpCNode->lpFunc;
				else
				{
					if (( lpCCopy->pcStr = Brainchild_AllocPooled( pMemPool, REAL_SIZE( _tcslen( lpCNode->pcStr ) + 1 ))) != NULL )
						_tcscpy( lpCCopy->pcStr, lpCNode->pcStr );
					else
					{
						Brainchild_FreePooled( pMemPool, lpCCopy );
						Brainchild_KillCList( lpCClones, pMemPool );
						return NULL;
					}
				}

				/*
				 *	Add it to the list.
				 */
				AddTail(( LPLIST )lpCClones, ( LPNODE )lpCCopy );
			}
			else
			{
				Brainchild_KillCList( lpCClones, pMemPool );
				return NULL;
			}
		}
	}
	return lpCClones;
}

/*
 *	Return the next command node
 *	entry data.
 */
DWORD Brainchild_NextCEntry( LPCOMLIST lpComList, LPDWORD lpdwAnchor, LPDWORD lpdwCType, LPDWORD lpdwCData )
{
	LPCNODE		lpCNode = ( LPCNODE )*lpdwAnchor;

	/*
	 *	Any input? If not get the
	 *	first node. Otherwise get the
	 *	next node.
	 */
	if ( lpCNode == NULL ) lpCNode = (( LPCLIST )lpComList )->lpFirst;
	else		       lpCNode = lpCNode->lpNext;

	/*
	 *	Any data in this node?
	 */
	if ( lpCNode->lpNext )
	{
		/*
		 *	Store data.
		 */
		*lpdwCType = ( DWORD )lpCNode->nType;

		/*
		 *	String or command?
		 */
		if (      lpCNode->nType == CTYPE_HARDCODED ) *lpdwCData = ( DWORD )Brainchild_FindCommandID( lpCNode->lpFunc );
		else					      *lpdwCData = ( DWORD )lpCNode->pcStr;

		/*
		 *	Return anchor.
		 */
		return ( DWORD )lpCNode;
	}
	return 0L;
}
