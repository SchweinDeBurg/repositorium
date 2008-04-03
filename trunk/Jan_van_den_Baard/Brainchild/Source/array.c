/*
 *	array.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	Dynamic arrays.
 */
 
#include "stdafx.h"

#include "defs.h"

/*
 *	Setup the array size.
 */
BOOL ArraySetSize( LPARRAY lpArray, int nSize, int nGrowBy )
{
	if ( nSize < 0 )
		return FALSE;

	/*
	 *	Do we have a new "grow-by" value?
	 */
	if ( nGrowBy != -1 )
		lpArray->nGrowBy = nGrowBy;

	/*
	 *	Destroy all elements?
	 */
	if ( nSize == 0 )
	{
		/*
		 *	Do we have an array?
		 */
		if ( lpArray->lpElements )
		{
			/*
			 *	Free the array.
			 */
			FreePooled( lpArray->mpPool, lpArray->lpElements );
			lpArray->lpElements = NULL;
		}

		/*
		 *	Reset values.
		 */
		lpArray->nSize = lpArray->nMax = 0;
	}
	else if ( nSize <= lpArray->nMax )
	{
		/*
		 *	Set new upper bound.
		 */
		//if ( nSize > lpArray->nSize )
			/*
			 *	Clear elements and adjust the array size.
			 */
		//	memset( &lpArray->lpElements[ lpArray->nSize * lpArray->nElemSize ], 0, ( nSize - lpArray->nSize ) * lpArray->nElemSize );

		/*
		 *	Set new upper bound.
		 */
		lpArray->nSize = nSize;
	}
	else
	{
		LPVOID	lpNewElem;
		int	nMaximum;

		/*
		 *	Grow the array. If no grow-by value is
		 *	present we use our own value.
		 */
		nGrowBy = lpArray->nGrowBy == 0 ? 1024 : lpArray->nGrowBy;

		/*
		 *	Will the requested size fit if we grow?
		 */
		if ( nSize > ( lpArray->nMax + nGrowBy ))
			/*
			 *	Make sure we grow enough.
			 */
			nMaximum = nSize;
		else
			/*
			 *	This is enough.
			 */
			nMaximum = lpArray->nMax + nGrowBy;

		/*
		 *	Allocate new elements.
		 */
		if (( lpNewElem = AllocPooled( lpArray->mpPool, nMaximum * lpArray->nElemSize )) == NULL )
			return FALSE;

		/*
		 *	Copy old contents and free it.
		 */
		if ( lpArray->lpElements )
		{
			memcpy(lpNewElem, lpArray->lpElements, lpArray->nSize * lpArray->nElemSize );
			FreePooled( lpArray->mpPool, lpArray->lpElements );
		}

		/*
		 *	Setup new array.
		 */
		lpArray->lpElements = lpNewElem;
		lpArray->nSize	    = nSize;
		lpArray->nMax	    = nMaximum;
	}
	return TRUE;
}

/*
 *	Sort the array contents.
 */
void ArraySort( LPARRAY lpArray, COMPFUNC lpfnCompare )
{
	/*
	 *	Sort the array.
	 */
	qsort( lpArray->lpElements, lpArray->nSize, lpArray->nElemSize, lpfnCompare );
}

/*
 *	Return the size of the array.
 */
int ArrayGetSize( LPARRAY lpArray )
{
	return lpArray->nSize;
}

/*
 *	Return a pointer to an array element.
 */
LPVOID ArrayGetAt( LPARRAY lpArray, int nIndex )
{
	/*
	 *	Valid index?
	 */
	if ( nIndex >= 0 && nIndex < lpArray->nSize )
		/*
		 *	Return a pointer to the array element.
		 */
		return ( LPVOID )( &lpArray->lpElements[ nIndex * lpArray->nElemSize ] );

	/*
	 *	Index out of range.
	 */
	return NULL;
}

/*
 *	Add a new element(s) to the array.
 */
BOOL ArrayAdd( LPARRAY lpArray, LPVOID lpElements, int nNum )
{
	int	nIndex = lpArray->nSize;

	/*
	 *	Grow the array if necessary.
	 */
	 if ( ArraySetSize( lpArray, nIndex + nNum, -1 ) == FALSE )
		 return FALSE;

	/*
	 *	Copy new elements.
	 */
	memcpy( &lpArray->lpElements[ nIndex * lpArray->nElemSize ], lpElements, lpArray->nElemSize * nNum );

	return TRUE;
}

/*
 *	Insert an element.
 */
BOOL ArrayInsertAt( LPARRAY lpArray, int nIndex, LPVOID lpElements, int nNum )
{
	int nSize;

	/*
	 *	Valid index?
	 */
	if ( nIndex < 0 )
		return FALSE;

	/*
	 *	Are we inserting at the end or
	 *	beyond the end of the array?
	 */
	if ( nIndex >= lpArray->nSize )
	{
		/*
		 *	Adding after the end of the array.
		 *	Grow the array.
		 */
		ArraySetSize( lpArray, nIndex + nNum, -1 );
	}
	else
	{		
		/*
		 *	Insert in the array.
		 */
		nSize = lpArray->nSize;

		/*
		 *	Grow the array.
		 */
		if ( ArraySetSize( lpArray, nSize + nNum, -1 ) == FALSE )
			return FALSE;

		/*
		 *	Move the data.
		 */
		memmove( &lpArray->lpElements[ ( nIndex + nNum )  * lpArray->nElemSize ], &lpArray->lpElements[ nIndex * lpArray->nElemSize ], ( nSize - nIndex ) * lpArray->nElemSize );
	}

	/*
	 *	Insert elements.
	 */
	memcpy( &lpArray->lpElements[ nIndex * lpArray->nElemSize ], lpElements, lpArray->nElemSize * nNum );
	return TRUE;
}

void ArrayRemoveAt( LPARRAY lpArray, int nIndex, int nNum )
{
	/*
	 *	Valid index?
	 */
	if ( nIndex >= 0 && nIndex + nNum <= lpArray->nSize )
	{
		/*
		 *	How many elements are to be
		 *	moved?
		 */
		int nMove = lpArray->nSize - ( nIndex + nNum );

		/*
		 *	Move any elements?
		 */
		if ( nMove )
			memmove( &lpArray->lpElements[ nIndex * lpArray->nElemSize ], &lpArray->lpElements[ ( nIndex + nNum ) * lpArray->nElemSize ], nMove * lpArray->nElemSize );

		/*
		 *	nNum less elements.
		 */
		lpArray->nSize -= nNum;
	}
}

/*
 *	Remove all array elements.
 */
void ArrayRemoveAll( LPARRAY lpArray )
{
	/*
	 *	Free array elements.
	 */
	ArraySetSize( lpArray, 0, -1 );
}

/*
 *	Remove all elements and
 *	free memory allocated in the
 *	pool.
 */
void ArrayReset( LPARRAY lpArray )
{
	/*
	 *	Free array elements.
	 */
	ArraySetSize( lpArray, 0, -1 );

	/*
	 *	Flush all allocated memory.
	 */
	FreeMemoryPool( lpArray->mpPool, FALSE );
}

/*
 *	Create an array handle.
 */
LPARRAY ArrayCreate( int nSize, int nGrowBy, int nElemSize )
{
	LPARRAY		lpArray;

	/*
	 *	Allocate the array structure.
	 */
	if (( lpArray = malloc( sizeof( ARRAY ))) != NULL )
	{
		/*
		 *	Create a memory pool.
		 */
		if (( lpArray->mpPool = GetMemoryPool( 4096L )) != NULL )
		{
			/*
			 *	Initialize the array.
			 */
			lpArray->nGrowBy    = nGrowBy;
			lpArray->nMax       = 0;
			lpArray->nSize      = 0;
			lpArray->nElemSize  = nElemSize;
			lpArray->lpElements = NULL;

			/*
			 *	Setup initial array.
			 */
			if ( ArraySetSize( lpArray, nSize, nGrowBy ))
				return lpArray;

			/*
			 *	Free the memory pool.
			 */
			FreeMemoryPool( lpArray->mpPool, TRUE );
		}
		/*
		 *	Free the structure.
		 */
		free( lpArray );
	}
	return NULL;
}

/*
 *	Delete the array.
 */
void ArrayDelete( LPARRAY lpArray )
{
	/*
	 *	Remove the array memory pool.
	 */
	FreeMemoryPool( lpArray->mpPool, TRUE );

	/*
	 *	Free the array structure.
	 */
	free( lpArray );
}

/*
 *	Allocate memory in the array memory pool.
 */
LPVOID ArrayAllocMem( LPARRAY lpArray, int nSize )
{
	return AllocPooled( lpArray->mpPool, nSize );
}

/*
 *	Free memory from the array memory pool.
 */
void ArrayFreeMem( LPARRAY lpArray, LPVOID lpMemPtr )
{
	FreePooled( lpArray->mpPool, lpMemPtr );
}
